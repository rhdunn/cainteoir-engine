/* CMU Pronunciation Dictionary Format.
 *
 * Copyright (C) 2015 Reece H. Dunn
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

struct cmudict_formatter : public tts::dictionary_formatter
{
	cmudict_formatter(FILE *aOut)
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

void cmudict_formatter::write_phoneme_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                           std::shared_ptr<tts::phoneme_writer> &writer,
                                           const ipa::phonemes &phonemes,
                                           const char *line_separator)
{
	bool in_variant = false;
	for (auto c : *word)
	{
		if (in_variant)
			fputc(c, mOut);
		else if (c == '@')
		{
			in_variant = true;
			fputc('(', mOut);
		}
		else
			fputc(toupper(c), mOut);
	}
	if (in_variant)
		fputc(')', mOut);
	fputc(' ', mOut);
	fputc(' ', mOut);
	for (auto p : phonemes)
		writer->write(p);
	writer->flush();
	fputc('\n', mOut);
}

void cmudict_formatter::write_say_as_entry(const std::shared_ptr<cainteoir::buffer> &word,
                                          const std::shared_ptr<cainteoir::buffer> &say_as,
                                          const char *line_separator)
{
	throw std::runtime_error("cmudict does not support say-as entries");
}

std::shared_ptr<tts::dictionary_formatter> tts::createCMUDictionaryFormatter(FILE *out)
{
	return std::make_shared<cmudict_formatter>(out);
}
