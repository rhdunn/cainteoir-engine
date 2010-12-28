/* Metadata Extractor.
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
#include <cainteoir/mimetypes.hpp>
#include <cainteoir/parsers.hpp>
#include <iostream>
#include <cstdio>

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xmldom;

int main(int argc, char ** argv)
{
	LIBXML_TEST_VERSION

	try
	{
		++argv;
		--argc;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		std::auto_ptr<cainteoir::buffer> text_buffer = std::auto_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[0]));

		cainteoir::mimetypes mime;

		const char *type = mime(text_buffer.get());
		if (strncmp(type, "application/xml", 15))
			throw std::runtime_error("file is not an XML document");

		xml::document doc(text_buffer.get());
		xml::node root = doc.root();

		rdf::model metadata;
		const rdf::uri subject = rdf::uri(argv[0], std::string());

		if (root == rdf::opf("package"))
			cainteoir::parseOpfDocument(root, subject, metadata);

		if (!metadata.empty())
		{
			(*rdf::create_formatter(std::cout, rdf::formatter::turtle))
				.add_namespace("rdf",  "http://www.w3.org/1999/02/22-rdf-syntax-ns#")
				.add_namespace("rdfs", "http://www.w3.org/2000/01/rdf-schema#")
				.add_namespace("xsd",  "http://www.w3.org/2001/XMLSchema#")
				.add_namespace("dc",   "http://purl.org/dc/elements/1.1/")
				.add_namespace("foaf", "http://xmlns.com/foaf/0.1/")
				.add_namespace("tts",  "http://rhdunn.github.com/2010/12/text-to-speech#")
				<< metadata;
		}
	}
	catch (std::exception &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	xmlCleanupParser();
	return 0;
}
