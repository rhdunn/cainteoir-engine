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

enum class frequency_unit_state : uint8_t
{
	invalid,
	empty,
	value,
	fraction_value,
	intermediate_1,
	intermediate_2,
	kilohertz,
	intermediate_3,
	hertz,
	note_C,
	note_Csharp,
	note_D,
	note_Dsharp,
	note_E,
	note_F,
	note_Fsharp,
	note_G,
	note_Gsharp,
	note_A,
	note_Asharp,
	note_B,
	musical_note,
};

#define _ 0
#define a 10
#define b 11
#define c 12
#define d 13
#define e 14
#define f 15
#define g 16
#define h 17
#define i 18
#define j 19
#define k 20

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

static const uint8_t frequency_unit_state_transitions[][26] = {
	// .  k  H  z  A  B  C  D  E  F  G  #  b
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 0 : invalid
	{  _, _, _, _, i, k, 9, b, d, e, g, _, _ }, // 1 : empty
	{  3, 4, 7, _, _, _, _, _, _, _, _, _, _ }, // 2 : value
	{  _, 4, 7, _, _, _, _, _, _, _, _, _, _ }, // 3 : fraction value
	{  _, _, 5, _, _, _, _, _, _, _, _, _, _ }, // 4 : k...
	{  _, _, _, 6, _, _, _, _, _, _, _, _, _ }, // 5 : kH...
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 6 : kilohertz (kHz)
	{  _, _, _, 8, _, _, _, _, _, _, _, _, _ }, // 7 : H...
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // 8 : hertz (Hz)
	{  _, _, _, _, _, _, _, _, _, _, _, a, _ }, // 9 : C
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // a : C#
	{  _, _, _, _, _, _, _, _, _, _, _, c, a }, // b : D
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // c : D#
	{  _, _, _, _, _, _, _, _, _, _, _, _, c }, // d : E
	{  _, _, _, _, _, _, _, _, _, _, _, f, _ }, // e : F
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // f : F#
	{  _, _, _, _, _, _, _, _, _, _, _, h, f }, // g : G
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // h : G#
	{  _, _, _, _, _, _, _, _, _, _, _, j, h }, // i : A
	{  _, _, _, _, _, _, _, _, _, _, _, _, _ }, // j : A#
	{  _, _, _, _, _, _, _, _, _, _, _, _, j }, // k : B
};

#undef _
#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
#undef h
#undef i
#undef j
#undef k

// Reference: http://www.phy.mtu.edu/~suits/notefreqs.html
static const float music_note_frequencies[][12] = {
// C        C#/Db    D        D#/Eb    E        F        F#/Gb    G        G#/Ab    A        A#/Bb    B
{    16.35,   17.32,   18.35,   19.45,   20.60,   21.83,   23.12,   24.50,   25.96,   27.50,   29.14,   30.87 }, // 0
{    32.70,   34.65,   36.71,   38.89,   41.20,   43.65,   46.25,   49.00,   51.91,   55.00,   58.27,   61.74 }, // 1
{    65.41,   69.30,   73.42,   77.78,   82.41,   87.31,   92.50,   98.00,  103.83,  110.00,  116.54,  123.47 }, // 2
{   130.81,  138.59,  146.83,  155.56,  164.81,  174.61,  185.00,  196.00,  207.65,  220.00,  233.08,  246.94 }, // 3
{   261.63,  277.18,  293.66,  311.13,  329.63,  349.23,  369.99,  392.00,  415.30,  440.00,  466.16,  493.88 }, // 4
{   523.25,  554.37,  587.33,  622.25,  659.25,  698.46,  739.99,  783.99,  830.61,  880.00,  932.33,  987.77 }, // 5
{  1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53 }, // 6
{  2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07 }, // 7
{  4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13 }, // 8
// C        C#/Db    D        D#/Eb    E        F        F#/Gb    G        G#/Ab    A        A#/Bb    B
};

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

css::frequency css::parse_frequency(const buffer &aValue)
{
	if (aValue.empty()) return frequency();

	uint8_t state = (uint8_t)frequency_unit_state::empty;
	int value = 0; // the value to set the time to
	int accumulator = 0; // the value of the current block
	int divisor = 1; // the number to divide by to convert value to a fraction
	int note = 0; // the musical note (if present)
	for (char c : aValue) switch (c)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		switch ((frequency_unit_state)state)
		{
		case frequency_unit_state::empty:
			state = (uint8_t)frequency_unit_state::value;
			// fall through...
		case frequency_unit_state::value:
			accumulator *= 10;
			accumulator += (c - '0');
			break;
		case frequency_unit_state::fraction_value:
			accumulator *= 10;
			accumulator += (c - '0');
			value   *= 10;
			divisor *= 10;
			break;
		case frequency_unit_state::note_C:
		case frequency_unit_state::note_Csharp:
		case frequency_unit_state::note_D:
		case frequency_unit_state::note_Dsharp:
		case frequency_unit_state::note_E:
		case frequency_unit_state::note_F:
		case frequency_unit_state::note_Fsharp:
		case frequency_unit_state::note_G:
		case frequency_unit_state::note_Gsharp:
		case frequency_unit_state::note_A:
		case frequency_unit_state::note_Asharp:
		case frequency_unit_state::note_B:
			note  = state - (uint8_t)frequency_unit_state::note_C;
			value = (c - '0');
			state = (uint8_t)frequency_unit_state::musical_note;
			break;
		default:
			state = (uint8_t)frequency_unit_state::invalid;
			break;
		}
		break;
	case '.': state = frequency_unit_state_transitions[state][ 0]; break;
	case 'k': state = frequency_unit_state_transitions[state][ 1]; break;
	case 'H': state = frequency_unit_state_transitions[state][ 2]; break;
	case 'z': state = frequency_unit_state_transitions[state][ 3]; break;
	case 'A': state = frequency_unit_state_transitions[state][ 4]; break;
	case 'B': state = frequency_unit_state_transitions[state][ 5]; break;
	case 'C': state = frequency_unit_state_transitions[state][ 6]; break;
	case 'D': state = frequency_unit_state_transitions[state][ 7]; break;
	case 'E': state = frequency_unit_state_transitions[state][ 8]; break;
	case 'F': state = frequency_unit_state_transitions[state][ 9]; break;
	case 'G': state = frequency_unit_state_transitions[state][10]; break;
	case '#': state = frequency_unit_state_transitions[state][11]; break;
	case 'b': state = frequency_unit_state_transitions[state][12]; break;
	default:
		throw std::runtime_error("invalid character found in frequency string");
	}

	switch ((frequency_unit_state)state)
	{
	case frequency_unit_state::value:
	case frequency_unit_state::fraction_value:
	case frequency_unit_state::hertz:
		return frequency(float(value + accumulator) / divisor, css::frequency::hertz);
	case frequency_unit_state::kilohertz:
		return frequency(float(value + accumulator) / divisor, css::frequency::kilohertz);
	case frequency_unit_state::musical_note:
		return frequency(music_note_frequencies[value][note], css::frequency::hertz);
	default:
		throw std::runtime_error("invalid character found in frequency string");
	}
}

css::frequency css::frequency::as(const type aUnits) const
{
	static constexpr float hertz_in_kilohertz = 1000;

	switch (mUnits)
	{
	case hertz:
		switch (aUnits)
		{
		case hertz:
			return *this;
		case kilohertz:
			return frequency(mValue / hertz_in_kilohertz, aUnits);
		case inherit:
			break;
		}
		break;
	case kilohertz:
		switch (aUnits)
		{
		case hertz:
			return frequency(mValue * hertz_in_kilohertz, aUnits);
		case kilohertz:
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
