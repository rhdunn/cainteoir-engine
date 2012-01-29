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
namespace xmldom = cainteoir::xmldom;

std::string parseNcxText(const xmldom::node &ncx)
{
	for (xmldom::node node = ncx.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (node == rdf::ncx("text"))
				return node.content()->str();
		}
	}
	return std::string();
}

void parseNavPoint(const xmldom::node &ncx, const rdf::uri &subject, cainteoir::document_events &events, int depth)
{
	std::string label;
	rdf::uri location = rdf::uri(std::string(), std::string());
	bool fired = false;

	for (xmldom::node node = ncx.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (node == rdf::ncx("navLabel"))
				label = node.content()->str();
			else if (node == rdf::ncx("content"))
			{
				for (xmldom::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
				{
					if (!strcmp(attr.name(), "src"))
					{
						std::string src = attr.content();
						std::string::size_type ref = src.find('#');
						if (ref == std::string::npos)
							location = rdf::uri(src, std::string());
						else
							location = rdf::uri(src.substr(0, ref), src.substr(ref+1));
					}
				}
			}
			else if (node == rdf::ncx("navPoint"))
			{
				if (!fired)
				{
					events.toc_entry(depth, location, label);
					fired = true;
				}
				parseNavPoint(node, subject, events, depth+1);
			}
		}
	}

	if (!fired)
		events.toc_entry(depth, location, label);
}

void parseNavMap(const xmldom::node &ncx, const rdf::uri &subject, cainteoir::document_events &events)
{
	for (xmldom::node node = ncx.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (node == rdf::ncx("navPoint"))
				parseNavPoint(node, subject, events, 1);
		}
	}
}

void parseNcxHead(const xmldom::node &ncx, const rdf::uri &subject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	for (xmldom::node node = ncx.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (node == rdf::ncx("meta"))
			{
				std::string name;
				std::string content;
				for (xmldom::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
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
						aGraph.statement(subject, rdf::dtb(meta), rdf::literal(content, rdf::xsd("int")));
					else
						aGraph.statement(subject, rdf::dtb(meta), rdf::literal(content));
				}
			}
		}
	}
}

void cainteoir::parseNcxDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	xmldom::document doc(aData);
	xmldom::node ncx = doc.root();

	if (ncx != rdf::ncx("ncx"))
		throw std::runtime_error(_("NCX file is not of a recognised format."));

	for (xmldom::node section = ncx.firstChild(); section.isValid(); section.next())
	{
		if (section.type() == XML_ELEMENT_NODE)
		{
			if (section == rdf::ncx("head"))
				parseNcxHead(section, aSubject, events, aGraph);
			else if (section == rdf::ncx("docAuthor"))
				aGraph.statement(aSubject, rdf::dc("creator"), rdf::literal(parseNcxText(section)));
			else if (section == rdf::ncx("docTitle"))
				aGraph.statement(aSubject, rdf::dc("title"), rdf::literal(parseNcxText(section)));
			else if (section == rdf::ncx("navMap"))
				parseNavMap(section, aSubject, events);
		}
	}
}
