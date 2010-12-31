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
#include <cainteoir/audio.hpp>
#include <cainteoir/mimetypes.hpp>
#include <cainteoir/parsers.hpp>
#include <iostream>
#include <cstdio>
#include <getopt.h>

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xmldom;

enum args
{
	ARG_NTRIPLE = 300,
	ARG_TURTLE = 301,
	ARG_VORBIS_COMMENTS = 302,
};

static struct option options[] =
{
	{ "ntriple", no_argument, 0, ARG_NTRIPLE },
	{ "turtle",  no_argument, 0, ARG_TURTLE },
	{ "vorbis",  no_argument, 0, ARG_VORBIS_COMMENTS },
	{ 0, 0, 0, 0 }
};

int main(int argc, char ** argv)
{
	LIBXML_TEST_VERSION

	try
	{
		enum
		{
			rdf_metadata,
			vorbis_comments,
		} output_type = rdf_metadata;

		rdf::formatter::format_type format = rdf::formatter::ntriple;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case ARG_NTRIPLE:
				output_type = rdf_metadata;
				format = rdf::formatter::ntriple;
				break;
			case ARG_TURTLE:
				output_type = rdf_metadata;
				format = rdf::formatter::turtle;
				break;
			case ARG_VORBIS_COMMENTS:
				output_type = vorbis_comments;
				break;
			}
		}

		argc -= optind;
		argv += optind;

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
			if (output_type == rdf_metadata)
			{
				(*rdf::create_formatter(std::cout, format))
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
			else
			{
				const rdf::uri subject = rdf::uri(argv[0], std::string());

				std::list<cainteoir::vorbis_comment> comments = cainteoir::vorbis_comments(metadata, subject);
				for (std::list<cainteoir::vorbis_comment>::const_iterator comment = comments.begin(), last = comments.end(); comment != last; ++comment)
					std::cout << comment->label << "=" << comment->value << std::endl;
			}
		}
	}
	catch (std::exception &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	xmlCleanupParser();
	return 0;
}
