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

#include <cainteoir/dictionary.hpp>
#include <cainteoir/unicode.hpp>

namespace tts = cainteoir::tts;

std::size_t tts::dictionary::key_hash::operator()(const key_type &a) const
{
	// DJB2 Hash Algorithm by Dan Bernstein:
	std::size_t hash = 5381;
	for (auto c : *a)
		hash = hash * 33 + c;
        return hash;
}

bool tts::dictionary::add_entries(const char *aDictionaryPath)
{
	try
	{
		add_entries(path(aDictionaryPath).parent(), make_file_buffer(aDictionaryPath));
	}
	catch (const std::exception &e)
	{
		return false;
	}
	return true;
}

void tts::dictionary::add_entry(const key_type &aEntry,
                                entry_type aType,
                                const std::shared_ptr<buffer> &aDefinition)
{
	auto &entry = mEntries[aEntry];
	entry.type = aType;
	if (aType == phonemes)
	{
		if (!mPhonemes.get())
			throw std::runtime_error("The dictionary does not specify a phonemeset.");
		entry.phonemes.clear();
		mPhonemes->reset(aDefinition);
		while (mPhonemes->read())
			entry.phonemes.push_back(*mPhonemes);
	}
	else
		entry.text = aDefinition;
}

void tts::dictionary::add_entries(const path &aBasePath,
                                  const std::shared_ptr<buffer> &aDictionary)
{
	const char *current = aDictionary->begin();
	const char *last    = aDictionary->end();
	while (current != last)
	{
		if (*current == '#')
		{
			while (current != last && (*current != '\r' && *current != '\n'))
				++current;
			while (current != last && (*current == '\r' || *current == '\n'))
				++current;
			continue;
		}

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

		auto entry = make_buffer(begin_entry, end_entry - begin_entry);
		if (*begin_entry == '.')
		{
			if (entry->compare(".import") == 0)
			{
				std::string definition(begin_definition, end_definition);
				if (!add_entries(aBasePath / definition))
					fprintf(stderr, "error: unable to load dictionary \"%s\"\n", definition.c_str());
			}
			else if (entry->compare(".phonemeset") == 0)
			{
				std::string phonemeset(begin_definition, end_definition);
				mPhonemes = tts::createPhonemeReader(phonemeset.c_str());
			}
		}
		else if (*begin_definition == '/')
		{
			++begin_definition;
			while (end_definition != begin_definition && *end_definition != '/')
				--end_definition;

			if (begin_definition == end_definition) continue;

			auto definition = cainteoir::make_buffer(begin_definition, end_definition - begin_definition);
			add_entry(entry, phonemes, definition);
		}
		else
		{
			auto definition = cainteoir::make_buffer(begin_definition, end_definition - begin_definition);
			add_entry(entry, say_as, definition);
		}
	}
}

const tts::dictionary::entry &tts::dictionary::lookup(const key_type &aEntry) const
{
	static const entry no_match = { tts::dictionary::no_match, {} };
	const auto &match = mEntries.find(aEntry);
	return (match == mEntries.end()) ? no_match : match->second;
}

const std::list<tts::phoneme> &tts::dictionary::pronounce(const std::shared_ptr<buffer> &aText, int depth)
{
	static const std::list<tts::phoneme> empty = {};

	const auto &entry = lookup(aText);
	switch (entry.type)
	{
	case dictionary::phonemes:
		return entry.phonemes;
	case dictionary::say_as:
		if (depth == 5)
		{
			fprintf(stderr, "error: too much recursion for entry '%s'.\n", aText->str().c_str());
			return empty;
		}
		return pronounce(entry.text, depth + 1);
	}

	return empty;
}
