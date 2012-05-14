/* NCX Document Parser.
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

#include "parsers.hpp"
#include <stdexcept>

namespace xml   = cainteoir::xml;
namespace xmlns = cainteoir::xml::xmlns;
namespace rdf   = cainteoir::rdf;

namespace ncx
{
	namespace events = cainteoir::events;

	static const xml::context::entry content_node   = { events::unknown, 0 };
	static const xml::context::entry docAuthor_node = { events::unknown, 0 };
	static const xml::context::entry docTitle_node  = { events::unknown, 0 };
	static const xml::context::entry head_node      = { events::unknown, 0 };
	static const xml::context::entry meta_node      = { events::unknown, 0 };
	static const xml::context::entry navLabel_node  = { events::unknown, 0 };
	static const xml::context::entry navMap_node    = { events::unknown, 0 };
	static const xml::context::entry navPoint_node  = { events::unknown, 0 };
	static const xml::context::entry ncx_node       = { events::unknown, 0 };
	static const xml::context::entry text_node      = { events::unknown, 0 };

	static const xml::context::entry content_attr = { events::unknown, 0 };
	static const xml::context::entry name_attr    = { events::unknown, 0 };
	static const xml::context::entry src_attr     = { events::unknown, 0 };
}

static const std::initializer_list<const xml::context::entry_ref> ncx_nodes =
{
	{ "content",   &ncx::content_node },
	{ "docAuthor", &ncx::docAuthor_node },
	{ "docTitle",  &ncx::docTitle_node },
	{ "head",      &ncx::head_node },
	{ "meta",      &ncx::meta_node },
	{ "navLabel",  &ncx::navLabel_node },
	{ "navMap",    &ncx::navMap_node },
	{ "navPoint",  &ncx::navPoint_node },
	{ "ncx",       &ncx::ncx_node },
	{ "text",      &ncx::text_node },
};

static const std::initializer_list<const xml::context::entry_ref> ncx_attrs =
{
	{ "content", &ncx::content_attr },
	{ "name",    &ncx::name_attr },
	{ "src",     &ncx::src_attr },
};

void parseNavPoint(xml::reader &reader, const rdf::uri &subject, cainteoir::document_events &events, int depth)
{
	std::string label;
	rdf::uri location = rdf::uri(std::string(), std::string());
	bool fired = false;
	const xml::context::entry *current = nullptr;
	const xml::context::entry *outer   = nullptr;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		if (current == &ncx::text_node)
		{
			if (outer == &ncx::navLabel_node)
				label = reader.nodeValue().str();
			current = outer = nullptr;
		}
		break;
	case xml::reader::attribute:
		if (current == &ncx::content_node && reader.context() == &ncx::src_attr)
		{
			std::string src = reader.nodeValue().str();
			std::string::size_type ref = src.find('#');
			if (ref == std::string::npos)
				location = rdf::uri(src, std::string());
			else
				location = rdf::uri(src.substr(0, ref), src.substr(ref+1));
		}
		break;
	case xml::reader::beginTagNode:
		current = reader.context();
		if (reader.context() == &ncx::navPoint_node)
		{
			if (!fired)
			{
				events.toc_entry(depth, location, label);
				fired = true;
			}
			parseNavPoint(reader, subject, events, depth+1);
		}
		else if (current == &ncx::navLabel_node)
			outer = reader.context();
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &ncx::navPoint_node)
		{
			if (!fired)
				events.toc_entry(depth, location, label);
			return;
		}
		break;
	}
}

void cainteoir::parseNcxDocument(xml::reader &reader, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	reader.set_nodes(xmlns::ncx, ncx_nodes);
	reader.set_attrs(xmlns::ncx, ncx_attrs);

	const xml::context::entry *outer   = nullptr;
	const xml::context::entry *current = nullptr;

	int depth = 0;
	std::string name;
	std::string content;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		if (current == &ncx::text_node)
		{
			std::string value = reader.nodeValue().str();
			if (outer == &ncx::docAuthor_node)
			{
				aGraph.statement(aSubject, rdf::dc("creator"), rdf::literal(value));
			}
			else if (outer == &ncx::docTitle_node)
			{
				events.toc_entry(0, aSubject, value);
				aGraph.statement(aSubject, rdf::dc("title"), rdf::literal(value));
			}
			current = outer = nullptr;
		}
		break;
	case xml::reader::attribute:
		if (current == &ncx::meta_node && outer == &ncx::head_node)
		{
			if (reader.context() == &ncx::name_attr)
				name = reader.nodeValue().str();
			else if (reader.context() == &ncx::content_attr)
				content = reader.nodeValue().str();
		}
		break;
	case xml::reader::beginTagNode:
		current = reader.context();
		if (reader.context() == &ncx::head_node)
			outer = reader.context();
		else if (reader.context() == &ncx::docAuthor_node)
			outer = reader.context();
		else if (reader.context() == &ncx::docTitle_node)
			outer = reader.context();
		else if (reader.context() == &ncx::navMap_node)
			depth = 1;
		else if (reader.context() == &ncx::navPoint_node && depth == 1)
			parseNavPoint(reader, aSubject, events, depth);
		break;
	case xml::reader::endTagNode:
		if (reader.context() == &ncx::meta_node)
		{
			if (name.find("dtb:") == 0)
			{
				std::string meta = name.substr(4);
				if (meta == "depth" || meta == "totalPageCount" || meta == "maxPageNumber")
					aGraph.statement(aSubject, rdf::dtb(meta), rdf::literal(content, rdf::xsd("int")));
				else
					aGraph.statement(aSubject, rdf::dtb(meta), rdf::literal(content));
			}
		}
		else if (reader.context() == &ncx::head_node)
			outer = nullptr;
		current = nullptr;
		break;
	}
}
