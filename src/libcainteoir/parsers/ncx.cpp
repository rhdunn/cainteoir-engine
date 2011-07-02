/* NCX Document Parser.
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
#include <cainteoir/platform.hpp>

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xmldom;

std::string parseNcxText(const xml::node &ncx)
{
	for (xml::node node = ncx.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (node == rdf::ncx("text"))
				return node.content()->str();
		}
	}
	return std::string();
}

void parseNcxHead(const xml::node &ncx, const rdf::uri &subject, cainteoir::document_events &events)
{
	for (xml::node node = ncx.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (node == rdf::ncx("meta"))
			{
				std::string name;
				std::string content;
				for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
				{
					if (!strcmp(attr.name(), "name"))
						name = attr.content();
					else if (!strcmp(attr.name(), "content"))
						content = attr.content();
				}

				if (name.find("dtb:") == 0)
				{
					std::string meta = name.substr(4);
					if (meta == "depth" || meta == "totalPageCount" || meta == "maxPageNumber")
						events.metadata(rdf::statement(subject, rdf::dtb(meta), rdf::literal(content, rdf::xsd("int"))));
					else
						events.metadata(rdf::statement(subject, rdf::dtb(meta), rdf::literal(content)));
				}
			}
		}
	}
}

void cainteoir::parseNcxDocument(const xml::node &ncx, const rdf::uri &subject, cainteoir::document_events &events)
{
	if (ncx != rdf::ncx("ncx"))
		throw std::runtime_error(_("NCX file is not of a recognised format."));

	for (xml::node section = ncx.firstChild(); section.isValid(); section.next())
	{
		if (section.type() == XML_ELEMENT_NODE)
		{
			if (section == rdf::ncx("head"))
				parseNcxHead(section, subject, events);
			else if (section == rdf::ncx("docAuthor"))
				events.metadata(rdf::statement(subject, rdf::dc("creator"), rdf::literal(parseNcxText(section))));
			else if (section == rdf::ncx("docTitle"))
				events.metadata(rdf::statement(subject, rdf::dc("title"), rdf::literal(parseNcxText(section))));
		}
	}
}
