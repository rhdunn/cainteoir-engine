/* Text Normalization Buffer for XML Strings.
 *
 * Copyright (C) 2010-2014 Reece H. Dunn
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
#include <ucd/ucd.h>

namespace utf8 = cainteoir::utf8;

class normalized_text_buffer : public cainteoir::buffer
{
public:
	normalized_text_buffer(const std::shared_ptr<buffer> &aBuffer,
	                       cainteoir::whitespace aWhitespace,
	                       cainteoir::whitespace aNewlines,
	                       cainteoir::whitespace aTrimLeft,
	                       cainteoir::whitespace aTrimRight);
	~normalized_text_buffer();
};

normalized_text_buffer::normalized_text_buffer(const std::shared_ptr<cainteoir::buffer> &aBuffer,
                                               cainteoir::whitespace aWhitespace,
                                               cainteoir::whitespace aNewlines,
                                               cainteoir::whitespace aTrimLeft,
                                               cainteoir::whitespace aTrimRight)
	: buffer(nullptr, nullptr)
{
	if (!aBuffer.get() || aBuffer->empty())
		return;

	const char *str  = aBuffer->begin();
	const char *l    = aBuffer->end();
	const char *next = str;
	uint32_t ch = 0;

	if (aTrimLeft == cainteoir::whitespace::collapse)
	{
		// trim space at the start:

		while ((next = utf8::read(str, ch)) && ucd::isspace(ch))
			str = next;

		if (str >= l)
			return;
	}

	first = last = new char[l-str+1];

	if (aTrimLeft == cainteoir::whitespace::preserve)
	{
		while ((next = utf8::read(str, ch)) && ucd::isspace(ch))
		{
			str  = next;
			last = utf8::write((char *)last, ch);
		}
	}

	// normalise the space within the string:

	while (str < l)
	{
		next = utf8::read(str, ch);
		if (ucd::isspace(ch))
			ch = ' ';

		uint32_t ch2 = 0;
		if (ch == ' ' && str < l && utf8::read(next, ch2) && ucd::isspace(ch2))
			str = next;
		else
		{
			str  = next;
			last = utf8::write((char *)last, ch);
		}
	}

	if (aTrimRight == cainteoir::whitespace::collapse)
	{
		// trim space at the end:

		while (last > first && (next = utf8::prev(last)) && utf8::read(next, ch) && ucd::isspace(ch))
			last = next;
		*(char *)last = '\0';
	}
}

normalized_text_buffer::~normalized_text_buffer()
{
	delete [] first;
}

std::shared_ptr<cainteoir::buffer>
cainteoir::normalize(const std::shared_ptr<buffer> &aBuffer)
{
	return std::make_shared<normalized_text_buffer>(aBuffer,
	                                                cainteoir::whitespace::collapse,
	                                                cainteoir::whitespace::collapse,
	                                                cainteoir::whitespace::collapse,
	                                                cainteoir::whitespace::collapse);
}

std::shared_ptr<cainteoir::buffer>
cainteoir::normalize(const std::shared_ptr<buffer> &aBuffer,
                     cainteoir::whitespace aWhitespace,
                     cainteoir::whitespace aNewlines,
                     cainteoir::whitespace aTrimLeft,
                     cainteoir::whitespace aTrimRight)
{
	return std::make_shared<normalized_text_buffer>(aBuffer, aWhitespace, aNewlines, aTrimLeft, aTrimRight);
}
