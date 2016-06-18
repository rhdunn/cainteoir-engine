/* An application for working with pronunciation dictionaries.
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
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/document.hpp>
#include <cainteoir/engines.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/language.hpp>
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

static bool
add_dictionary_entries(const char *aDictionaryPath,
                       tts::dictionary &aDictionary)
{
	auto reader = tts::createDictionaryReader(aDictionaryPath);
	if (!reader)
	{
		fprintf(stderr, "unsupported dictionary format for: %s\n", aDictionaryPath);
		return false;
	}
	while (true) try
	{
		if (!reader->read())
			return true;
		aDictionary.add_entry(reader->word, reader->entry);
	}
	catch (const tts::phoneme_error &e)
	{
		fprintf(stderr, "error reading entry \"%s\": %s\n", reader->word->str().c_str(), e.what());
	}
	return true;
}

struct dictionary_phoneme_reader : public tts::phoneme_reader
{
	dictionary_phoneme_reader(tts::dictionary &aDictionary)
		: mDictionary(aDictionary)
	{
	}

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	tts::dictionary &mDictionary;
	ipa::phonemes mPhonemes;
	ipa::phonemes::const_iterator mCurrent;
	ipa::phonemes::const_iterator mLast;
};

void dictionary_phoneme_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mPhonemes.clear();
	if (!mDictionary.pronounce(aBuffer, {}, mPhonemes))
	{
		char msg[512];
		snprintf(msg, sizeof(msg), "cannot pronounce \"%s\"", aBuffer->str().c_str());
		throw tts::phoneme_error(msg);
	}

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

static bool
matches(const ipa::phonemes &a,
        const ipa::phonemes &b,
        bool ignore_syllable_breaks,
        ipa::phoneme::value_type mask)
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

		if (first1->get(mask) != first2->get(mask)) return false;
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
		if (c == '^') return true;
	}
	return false;
}

static bool pronounce(std::shared_ptr<tts::phoneme_reader> &dict,
                      const std::shared_ptr<cainteoir::buffer> &word,
                      std::shared_ptr<tts::phoneme_reader> &rules,
                      std::shared_ptr<tts::dictionary_formatter> &formatter,
                      std::shared_ptr<tts::phoneme_writer> &writer,
                      const char *phonemeset,
                      tts::stress_type stress,
                      bool ignore_syllable_breaks,
                      ipa::phoneme::value_type mask,
                      mode_type mode)
{
	static const cainteoir::object null_entry;

	ipa::phonemes phonemes;
	if (mode != mode_type::pronounce_entries)
	{
		try
		{
			dict->reset(word);
			while (dict->read())
				phonemes.push_back(*dict);
		}
		catch (const tts::phoneme_error &e)
		{
			fprintf(stdout, "missing entry : %s\n", e.what());
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
	catch (const std::exception &e)
	{
		if (mode == mode_type::compare_entries)
		{
			formatter->write_phoneme_entry(word, writer, phonemes, null_entry, " ... ");
			fprintf(stdout, "cannot pronounce : %s\n", e.what());
		}
		else
			fprintf(stderr, "cannot pronounce '%s': %s\n", word->str().c_str(), e.what());
		return false;
	}

	tts::make_stressed(pronounced, stress);

	bool match = matches(pronounced, phonemes, ignore_syllable_breaks, mask);
	if (mode == mode_type::mismatched_entries && match)
		return true;

	if (mode == mode_type::compare_entries)
	{
		formatter->write_phoneme_entry(word, writer, phonemes, null_entry, " ... ");
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
		formatter->write_phoneme_entry(word, writer, phonemes, null_entry);
	else
		formatter->write_phoneme_entry(word, writer, pronounced, null_entry);

	return match;
}

static bool pronounce(tts::dictionary &words,
                      std::shared_ptr<tts::phoneme_reader> &dict,
                      std::shared_ptr<tts::phoneme_reader> rules,
                      std::shared_ptr<tts::dictionary_formatter> &formatter,
                      std::shared_ptr<tts::phoneme_writer> writer,
                      const char *phonemeset,
                      tts::stress_type stress,
                      bool ignore_syllable_breaks,
                      bool ignore_stress,
                      mode_type mode)
{
	ipa::phoneme::value_type mask = ipa::main | ipa::diacritics;
	if (ignore_stress)
		mask |= ipa::tone_start | ipa::tone_middle | ipa::tone_end | ipa::length;
	else
		mask |= ipa::suprasegmentals;

	writer->reset(stdout);

	int matched = 0;
	int entries = 0;

	for (auto &entry : words)
	{
		if (is_variant(*entry.first)) continue;

		if (pronounce(dict, entry.first,
		              rules, formatter, writer, phonemeset, stress, ignore_syllable_breaks, mask, mode))
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
	auto tokens = tts::create_text_reader();
	tokens->reset(reader);
	while (tokens->read()) switch (tokens->token.get("Token:type").integer())
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		{
			auto text = tokens->token.get("Token:text").buffer();
			auto &match = base_dict.lookup(text);
			switch (match.get(0).get("Entry::pronunciation").type())
			{
			case cainteoir::object_type::null: // no match
				switch (word_mode)
				{
				case word_mode_type::merge:
				case word_mode_type::only_in_document:
					dict.add_entry(text, text);
					++words;
					break;
				default:
					fprintf(stderr, "error: cannot find '%s' in the dictionary\n", text->str().c_str());
					break;
				}
				break;
			case cainteoir::object_type::buffer: // say-as entry
				if (word_mode == word_mode_type::in_dictionary_and_document)
				{
					ipa::phonemes pronunciation;
					if (base_dict.pronounce(text, {}, pronunciation))
					{
						dict.add_entry(text, pronunciation);
						++words;
					}
				}
				break;
			case cainteoir::object_type::phonemes: case cainteoir::object_type::phonemes_ref:
				if (word_mode == word_mode_type::in_dictionary_and_document)
				{
					dict.add_entry(text, match);
					++words;
				}
				break;
			}
		}
		break;
	}

	return words;
}

static uint32_t from_dictionary(tts::dictionary &base_dict,
                                tts::dictionary &dict,
                                tts::dictionary &src_dict,
                                word_mode_type word_mode)
{
	uint32_t words = 0;
	for (auto &entry : src_dict)
	{
		auto &match = base_dict.lookup(entry.first);
		switch (match.get(0).get("Entry::pronunciation").type())
		{
		case cainteoir::object_type::null: // no match
			switch (word_mode)
			{
			case word_mode_type::merge:
			case word_mode_type::only_in_document:
				dict.add_entry(entry.first, entry.second);
				++words;
				break;
			default:
				fprintf(stderr, "error: cannot find '%s' in the dictionary\n", entry.first->str().c_str());
				break;
			}
			break;
		case cainteoir::object_type::buffer: // say-as entry
			if (word_mode == word_mode_type::in_dictionary_and_document)
			{
				ipa::phonemes pronunciation;
				if (base_dict.pronounce(entry.first, {}, pronunciation))
				{
					dict.add_entry(entry.first, pronunciation);
					++words;
				}
			}
			break;
		case cainteoir::object_type::phonemes: case cainteoir::object_type::phonemes_ref:
			if (word_mode == word_mode_type::in_dictionary_and_document)
			{
				dict.add_entry(entry.first, match);
				++words;
			}
			break;
		}
	}
	return words;
}

static std::shared_ptr<tts::phoneme_reader>
create_dict_reader(tts::dictionary &dict,
                   const char *phoneme_map,
                   const char *accent)
{
	std::shared_ptr<tts::phoneme_reader> reader = std::make_shared<dictionary_phoneme_reader>(dict);
	if (phoneme_map)
		reader = tts::createPhonemeToPhonemeConverter(phoneme_map, reader);
	if (accent)
		reader = tts::createAccentConverter(accent, reader);
	return reader;
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
		bool ignore_stress = false;
		const char *voicename = nullptr;
		const char *language = nullptr;
		const char *ruleset = nullptr;
		const char *dictionary = nullptr;
		const char *dictionary_format = nullptr;
		const char *phonemeset = "ipa";
		const char *source_dictionary = nullptr;
		const char *phoneme_map = nullptr;
		const char *accent = nullptr;
		const char *dictionary_phoneme_map = nullptr;
		const char *dictionary_accent = nullptr;

		const option_group general_options = { nullptr, {
			{ 't', "time", bind_value(time, true),
			  i18n("Time how long it takes to complete the action") },
			{ 'd', "dictionary", dictionary, "DICTIONARY",
			  i18n("Use the words in DICTIONARY") },
			{ 0, "dictionary-phoneme-map", dictionary_phoneme_map, "PHONEME_MAP",
			  i18n("Use PHONEME_MAP to convert dictionary phonemes (e.g. accent conversion)") },
			{ 0, "dictionary-accent", dictionary_accent, "ACCENT",
			  i18n("Use ACCENT to convert dictionary phonemes to the specified accent") },
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
			{ 0, "ignore-stress", bind_value(ignore_stress, true),
			  i18n("Ignore stress markers when comparing pronunciations.") },
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
			{ 'r', "ruleset", ruleset, "RULESET",
			  i18n("Use the RULESET pronunciation rule file") },
			{ 'v', "voice", voicename, "VOICE",
			  i18n("Use the TTS voice named VOICE") },
			{ 'l', "language", language, "LANG",
			  i18n("Use LANG with the ruleset, or to locate the TTS voice") },
			{ 'M', "phoneme-map", phoneme_map, "PHONEME_MAP",
			  i18n("Use PHONEME_MAP to convert phonemes (e.g. accent conversion)") },
			{ 'a', "accent", accent, "ACCENT",
			  i18n("Use ACCENT to convert phonemes to the specified accent") },
		}};

		const std::initializer_list<const option_group *> options = {
			&general_options,
			&output_options,
			&stress_options,
			&action_options,
			&pronunciation_options
		};

		const std::initializer_list<const char *> usage = {
			i18n("dictionary [OPTION..] DOCUMENT.."),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		auto writer = tts::createPhonemeWriter(phonemeset);
		cainteoir::stopwatch timer;

		tts::dictionary base_dict;
		tts::dictionary dict;
		tts::dictionary src_dict;

		uint32_t words = 0;
		if (dictionary != nullptr)
		{
			if (!add_dictionary_entries(dictionary, base_dict))
				return 0;
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

		if (source_dictionary != nullptr)
		{
			if (!add_dictionary_entries(source_dictionary, src_dict))
				return 0;
		}

		auto formatter = tts::createDictionaryFormatter(stdout, dictionary_format);
		if (!formatter)
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
			if (phoneme_map || accent)
			{
				auto rules = create_dict_reader(dict, phoneme_map, accent);
				tts::formatDictionary(dict, formatter, rules, writer);
			}
			else
				tts::formatDictionary(dict, formatter, writer, mode == mode_type::resolve_say_as_entries);
			break;
		case mode_type::pronounce_entries:
		case mode_type::compare_entries:
		case mode_type::mismatched_entries:
			if (source_dictionary != nullptr)
			{
				auto rules = create_dict_reader(src_dict, phoneme_map, accent);
				auto dictionary = create_dict_reader(dict, dictionary_phoneme_map, dictionary_accent);
				pronounce(dict, dictionary, rules, formatter, writer, phonemeset, stress, ignore_syllable_breaks, ignore_stress, mode);
			}
			else if (ruleset != nullptr)
			{
				auto locale = cainteoir::language::make_lang(language ? language : "");
				auto rules = tts::createPronunciationRules(ruleset, locale);
				if (!rules.get())
				{
					fprintf(stderr, "cannot load letter-to-phoneme rule file \"%s\"\n", argv[1]);
					return 0;
				}
				if (phoneme_map)
					rules = tts::createPhonemeToPhonemeConverter(phoneme_map, rules);
				if (accent)
					rules = tts::createAccentConverter(accent, rules);
				auto dictionary = create_dict_reader(dict, dictionary_phoneme_map, dictionary_accent);
				pronounce(dict, dictionary, rules, formatter, writer, phonemeset, stress, ignore_syllable_breaks, ignore_stress, mode);
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
				auto rules = engine.pronunciation();
				if (phoneme_map)
					rules = tts::createPhonemeToPhonemeConverter(phoneme_map, rules);
				if (accent)
					rules = tts::createAccentConverter(accent, rules);
				auto dictionary = create_dict_reader(dict, dictionary_phoneme_map, dictionary_accent);
				pronounce(dict, dictionary, rules, formatter, writer, phonemeset, stress, ignore_syllable_breaks, ignore_stress, mode);
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
