/* Cainteoir Pronunciation Dictionary Format.
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
#include <cainteoir/path.hpp>

namespace tts = cainteoir::tts;

static void parseCainteoirDictionary(tts::dictionary &dict,
                                     const cainteoir::path &aBasePath,
                                     const std::shared_ptr<cainteoir::buffer> &aDictionary)
{
	std::shared_ptr<tts::phoneme_reader> phonemeset;
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

		auto entry = cainteoir::make_buffer(begin_entry, end_entry - begin_entry);
		if (*begin_entry == '.')
		{
			if (entry->compare(".import") == 0)
			{
				std::string definition(begin_definition, end_definition);
				if (!tts::parseCainteoirDictionary(dict, aBasePath / definition))
					fprintf(stderr, "error: unable to load dictionary \"%s\"\n", definition.c_str());
			}
			else if (entry->compare(".phonemeset") == 0)
			{
				std::string phonemes(begin_definition, end_definition);
				phonemeset = tts::createPhonemeReader(phonemes.c_str());
			}
		}
		else if (*begin_definition == '/')
		{
			++begin_definition;
			while (end_definition != begin_definition && *end_definition != '/')
				--end_definition;

			if (begin_definition == end_definition) continue;
			if (!phonemeset.get())
				throw std::runtime_error("The dictionary does not specify a phonemeset.");

			auto definition = cainteoir::make_buffer(begin_definition, end_definition - begin_definition);
			dict.add_entry(entry, tts::dictionary::phonemes, phonemeset, definition);
		}
		else
		{
			auto definition = cainteoir::make_buffer(begin_definition, end_definition - begin_definition);
			dict.add_entry(entry, tts::dictionary::say_as, phonemeset, definition);
		}
	}
}

bool tts::parseCainteoirDictionary(tts::dictionary &dict,
                                   const char *aDictionaryPath)
{
	try
	{
		::parseCainteoirDictionary(dict, path(aDictionaryPath).parent(), make_file_buffer(aDictionaryPath));
	}
	catch (const std::exception &e)
	{
		return false;
	}
	return true;
}

struct cainteoir_formatter : public tts::dictionary_formatter
{
	cainteoir_formatter(FILE *aOut)
		: mOut(aOut)
	{
	}

	void write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                         std::shared_ptr<tts::phoneme_writer> &writer,
	                         const std::list<tts::phoneme> &phonemes,
	                         const char *line_separator);

	void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                        const std::shared_ptr<cainteoir::buffer> &say_as,
	                        const char *line_separator);

	FILE *mOut;
};

void cainteoir_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                              std::shared_ptr<tts::phoneme_writer> &writer,
                                              const std::list<tts::phoneme> &phonemes,
                                              const char *line_separator)
{
	if (fprintf(mOut, "%s", word->str().c_str()) < 8)
		fprintf(mOut, "\t");
	fprintf(mOut, "\t/");
	for (auto p : phonemes)
		writer->write(p);
	fprintf(mOut, "/%s", line_separator);
}

void cainteoir_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                             const std::shared_ptr<cainteoir::buffer> &say_as,
                                             const char *line_separator)
{
	if (fprintf(mOut, "%s", word->str().c_str()) < 8)
		fprintf(mOut, "\t");
	fprintf(mOut, "\t%s%s", say_as->str().c_str(), line_separator);
}

std::shared_ptr<tts::dictionary_formatter> tts::createCainteoirDictionaryFormatter(FILE *out)
{
	return std::make_shared<cainteoir_formatter>(out);
}
