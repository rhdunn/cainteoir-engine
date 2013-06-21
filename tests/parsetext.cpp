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

#include <ucd/ucd.h>
#include <cainteoir/document.hpp>
#include <cainteoir/unicode.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/engines.hpp>

#include <stdexcept>
#include <cstdio>
#include <getopt.h>

namespace rdf  = cainteoir::rdf;
namespace tts  = cainteoir::tts;
namespace lang = cainteoir::language;

enum
{
	ARG_PARSETEXT       = 301,
	ARG_WORDSTREAM      = 302,
	ARG_PHONEMESTREAM   = 303,
	ARG_CONTEXTANALYSIS = 304,
	ARG_PRONOUNCE       = 305,
	ARG_SHORTSCALE      = 401,
	ARG_LONGSCALE       = 402,
};

static struct option options[] =
{
	{ "locale",          required_argument, 0, 'l' },
	{ "parsetext",       no_argument,       0, ARG_PARSETEXT },
	{ "wordstream",      no_argument,       0, ARG_WORDSTREAM },
	{ "phonemestream",   no_argument,       0, ARG_PHONEMESTREAM },
	{ "contextanalysis", no_argument,       0, ARG_CONTEXTANALYSIS },
	{ "pronounce",       no_argument,       0, ARG_PRONOUNCE },
	{ "short-scale",     no_argument,       0, ARG_SHORTSCALE },
	{ "long-scale",      no_argument,       0, ARG_LONGSCALE },
	{ "document-object", no_argument,       0, 'd' },
	{ 0, 0, 0, 0 }
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
	auto reader = engine.pronounciation();

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

void parse_text(std::shared_ptr<cainteoir::document_reader> reader,
                int type,
                const lang::tag &locale,
                tts::word_stream::number_scale scale,
                int argc,
                char **argv)
{
	if (type == ARG_WORDSTREAM)
	{
		tts::word_stream text(reader, locale, scale);
		generate_events(text);
	}
	else if (type == ARG_PRONOUNCE)
	{
		if (argc != 2)
			throw std::runtime_error("usage: parsetext --prounounce <document> <phoneme-set>");

		tts::word_stream text(reader, locale, scale);
		pronounce(text, argv[1]);
	}
	else if (type == ARG_PHONEMESTREAM)
	{
		if (argc != 3)
			throw std::runtime_error("usage: parsetext --phonemestream <document> <ruleset> <dictionary>");

		auto rules = tts::createPronunciationRules(cainteoir::path(argv[1]));
		if (!rules.get())
		{
			fprintf(stderr, "unable to load pronunciation rules: %s\n", argv[1]);
			return;
		}

		tts::phoneme_stream text(reader, locale, scale, rules, cainteoir::path(argv[2]));
		generate_events(text);
	}
	else if (type == ARG_CONTEXTANALYSIS)
	{
		tts::context_analysis text(reader);
		generate_events(text);
	}
	else
	{
		tts::text_reader text(reader);
		generate_events(text);
	}
}

int main(int argc, char ** argv)
{
	try
	{
		lang::tag locale = { "en" };
		int type = ARG_PARSETEXT;
		tts::word_stream::number_scale scale = tts::word_stream::short_scale;
		bool document_object = false;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "dl:", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'd':
				document_object = true;
				break;
			case 'l':
				locale = lang::make_lang(optarg);
				break;
			case ARG_PARSETEXT:
			case ARG_WORDSTREAM:
			case ARG_PHONEMESTREAM:
			case ARG_CONTEXTANALYSIS:
			case ARG_PRONOUNCE:
				type = c;
				break;
			case ARG_SHORTSCALE:
				scale = tts::word_stream::short_scale;
				break;
			case ARG_LONGSCALE:
				scale = tts::word_stream::long_scale;
				break;
			}
		}

		argc -= optind;
		argv += optind;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		if (document_object)
		{
			cainteoir::document doc(reader);
			auto docreader = cainteoir::createDocumentReader(doc.children());
			parse_text(docreader, type, locale, scale, argc, argv);
		}
		else
			parse_text(reader, type, locale, scale, argc, argv);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
