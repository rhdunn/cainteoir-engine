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

#include <config.h>
#include <cainteoir/metadata.hpp>
#include <cainteoir/audio.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/platform.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <getopt.h>

namespace rdf = cainteoir::rdf;

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
	{ "help",    no_argument, 0, 'h' },
	{ 0, 0, 0, 0 }
};

void help()
{
	fprintf(stdout, _("usage: metadata [OPTION..] document\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("Formats:\n"));
	fprintf(stdout, _(" --ntriple              Output RDF N-Triples statements\n"));
	fprintf(stdout, _(" --turtle               Output RDF Turtle statements\n"));
	fprintf(stdout, _(" --vorbis               Output VorbisComment entries\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("General:\n"));
	fprintf(stdout, _(" -h, --help             This help text\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("Report bugs to msclrhd@gmail.com\n"));
}

struct rdfmetadata
	: public cainteoir::document_events
	, public rdf::graph
{
	void metadata(const rdf::statement &aStatement)
	{
		push_back(aStatement);
	}

	const rdf::bnode genid()
	{
		return rdf::graph::genid();
	}

	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
	}
};

int main(int argc, char ** argv)
{
	cainteoir::initialise();

	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

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
			int c = getopt_long(argc, argv, "h", options, &option_index);
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
			case 'h':
				help();
				return 0;
			}
		}

		argc -= optind;
		argv += optind;

		if (argc != 1)
			throw std::runtime_error(_("no document specified"));

		rdfmetadata metadata;
		if (!cainteoir::parseDocument(argv[0], metadata))
			fprintf(stderr, _("unsupported document format for file \"%s\"\n"), argv[0]);

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
					<< rdf::dcterms
					<< rdf::dcam
					<< rdf::ocf
					<< rdf::opf
					<< rdf::smil
					<< rdf::xhtml
					<< rdf::skos
					<< rdf::foaf
					<< rdf::tts
					<< metadata;
			}
			else
			{
				const rdf::uri subject = rdf::uri(argv[0], std::string());

				foreach_iter(comment, cainteoir::vorbis_comments(metadata, subject))
					std::cout << comment->label << "=" << comment->value << std::endl;
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, _("error: %s\n"), e.what());
	}

	cainteoir::cleanup();
	return 0;
}
