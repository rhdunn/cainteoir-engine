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

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

struct prosody_reader_t : public tts::prosody_reader
{
	prosody_reader_t(const std::shared_ptr<tts::text_reader> &aTextReader,
	                 const std::shared_ptr<tts::duration_model> &aDurationModel);

	bool read();
private:
	bool next_event();

	std::shared_ptr<tts::text_reader> mTextReader;
	std::shared_ptr<tts::duration_model> mDurationModel;

	std::list<ipa::phoneme>::const_iterator mCurrent;
	std::list<ipa::phoneme>::const_iterator mLast;
};

prosody_reader_t::prosody_reader_t(const std::shared_ptr<tts::text_reader> &aTextReader,
                                   const std::shared_ptr<tts::duration_model> &aDurationModel)
	: mTextReader(aTextReader)
	, mDurationModel(aDurationModel)
{
}

bool prosody_reader_t::read()
{
	if (mCurrent == mLast)
	{
		if (!next_event()) return false;
	}

	first.phoneme1 = *mCurrent++;
	if (mCurrent != mLast)
	{
		if (first.phoneme1.get(ipa::joined_to_next_phoneme) == ipa::joined_to_next_phoneme)
			first.phoneme2 = *mCurrent++;
		else if (mCurrent->get(ipa::syllabicity) == ipa::non_syllabic)
			first.phoneme2 = *mCurrent++;
		else
			first.phoneme2 = { ipa::unspecified };
	}
	else
		first.phoneme2 = { ipa::unspecified };

	if (first.phoneme1 == ipa::syllable_break)
		first.duration = {};
	else
		first.duration = mDurationModel->lookup(first).value(0);

	return true;
}

bool prosody_reader_t::next_event()
{
	while (mTextReader->read())
	{
		auto &event = mTextReader->event();
		mCurrent = event.phonemes.begin();
		mLast    = event.phonemes.end();
		if (mCurrent != mLast)
			return true;
	}

	return false;
}

std::shared_ptr<tts::prosody_reader>
tts::createProsodyReader(const std::shared_ptr<text_reader> &aTextReader,
                         const std::shared_ptr<duration_model> &aDurationModel)
{
	return std::make_shared<prosody_reader_t>(aTextReader, aDurationModel);
}
