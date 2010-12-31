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

		rdf::model metadata;

		std::string type = cainteoir::mimetypes()(argv[0]);
		if (type == "application/xml")
		{
			const rdf::uri subject = rdf::uri(argv[0], std::string());

			xml::document doc(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[0])));
			xml::node root = doc.root();

			if (root == rdf::opf("package"))
				cainteoir::parseOpfDocument(root, subject, metadata);
			else if (root == rdf::rdf("RDF"))
				cainteoir::parseRdfXmlDocument(root, subject, metadata);
			else if (root == rdf::smil("smil"))
				cainteoir::parseSmilDocument(root, subject, metadata);
		}
		else if (type == "application/epub+zip")
			cainteoir::parseEpubDocument(argv[0], metadata);
		else
			fprintf(stderr, "unsupported document format: %s\n", type.c_str());

		if (!metadata.empty())
		{
			(*rdf::create_formatter(std::cout, rdf::formatter::turtle))
				<< rdf::rdf
				<< rdf::rdfs
				<< rdf::xsd
				<< rdf::owl
				<< rdf::dc
				<< rdf::foaf
				<< rdf::tts
				<< rdf::opf
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
