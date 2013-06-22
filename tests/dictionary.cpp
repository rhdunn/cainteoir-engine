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
#include <cainteoir/engines.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/unicode.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>
#include <getopt.h>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

enum class mode_type
{
	from_document,
	list_entries,
	pronounce_entries,
};

static struct option options[] =
{
	{ "compare",   no_argument, 0, 'c' },
	{ "list",      no_argument, 0, 'l' },
	{ "pronounce", no_argument, 0, 'p' },
	{ "time",      no_argument, 0, 't' },
	{ 0, 0, 0, 0 }
};

static bool matches(const std::list<tts::phoneme> &a, const std::list<tts::phoneme> &b)
{
	auto first1 = a.begin(), last1 = a.end();
	auto first2 = b.begin(), last2 = b.end();
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 != *first2) return false;
		++first1;
		++first2;
	}
	return first1 == last1 && first2 == last2;
}

static void list_entries(const char *dictionary, bool time)
{
	cainteoir::stopwatch timer;
	tts::dictionary dict;
	if (!dict.add_entries(cainteoir::path(dictionary)))
	{
		fprintf(stderr, "cannot load dictionary file \"%s\"\n", dictionary);
		return;
	}

	if (time)
	{
		printf("time:    %G\n", timer.elapsed());
		return;
	}

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

static void pronounce(const char *dictionary,
                      std::shared_ptr<tts::phoneme_reader> rules,
                      bool time,
                      bool compare)
{
	tts::dictionary dict;
	if (!dict.add_entries(cainteoir::path(dictionary)))
	{
		fprintf(stderr, "cannot load dictionary file \"%s\"\n", dictionary);
		return;
	}

	auto ipa = tts::createPhonemeWriter("ipa");
	ipa->reset(stdout);

	int matched = 0;
	int entries = 0;

	cainteoir::stopwatch timer;
	for (auto &entry : dict)
	{
		std::list<tts::phoneme> pronounced;
		rules->reset(entry.first);
		while (rules->read())
			pronounced.push_back(*rules);

		fprintf(stdout, "\"%s\" => /",
		        entry.first->str().c_str());
		if (compare)
		{
			for (auto p : entry.second.phonemes)
				ipa->write(p);
			fprintf(stdout, "/ ... ");
			if (matches(pronounced, entry.second.phonemes))
			{
				fprintf(stdout, "matched\n");
				++matched;
			}
			else
			{
				fprintf(stdout, "mismatched; got /");
				for (auto p : pronounced)
					ipa->write(p);
				fprintf(stdout, "/\n");
			}
		}
		else
		{
			for (auto p : pronounced)
				ipa->write(p);
			fprintf(stdout, "/ [ipa]\n");
		}
		++entries;
	}

	if (compare)
	{
		fprintf(stderr, "... matched: %d (%.0f%%)\n", matched, (float(matched) / entries * 100.0f));
		fprintf(stderr, "... entries: %d\n", entries);
	}
	if (time)
		fprintf(stderr, "... time:    %G\n", timer.elapsed());
}

static void from_documents(int argc, char **argv, bool time)
{
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

	fprintf(stderr, "... words:   %d\n", words);
	fprintf(stderr, "... indexed: %d\n", dict.size());

	if (time)
		fprintf(stderr, "... time:    %G\n", timer.elapsed());
}

int main(int argc, char ** argv)
{
	try
	{
		mode_type mode = mode_type::from_document;
		bool time = false;
		bool compare = false;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "clpt", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'c':
				mode = mode_type::pronounce_entries;
				compare = true;
				break;
			case 'l':
				mode = mode_type::list_entries;
				break;
			case 'p':
				mode = mode_type::pronounce_entries;
				break;
			case 't':
				time = true;
				break;
			}
		}

		argc -= optind;
		argv += optind;

		switch (mode)
		{
		case mode_type::list_entries:
			if (argc != 1)
				throw std::runtime_error("no document specified");
			list_entries(argv[0], time);
			break;
		case mode_type::pronounce_entries:
			if (argc == 2)
			{
				auto rules = tts::createPronunciationRules(cainteoir::path(argv[1]));
				if (!rules.get())
				{
					fprintf(stderr, "cannot load letter-to-phoneme rule file \"%s\"\n", argv[1]);
					return 0;
				}
				pronounce(argv[0], rules, time, compare);
			}
			else if (argc == 1)
			{
				rdf::graph metadata;
				tts::engines engine(metadata);
				pronounce(argv[0], engine.pronounciation(), time, compare);
			}
			else
				throw std::runtime_error("no document specified");
			break;
		case mode_type::from_document:
			if (argc == 0)
				throw std::runtime_error("no document specified");
			from_documents(argc, argv, time);
			break;
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
