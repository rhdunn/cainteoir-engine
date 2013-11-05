/* Music player.
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

/* This is a demonstration of the Cainteoir Text-to-Speech audio APIs used in
 * the listening and recording of text, and the implementation of audio overlay
 * formats such as ePub 3 Media Overlays and SMIL.
 */

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/audio.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>

namespace css = cainteoir::css;

void print_time(const css::time &time, const char *name)
{
	switch (time.units())
	{
	case css::time::inherit:
		fprintf(stdout, "%s : unspecified\n", name);
		break;
	case css::time::seconds:
		fprintf(stdout, "%s : %Gs\n", name, time.value());
		break;
	case css::time::milliseconds:
		fprintf(stdout, "%s : %Gms\n", name, time.value());
		break;
	}
}

int main(int argc, char ** argv)
{
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	try
	{
		const char *start_time = nullptr;
		const char *end_time   = nullptr;

		const option_group general_options = { nullptr, {
			{ 's', "start", start_time, "TIME",
			  i18n("The time to start playback from") },
			{ 'e', "end", end_time, "TIME",
			  i18n("The time to end playback at") },
		}};

		const std::initializer_list<option_group> options = {
			general_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("musicplayer [OPTION..] AUDIO_FILE"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		css::time start = css::parse_smil_time(start_time);
		css::time end   = css::parse_smil_time(end_time);

		print_time(start, "start time");
		print_time(end,   "end time  ");
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
