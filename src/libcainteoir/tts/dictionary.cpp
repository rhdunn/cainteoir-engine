/* Pronunciation Dictionary.
 *
 * Copyright (C) 2013 Reece H. Dunn
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
#include "compatibility.hpp"

#include <cainteoir/text.hpp>

namespace tts = cainteoir::tts;

tts::dictionary::dictionary()
	: mScript(ucd::Zzzz)
{
}

tts::dictionary::dictionary(ucd::script aScript, const std::initializer_list<value_type> &aEntries)
	: std::unordered_map<std::string, std::shared_ptr<buffer>>(aEntries)
	, mScript(aScript)
{
}

const std::shared_ptr<cainteoir::buffer> &tts::dictionary::lookup(const std::string &aEntry) const
{
	static const std::shared_ptr<cainteoir::buffer> no_match;
	const auto &match = find(aEntry);
	return (match == end()) ? no_match : match->second;
}
