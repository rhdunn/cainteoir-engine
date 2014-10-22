/* Window functions.
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

#include <cainteoir/audio.hpp>
#include <cmath>

std::vector<float>
cainteoir::cosine_window(float aAlpha, float aBeta, uint32_t aWindowSize)
{
	std::vector<float> window;
	window.resize(aWindowSize);

	float theta = 2 * M_PI / (aWindowSize - 1);
	uint32_t n = 0;
	for (auto &value : window)
	{
		value = aAlpha - (aBeta * std::cos(theta * n));
		++n;
	}
	return window;
}

std::vector<float>
cainteoir::window(const char *aName, uint32_t aWindowSize)
{
	if (!aName) return {};

	if (strcmp(aName, "hanning") == 0 || strcmp(aName, "hann") == 0)
		return cosine_window(0.5, 0.5, aWindowSize);
	else if (strcmp(aName, "hanning0") == 0 || strcmp(aName, "hann0") == 0)
		return cosine_window(0.5, -0.5, aWindowSize);
	else if (strcmp(aName, "hamming") == 0)
		return cosine_window(0.54, 0.46, aWindowSize);
	else if (strcmp(aName, "hamming0") == 0)
		return cosine_window(0.54, -0.46, aWindowSize);

	return {};
}
