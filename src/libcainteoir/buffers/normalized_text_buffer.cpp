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

namespace utf8 = cainteoir::utf8;

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
}}

class normalized_text_buffer : public cainteoir::buffer
{
public:
	normalized_text_buffer(const std::shared_ptr<buffer> &aBuffer);
	~normalized_text_buffer();
};

normalized_text_buffer::normalized_text_buffer(const std::shared_ptr<cainteoir::buffer> &aBuffer)
	: buffer(nullptr, nullptr)
{
	if (!aBuffer.get() || aBuffer->empty())
		return;

	const char *str = aBuffer->begin();
	const char *l   = aBuffer->end();

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
		if (utf8::isspace(ch))
			ch = ' ';

		uint32_t ch2 = 0;
		if (ch == ' ' && str < l && utf8::read(next, ch2) && utf8::isspace(ch2))
			str = next;
		else
		{
			str  = next;
			last = utf8::write((char *)last, ch);
		}
	}

	// trim space at the end:

	while (last > first && (next = utf8::prev(last)) && utf8::read(next, ch) && utf8::isspace(ch))
		last = next;
	*(char *)last = '\0';
}

normalized_text_buffer::~normalized_text_buffer()
{
	delete [] first;
}

/** @brief Create a whitespace-normalized buffer.
  *
  * @param[in] aBuffer The buffer containing the text to normalize.
  *
  * @return A new buffer with the whitespace normalized.
  *
  * This trims whitespace from the start and end of the buffer, as well as
  * consecutive whitespace characters within the buffer. Any whitespace
  * character is replaced by an ASCII space character.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::normalize(const std::shared_ptr<buffer> &aBuffer)
{
	return std::make_shared<normalized_text_buffer>(aBuffer);
}
