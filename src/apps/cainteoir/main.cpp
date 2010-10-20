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
#include <cstdio>
#include <memory>
#include <getopt.h>

enum args
{
	ARG_STDOUT = 300,
};

static struct option options[] =
{
	{ "output", required_argument, 0, 'o' },
	{ "stdout", no_argument,       0, ARG_STDOUT },
	{ "type",   required_argument, 0, 't' },
	{ "voice",  required_argument, 0, 'v' },
	{ 0, 0, 0, 0 }
};

int main(int argc, char ** argv)
{
	try
	{
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
			}
		}

		argc -= optind;
		argv += optind;

		if (argc != 1)
		{
			fprintf(stderr, "error: no document specified\n");
			return 0;
		}

		text_buffer = std::auto_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[0]));

		std::auto_ptr<cainteoir::tts_engine> tts = cainteoir::create_espeak_engine();
		if (!tts->set_voice_by_name(voice))
		{
			fprintf(stderr, "error: unrecognised voice %s\n", voice);
			return 0;
		}

		std::auto_ptr<cainteoir::audio> out;
		if (outfile)
		{
			if (!strcmp(outfile, "-"))
				outfile = NULL;

			if (!strcmp(outformat, "wave"))
				out = cainteoir::create_wav_file(outfile, tts.get());
			else if (!strcmp(outformat, "ogg"))
				out = cainteoir::create_ogg_file(outfile, tts.get(), 0.3);

			if (!out.get())
			{
				fprintf(stderr, "error: unsupported audio file format\n");
				return 0;
			}
		}
		else
			out = cainteoir::create_pulseaudio_device(NULL, tts.get());

		fprintf(stderr, "... using text-to-speech engine %s (%s %s %s)\n",
			tts->get_metadata(cainteoir::title),
			tts->get_metadata(cainteoir::frequency),
			tts->get_metadata(cainteoir::audio_format),
			tts->get_metadata(cainteoir::channels));

		tts->speak(text_buffer.get(), out.get());
	}
	catch (std::exception &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
