/* Memory (Data) Buffer.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include <cainteoir/buffer.hpp>
#include <libxml/xmlmemory.h>

namespace utf8
{
	inline bool isspace(const char *c)
	{
		switch (*c)
		{
		case '\t': // U+0009 -- HORIZONTAL TAB
		case '\n': // U+000A -- LINE FEED
		case '\r': // U+000D -- CARRIDGE RETURN
		case ' ':  // U+0020 -- SPACE
			return true;
		}
		return false;
	}
}

cainteoir::xmlstring_buffer::xmlstring_buffer(const char *str)
	: buffer(str, str)
	, data(str)
{
	// trim space at the start:

	while (utf8::isspace(first))
		++first;

	// normalise the space within the string:

	const char *current = first;
	const char *next = first+1;

	last = first;
	while (*current)
	{
		if (utf8::isspace(current) && utf8::isspace(next))
		{
			++current;
			++next;
		}
		else
		{
			*(char *)last = *current;
			++last;
			++current;
			++next;
		}
	}

	// trim space at the end:

	while (utf8::isspace(--last))
		;
	*(char *)++last = '\0';
}

cainteoir::xmlstring_buffer::~xmlstring_buffer()
{
	xmlFree((char *)data);
}
