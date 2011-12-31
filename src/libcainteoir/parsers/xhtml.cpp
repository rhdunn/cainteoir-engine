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

enum html_node
{
	attr_id,
	attr_lang,
	node_head,
	node_html,
	node_li,
	node_link,
	node_meta,
	node_script,
	node_style,
	node_title,
	node_unknown,
};

struct context_node
{
	const char * name;
	html_node node;
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
	{ "b",        node_unknown, cainteoir::document_events::span,      cainteoir::document_events::strong },
	{ "cite",     node_unknown, cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "code",     node_unknown, cainteoir::document_events::span,      cainteoir::document_events::monospace },
	{ "div",      node_unknown, cainteoir::document_events::paragraph, 0 },
	{ "em",       node_unknown, cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "h1",       node_unknown, cainteoir::document_events::heading,   1 },
	{ "h2",       node_unknown, cainteoir::document_events::heading,   2 },
	{ "h3",       node_unknown, cainteoir::document_events::heading,   3 },
	{ "h4",       node_unknown, cainteoir::document_events::heading,   4 },
	{ "h5",       node_unknown, cainteoir::document_events::heading,   5 },
	{ "h6",       node_unknown, cainteoir::document_events::heading,   6 },
	{ "head",     node_head,    cainteoir::document_events::unknown,   0 },
	{ "html",     node_html,    cainteoir::document_events::unknown,   0 },
	{ "i",        node_unknown, cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "id",       attr_id,      cainteoir::document_events::unknown,   0 },
	{ "lang",     attr_lang,    cainteoir::document_events::unknown,   0 },
	{ "li",       node_li,      cainteoir::document_events::unknown,   0 },
	{ "link",     node_link,    cainteoir::document_events::unknown,   0 },
	{ "meta",     node_meta,    cainteoir::document_events::unknown,   0 },
	{ "ol",       node_unknown, cainteoir::document_events::list,      number_list },
	{ "p",        node_unknown, cainteoir::document_events::paragraph, 0 },
	{ "pre",      node_unknown, cainteoir::document_events::paragraph, cainteoir::document_events::monospace },
	{ "script",   node_script,  cainteoir::document_events::unknown,   0 },
	{ "style",    node_style,   cainteoir::document_events::unknown,   0 },
	{ "strong",   node_unknown, cainteoir::document_events::span,      cainteoir::document_events::strong },
	{ "sub",      node_unknown, cainteoir::document_events::span,      cainteoir::document_events::subscript },
	{ "sup",      node_unknown, cainteoir::document_events::span,      cainteoir::document_events::superscript },
	{ "title",    node_title,   cainteoir::document_events::unknown,   0 },
	{ "ul",       node_unknown, cainteoir::document_events::list,      bullet_list },
	{ "xml:lang", attr_lang,    cainteoir::document_events::unknown,   0 },
};

static const context_node unknown_context = { NULL, node_unknown, cainteoir::document_events::unknown, 0 };

#define countof(a) (sizeof(a)/sizeof(a[0]))

const context_node * lookup_context(const cainteoir::buffer & node)
{
	for (auto item = context_nodes, last = context_nodes + countof(context_nodes); item != last; ++item)
	{
		if (!node.comparei(item->name))
			return item;
	}
	return &unknown_context;
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

void parseTitleNode(xml::reader & reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	while (reader.read())
	{
		const context_node *context = lookup_context(reader.nodeName());
		switch (reader.nodeType())
		{
		case xml::reader::endTagNode:
			if (context->node == node_title)
				return;
			break;
		case xml::reader::textNode:
			{
				std::string title = reader.nodeValue().normalize()->str();
				if (!title.empty())
					aGraph.statement(aSubject, rdf::dc("title"), rdf::literal(title));
			}
			break;
		}
	}
}

void parseHeadNode(xml::reader & reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	while (reader.read())
	{
		const context_node *context = lookup_context(reader.nodeName());
		switch (reader.nodeType())
		{
		case xml::reader::beginTagNode:
			if (context->node == node_title)
				parseTitleNode(reader, aSubject, events, aGraph);
			else if (context->node == node_meta || context->node == node_link)
				;
			else
				skipNode(reader, reader.nodeName());
		case xml::reader::endTagNode:
			if (context->node == node_head)
				return;
			break;
		}
	}
}

void parseListNode(xml::reader & reader, const rdf::uri &aSubject, cainteoir::document_events &events, const context_node *list_ctx)
{
	int number = 1;

	while (reader.read())
	{
		const context_node *context = lookup_context(reader.nodeName());
		switch (reader.nodeType())
		{
		case xml::reader::beginTagNode:
			if (context->node == node_li)
			{
				events.begin_context(cainteoir::document_events::list_item);
				if (list_ctx->parameter == bullet_list)
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
			if (context == list_ctx)
			{
				events.end_context();
				return;
			}
			if (context->node == node_li)
				events.end_context();
			break;
		}
	}
}

void parseBodyNode(xml::reader & reader, const rdf::uri &aSubject, cainteoir::document_events &events, const context_node *body_ctx)
{
	while (reader.read())
	{
		const context_node *context = lookup_context(reader.nodeName());
		switch (reader.nodeType())
		{
		case xml::reader::attribute:
			if (context->node == attr_id)
				events.anchor(rdf::uri(aSubject.str(), reader.nodeValue().str()), std::string());
			break;
		case xml::reader::beginTagNode:
			if (context->node == node_script || context->node == node_style)
				skipNode(reader, reader.nodeName());
			else if (context->context != cainteoir::document_events::unknown)
			{
				events.begin_context(context->context, context->parameter);
				if (context->context == cainteoir::document_events::list)
					parseListNode(reader, aSubject, events, context);
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
			if (context == body_ctx)
				return;
			if (context->context != cainteoir::document_events::unknown)
				events.end_context();
			break;
		}
	}
}

void parseHtmlNode(xml::reader & reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string lang;
	while (reader.read())
	{
		const context_node *context = lookup_context(reader.nodeName());
		switch (reader.nodeType())
		{
		case xml::reader::beginTagNode:
			if (context->node == node_head)
				parseHeadNode(reader, aSubject, events, aGraph);
			else
				parseBodyNode(reader, aSubject, events, context);
			break;
		case xml::reader::attribute:
			if (context->node == attr_lang && lang.empty())
			{
				lang = reader.nodeValue().buffer()->str();
				aGraph.statement(aSubject, rdf::dc("language"), rdf::literal(lang));
			}
			break;
		case xml::reader::endTagNode:
			if (context->node == node_html)
				return;
			break;
		}
	}
}

void cainteoir::parseXHtmlDocument(std::tr1::shared_ptr<cainteoir::buffer> data, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	xml::reader reader(data);

	if (reader.isPlainText())
	{
		events.text(data);
		return;
	}

	while (reader.read())
	{
		const context_node *context = lookup_context(reader.nodeName());
		switch (reader.nodeType())
		{
		case xml::reader::beginTagNode:
			if (context->node == node_html)
				parseHtmlNode(reader, aSubject, events, aGraph);
			break;
		}
	}
}
