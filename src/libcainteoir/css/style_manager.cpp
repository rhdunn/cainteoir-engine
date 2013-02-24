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

struct css_reader
{
	enum token_type
	{
		identifier,
		at_keyword,
		string,
		integer,
		open_block,
		close_block,
		colon,
		semicolon,
		comma,
		error,
	};

	cainteoir::buffer value;
	token_type type;

	bool read();

	css_reader(std::shared_ptr<cainteoir::buffer> aData);
private:
	void parse_identifier(token_type aType);

	std::shared_ptr<cainteoir::buffer> mData;
	const char *mCurrent;
};

#define _  0 // error
#define S  1 // whitespace
#define B  2 // open  curly brace
#define E  3 // close curly brace
#define A  4 // at
#define L  5 // letter
#define N  6 // number
#define C  7 // colon
#define c  8 // semicolon
#define Q  9 // double-quote
#define q 10 // single-quote
#define M 11 // hyphen/minus
#define F 12 // forward slash
#define s 13 // comma

static const char css_lookup_table[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, S, S, S, S, S, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ S, _, Q, _, _, _, _, q, _, _, _, _, s, M, _, F,
	/* 3x */ N, N, N, N, N, N, N, N, N, N, C, c, _, _, _, _,
	/* 4x */ A, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,
	/* 5x */ L, L, L, L, L, L, L, L, L, L, L, _, _, _, _, _,
	/* 6x */ _, L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,
	/* 7x */ L, L, L, L, L, L, L, L, L, L, L, B, _, E, _, _,
	/* 8x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 9x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ax */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Bx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Cx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Dx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ex */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Fx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
};

css_reader::css_reader(std::shared_ptr<cainteoir::buffer> aData)
	: mData(aData)
	, mCurrent(mData->begin())
	, value(nullptr, nullptr)
{
}

bool css_reader::read()
{
	type = error;
	value = cainteoir::buffer(nullptr, nullptr);

	if (mCurrent >= mData->end())
		return false;

	const char *start = mCurrent;
	while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent++])
	{
	case S: start = mCurrent; break;
	case B: type  = open_block;  return true;
	case E: type  = close_block; return true;
	case C: type  = colon;       return true;
	case c: type  = semicolon;   return true;
	case s: type  = comma;       return true;
	case A:
		type = at_keyword;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case L:
		case N:
		case M:
			++mCurrent;
			break;
		default:
			value = cainteoir::buffer(start, mCurrent);
			return true;
		}
		value = cainteoir::buffer(start, mCurrent);
		return true;
	case N:
	case M:
		type = integer;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case N:
			++mCurrent;
			break;
		default:
			value = cainteoir::buffer(start, mCurrent);
			return true;
		}
		value = cainteoir::buffer(start, mCurrent);
		return true;
	case L:
		type = identifier;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case L:
		case N:
		case M:
			++mCurrent;
			break;
		default:
			value = cainteoir::buffer(start, mCurrent);
			return true;
		}
		value = cainteoir::buffer(start, mCurrent);
		return true;
	case q:
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		default:
			++mCurrent;
			break;
		case q:
			value = cainteoir::buffer(start+1, mCurrent);
			type  = string;
			++mCurrent;
			return true;
		}
		value = cainteoir::buffer(start+1, mCurrent);
		type  = error;
		return true;
	case F:
		if (*mCurrent == '*')
		{
			++mCurrent;
			++mCurrent;
			while (mCurrent[0] != '*' && mCurrent[1] != '/')
				++mCurrent;
			++mCurrent;
			++mCurrent;
			start = mCurrent;
		}
		else
		{
			value = cainteoir::buffer(start, mCurrent);
			type  = error;
			return true;
		}
		break;
	default:
		value = cainteoir::buffer(start, mCurrent);
		type  = error;
		return true;
	}

	return false;
}

#undef _
#undef S
#undef B
#undef E
#undef A
#undef L
#undef N
#undef C
#undef c
#undef Q
#undef q
#undef M
#undef F
#undef s

static void parse_counter_style(css_reader &css, css::counter_style *style)
{
	while (css.read())
	{
		if (css.type == css_reader::close_block)
			return;

		if (css.type != css_reader::identifier)
			continue;

		cainteoir::buffer name = css.value;
		if (!css.read() || css.type != css_reader::colon || !css.read())
			continue;

		if (name.comparei("system") == 0 && css.type == css_reader::identifier)
		{
			if (css.value.comparei("cyclic") == 0)
				style->system = css::counter_system::cyclic;
			else if (css.value.comparei("fixed") == 0)
			{
				style->system = css::counter_system::fixed;
				if (css.read() && css.type == css_reader::integer)
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
		else if (name.comparei("negative") == 0 && css.type == css_reader::string)
		{
			style->negative_prefix = css.value.str();
			if (css.read() && css.type == css_reader::string)
				style->negative_suffix = css.value.str();
		}
		else if (name.comparei("prefix") == 0 && css.type == css_reader::string)
			style->prefix = css.value.str();
		else if (name.comparei("suffix") == 0 && css.type == css_reader::string)
			style->suffix = css.value.str();
		else if (name.comparei("range") == 0)
		{
			if (css.type == css_reader::identifier && css.value.comparei("auto") == 0)
				style->range = css::counter_style::get_auto_range(style->system);
			else
			{
				bool valid_range = true;
				css::counter_style::value_t values[2] = { 0, 0 };
				for (int i = 0; i < 2 && valid_range; ++i)
				{
					if (css.type == css_reader::integer)
						values[i] = atoi(css.value.begin());
					else if (css.type == css_reader::identifier && css.value.comparei("infinite") == 0)
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
		else if (name.comparei("symbols") == 0 && css.type == css_reader::string)
		{
			style->symbols.clear();
			while (css.type == css_reader::string)
			{
				style->symbols.push_back(css.value.str());
				css.read();
			}
		}
		else if (name.comparei("additive-symbols") == 0 && css.type == css_reader::integer)
		{
			style->additive_symbols.clear();
			while (css.type == css_reader::integer)
			{
				std::pair<css::counter_style::value_t, std::string> value;
				value.first = atoi(css.value.begin());
				if (css.read() && css.type == css_reader::string)
				{
					value.second = css.value.str();
					style->additive_symbols.push_back(value);
					if (css.read() && css.type == css_reader::comma)
						css.read();
				}
			}
		}

		while (css.type != css_reader::semicolon)
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
	css_reader css(style);
	while (css.read())
	{
		if (css.type == css_reader::at_keyword && css.value.comparei("@counter-style") == 0 &&
		    css.read() && css.type == css_reader::identifier)
		{
			css::counter_style *style = create_counter_style(css.value.str());
			if (css.read() && css.type == css_reader::open_block)
				parse_counter_style(css, style);
		}
	}
}
