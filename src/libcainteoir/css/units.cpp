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

enum class time_unit_state : uint8_t
{
	invalid,
	value,
	hours,
	intermediate_1,
	intermediate_2,
	minutes,
	seconds,
	milliseconds,
	clock,
};

#define _ 0

static const uint8_t time_unit_state_transitions[][26] = {
	// a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z
	{  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 0 : invalid
	{  _, _, _, _, _, _, _, 2, _, _, _, _, 3, _, _, _, _, _, 6, _, _, _, _, _, _, _ }, // 1 : value
	{  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 2 : hours (h)
	{  _, _, _, _, _, _, _, _, 4, _, _, _, _, _, _, _, _, _, 7, _, _, _, _, _, _, _ }, // 3 : m...
	{  _, _, _, _, _, _, _, _, _, _, _, _, _, 5, _, _, _, _, _, _, _, _, _, _, _, _ }, // 4 : mi...
	{  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 5 : minutes (min)
	{  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 6 : seconds (s)
	{  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 7 : milliseconds (ms)
	{  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 8 : clock (e.g. MM:SS)
};

#undef _

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

css::time css::parse_smil_time(const buffer &aValue)
{
	if (aValue.empty()) return time();

	uint8_t state = (uint8_t)time_unit_state::value;
	int value = 0; // the value to set the time to
	int accumulator = 0; // the value of the current block
	int digits = 0; // the number of digits in the current block
	int divisor = 1; // the number to divide by to convert value to a fraction
	int groups = 1; // the number of time groups (separated by a ':' character)
	bool is_fraction = false;
	for (char c : aValue) switch (c)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		switch ((time_unit_state)state)
		{
		case time_unit_state::value:
		case time_unit_state::clock:
			accumulator *= 10;
			accumulator += (c - '0');
			if (is_fraction)
			{
				value   *= 10;
				divisor *= 10;
			}
			else
				++digits;
			break;
		default:
			state = (uint8_t)time_unit_state::invalid;
			break;
		}
		break;
	case ':':
		switch ((time_unit_state)state)
		{
		case time_unit_state::value:
		case time_unit_state::clock:
			state = (uint8_t)time_unit_state::clock;
			break;
		default:
			state = (uint8_t)time_unit_state::invalid;
			break;
		}

		if (groups == 3)
			throw std::runtime_error("clock values cannot specify days (i.e. have 3 ':' characters)");
		if (digits == 0)
			throw std::runtime_error("no value in a hours, minutes or seconds block");
		if (groups > 1 && digits != 2)
			throw std::runtime_error("an hours, minutes or seconds block must have 2 digits");
		value += accumulator;
		value *= 60;
		accumulator = 0;
		digits = 0;
		++groups;
		break;
	case '.':
		switch ((time_unit_state)state)
		{
		case time_unit_state::value:
		case time_unit_state::clock:
			if (is_fraction)
				throw std::runtime_error("multiple '.' characters found in time string");
			is_fraction = true;
			break;
		default:
			state = (uint8_t)time_unit_state::invalid;
			break;
		}
		break;
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
	case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
	case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
		state = time_unit_state_transitions[state][c - 'a'];
		break;
	default:
		throw std::runtime_error("invalid character found in time string");
	}

	if (groups > 1)
	{
		if (digits == 0)
			throw std::runtime_error("no value in a hours, minutes or seconds block");
		if (digits != 2)
			throw std::runtime_error("an hours, minutes or seconds block must have 2 digits");
	}

	float time_value = float(value + accumulator) / divisor;
	switch ((time_unit_state)state)
	{
	case time_unit_state::hours:
		return css::time(time_value * 3600.0, css::time::seconds);
	case time_unit_state::minutes:
		return css::time(time_value * 60.0, css::time::seconds);
	case time_unit_state::value:
	case time_unit_state::clock:
	case time_unit_state::seconds:
		return css::time(time_value, css::time::seconds);
	case time_unit_state::milliseconds:
		return css::time(time_value, css::time::milliseconds);
	default:
		throw std::runtime_error("invalid character found in time string");
	}
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
