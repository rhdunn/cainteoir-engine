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
	}
	return infinite;
}

const css::styles cainteoir::unknown =
{
	"unknown",
	css::display::inherit,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::paragraph =
{
	"paragraph",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"sans-serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::paragraph,
	0,
};

const css::styles cainteoir::heading0 =
{
	"heading0",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 2, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	0,
};

const css::styles cainteoir::heading1 =
{
	"heading1",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 2, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	1,
};

const css::styles cainteoir::heading2 =
{
	"heading2",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 1.5, css::length::picas },
	{ {}, {}, {}, { 2, css::length::picas } },
	css::text_structure::heading,
	2,
};

const css::styles cainteoir::heading3 =
{
	"heading3",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	3,
};

const css::styles cainteoir::heading4 =
{
	"heading4",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	4,
};

const css::styles cainteoir::heading5 =
{
	"heading5",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::left,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	5,
};

const css::styles cainteoir::heading6 =
{
	"heading6",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::left,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::heading,
	6,
};

const css::styles cainteoir::span =
{
	"span",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::sentence =
{
	"sentence",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::sentence,
	0,
};

const css::styles cainteoir::superscript =
{
	"superscript",
	css::display::inlined,
	css::vertical_align::super,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{ 0.75, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::subscript =
{
	"subscript",
	css::display::inlined,
	css::vertical_align::sub,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{ 0.75, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::emphasized =
{
	"emphasized",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::italic,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::emphasized_block =
{
	"emphasized_block",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::italic,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::paragraph,
	0,
};

const css::styles cainteoir::strong =
{
	"strong",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::bold,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::reduced =
{
	"reduced",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::normal,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::underlined =
{
	"underlined",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::underline,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::monospace =
{
	"monospace",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"monospace",
	{ 1, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::monospace_block =
{
	"monospace_block",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"monospace",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::bullet_list =
{
	"bullet_list",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"disc",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::number_list =
{
	"number_list",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"decimal",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::list_item =
{
	"list_item",
	css::display::list_item,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"sans-serif",
	{ 1, css::length::picas },
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::table =
{
	"table",
	css::display::table,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};

const css::styles cainteoir::table_row =
{
	"table_row",
	css::display::table_row,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::text_structure::none,
	0,
};

const css::styles cainteoir::table_cell =
{
	"table_cell",
	css::display::table_cell,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::font_style::inherit,
	css::font_variant::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::text_structure::none,
	0,
};
