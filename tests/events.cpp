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
			if (reader->type & cainteoir::events::toc_entry)
			{
				fprintf(stdout, "toc-entry [%s]%s depth=%d title=\"\"\"%s\"\"\"\n",
				        reader->anchor.ns.c_str(),
				        reader->anchor.ref.c_str(),
				        reader->parameter,
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
				switch (reader->context)
				{
				case events::paragraph: fprintf(stdout, "paragraph"); break;
				case events::heading:   fprintf(stdout, "heading %d", reader->parameter); break;
				case events::span:      fprintf(stdout, "span"); break;
				case events::list:      fprintf(stdout, "list"); break;
				case events::list_item: fprintf(stdout, "list-item"); break;
				case events::sentence:  fprintf(stdout, "sentence"); break;
				case events::table:     fprintf(stdout, "table"); break;
				case events::row:       fprintf(stdout, "row"); break;
				case events::cell:      fprintf(stdout, "cell"); break;
				}

				if (reader->context != events::heading)
				{
					if (reader->parameter & events::superscript)
						fprintf(stdout, " +superscript");
					if (reader->parameter & events::subscript)
						fprintf(stdout, " +subscript");
					if (reader->parameter & events::emphasized)
						fprintf(stdout, " +emphasized");
					if (reader->parameter & events::strong)
						fprintf(stdout, " +strong");
					if (reader->parameter & events::underline)
						fprintf(stdout, " +underline");
					if (reader->parameter & events::monospace)
						fprintf(stdout, " +monospace");
					if (reader->parameter & events::reduced)
						fprintf(stdout, " +reduced");
				}
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
