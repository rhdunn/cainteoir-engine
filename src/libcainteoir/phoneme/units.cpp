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
	unit_writer(const std::vector<tts::unit_t> &aUnits)
		: mUnits(aUnits)
	{
	}

	void reset(FILE *aOutput);

	bool write(const ipa::phoneme &aPhoneme);

	void flush();

	const char *name() const { return "units"; }
private:
	FILE *mOutput;
	const std::vector<tts::unit_t> &mUnits;
};

void unit_writer::reset(FILE *aOutput)
{
	mOutput = aOutput;
}

bool unit_writer::write(const ipa::phoneme &aPhoneme)
{
	if (aPhoneme.get(ipa::unit) == 0)
		return false;

	uint16_t index = aPhoneme.get(ipa::unit_value);
	if (index >= mUnits.size())
		return false;

	fputs(mUnits[index].name, mOutput);
	return false;
}

void unit_writer::flush()
{
}

std::shared_ptr<tts::phoneme_writer>
tts::create_unit_writer(const std::vector<tts::unit_t> &aUnits)
{
	return std::make_shared<unit_writer>(aUnits);
}
