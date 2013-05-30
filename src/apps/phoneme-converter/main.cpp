/* Convert phonemes from one transcription scheme to another.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#include <cainteoir/phoneme.hpp>
#include <getopt.h>

namespace tts = cainteoir::tts;

static struct option options[] =
{
	{ "help", no_argument, 0, 'h' },
	{ "separate", no_argument, 0, 's' },
	{ 0, 0, 0, 0 }
};

void help()
{
	fprintf(stdout, i18n("usage: phoneme-converter [OPTION..] <from-scheme> <to-scheme> document\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, i18n("Options:\n"));
	fprintf(stdout, i18n(" -h, --help             This help text\n"));
	fprintf(stdout, i18n(" -s, --separate         Display each phoneme on a new line\n"));
	fprintf(stdout, "\n");
	fprintf(stdout, i18n("Report bugs to msclrhd@gmail.com\n"));
}

int main(int argc, char ** argv)
{
	try
	{
		bool separate_phonemes = false;

		while (1)
		{
			int option_index = 0;
			int c = getopt_long(argc, argv, "hs", options, &option_index);
			if (c == -1)
				break;

			switch (c)
			{
			case 'h':
				help();
				return 0;
			case 's':
				separate_phonemes = true;
				break;
			}
		}

		argc -= optind;
		argv += optind;

		if (argc != 3)
		{
			help();
			return 0;
		}

		auto from = tts::createPhonemeReader(argv[0]);
		auto to   = tts::createPhonemeWriter(argv[1]);

		from->reset(cainteoir::make_file_buffer(argv[2]));
		to->reset(stdout);
		while (from->read())
		{
			to->write(*from);
			if (separate_phonemes) fputc('\n', stdout);
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
