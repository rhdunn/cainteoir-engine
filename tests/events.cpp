/* Test for generated speech events.
 *
 * Copyright (C) 2010-2014 Reece H. Dunn
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/document.hpp>

#include <stdexcept>
#include <iostream>
#include <cstdio>

namespace css    = cainteoir::css;
namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

void print_time(const css::time &time)
{
	switch (time.units())
	{
	case css::time::inherit:      fprintf(stdout, "unspecified"); break;
	case css::time::seconds:      fprintf(stdout, "%Gs",  time.value()); break;
	case css::time::milliseconds: fprintf(stdout, "%Gms", time.value()); break;
	}
}

void format_style(const css::styles &styles)
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

void print_navigation(const rdf::graph &metadata, const cainteoir::document &doc)
{
	const rdf::uri type = rdf::epv("toc");
	for (const auto &entry : doc.navigation(metadata, type))
	{
		fprintf(stdout, "[%s]%s level=%d target=[%s]%s title=\"\"\"%s\"\"\"\n",
		        type.ns.c_str(), type.ref.c_str(),
		        entry.depth,
		        entry.location.ns.c_str(), entry.location.ref.c_str(),
		        entry.title.c_str());
	}
}

void print_events(const std::shared_ptr<cainteoir::document_reader> &reader)
{
	while (reader->read())
	{
		if (reader->type & cainteoir::events::anchor)
		{
			fprintf(stdout, "anchor [%s]%s\n",
			        reader->anchor.ns.c_str(),
			        reader->anchor.ref.c_str());
		}
		if (reader->type & cainteoir::events::text_ref)
		{
			fprintf(stdout, "text-ref [%s]%s\n",
			        reader->anchor.ns.c_str(),
			        reader->anchor.ref.c_str());
		}
		if (reader->type & cainteoir::events::media_ref)
		{
			fprintf(stdout, "media-ref [%s]%s [from=",
			        reader->anchor.ns.c_str(),
			        reader->anchor.ref.c_str());
			print_time(reader->media_begin);
			fprintf(stdout, " ; to=");
			print_time(reader->media_end);
			fprintf(stdout, "]\n");
		}
		if (reader->type & cainteoir::events::begin_context)
		{
			fprintf(stdout, "begin-context ");
			if (reader->styles)
				format_style(*reader->styles);
			fprintf(stdout, "\n");
		}
		if (reader->type & cainteoir::events::text)
		{
			fprintf(stdout, "text(%zu): \"\"\"", reader->content->size());
			fwrite(reader->content->begin(), 1, reader->content->size(), stdout);
			fwrite("\"\"\"\n", 1, 4, stdout);
		}
		if (reader->type & cainteoir::events::end_context)
			fprintf(stdout, "end-context\n");
	}
}

int main(int argc, char ** argv)
{
	try
	{
		bool document_object = false;
		bool show_navigation = false;

		const option_group general_options = { nullptr, {
			{ 'm', "document-object", bind_value(document_object, true),
			  i18n("Process events through a cainteoir::document object model") },
			{ 'n', "show-navigation", bind_value(show_navigation, true),
			  i18n("Print the navigation structure, not document content") },
		}};

		const std::initializer_list<const char *> usage = {
			i18n("events [OPTION..] DOCUMENT"),
			i18n("events [OPTION..]"),
		};

		const std::initializer_list<option_group> options = {
			general_options,
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		rdf::graph metadata;
		const char *filename = (argc == 1) ? argv[0] : nullptr;
		auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		if (document_object || show_navigation)
		{
			cainteoir::document doc(reader);
			if (show_navigation)
				print_navigation(metadata, doc);
			else
			{
				auto docreader = cainteoir::createDocumentReader(doc.children());
				print_events(docreader);
			}
		}
		else
			print_events(reader);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
