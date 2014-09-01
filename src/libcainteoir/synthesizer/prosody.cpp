/* Phoneme to prosody conversion.
 *
 * Copyright (C) 2014 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "i18n.h"
#include "compatibility.hpp"

#include <cainteoir/synthesizer.hpp>
#include "../phoneme/phonemeset.hpp"

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

struct phonemes_to_prosody : public tts::prosody_reader
{
	phonemes_to_prosody(const std::shared_ptr<tts::phoneme_reader> &aPhonemes,
	                    const std::shared_ptr<tts::duration_model> &aDurationModel,
	                    const std::shared_ptr<tts::pitch_model> &aPitchModel,
	                    tts::probability_distribution aProbabilityDistribution)
		: mPhonemes(aPhonemes)
		, mDurationModel(aDurationModel)
		, mPitchModel(aPitchModel)
		, mProbabilityDistribution(aProbabilityDistribution)
		, mNeedPhoneme(true)
	{
	}

	bool read();
private:
	std::shared_ptr<tts::phoneme_reader> mPhonemes;
	std::shared_ptr<tts::duration_model> mDurationModel;
	std::shared_ptr<tts::pitch_model> mPitchModel;
	tts::probability_distribution mProbabilityDistribution;
	bool mNeedPhoneme;
};

bool phonemes_to_prosody::read()
{
	if (mNeedPhoneme)
	{
		if (!mPhonemes->read())
			return false;
	}

	first.phoneme1 = *mPhonemes;
	if (mPhonemes->read())
	{
		if (first.phoneme1.get(ipa::joined_to_next_phoneme) == ipa::joined_to_next_phoneme)
		{
			first.phoneme2 = *mPhonemes;
			mNeedPhoneme = true;
		}
		else if (mPhonemes->get(ipa::syllabicity) == ipa::non_syllabic)
		{
			first.phoneme2 = *mPhonemes;
			mNeedPhoneme = true;
		}
		else
		{
			first.phoneme2 = { ipa::unspecified };
			mNeedPhoneme = false;
		}
	}
	else
	{
		first.phoneme2 = { ipa::unspecified };
		mNeedPhoneme = true;
	}

	if (first.phoneme1 == ipa::syllable_break)
	{
		first.duration = {};
		return true;
	}

	if (first.phoneme2 == ipa::unspecified)
	{
		if (first.phoneme1.get(ipa::syllabicity) == ipa::non_syllabic)
		{
			// The /aI_^@_^/ (FIRE) phoneme gets split into /aI_^/ and /@_^/, while
			// the /aU_^@_^/ (HOUR) phoneme gets split into /aU_^/ and /@_^/.
			// Convert /@_^/ to /@/, allowing the phoneme to be processed.
			first.phoneme1.clear(ipa::syllabicity);
		}
		else if (first.phoneme1.get(ipa::phoneme_type) == ipa::separator)
			return read();
	}

	first.duration = mDurationModel->lookup(first, mProbabilityDistribution);
	if (first.duration.units() == css::time::inherit)
	{
		fprintf(stdout, "Phoneme /");
		tts::write_explicit_feature(stdout, first.phoneme1);
		if (first.phoneme2 != ipa::unspecified)
			tts::write_explicit_feature(stdout, first.phoneme2);
		fprintf(stdout, "/ is not supported by the duration model.\n");
		return read();
	}

	if (mPitchModel.get())
		envelope = mPitchModel->envelope(first.phoneme1, mProbabilityDistribution);
	return true;
}

std::shared_ptr<tts::prosody_reader>
tts::createProsodyReader(const std::shared_ptr<phoneme_reader> &aPhonemes,
                         const std::shared_ptr<duration_model> &aDurationModel,
                         const std::shared_ptr<tts::pitch_model> &aPitchModel,
                         tts::probability_distribution aProbabilityDistribution)
{
	return std::make_shared<phonemes_to_prosody>(aPhonemes, aDurationModel, aPitchModel, aProbabilityDistribution);
}
