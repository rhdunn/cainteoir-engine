/* XML DOM API.
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

#include "xml.hpp"

namespace cainteoir { namespace xmldom
{
	namespace utf8
	{
		inline bool isspace(xmlChar *c)
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

	class string
	{
	public:
		string(xmlChar *aString);

		~string()
		{
			xmlFree(buffer);
		}

		std::string str() const
		{
			if (!data) return std::string();
			return (const char *)data;
		}
	private:
		xmlChar *buffer;
		xmlChar *data;
	};
}}

namespace xml = cainteoir::xmldom;

xml::string::string(xmlChar *aString)
	: buffer(aString)
	, data(aString)
{
	// trim space at the start:

	while (utf8::isspace(data))
		++data;

	// normalise the space within the string:

	xmlChar *current = data;
	xmlChar *next = data+1;
	xmlChar *marker = data;
	while (*current)
	{
		if (utf8::isspace(current) && utf8::isspace(next))
		{
			++current;
			++next;
		}
		else
		{
			*marker = *current;
			++marker;
			++current;
			++next;
		}
	}

	// trim space at the end:

	while (utf8::isspace(--marker))
		;
	*++marker = '\0';
}

std::string xml::attribute::content() const
{
	if (!attr) return std::string();
	return xml::string(xmlNodeListGetString(attr->doc, attr->children, 1)).str();
}

std::string xml::node::content() const
{
	if (!item) return std::string();
	return xml::string(xmlNodeGetContent(item)).str();
}

