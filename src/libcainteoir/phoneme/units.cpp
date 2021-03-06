/* Unit-Based Phoneme Reader/Writer.
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

#include <cainteoir/phoneme.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

struct unit_writer : public tts::phoneme_writer
{
	unit_writer(const std::vector<tts::unit_t> &aUnits, const char *aPause)
		: mUnits(aUnits)
		, mPause(aPause)
	{
	}

	void reset(FILE *aOutput);

	bool write(const ipa::phoneme &aPhoneme);

	void flush();

	const char *name() const { return "units"; }
private:
	FILE *mOutput;
	const std::vector<tts::unit_t> &mUnits;
	const char *mPause;
};

void unit_writer::reset(FILE *aOutput)
{
	mOutput = aOutput;
}

bool unit_writer::write(const ipa::phoneme &aPhoneme)
{
	switch (aPhoneme.get(ipa::phoneme_type))
	{
	case ipa::unit:
		break;
	case ipa::foot_break:
	case ipa::intonation_break:
		fputs(mPause, mOutput);
		return true;
	default:
		return false;
	}

	uint16_t index = aPhoneme.get(ipa::unit_value) >> 8;
	if (index >= mUnits.size())
		return false;

	fputs(mUnits[index].name, mOutput);
	return true;
}

void unit_writer::flush()
{
}

std::shared_ptr<tts::phoneme_writer>
tts::create_unit_writer(const std::vector<tts::unit_t> &aUnits, const char *aPause)
{
	return std::make_shared<unit_writer>(aUnits, aPause);
}
