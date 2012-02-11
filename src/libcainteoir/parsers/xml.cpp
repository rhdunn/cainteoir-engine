/* XML DOM API.
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

#include "xml.hpp"

namespace xml = cainteoir::xmldom;

std::string xml::attribute::content() const
{
	if (!item) return std::string();

	std::shared_ptr<xmlChar> s(xmlNodeListGetString(item->doc, item->children, 1), xmlFree);
	return normalized_text_buffer((const char *)s.get()).str();
}

std::tr1::shared_ptr<cainteoir::buffer> xml::node::content(bool normalize) const
{
	if (item)
	{
		std::shared_ptr<xmlChar> s(xmlNodeGetContent(item), xmlFree);
		if (normalize)
			return std::tr1::shared_ptr<cainteoir::buffer>(new normalized_text_buffer((const char *)s.get()));
		else
		{
			const char * str = (const char *)s.get();
			while (*str && (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'))
				++str;

			if (*str)
			{
				size_t len = strlen((const char *)s.get());
				std::tr1::shared_ptr<cainteoir::buffer> data(new cainteoir::data_buffer(len));
				memcpy((void *)data->begin(), (const char *)s.get(), len);
				return data;
			}
		}
	}
	return std::tr1::shared_ptr<cainteoir::buffer>(new buffer(NULL, NULL));
}
