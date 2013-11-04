/* CSS Counter Styles.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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

#include <cainteoir/content.hpp>
#include <sstream>
#include <limits>

namespace css = cainteoir::css;

std::string css::counter_style::marker(value_t count) const
{
	std::ostringstream textval;
	textval << prefix;
	int n = symbols.size();
	if (n != 0) switch (system)
	{
	case css::counter_system::cyclic:
		textval << symbols[count % n];
		break;
	case css::counter_system::numeric:
		if (count == 0)
			textval << symbols[0];
		else
		{
			std::string s;
			while (count != 0)
			{
				s = symbols[count % n] + s;
				count = count / n;
			}
			textval << s;
		}
		break;
	default:
		break;
	}
	textval << suffix << ' ';
	return textval.str();
}

const css::counter_style::range_t css::counter_style::infinite = {
	std::numeric_limits<value_t>::min(),
	std::numeric_limits<value_t>::max()
};

const css::counter_style::range_t css::counter_style::get_auto_range(counter_system system)
{
	switch (system)
	{
	case counter_system::alphabetic:
	case counter_system::symbolic:
		return { 1, infinite.second };
	case counter_system::additive:
		return { 0, infinite.second };
	default:
		break;
	}
	return infinite;
}
