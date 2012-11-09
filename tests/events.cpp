/* Test for generated speech events.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/document.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <getopt.h>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

void format_style(const cainteoir::styles &styles)
{
	using cainteoir::display;
	using cainteoir::vertical_align;
	using cainteoir::text_decoration;
	using cainteoir::font_style;
	using cainteoir::font_weight;
	using cainteoir::text_structure;

	switch (styles.display)
	{
	case display::inherit:    break;
	case display::block:
		switch (styles.text_structure)
		{
		case text_structure::none:    fprintf(stdout, "paragraph"); break;
		case text_structure::heading: fprintf(stdout, "heading %d", styles.toc_level); return;
		}
		break;
	case display::inlined:
		switch (styles.text_structure)
		{
		case text_structure::none:     fprintf(stdout, "span"); break;
		case text_structure::sentence: fprintf(stdout, "sentence"); break;
		}
		break;
	case display::list_item:  fprintf(stdout, "list-item"); break;
	case display::table:      fprintf(stdout, "table"); break;
	case display::table_row:  fprintf(stdout, "row"); break;
	case display::table_cell: fprintf(stdout, "cell"); break;
	case display::none:       fprintf(stdout, "none"); break;
	}

	switch (styles.vertical_align)
	{
	case vertical_align::inherit:  break;
	case vertical_align::baseline: fprintf(stdout, " +baseline"); break;
	case vertical_align::sub:      fprintf(stdout, " +subscript"); break;
	case vertical_align::super:    fprintf(stdout, " +superscript"); break;
	}

	switch (styles.text_decoration)
	{
	case text_decoration::inherit:      break;
	case text_decoration::none:         fprintf(stdout, " -underline -line-through"); break;
	case text_decoration::underline:    fprintf(stdout, " +underline"); break;
	case text_decoration::line_through: fprintf(stdout, " +line-through"); break;
	}

	switch (styles.font_style)
	{
	case font_style::inherit: break;
	case font_style::normal:  fprintf(stdout, " +normal-style"); break;
	case font_style::italic:  fprintf(stdout, " +emphasized"); break;
	case font_style::oblique: fprintf(stdout, " +oblique"); break;
	}

	switch (styles.font_weight)
	{
	case font_weight::inherit: break;
	case font_weight::normal:  fprintf(stdout, " +normal-weight"); break;
	case font_weight::bold:    fprintf(stdout, " +strong"); break;
	}

	if (styles.list_style_type)
		fprintf(stdout, " +list=%s", styles.list_style_type->name.c_str());

	if (!styles.font_family.empty())
		fprintf(stdout, " +%s", styles.font_family.c_str());
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		while (reader->read())
		{
			if (reader->type & cainteoir::events::toc_entry && reader->styles)
			{
				fprintf(stdout, "toc-entry [%s]%s depth=%d title=\"\"\"%s\"\"\"\n",
				        reader->anchor.ns.c_str(),
				        reader->anchor.ref.c_str(),
				        reader->styles->toc_level,
				        reader->text->str().c_str());
			}
			if (reader->type & cainteoir::events::anchor)
			{
				fprintf(stdout, "anchor [%s]%s\n",
				        reader->anchor.ns.c_str(),
				        reader->anchor.ref.c_str());
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
				fprintf(stdout, "text(%zu): \"\"\"", reader->text->size());
				fwrite(reader->text->begin(), 1, reader->text->size(), stdout);
				fwrite("\"\"\"\n", 1, 4, stdout);
			}
			if (reader->type & cainteoir::events::end_context)
				fprintf(stdout, "end-context\n");
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
