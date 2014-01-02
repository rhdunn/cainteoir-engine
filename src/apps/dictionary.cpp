/* Test for the exception dictionary API.
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
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/document.hpp>
#include <cainteoir/engines.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/path.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

enum class stress_type
{
	as_transcribed,
	vowel,
	syllable,
};

enum class mode_type
{
	from_document,
	list_entries,
	resolve_say_as_entries,
	pronounce_entries,
	compare_entries,
	mismatched_entries,
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

static bool is_variant(const cainteoir::buffer &s)
{
	for (auto c : s)
	{
		if (c == '@') return true;
	}
	return false;
}

static bool pronounce(tts::dictionary &dict,
                      const std::shared_ptr<cainteoir::buffer> &word,
                      std::shared_ptr<tts::phoneme_reader> &rules,
                      std::shared_ptr<tts::dictionary_formatter> &formatter,
                      std::shared_ptr<tts::phoneme_writer> &writer,
                      const char *phonemeset,
                      stress_type stress,
                      mode_type mode)
{
	std::list<tts::phoneme> phonemes;
	if (mode != mode_type::pronounce_entries)
	{
		phonemes = dict.pronounce(word);
		if (phonemes.empty())
			return false;
	}

	std::list<tts::phoneme> pronounced;
	rules->reset(word);
	while (rules->read())
		pronounced.push_back(*rules);

	switch (stress)
	{
	case stress_type::vowel:
		tts::make_vowel_stressed(pronounced);
		break;
	case stress_type::syllable:
		tts::make_syllable_stressed(pronounced);
		break;
	}

	bool match = matches(pronounced, phonemes);
	if (mode == mode_type::mismatched_entries && match)
		return true;

	if (mode == mode_type::compare_entries)
	{
		formatter->write_phoneme_entry(word, writer, phonemes, " ... ");
		if (match)
		{
			fprintf(stdout, "matched\n");
		}
		else
		{
			fprintf(stdout, "mismatched; got /");
			for (auto p : pronounced)
				writer->write(p);
			fprintf(stdout, "/\n");
		}
	}
	else if (mode == mode_type::mismatched_entries)
		formatter->write_phoneme_entry(word, writer, phonemes);
	else
		formatter->write_phoneme_entry(word, writer, pronounced);

	return match;
}

static void pronounce(tts::dictionary &dict,
                      std::shared_ptr<tts::phoneme_reader> rules,
                      std::shared_ptr<tts::dictionary_formatter> &formatter,
                      std::shared_ptr<tts::phoneme_writer> writer,
                      const char *phonemeset,
                      stress_type stress,
                      mode_type mode)
{
	writer->reset(stdout);

	int matched = 0;
	int entries = 0;

	for (auto &entry : dict)
	{
		if (is_variant(*entry.first)) continue;

		if (pronounce(dict, entry.first,
		              rules, formatter, writer, phonemeset, stress, mode))
			++matched;
		++entries;
	}

	fflush(stdout);

	if (mode == mode_type::compare_entries)
	{
		fprintf(stderr, "... matched: %d (%.0f%%)\n", matched, (float(matched) / entries * 100.0f));
		fprintf(stderr, "... entries: %d\n", entries);
	}
}

static uint32_t from_document(const tts::dictionary &base_dict,
                              tts::dictionary &dict,
                              const char *filename,
                              bool silent)
{
	if (!silent)
		fprintf(stdout, "reading %s\n", (filename == nullptr) ? "<stdin>" : filename);

	rdf::graph metadata;
	auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
	if (!reader)
	{
		fprintf(stderr, "unsupported document format for file \"%s\"\n",
		        (filename == nullptr) ? "<stdin>" : filename);
		return 0;
	}

	std::shared_ptr<tts::phoneme_reader> phonemeset;
	uint32_t words = 0;
	tts::text_reader text(reader);
	while (text.read()) switch (text.event().type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		if (base_dict.lookup(text.event().text).type == tts::dictionary::no_match)
		{
			dict.add_entry(text.event().text, { text.event().text });
			++words;
		}
		break;
	}

	return words;
}

int main(int argc, char ** argv)
{
	try
	{
		stress_type stress = stress_type::as_transcribed;
		mode_type mode = mode_type::from_document;
		bool time = false;
		bool new_words = false;
		const char *voicename = nullptr;
		const char *language = nullptr;
		const char *ruleset = nullptr;
		const char *dictionary = nullptr;
		const char *dictionary_format = nullptr;
		const char *phonemeset = "ipa";

		const option_group general_options = { nullptr, {
			{ 'L', "list", bind_value(mode, mode_type::list_entries),
			  i18n("List the entries in the dictionary") },
			{ 'R', "resolve-say-as", bind_value(mode, mode_type::resolve_say_as_entries),
			  i18n("Look up say-as entries in the dictionary (implies --list)") },
			{ 'D', "as-dictionary", dictionary_format, "FORMAT",
			  i18n("List the entries in the given dictionary format") },
			{ 't', "time", bind_value(time, true),
			  i18n("Time how long it takes to complete the action") },
			{ 'd', "dictionary", dictionary, "DICTIONARY",
			  i18n("Use the words in DICTIONARY") },
			{ 'n', "new-words", bind_value(new_words, true),
			  i18n("Only use words not in the loaded dictionary") },
			{ 'P', "phonemeset", phonemeset, "PHONEMESET",
			  i18n("Use PHONEMESET to transcribe phoneme entries (default: ipa)") },
		}};

		const option_group stress_options = { i18n("Stress:"), {
			{ 0, "vowel-stress", bind_value(stress, stress_type::vowel),
			  i18n("Place the stress on vowels (e.g. espeak, arpabet)") },
			{ 0, "syllable-stress", bind_value(stress, stress_type::syllable),
			  i18n("Place the stress on syllable boundaries") },
		}};

		const option_group pronunciation_options = { i18n("Pronunciation:"), {
			{ 'p', "pronounce", bind_value(mode, mode_type::pronounce_entries),
			  i18n("Pronounce the dictionary items using the ruleset/engine") },
			{ 'c', "compare", bind_value(mode, mode_type::compare_entries),
			  i18n("Compare dictionary and ruleset/engine pronunciations") },
			{ 'm', "mismatched", bind_value(mode, mode_type::mismatched_entries),
			  i18n("Only display mismatched pronunciations (implies --compare)") },
			//{ 'r', "ruleset", ruleset, "RULESET",
			//  i18n("Use the RULESET pronunciation rule file") },
			{ 'v', "voice", voicename, "VOICE",
			  i18n("Use the TTS voice named VOICE") },
			{ 'l', "language", language, "LANG",
			  i18n("Use a TTS voice that speaks the language LANG") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("dictionary [OPTION..] DOCUMENT.."),
		};

		if (!parse_command_line({ general_options, stress_options, pronunciation_options }, usage, argc, argv))
			return 0;

		auto writer = tts::createPhonemeWriter(phonemeset);
		cainteoir::stopwatch timer;

		tts::dictionary base_dict;
		tts::dictionary dict;

		uint32_t words = 0;
		if (dictionary != nullptr)
		{
			auto reader = tts::createCainteoirDictionaryReader(dictionary);
			while (reader->read())
				base_dict.add_entry(reader->word, reader->entry);
			if (!new_words)
			{
				dict  = base_dict;
				words = dict.size();
			}
		}

		if (dictionary == nullptr || new_words)
		{
			if (argc == 0)
				words += from_document(base_dict, dict, nullptr, dictionary_format != nullptr);
			else for (int i = 0; i != argc; ++i)
				words += from_document(base_dict, dict, argv[i], dictionary_format != nullptr);
		}

		std::shared_ptr<tts::dictionary_formatter> formatter;
		if (!dictionary_format)
			formatter = tts::createDictionaryEntryFormatter(stdout);
		else if (!strcmp(dictionary_format, "cainteoir"))
			formatter = tts::createCainteoirDictionaryFormatter(stdout);
		else if (!strcmp(dictionary_format, "espeak"))
			formatter = tts::createEspeakDictionaryFormatter(stdout);
		else
		{
			fprintf(stderr, "unsupported dictionary format \"%s\"\n", dictionary_format);
			return 0;
		}

		if (mode == mode_type::from_document && dictionary_format != nullptr)
			mode = mode_type::list_entries;

		switch (mode)
		{
		case mode_type::list_entries:
		case mode_type::resolve_say_as_entries:
			writer->reset(stdout);
			tts::formatDictionary(dict, formatter, writer, mode == mode_type::resolve_say_as_entries);
			break;
		case mode_type::pronounce_entries:
		case mode_type::compare_entries:
		case mode_type::mismatched_entries:
			if (ruleset != nullptr)
			{
				auto rules = tts::createPronunciationRules(ruleset);
				if (!rules.get())
				{
					fprintf(stderr, "cannot load letter-to-phoneme rule file \"%s\"\n", argv[1]);
					return 0;
				}
				pronounce(dict, rules, formatter, writer, phonemeset, stress, mode);
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
				pronounce(dict, engine.pronunciation(), formatter, writer, phonemeset, stress, mode);
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
