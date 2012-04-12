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
#include <cainteoir/unicode.hpp>

namespace cainteoir { namespace utf8
{
	static bool isspace(uint32_t c)
	{
		switch (c)
		{
		case 0x000009: // HORIZONTAL TAB
		case 0x00000A: // LINE FEED
		case 0x00000D: // CARRIDGE RETURN
		case 0x000020: // SPACE
		case 0x0000A0: // NON-BREAKING SPACE
			return true;
		}
		return false;
	}

	static bool isspace(const char *c)
	{
		uint32_t ch = 0;
		utf8::read(c, ch);
		return isspace(ch);
	}
}}

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

	uint32_t ch = 0;
	const char *next = str;
	while ((next = utf8::read(str, ch)) && utf8::isspace(ch))
		str = next;

	if (str >= l)
		return;

	first = last = new char[l-str+1];

	// normalise the space within the string:

	while (str < l)
	{
		next = utf8::read(str, ch);

		uint32_t ch2 = 0;
		if (str < l && utf8::isspace(ch) && utf8::read(next, ch2) && utf8::isspace(ch2))
			str = next;
		else
		{
			next = utf8::write((char *)last, ch);
			str += next-last;
			last = next;
		}
	}

	// trim space at the end:

	while (last > first && utf8::isspace(--last))
		;
	*(char *)++last = '\0';
}
