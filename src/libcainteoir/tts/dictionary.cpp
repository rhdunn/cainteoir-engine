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
#include <cainteoir/unicode.hpp>
#include <cainteoir/path.hpp>

namespace tts = cainteoir::tts;

bool tts::dictionary::add_entries(const path &aDictionaryPath)
{
	try
	{
		add_entries(aDictionaryPath.parent(), make_file_buffer(aDictionaryPath));
	}
	catch (const std::exception &)
	{
		return false;
	}
	return true;
}

void tts::dictionary::add_entry(const std::string &aEntry,
                                entry_type aType,
                                ucd::script aScript,
                                const std::shared_ptr<buffer> &aDefinition)
{
	mEntries[aEntry] = { aType, aScript, aDefinition };
}

void tts::dictionary::add_entries(const path &aBasePath,
                                  const std::shared_ptr<buffer> &aDictionary)
{
	const char *current = aDictionary->begin();
	const char *last    = aDictionary->end();
	while (current != last)
	{
		const char *begin_entry = current;
		const char *end_entry   = current;
		while (end_entry != last && *end_entry != '\t')
			++end_entry;

		current = end_entry;
		while (current != last && *current == '\t')
			++current;

		if (current == last) return;

		const char *begin_definition = current;
		const char *end_definition   = current;
		while (end_definition != last && *end_definition != '\r' && *end_definition != '\n')
			++end_definition;

		current = end_definition;
		while (current != last && (*current == '\r' || *current == '\n'))
			++current;

		std::string entry(begin_entry, end_entry);
		if (*begin_entry == '.')
		{
			if (entry == ".import")
			{
				std::string definition(begin_definition, end_definition);
				if (!add_entries(aBasePath / definition))
					fprintf(stderr, "error: unable to load dictionary \"%s\"\n", definition.c_str());
			}
		}
		else if (*begin_definition == '/' && *(end_definition - 1) == '/')
		{
			++begin_definition;
			--end_definition;
			auto definition = cainteoir::make_buffer(begin_definition, end_definition - begin_definition);
			add_entry(entry, phonemes, ucd::Zzzz, definition);
		}
		else
		{
			ucd::codepoint_t cp = 0;
			cainteoir::utf8::read(begin_definition, cp);

			auto definition = cainteoir::make_buffer(begin_definition, end_definition - begin_definition);
			add_entry(entry, say_as, ucd::lookup_script(cp), definition);
		}
	}
}

const tts::dictionary::entry &tts::dictionary::lookup(const std::string &aEntry) const
{
	static const entry no_match = { say_as, ucd::Zzzz, {} };
	const auto &match = mEntries.find(aEntry);
	return (match == mEntries.end()) ? no_match : match->second;
}
