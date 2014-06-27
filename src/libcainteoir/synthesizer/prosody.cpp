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

struct prosody_reader_t : public tts::prosody_reader
{
	prosody_reader_t(const std::shared_ptr<tts::text_reader> &aTextReader);

	bool read();
private:
	std::shared_ptr<tts::text_reader> mTextReader;
};

prosody_reader_t::prosody_reader_t(const std::shared_ptr<tts::text_reader> &aTextReader)
	: mTextReader(aTextReader)
{
}

bool prosody_reader_t::read()
{
	return false;
}

std::shared_ptr<tts::prosody_reader>
tts::createProsodyReader(const std::shared_ptr<text_reader> &aTextReader)
{
	return std::make_shared<prosody_reader_t>(aTextReader);
}
