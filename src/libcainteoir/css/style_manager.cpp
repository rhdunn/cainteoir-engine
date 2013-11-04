/* CSS Style Management.
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
#include <cainteoir/path.hpp>
#include <stdexcept>
#include <sstream>
#include <limits>
#include <stdlib.h>

namespace css = cainteoir::css;

static void parse_counter_style(css::reader &css, css::counter_style *style)
{
	while (css.read())
	{
		if (css.type == css::reader::close_block)
			return;

		if (css.type != css::reader::identifier)
			continue;

		cainteoir::buffer name = css.value;
		if (!css.read() || css.type != css::reader::colon || !css.read())
			continue;

		if (name.comparei("system") == 0 && css.type == css::reader::identifier)
		{
			if (css.value.comparei("cyclic") == 0)
				style->system = css::counter_system::cyclic;
			else if (css.value.comparei("fixed") == 0)
			{
				style->system = css::counter_system::fixed;
				if (css.read() && css.type == css::reader::integer)
					style->initial_symbol_value = atoi(css.value.begin());
			}
			else if (css.value.comparei("symbolic") == 0)
				style->system = css::counter_system::symbolic;
			else if (css.value.comparei("alphabetic") == 0)
				style->system = css::counter_system::alphabetic;
			else if (css.value.comparei("numeric") == 0)
				style->system = css::counter_system::numeric;
			else if (css.value.comparei("additive") == 0)
				style->system = css::counter_system::additive;
			style->range = css::counter_style::get_auto_range(style->system);
		}
		else if (name.comparei("negative") == 0 && css.type == css::reader::string)
		{
			style->negative_prefix = css.value.str();
			if (css.read() && css.type == css::reader::string)
				style->negative_suffix = css.value.str();
		}
		else if (name.comparei("prefix") == 0 && css.type == css::reader::string)
			style->prefix = css.value.str();
		else if (name.comparei("suffix") == 0 && css.type == css::reader::string)
			style->suffix = css.value.str();
		else if (name.comparei("range") == 0)
		{
			if (css.type == css::reader::identifier && css.value.comparei("auto") == 0)
				style->range = css::counter_style::get_auto_range(style->system);
			else
			{
				bool valid_range = true;
				css::counter_style::value_t values[2] = { 0, 0 };
				for (int i = 0; i < 2 && valid_range; ++i)
				{
					if (css.type == css::reader::integer)
						values[i] = atoi(css.value.begin());
					else if (css.type == css::reader::identifier && css.value.comparei("infinite") == 0)
						values[i] = (i == 0) ? css::counter_style::infinite.first : css::counter_style::infinite.second;
					else
						valid_range = false;

					if (valid_range)
						css.read();
				}

				if (valid_range && values[0] <= values[1])
					style->range = { values[0], values[1] };
			}
		}
		else if (name.comparei("symbols") == 0 && css.type == css::reader::string)
		{
			style->symbols.clear();
			while (css.type == css::reader::string)
			{
				style->symbols.push_back(css.value.str());
				css.read();
			}
		}
		else if (name.comparei("additive-symbols") == 0 && css.type == css::reader::integer)
		{
			style->additive_symbols.clear();
			while (css.type == css::reader::integer)
			{
				std::pair<css::counter_style::value_t, std::string> value;
				value.first = atoi(css.value.begin());
				if (css.read() && css.type == css::reader::string)
				{
					value.second = css.value.str();
					style->additive_symbols.push_back(value);
					if (css.read() && css.type == css::reader::comma)
						css.read();
				}
			}
		}

		while (css.type != css::reader::semicolon)
			css.read();
	}
}

const css::counter_style *css::style_manager::get_counter_style(const std::string &aName) const
{
	auto item = mCounterStyles.find(aName);
	if (item != mCounterStyles.end())
		return item->second;
	return nullptr;
}

css::counter_style *css::style_manager::create_counter_style(const std::string &aName)
{
	std::shared_ptr<counter_style> style = std::make_shared<counter_style>();
	mCounterStyleRegistry.push_back(style);
	mCounterStyles[aName] = style.get();
	return style.get();
}

void css::style_manager::parse(const char *css_file)
{
	parse(make_file_buffer(get_data_path() / css_file));
}

void css::style_manager::parse(const std::shared_ptr<buffer> &style)
{
	css::reader css(style);
	while (css.read())
	{
		if (css.type == css::reader::at_keyword && css.value.comparei("@counter-style") == 0 &&
		    css.read() && css.type == css::reader::identifier)
		{
			css::counter_style *style = create_counter_style(css.value.str());
			if (css.read() && css.type == css::reader::open_block)
				parse_counter_style(css, style);
		}
	}
}

const css::styles cainteoir::unknown =
{
	"unknown",
	css::display::inherit,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::paragraph =
{
	"paragraph",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"sans-serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::paragraph,
	0,
};

const css::styles cainteoir::heading0 =
{
	"heading0",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 2, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::heading,
	0,
};

const css::styles cainteoir::heading1 =
{
	"heading1",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 2, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::heading,
	1,
};

const css::styles cainteoir::heading2 =
{
	"heading2",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 1.5, css::length::picas },
	{ {}, {}, {}, { 2, css::length::picas } },
	css::role::heading,
	2,
};

const css::styles cainteoir::heading3 =
{
	"heading3",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::heading,
	3,
};

const css::styles cainteoir::heading4 =
{
	"heading4",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::center,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::normal,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::heading,
	4,
};

const css::styles cainteoir::heading5 =
{
	"heading5",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::left,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::heading,
	5,
};

const css::styles cainteoir::heading6 =
{
	"heading6",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::left,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::bold,
	"",
	"serif",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::heading,
	6,
};

const css::styles cainteoir::span =
{
	"span",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::sentence =
{
	"sentence",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::sentence,
	0,
};

const css::styles cainteoir::superscript =
{
	"superscript",
	css::display::inlined,
	css::vertical_align::super,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{ 0.75, css::length::picas },
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::subscript =
{
	"subscript",
	css::display::inlined,
	css::vertical_align::sub,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{ 0.75, css::length::picas },
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::emphasized =
{
	"emphasized",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::italic,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::emphasized_block =
{
	"emphasized_block",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::italic,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::paragraph,
	0,
};

const css::styles cainteoir::strong =
{
	"strong",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::bold,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::reduced =
{
	"reduced",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::normal,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::underlined =
{
	"underlined",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::underline,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::monospace =
{
	"monospace",
	css::display::inlined,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"monospace",
	{ 1, css::length::picas },
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::monospace_block =
{
	"monospace_block",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::preserved,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"monospace",
	{ 1, css::length::picas },
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::none,
	0,
};

const css::styles cainteoir::bullet_list =
{
	"bullet_list",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"disc",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::none,
	0,
};

const css::styles cainteoir::number_list =
{
	"number_list",
	css::display::block,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"decimal",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::none,
	0,
};

const css::styles cainteoir::list_item =
{
	"list_item",
	css::display::list_item,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"sans-serif",
	{ 1, css::length::picas },
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::table =
{
	"table",
	css::display::table,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};

const css::styles cainteoir::table_row =
{
	"table_row",
	css::display::table_row,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{ {}, {}, {}, { 1, css::length::picas } },
	css::role::none,
	0,
};

const css::styles cainteoir::table_cell =
{
	"table_cell",
	css::display::table_cell,
	css::vertical_align::inherit,
	css::text_align::inherit,
	css::text_decoration::inherit,
	css::whitespace::normal,
	css::font_style::inherit,
	css::font_variant_caps::inherit,
	css::font_weight::inherit,
	"",
	"",
	{},
	{},
	css::role::none,
	0,
};
