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

namespace tts = cainteoir::tts;

std::size_t tts::dictionary::key_hash::operator()(const key_type &a) const
{
	// DJB2 Hash Algorithm by Dan Bernstein:
	std::size_t hash = 5381;
	for (auto c : *a)
		hash = hash * 33 + c;
        return hash;
}

void tts::dictionary::add_entry(const key_type &aEntry,
                                entry_type aType,
                                std::shared_ptr<phoneme_reader> &phonemeset,
                                const std::shared_ptr<buffer> &aDefinition)
{
	auto &entry = mEntries[aEntry];
	entry.type = aType;
	if (aType == phonemes)
	{
		entry.phonemes.clear();
		phonemeset->reset(aDefinition);
		while (phonemeset->read())
			entry.phonemes.push_back(*phonemeset);
	}
	else
		entry.text = aDefinition;
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
