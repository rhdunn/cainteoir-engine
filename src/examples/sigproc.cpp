/* Signal Processing Tools
 *
 * Copyright (C) 2014 Reece H. Dunn
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

#include <cainteoir/buffer.hpp>
#include <cainteoir/audio.hpp>
#include <stdexcept>

int main(int argc, char **argv)
{
	try
	{
		const std::initializer_list<option_group> options = {
		};

		const std::initializer_list<const char *> usage = {
			i18n("sigproc [OPTION..] AUDIO_FILE"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		if (argc == 0)
		{
			print_help(options, usage);
			return 0;
		}

		auto audio = cainteoir::create_media_reader(cainteoir::make_file_buffer(argv[0]));

		fprintf(stdout, "channels    : %d\n", audio->channels());
		fprintf(stdout, "format      : %s\n", audio->format().str().c_str());
		fprintf(stdout, "sample rate : %d\n", audio->frequency());

		audio->set_target(audio);

		int frame = 1;
		while (audio->read())
		{
			fprintf(stdout, "frame #%d : size=%zu\n", frame, audio->data.size());
			++frame;
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
