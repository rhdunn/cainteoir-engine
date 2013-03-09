/* CSS Document Reader.
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

css::reader::reader(std::shared_ptr<cainteoir::buffer> aData)
	: mData(aData)
	, mCurrent(mData->begin())
	, value(nullptr, nullptr)
{
}

bool css::reader::read()
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
