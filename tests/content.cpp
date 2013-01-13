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

static void test_conversion_(float aFromValue, const css::length::type aFromUnits,
                             float aToValue, const css::length::type aToUnits,
                             bool throws,
                             const char *location, int line)
{
	try
	{
		css::length to = css::length(aFromValue, aFromUnits).as(aToUnits);
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

TEST_CASE("unit conversion")
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

TEST_CASE("style defaults")
{
	css::styles styles("defaults");
	assert(styles.name == "defaults");
	assert(styles.display == css::display::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - unknown")
{
	css::styles styles = cainteoir::unknown;
	assert(styles.name == "unknown");
	assert(styles.display == css::display::inherit);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - paragraph")
{
	css::styles styles = cainteoir::paragraph;
	assert(styles.name == "paragraph");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::paragraph);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - heading0")
{
	css::styles styles = cainteoir::heading0;
	assert(styles.name == "heading0");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::heading);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - heading1")
{
	css::styles styles = cainteoir::heading1;
	assert(styles.name == "heading1");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::heading);
	assert(styles.toc_level == 1);
}

TEST_CASE("style - heading2")
{
	css::styles styles = cainteoir::heading2;
	assert(styles.name == "heading2");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::heading);
	assert(styles.toc_level == 2);
}

TEST_CASE("style - heading3")
{
	css::styles styles = cainteoir::heading3;
	assert(styles.name == "heading3");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::heading);
	assert(styles.toc_level == 3);
}

TEST_CASE("style - heading4")
{
	css::styles styles = cainteoir::heading4;
	assert(styles.name == "heading4");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::center);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::heading);
	assert(styles.toc_level == 4);
}

TEST_CASE("style - heading5")
{
	css::styles styles = cainteoir::heading5;
	assert(styles.name == "heading5");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::left);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::heading);
	assert(styles.toc_level == 5);
}

TEST_CASE("style - heading6")
{
	css::styles styles = cainteoir::heading6;
	assert(styles.name == "heading6");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::left);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::heading);
	assert(styles.toc_level == 6);
}

TEST_CASE("style - span")
{
	css::styles styles = cainteoir::span;
	assert(styles.name == "span");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - sentence")
{
	css::styles styles = cainteoir::sentence;
	assert(styles.name == "sentence");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::sentence);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - superscript")
{
	css::styles styles = cainteoir::superscript;
	assert(styles.name == "superscript");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::super);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - subscript")
{
	css::styles styles = cainteoir::subscript;
	assert(styles.name == "subscript");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::sub);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - emphasized")
{
	css::styles styles = cainteoir::emphasized;
	assert(styles.name == "emphasized");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - emphasized-block")
{
	css::styles styles = cainteoir::emphasized_block;
	assert(styles.name == "emphasized_block");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::paragraph);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - strong")
{
	css::styles styles = cainteoir::strong;
	assert(styles.name == "strong");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - reduced")
{
	css::styles styles = cainteoir::reduced;
	assert(styles.name == "reduced");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - underlined")
{
	css::styles styles = cainteoir::underlined;
	assert(styles.name == "underlined");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::underline);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - monospace")
{
	css::styles styles = cainteoir::monospace;
	assert(styles.name == "monospace");
	assert(styles.display == css::display::inlined);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - monospace-block")
{
	css::styles styles = cainteoir::monospace_block;
	assert(styles.name == "monospace_block");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - bullet_list")
{
	css::styles styles = cainteoir::bullet_list;
	assert(styles.name == "bullet_list");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - number_list")
{
	css::styles styles = cainteoir::number_list;
	assert(styles.name == "number_list");
	assert(styles.display == css::display::block);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - list_item")
{
	css::styles styles = cainteoir::list_item;
	assert(styles.name == "list_item");
	assert(styles.display == css::display::list_item);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - table")
{
	css::styles styles = cainteoir::table;
	assert(styles.name == "table");
	assert(styles.display == css::display::table);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - table_row")
{
	css::styles styles = cainteoir::table_row;
	assert(styles.name == "table_row");
	assert(styles.display == css::display::table_row);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}

TEST_CASE("style - table_cell")
{
	css::styles styles = cainteoir::table_cell;
	assert(styles.name == "table_cell");
	assert(styles.display == css::display::table_cell);
	assert(styles.vertical_align == css::vertical_align::inherit);
	assert(styles.text_align == css::text_align::inherit);
	assert(styles.text_decoration == css::text_decoration::inherit);
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
	assert(styles.text_structure == css::text_structure::none);
	assert(styles.toc_level == 0);
}
