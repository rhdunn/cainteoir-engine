/* Fast fourier transform.
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

static uint32_t log2(uint32_t aValue)
{
	uint32_t exponent = 0;
	while (aValue >>= 1)
		++exponent;
	return exponent;
}

static void fft(cainteoir::complex_array &aData, int sign)
{
	uint32_t size = aData.size();
	uint32_t m = log2(size);

	if (size == 0 || (size & (size - 1)) != 0) // if size is not a power of 2 ...
		throw std::runtime_error("FFT data must have 2^n elements (for any n > 0)");

	uint32_t le  = size;
	uint32_t le1 = size;
	for (uint32_t l = 0; l < m; ++l)
	{
		le    = le1;
		le1 >>= 1;

		cainteoir::complex u = { 1.0f, 0.0f };
		cainteoir::complex w = { cosf(M_PI / le1), sign * sinf(M_PI / le1) };

		for (uint32_t j = 0; j < le1; ++j)
		{
			for (uint32_t i = j; i < size-le1; i += le)
			{
				cainteoir::complex *a = &aData[i];
				cainteoir::complex *b = &aData[i + le1];

				cainteoir::complex p = { a->re + b->re, a->im + b->im };
				cainteoir::complex q = { a->re - b->re, a->im - b->im };

				b->re = q.re*u.re - q.im*u.im;
				b->im = q.re*u.im + q.im*u.re;

				*a = p;
			}

			u = { u.re*w.re - u.im*w.im, u.re*w.im + u.im*w.re };
		}
	}

	uint32_t j = 0;
	for (uint32_t i = 0; i <= size - 2; ++i)
	{
		if (i < j)
			std::swap(aData[i], aData[j]);

		uint32_t k = size / 2;
		for (; k <= j; k /= 2)
			j -= k;
		j += k;
	}
}

void cainteoir::fft(complex_array &aData)
{
	::fft(aData, -1);
}

void cainteoir::ifft(complex_array &aData)
{
	::fft(aData, 1);

	float n = aData.size();
	for (auto & c : aData)
	{
		c.re /= n;
		c.im /= n;
	}
}
