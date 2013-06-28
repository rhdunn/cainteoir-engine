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
#include "i18n.h"
#include "options.hpp"

#include <ucd/ucd.h>
#include <cainteoir/document.hpp>
#include <cainteoir/engines.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/unicode.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

enum class mode_type
{
	from_document,
	list_entries,
	pronounce_entries,
	compare_entries,
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

static void list_entries(const tts::dictionary &dict, bool time)
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

static void pronounce(const tts::dictionary &dict,
                      std::shared_ptr<tts::phoneme_reader> rules,
                      bool time,
                      bool compare)
{
	auto ipa = tts::createPhonemeWriter("ipa");
	ipa->reset(stdout);

	int matched = 0;
	int entries = 0;

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

	fflush(stdout);

	if (compare)
	{
		fprintf(stderr, "... matched: %d (%.0f%%)\n", matched, (float(matched) / entries * 100.0f));
		fprintf(stderr, "... entries: %d\n", entries);
	}
}

static uint32_t from_document(tts::dictionary &dict, const char *filename)
{
	fprintf(stdout, "reading %s\n", (filename == nullptr) ? "<stdin>" : filename);

	rdf::graph metadata;
	auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
	if (!reader)
	{
		fprintf(stderr, "unsupported document format for file \"%s\"\n",
		        (filename == nullptr) ? "<stdin>" : filename);
		return 0;
	}

	uint32_t words = 0;
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

	return words;
}

int main(int argc, char ** argv)
{
	try
	{
		mode_type mode = mode_type::from_document;
		bool time = false;
		const char *voicename = nullptr;
		const char *language = nullptr;
		const char *ruleset = nullptr;
		const char *dictionary = nullptr;

		const option_group general_options = { nullptr, {
			{ 'L', "list", no_argument, nullptr,
			  i18n("List the entries in the dictionary"),
			  [&mode](const char *) { mode = mode_type::list_entries; }},
			{ 't', "time", no_argument, nullptr,
			  i18n("Time how long it takes to complete the action"),
			  [&time](const char *) { time = true; }},
			{ 'd', "dictionary", required_argument, "DICTIONARY",
			  i18n("Use the words in DICTIONARY"),
			  [&dictionary](const char *arg) { dictionary = arg; }},
		}};

		const option_group pronunciation_options = { i18n("Pronunciation:"), {
			{ 'p', "pronounce", no_argument, nullptr,
			  i18n("Pronounce the dictionary items using the ruleset/engine"),
			  [&mode](const char *) { mode = mode_type::pronounce_entries; }},
			{ 'c', "compare", no_argument, nullptr,
			  i18n("Compare dictionary and ruleset/engine pronunciations"),
			  [&mode](const char *) { mode = mode_type::compare_entries; }},
			{ 'r', "ruleset", required_argument, "RULESET",
			  i18n("Use the RULESET pronunciation rule file"),
			  [&ruleset](const char *arg) { ruleset = arg; }},
			{ 'v', "voice", required_argument, "VOICE",
			  i18n("Use the TTS voice named VOICE"),
			  [&voicename](const char *arg) { voicename = arg; }},
			{ 'l', "language", required_argument, "LANG",
			  i18n("Use a TTS voice that speaks the language LANG"),
			  [&language](const char *arg) { language = arg; }},
		}};

		const std::initializer_list<const char *> usage = {
			i18n("dictionary [OPTION..] DOCUMENT.."),
		};

		if (!parse_command_line({ general_options, pronunciation_options }, usage, argc, argv))
			return 0;

		cainteoir::stopwatch timer;

		tts::dictionary dict;
		uint32_t words = 0;
		if (dictionary != nullptr)
		{
			if (!dict.add_entries(cainteoir::path(dictionary)))
			{
				fprintf(stderr, "cannot load dictionary \"%s\"\n", dictionary);
				return 0;
			}
			words = dict.size();
		}
		else if (argc == 0)
			words += from_document(dict, nullptr);
		else for (int i = 0; i != argc; ++i)
			words += from_document(dict, argv[i]);

		switch (mode)
		{
		case mode_type::list_entries:
			list_entries(dict, time);
			break;
		case mode_type::pronounce_entries:
		case mode_type::compare_entries:
			if (ruleset != nullptr)
			{
				auto rules = tts::createPronunciationRules(ruleset);
				if (!rules.get())
				{
					fprintf(stderr, "cannot load letter-to-phoneme rule file \"%s\"\n", argv[1]);
					return 0;
				}
				pronounce(dict, rules, time, mode == mode_type::compare_entries);
			}
			else
			{
				rdf::graph metadata;
				tts::engines engine(metadata);
				if (voicename)
				{
					const rdf::uri *ref = tts::get_voice_uri(metadata, rdf::tts("name"), voicename);
					if (ref)
						engine.select_voice(metadata, *ref);
				}
				else if (language)
				{
					const rdf::uri *ref = tts::get_voice_uri(metadata, rdf::dc("language"), language);
					if (ref)
						engine.select_voice(metadata, *ref);
				}
				pronounce(dict, engine.pronunciation(), time, mode == mode_type::compare_entries);
			}
			break;
		case mode_type::from_document:
			fflush(stdout);
			fprintf(stderr, "... words:   %d\n", words);
			fprintf(stderr, "... indexed: %d\n", dict.size());
			break;
		}

		if (time)
			fprintf(stderr, "... time:    %G\n", timer.elapsed());
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
