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

#include <cainteoir/engines.hpp>
#include <cainteoir/document.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <memory>
#include <getopt.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

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
	{ "language", required_argument, 0, 'l' },
	{ "metadata", no_argument,       0, ARG_METADATA },
	{ 0, 0, 0, 0 }
};

std::string select_value(const rdf::model &aMetadata, const rdf::uri &uri, const rdf::uri &predicate)
{
	foreach_iter(query, rql::select(aMetadata, rql::subject, uri))
	{
		if (rql::predicate(*query) == predicate)
			return rql::value(rql::object(*query));
	}

	return std::string();
}

const rdf::uri *select_voice(const rdf::model &aMetadata, const rdf::uri &predicate, const std::string &value)
{
	rql::results voices = rql::select(
		rql::select(aMetadata, rql::predicate, rdf::rdf("type")),
		rql::object, rdf::tts("Voice"));

	foreach_iter(voice, voices)
	{
		const rdf::uri *uri = rql::subject(*voice);
		if (uri)
		{
			rql::results statements = rql::select(aMetadata, rql::subject, *uri);
			foreach_iter(statement, statements)
			{
				if (rql::predicate(*statement) == predicate && rql::value(rql::object(*statement)) == value)
					return uri;
			}
		}
	}

	return NULL;
}

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

		const char *voicename = NULL;
		const char *language = NULL;
		const char *outfile = NULL;
		const char *outformat = "wave";
		std::auto_ptr<cainteoir::buffer> text_buffer;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "o:t:v:l:", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'l':
				language = optarg;
				break;
			case 'v':
				voicename = optarg;
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

		rdf::model metadata;

		cainteoir::tts::engines tts(metadata);
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
				<< rdf::xhtml
				<< rdf::skos
				<< rdf::foaf
				<< rdf::tts
				<< metadata;
			return 0;
		}

		if (argc != 1)
			throw std::runtime_error("no document specified");

		std::list<cainteoir::event> events;
		cainteoir::parseDocument(argv[0], metadata, events);

		const rdf::uri subject = rdf::uri(argv[0], std::string());

		const rdf::uri *voice = NULL;
		if (language)
			voice = select_voice(metadata, rdf::dc("language"), language);
		else if (voicename)
			voice = select_voice(metadata, rdf::tts("name"), voicename);
		else
		{
			std::string lang = select_value(metadata, subject, rdf::dc("language"));
			if (!lang.empty())
				voice = select_voice(metadata, rdf::dc("language"), lang);

			if (!voice)
				voice = select_voice(metadata, rdf::tts("name"), "default");
		}

		if (!voice || !tts.select_voice(metadata, *voice))
			throw std::runtime_error("unrecognised voice");

		cainteoir::audio_format audioformat = tts.get_audioformat();
		int channels = tts.get_channels();
		int frequency = tts.get_frequency();

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

		out->open();
		foreach_iter(event, events)
		{
			if (event->type == cainteoir::text_event)
				tts.speak(event->data.get(), out.get());
		}
		out->close();
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	xmlCleanupParser();
	return 0;
}
