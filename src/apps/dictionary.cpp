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
namespace ipa = cainteoir::ipa;

enum class mode_type
{
	from_document,
	list_entries,
	resolve_say_as_entries,
	pronounce_entries,
	compare_entries,
	mismatched_entries,
};

enum class word_mode_type
{
	merge,
	only_in_document, // new words
	in_dictionary_and_document,
};

struct dictionary_phoneme_reader : public tts::phoneme_reader
{
	dictionary_phoneme_reader(const char *aDictionary);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	tts::dictionary mDictionary;
	ipa::phonemes mPhonemes;
	ipa::phonemes::const_iterator mCurrent;
	ipa::phonemes::const_iterator mLast;
};

dictionary_phoneme_reader::dictionary_phoneme_reader(const char *aDictionary)
{
	auto reader = tts::createCainteoirDictionaryReader(aDictionary);
	while (reader->read())
		mDictionary.add_entry(reader->word, reader->entry);
}

void dictionary_phoneme_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mPhonemes.clear();
	if (!mDictionary.pronounce(aBuffer, {}, mPhonemes))
		throw tts::phoneme_error("the word is not in the dictionary");

	mCurrent = mPhonemes.begin();
	mLast = mPhonemes.end();
}

bool dictionary_phoneme_reader::read()
{
	if (mCurrent == mLast)
		return false;
	*(ipa::phoneme *)this = *mCurrent++;
	return true;
}

static bool matches(const ipa::phonemes &a, const ipa::phonemes &b, bool ignore_syllable_breaks)
{
	auto first1 = a.begin(), last1 = a.end();
	auto first2 = b.begin(), last2 = b.end();
	while (first1 != last1 && first2 != last2)
	{
		if (ignore_syllable_breaks)
		{
			if (*first1 == ipa::syllable_break) { ++first1; continue; }
			if (*first2 == ipa::syllable_break) { ++first2; continue; }
		}

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
                      tts::stress_type stress,
                      bool ignore_syllable_breaks,
                      mode_type mode)
{
	ipa::phonemes phonemes;
	if (mode != mode_type::pronounce_entries)
	{
		if (!dict.pronounce(word, {}, phonemes))
		{
			auto &entry = dict.lookup(word);
			switch (entry.type)
			{
			case tts::dictionary::say_as:
				fprintf(stderr, "error: cannot resolve '%s' from '%s'.\n",
				        entry.text->str().c_str(),
				        word->str().c_str());
				break;
			case tts::dictionary::no_match:
				fprintf(stderr, "error: no entry for '%s'.\n", word->str().c_str());
				break;
			}
			return false;
		}
	}

	ipa::phonemes pronounced;
	try
	{
		rules->reset(word);
		while (rules->read())
			pronounced.push_back(*rules);
	}
	catch (const tts::phoneme_error &e)
	{
		fprintf(stderr, "cannot pronounce '%s': %s\n", word->str().c_str(), e.what());
		return false;
	}

	tts::make_stressed(pronounced, stress);

	bool match = matches(pronounced, phonemes, ignore_syllable_breaks);
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
			writer->flush();
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
                      tts::stress_type stress,
                      bool ignore_syllable_breaks,
                      mode_type mode)
{
	writer->reset(stdout);

	int matched = 0;
	int entries = 0;

	for (auto &entry : dict)
	{
		if (is_variant(*entry.first)) continue;

		if (pronounce(dict, entry.first,
		              rules, formatter, writer, phonemeset, stress, ignore_syllable_breaks, mode))
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

static uint32_t from_document(tts::dictionary &base_dict,
                              tts::dictionary &dict,
                              const char *filename,
                              bool silent,
                              word_mode_type word_mode)
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
	auto text = tts::create_text_reader(reader);
	while (text->read()) switch (text->event().type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		{
			auto &match = base_dict.lookup(text->event().text);
			switch (match.type)
			{
			case tts::dictionary::no_match:
				if (word_mode == word_mode_type::only_in_document)
				{
					dict.add_entry(text->event().text, { text->event().text });
					++words;
				}
				else
				{
					fprintf(stderr, "error: cannot find '%s' in the dictionary\n", text->event().text->str().c_str());
				}
				break;
			case tts::dictionary::say_as:
				if (word_mode == word_mode_type::in_dictionary_and_document)
				{
					ipa::phonemes pronunciation;
					if (base_dict.pronounce(text->event().text, {}, pronunciation))
					{
						dict.add_entry(text->event().text, { pronunciation });
						++words;
					}
				}
				break;
			case tts::dictionary::phonemes:
				if (word_mode == word_mode_type::in_dictionary_and_document)
				{
					dict.add_entry(text->event().text, match );
					++words;
				}
				break;
			}
		}
		break;
	}

	return words;
}

int main(int argc, char ** argv)
{
	try
	{
		tts::stress_type stress = tts::stress_type::as_transcribed;
		mode_type mode = mode_type::from_document;
		bool time = false;
		word_mode_type word_mode = word_mode_type::merge;
		bool ignore_syllable_breaks = false;
		const char *voicename = nullptr;
		const char *language = nullptr;
		const char *ruleset = nullptr;
		const char *dictionary = nullptr;
		const char *dictionary_format = nullptr;
		const char *phonemeset = "ipa";
		const char *source_dictionary = nullptr;

		const option_group general_options = { nullptr, {
			{ 't', "time", bind_value(time, true),
			  i18n("Time how long it takes to complete the action") },
			{ 'd', "dictionary", dictionary, "DICTIONARY",
			  i18n("Use the words in DICTIONARY") },
		}};

		const option_group output_options = { i18n("Output:"), {
			{ 'D', "as-dictionary", dictionary_format, "FORMAT",
			  i18n("List the entries in the given dictionary format") },
			{ 'P', "phonemeset", phonemeset, "PHONEMESET",
			  i18n("Use PHONEMESET to transcribe phoneme entries (default: ipa)") },
			{ 'f', "filter-words", bind_value(word_mode, word_mode_type::in_dictionary_and_document),
			  i18n("Only use dictionary entries that are in the input documents") },
			{ 'n', "new-words", bind_value(word_mode, word_mode_type::only_in_document),
			  i18n("Only use words not in the loaded dictionary") },
		}};

		const option_group stress_options = { i18n("Stress:"), {
			{ 0, "vowel-stress", bind_value(stress, tts::stress_type::vowel),
			  i18n("Place the stress on vowels (e.g. espeak, arpabet)") },
			{ 0, "syllable-stress", bind_value(stress, tts::stress_type::syllable),
			  i18n("Place the stress on syllable boundaries") },
			{ 0, "ignore-syllable-breaks", bind_value(ignore_syllable_breaks, true),
			  i18n("Ignore syllable breaks (/./) when comparing pronunciations.") },
		}};

		const option_group action_options = { i18n("Action:"), {
			{ 'L', "list", bind_value(mode, mode_type::list_entries),
			  i18n("List the entries in the dictionary") },
			{ 'R', "resolve-say-as", bind_value(mode, mode_type::resolve_say_as_entries),
			  i18n("Look up say-as entries in the dictionary (implies --list)") },
			{ 'p', "pronounce", bind_value(mode, mode_type::pronounce_entries),
			  i18n("Use the Pronunciation options to pronounce the entries") },
			{ 'c', "compare", bind_value(mode, mode_type::compare_entries),
			  i18n("Compare dictionary and Pronunciation options pronunciation") },
			{ 'm', "mismatched", bind_value(mode, mode_type::mismatched_entries),
			  i18n("Only display mismatched pronunciations (implies --compare)") },
		}};

		const option_group pronunciation_options = { i18n("Pronunciation:"), {
			{ 's', "source-dictionary", source_dictionary, "DICTIONARY",
			  i18n("Use the pronunciation of the words in DICTIONARY") },
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

		if (!parse_command_line({ general_options, output_options, stress_options, action_options, pronunciation_options }, usage, argc, argv))
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
			if (word_mode == word_mode_type::merge)
			{
				dict  = base_dict;
				words = dict.size();
			}
		}

		if (dictionary == nullptr || word_mode != word_mode_type::merge)
		{
			if (argc == 0)
				words += from_document(base_dict, dict, nullptr, dictionary_format != nullptr, word_mode);
			else for (int i = 0; i != argc; ++i)
				words += from_document(base_dict, dict, argv[i], dictionary_format != nullptr, word_mode);
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
			if (source_dictionary != nullptr)
			{
				auto rules = std::make_shared<dictionary_phoneme_reader>(source_dictionary);
				pronounce(dict, rules, formatter, writer, phonemeset, stress, ignore_syllable_breaks, mode);
			}
			else if (ruleset != nullptr)
			{
				auto rules = tts::createPronunciationRules(ruleset);
				if (!rules.get())
				{
					fprintf(stderr, "cannot load letter-to-phoneme rule file \"%s\"\n", argv[1]);
					return 0;
				}
				pronounce(dict, rules, formatter, writer, phonemeset, stress, ignore_syllable_breaks, mode);
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
				pronounce(dict, engine.pronunciation(), formatter, writer, phonemeset, stress, ignore_syllable_breaks, mode);
			}
			break;
		case mode_type::from_document:
			fflush(stdout);
			fprintf(stderr, "... words:   %d\n", words);
			fprintf(stderr, "... indexed: %zd\n", dict.size());
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
