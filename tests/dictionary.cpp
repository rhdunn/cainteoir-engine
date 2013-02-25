/* Test for the exception dictionary API.
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

#include <cainteoir/document.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>
#include <getopt.h>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

enum mode_type
{
	from_document,
	list_entries,
	pronounce_entries,
};

static struct option options[] =
{
	{ "list",      no_argument, 0, 'l' },
	{ "pronounce", no_argument, 0, 'p' },
	{ "time",      no_argument, 0, 't' },
	{ 0, 0, 0, 0 }
};

struct test_results
{
	uint32_t    words;
	std::size_t index_size;
};

test_results parse_words(const std::shared_ptr<cainteoir::document_reader> &reader)
{
	uint32_t words = 0;
	tts::dictionary dict;
	tts::text_reader text(reader);
	while (text.read()) switch (text.event().type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		dict.add_entry(text.event().text->str(), tts::dictionary::say_as, ucd::Zzzz, text.event().text);
		++words;
		break;
	}
	return { words, dict.size() };
}

int main(int argc, char ** argv)
{
	try
	{
		mode_type mode = from_document;
		bool      time = false;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "lpt", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'l':
				mode = list_entries;
				break;
			case 'p':
				mode = pronounce_entries;
				break;
			case 't':
				time = true;
				break;
			}
		}

		argc -= optind;
		argv += optind;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		if (mode == list_entries)
		{
			cainteoir::stopwatch timer;
			tts::dictionary dict;
			if (!dict.add_entries(cainteoir::path(argv[0])))
			{
				fprintf(stderr, "cannot load dictionary file \"%s\"\n", argv[0]);
				return 0;
			}

			if (time)
				printf("time:    %G\n", timer.elapsed());
			else
			{
				for (auto &entry : dict)
				{
					if (entry.second.type == tts::dictionary::say_as)
					{
						fprintf(stdout, "\"%s\" => \"%s\"@%s [say-as]\n",
						        entry.first.c_str(),
						        entry.second.text->str().c_str(),
						        ucd::get_script_string(entry.second.script));
					}
					else
					{
						fprintf(stdout, "\"%s\" => /%s/ [ipa]\n",
						        entry.first.c_str(),
						        entry.second.text->str().c_str());
					}
				}
			}
		}
		else if (mode == pronounce_entries)
		{
			tts::dictionary dict;
			if (!dict.add_entries(cainteoir::path(argv[0])))
			{
				fprintf(stderr, "cannot load dictionary file \"%s\"\n", argv[0]);
				return 0;
			}

			tts::ruleset rules = tts::en_rules();

			cainteoir::stopwatch timer;
			for (auto &entry : dict)
			{
				auto s = cainteoir::make_buffer(entry.first.c_str(), entry.first.size());
				fprintf(stdout, "\"%s\" => /%s/ [ipa]\n",
				        entry.first.c_str(),
				        rules.pronounce(s)->str().c_str());
			}

			if (time)
				printf("time:    %G\n", timer.elapsed());
		}
		else if (mode == from_document)
		{
			rdf::graph metadata;
			auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
			if (!reader)
			{
				fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
				return 0;
			}

			cainteoir::stopwatch timer;
			test_results results = parse_words(reader);
			printf("words:   %d\n", results.words);
			printf("indexed: %d\n", results.index_size);

			if (time)
				printf("time:    %G\n", timer.elapsed());
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
