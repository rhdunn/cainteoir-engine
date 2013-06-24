/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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
	pronounce,
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
	"pause",
};

template <typename Reader>
void generate_events(Reader &text)
{
	auto ipa = tts::createPhonemeWriter("ipa");
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
			        event.range.begin(),
			        event.range.end(),
			        event.text->str().c_str());
			break;
		case tts::paragraph:
			fprintf(stdout, ".%-13s [%d..%d] \n",
			        token_name[event.type],
			        event.range.begin(),
			        event.range.end());
			break;
		case tts::pause:
			fprintf(stdout, ".%-13s [%d..%d] %dms\n",
			        token_name[event.type],
			        event.range.begin(),
			        event.range.end(),
			        event.duration);
			break;
		case tts::phonemes:
			fprintf(stdout, ".%-13s [%d..%d] /",
			        token_name[event.type],
			        event.range.begin(),
			        event.range.end());
			for (auto p : event.phonemes)
				ipa->write(p);
			fprintf(stdout, "/\n");
			break;
		default:
			fprintf(stdout, ".%-13s [%d..%d] %s\n",
			        token_name[event.type],
			        event.range.begin(),
			        event.range.end(),
			        event.text->str().c_str());
			break;
		}
	}
}

void pronounce(tts::word_stream &text, const char *phonemeset)
{
	rdf::graph metadata;
	tts::engines engine(metadata);
	auto reader = engine.pronunciation();

	auto writer = tts::createPhonemeWriter(phonemeset);
	writer->reset(stdout);

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
			reader->reset(event.text);
			fprintf(stdout, "%s /", event.text->str().c_str());
			while (reader->read())
				writer->write(*reader);
			fprintf(stdout, "/\n");
			break;
		}
	}
}

bool parse_text(std::shared_ptr<cainteoir::document_reader> reader,
                mode_type type,
                const lang::tag &locale,
                tts::word_stream::number_scale scale,
                int argc,
                char **argv)
{
	if (type == mode_type::word_stream)
	{
		tts::word_stream text(reader, locale, scale);
		generate_events(text);
	}
	else if (type == mode_type::pronounce)
	{
		if (argc != 2)
			return true;

		tts::word_stream text(reader, locale, scale);
		pronounce(text, argv[1]);
	}
	else if (type == mode_type::phoneme_stream)
	{
		if (argc != 3)
			return true;

		auto rules = tts::createPronunciationRules(argv[1]);
		if (!rules.get())
		{
			fprintf(stderr, "unable to load pronunciation rules: %s\n", argv[1]);
			return false;
		}

		tts::phoneme_stream text(reader, locale, scale, rules, cainteoir::path(argv[2]));
		generate_events(text);
	}
	else if (type == mode_type::context_analysis)
	{
		tts::context_analysis text(reader);
		generate_events(text);
	}
	else
	{
		tts::text_reader text(reader);
		generate_events(text);
	}
	return false;
}

int main(int argc, char ** argv)
{
	try
	{
		lang::tag locale = { "en" };
		mode_type type = mode_type::parse_text;
		tts::word_stream::number_scale scale = tts::word_stream::short_scale;
		bool document_object = false;

		const option_group general_options = { nullptr, {
			{ 'd', "document-object", no_argument, nullptr,
			  i18n("Process events through a cainteoir::document object"),
			  [&document_object](const char *) { document_object = true; }},
			{ 'l', "locale", required_argument, "LOCALE",
			  i18n("Use LOCALE for processing numbers"),
			  [&locale](const char *arg) { locale = lang::make_lang(arg); }},
			{ 0, "short-scale", no_argument, nullptr,
			  i18n("Use the short scale for processing numbers"),
			  [&scale](const char *) { scale = tts::word_stream::short_scale; }},
			{ 0, "long-scale", no_argument, nullptr,
			  i18n("Use the long scale for processing numbers"),
			  [&scale](const char *) { scale = tts::word_stream::long_scale; }},
		}};

		const option_group mode_options = { i18n("Processing Mode:"), {
			{ 0, "parsetext", no_argument, nullptr,
			  i18n("Split the text into lexical segments"),
			  [&type](const char *) { type = mode_type::parse_text; }},
			{ 0, "wordstream", no_argument, nullptr,
			  i18n("Convert the document into a sequence of words"),
			  [&type](const char *) { type = mode_type::word_stream; }},
			{ 0, "phonemestream", no_argument, nullptr,
			  i18n("Convert the document into phonetic pronunciations"),
			  [&type](const char *) { type = mode_type::phoneme_stream; }},
			{ 0, "contextanalysis", no_argument, nullptr,
			  i18n("Apply context analysis on the document"),
			  [&type](const char *) { type = mode_type::context_analysis; }},
			{ 0, "pronounce", no_argument, nullptr,
			  i18n("Pronounce all the words in the document"),
			  [&type](const char *) { type = mode_type::pronounce; }},
		}};

		const std::initializer_list<const char *> usage = {
			i18n("parsetext [OPTION..] DOCUMENT"),
			i18n("parsetext [OPTION..] --pronounce DOCUMENT PHONEMESET"),
			i18n("parsetext [OPTION..] --phonemestream DOCUMENT RULESET DICTIONARY"),
		};

		if (!parse_command_line({ general_options, mode_options }, usage, argc, argv))
			return 0;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		bool show_help = false;
		if (document_object)
		{
			cainteoir::document doc(reader);
			auto docreader = cainteoir::createDocumentReader(doc.children());
			show_help = parse_text(docreader, type, locale, scale, argc, argv);
		}
		else
			show_help = parse_text(reader, type, locale, scale, argc, argv);
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
