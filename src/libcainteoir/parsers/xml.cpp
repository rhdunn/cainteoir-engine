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

namespace xml = cainteoir::xmldom;

std::string xml::attribute::content() const
{
	if (!item) return std::string();
	return xmlstring_buffer((const char *)xmlNodeListGetString(item->doc, item->children, 1)).str();
}

std::tr1::shared_ptr<cainteoir::buffer> xml::node::content() const
{
	const char *data = NULL;
	if (item)
		data = (const char *)xmlNodeGetContent(item);
	return std::tr1::shared_ptr<cainteoir::buffer>(new xmlstring_buffer(data));
}
