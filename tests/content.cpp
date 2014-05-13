/* Document Content Rendering Model tests.
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

#include <cainteoir/content.hpp>
#include <cstdio>

#include "tester.hpp"

namespace css = cainteoir::css;

REGISTER_TESTSUITE("content");

template <typename UnitT> struct unit_type;
template <> struct unit_type<css::length::type>    { typedef css::length    type; };
template <> struct unit_type<css::time::type>      { typedef css::time      type; };
template <> struct unit_type<css::frequency::type> { typedef css::frequency type; };

template <typename UnitT>
static void test_equal_(const typename unit_type<UnitT>::type aObject,
                        float aValue, const UnitT aUnits,
                        const char *location, int line)
{
	assert_location(aObject.units() == aUnits, location, line);
	assert_location(aObject.value() == aValue, location, line);
}

#define test_equal(a, b, c) test_equal_(a, b, c, __FILE__, __LINE__)

template <typename UnitT>
static void test_conversion_(float aFromValue, const UnitT aFromUnits,
                             float aToValue,   const UnitT aToUnits,
                             bool throws,
                             const char *location, int line)
{
	try
	{
		typedef typename unit_type<UnitT>::type unit_t;
		unit_t to = unit_t(aFromValue, aFromUnits).as(aToUnits);
		assert_location(to.units() == aToUnits, location, line);
		assert_location(to.value() == aToValue, location, line);
		assert_location(!throws, location, line);
	}
	catch (std::exception &e)
	{
		assert_location(throws, location, line);
	}
}

#define test_conversion(a, b, c, d, e) test_conversion_(a, b, c, d, e, __FILE__, __LINE__)

static void test_parser_(const char *aString, decltype(css::parse_smil_time) *time_parser,
                         float aValue, const css::time::type aUnits,
                         bool throws,
                         const char *location, int line)
{
	try
	{
		css::time value = time_parser(aString);
		assert_location(value.units() == aUnits, location, line);
		assert_location(value.value() == aValue, location, line);
		assert_location(!throws, location, line);
	}
	catch (std::exception &e)
	{
		assert_location(throws, location, line);
	}
}

static void test_parser_(const char *aString, decltype(css::parse_frequency) *frequency_parser,
                         float aValue, const css::frequency::type aUnits,
                         bool throws,
                         const char *location, int line)
{
	try
	{
		css::frequency value = frequency_parser(aString);
		assert_location(value.units() == aUnits, location, line);
		assert_location(value.value() == aValue, location, line);
		assert_location(!throws, location, line);
	}
	catch (std::exception &e)
	{
		assert_location(throws, location, line);
	}
}

#define test_parser(a, b, c, d, e) test_parser_(a, b, c, d, e, __FILE__, __LINE__)

TEST_CASE("length construction")
{
	test_equal(css::length(), 0, css::length::inherit);

	test_equal(css::length(9, css::length::inherit),     9, css::length::inherit);
	test_equal(css::length(8, css::length::millimeters), 8, css::length::millimeters);
	test_equal(css::length(7, css::length::centimeters), 7, css::length::centimeters);
	test_equal(css::length(6, css::length::inches),      6, css::length::inches);
	test_equal(css::length(5, css::length::points),      5, css::length::points);
	test_equal(css::length(4, css::length::picas),       4, css::length::picas);
	test_equal(css::length(3, css::length::pixels),      3, css::length::pixels);
}

TEST_CASE("length conversion")
{
	test_conversion(1, css::length::inherit, 1, css::length::inherit,     true);
	test_conversion(1, css::length::inherit, 1, css::length::millimeters, true);
	test_conversion(1, css::length::inherit, 1, css::length::centimeters, true);
	test_conversion(1, css::length::inherit, 1, css::length::inches,      true);
	test_conversion(1, css::length::inherit, 1, css::length::points,      true);
	test_conversion(1, css::length::inherit, 1, css::length::picas,       true);
	test_conversion(1, css::length::inherit, 1, css::length::pixels,      true);

	test_conversion(1,    css::length::millimeters,  1, css::length::inherit,     true);
	test_conversion(1,    css::length::millimeters,  1, css::length::millimeters, false);
	test_conversion(10,   css::length::millimeters,  1, css::length::centimeters, false);
	test_conversion(25.4, css::length::millimeters,  1, css::length::inches,      false);
	test_conversion(25.4, css::length::millimeters, 72, css::length::points,      false);
	test_conversion(25.4, css::length::millimeters,  6, css::length::picas,       false);
	test_conversion(25.4, css::length::millimeters, 96, css::length::pixels,      false);

	test_conversion(1,    css::length::centimeters,  1, css::length::inherit,     true);
	test_conversion(1,    css::length::centimeters, 10, css::length::millimeters, false);
	test_conversion(1,    css::length::centimeters,  1, css::length::centimeters, false);
	test_conversion(2.54, css::length::centimeters,  1, css::length::inches,      false);
	test_conversion(2.54, css::length::centimeters, 72, css::length::points,      false);
	test_conversion(2.54, css::length::centimeters,  6, css::length::picas,       false);
	test_conversion(2.54, css::length::centimeters, 96, css::length::pixels,      false);

	test_conversion(1, css::length::inches,  1,    css::length::inherit,     true);
	test_conversion(1, css::length::inches, 25.4,  css::length::millimeters, false);
	test_conversion(1, css::length::inches,  2.54, css::length::centimeters, false);
	test_conversion(1, css::length::inches,  1,    css::length::inches,      false);
	test_conversion(1, css::length::inches, 72,    css::length::points,      false);
	test_conversion(1, css::length::inches,  6,    css::length::picas,       false);
	test_conversion(1, css::length::inches, 96,    css::length::pixels,      false);

	test_conversion(1,  css::length::points,  1,    css::length::inherit,     true);
	test_conversion(72, css::length::points, 25.4,  css::length::millimeters, false);
	test_conversion(72, css::length::points,  2.54, css::length::centimeters, false);
	test_conversion(72, css::length::points,  1,    css::length::inches,      false);
	test_conversion(1,  css::length::points,  1,    css::length::points,      false);
	test_conversion(12, css::length::points,  1,    css::length::picas,       false);
	test_conversion(72, css::length::points, 96,    css::length::pixels,      false);

	test_conversion(1, css::length::picas,  1,    css::length::inherit,     true);
	test_conversion(6, css::length::picas, 25.4,  css::length::millimeters, false);
	test_conversion(6, css::length::picas,  2.54, css::length::centimeters, false);
	test_conversion(6, css::length::picas,  1,    css::length::inches,      false);
	test_conversion(1, css::length::picas, 12,    css::length::points,      false);
	test_conversion(1, css::length::picas,  1,    css::length::picas,       false);
	test_conversion(6, css::length::picas, 96,    css::length::pixels,      false);

	test_conversion( 1, css::length::pixels,  1,    css::length::inherit,     true);
	test_conversion(96, css::length::pixels, 25.4,  css::length::millimeters, false);
	test_conversion(96, css::length::pixels,  2.54, css::length::centimeters, false);
	test_conversion(96, css::length::pixels,  1,    css::length::inches,      false);
	test_conversion(96, css::length::pixels, 72,    css::length::points,      false);
	test_conversion(96, css::length::pixels,  6,    css::length::picas,       false);
	test_conversion( 1, css::length::pixels,  1,    css::length::pixels,      false);
}

TEST_CASE("time construction")
{
	test_equal(css::time(), 0, css::time::inherit);

	test_equal(css::time(9, css::time::inherit),      9, css::time::inherit);
	test_equal(css::time(8, css::time::milliseconds), 8, css::time::milliseconds);
	test_equal(css::time(7, css::time::seconds),      7, css::time::seconds);
}

TEST_CASE("time parser - nullptr")
{
	test_parser(nullptr, css::parse_smil_time, 0, css::time::inherit, false);
}

TEST_CASE("time parser - SS (seconds)")
{
	test_parser("1",       css::parse_smil_time,       1, css::time::seconds, false);
	test_parser("25",      css::parse_smil_time,      25, css::time::seconds, false);
	test_parser("1500000", css::parse_smil_time, 1500000, css::time::seconds, false); // 416 hours, 40 min
}

TEST_CASE("time parser - SS.mmm (seconds, milliseconds)")
{
	test_parser( "1.",    css::parse_smil_time,  1,     css::time::seconds, false);
	test_parser( "0.5",   css::parse_smil_time,  0.5,   css::time::seconds, false);
	test_parser(  ".6",   css::parse_smil_time,  0.6,   css::time::seconds, false);
	test_parser( "1.2",   css::parse_smil_time,  1.2,   css::time::seconds, false);
	test_parser("12.345", css::parse_smil_time, 12.345, css::time::seconds, false);

	test_parser( "1..3",  css::parse_smil_time, 1, css::time::inherit, true); // invalid
	test_parser( "1.2.3", css::parse_smil_time, 1, css::time::inherit, true); // invalid
}

TEST_CASE("time parser - MM:SS (minutes, seconds)")
{
	test_parser("0:23", css::parse_smil_time,  23, css::time::seconds, false);
	test_parser("1:04", css::parse_smil_time,  64, css::time::seconds, false);

	test_parser("00:23", css::parse_smil_time,  23, css::time::seconds, false);
	test_parser("01:04", css::parse_smil_time,  64, css::time::seconds, false);
	test_parser("12:32", css::parse_smil_time, 752, css::time::seconds, false);
	test_parser("04:00", css::parse_smil_time, 240, css::time::seconds, false);

	test_parser( ":25", css::parse_smil_time, 1, css::time::inherit, true); // invalid
	test_parser( "1:",  css::parse_smil_time, 1, css::time::inherit, true); // invalid

	test_parser( "1:2", css::parse_smil_time, 1, css::time::inherit, true); // invalid
}

TEST_CASE("time parser - MM:SS.mmm (minutes, seconds)")
{
	test_parser( "0:12.5",  css::parse_smil_time,  12.5,  css::time::seconds, false);
	test_parser( "2:01.22", css::parse_smil_time, 121.22, css::time::seconds, false);

	test_parser("00:12.5",   css::parse_smil_time,  12.5,   css::time::seconds, false);
	test_parser("02:01.22",  css::parse_smil_time, 121.22,  css::time::seconds, false);
	test_parser("10:30.75",  css::parse_smil_time, 630.75,  css::time::seconds, false);
	test_parser("03:00.333", css::parse_smil_time, 180.333, css::time::seconds, false);
}

TEST_CASE("time parser - HH:MM:SS (hours, minutes, seconds)")
{
	test_parser(  "0:00:23", css::parse_smil_time,     23, css::time::seconds, false);
	test_parser(  "1:01:04", css::parse_smil_time,   3664, css::time::seconds, false);
	test_parser( "11:00:23", css::parse_smil_time,  39623, css::time::seconds, false);
	test_parser( "24:42:00", css::parse_smil_time,  88920, css::time::seconds, false);
	test_parser("140:12:32", css::parse_smil_time, 504752, css::time::seconds, false);

	test_parser( ":00:25", css::parse_smil_time, 1, css::time::inherit, true); // invalid
	test_parser( "0::25",  css::parse_smil_time, 1, css::time::inherit, true); // invalid
	test_parser( "0:01:",  css::parse_smil_time, 1, css::time::inherit, true); // invalid

	test_parser( "0:0:25", css::parse_smil_time, 1, css::time::inherit, true); // invalid
	test_parser( "0:00:5", css::parse_smil_time, 1, css::time::inherit, true); // invalid
}

TEST_CASE("time parser - HH:MM:SS.mmm (hours, minutes, seconds, milliseconds)")
{
	test_parser(  "0:00:23.1", css::parse_smil_time,     23.1, css::time::seconds, false);
	test_parser(  "1:01:04.2", css::parse_smil_time,   3664.2, css::time::seconds, false);
	test_parser( "11:00:23.3", css::parse_smil_time,  39623.3, css::time::seconds, false);
	test_parser( "24:42:00.4", css::parse_smil_time,  88920.4, css::time::seconds, false);
	test_parser("140:12:32.5", css::parse_smil_time, 504752.5, css::time::seconds, false);
}

TEST_CASE("time parser - DD:HH:MM:SS.mmm (days, hours, minutes, seconds, milliseconds)")
{
	// These are invalid, only upto hours are allowed by SMIL clock values ...

	test_parser( "0:00:00:00", css::parse_smil_time, 1, css::time::inherit, true);
	test_parser( "1:02:03:04", css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("11:22:43:44", css::parse_smil_time, 1, css::time::inherit, true);
}

TEST_CASE("time parser - #h (hours)")
{
	test_parser("2h",   css::parse_smil_time, 7200, css::time::seconds, false);
	test_parser("2.5h", css::parse_smil_time, 9000, css::time::seconds, false);
}

TEST_CASE("time parser - #min (seconds)")
{
	test_parser("5min",    css::parse_smil_time, 300, css::time::seconds, false);
	test_parser("2.25min", css::parse_smil_time, 135, css::time::seconds, false);
}

TEST_CASE("time parser - #s (seconds)")
{
	test_parser("25s",  css::parse_smil_time, 25,   css::time::seconds, false);
	test_parser("1.2s", css::parse_smil_time,  1.2, css::time::seconds, false);
}

TEST_CASE("time parser - #ms (milliseconds)")
{
	test_parser("11ms",  css::parse_smil_time, 11,   css::time::milliseconds, false);
	test_parser("9.7ms", css::parse_smil_time,  9.7, css::time::milliseconds, false);
}

TEST_CASE("time parser - invalid suffixes")
{
	test_parser("2d",    css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2ha",   css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2mun",  css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2m",    css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2mi",   css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2mio",  css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2mins", css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2mo",   css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2mss",  css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("2st",   css::parse_smil_time, 1, css::time::inherit, true);

	test_parser("1:15s", css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("1h:15", css::parse_smil_time, 1, css::time::inherit, true);

	test_parser("1h5min", css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("1m5in",  css::parse_smil_time, 1, css::time::inherit, true);

	test_parser("1h.5",  css::parse_smil_time, 1, css::time::inherit, true);
	test_parser("1m.in", css::parse_smil_time, 1, css::time::inherit, true);
}

TEST_CASE("time conversion")
{
	test_conversion(1, css::time::inherit, 1, css::time::inherit,      true);
	test_conversion(1, css::time::inherit, 1, css::time::milliseconds, true);
	test_conversion(1, css::time::inherit, 1, css::time::seconds,      true);

	test_conversion(   1, css::time::milliseconds, 1,     css::time::inherit,      true);
	test_conversion(   1, css::time::milliseconds, 1,     css::time::milliseconds, false);
	test_conversion(1000, css::time::milliseconds, 1,     css::time::seconds,      false);
	test_conversion( 500, css::time::milliseconds, 0.5,   css::time::seconds,      false);
	test_conversion(  20, css::time::milliseconds, 0.020, css::time::seconds,      false);
	test_conversion(   1, css::time::milliseconds, 0.001, css::time::seconds,      false);

	test_conversion(1,     css::time::seconds,    1, css::time::inherit,      true);
	test_conversion(1,     css::time::seconds, 1000, css::time::milliseconds, false);
	test_conversion(0.9,   css::time::seconds,  900, css::time::milliseconds, false);
	test_conversion(0.06,  css::time::seconds,   60, css::time::milliseconds, false);
	test_conversion(0.003, css::time::seconds,    3, css::time::milliseconds, false);
	test_conversion(1,     css::time::seconds,    1, css::time::seconds,      false);
}

TEST_CASE("frequency construction")
{
	test_equal(css::frequency(), 0, css::frequency::inherit);

	test_equal(css::frequency(9, css::frequency::inherit),   9, css::frequency::inherit);
	test_equal(css::frequency(8, css::frequency::hertz),     8, css::frequency::hertz);
	test_equal(css::frequency(7, css::frequency::kilohertz), 7, css::frequency::kilohertz);
}

TEST_CASE("frequency parser - nullptr")
{
	test_parser(nullptr, css::parse_frequency, 0, css::frequency::inherit, false);
}

TEST_CASE("frequency parser - number (hertz)")
{
	test_parser("2",   css::parse_frequency,  2.0, css::frequency::hertz, false);
	test_parser("37",  css::parse_frequency, 37.0, css::frequency::hertz, false);
	test_parser("1.5", css::parse_frequency,  1.5, css::frequency::hertz, false);
}

TEST_CASE("frequency parser - #Hz (hertz)")
{
	test_parser("2Hz",   css::parse_frequency,  2.0, css::frequency::hertz, false);
	test_parser("37Hz",  css::parse_frequency, 37.0, css::frequency::hertz, false);
	test_parser("1.5Hz", css::parse_frequency,  1.5, css::frequency::hertz, false);
}

TEST_CASE("frequency parser - #kHz (kilohertz)")
{
	test_parser("2kHz",   css::parse_frequency,  2.0, css::frequency::kilohertz, false);
	test_parser("37kHz",  css::parse_frequency, 37.0, css::frequency::kilohertz, false);
	test_parser("1.5kHz", css::parse_frequency,  1.5, css::frequency::kilohertz, false);
}

TEST_CASE("frequency parser - musical notes")
{
	// Equal-tempered scale; A4 = 440Hz
	// Reference: http://www.phy.mtu.edu/~suits/notefreqs.html

	test_parser("C0",  css::parse_frequency,   16.35, css::frequency::hertz, false);
	test_parser("C#0", css::parse_frequency,   17.32, css::frequency::hertz, false);
	test_parser("Db0", css::parse_frequency,   17.32, css::frequency::hertz, false);
	test_parser("D0",  css::parse_frequency,   18.35, css::frequency::hertz, false);
	test_parser("D#0", css::parse_frequency,   19.45, css::frequency::hertz, false);
	test_parser("Eb0", css::parse_frequency,   19.45, css::frequency::hertz, false);
	test_parser("E0",  css::parse_frequency,   20.60, css::frequency::hertz, false);
	test_parser("F0",  css::parse_frequency,   21.83, css::frequency::hertz, false);
	test_parser("F#0", css::parse_frequency,   23.12, css::frequency::hertz, false);
	test_parser("Gb0", css::parse_frequency,   23.12, css::frequency::hertz, false);
	test_parser("G0",  css::parse_frequency,   24.50, css::frequency::hertz, false);
	test_parser("G#0", css::parse_frequency,   25.96, css::frequency::hertz, false);
	test_parser("Ab0", css::parse_frequency,   25.96, css::frequency::hertz, false);
	test_parser("A0",  css::parse_frequency,   27.50, css::frequency::hertz, false);
	test_parser("A#0", css::parse_frequency,   29.14, css::frequency::hertz, false);
	test_parser("Bb0", css::parse_frequency,   29.14, css::frequency::hertz, false);
	test_parser("B0",  css::parse_frequency,   30.87, css::frequency::hertz, false);

	test_parser("C1",  css::parse_frequency,   32.70, css::frequency::hertz, false);
	test_parser("C#1", css::parse_frequency,   34.65, css::frequency::hertz, false);
	test_parser("Db1", css::parse_frequency,   34.65, css::frequency::hertz, false);
	test_parser("D1",  css::parse_frequency,   36.71, css::frequency::hertz, false);
	test_parser("D#1", css::parse_frequency,   38.89, css::frequency::hertz, false);
	test_parser("Eb1", css::parse_frequency,   38.89, css::frequency::hertz, false);
	test_parser("E1",  css::parse_frequency,   41.20, css::frequency::hertz, false);
	test_parser("F1",  css::parse_frequency,   43.65, css::frequency::hertz, false);
	test_parser("F#1", css::parse_frequency,   46.25, css::frequency::hertz, false);
	test_parser("Gb1", css::parse_frequency,   46.25, css::frequency::hertz, false);
	test_parser("G1",  css::parse_frequency,   49.00, css::frequency::hertz, false);
	test_parser("G#1", css::parse_frequency,   51.91, css::frequency::hertz, false);
	test_parser("Ab1", css::parse_frequency,   51.91, css::frequency::hertz, false);
	test_parser("A1",  css::parse_frequency,   55.00, css::frequency::hertz, false);
	test_parser("A#1", css::parse_frequency,   58.27, css::frequency::hertz, false);
	test_parser("Bb1", css::parse_frequency,   58.27, css::frequency::hertz, false);
	test_parser("B1",  css::parse_frequency,   61.74, css::frequency::hertz, false);

	test_parser("C2",  css::parse_frequency,   65.41, css::frequency::hertz, false);
	test_parser("C#2", css::parse_frequency,   69.30, css::frequency::hertz, false);
	test_parser("Db2", css::parse_frequency,   69.30, css::frequency::hertz, false);
	test_parser("D2",  css::parse_frequency,   73.42, css::frequency::hertz, false);
	test_parser("D#2", css::parse_frequency,   77.78, css::frequency::hertz, false);
	test_parser("Eb2", css::parse_frequency,   77.78, css::frequency::hertz, false);
	test_parser("E2",  css::parse_frequency,   82.41, css::frequency::hertz, false);
	test_parser("F2",  css::parse_frequency,   87.31, css::frequency::hertz, false);
	test_parser("F#2", css::parse_frequency,   92.50, css::frequency::hertz, false);
	test_parser("Gb2", css::parse_frequency,   92.50, css::frequency::hertz, false);
	test_parser("G2",  css::parse_frequency,   98.00, css::frequency::hertz, false);
	test_parser("G#2", css::parse_frequency,  103.83, css::frequency::hertz, false);
	test_parser("Ab2", css::parse_frequency,  103.83, css::frequency::hertz, false);
	test_parser("A2",  css::parse_frequency,  110.00, css::frequency::hertz, false);
	test_parser("A#2", css::parse_frequency,  116.54, css::frequency::hertz, false);
	test_parser("Bb2", css::parse_frequency,  116.54, css::frequency::hertz, false);
	test_parser("B2",  css::parse_frequency,  123.47, css::frequency::hertz, false);

	test_parser("C3",  css::parse_frequency,  130.81, css::frequency::hertz, false);
	test_parser("C#3", css::parse_frequency,  138.59, css::frequency::hertz, false);
	test_parser("Db3", css::parse_frequency,  138.59, css::frequency::hertz, false);
	test_parser("D3",  css::parse_frequency,  146.83, css::frequency::hertz, false);
	test_parser("D#3", css::parse_frequency,  155.56, css::frequency::hertz, false);
	test_parser("Eb3", css::parse_frequency,  155.56, css::frequency::hertz, false);
	test_parser("E3",  css::parse_frequency,  164.81, css::frequency::hertz, false);
	test_parser("F3",  css::parse_frequency,  174.61, css::frequency::hertz, false);
	test_parser("F#3", css::parse_frequency,  185.00, css::frequency::hertz, false);
	test_parser("Gb3", css::parse_frequency,  185.00, css::frequency::hertz, false);
	test_parser("G3",  css::parse_frequency,  196.00, css::frequency::hertz, false);
	test_parser("G#3", css::parse_frequency,  207.65, css::frequency::hertz, false);
	test_parser("Ab3", css::parse_frequency,  207.65, css::frequency::hertz, false);
	test_parser("A3",  css::parse_frequency,  220.00, css::frequency::hertz, false);
	test_parser("A#3", css::parse_frequency,  233.08, css::frequency::hertz, false);
	test_parser("Bb3", css::parse_frequency,  233.08, css::frequency::hertz, false);
	test_parser("B3",  css::parse_frequency,  246.94, css::frequency::hertz, false);

	test_parser("C4",  css::parse_frequency,  261.63, css::frequency::hertz, false);
	test_parser("C#4", css::parse_frequency,  277.18, css::frequency::hertz, false);
	test_parser("Db4", css::parse_frequency,  277.18, css::frequency::hertz, false);
	test_parser("D4",  css::parse_frequency,  293.66, css::frequency::hertz, false);
	test_parser("D#4", css::parse_frequency,  311.13, css::frequency::hertz, false);
	test_parser("Eb4", css::parse_frequency,  311.13, css::frequency::hertz, false);
	test_parser("E4",  css::parse_frequency,  329.63, css::frequency::hertz, false);
	test_parser("F4",  css::parse_frequency,  349.23, css::frequency::hertz, false);
	test_parser("F#4", css::parse_frequency,  369.99, css::frequency::hertz, false);
	test_parser("Gb4", css::parse_frequency,  369.99, css::frequency::hertz, false);
	test_parser("G4",  css::parse_frequency,  392.00, css::frequency::hertz, false);
	test_parser("G#4", css::parse_frequency,  415.30, css::frequency::hertz, false);
	test_parser("Ab4", css::parse_frequency,  415.30, css::frequency::hertz, false);
	test_parser("A4",  css::parse_frequency,  440.00, css::frequency::hertz, false);
	test_parser("A#4", css::parse_frequency,  466.16, css::frequency::hertz, false);
	test_parser("Bb4", css::parse_frequency,  466.16, css::frequency::hertz, false);
	test_parser("B4",  css::parse_frequency,  493.88, css::frequency::hertz, false);

	test_parser("C5",  css::parse_frequency,  523.25, css::frequency::hertz, false);
	test_parser("C#5", css::parse_frequency,  554.37, css::frequency::hertz, false);
	test_parser("Db5", css::parse_frequency,  554.37, css::frequency::hertz, false);
	test_parser("D5",  css::parse_frequency,  587.33, css::frequency::hertz, false);
	test_parser("D#5", css::parse_frequency,  622.25, css::frequency::hertz, false);
	test_parser("Eb5", css::parse_frequency,  622.25, css::frequency::hertz, false);
	test_parser("E5",  css::parse_frequency,  659.25, css::frequency::hertz, false);
	test_parser("F5",  css::parse_frequency,  698.46, css::frequency::hertz, false);
	test_parser("F#5", css::parse_frequency,  739.99, css::frequency::hertz, false);
	test_parser("Gb5", css::parse_frequency,  739.99, css::frequency::hertz, false);
	test_parser("G5",  css::parse_frequency,  783.99, css::frequency::hertz, false);
	test_parser("G#5", css::parse_frequency,  830.61, css::frequency::hertz, false);
	test_parser("Ab5", css::parse_frequency,  830.61, css::frequency::hertz, false);
	test_parser("A5",  css::parse_frequency,  880.00, css::frequency::hertz, false);
	test_parser("A#5", css::parse_frequency,  932.33, css::frequency::hertz, false);
	test_parser("Bb5", css::parse_frequency,  932.33, css::frequency::hertz, false);
	test_parser("B5",  css::parse_frequency,  987.77, css::frequency::hertz, false);

	test_parser("C6",  css::parse_frequency, 1046.50, css::frequency::hertz, false);
	test_parser("C#6", css::parse_frequency, 1108.73, css::frequency::hertz, false);
	test_parser("Db6", css::parse_frequency, 1108.73, css::frequency::hertz, false);
	test_parser("D6",  css::parse_frequency, 1174.66, css::frequency::hertz, false);
	test_parser("D#6", css::parse_frequency, 1244.51, css::frequency::hertz, false);
	test_parser("Eb6", css::parse_frequency, 1244.51, css::frequency::hertz, false);
	test_parser("E6",  css::parse_frequency, 1318.51, css::frequency::hertz, false);
	test_parser("F6",  css::parse_frequency, 1396.91, css::frequency::hertz, false);
	test_parser("F#6", css::parse_frequency, 1479.98, css::frequency::hertz, false);
	test_parser("Gb6", css::parse_frequency, 1479.98, css::frequency::hertz, false);
	test_parser("G6",  css::parse_frequency, 1567.98, css::frequency::hertz, false);
	test_parser("G#6", css::parse_frequency, 1661.22, css::frequency::hertz, false);
	test_parser("Ab6", css::parse_frequency, 1661.22, css::frequency::hertz, false);
	test_parser("A6",  css::parse_frequency, 1760.00, css::frequency::hertz, false);
	test_parser("A#6", css::parse_frequency, 1864.66, css::frequency::hertz, false);
	test_parser("Bb6", css::parse_frequency, 1864.66, css::frequency::hertz, false);
	test_parser("B6",  css::parse_frequency, 1975.53, css::frequency::hertz, false);

	test_parser("C7",  css::parse_frequency, 2093.00, css::frequency::hertz, false);
	test_parser("C#7", css::parse_frequency, 2217.46, css::frequency::hertz, false);
	test_parser("Db7", css::parse_frequency, 2217.46, css::frequency::hertz, false);
	test_parser("D7",  css::parse_frequency, 2349.32, css::frequency::hertz, false);
	test_parser("D#7", css::parse_frequency, 2489.02, css::frequency::hertz, false);
	test_parser("Eb7", css::parse_frequency, 2489.02, css::frequency::hertz, false);
	test_parser("E7",  css::parse_frequency, 2637.02, css::frequency::hertz, false);
	test_parser("F7",  css::parse_frequency, 2793.83, css::frequency::hertz, false);
	test_parser("F#7", css::parse_frequency, 2959.96, css::frequency::hertz, false);
	test_parser("Gb7", css::parse_frequency, 2959.96, css::frequency::hertz, false);
	test_parser("G7",  css::parse_frequency, 3135.96, css::frequency::hertz, false);
	test_parser("G#7", css::parse_frequency, 3322.44, css::frequency::hertz, false);
	test_parser("Ab7", css::parse_frequency, 3322.44, css::frequency::hertz, false);
	test_parser("A7",  css::parse_frequency, 3520.00, css::frequency::hertz, false);
	test_parser("A#7", css::parse_frequency, 3729.31, css::frequency::hertz, false);
	test_parser("Bb7", css::parse_frequency, 3729.31, css::frequency::hertz, false);
	test_parser("B7",  css::parse_frequency, 3951.07, css::frequency::hertz, false);

	test_parser("C8",  css::parse_frequency, 4186.01, css::frequency::hertz, false);
	test_parser("C#8", css::parse_frequency, 4434.92, css::frequency::hertz, false);
	test_parser("Db8", css::parse_frequency, 4434.92, css::frequency::hertz, false);
	test_parser("D8",  css::parse_frequency, 4698.63, css::frequency::hertz, false);
	test_parser("D#8", css::parse_frequency, 4978.03, css::frequency::hertz, false);
	test_parser("Eb8", css::parse_frequency, 4978.03, css::frequency::hertz, false);
	test_parser("E8",  css::parse_frequency, 5274.04, css::frequency::hertz, false);
	test_parser("F8",  css::parse_frequency, 5587.65, css::frequency::hertz, false);
	test_parser("F#8", css::parse_frequency, 5919.91, css::frequency::hertz, false);
	test_parser("Gb8", css::parse_frequency, 5919.91, css::frequency::hertz, false);
	test_parser("G8",  css::parse_frequency, 6271.93, css::frequency::hertz, false);
	test_parser("G#8", css::parse_frequency, 6644.88, css::frequency::hertz, false);
	test_parser("Ab8", css::parse_frequency, 6644.88, css::frequency::hertz, false);
	test_parser("A8",  css::parse_frequency, 7040.00, css::frequency::hertz, false);
	test_parser("A#8", css::parse_frequency, 7458.62, css::frequency::hertz, false);
	test_parser("Bb8", css::parse_frequency, 7458.62, css::frequency::hertz, false);
	test_parser("B8",  css::parse_frequency, 7902.13, css::frequency::hertz, false);
}

TEST_CASE("frequency conversion")
{
	test_conversion(1, css::frequency::inherit, 1, css::frequency::inherit,   true);
	test_conversion(1, css::frequency::inherit, 1, css::frequency::hertz,     true);
	test_conversion(1, css::frequency::inherit, 1, css::frequency::kilohertz, true);

	test_conversion(   1, css::frequency::hertz, 1,     css::frequency::inherit,   true);
	test_conversion(   1, css::frequency::hertz, 1,     css::frequency::hertz,     false);
	test_conversion(1000, css::frequency::hertz, 1,     css::frequency::kilohertz, false);
	test_conversion( 500, css::frequency::hertz, 0.5,   css::frequency::kilohertz, false);
	test_conversion(  20, css::frequency::hertz, 0.020, css::frequency::kilohertz, false);
	test_conversion(   1, css::frequency::hertz, 0.001, css::frequency::kilohertz, false);

	test_conversion(1,     css::frequency::kilohertz,    1, css::frequency::inherit,   true);
	test_conversion(1,     css::frequency::kilohertz, 1000, css::frequency::hertz,     false);
	test_conversion(0.9,   css::frequency::kilohertz,  900, css::frequency::hertz,     false);
	test_conversion(0.06,  css::frequency::kilohertz,   60, css::frequency::hertz,     false);
	test_conversion(0.003, css::frequency::kilohertz,    3, css::frequency::hertz,     false);
	test_conversion(1,     css::frequency::kilohertz,    1, css::frequency::kilohertz, false);
}

TEST_CASE("style defaults")
{
	css::styles styles("defaults");
	assert(styles.name == "defaults");
	assert(styles.display == css::display::inherit);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - unknown")
{
	css::styles styles = cainteoir::unknown;
	assert(styles.name == "unknown");
	assert(styles.display == css::display::inherit);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - sequential")
{
	css::styles styles = cainteoir::sequential;
	assert(styles.name == "sequential");
	assert(styles.display == css::display::inherit);
	assert(styles.media_synchronisation == css::media_synchronisation::sequential);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - parallel")
{
	css::styles styles = cainteoir::parallel;
	assert(styles.name == "parallel");
	assert(styles.display == css::display::inherit);
	assert(styles.media_synchronisation == css::media_synchronisation::parallel);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - hidden")
{
	css::styles styles = cainteoir::hidden;
	assert(styles.name == "hidden");
	assert(styles.display == css::display::none);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - block")
{
	css::styles styles = cainteoir::block;
	assert(styles.name == "block");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - paragraph")
{
	css::styles styles = cainteoir::paragraph;
	assert(styles.name == "paragraph");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::paragraph);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - heading0")
{
	css::styles styles = cainteoir::heading0;
	assert(styles.name == "heading0");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::normal);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 2);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::heading);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - heading1")
{
	css::styles styles = cainteoir::heading1;
	assert(styles.name == "heading1");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::normal);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 2);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::heading);
	assert(styles.aria_level == 1);
}

TEST_CASE("style - heading2")
{
	css::styles styles = cainteoir::heading2;
	assert(styles.name == "heading2");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::normal);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1.5);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 2);
	assert(styles.role == css::role::heading);
	assert(styles.aria_level == 2);
}

TEST_CASE("style - heading3")
{
	css::styles styles = cainteoir::heading3;
	assert(styles.name == "heading3");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::bold);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::heading);
	assert(styles.aria_level == 3);
}

TEST_CASE("style - heading4")
{
	css::styles styles = cainteoir::heading4;
	assert(styles.name == "heading4");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::normal);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::heading);
	assert(styles.aria_level == 4);
}

TEST_CASE("style - heading5")
{
	css::styles styles = cainteoir::heading5;
	assert(styles.name == "heading5");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::left);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::bold);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::heading);
	assert(styles.aria_level == 5);
}

TEST_CASE("style - heading6")
{
	css::styles styles = cainteoir::heading6;
	assert(styles.name == "heading6");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::left);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::bold);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::heading);
	assert(styles.aria_level == 6);
}

TEST_CASE("style - span")
{
	css::styles styles = cainteoir::span;
	assert(styles.name == "span");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - sentence")
{
	css::styles styles = cainteoir::sentence;
	assert(styles.name == "sentence");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::sentence);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - superscript")
{
	css::styles styles = cainteoir::superscript;
	assert(styles.name == "superscript");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::super);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 0.75);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - subscript")
{
	css::styles styles = cainteoir::subscript;
	assert(styles.name == "subscript");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::sub);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 0.75);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - emphasized")
{
	css::styles styles = cainteoir::emphasized;
	assert(styles.name == "emphasized");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::italic);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - emphasized-block")
{
	css::styles styles = cainteoir::emphasized_block;
	assert(styles.name == "emphasized_block");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::italic);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::paragraph);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - strong")
{
	css::styles styles = cainteoir::strong;
	assert(styles.name == "strong");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::bold);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - reduced")
{
	css::styles styles = cainteoir::reduced;
	assert(styles.name == "reduced");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::normal);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - underlined")
{
	css::styles styles = cainteoir::underlined;
	assert(styles.name == "underlined");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::underline);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - monospace")
{
	css::styles styles = cainteoir::monospace;
	assert(styles.name == "monospace");
	assert(styles.display == css::display::inlined);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "monospace");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - monospace-block")
{
	css::styles styles = cainteoir::monospace_block;
	assert(styles.name == "monospace_block");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::preserved);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "monospace");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - bullet_list")
{
	css::styles styles = cainteoir::bullet_list;
	assert(styles.name == "bullet_list");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type == "disc");
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - number_list")
{
	css::styles styles = cainteoir::number_list;
	assert(styles.name == "number_list");
	assert(styles.display == css::display::block);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type == "decimal");
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - list_item")
{
	css::styles styles = cainteoir::list_item;
	assert(styles.name == "list_item");
	assert(styles.display == css::display::list_item);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size.units() == css::length::picas);
	assert(styles.font_size.value() == 1);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - table")
{
	css::styles styles = cainteoir::table;
	assert(styles.name == "table");
	assert(styles.display == css::display::table);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - table_row")
{
	css::styles styles = cainteoir::table_row;
	assert(styles.name == "table_row");
	assert(styles.display == css::display::table_row);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::picas);
	assert(styles.margin.bottom.value() == 1);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}

TEST_CASE("style - table_cell")
{
	css::styles styles = cainteoir::table_cell;
	assert(styles.name == "table_cell");
	assert(styles.display == css::display::table_cell);
	assert(styles.media_synchronisation == css::media_synchronisation::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
	assert(styles.whitespace == css::whitespace::normal);
	assert(styles.font_style == css::font_style::inherit);
	assert(styles.font_variant_caps == css::font_variant_caps::inherit);
	assert(styles.font_weight == css::font_weight::inherit);
	assert(styles.list_style_type.empty());
	assert(styles.font_family.empty());
	assert(styles.font_size.units() == css::length::inherit);
	assert(styles.font_size.value() == 0);
	assert(styles.margin.left.units() == css::length::inherit);
	assert(styles.margin.left.value() == 0);
	assert(styles.margin.top.units() == css::length::inherit);
	assert(styles.margin.top.value() == 0);
	assert(styles.margin.right.units() == css::length::inherit);
	assert(styles.margin.right.value() == 0);
	assert(styles.margin.bottom.units() == css::length::inherit);
	assert(styles.margin.bottom.value() == 0);
	assert(styles.role == css::role::none);
	assert(styles.aria_level == 0);
}
