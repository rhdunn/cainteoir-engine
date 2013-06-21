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

#include <ucd/ucd.h>
#include <cainteoir/document.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/unicode.hpp>
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

		if (mode == list_entries)
		{
			if (argc != 1)
				throw std::runtime_error("no document specified");

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
				auto ipa = tts::createPhonemeWriter("ipa");
				ipa->reset(stdout);
				for (auto &entry : dict)
				{
					if (entry.second.type == tts::dictionary::say_as)
					{
						ucd::codepoint_t cp = 0;
						cainteoir::utf8::read(entry.second.text->begin(), cp);

						fprintf(stdout, "\"%s\" => \"%s\"@%s [say-as]\n",
						        entry.first->str().c_str(),
						        entry.second.text->str().c_str(),
						        ucd::get_script_string(ucd::lookup_script(cp)));
					}
					else
					{
						fprintf(stdout, "\"%s\" => /",
						        entry.first->str().c_str());
						for (auto p : entry.second.phonemes)
							ipa->write(p);
						fprintf(stdout, "/ [ipa]\n");
					}
				}
			}
		}
		else if (mode == pronounce_entries)
		{
			if (argc != 2)
				throw std::runtime_error("usage: dictionary --pronounce <dictionary> <ruleset>");

			tts::dictionary dict;
			if (!dict.add_entries(cainteoir::path(argv[0])))
			{
				fprintf(stderr, "cannot load dictionary file \"%s\"\n", argv[0]);
				return 0;
			}

			auto rules = tts::createPronunciationRules(cainteoir::path(argv[1]));
			if (!rules.get())
			{
				fprintf(stderr, "cannot load letter-to-phoneme rule file \"%s\"\n", argv[1]);
				return 0;
			}

			cainteoir::stopwatch timer;
			for (auto &entry : dict)
			{
				fprintf(stdout, "\"%s\" => /%s/ [ipa]\n",
				        entry.first->str().c_str(),
				        rules->pronounce(entry.first)->str().c_str());
			}

			if (time)
				printf("time:    %G\n", timer.elapsed());
		}
		else if (mode == from_document)
		{
			if (argc == 0)
				throw std::runtime_error("no document specified");

			rdf::graph metadata;
			uint32_t words = 0;
			tts::dictionary dict;
			cainteoir::stopwatch timer;
			for (int i = 0; i < argc; ++i)
			{
				auto reader = cainteoir::createDocumentReader(argv[i], metadata, std::string());
				if (!reader)
					fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[i]);

				fprintf(stdout, "reading %s\n", argv[i]);

				tts::text_reader text(reader);
				while (text.read()) switch (text.event().type)
				{
				case tts::word_uppercase:
				case tts::word_lowercase:
				case tts::word_capitalized:
				case tts::word_mixedcase:
				case tts::word_script:
					dict.add_entry(text.event().text,
					               tts::dictionary::say_as,
					               text.event().text);
					++words;
					break;
				}
			}

			printf("words:   %d\n", words);
			printf("indexed: %d\n", dict.size());

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
