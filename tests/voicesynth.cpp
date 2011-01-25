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

// NOTE: Use `aplay -c 1 -f FLOAT_LE -r 44100` to hear the audio.

#include <cstdio>
#include <cmath>
#include <stdexcept>

int main(int argc, char ** argv)
{
	try
	{
		float A = 5;      // amplitude -- a.k.a. volume
		float f = 261.63; // frequency (cycles per second) -- middle C (C4)
		long  R = 44100;  // sample rate (samples per second)
		long  s = 3;      // length of the sample in seconds

		for (long i = 0; i < s*R; ++i)
		{
			float res = A * std::sin((2 * M_PI * i * f)/R);
			fwrite(&res, sizeof(res), 1, stdout);
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
