/* Generate a tag cloud from a document.
 *
 * Copyright (C) 2011-2013 Reece H. Dunn
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/text.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <math.h>

#include <map>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

static const char * common_words[] = {
	"a", "an", "and", "as", "at", "about", "all", "after",
	"but", "be", "been", "by", "back",
	"could",
	"did", "down", "do", "done",
	"even",
	"for", "from",
	"get", "got",
	"he", "her", "had", "his", "him", "have", "here",
	"i", "is", "it", "in", "if", "into", "its",
	"just",
	"like",
	"me", "more", "my", "mine", "man",
	"no", "not", "now",
	"of", "on", "or", "out", "one", "over", "off", "only",
	"she", "so", "said",
	"the", "to", "that", "than", "then", "they", "this", "there", "them", "their",
	"up",
	"was", "with", "were", "would", "what", "which", "when", "we", "who", "woman",
	"you",
	nullptr,
};

bool common(const std::string & word)
{
	for (const char ** words = common_words; *words; ++words)
	{
		if (word == *words)
			return true;
	}
	return false;
}

int main(int argc, char ** argv)
{
	try
	{
		enum
		{
			html_format,
			text_format,
		} format = html_format;

		bool show_all = false;

		const option_group general_options = { nullptr, {
			{ 'a', "all", no_argument, nullptr,
			  i18n("Show all words"),
			  [&show_all](const char *) { show_all = true; }},
		}};

		const option_group format_options = { i18n("Format:"), {
			{ 0, "text", no_argument, nullptr,
			  i18n("Output a text word list"),
			  [&format](const char *) { format = text_format; }},
			{ 0, "html", no_argument, nullptr,
			  i18n("Output a HTML tag cloud"),
			  [&format](const char *) { format = html_format; }},
		}};

		const std::initializer_list<const char *> usage = {
			i18n("tagcloud [OPTION..] DOCUMENT"),
			i18n("tagcloud [OPTION..]"),
		};

		if (!parse_command_line({ general_options, format_options }, usage, argc, argv))
			return 0;

		const char *filename = (argc == 1) ? argv[0] : nullptr;
		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, i18n("unsupported document format for file \"%s\"\n"), filename ? filename : "<stdin>");
			return EXIT_FAILURE;
		}

		std::map<std::string, int> words;
		tts::text_reader text(reader);
		while (text.read()) switch (text.event().type)
		{
		case tts::word_uppercase:
		case tts::word_lowercase:
		case tts::word_mixedcase:
		case tts::word_capitalized:
			++words[text.event().text->str()];
			break;
		}

		if (format == html_format)
		{
			printf("<html><body><p>\n");
			for (auto &word : words)
			{
				if (show_all || (word.second > 3 && !common(word.first)))
				{
					int size = (log(word.second) * 30) + 80;
					printf("<span style=\"font-size: %d%%;\">%s</span>\n", size, word.first.c_str());
				}
			}
			printf("</p></body></html>\n");
		}
		else
		{
			for (auto &word : words)
			{
				if (show_all || (word.second > 3 && !common(word.first)))
					printf("%8d %s\n", word.second, word.first.c_str());
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
