/* Phoneme Model.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#include <cainteoir/phoneme.hpp>

namespace tts = cainteoir::tts;

tts::phoneme::phoneme(const feature a, const feature b, const feature c, const feature d)
{
	features[0] = a;
	features[1] = b;
	features[2] = c;
	features[3] = d;
}

bool tts::phoneme::contains(const feature f) const
{
	if (f != feature::unspecified) for (const feature x : features)
	{
		if (x == f) return true;
	}
	return false;
}
