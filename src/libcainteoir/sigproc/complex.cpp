/* Complex math functions.
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

void cainteoir::abs(std::vector<complex> &aData)
{
	for (auto & c : aData)
	{
		c.re = sqrt(c.re*c.re + c.im*c.im);
		c.im = 0;
	}
}

void cainteoir::abs2(std::vector<complex> &aData)
{
	for (auto & c : aData)
	{
		c.re = c.re*c.re + c.im*c.im;
		c.im = 0;
	}
}

void cainteoir::logr(std::vector<complex> &aData)
{
	for (auto & c : aData)
	{
		c.re = log(c.re);
	}
}
