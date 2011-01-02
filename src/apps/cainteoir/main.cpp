/* Cainteoir Command-Line Application.
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

#include <cainteoir/tts_engine.hpp>
#include <cainteoir/document.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <memory>
#include <getopt.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

namespace rdf = cainteoir::rdf;

enum args
{
	ARG_STDOUT = 300,
	ARG_METADATA = 301,
};

static struct option options[] =
{
	{ "output",   required_argument, 0, 'o' },
	{ "stdout",   no_argument,       0, ARG_STDOUT },
	{ "type",     required_argument, 0, 't' },
	{ "voice",    required_argument, 0, 'v' },
	{ "metadata", no_argument,       0, ARG_METADATA },
	{ 0, 0, 0, 0 }
};

int main(int argc, char ** argv)
{
	LIBXML_TEST_VERSION

	try
	{
		enum
		{
			speak,
			show_metadata,
		} action = speak;

		const char *voice = NULL;
		const char *outfile = NULL;
		const char *outformat = "wave";
		std::auto_ptr<cainteoir::buffer> text_buffer;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "o:t:v:", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'v':
				voice = optarg;
				break;
			case 'o':
				outfile = optarg;
				break;
			case 't':
				outformat = optarg;
				break;
			case ARG_STDOUT:
				outfile = "-";
				break;
			case ARG_METADATA:
				action = show_metadata;
				break;
			}
		}

		argc -= optind;
		argv += optind;

		std::auto_ptr<cainteoir::tts_engine> tts = cainteoir::create_espeak_engine();
		if (!tts->set_voice_by_name(voice))
			throw std::runtime_error("unrecognised voice");

		if (action == show_metadata)
		{
			(*rdf::create_formatter(std::cout, rdf::formatter::turtle))
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
				<< rdf::skos
				<< rdf::foaf
				<< rdf::tts
				<< tts->get_metadata();
			return 0;
		}

		if (argc != 1)
			throw std::runtime_error("no document specified");

		rdf::model metadata;
		cainteoir::parseDocument(argv[0], metadata);

		const rdf::uri subject = rdf::uri(argv[0], std::string());

		text_buffer = std::auto_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[0]));

		cainteoir::audio_format audioformat = tts->get_audioformat();
		int channels = tts->get_channels();
		int frequency = tts->get_frequency();

		std::auto_ptr<cainteoir::audio> out;
		if (outfile)
		{
			if (!strcmp(outfile, "-"))
				outfile = NULL;

			if (!strcmp(outformat, "wave"))
				out = cainteoir::create_wav_file(outfile, audioformat, channels, frequency);
			else if (!strcmp(outformat, "ogg"))
				out = cainteoir::create_ogg_file(outfile, audioformat, channels, frequency, 0.3, metadata, subject);

			if (!out.get())
				throw std::runtime_error("unsupported audio file format");
		}
		else
			out = cainteoir::create_pulseaudio_device(NULL, audioformat, channels, frequency);

		tts->speak(text_buffer.get(), out.get());
	}
	catch (std::exception &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	xmlCleanupParser();
	return 0;
}
