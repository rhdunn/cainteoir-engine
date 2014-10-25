/* Signal Processing API.
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

#ifndef CAINTEOIR_ENGINE_SIGPROC_HPP
#define CAINTEOIR_ENGINE_SIGPROC_HPP

#include "audio.hpp"
#include <stdint.h>

namespace cainteoir
{
	struct complex
	{
		float re;
		float im;
	};

	typedef std::vector<float>   float_array;
	typedef std::vector<complex> complex_array;

	void abs(complex_array &aData);

	void abs2(complex_array &aData);

	void logr(complex_array &aData);

	template <typename T>
	struct audio_data
	{
		std::vector<T> samples;
		std::shared_ptr<audio_info> info;
	};

	audio_data<short>
	read_s16_samples(const std::shared_ptr<cainteoir::buffer> &aData,
	                 const css::time &aStart,
	                 const css::time &aEnd,
	                 int aChannel,
	                 int aFrequency);

	float_array
	cosine_window(float aAlpha, float aBeta, uint32_t aWindowSize);

	float_array
	window(const char *aName, uint32_t aWindowSize);

	struct window_enumerator
	{
		virtual ~window_enumerator() {}

		virtual bool next() = 0;

		complex_array data;
	};

	std::shared_ptr<window_enumerator>
	create_s16_window_enumerator(const audio_data<short> &aData,
	                             const float_array &aWindow,
	                             uint32_t aStepSize);
}

#endif
