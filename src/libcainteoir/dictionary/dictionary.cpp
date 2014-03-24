/* Pronunciation Dictionary.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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

#include <ucd/ucd.h>
#include <cainteoir/dictionary.hpp>
#include <cainteoir/unicode.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

tts::dictionary::entry::entry(const std::shared_ptr<buffer> &aPhonemes, std::shared_ptr<phoneme_reader> &aPhonemeSet)
	: type(tts::dictionary::phonemes)
{
	aPhonemeSet->reset(aPhonemes);
	while (aPhonemeSet->read())
		phonemes.push_back(*aPhonemeSet);
}

std::size_t tts::dictionary::key_hash::operator()(const key_type &a) const
{
	// DJB2 Hash Algorithm by Dan Bernstein:
	std::size_t hash = 5381;
	for (auto c : *a)
		hash = hash * 33 + c;
        return hash;
}

void tts::dictionary::add_entry(const key_type &aWord, const entry &aEntry)
{
	mEntries[aWord] = aEntry;
}

const tts::dictionary::entry &tts::dictionary::lookup(const key_type &aWord) const
{
	static const entry no_match = {};
	const auto &match = mEntries.find(aWord);
	return (match == mEntries.end()) ? no_match : match->second;
}

const std::list<ipa::phoneme> &tts::dictionary::pronounce(const std::shared_ptr<buffer> &aWord, int depth)
{
	static const std::list<ipa::phoneme> empty = {};

	const auto &entry = lookup(aWord);
	switch (entry.type)
	{
	case dictionary::phonemes:
		return entry.phonemes;
	case dictionary::say_as:
		if (depth == 5)
		{
			fprintf(stderr, "error: too much recursion for entry '%s'.\n", aWord->str().c_str());
			return empty;
		}
		return pronounce(entry.text, depth + 1);
	}

	return empty;
}

struct dictionary_entry_formatter : public tts::dictionary_formatter
{
	dictionary_entry_formatter(FILE *aOut)
		: mOut(aOut)
	{
	}

	void write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                         std::shared_ptr<tts::phoneme_writer> &writer,
	                         const std::list<ipa::phoneme> &phonemes,
	                         const char *line_separator);

	void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                        const std::shared_ptr<cainteoir::buffer> &say_as,
	                        const char *line_separator);

	FILE *mOut;
};

void dictionary_entry_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                                     std::shared_ptr<tts::phoneme_writer> &writer,
                                                     const std::list<ipa::phoneme> &phonemes,
                                                     const char *line_separator)
{
	fprintf(stdout, "\"%s\" => /", word->str().c_str());
	for (auto p : phonemes)
		writer->write(p);
	writer->flush();
	fprintf(stdout, "/ [%s]%s", writer->name(), line_separator);
}

void dictionary_entry_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                                    const std::shared_ptr<cainteoir::buffer> &say_as,
                                                    const char *line_separator)
{
	ucd::codepoint_t cp = 0;
	cainteoir::utf8::read(say_as->begin(), cp);

	fprintf(stdout, "\"%s\" => \"%s\"@%s [say-as]%s",
	        word->str().c_str(),
	        say_as->str().c_str(),
	        ucd::get_script_string(ucd::lookup_script(cp)),
	        line_separator);
}

std::shared_ptr<tts::dictionary_formatter> tts::createDictionaryEntryFormatter(FILE *out)
{
	return std::make_shared<dictionary_entry_formatter>(out);
}

void tts::formatDictionary(tts::dictionary &dict,
	                   std::shared_ptr<dictionary_formatter> &formatter,
	                   std::shared_ptr<phoneme_writer> &writer,
	                   bool resolve_say_as_entries)
{
	for (auto &entry : dict)
	{
		if (entry.second.type == tts::dictionary::phonemes)
			formatter->write_phoneme_entry(entry.first, writer, entry.second.phonemes);
		else if (resolve_say_as_entries)
			formatter->write_phoneme_entry(entry.first, writer, dict.pronounce(entry.first));
		else
			formatter->write_say_as_entry(entry.first, entry.second.text);
	}
}
