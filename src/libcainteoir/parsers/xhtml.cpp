/* XHTML Document Parser.
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

#include "parsers.hpp"
#include <cainteoir/xmlreader.hpp>
#include <stack>

#define countof(a) (sizeof(a)/sizeof(a[0]))

namespace xml = cainteoir::xml;
namespace rdf = cainteoir::rdf;

enum html_node
{
	node_unknown,
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
};

enum list_type
{
	bullet_list = 0x00000000,
	number_list = 0x80000000,
};

static const std::initializer_list<const xml::context::entry> html_node_data =
{
	{ "a",          node_unknown, cainteoir::document_events::unknown,   0 },
	{ "abbr",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "address",    node_unknown, cainteoir::document_events::unknown,   0 },
	{ "acronym",    node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "applet",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "area",       node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "article",    node_unknown, cainteoir::document_events::unknown,   0 },
	{ "aside",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "audio",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "b",          node_unknown, cainteoir::document_events::span,      cainteoir::document_events::strong },
	{ "base",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "basefont",   node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "bdi",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "bdo",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "big",        node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "blockquote", node_unknown, cainteoir::document_events::unknown,   0 },
	{ "body",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "br",         node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "button",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "canvas",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "caption",    node_unknown, cainteoir::document_events::unknown,   0 },
	{ "center",     node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "cite",       node_unknown, cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "code",       node_unknown, cainteoir::document_events::span,      cainteoir::document_events::monospace },
	{ "col",        node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "colgroup",   node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "command",    node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "data",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "datalist",   node_unknown, cainteoir::document_events::unknown,   0 },
	{ "dd",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "del",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "details",    node_unknown, cainteoir::document_events::unknown,   0 },
	{ "dfn",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "div",        node_unknown, cainteoir::document_events::paragraph, 0 },
	{ "dl",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "dt",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "em",         node_unknown, cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "embed",      node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "fieldset",   node_unknown, cainteoir::document_events::unknown,   0 },
	{ "figcaption", node_unknown, cainteoir::document_events::unknown,   0 },
	{ "figure",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "font",       node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "footer",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "form",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "frame",      node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "frameset",   node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "h1",         node_unknown, cainteoir::document_events::heading,   1 },
	{ "h2",         node_unknown, cainteoir::document_events::heading,   2 },
	{ "h3",         node_unknown, cainteoir::document_events::heading,   3 },
	{ "h4",         node_unknown, cainteoir::document_events::heading,   4 },
	{ "h5",         node_unknown, cainteoir::document_events::heading,   5 },
	{ "h6",         node_unknown, cainteoir::document_events::heading,   6 },
	{ "head",       node_head,    cainteoir::document_events::unknown,   0 },
	{ "header",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "hgroup",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "hr",         node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "html",       node_html,    cainteoir::document_events::unknown,   0 },
	{ "i",          node_unknown, cainteoir::document_events::span,      cainteoir::document_events::emphasized },
	{ "iframe",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "img",        node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "input",      node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "ins",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "isindex",    node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "kbd",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "keygen",     node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "label",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "legend",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "li",         node_li,      cainteoir::document_events::unknown,   0 },
	{ "link",       node_link,    cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "map",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "mark",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "marquee",    node_unknown, cainteoir::document_events::unknown,   0 },
	{ "menu",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "meta",       node_meta,    cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "meter",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "nav",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "noad",       node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag }, // adsense, e.g. m.fanfiction.net
	{ "noframes",   node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "noscript",   node_unknown, cainteoir::document_events::unknown,   0 },
	{ "object",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "ol",         node_unknown, cainteoir::document_events::list,      number_list },
	{ "optgroup",   node_unknown, cainteoir::document_events::unknown,   0 },
	{ "option",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "output",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "p",          node_unknown, cainteoir::document_events::paragraph, 0 },
	{ "param",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "pre",        node_unknown, cainteoir::document_events::paragraph, cainteoir::document_events::monospace },
	{ "progress",   node_unknown, cainteoir::document_events::unknown,   0 },
	{ "q",          node_unknown, cainteoir::document_events::unknown,   0 },
	{ "rp",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "rt",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "ruby",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "s",          node_unknown, cainteoir::document_events::unknown,   0 },
	{ "samp",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "script",     node_script,  cainteoir::document_events::unknown,   0 },
	{ "section",    node_unknown, cainteoir::document_events::unknown,   0 },
	{ "select",     node_unknown, cainteoir::document_events::unknown,   0 },
	{ "small",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "source",     node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "span",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "strike",     node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "strong",     node_unknown, cainteoir::document_events::span,      cainteoir::document_events::strong },
	{ "style",      node_style,   cainteoir::document_events::unknown,   0 },
	{ "sub",        node_unknown, cainteoir::document_events::span,      cainteoir::document_events::subscript },
	{ "summary",    node_unknown, cainteoir::document_events::unknown,   0 },
	{ "sup",        node_unknown, cainteoir::document_events::span,      cainteoir::document_events::superscript },
	{ "table",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "tbody",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "td",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "textarea",   node_unknown, cainteoir::document_events::unknown,   0 },
	{ "tfoot",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "th",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "thead",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "time",       node_unknown, cainteoir::document_events::unknown,   0 },
	{ "title",      node_title,   cainteoir::document_events::unknown,   0 },
	{ "tr",         node_unknown, cainteoir::document_events::unknown,   0 },
	{ "track",      node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
	{ "tt",         node_unknown, cainteoir::document_events::unknown,   0 }, // html4
	{ "u",          node_unknown, cainteoir::document_events::span,      cainteoir::document_events::underline },
	{ "ul",         node_unknown, cainteoir::document_events::list,      bullet_list },
	{ "var",        node_unknown, cainteoir::document_events::unknown,   0 },
	{ "video",      node_unknown, cainteoir::document_events::unknown,   0 },
	{ "wbr",        node_unknown, cainteoir::document_events::unknown,   0, xml::context::implicit_end_tag },
};

static const std::initializer_list<const xml::context::entry> html_attr_data =
{
	{ "id",   attr_id,   cainteoir::document_events::unknown, 0 },
	{ "lang", attr_lang, cainteoir::document_events::unknown, 0 },
};

const xml::context xml::html_nodes(html_node_data);
const xml::context xml::html_attrs(html_attr_data);

class html_reader : public xml::reader
{
public:
	html_reader(std::tr1::shared_ptr<cainteoir::buffer> aData)
		: xml::reader(aData)
		, mContext(&xml::unknown_context)
	{
	}

	bool read();

	const xml::context::entry *context() const { return mContext; }
private:
	const xml::context::entry *mContext;
};

bool html_reader::read()
{
	bool ret = xml::reader::read();
	if (ret) switch (nodeType())
	{
	case xml::reader::beginTagNode:
		mContext = xml::html_nodes.lookup(nodeName());
		if (mContext->parse_type == xml::context::implicit_end_tag)
			hasImplicitEndTag();
		if (!mContext->name)
			fprintf(stderr, "html parser: unknown html tag '%s'\n", nodeName().str().c_str());
		break;
	case xml::reader::endTagNode:
		mContext = xml::html_nodes.lookup(nodeName());
		break;
	case xml::reader::attribute:
		mContext = xml::html_attrs.lookup(nodeName());
		break;
	default:
		mContext = &xml::unknown_context;
		break;
	}
	return ret;
}

void skipNode(html_reader &reader, const cainteoir::buffer name)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (!reader.nodeName().compare(name))
			return;
		break;
	}
}

void parseTitleNode(html_reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::endTagNode:
		if (reader.context()->id == node_title)
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

void parseHeadNode(html_reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		switch (reader.context()->id)
		{
		case node_title:
			parseTitleNode(reader, aSubject, events, aGraph);
			break;
		case node_meta:
		case node_link:
			skipNode(reader, reader.nodeName());
			break;
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context()->id == node_head)
			return;
		break;
	}
}

void parseListNode(html_reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, const xml::context::entry *list_ctx)
{
	int number = 1;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		if (reader.context()->id == node_li)
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
		if (reader.context() == list_ctx)
		{
			events.end_context();
			return;
		}
		if (reader.context()->id == node_li)
			events.end_context();
		break;
	}
}

void parseBodyNode(html_reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, const xml::context::entry *body_ctx)
{
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context()->id == attr_id)
			events.anchor(rdf::uri(aSubject.str(), reader.nodeValue().str()), std::string());
		break;
	case xml::reader::beginTagNode:
		if (reader.context()->id == node_script ||
		    reader.context()->id == node_style)
			skipNode(reader, reader.nodeName());
		else if (reader.context()->context != cainteoir::document_events::unknown)
		{
			events.begin_context((cainteoir::document_events::context)reader.context()->context, reader.context()->parameter);
			if (reader.context()->context == cainteoir::document_events::list)
				parseListNode(reader, aSubject, events, reader.context());
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
		if (reader.context() == body_ctx)
			return;
		if (reader.context()->context != cainteoir::document_events::unknown)
			events.end_context();
		break;
	}
}

void parseHtmlNode(html_reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	std::string lang;
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		switch (reader.context()->id)
		{
		case node_head:
			parseHeadNode(reader, aSubject, events, aGraph);
			break;
		default:
			parseBodyNode(reader, aSubject, events, reader.context());
			break;
		}
		break;
	case xml::reader::attribute:
		if (reader.context()->id == attr_lang && lang.empty())
		{
			lang = reader.nodeValue().buffer()->str();
			aGraph.statement(aSubject, rdf::dc("language"), rdf::literal(lang));
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context()->id == node_html)
			return;
	}
}

void cainteoir::parseXHtmlDocument(std::tr1::shared_ptr<cainteoir::buffer> data, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	html_reader reader(data);

	if (reader.isPlainText())
	{
		events.text(data);
		return;
	}

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::beginTagNode:
		switch (reader.context()->id)
		{
		case node_html:
			parseHtmlNode(reader, aSubject, events, aGraph);
			break;
		case node_head:
			parseHeadNode(reader, aSubject, events, aGraph);
			break;
		default:
			parseBodyNode(reader, aSubject, events, reader.context());
			break;
		}
		break;
	}
}
