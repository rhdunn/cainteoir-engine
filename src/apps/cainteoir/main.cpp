/* Cainteoir Command-Line Application.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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
#include <cainteoir/engines.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/platform.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <getopt.h>
#include <cmath>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

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
	{ "record",   required_argument, 0, 'r' },
	{ "voice",    required_argument, 0, 'v' },
	{ "language", required_argument, 0, 'l' },
	{ "metadata", no_argument,       0, ARG_METADATA },
	{ "help",     no_argument,       0, 'h' },
	{ 0, 0, 0, 0 }
};

void list_formats(const rdf::graph &aMetadata, const rdf::uri &aType, bool showName)
{
	rql::results formats = rql::select(aMetadata,
		rql::both(rql::matches(rql::predicate, rdf::rdf("type")),
		          rql::matches(rql::object, aType)));

	foreach_iter(format, formats)
	{
		rql::results data = rql::select(aMetadata, rql::matches(rql::subject, rql::subject(*format)));
		std::string description = rql::select_value<std::string>(data, rql::matches(rql::predicate, rdf::dc("description")));
		if (showName)
		{
			std::string name = rql::select_value<std::string>(data, rql::matches(rql::predicate, rdf::tts("name")));
			fprintf(stdout, "            %-5s - %s\n", name.c_str(), description.c_str());
		}
		else
			fprintf(stdout, "          *  %s\n", description.c_str());
	}
}

void help(const rdf::graph &aMetadata)
{
	fprintf(stdout, _("usage: cainteoir [OPTION..] DOCUMENT\n"));
	fprintf(stdout, _("       where DOCUMENT is one of:\n"));
	list_formats(aMetadata, rdf::tts("DocumentFormat"), false);
	fprintf(stdout, "\n");
	fprintf(stdout, _("Speech options:\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _(" -v, --voice=VOICE      Use the voice named VOICE\n"));
	fprintf(stdout, _(" -l, --language=LANG    Use a voice that speaks the language LANG\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("Recording audio options:\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _(" -o, --output=FILE      Recorded audio is written to FILE\n"));
	fprintf(stdout, _("     --stdout           Recorded audio is written to the standard output\n"));
	fprintf(stdout, _(" -r, --record=FORMAT    Record the audio as a FORMAT file (default: wav)\n"));
	fprintf(stdout, _("       where FORMAT is one of:\n"));
	list_formats(aMetadata, rdf::tts("AudioFormat"), true);
	fprintf(stdout, "\n");
	fprintf(stdout, _("General options:\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("     --metadata         Show the RDF metadata for the engine and voices\n"));
	fprintf(stdout, _(" -h, --help             This help text\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("The arguments to the long options also apply to their short option equivalents.\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, _("Report bugs to msclrhd@gmail.com\n"));
}

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
	int ms = int(seconds * 10.0) % 10;

	int minutes = floor(seconds / 60.0);
	seconds = seconds - (minutes * 60.0);

	int hours = floor(minutes / 60.0);
	minutes = minutes - (hours * 60.0);

	snprintf(s, n, "%02d:%02d:%02d.%01d", hours, minutes, (int)floor(seconds), ms);
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

const rdf::uri *select_voice(const rdf::graph &aMetadata, const rdf::uri &predicate, const std::string &value)
{
	rql::results voices = rql::select(aMetadata,
		rql::both(rql::matches(rql::predicate, rdf::rdf("type")),
		          rql::matches(rql::object, rdf::tts("Voice"))));

	foreach_iter(voice, voices)
	{
		const rdf::uri *uri = rql::subject(*voice);
		if (uri)
		{
			rql::results statements = rql::select(aMetadata, rql::matches(rql::subject, *uri));
			foreach_iter(statement, statements)
			{
				if (rql::predicate(*statement) == predicate && rql::value(*statement) == value)
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
		, voiceSelected(false)
		, m_doc(new cainteoir::document())
	{
	}

	void metadata(const std::tr1::shared_ptr<const rdf::triple> &aStatement)
	{
		m_metadata.push_back(aStatement);

		if (rql::subject(aStatement) == subject && rql::predicate(aStatement) == rdf::dc("language"))
			select_voice(rdf::dc("language"), rql::value(aStatement));
	}

	const rdf::bnode genid()
	{
		return m_metadata.genid();
	}

	void text(std::tr1::shared_ptr<cainteoir::buffer> aText)
	{
		m_doc->add(aText);
	}

	void select_voice(const rdf::uri &predicate, const std::string &value)
	{
		if (voiceSelected)
			return;

		const rdf::uri *voice = ::select_voice(m_metadata, predicate, value);
		if (voice)
			voiceSelected = tts.select_voice(m_metadata, *voice);
	}

	const rdf::uri subject;
	rdf::graph m_metadata;
	std::tr1::shared_ptr<cainteoir::document> m_doc;
	cainteoir::tts::engines tts;
	bool voiceSelected;
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
			speak,
			show_metadata,
			show_help,
		} action = speak;

		const char *voicename = NULL;
		const char *language = NULL;
		const char *outfile = NULL;
		const char *outformat = NULL;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "hl:o:r:v:", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'h':
				action = show_help;
				break;
			case 'l':
				language = optarg;
				break;
			case 'v':
				voicename = optarg;
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'r':
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
				<< rdf::ncx
				<< rdf::dtb
				<< rdf::smil
				<< rdf::xhtml
				<< rdf::skos
				<< rdf::foaf
				<< rdf::tts
				<< doc.m_metadata;
			return 0;
		}
		else if (action == show_help)
		{
			help(doc.m_metadata);
			return 0;
		}

		if (voicename)
			doc.select_voice(rdf::tts("name"), voicename);

		if (language)
			doc.select_voice(rdf::dc("language"), language);

		if (argc == 1)
			cainteoir::parseDocument(argv[0], doc);
		else
			cainteoir::parseDocument(NULL, doc);

		std::string author;
		std::string title;

		foreach_iter (query, rql::select(doc.m_metadata, rql::matches(rql::subject, doc.subject)))
		{
			if (rql::predicate(*query).as<rdf::uri>()->ns == rdf::dc || rql::predicate(*query).as<rdf::uri>()->ns == rdf::dcterms)
			{
				rdf::any_type object = rql::object(*query);
				if (object.as<rdf::literal>())
				{
					if (rql::predicate(*query).as<rdf::uri>()->ref == "title")
						title = rql::value(object);
					else if (rql::predicate(*query).as<rdf::uri>()->ref == "creator")
						author = rql::value(object);
				}
				else
				{
					rql::results selection = rql::select(doc.m_metadata, rql::matches(rql::subject, object));

					if (rql::predicate(*query).as<rdf::uri>()->ref == "creator")
					{
						std::string role;
						std::string value;

						for(auto data = selection.begin(), last = selection.end(); data != last; ++data)
						{
							const std::string &object = rql::value(*data);
							if (rql::predicate(*data) == rdf::rdf("value"))
								value = object;
							else if (rql::predicate(*data) == rdf::opf("role") || rql::predicate(*data) == rdf::pkg("role"))
								role = object;
						}

						if (!value.empty() && (role == "aut" || role.empty()))
							author = value;
					}
					else if (rql::predicate(*query).as<rdf::uri>()->ref == "title")
					{
						for(auto data = selection.begin(), last = selection.end(); data != last; ++data)
						{
							if (rql::predicate(*data) == rdf::rdf("value"))
								title = rql::value(*data);
						}
					}
				}
			}
		}

		std::tr1::shared_ptr<cainteoir::audio> out;
		const char *state;
		bool show_progress = true;
		if (outformat || outfile)
		{
			state = _("recording");
			std::stringstream file;

			if (outfile)
				file << outfile;
			else
				file << author << " - " << title << "." << outformat;

			std::string outfile = file.str();

			if (!outformat)
				outformat = "wav";

			out = cainteoir::create_audio_file(outfile.c_str(), outformat, 0.3, doc.m_metadata, doc.subject, doc.tts.voice());
			if (!out.get())
				throw std::runtime_error(_("unsupported audio file format"));

			if (outfile != "-")
			{
				fprintf(stdout, _("Recording \"%s\"\n"), doc.subject.str().c_str());
				fprintf(stdout, _("       to \"%s\"\n\n"), outfile.c_str());
			}
			else
				show_progress = false;
		}
		else
		{
			state = "reading";
			out = cainteoir::open_audio_device(NULL, "pulse", 0.3, doc.m_metadata, doc.subject, doc.tts.voice());

			fprintf(stdout, _("Reading \"%s\"\n\n"), doc.subject.str().c_str());
		}

		if (show_progress)
		{
			fprintf(stdout, _("Author : %s\n"), author.c_str());
			fprintf(stdout, _("Title  : %s\n\n"), title.c_str());
		}

		std::tr1::shared_ptr<cainteoir::tts::speech> speech = doc.tts.speak(doc.m_doc, out, 0);
		while (speech->is_speaking())
		{
			if (show_progress)
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

		if (show_progress)
		{
			status_line(speech->elapsedTime(), speech->totalTime(), speech->completed(), _("stopped"));
			fprintf(stdout, "\n");
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, _("error: %s\n"), e.what());
	}

	cainteoir::cleanup();
	return 0;
}
