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

struct css_reader
{
	enum token_type
	{
		identifier,
		at_keyword,
		string,
		open_block,
		close_block,
		colon,
		semicolon,
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
#define I  5 // identifier
#define N  6 // number
#define C  7 // colon
#define c  8 // semicolon
#define Q  9 // double-quote
#define q 10 // single-quote

static const char css_lookup_table[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, S, S, S, S, S, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ S, _, Q, _, _, _, _, q, _, _, _, _, _, I, _, _,
	/* 3x */ N, N, N, N, N, N, N, N, N, N, C, c, _, _, _, _,
	/* 4x */ A, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,
	/* 5x */ I, I, I, I, I, I, I, I, I, I, I, _, _, _, _, _,
	/* 6x */ _, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,
	/* 7x */ I, I, I, I, I, I, I, I, I, I, I, B, _, E, _, _,
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
	case A:
		type = at_keyword;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case I:
		case N:
			++mCurrent;
			break;
		default:
			value = cainteoir::buffer(start, mCurrent);
			return true;
		}
		value = cainteoir::buffer(start, mCurrent);
		return true;
	case I:
		type = identifier;
		while (mCurrent < mData->end()) switch (css_lookup_table[*mCurrent])
		{
		case I:
		case N:
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
	case Q:
	case _:
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
#undef I
#undef N
#undef C
#undef c

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

static void parse_counter_style(css_reader &css, cainteoir::counter_style *style)
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
				style->system = cainteoir::counter_system::cyclic;
			else if (css.value.comparei("fixed") == 0)
				style->system = cainteoir::counter_system::fixed;
			else if (css.value.comparei("symbolic") == 0)
				style->system = cainteoir::counter_system::symbolic;
			else if (css.value.comparei("alphabetic") == 0)
				style->system = cainteoir::counter_system::alphabetic;
			else if (css.value.comparei("numeric") == 0)
				style->system = cainteoir::counter_system::numeric;
			else if (css.value.comparei("additive") == 0)
				style->system = cainteoir::counter_system::additive;
			style->range = cainteoir::counter_style::get_auto_range(style->system);
		}
		else if (name.comparei("negative") == 0 && css.type == css_reader::string)
			style->negative = css.value.str();
		else if (name.comparei("prefix") == 0 && css.type == css_reader::string)
			style->prefix = css.value.str();
		else if (name.comparei("suffix") == 0 && css.type == css_reader::string)
			style->suffix = css.value.str();
		else if (name.comparei("symbols") == 0 && css.type == css_reader::string)
		{
			style->symbols.clear();
			while (css.type == css_reader::string)
			{
				style->symbols.push_back(css.value.str());
				css.read();
			}
		}

		while (css.type != css_reader::semicolon)
			css.read();
	}
}

void cainteoir::style_manager::parse(const char *css_file)
{
	const char *datadir = getenv("CAINTEOIR_DATA_DIR");
	if (!datadir)
		datadir = DATADIR "/" PACKAGE;

	const std::string filename = datadir + std::string(css_file);
	parse(make_file_buffer(filename.c_str()));
}

void cainteoir::style_manager::parse(const std::shared_ptr<buffer> &style)
{
	css_reader css(style);
	while (css.read())
	{
		if (css.type == css_reader::at_keyword && css.value.comparei("@counter-style") == 0 &&
		    css.read() && css.type == css_reader::identifier)
		{
			cainteoir::counter_style *style = create_counter_style(css.value.str());
			if (css.read() && css.type == css_reader::open_block)
				parse_counter_style(css, style);
		}
	}
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
