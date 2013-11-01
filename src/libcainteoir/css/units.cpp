/* CSS Values and Units.
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
#include <stdexcept>

namespace css = cainteoir::css;

css::length css::length::as(const type aUnits) const
{
	static constexpr float points_in_pica = 12;
	static constexpr float points_in_inch = 72;
	static constexpr float picas_in_inch = points_in_inch / points_in_pica;
	static constexpr float centimeters_in_inch = 2.54;
	static constexpr float centimeters_in_point = centimeters_in_inch / points_in_inch;
	static constexpr float centimeters_in_pica = centimeters_in_inch / picas_in_inch;
	static constexpr float millimeters_in_centimeter = 10;
	static constexpr float millimeters_in_inch = centimeters_in_inch * millimeters_in_centimeter;
	static constexpr float millimeters_in_point = millimeters_in_inch / points_in_inch;
	static constexpr float millimeters_in_pica  = millimeters_in_inch / picas_in_inch;
	static constexpr float pixels_in_inch = 96;
	static constexpr float pixels_in_point = pixels_in_inch / points_in_inch;
	static constexpr float pixels_in_pica = pixels_in_inch / picas_in_inch;
	static constexpr float pixels_in_centimeter = pixels_in_inch / centimeters_in_inch;
	static constexpr float pixels_in_millimeter = pixels_in_inch / millimeters_in_inch;

	switch (mUnits)
	{
	case millimeters:
		switch (aUnits)
		{
		case millimeters:
			return *this;
		case centimeters:
			return length(mValue / millimeters_in_centimeter, aUnits);
		case inches:
			return length(mValue / millimeters_in_inch, aUnits);
		case points:
			return length(mValue / millimeters_in_point, aUnits);
		case picas:
			return length(mValue / millimeters_in_pica, aUnits);
		case pixels:
			return length(mValue * pixels_in_millimeter, aUnits);
		case inherit:
			break;
		}
		break;
	case centimeters:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_centimeter, aUnits);
		case centimeters:
			return *this;
		case inches:
			return length(mValue / centimeters_in_inch, aUnits);
		case points:
			return length(mValue / centimeters_in_point, aUnits);
		case picas:
			return length(mValue / centimeters_in_pica, aUnits);
		case pixels:
			return length(mValue * pixels_in_centimeter, aUnits);
		case inherit:
			break;
		}
		break;
	case inches:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_inch, aUnits);
		case centimeters:
			return length(mValue * centimeters_in_inch, aUnits);
		case inches:
			return *this;
		case points:
			return length(mValue * points_in_inch, aUnits);
		case picas:
			return length(mValue * picas_in_inch, aUnits);
		case pixels:
			return length(mValue * pixels_in_inch, aUnits);
		case inherit:
			break;
		}
		break;
	case points:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_point, aUnits);
		case centimeters:
			return length(mValue * centimeters_in_point, aUnits);
		case inches:
			return length(mValue / points_in_inch, aUnits);
		case points:
			return *this;
		case picas:
			return length(mValue / points_in_pica, aUnits);
		case pixels:
			return length(mValue * pixels_in_point, aUnits);
		case inherit:
			break;
		}
		break;
	case picas:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue * millimeters_in_pica, aUnits);
		case centimeters:
			return length(mValue * centimeters_in_pica, aUnits);
		case inches:
			return length(mValue / picas_in_inch, aUnits);
		case points:
			return length(mValue * points_in_pica, aUnits);
		case picas:
			return *this;
		case pixels:
			return length(mValue * pixels_in_pica, aUnits);
		case inherit:
			break;
		}
		break;
	case pixels:
		switch (aUnits)
		{
		case millimeters:
			return length(mValue / pixels_in_millimeter, aUnits);
		case centimeters:
			return length(mValue / pixels_in_centimeter, aUnits);
		case inches:
			return length(mValue / pixels_in_inch, aUnits);
		case points:
			return length(mValue / pixels_in_point, aUnits);
		case picas:
			return length(mValue / pixels_in_pica, aUnits);
		case pixels:
			return *this;
		case inherit:
			break;
		}
		break;
	case inherit:
		break;
	}

	throw std::runtime_error("unable to convert to the specified units");
}

css::time::time(const buffer &aValue, const parse_as_type aParseAs)
	: mValue(0)
	, mUnits(css::time::inherit)
{
}

css::time css::time::as(const type aUnits) const
{
	static constexpr float milliseconds_in_seconds = 1000;

	switch (mUnits)
	{
	case seconds:
		switch (aUnits)
		{
		case seconds:
			return *this;
		case milliseconds:
			return time(mValue * milliseconds_in_seconds, aUnits);
		case inherit:
			break;
		}
		break;
	case milliseconds:
		switch (aUnits)
		{
		case seconds:
			return time(mValue / milliseconds_in_seconds, aUnits);
		case milliseconds:
			return *this;
		case inherit:
			break;
		}
		break;
	case inherit:
		break;
	}

	throw std::runtime_error("unable to convert to the specified units");
}
