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

#include <cainteoir/audio.hpp>
#include <cstdint>
#include <cstdio>
#include <cmath>

namespace rdf = cainteoir::rdf;

int main()
{
	try
	{
		uint16_t sample_rate = 44100;
		float frequency = 440.0; // A4
		float duration = 1.0; // 1 second

		rdf::graph metadata;
		rdf::uri doc;
		auto out = cainteoir::open_audio_device(nullptr, metadata, doc, rdf::tts("float32le"), 1, sample_rate);
		out->open();

		float sample_duration = 1.0 / sample_rate;
		float t = 0.0;
		while (true)
		{
			float a = sin(2 * M_PI * frequency * t);
			out->write((const char *)&a, sizeof(a));

			if (t > duration) break;
			t += sample_duration;
		}

		out->close();
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}
	return 0;
}
