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
