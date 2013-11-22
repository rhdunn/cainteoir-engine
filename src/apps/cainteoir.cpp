/* Cainteoir Command-Line Application.
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

#include <cainteoir/engines.hpp>
#include <cainteoir/document.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cmath>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <limits.h>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

enum actions
{
	speak,
	show_metadata,
	show_contents,
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

int main(int argc, char ** argv)
{
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	try
	{
		actions action = speak;
		bool use_narrator = false;
		bool use_tts_fallback = false;
		bool show_progress = true;

		const char *voicename = nullptr;
		const char *language = nullptr;
		const char *outfile = nullptr;
		const char *outformat = nullptr;

		int speed = INT_MAX;
		int pitch = INT_MAX;
		int range = INT_MAX;
		int volume = INT_MAX;

		std::pair<size_t, size_t> toc_range = { -1, -1 };

		const option_group general_options = { nullptr, {
			{ 'M', "metadata", bind_value(action, show_metadata),
			  i18n("Show the RDF metadata for the engine and voices") },
			{ 'q', "quiet", bind_value(show_progress, false),
			  i18n("Do not print any output (including current playing/recording time)") },
		}};

		const option_group speech_options = { i18n("Speech:"), {
			{ 'v', "voice", voicename, "VOICE",
			  i18n("Use the voice named VOICE") },
			{ 'l', "language", language, "LANG",
			  i18n("Use a voice that speaks the language LANG") },
			{ 's', "speed", speed, "SPEED",
			  i18n("Set the voice's speed to SPEED words per minute") },
			{ 'p', "pitch", pitch, "PITCH",
			  i18n("Set the voice's base pitch to PITCH") },
			{ 'P', "pitch-range", range, "RANGE",
			  i18n("Set the voice's pitch to vary by RANGE") },
			{ 'V', "volume", volume, "VOLUME",
			  i18n("Set the voice's volume to VOLUME percent") },
			{ 'm', "monotone", bind_value(range, 0),
			  i18n("Set the voice to monotone (pitch varies by 0)") },
		}};

		const option_group narration_options = { i18n("Narration:"), {
			{ 0, "narrator", bind_value(use_narrator, true),
			  i18n("Use any narration/embedded audio in the document") },
			{ 0, "tts-fallback", bind_value(use_tts_fallback, true),
			  i18n("Use the TTS voice if no narration is provided for text") },
		}};

		const option_group toc_options = { i18n("Table of Contents:"), {
			{ 'c', "contents", bind_value(action, show_contents),
			  i18n("List the table of contents for the specified document") },
			{ 'f', "from", toc_range.first, "FROM",
			  i18n("Start reading/recoding from contents marker FROM") },
			{ 't', "to", toc_range.second, "TO",
			  i18n("Finish reading/recording after contents marker TO") },
		}};

		const option_group recording_options = { i18n("Recording:"), {
			{ 'o', "output", outfile, "FILE",
			  i18n("Recorded audio is written to FILE") },
			{ 0, "stdout", bind_value(outfile, "-"),
			  i18n("Recorded audio is written to the standard output") },
			{ 'r', "record", outformat, "FORMAT",
			  i18n("Record the audio as a FORMAT file (default: wav)") },
		}};

		const std::initializer_list<option_group> options = {
			general_options,
			speech_options,
			narration_options,
			toc_options,
			recording_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("cainteoir [OPTION..] DOCUMENT"),
			i18n("cainteoir [OPTION..]"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		if (toc_range.second != -1) ++toc_range.second;
		if (outformat && !strcmp(outformat, "wave"))
			outformat = "wav";

		auto mode = tts::media_overlays_mode::tts_only;
		if (use_narrator)
			mode = use_tts_fallback
			     ? tts::media_overlays_mode::tts_and_media_overlays
			     : tts::media_overlays_mode::media_overlays_only;

		rdf::graph metadata;
		cainteoir::supportedDocumentFormats(metadata, cainteoir::text_support);
		cainteoir::supportedAudioFormats(metadata);

		cainteoir::tts::engines tts(metadata);

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
				<< rdf::iana
				<< rdf::subtag
				<< metadata;
			return 0;
		}

		if (voicename)
		{
			const rdf::uri *ref = tts::get_voice_uri(metadata, rdf::tts("name"), voicename);
			if (ref)
				tts.select_voice(metadata, *ref);
		}
		else if (language)
		{
			const rdf::uri *ref = tts::get_voice_uri(metadata, rdf::dc("language"), language);
			if (ref)
				tts.select_voice(metadata, *ref);
		}

		if (speed  != INT_MAX) tts.parameter(tts::parameter::rate)->set_value(speed);
		if (pitch  != INT_MAX) tts.parameter(tts::parameter::pitch)->set_value(pitch);
		if (range  != INT_MAX) tts.parameter(tts::parameter::pitch_range)->set_value(range);
		if (volume != INT_MAX) tts.parameter(tts::parameter::volume)->set_value(volume);

		const char *filename = (argc == 1) ? argv[0] : nullptr;
		auto reader = cainteoir::createDocumentReader(filename, metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, i18n("unsupported document format for file \"%s\"\n"), filename ? filename : "<stdin>");
			return EXIT_FAILURE;
		}

		cainteoir::document doc(reader);
		if (action == show_contents)
		{
			int toc_number = 1;
			for (auto &entry : doc.toc())
			{
				printf(" %4d ", toc_number);
				for (int i = 0; i < entry.depth; ++i)
					printf("... ");
				printf("%s\n", entry.title.c_str());
				++toc_number;
			}
			return 0;
		}

		std::string author;
		std::string title;
		rdf::uri subject(filename ? filename : std::string(), std::string());

		for (auto &query : rql::select(metadata, rql::subject == subject))
		{
			if (rql::predicate(query).ns == rdf::dc || rql::predicate(query).ns == rdf::dcterms)
			{
				const rdf::uri &object = rql::object(query);
				if (object.empty())
				{
					if (rql::predicate(query).ref == "title")
						title = rql::value(query);
					else if (rql::predicate(query).ref == "creator")
						author = rql::value(query);
				}
				else
				{
					rql::results selection = rql::select(metadata, rql::subject == object);

					if (rql::predicate(query).ref == "creator")
					{
						std::string role;
						std::string value;

						for (auto &data : selection)
						{
							const std::string &object = rql::value(data);
							if (rql::predicate(data) == rdf::rdf("value"))
								value = object;
							else if (rql::predicate(data) == rdf::opf("role") || rql::predicate(data) == rdf::pkg("role"))
								role = object;
						}

						if (!value.empty() && (role == "aut" || role.empty()))
							author = value;
					}
					else if (rql::predicate(query).ref == "title")
					{
						for (auto &data : selection)
						{
							if (rql::predicate(data) == rdf::rdf("value"))
								title = rql::value(data);
						}
					}
				}
			}
		}

		std::shared_ptr<cainteoir::audio> out;
		const char *state;
		if (outformat || outfile)
		{
			state = i18n("recording");
			std::stringstream file;

			if (outfile)
				file << outfile;
			else
				file << author << " - " << title << "." << outformat;

			std::string outfile = file.str();

			if (!outformat)
				outformat = "wav";

			out = cainteoir::create_audio_file(outfile.c_str(), outformat, 0.3, metadata, subject, metadata, tts.voice());
			if (!out.get())
				throw std::runtime_error(i18n("unsupported audio file format"));

			if (outfile != "-")
			{
				fprintf(stdout, i18n("Recording \"%s\"\n"),   filename);
				fprintf(stdout, i18n("       to \"%s\"\n\n"), outfile.c_str());
			}
			else
				show_progress = false;
		}
		else
		{
			state = i18n("reading");
			out = cainteoir::open_audio_device(nullptr, metadata, subject, metadata, tts.voice());

			fprintf(stdout, i18n("Reading \"%s\"\n\n"), filename);
		}

		if (show_progress)
		{
			fprintf(stdout, i18n("Author : %s\n"), author.c_str());
			fprintf(stdout, i18n("Title  : %s\n\n"), title.c_str());
		}

		auto speech = tts.speak(out, doc.toc(), doc.children(toc_range), mode);
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
			status_line(speech->elapsedTime(), speech->totalTime(), speech->completed(), i18n("stopped"));
			fprintf(stdout, "\n");
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
