/* Phoneme to unit prosody conversion.
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

struct unit_reader : public tts::prosody_reader
{
	unit_reader(const std::shared_ptr<tts::prosody_reader> &aProsody,
	            const std::vector<tts::unit_t> &aUnits,
	            const cainteoir::range<const tts::phoneme_units *> &aPhonemes)
		: mProsody(aProsody)
		, mUnits(aUnits)
		, mPhonemes(aPhonemes)
		, mCurrentUnit(0)
		, mLastUnit(0)
		, mRemainingOffset(100)
	{
	}

	bool read();
private:
	bool next_phoneme();

	std::shared_ptr<tts::prosody_reader> mProsody;
	const std::vector<tts::unit_t> &mUnits;
	cainteoir::range<const tts::phoneme_units *> mPhonemes;

	uint16_t mCurrentUnit;
	uint16_t mLastUnit;
	uint8_t  mRemainingOffset;
};

bool unit_reader::read()
{
	if (mCurrentUnit == mLastUnit)
	{
		if (!next_phoneme())
			return false;
	}

	first.phoneme1 = ipa::unit | mCurrentUnit;
	++mCurrentUnit;

	uint8_t offset = (mCurrentUnit == mLastUnit) ? mRemainingOffset : mUnits[mCurrentUnit].phoneme_start;
	mRemainingOffset -= offset;

	first.duration = css::time((mProsody->first.duration.value() * offset) / 100.0,
	                            mProsody->first.duration.units());

	return true;
}

bool unit_reader::next_phoneme()
{
	if (!mProsody->read())
		return false;

	constexpr auto mask = ~ipa::stress;

	uint16_t index = 0;
	for (const auto &entry : mPhonemes)
	{
		if (mProsody->first.phoneme1.get(mask) == entry.phoneme1 &&
		    mProsody->first.phoneme2 == entry.phoneme2)
		{
			mCurrentUnit = entry.first_unit;
			mLastUnit = entry.first_unit + entry.num_units;
			envelope = mProsody->envelope;
			mRemainingOffset = 100;
			return true;
		}
		++index;
	}

	if (!(mProsody->first.phoneme1 == ipa::syllable_break && mProsody->first.phoneme2 == ipa::unspecified))
	{
		fprintf(stdout, "Phoneme /");
		tts::write_explicit_feature(stdout, mProsody->first.phoneme1);
		if (mProsody->first.phoneme2 != ipa::unspecified)
			tts::write_explicit_feature(stdout, mProsody->first.phoneme2);
		fprintf(stdout, "/ is not supported.\n");
	}

	return next_phoneme();
}

std::shared_ptr<tts::prosody_reader>
tts::create_unit_reader(const std::shared_ptr<prosody_reader> &aProsody,
                        const std::vector<tts::unit_t> &aUnits,
                        const cainteoir::range<const tts::phoneme_units *> &aPhonemes)
{
	return std::make_shared<unit_reader>(aProsody, aUnits, aPhonemes);
}
