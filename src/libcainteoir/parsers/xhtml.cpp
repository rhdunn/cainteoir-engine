/* XHTML Document Parser.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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
namespace rdf = cainteoir::rdf;

struct context_node
{
	const char * name;
	cainteoir::document_events::context context;
	uint32_t parameter;
};

enum list_type
{
	bullet_list = 0x00000000,
	number_list = 0x80000000,
};

static const context_node context_nodes[] =
{
	{ "b",      cainteoir::document_events::span,      cainteoir::document_events::strong },
	{ "cite",   cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "code",   cainteoir::document_events::span,      cainteoir::document_events::monospace },
	{ "div",    cainteoir::document_events::paragraph, 0 },
	{ "em",     cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "h1",     cainteoir::document_events::heading,   1 },
	{ "h2",     cainteoir::document_events::heading,   2 },
	{ "h3",     cainteoir::document_events::heading,   3 },
	{ "h4",     cainteoir::document_events::heading,   4 },
	{ "h5",     cainteoir::document_events::heading,   5 },
	{ "h6",     cainteoir::document_events::heading,   6 },
	{ "i",      cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "ol",     cainteoir::document_events::list,      number_list },
	{ "p",      cainteoir::document_events::paragraph, 0 },
	{ "pre",    cainteoir::document_events::paragraph, cainteoir::document_events::monospace },
	{ "strong", cainteoir::document_events::span,      cainteoir::document_events::strong },
	{ "sub",    cainteoir::document_events::span,      cainteoir::document_events::subscript },
	{ "sup",    cainteoir::document_events::span,      cainteoir::document_events::superscript },
	{ "ul",     cainteoir::document_events::list,      bullet_list },
};

#define countof(a) (sizeof(a)/sizeof(a[0]))

const context_node * lookup_context(const cainteoir::buffer & node)
{
	for (auto item = context_nodes, last = context_nodes + countof(context_nodes); item != last; ++item)
	{
		if (!node.comparei(item->name))
			return item;
	}
	return NULL;
}

void skipNode(xml::reader & reader, const cainteoir::buffer name)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		break;
	}
}

void parseTitleNode(xml::reader & reader, const cainteoir::buffer name, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		break;
	case xml::reader::textNode:
		{
			std::string title = reader.nodeValue().normalize()->str();
			if (!title.empty())
				events.metadata(rdf::statement(aSubject, rdf::dc("title"), rdf::literal(title)));
		}
		break;
	}
}

void parseHeadNode(xml::reader & reader, const cainteoir::buffer name, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (!reader.nodeName().comparei("title"))
			parseTitleNode(reader, reader.nodeName(), aSubject, events);
		else if (!reader.nodeName().comparei("meta") ||
		         !reader.nodeName().comparei("link"))
			;
		else
			skipNode(reader, reader.nodeName());
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		break;
	}
}

void parseListNode(xml::reader & reader, const cainteoir::buffer name, const rdf::uri &aSubject, cainteoir::document_events &events, uint32_t type)
{
	int number = 1;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (!reader.nodeName().comparei("li"))
		{
			events.begin_context(cainteoir::document_events::list_item);
			if (type == bullet_list)
				events.text(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer("\xE2\x80\xA2 ")));
			else
			{
				char text[100];
				int len = snprintf(text, sizeof(text), "%d. ", number);
				text[len] = '\0';

				std::tr1::shared_ptr<cainteoir::buffer> data(new cainteoir::data_buffer(len));
				strcpy((char *)data->begin(), text);
				events.text(data);

				++number;
			}
		}
		break;
	case xml::reader::textNode:
		{
			std::tr1::shared_ptr<cainteoir::buffer> text = reader.nodeValue().buffer();

			const char * str = text->begin();
			const char * end = text->end();
			while (str != end && (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'))
				++str;

			if (str != end)
				events.text(text);
		}
		break;
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
		{
			events.end_context();
			return;
		}
		if (!reader.nodeName().comparei("li"))
			events.end_context();
		break;
	}
}

void parseBodyNode(xml::reader & reader, const cainteoir::buffer name, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (!reader.nodeName().comparei("script") ||
		    !reader.nodeName().comparei("style"))
			skipNode(reader, reader.nodeName());
		else
		{
			const context_node * node = lookup_context(reader.nodeName());
			if (node)
			{
				events.begin_context(node->context, node->parameter);
				if (node->context == cainteoir::document_events::list)
					parseListNode(reader, reader.nodeName(), aSubject, events, node->parameter);
			}
		}
		break;
	case xml::reader::textNode:
		{
			std::tr1::shared_ptr<cainteoir::buffer> text = reader.nodeValue().buffer();

			const char * str = text->begin();
			const char * end = text->end();
			while (str != end && (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'))
				++str;

			if (str != end)
				events.text(text);
		}
		break;
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		if (lookup_context(reader.nodeName()))
			events.end_context();
		break;
	}
}

void parseHtmlNode(xml::reader & reader, const cainteoir::buffer name, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	std::string lang;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (!reader.nodeName().comparei("head"))
			parseHeadNode(reader, reader.nodeName(), aSubject, events);
		else
			parseBodyNode(reader, reader.nodeName(), aSubject, events);
		break;
	case xml::reader::attribute:
		if ((!reader.nodeName().comparei("lang") || !reader.nodeName().comparei("xml:lang")) && lang.empty())
		{
			lang = reader.nodeValue().buffer()->str();
			events.metadata(rdf::statement(aSubject, rdf::dc("language"), rdf::literal(lang)));
		}
		break;
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		break;
	}
}

void cainteoir::parseXHtmlDocument(std::tr1::shared_ptr<cainteoir::buffer> data, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	xml::reader reader(data);

	if (reader.isPlainText())
	{
		events.text(data);
		return;
	}

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (!reader.nodeName().comparei("html"))
			parseHtmlNode(reader, reader.nodeName(), aSubject, events);
		break;
	}
}
