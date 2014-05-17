/* MBROLA pho file.
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

std::shared_ptr<tts::prosody_reader>
tts::createPhoReader(const std::shared_ptr<phoneme_reader> &aPhonemeSet)
{
	return {};
}

std::shared_ptr<tts::prosody_writer>
tts::createPhoWriter(const std::shared_ptr<phoneme_writer> &aPhonemeSet)
{
	return {};
}
