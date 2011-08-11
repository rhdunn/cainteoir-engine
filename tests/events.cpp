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
#include <cainteoir/platform.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <getopt.h>

namespace rdf = cainteoir::rdf;

struct events : public cainteoir::document_events
{
	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
		fprintf(stdout, "text(%zu): \"\"\"", aText->size());
		fwrite(aText->begin(), 1, aText->size(), stdout);
		fwrite("\"\"\"\n", 1, 4, stdout);
	}

	void begin_context(context aContext, uint32_t aParameter)
	{
		fprintf(stdout, "begin-context ");
		switch (aContext)
		{
		case paragraph: fprintf(stdout, "paragraph"); break;
		case heading:   fprintf(stdout, "heading %d", aParameter); break;
		case span:      fprintf(stdout, "span"); break;
		case list:      fprintf(stdout, "list"); break;
		case list_item: fprintf(stdout, "list-item"); break;
		case sentence:  fprintf(stdout, "sentence"); break;
		}
		if (aContext != heading)
		{
			if (aParameter & superscript) fprintf(stdout, " +superscript");
			if (aParameter & subscript)   fprintf(stdout, " +subscript");
			if (aParameter & emphasized)  fprintf(stdout, " +emphasized");
			if (aParameter & strong)      fprintf(stdout, " +strong");
			if (aParameter & underline)   fprintf(stdout, " +underline");
			if (aParameter & monospace)   fprintf(stdout, " +monospace");
			if (aParameter & reduced)     fprintf(stdout, " +reduced");
		}
		fprintf(stdout, "\n");
	}

	void end_context()
	{
		fprintf(stdout, "end-context\n");
	}

	void toc_entry(int depth, const rdf::uri &location, const std::string &title)
	{
		fprintf(stdout, "toc-entry [%s]%s depth=%d title=\"\"\"%s\"\"\"\n", location.ns.c_str(), location.ref.c_str(), depth, title.c_str());
	}

	void anchor(const rdf::uri &location)
	{
		fprintf(stdout, "anchor [%s]%s\n", location.ns.c_str(), location.ref.c_str());
	}
};

int main(int argc, char ** argv)
{
	cainteoir::initialise();

	try
	{
		argc -= 1;
		argv += 1;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		events events;
		rdf::graph metadata;
		if (!cainteoir::parseDocument(argv[0], events, metadata))
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	cainteoir::cleanup();
	return 0;
}
