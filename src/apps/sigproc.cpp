/* Signal Processing.
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

#include <cainteoir/audio.hpp>
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace css = cainteoir::css;

int main(int argc, char ** argv)
{
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	try
	{
		const char *start_time  = nullptr;
		const char *end_time    = nullptr;
		int frequency = -1;
		int channel = 0;

		const option_group general_options = { nullptr, {
			{ 's', "start", start_time, "TIME",
			  i18n("The time to start processing from") },
			{ 'e', "end", end_time, "TIME",
			  i18n("The time to end processing at") },
			{ 'c', "channel", channel, "CHANNEL",
			  i18n("The desired audio channel to use") },
			{ 'f', "frequency", frequency, "FREQUENCY",
			  i18n("The desired audio frequency") },
		}};

		const std::initializer_list<option_group> options = {
			general_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("sigproc [OPTION..] info AUDIO_FILE"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		if (argc != 2)
		{
			print_help(options, usage);
			return 0;
		}

		css::time start = css::parse_smil_time(start_time);
		css::time end   = css::parse_smil_time(end_time);

		auto data = cainteoir::make_file_buffer(argv[1]);
		auto player = cainteoir::create_media_reader(data);
		if (!player)
			throw std::runtime_error("unable to read the audio file");

		auto target = cainteoir::create_audio_info(
			rdf::tts("s16le"),
			player->channels(),
			(frequency == -1) ? player->frequency() : frequency);

		std::vector<short> samples;
		player->set_interval(start, end);
		player->set_target(target);
		while (player->read())
		{
			const short *current = (const short *)player->data.begin();
			const short *last    = (const short *)player->data.end();
			int channels = player->channels();
			switch (channels)
			{
			case 1:
				samples.insert(samples.end(), current, last);
				break;
			default:
				{
					uint32_t n = 0;
					for (; current != last; ++current)
					{
						if (n % channels == channel)
							samples.push_back(*current);
						++n;
					}
				}
				break;
			}
		}

		if (strcmp(argv[0], "info") == 0)
		{
			float time = (float)samples.size() / target->frequency();
			fprintf(stdout, "frequency : %d\n",  target->frequency());
			fprintf(stdout, "samples   : %zd\n", samples.size());
			fprintf(stdout, "duration  : %G\n",  time);
		}
		else
			print_help(options, usage);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
