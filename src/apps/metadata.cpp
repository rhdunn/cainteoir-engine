/* Metadata Extractor.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>

namespace rdf = cainteoir::rdf;

int main(int argc, char ** argv)
{
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
		bool print_time = false;

		const option_group general_options = { nullptr, {
			{ 't', "time", no_argument, nullptr,
			  i18n("Time how long it takes to extract the metadata"),
			  [&print_time](const char *) { print_time = true; }},
		}};

		const option_group format_options = { i18n("Metadata Format:"), {
			{ 0, "ntriple", no_argument, nullptr,
			  i18n("Output RDF N-Triple metadata statements"),
			  [&output_type, &format](const char *) {
				output_type = rdf_metadata;
				format = rdf::formatter::ntriple;
			  }},
			{ 0, "turtle", no_argument, nullptr,
			  i18n("Output RDF Turtle metadata statements"),
			  [&output_type, &format](const char *) {
				output_type = rdf_metadata;
				format = rdf::formatter::turtle;
			  }},
			{ 0, "vorbis", no_argument, nullptr,
			  i18n("Output VorbisComment entries"),
			  [&output_type](const char *) {
				output_type = vorbis_comments;
			  }},
		}};

		const std::initializer_list<option_group> options = {
			general_options,
			format_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("metadata [OPTION..] DOCUMENT.."),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		cainteoir::stopwatch timer;
		rdf::graph metadata;

		if (argc == 0)
			cainteoir::createDocumentReader(nullptr, metadata, std::string());
		else for(int i = 0; i < argc; ++i)
		{
			auto reader = cainteoir::createDocumentReader(argv[i], metadata, std::string());
			if (!reader)
				fprintf(stderr, i18n("unsupported document format for file \"%s\"\n"), argv[i]);
		}

		if (print_time)
			fprintf(stderr, "Extraction Time: %G\n", timer.elapsed());

		if (!metadata.empty())
		{
			if (output_type == rdf_metadata)
			{
				(*rdf::create_formatter(std::cout, format))
					<< rdf::rdf
					<< rdf::rdfa
					<< rdf::rdfs
					<< rdf::xsd
					<< rdf::xml
					<< rdf::owl
					<< rdf::dc
					<< rdf::dcterms
					<< rdf::dcam
					<< rdf::epub
					<< rdf::opf
					<< rdf::ocf
					<< rdf::pkg
					<< rdf::media
					<< rdf::onix
					<< rdf::marc
					<< rdf::ncx
					<< rdf::dtb
					<< rdf::smil
					<< rdf::xhtml
					<< rdf::skos
					<< rdf::foaf
					<< rdf::tts
					<< rdf::iana
					<< rdf::subtag
					<< metadata;
			}
			else
			{
				const rdf::uri subject = rdf::uri(argv[0], std::string());
				for (auto &comment : cainteoir::vorbis_comments(metadata, subject))
					std::cout << comment.label << "=" << comment.value << std::endl;
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
