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

struct usage_exception : public std::runtime_error
{
	usage_exception() : std::runtime_error("usage error") {}
};

static cainteoir::audio_data<short>
read_audio(const char *filename,
           const char *start_time,
           const char *end_time,
           int channel,
           int frequency,
           const char *target_amplitude)
{
	css::time start = css::parse_smil_time(start_time);
	css::time end   = css::parse_smil_time(end_time);

	auto audio = cainteoir::make_file_buffer(filename);
	auto data  = cainteoir::read_s16_samples(audio, start, end, channel, frequency);

	if (target_amplitude)
	{
		auto range = cainteoir::minmax(data.samples);
		auto scale = cainteoir::scale_factor(target_amplitude, range);
		for (auto &sample : data.samples)
			sample *= scale;
	}
}

int main(int argc, char ** argv)
{
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	const char *start_time  = nullptr;
	const char *end_time    = nullptr;
	int frequency = -1;
	int channel = 0;
	const char *target_amplitude = nullptr;

	const option_group general_options = { nullptr, {
		{ 's', "start", start_time, "TIME",
		  i18n("The time to start processing from") },
		{ 'e', "end", end_time, "TIME",
		  i18n("The time to end processing at") },
		{ 'c', "channel", channel, "CHANNEL",
		  i18n("The desired audio channel to use") },
		{ 'a', "amplitude", target_amplitude, "AMPLITUDE",
		  i18n("The desired maximum amplitude (from 0.0 to 1.0)") },
		{ 'f', "frequency", frequency, "FREQUENCY",
		  i18n("The desired audio frequency") },
	}};

	const std::initializer_list<option_group> options = {
		general_options,
	};

	const std::initializer_list<const char *> usage = {
		i18n("sigproc [OPTION..] convert AUDIO_FILE OUTPUT_FILE"),
		i18n("sigproc [OPTION..] info AUDIO_FILE"),
	};

	try
	{
		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		if (strcmp(argv[0], "convert") == 0)
		{
			if (argc != 3)
				throw usage_exception();

			auto data = read_audio(argv[1], start_time, end_time, channel, frequency, target_amplitude);

			rdf::graph metadata;
			rdf::uri   subject(argv[1], std::string());
			auto out = cainteoir::create_audio_file(argv[2], "wav", 0.3, metadata, subject,
				data.info->format(),
				1,
				data.info->frequency());
			if (!out.get())
				throw std::runtime_error(i18n("unsupported audio file format"));

			out->open();
			out->write((const char *)&data.samples[0], data.samples.size() * 2);
			out->close();
		}
		else if (strcmp(argv[0], "info") == 0)
		{
			if (argc != 3)
				throw usage_exception();

			auto data = read_audio(argv[1], start_time, end_time, channel, frequency, target_amplitude);

			auto range = cainteoir::minmax(data.samples);
			float time = (float)data.samples.size() / data.info->frequency();
			fprintf(stdout, "frequency : %d\n",  data.info->frequency());
			fprintf(stdout, "samples   : %zd\n", data.samples.size());
			fprintf(stdout, "duration  : %G\n",  time);
			fprintf(stdout, "minimum   : %d (%.2f%%)\n",  *range.begin(),
				((float)std::abs(*range.begin()) * 100) / std::numeric_limits<short>::max());
			fprintf(stdout, "maximum   : %d (%.2f%%)\n",  *range.end(),
				((float)std::abs(*range.end()) * 100) / std::numeric_limits<short>::max());
		}
		else
			throw usage_exception();
	}
	catch (usage_exception &e)
	{
		print_help(options, usage);
		return EXIT_FAILURE;
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
