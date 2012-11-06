/* Document Content Rendering Model tests.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

REGISTER_TESTSUITE("content");

TEST_CASE("style defaults")
{
	cainteoir::styles styles("defaults");
	assert(styles.name == "defaults");
	assert(styles.display == cainteoir::display::inherit);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - unknown")
{
	cainteoir::styles styles = cainteoir::unknown;
	assert(styles.name == "unknown");
	assert(styles.display == cainteoir::display::inherit);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - paragraph")
{
	cainteoir::styles styles = cainteoir::paragraph;
	assert(styles.name == "paragraph");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size == 11);
}

TEST_CASE("style - heading1")
{
	cainteoir::styles styles = cainteoir::heading1;
	assert(styles.name == "heading1");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::center);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::bold);
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size == 24);
}

TEST_CASE("style - heading2")
{
	cainteoir::styles styles = cainteoir::heading2;
	assert(styles.name == "heading2");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::center);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::bold);
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size == 18);
}

TEST_CASE("style - heading3")
{
	cainteoir::styles styles = cainteoir::heading3;
	assert(styles.name == "heading3");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::center);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::bold);
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size == 11);
}

TEST_CASE("style - heading4")
{
	cainteoir::styles styles = cainteoir::heading4;
	assert(styles.name == "heading4");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::center);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::bold);
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size == 11);
}

TEST_CASE("style - heading5")
{
	cainteoir::styles styles = cainteoir::heading5;
	assert(styles.name == "heading5");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::center);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::bold);
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size == 11);
}

TEST_CASE("style - heading6")
{
	cainteoir::styles styles = cainteoir::heading6;
	assert(styles.name == "heading6");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::center);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::bold);
	assert(styles.font_family == "sans-serif");
	assert(styles.font_size == 11);
}

TEST_CASE("style - span")
{
	cainteoir::styles styles = cainteoir::span;
	assert(styles.name == "span");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - sentence")
{
	cainteoir::styles styles = cainteoir::sentence;
	assert(styles.name == "sentence");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - superscript")
{
	cainteoir::styles styles = cainteoir::superscript;
	assert(styles.name == "superscript");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::super);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - subscript")
{
	cainteoir::styles styles = cainteoir::subscript;
	assert(styles.name == "subscript");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::sub);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - emphasized")
{
	cainteoir::styles styles = cainteoir::emphasized;
	assert(styles.name == "emphasized");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::italic);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - strong")
{
	cainteoir::styles styles = cainteoir::strong;
	assert(styles.name == "strong");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::bold);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - reduced")
{
	cainteoir::styles styles = cainteoir::reduced;
	assert(styles.name == "reduced");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::normal);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - underlined")
{
	cainteoir::styles styles = cainteoir::underlined;
	assert(styles.name == "underlined");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::underline);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - monospace")
{
	cainteoir::styles styles = cainteoir::monospace;
	assert(styles.name == "monospace");
	assert(styles.display == cainteoir::display::inlined);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family == "monospace");
	assert(styles.font_size == 11);
}

TEST_CASE("style - bullet_list")
{
	cainteoir::styles styles = cainteoir::bullet_list;
	assert(styles.name == "bullet_list");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - number_list")
{
	cainteoir::styles styles = cainteoir::number_list;
	assert(styles.name == "number_list");
	assert(styles.display == cainteoir::display::block);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - list_item")
{
	cainteoir::styles styles = cainteoir::list_item;
	assert(styles.name == "list_item");
	assert(styles.display == cainteoir::display::list_item);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - table")
{
	cainteoir::styles styles = cainteoir::table;
	assert(styles.name == "table");
	assert(styles.display == cainteoir::display::table);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - table_row")
{
	cainteoir::styles styles = cainteoir::table_row;
	assert(styles.name == "table_row");
	assert(styles.display == cainteoir::display::table_row);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}

TEST_CASE("style - table_cell")
{
	cainteoir::styles styles = cainteoir::table_cell;
	assert(styles.name == "table_cell");
	assert(styles.display == cainteoir::display::table_cell);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}
