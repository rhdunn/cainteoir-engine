/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012-2014 Reece H. Dunn
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

#include <ucd/ucd.h>
#include <cainteoir/document.hpp>
#include <cainteoir/unicode.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/path.hpp>
#include <cainteoir/engines.hpp>

#include <stdexcept>
#include <cstdio>

namespace rdf  = cainteoir::rdf;
namespace tts  = cainteoir::tts;
namespace lang = cainteoir::language;

enum class mode_type
{
	parse_text,
	word_stream,
	phoneme_stream,
	context_analysis,
};

enum class phoneme_mode
{
	events,
	phonemes,
	broad_markers,
	narrow_markers,
	espeak_markers,
};

static const char *token_name[] = {
	"error",
	"upper",
	"lower",
	"capital",
	"mixed",
	"script",
	"number",
	"ordinal",
	"punctuation",
	"comma",
	"semicolon",
	"colon",
	"ellipsis",
	"full-stop",
	"double-stop",
	"exclamation",
	"question",
	"symbol",
	"end-para",
	"phonemes",
	"en-dash",
	"em-dash",
};

template <typename Reader>
void generate_events(Reader &text, const char *phonemeset)
{
	auto ipa = tts::createPhonemeWriter(phonemeset);
	ipa->reset(stdout);

	ucd::codepoint_t cp = 0;
	while (text.read())
	{
		auto &event = text.event();
		switch (event.type)
		{
		case tts::word_uppercase:
		case tts::word_lowercase:
		case tts::word_capitalized:
		case tts::word_mixedcase:
		case tts::word_script:
			cainteoir::utf8::read(event.text->begin(), cp);
			fprintf(stdout, ".%s.%-8s [%d..%d] %s\n",
			        ucd::get_script_string(ucd::lookup_script(cp)),
			        token_name[event.type],
			        *event.range.begin(),
			        *event.range.end(),
			        event.text->str().c_str());
			break;
		case tts::paragraph:
			fprintf(stdout, ".%-13s [%d..%d] \n",
			        token_name[event.type],
			        *event.range.begin(),
			        *event.range.end());
			break;
		case tts::phonemes:
			fprintf(stdout, ".%-13s [%d..%d] /",
			        token_name[event.type],
			        *event.range.begin(),
			        *event.range.end());
			for (auto p : event.phonemes)
				ipa->write(p);
			fprintf(stdout, "/\n");
			break;
		default:
			fprintf(stdout, ".%-13s [%d..%d] %s\n",
			        token_name[event.type],
			        *event.range.begin(),
			        *event.range.end(),
			        event.text->str().c_str());
			break;
		}
	}
}

bool parse_text(std::shared_ptr<cainteoir::document_reader> reader,
                mode_type type,
                phoneme_mode phonemes,
                const lang::tag &locale,
                tts::word_stream::number_scale scale,
                const char *ruleset,
                const char *dictionary,
                const char *phonemeset,
                tts::stress_type stress)
{
	if (type == mode_type::word_stream)
	{
		tts::word_stream text(reader, locale, scale);
		generate_events(text, phonemeset);
	}
	else if (type == mode_type::phoneme_stream)
	{
		auto rules = tts::createPronunciationRules(ruleset);
		auto dict = tts::createCainteoirDictionaryReader(dictionary);
		tts::phoneme_stream text(reader, locale, scale, rules, dict);
		switch (phonemes)
		{
		case phoneme_mode::events:
			generate_events(text, phonemeset);
			break;
		case phoneme_mode::phonemes:
			tts::generate_phonemes(text, stdout, phonemeset, stress, nullptr, nullptr);
			break;
		case phoneme_mode::broad_markers:
			tts::generate_phonemes(text, stdout, phonemeset, stress, "/", "/");
			break;
		case phoneme_mode::narrow_markers:
			tts::generate_phonemes(text, stdout, phonemeset, stress, "[", "]");
			break;
		case phoneme_mode::espeak_markers:
			tts::generate_phonemes(text, stdout, phonemeset, stress, "[[", "]]");
			break;
		}
	}
	else if (type == mode_type::context_analysis)
	{
		tts::context_analysis text(reader);
		generate_events(text, phonemeset);
	}
	else
	{
		tts::text_reader text(reader);
		generate_events(text, phonemeset);
	}
	return false;
}

int main(int argc, char ** argv)
{
	try
	{
		const char *ruleset = nullptr;
		const char *dictionary = nullptr;
		const char *phonemeset = "ipa";
		const char *locale_name = "en";
		tts::stress_type stress = tts::stress_type::as_transcribed;
		mode_type type = mode_type::parse_text;
		phoneme_mode phonemes = phoneme_mode::events;
		tts::word_stream::number_scale scale = tts::word_stream::short_scale;
		bool document_object = false;

		const option_group general_options = { nullptr, {
			{ 'm', "document-object", bind_value(document_object, true),
			  i18n("Process events through a cainteoir::document object model") },
		}};

		const option_group processing_options = { i18n("Processing Options:"), {
			{ 'l', "locale", locale_name, "LOCALE",
			  i18n("Use LOCALE for processing numbers") },
			{ 0, "short-scale", bind_value(scale, tts::word_stream::short_scale),
			  i18n("Use the short scale for processing numbers") },
			{ 0, "long-scale", bind_value(scale, tts::word_stream::long_scale),
			  i18n("Use the long scale for processing numbers") },
			{ 'd', "dictionary", dictionary, "DICTIONARY",
			  i18n("Use the DICTIONARY pronunciation dictionary") },
			{ 'r', "ruleset", ruleset, "RULESET",
			  i18n("Use the RULESET pronunciation rule file") },
			{ 'P', "phonemeset", phonemeset, "PHONEMESET",
			  i18n("Use PHONEMESET to transcribe phonemes as (default: ipa)") },
		}};

		const option_group mode_options = { i18n("Processing Mode:"), {
			{ 0, "parsetext", bind_value(type, mode_type::parse_text),
			  i18n("Split the text into lexical segments") },
			{ 0, "wordstream", bind_value(type, mode_type::word_stream),
			  i18n("Convert the document into a sequence of words") },
			{ 0, "phonemestream", bind_value(type, mode_type::phoneme_stream),
			  i18n("Convert the document into phonetic pronunciations") },
			{ 0, "contextanalysis", bind_value(type, mode_type::context_analysis),
			  i18n("Apply context analysis on the document") },
		}};

		const option_group phoneme_options = { i18n("Phoneme Stream Mode:"), {
			{ 0, "phonemes", bind_value(phonemes, phoneme_mode::phonemes),
			  i18n("Show phonemes without event annotations") },
			{ 0, "broad", bind_value(phonemes, phoneme_mode::broad_markers),
			  i18n("Use /.../ between phonetic transcriptions") },
			{ 0, "narrow", bind_value(phonemes, phoneme_mode::narrow_markers),
			  i18n("Use [...] between phonetic transcriptions") },
			{ 0, "espeak", bind_value(phonemes, phoneme_mode::espeak_markers),
			  i18n("Use [[...]] between phonetic transcriptions") },
			{ 0, "vowel-stress", bind_value(stress, tts::stress_type::vowel),
			  i18n("Place the stress on vowels (e.g. espeak, arpabet)") },
			{ 0, "syllable-stress", bind_value(stress, tts::stress_type::syllable),
			  i18n("Place the stress on syllable boundaries") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("parsetext [OPTION..] DOCUMENT"),
			i18n("parsetext [OPTION..]"),
		};

		const std::initializer_list<option_group> options = {
			general_options,
			mode_options,
			processing_options,
			phoneme_options
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		lang::tag locale = lang::make_lang(locale_name);

		rdf::graph metadata;
		const char *filename = (argc == 1) ? argv[0] : nullptr;
		auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", filename ? filename : "<stdin>");
			return 0;
		}

		bool show_help = false;
		if (document_object)
		{
			cainteoir::document doc(reader, metadata);
			auto docreader = cainteoir::createDocumentReader(doc.children());
			show_help = parse_text(docreader, type, phonemes, locale, scale, ruleset, dictionary, phonemeset, stress);
		}
		else
			show_help = parse_text(reader, type, phonemes, locale, scale, ruleset, dictionary, phonemeset, stress);
		if (show_help)
		{
			print_help({ general_options, mode_options }, usage);
			return 0;
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
