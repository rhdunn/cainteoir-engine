/* Document Content Rendering Model.
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

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/content.hpp>

const cainteoir::styles cainteoir::unknown =
{
	"unknown",
	cainteoir::display::inherit,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::paragraph =
{
	"paragraph",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"sans-serif",
	11,
	{},
};

const cainteoir::styles cainteoir::heading1 =
{
	"heading1",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::center,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"sans-serif",
	24,
	{},
};

const cainteoir::styles cainteoir::heading2 =
{
	"heading2",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::center,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"sans-serif",
	18,
	{},
};

const cainteoir::styles cainteoir::heading3 =
{
	"heading3",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::center,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"sans-serif",
	11,
	{},
};

const cainteoir::styles cainteoir::heading4 =
{
	"heading4",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::center,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"sans-serif",
	11,
	{},
};

const cainteoir::styles cainteoir::heading5 =
{
	"heading5",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::center,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"sans-serif",
	11,
	{},
};

const cainteoir::styles cainteoir::heading6 =
{
	"heading6",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::center,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"sans-serif",
	11,
	{},
};

const cainteoir::styles cainteoir::span =
{
	"span",
	cainteoir::display::inlined,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::sentence =
{
	"sentence",
	cainteoir::display::inlined,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::superscript =
{
	"superscript",
	cainteoir::display::inlined,
	cainteoir::vertical_align::super,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::subscript =
{
	"subscript",
	cainteoir::display::inlined,
	cainteoir::vertical_align::sub,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::emphasized =
{
	"emphasized",
	cainteoir::display::inlined,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::italic,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::strong =
{
	"strong",
	cainteoir::display::inlined,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::reduced =
{
	"reduced",
	cainteoir::display::inlined,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::normal,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::underlined =
{
	"underlined",
	cainteoir::display::inlined,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::underline,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::monospace =
{
	"monospace",
	cainteoir::display::inlined,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"monospace",
	11,
	{},
};

const cainteoir::styles cainteoir::bullet_list =
{
	"bullet_list",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::number_list =
{
	"number_list",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::list_item =
{
	"list_item",
	cainteoir::display::list_item,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::table =
{
	"table",
	cainteoir::display::table,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::table_row =
{
	"table_row",
	cainteoir::display::table_row,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};

const cainteoir::styles cainteoir::table_cell =
{
	"table_cell",
	cainteoir::display::table_cell,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	0,
	{},
};
