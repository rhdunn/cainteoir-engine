/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012-2015 Reece H. Dunn
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
#include <cainteoir/language.hpp>
#include <cainteoir/path.hpp>
#include <cainteoir/engines.hpp>

#include <stdexcept>
#include <cstdio>

namespace css  = cainteoir::css;
namespace rdf  = cainteoir::rdf;
namespace tts  = cainteoir::tts;
namespace lang = cainteoir::language;

enum class mode_type
{
	parse_text,
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
	"en-dash",
	"em-dash",
};

static void
print_time(const css::time &time)
{
	switch (time.units())
	{
	case css::time::inherit:      fprintf(stdout, "unspecified"); break;
	case css::time::seconds:      fprintf(stdout, "%Gs",  time.value()); break;
	case css::time::milliseconds: fprintf(stdout, "%Gms", time.value()); break;
	}
}

static void
format_style(const css::styles &styles)
{
	switch (styles.display)
	{
	case css::display::inherit:    break;
	case css::display::block:
		switch (styles.role)
		{
		case css::role::none:      fprintf(stdout, "block"); break;
		case css::role::paragraph: fprintf(stdout, "paragraph"); break;
		case css::role::heading:   fprintf(stdout, "heading %d", styles.aria_level); return;
		default:                   break;
		}
		break;
	case css::display::inlined:
		switch (styles.role)
		{
		case css::role::none:     fprintf(stdout, "span"); break;
		case css::role::sentence: fprintf(stdout, "sentence"); break;
		default:                   break;
		}
		break;
	case css::display::line_break: fprintf(stdout, "line-break"); break;
	case css::display::list_item:  fprintf(stdout, "list-item"); break;
	case css::display::table:      fprintf(stdout, "table"); break;
	case css::display::table_row:  fprintf(stdout, "row"); break;
	case css::display::table_cell: fprintf(stdout, "cell"); break;
	case css::display::none:       fprintf(stdout, "none"); break;
	}

	switch (styles.media_synchronisation)
	{
	case css::media_synchronisation::inherit:    break;
	case css::media_synchronisation::sequential: fprintf(stdout, " +sequential"); break;
	case css::media_synchronisation::parallel:   fprintf(stdout, " +parallel"); break;
	}

	switch (styles.vertical_align)
	{
	case css::vertical_align::inherit:  break;
	case css::vertical_align::baseline: fprintf(stdout, " +baseline"); break;
	case css::vertical_align::sub:      fprintf(stdout, " +subscript"); break;
	case css::vertical_align::super:    fprintf(stdout, " +superscript"); break;
	}

	switch (styles.text_decoration)
	{
	case css::text_decoration::inherit:      break;
	case css::text_decoration::none:         fprintf(stdout, " -underline -line-through"); break;
	case css::text_decoration::underline:    fprintf(stdout, " +underline"); break;
	case css::text_decoration::line_through: fprintf(stdout, " +line-through"); break;
	}

	switch (styles.font_style)
	{
	case css::font_style::inherit: break;
	case css::font_style::normal:  fprintf(stdout, " +normal-style"); break;
	case css::font_style::italic:  fprintf(stdout, " +emphasized"); break;
	case css::font_style::oblique: fprintf(stdout, " +oblique"); break;
	}

	switch (styles.font_weight)
	{
	case css::font_weight::inherit: break;
	case css::font_weight::normal:  fprintf(stdout, " +normal-weight"); break;
	case css::font_weight::bold:    fprintf(stdout, " +strong"); break;
	}

	if (!styles.list_style_type.empty())
		fprintf(stdout, " +list=%s", styles.list_style_type.c_str());

	if (!styles.font_family.empty())
		fprintf(stdout, " +%s", styles.font_family.c_str());
}

struct document_events: public tts::text_callback
{
	void onevent(const cainteoir::document_item &item);
};

void
document_events::onevent(const cainteoir::document_item &item)
{
	if (item.type & cainteoir::events::anchor)
	{
		fprintf(stdout, "anchor [%s]%s\n",
		        item.anchor.ns.c_str(),
		        item.anchor.ref.c_str());
	}
	if (item.type & cainteoir::events::text_ref)
	{
		fprintf(stdout, "text-ref [%s]%s\n",
		        item.anchor.ns.c_str(),
		        item.anchor.ref.c_str());
	}
	if (item.type & cainteoir::events::media_ref)
	{
		fprintf(stdout, "media-ref [%s]%s [from=",
		        item.anchor.ns.c_str(),
		        item.anchor.ref.c_str());
		print_time(item.media_begin);
		fprintf(stdout, " ; to=");
		print_time(item.media_end);
		fprintf(stdout, "]\n");
	}
	if (item.type & cainteoir::events::begin_context)
	{
		fprintf(stdout, "begin-context ");
		if (item.styles)
			format_style(*item.styles);
		fprintf(stdout, "\n");
	}
	if (item.type & cainteoir::events::text)
	{
		fprintf(stdout, "text(%zu) [%u..%u]: \"\"\"",
			item.content->size(),
			*item.range.begin(),
			*item.range.end());
		fwrite(item.content->begin(), 1, item.content->size(), stdout);
		fwrite("\"\"\"\n", 1, 4, stdout);
	}
	if (item.type & cainteoir::events::end_context)
		fprintf(stdout, "end-context\n");
}

static void
print_event(const tts::text_event &event)
{
	ucd::codepoint_t cp = 0;
	bool need_space = true;
	switch (event.type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		cainteoir::utf8::read(event.text->begin(), cp);
		fprintf(stdout, ".%s.%-8s [%d..%d] %s",
		        ucd::get_script_string(ucd::lookup_script(cp)),
		        token_name[event.type],
		        *event.range.begin(),
		        *event.range.end(),
		        event.text ? event.text->str().c_str() : "(null)");
		break;
	case tts::paragraph:
		fprintf(stdout, ".%-13s [%d..%d] ",
		        token_name[event.type],
		        *event.range.begin(),
		        *event.range.end());
		need_space = false;
		break;
	default:
		fprintf(stdout, ".%-13s [%d..%d] %s",
		        token_name[event.type],
		        *event.range.begin(),
		        *event.range.end(),
		        event.text ? event.text->str().c_str() : "(null)");
		break;
	}
	fputc('\n', stdout);
}

static void
print_events(const std::shared_ptr<tts::text_reader> &text)
{
	while (text->read())
	{
		tts::text_event event(text->token.get("Token:text").buffer(),
		                      (tts::event_type)text->token.get("Token:type").integer(),
		                      text->token.get("Token:range").range(),
		                      text->token.get("Token:codepoint").codepoint());
		print_event(event);
	}
}

int main(int argc, char ** argv)
{
	try
	{
		mode_type type = mode_type::parse_text;
		bool document_object = false;
		bool print_document_events = false;

		const option_group general_options = { nullptr, {
			{ 'm', "document-object", bind_value(document_object, true),
			  i18n("Process events through a cainteoir::document object model") },
			{ 0, "document-events", bind_value(print_document_events, true),
			  i18n("Print document events along side the parsed text events") },
		}};

		const option_group mode_options = { i18n("Processing Mode:"), {
			{ 0, "parsetext", bind_value(type, mode_type::parse_text),
			  i18n("Split the text into lexical segments") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("parsetext [OPTION..] DOCUMENT"),
			i18n("parsetext [OPTION..]"),
		};

		const std::initializer_list<const option_group *> options = {
			&general_options,
			&mode_options,
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		rdf::graph metadata;
		const char *filename = (argc == 1) ? argv[0] : nullptr;
		auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", filename ? filename : "<stdin>");
			return 0;
		}

		document_events events;
		auto text = tts::create_text_reader(print_document_events ? &events : nullptr);

		if (document_object)
		{
			cainteoir::document doc(reader, metadata);
			auto docreader = cainteoir::createDocumentReader(doc.children());
			text->reset(docreader);
			print_events(text);
		}
		else
		{
			text->reset(reader);
			print_events(text);
		}

		return 0;
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
