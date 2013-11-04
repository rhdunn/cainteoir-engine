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
template <> struct unit_type<css::length::type> { typedef css::length type; };
template <> struct unit_type<css::time::type>   { typedef css::time   type; };

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

static void test_parser_(const char *aString, const css::time::parse_as_type aParseAs,
                         float aValue, const css::time::type aUnits,
                         bool throws,
                         const char *location, int line)
{
	try
	{
		css::time value = css::time(aString, aParseAs);
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
	test_parser(nullptr, css::time::css_value,  0, css::time::inherit, false);
	test_parser(nullptr, css::time::smil_value, 0, css::time::inherit, false);
}

TEST_CASE("time parser - SS (seconds)")
{
	test_parser("1",       css::time::smil_value,       1, css::time::seconds, false);
	test_parser("25",      css::time::smil_value,      25, css::time::seconds, false);
	test_parser("1500000", css::time::smil_value, 1500000, css::time::seconds, false); // 416 hours, 40 min
}

TEST_CASE("time parser - SS.mmm (seconds, milliseconds)")
{
	test_parser( "1.",    css::time::smil_value,  1,     css::time::seconds, false);
	test_parser( "0.5",   css::time::smil_value,  0.5,   css::time::seconds, false);
	test_parser(  ".6",   css::time::smil_value,  0.6,   css::time::seconds, false);
	test_parser( "1.2",   css::time::smil_value,  1.2,   css::time::seconds, false);
	test_parser("12.345", css::time::smil_value, 12.345, css::time::seconds, false);

	test_parser( "1..3",  css::time::smil_value, 1, css::time::inherit, true); // invalid
	test_parser( "1.2.3", css::time::smil_value, 1, css::time::inherit, true); // invalid
}

TEST_CASE("time parser - MM:SS (minutes, seconds)")
{
	test_parser("0:23", css::time::smil_value,  23, css::time::seconds, false);
	test_parser("1:04", css::time::smil_value,  64, css::time::seconds, false);

	test_parser("00:23", css::time::smil_value,  23, css::time::seconds, false);
	test_parser("01:04", css::time::smil_value,  64, css::time::seconds, false);
	test_parser("12:32", css::time::smil_value, 752, css::time::seconds, false);
	test_parser("04:00", css::time::smil_value, 240, css::time::seconds, false);

	test_parser( ":25", css::time::smil_value, 1, css::time::inherit, true); // invalid
	test_parser( "1:",  css::time::smil_value, 1, css::time::inherit, true); // invalid

	test_parser( "1:2", css::time::smil_value, 1, css::time::inherit, true); // invalid
}

TEST_CASE("time parser - MM:SS.mmm (minutes, seconds)")
{
	test_parser( "0:12.5",  css::time::smil_value,  12.5,  css::time::seconds, false);
	test_parser( "2:01.22", css::time::smil_value, 121.22, css::time::seconds, false);

	test_parser("00:12.5",   css::time::smil_value,  12.5,   css::time::seconds, false);
	test_parser("02:01.22",  css::time::smil_value, 121.22,  css::time::seconds, false);
	test_parser("10:30.75",  css::time::smil_value, 630.75,  css::time::seconds, false);
	test_parser("03:00.333", css::time::smil_value, 180.333, css::time::seconds, false);
}

TEST_CASE("time parser - HH:MM:SS (hours, minutes, seconds)")
{
	test_parser(  "0:00:23", css::time::smil_value,     23, css::time::seconds, false);
	test_parser(  "1:01:04", css::time::smil_value,   3664, css::time::seconds, false);
	test_parser( "11:00:23", css::time::smil_value,  39623, css::time::seconds, false);
	test_parser( "24:42:00", css::time::smil_value,  88920, css::time::seconds, false);
	test_parser("140:12:32", css::time::smil_value, 504752, css::time::seconds, false);

	test_parser( ":00:25", css::time::smil_value, 1, css::time::inherit, true); // invalid
	test_parser( "0::25",  css::time::smil_value, 1, css::time::inherit, true); // invalid
	test_parser( "0:01:",  css::time::smil_value, 1, css::time::inherit, true); // invalid

	test_parser( "0:0:25", css::time::smil_value, 1, css::time::inherit, true); // invalid
	test_parser( "0:00:5", css::time::smil_value, 1, css::time::inherit, true); // invalid
}

TEST_CASE("time parser - HH:MM:SS.mmm (hours, minutes, seconds, milliseconds)")
{
	test_parser(  "0:00:23.1", css::time::smil_value,     23.1, css::time::seconds, false);
	test_parser(  "1:01:04.2", css::time::smil_value,   3664.2, css::time::seconds, false);
	test_parser( "11:00:23.3", css::time::smil_value,  39623.3, css::time::seconds, false);
	test_parser( "24:42:00.4", css::time::smil_value,  88920.4, css::time::seconds, false);
	test_parser("140:12:32.5", css::time::smil_value, 504752.5, css::time::seconds, false);
}

TEST_CASE("time parser - DD:HH:MM:SS.mmm (days, hours, minutes, seconds, milliseconds)")
{
	// These are invalid, only upto hours are allowed by SMIL clock values ...

	test_parser( "0:00:00:00", css::time::smil_value, 1, css::time::inherit, true);
	test_parser( "1:02:03:04", css::time::smil_value, 1, css::time::inherit, true);
	test_parser("11:22:43:44", css::time::smil_value, 1, css::time::inherit, true);
}

TEST_CASE("time parser - #h (hours)")
{
	test_parser("2h",   css::time::smil_value, 7200, css::time::seconds, false);
	test_parser("2.5h", css::time::smil_value, 9000, css::time::seconds, false);
}

TEST_CASE("time parser - #min (seconds)")
{
	test_parser("5min",    css::time::smil_value, 300, css::time::seconds, false);
	test_parser("2.25min", css::time::smil_value, 135, css::time::seconds, false);
}

TEST_CASE("time parser - #s (seconds)")
{
	test_parser("25s",  css::time::smil_value, 25,   css::time::seconds, false);
	test_parser("1.2s", css::time::smil_value,  1.2, css::time::seconds, false);
}

TEST_CASE("time parser - #ms (milliseconds)")
{
	test_parser("11ms",  css::time::smil_value, 11,   css::time::milliseconds, false);
	test_parser("9.7ms", css::time::smil_value,  9.7, css::time::milliseconds, false);
}

TEST_CASE("time parser - invalid suffixes")
{
	test_parser("2d",    css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2ha",   css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2mun",  css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2m",    css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2mi",   css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2mio",  css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2mins", css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2mo",   css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2mss",  css::time::smil_value, 1, css::time::inherit, true);
	test_parser("2st",   css::time::smil_value, 1, css::time::inherit, true);
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

TEST_CASE("style defaults")
{
	css::styles styles("defaults");
	assert(styles.name == "defaults");
	assert(styles.display == css::display::inherit);
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
