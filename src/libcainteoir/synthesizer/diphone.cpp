/* Phoneme to diphone prosody conversion.
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

#include <cainteoir/prosody.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

struct diphone_reader : public tts::prosody_reader
{
	diphone_reader(const std::shared_ptr<tts::prosody_reader> &aProsody);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	std::shared_ptr<tts::prosody_reader> mProsody;
};

diphone_reader::diphone_reader(const std::shared_ptr<tts::prosody_reader> &aProsody)
	: mProsody(aProsody)
{
}

void diphone_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mProsody->reset(aBuffer);
}

bool diphone_reader::read()
{
	bool next = mProsody->read();
	if (next)
	{
		*((tts::prosody *)this) = *mProsody;
	}
	return next;
}

std::shared_ptr<tts::prosody_reader>
tts::createDiphoneReader(const std::shared_ptr<prosody_reader> &aProsody)
{
	return std::make_shared<diphone_reader>(aProsody);
}
