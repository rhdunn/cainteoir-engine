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
	void metadata(const rdf::statement &aStatement)
	{
	}

	const rdf::bnode genid()
	{
		return rdf::bnode(std::string());
	}

	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
		fprintf(stdout, "text(%zu): \"\"\"", aText->size());
		fwrite(aText->begin(), 1, aText->size(), stdout);
		fwrite("\"\"\"\n", 1, 4, stdout);
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
		if (!cainteoir::parseDocument(argv[0], events))
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	cainteoir::cleanup();
	return 0;
}
