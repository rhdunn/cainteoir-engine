/* Text Normalization Buffer for XML Strings.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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

#include <cainteoir/buffer.hpp>

namespace utf8
{
	static bool isspace(const char *c)
	{
		switch ((uint8_t)*c)
		{
		case '\t': // U+0009 -- HORIZONTAL TAB
		case '\n': // U+000A -- LINE FEED
		case '\r': // U+000D -- CARRIDGE RETURN
		case ' ':  // U+0020 -- SPACE
			return true;
		case 0xC2:
			if (uint8_t(*++c) == 0xA0) // U+0x00A0 -- NON-BREAKING SPACE
				return true;
		}
		return false;
	}

	static const char *next(const char *c)
	{
		if (uint8_t(*c) < 0x80) return ++c;
		switch (uint8_t(*c))
		{
		default:   return c + 2;
		case 0xE0: return c + 3;
		case 0xF0: return c + 4;
		}
	}
}

cainteoir::normalized_text_buffer::normalized_text_buffer(const char *str)
	: buffer(nullptr, nullptr)
{
	if (str)
		normalize(str, str+strlen(str));
}

cainteoir::normalized_text_buffer::normalized_text_buffer(const char *f, const char *l)
	: buffer(nullptr, nullptr)
{
	if (f != l)
		normalize(f, l);
}

cainteoir::normalized_text_buffer::normalized_text_buffer(const std::shared_ptr<cainteoir::buffer> &str)
	: buffer(nullptr, nullptr)
{
	normalize(str->begin(), str->end());
}

cainteoir::normalized_text_buffer::~normalized_text_buffer()
{
	delete [] first;
}

void cainteoir::normalized_text_buffer::normalize(const char *str, const char *l)
{
	// trim space at the start:

	while (utf8::isspace(str))
		str = utf8::next(str);

	if (str >= l)
		return;

	first = last = new char[l-str+1];

	// normalise the space within the string:

	while (str != l)
	{
		if (utf8::isspace(str) && utf8::isspace(utf8::next(str)))
			str = utf8::next(str);
		else
		{
			*(char *)last = *str;
			++last;
			++str;
		}
	}

	// trim space at the end:

	while (last > first && utf8::isspace(--last))
		;
	*(char *)++last = '\0';
}
