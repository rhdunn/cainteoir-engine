/* XHTML Document Parser.
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

#include "parsers.hpp"
#include <cainteoir/xmlreader.hpp>

namespace xml = cainteoir::xml;

void skipNode(xml::reader & reader, const cainteoir::buffer &name)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		break;
	}
}

void cainteoir::parseXHtmlDocument(std::tr1::shared_ptr<cainteoir::buffer> data, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	xml::reader reader(data);
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (!reader.nodeName().compare("head") || !reader.nodeName().compare("script") || !reader.nodeName().compare("style"))
			skipNode(reader, reader.nodeName());
		break;
	case xml::reader::textNode:
		{
			const char * str = reader.nodeValue().begin();
			const char * end = reader.nodeValue().end();
			while (str != end && (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'))
				++str;

			if (str != end)
				events.text(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::range_buffer(data, reader.nodeValue())));
		}
		break;
	}
}
