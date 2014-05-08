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

#include <cstdint>
#include <cstdio>

int main()
{
	uint16_t sample_rate = 44100;
	float duration = 1.0; // 1 second

	float sample_duration = 1.0 / sample_rate;
	float t = 0.0;
	while (true)
	{
		fprintf(stdout, "%G\n", t);

		if (t > duration) break;
		t += sample_duration;
	}

	return 0;
}
