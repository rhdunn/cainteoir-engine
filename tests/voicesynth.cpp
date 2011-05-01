/* Voice Synthesiser -- test the audio generation engine
 *
 * Copyright (C) 2011 Reece H. Dunn
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
#include <cainteoir/metadata.hpp>
#include <cstdio>
#include <cmath>
#include <stdexcept>

namespace rdf = cainteoir::rdf;

int main(int argc, char ** argv)
{
	try
	{
		float A = 5;      // amplitude -- a.k.a. volume
		float f = 261.63; // frequency (cycles per second) -- middle C (C4)
		long  R = 44100;  // sample rate (samples per second)
		long  s = 3;      // length of the sample in seconds

		/* NOTE: The PulseAudio API produces a click at the boundary of each
		 * frame if the frame length is set to 1 second and that 3 frames are
		 * written to.
		 */

		float * frame = new float[s*R]; // 1 second of audio

		for (long i = 0; i < s*R; ++i)
			frame[i] = A * std::sin((2 * M_PI * i * f)/R);

		cainteoir::rdf::graph metadata;
		cainteoir::rdf::uri subject = cainteoir::rdf::uri(std::string(), std::string());
		cainteoir::rdf::uri audioformat = cainteoir::rdf::uri(std::string(), std::string("audio"));

		metadata.push_back(rdf::statement(audioformat, rdf::tts("channels"), rdf::literal(1, rdf::xsd("int"))));
		metadata.push_back(rdf::statement(audioformat, rdf::tts("frequency"), rdf::literal(R, rdf::tts("hertz"))));
		metadata.push_back(rdf::statement(audioformat, rdf::tts("audio-format"), rdf::tts("float32le")));

		std::shared_ptr<cainteoir::audio> audio = cainteoir::open_audio_device(NULL, "pulse", 0.3, metadata, subject, audioformat);
		audio->open();
		audio->write((const char *)frame, sizeof(float) * s*R);
		audio->close();

		delete [] frame;
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
