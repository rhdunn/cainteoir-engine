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
#include <stdexcept>
#include <sstream>
#include <limits>

cainteoir::size cainteoir::size::as(const size_units aUnits) const
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
	case size_units::millimeters:
		switch (aUnits)
		{
		case size_units::millimeters:
			return *this;
		case size_units::centimeters:
			return size(mValue / millimeters_in_centimeter, aUnits);
		case size_units::inches:
			return size(mValue / millimeters_in_inch, aUnits);
		case size_units::points:
			return size(mValue / millimeters_in_point, aUnits);
		case size_units::picas:
			return size(mValue / millimeters_in_pica, aUnits);
		case size_units::pixels:
			return size(mValue * pixels_in_millimeter, aUnits);
		}
		break;
	case size_units::centimeters:
		switch (aUnits)
		{
		case size_units::millimeters:
			return size(mValue * millimeters_in_centimeter, aUnits);
		case size_units::centimeters:
			return *this;
		case size_units::inches:
			return size(mValue / centimeters_in_inch, aUnits);
		case size_units::points:
			return size(mValue / centimeters_in_point, aUnits);
		case size_units::picas:
			return size(mValue / centimeters_in_pica, aUnits);
		case size_units::pixels:
			return size(mValue * pixels_in_centimeter, aUnits);
		}
		break;
	case size_units::inches:
		switch (aUnits)
		{
		case size_units::millimeters:
			return size(mValue * millimeters_in_inch, aUnits);
		case size_units::centimeters:
			return size(mValue * centimeters_in_inch, aUnits);
		case size_units::inches:
			return *this;
		case size_units::points:
			return size(mValue * points_in_inch, aUnits);
		case size_units::picas:
			return size(mValue * picas_in_inch, aUnits);
		case size_units::pixels:
			return size(mValue * pixels_in_inch, aUnits);
		}
		break;
	case size_units::points:
		switch (aUnits)
		{
		case size_units::millimeters:
			return size(mValue * millimeters_in_point, aUnits);
		case size_units::centimeters:
			return size(mValue * centimeters_in_point, aUnits);
		case size_units::inches:
			return size(mValue / points_in_inch, aUnits);
		case size_units::points:
			return *this;
		case size_units::picas:
			return size(mValue / points_in_pica, aUnits);
		case size_units::pixels:
			return size(mValue * pixels_in_point, aUnits);
		}
		break;
	case size_units::picas:
		switch (aUnits)
		{
		case size_units::millimeters:
			return size(mValue * millimeters_in_pica, aUnits);
		case size_units::centimeters:
			return size(mValue * centimeters_in_pica, aUnits);
		case size_units::inches:
			return size(mValue / picas_in_inch, aUnits);
		case size_units::points:
			return size(mValue * points_in_pica, aUnits);
		case size_units::picas:
			return *this;
		case size_units::pixels:
			return size(mValue * pixels_in_pica, aUnits);
		}
		break;
	case size_units::pixels:
		switch (aUnits)
		{
		case size_units::millimeters:
			return size(mValue / pixels_in_millimeter, aUnits);
		case size_units::centimeters:
			return size(mValue / pixels_in_centimeter, aUnits);
		case size_units::inches:
			return size(mValue / pixels_in_inch, aUnits);
		case size_units::points:
			return size(mValue / pixels_in_point, aUnits);
		case size_units::picas:
			return size(mValue / pixels_in_pica, aUnits);
		case size_units::pixels:
			return *this;
		}
		break;
	}

	throw std::runtime_error("unable to convert to the specified units");
}

std::string cainteoir::counter_style::marker(value_t count) const
{
	std::ostringstream textval;
	textval << prefix;
	int n = symbols.size();
	if (n != 0) switch (system)
	{
	case cainteoir::counter_system::cyclic:
		textval << symbols[count % n];
		break;
	case cainteoir::counter_system::numeric:
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

const cainteoir::counter_style::range_t cainteoir::counter_style::infinite = {
	std::numeric_limits<value_t>::min(),
	std::numeric_limits<value_t>::max()
};

const cainteoir::counter_style::range_t cainteoir::counter_style::get_auto_range(counter_system system)
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

const cainteoir::counter_style *cainteoir::style_manager::get_counter_style(const std::string &aName) const
{
	auto item = mCounterStyles.find(aName);
	if (item != mCounterStyles.end())
		return item->second;
	return nullptr;
}

cainteoir::counter_style *cainteoir::style_manager::create_counter_style(const std::string &aName)
{
	std::shared_ptr<counter_style> style = std::make_shared<counter_style>();
	mCounterStyleRegistry.push_back(style);
	mCounterStyles[aName] = style.get();
	return style.get();
}

cainteoir::style_manager::style_manager()
{
	counter_style *decimal = create_counter_style("decimal");
	decimal->system = counter_system::numeric;
	decimal->range  = counter_style::get_auto_range(decimal->system);
	decimal->symbols.push_back("0");
	decimal->symbols.push_back("1");
	decimal->symbols.push_back("2");
	decimal->symbols.push_back("3");
	decimal->symbols.push_back("4");
	decimal->symbols.push_back("5");
	decimal->symbols.push_back("6");
	decimal->symbols.push_back("7");
	decimal->symbols.push_back("8");
	decimal->symbols.push_back("9");

	counter_style *disc = create_counter_style("disc");
	disc->system = counter_system::cyclic;
	disc->range  = counter_style::get_auto_range(disc->system);
	disc->suffix = "";
	disc->symbols.push_back("\xE2\x80\xA2"); // \2022
}

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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	"sans-serif",
	{ 1, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::paragraph,
	0,
};

const cainteoir::styles cainteoir::heading0 =
{
	"heading0",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::center,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::normal,
	"",
	"serif",
	{ 2, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::heading,
	0,
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
	cainteoir::font_weight::normal,
	"",
	"serif",
	{ 2, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::heading,
	1,
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
	cainteoir::font_weight::normal,
	"",
	"serif",
	{ 1.5, cainteoir::size_units::picas },
	{ {}, {}, {}, { 2, cainteoir::size_units::picas } },
	cainteoir::text_structure::heading,
	2,
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
	"",
	"serif",
	{ 1, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::heading,
	3,
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
	cainteoir::font_weight::normal,
	"",
	"serif",
	{ 1, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::heading,
	4,
};

const cainteoir::styles cainteoir::heading5 =
{
	"heading5",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::left,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"",
	"serif",
	{ 1, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::heading,
	5,
};

const cainteoir::styles cainteoir::heading6 =
{
	"heading6",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::left,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::bold,
	"",
	"serif",
	{ 1, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::heading,
	6,
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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	{},
	{},
	cainteoir::text_structure::sentence,
	0,
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
	"",
	{ 0.75, cainteoir::size_units::picas },
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	{ 0.75, cainteoir::size_units::picas },
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
};

const cainteoir::styles cainteoir::emphasized_block =
{
	"emphasized_block",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::italic,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	"",
	{},
	{},
	cainteoir::text_structure::paragraph,
	0,
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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	"monospace",
	{ 1, cainteoir::size_units::picas },
	{},
	cainteoir::text_structure::none,
	0,
};

const cainteoir::styles cainteoir::monospace_block =
{
	"monospace_block",
	cainteoir::display::block,
	cainteoir::vertical_align::inherit,
	cainteoir::text_align::inherit,
	cainteoir::text_decoration::inherit,
	cainteoir::font_style::inherit,
	cainteoir::font_variant::inherit,
	cainteoir::font_weight::inherit,
	"",
	"monospace",
	{ 1, cainteoir::size_units::picas },
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::none,
	0,
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
	"disc",
	"",
	{},
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::none,
	0,
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
	"decimal",
	"",
	{},
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::none,
	0,
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
	"sans-serif",
	{ 1, cainteoir::size_units::picas },
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
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
	"",
	{},
	{ {}, {}, {}, { 1, cainteoir::size_units::picas } },
	cainteoir::text_structure::none,
	0,
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
	"",
	{},
	{},
	cainteoir::text_structure::none,
	0,
};
