/* eSpeak Pronunciation Dictionary Format.
 *
 * Copyright (C) 2013-2015 Reece H. Dunn
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
#include "dictionary_format.hpp"

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

static bool is_multiword_entry(const cainteoir::buffer &s)
{
	for (auto c : s)
	{
		if (c == ' ') return true;
	}
	return false;
}

struct espeak_formatter : public tts::dictionary_formatter
{
	espeak_formatter(FILE *aOut)
		: mOut(aOut)
	{
	}

	void write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                         std::shared_ptr<tts::phoneme_writer> &writer,
	                         const ipa::phonemes &phonemes,
	                         const char *line_separator);

	void write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
	                        const std::shared_ptr<cainteoir::buffer> &say_as,
	                        const char *line_separator);

	FILE *mOut;
};

void espeak_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                           std::shared_ptr<tts::phoneme_writer> &writer,
                                           const ipa::phonemes &phonemes,
                                           const char *line_separator)
{
	if (fprintf(mOut, is_multiword_entry(*word) ? "(%s)" : "%s", word->str().c_str()) < 8)
		fprintf(mOut, "\t");
	fprintf(mOut, "\t");
	for (auto p : phonemes)
		writer->write(p);
	writer->flush();
	fprintf(mOut, "\n");
}

void espeak_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                          const std::shared_ptr<cainteoir::buffer> &say_as,
                                          const char *line_separator)
{
	if (fprintf(mOut, is_multiword_entry(*word) ? "(%s)" : "%s", word->str().c_str()) < 8)
		fprintf(mOut, "\t");
	fprintf(mOut, "\t%s\n", say_as->str().c_str());
}

std::shared_ptr<tts::dictionary_formatter> tts::createEspeakDictionaryFormatter(FILE *out)
{
	return std::make_shared<espeak_formatter>(out);
}
