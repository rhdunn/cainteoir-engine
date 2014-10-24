/* Window enumeration.
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

#include <cainteoir/sigproc.hpp>
#include <cmath>

struct s16_window_enumerator : public cainteoir::window_enumerator
{
	s16_window_enumerator(const cainteoir::audio_data<short> &aData,
	                      const std::vector<float> &aWindow,
	                      uint32_t aStepSize);

	bool next();
private:
	const std::vector<short> &mSamples;
	const std::vector<float> &mWindow;
	uint32_t mStepSize;
	uint32_t mOffset;
};

s16_window_enumerator::s16_window_enumerator(const cainteoir::audio_data<short> &aData,
                                             const std::vector<float> &aWindow,
                                             uint32_t aStepSize)
	: mSamples(aData.samples)
	, mWindow(aWindow)
	, mStepSize(aStepSize)
	, mOffset(0)
{
	data.resize(mSamples.size());
}

bool s16_window_enumerator::next()
{
	uint32_t sample_count = mSamples.size();
	if (mOffset >= sample_count)
		return false;

	uint32_t window_size = mWindow.size();
	uint32_t samples = std::min(sample_count - mOffset, window_size);

	const short   *current = &mSamples[0] + mOffset;
	const short   *end     = current + samples;
	const float   *window  = &mWindow[0];

	cainteoir::complex *out = &data[0];
	while (current != end)
	{
		out->re = ((float)*current / 32768) * *window;
		out->im = 0.0f;

		++current;
		++window;
		++out;
	}

	while (samples != window_size)
	{
		out->re = 0.0f;
		out->im = 0.0f;

		++samples;
		++out;
	}

	mOffset += mStepSize;
	return true;
}

std::shared_ptr<cainteoir::window_enumerator>
cainteoir::create_s16_window_enumerator(const audio_data<short> &aData,
                                        const std::vector<float> &aWindow,
                                        uint32_t aStepSize)
{
	return std::make_shared<s16_window_enumerator>(aData, aWindow, aStepSize);
}
