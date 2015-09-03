/* Test for applying langdb lexical rewrite rules.
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
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/language.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdio>

void
phonemes(const char *ruleset,
         const cainteoir::language::tag &locale,
         const char *phonemeset,
         std::istream &is)
{
	auto rewriter = cainteoir::tts::createPronunciationRules(ruleset, locale);
	if (!rewriter)
		throw std::runtime_error("unable to load the specified ruleset");

	auto out = cainteoir::tts::createPhonemeWriter(phonemeset);
	out->reset(stdout);

	std::string line;
	while (std::getline(is, line))
	{
		rewriter->reset(cainteoir::make_buffer(line.c_str(), line.size()));
		while (rewriter->read())
			out->write(*rewriter);
		out->flush();
		fprintf(stdout, "\n");
	}
}

int main(int argc, char ** argv)
{
	try
	{
		enum class mode_t
		{
			phonemes,
			text,
		};

		const char *ruleset = nullptr;
		const char *phonemeset = "ipa";
		const char *locale_name = "en";
		mode_t mode = mode_t::text;

		const option_group processing_options = { i18n("Processing Options:"), {
			{ 'r', "ruleset", ruleset, "RULESET",
			  i18n("Use the RULESET pronunciation rule file") },
			{ 0, "phonemes", bind_value(mode, mode_t::phonemes),
			  i18n("Use letter-to-phoneme rules") },
			{ 'l', "locale", locale_name, "LOCALE",
			  i18n("Use LOCALE with the ruleset") },
			{ 'p', "phonemeset", phonemeset, "PHONEMESET",
			  i18n("Use the PHONEMESET phonemeset to output phonemes in") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("rewrite [OPTION..] DOCUMENT"),
			i18n("rewrite [OPTION..]"),
		};

		const std::initializer_list<option_group> options = {
			processing_options,
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		auto locale = cainteoir::language::make_lang(locale_name);

		if (mode == mode_t::text)
		{
			std::shared_ptr<cainteoir::buffer> text;
			if (argc == 1)
				text = cainteoir::make_file_buffer(argv[0]);
			else
				text = cainteoir::make_file_buffer(stdin);

			auto rewriter = cainteoir::tts::createLexicalRewriteRules(ruleset);
			if (!rewriter)
				throw std::runtime_error("unable to load the specified ruleset");

			auto out = rewriter->rewrite(text);
			fwrite(out->begin(), 1, out->size(), stdout);
		}
		else
		{
			if (argc == 1)
			{
				std::ifstream f(argv[0]);
				phonemes(ruleset, locale, phonemeset, f);
			}
			else
				phonemes(ruleset, locale, phonemeset, std::cin);
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
