/* A voice synthesizer.
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
#include <cstdint>
#include <cstdio>
#include <cmath>

namespace rdf = cainteoir::rdf;

inline float clamp(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

int main(int argc, char **argv)
{
	try
	{
		const char *device_name = nullptr;
		const char *outfile = nullptr;
		const char *outformat = nullptr;

		auto sample_format = rdf::tts("float32le");
		int channels = 1;
		uint16_t sample_rate = 44100;

		float frequency = 440.0; // A4
		float duration = 1.0; // 1 second
		float amplitude = 1.0;

		const option_group general_options = { nullptr, {
			{ 'D', "device", device_name, "DEVICE",
			  i18n("Use DEVICE for audio output (ALSA/pulseaudio device name)") },
		}};

		const option_group sound_options = { i18n("Sound:"), {
			{ 'a', "amplitude", amplitude, "AMPLITUDE",
			  i18n("Set the amplitude (volume) of the sound to AMPLITUDE") },
			{ 'f', "frequency", frequency, "FREQUENCY",
			  i18n("Set the frequency (pitch) of the sound to FREQUENCY") },
			{ 'd', "duration", duration, "DURATION",
			  i18n("Set the duration of the sound to DURATION") },
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
			sound_options,
			recording_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("synthesizer [OPTION..]"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		if (outformat && !strcmp(outformat, "wave"))
			outformat = "wav";

		amplitude = clamp(amplitude, 0.0, 1.0);

		rdf::graph metadata;
		rdf::uri doc;
		std::shared_ptr<cainteoir::audio> out;
		if (outformat || outfile)
			out = cainteoir::create_audio_file(outfile, outformat ? outformat : "wav", 0.3, metadata, doc, sample_format, channels, sample_rate);
		else
			out = cainteoir::open_audio_device(device_name, metadata, doc, sample_format, channels, sample_rate);
		out->open();

		float sample_duration = 1.0 / sample_rate;
		float t = 0.0;
		for (uint32_t sample : cainteoir::range<uint32_t>(0, sample_rate * duration))
		{
			float a = sin(2 * M_PI * t);
			a *= amplitude;
			out->write((const char *)&a, sizeof(a));

			t += sample_duration * frequency;
		}

		out->close();
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}
	return 0;
}
