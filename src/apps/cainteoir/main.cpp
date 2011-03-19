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
#include <cmath>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

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

int termchar()
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	return ch;
}

void format_time(char *s, int n, double seconds)
{
	int ms = int(seconds * 100.0) % 100;

	double minutes = floor(seconds / 60.0);
	seconds = seconds - (minutes * 60.0);

	double hours = floor(minutes / 60.0);
	minutes = minutes - (hours * 60.0);

	snprintf(s, n, "%02.0f:%02.0f:%02.0f.%02d", hours, minutes, seconds, ms);
}

void status_line(double elapsed, double total, double progress, const char *state)
{
	char elapsed_time[80];
	char total_time[80];

	format_time(elapsed_time, 80, elapsed);
	format_time(total_time, 80, total);

#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
	fprintf(stdout, HIDE_CURSOR " : %s of %s [%.2f%%] : %s        \r" SHOW_CURSOR, elapsed_time, total_time, progress, state);
#undef  SHOW_CURSOR
#undef  HIDE_CURSOR

	fflush(stdout);
}

std::string select_value(const rdf::graph &aMetadata, const rdf::uri &uri, const rdf::uri &predicate)
{
	foreach_iter(query, rql::select(aMetadata, rql::subject, uri))
	{
		if (rql::predicate(*query) == predicate)
			return rql::value(rql::object(*query));
	}

	return std::string();
}

const rdf::uri *select_voice(const rdf::graph &aMetadata, const rdf::uri &predicate, const std::string &value)
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

struct document : public cainteoir::document_events
{
	document(const rdf::uri &aSubject)
		: tts(m_metadata)
		, subject(aSubject)
		, m_doc(new cainteoir::document())
	{
	}

	void metadata(const rdf::statement &aStatement)
	{
		m_metadata.push_back(aStatement);

		if (rql::subject(aStatement) == subject && rql::predicate(aStatement) == rdf::dc("language"))
			select_voice(rdf::dc("language"), rql::value(rql::object(aStatement)));
	}

	const rdf::bnode genid()
	{
		return m_metadata.genid();
	}

	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
		m_doc->add(aText);
	}

	bool select_voice(const rdf::uri &predicate, const std::string &value)
	{
		const rdf::uri *voice = ::select_voice(m_metadata, predicate, value);
		if (!voice)
			return false;
		return tts.select_voice(m_metadata, *voice);
	}

	const rdf::uri subject;
	rdf::graph m_metadata;
	std::tr1::shared_ptr<cainteoir::document> m_doc;
	cainteoir::tts::engines tts;
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

		const char *voicename = NULL;
		const char *language = NULL;
		const char *outfile = NULL;
		const char *outformat = NULL;
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
				if (!strcmp(outformat, "wave"))
					outformat = "wav";
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

		document doc(rdf::uri(argc == 1 ? argv[0] : std::string(), std::string()));
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
				<< doc.m_metadata;
			return 0;
		}

		if (language)
			doc.select_voice(rdf::dc("language"), language);
		else if (voicename)
			doc.select_voice(rdf::tts("name"), voicename);

		if (argc == 1)
			cainteoir::parseDocument(argv[0], doc);
		else
			cainteoir::parseDocument(NULL, doc);

		cainteoir::audio_format audioformat = doc.tts.get_audioformat();
		int channels = doc.tts.get_channels();
		int frequency = doc.tts.get_frequency();

		std::string author = select_value(doc.m_metadata, doc.subject, rdf::dc("creator"));
		std::string title  = select_value(doc.m_metadata, doc.subject, rdf::dc("title"));

		std::auto_ptr<cainteoir::audio> out;
		const char *state;
		if (outformat || outfile)
		{
			state = "recording";
			std::stringstream file;

			if (outfile)
				file << outfile;
			else
				file << author << " - " << title << "." << outformat;

			std::string outfile = file.str();

			if (!outformat || !strcmp(outformat, "wav"))
				out = cainteoir::create_wav_file(outfile == "-" ? NULL : outfile.c_str(), audioformat, channels, frequency);
			else if (!strcmp(outformat, "ogg"))
				out = cainteoir::create_ogg_file(outfile == "-" ? NULL : outfile.c_str(), audioformat, channels, frequency, 0.3, doc.m_metadata, doc.subject);

			if (!out.get())
				throw std::runtime_error("unsupported audio file format");

			if (outfile != "-")
			{
				fprintf(stdout, "Recording \"%s\"\n", doc.subject.str().c_str());
				fprintf(stdout, "       to \"%s\"\n\n", outfile.c_str());
			}
		}
		else
		{
			state = "reading";
			out = cainteoir::create_pulseaudio_device(NULL, audioformat, channels, frequency);

			fprintf(stdout, "Reading \"%s\"\n\n", doc.subject.str().c_str());
		}

		size_t length = doc.m_doc->length();

		fprintf(stdout, "Author : %s\n", author.c_str());
		fprintf(stdout, "Title  : %s\n\n", title.c_str());

		std::auto_ptr<cainteoir::tts::speech> speech = doc.tts.speak(doc.m_doc, out.get(), 0);
		while (speech->is_speaking())
		{
			status_line(speech->elapsedTime(), speech->totalTime(), speech->completed(), state);

			switch (termchar())
			{
			case 'q':
				speech->stop();
				break;
			default:
				usleep(250);
				break;
			}
		}

		status_line(speech->elapsedTime(), speech->totalTime(), speech->completed(), "stopped");
		fprintf(stdout, "\n");
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	xmlCleanupParser();
	return 0;
}
