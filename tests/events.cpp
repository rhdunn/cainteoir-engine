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

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

struct event_printer : public cainteoir::document_events
{
	event_printer(bool aTextOnly) : mTextOnly(aTextOnly)
	{
	}

	void text(std::shared_ptr<cainteoir::buffer> aText)
	{
		if (!mTextOnly)
			fprintf(stdout, "text(%zu): \"\"\"", aText->size());
		fwrite(aText->begin(), 1, aText->size(), stdout);
		if (!mTextOnly)
			fwrite("\"\"\"\n", 1, 4, stdout);
	}

	void begin_context(events::context aContext, uint32_t aParameter)
	{
		if (mTextOnly)
		{
			switch (aContext)
			{
			case events::paragraph:
			case events::heading:
			case events::list:
			case events::list_item:
				fwrite("\n\n", 1, 2, stdout);
				break;
			}
			return;
		}

		fprintf(stdout, "begin-context ");
		switch (aContext)
		{
		case events::paragraph: fprintf(stdout, "paragraph"); break;
		case events::heading:   fprintf(stdout, "heading %d", aParameter); break;
		case events::span:      fprintf(stdout, "span"); break;
		case events::list:      fprintf(stdout, "list"); break;
		case events::list_item: fprintf(stdout, "list-item"); break;
		case events::sentence:  fprintf(stdout, "sentence"); break;
		}
		if (aContext != events::heading)
		{
			if (aParameter & events::superscript) fprintf(stdout, " +superscript");
			if (aParameter & events::subscript)   fprintf(stdout, " +subscript");
			if (aParameter & events::emphasized)  fprintf(stdout, " +emphasized");
			if (aParameter & events::strong)      fprintf(stdout, " +strong");
			if (aParameter & events::underline)   fprintf(stdout, " +underline");
			if (aParameter & events::monospace)   fprintf(stdout, " +monospace");
			if (aParameter & events::reduced)     fprintf(stdout, " +reduced");
		}
		fprintf(stdout, "\n");
	}

	void end_context()
	{
		if (!mTextOnly)
			fprintf(stdout, "end-context\n");
	}

	void toc_entry(int depth, const rdf::uri &location, const std::string &title)
	{
		if (!mTextOnly)
			fprintf(stdout, "toc-entry [%s]%s depth=%d title=\"\"\"%s\"\"\"\n", location.ns.c_str(), location.ref.c_str(), depth, title.c_str());
	}

	void anchor(const rdf::uri &location, const std::string &mimetype)
	{
		if (!mTextOnly)
			fprintf(stdout, "anchor [%s]%s\n", location.ns.c_str(), location.ref.c_str());
	}

	bool mTextOnly;
};

int main(int argc, char ** argv)
{
	cainteoir::initialise();

	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		event_printer events(argc > 1 && !strcmp(argv[1], "--text-only"));
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
