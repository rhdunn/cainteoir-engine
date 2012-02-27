/* SSML Document Parser.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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
#include <stdexcept>

namespace xml   = cainteoir::xml;
namespace xmlns = cainteoir::xml::xmlns;
namespace rdf   = cainteoir::rdf;

namespace ssml
{
	namespace events = cainteoir::events;

	static const xml::context::entry emphasis_node  = { events::span,      events::emphasized };
	static const xml::context::entry meta_node      = { events::unknown,   0 };
	static const xml::context::entry p_node         = { events::paragraph, 0 };
	static const xml::context::entry s_node         = { events::sentence,  0 };
	static const xml::context::entry speak_node     = { events::unknown,   0 };

	static const xml::context::entry content_attr = { events::unknown, 0 };
	static const xml::context::entry level_attr   = { events::unknown, 0 };
	static const xml::context::entry name_attr    = { events::unknown, 0 };

	static const xml::context::entry emphasis_strong  = { events::span, events::strong };
	static const xml::context::entry emphasis_reduced = { events::span, events::reduced };
	static const xml::context::entry emphasis_none    = { events::span, events::nostyle };
}

static const std::initializer_list<const xml::context::entry_ref> ssml_nodes =
{
	{ "emphasis", &ssml::emphasis_node },
	{ "meta",     &ssml::meta_node },
	{ "p",        &ssml::p_node },
	{ "s",        &ssml::s_node },
	{ "speak",    &ssml::speak_node },
};

static const std::initializer_list<const xml::context::entry_ref> ssml_attrs =
{
	{ "content", &ssml::content_attr },
	{ "level",   &ssml::level_attr },
	{ "name",    &ssml::name_attr },
};

void parseSsmlContext(xml::reader &reader, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph, const xml::context::entry *ctx)
{
	const xml::context::entry *emphasis = nullptr;
	if (ctx == &ssml::emphasis_node)
		emphasis = ctx;
	else
		events.begin_context((cainteoir::events::context)reader.context()->context, reader.context()->parameter);

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (ctx == &ssml::emphasis_node)
		{
			if (reader.context() == &ssml::level_attr && emphasis != nullptr)
			{
				std::string value = reader.nodeValue().str();
				if (value == "strong")
					emphasis = &ssml::emphasis_strong;
				else if (value == "reduced")
					emphasis = &ssml::emphasis_reduced;
				else if (value == "none")
					emphasis = &ssml::emphasis_none;
			}
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		{
			if (emphasis != nullptr)
			{
				events.begin_context((cainteoir::events::context)emphasis->context, emphasis->parameter);
				emphasis = nullptr;
			}
			std::shared_ptr<cainteoir::buffer> text = reader.nodeValue().content();
			if (text)
				events.text(text);
		}
		break;
	case xml::reader::endTagNode:
		if (reader.context() == ctx && emphasis == nullptr)
		{
			events.end_context();
			return;
		}
		break;
	case xml::reader::beginTagNode:
		if (reader.context()->context != cainteoir::events::unknown)
		{
			if (emphasis != nullptr)
			{
				events.begin_context((cainteoir::events::context)emphasis->context, emphasis->parameter);
				emphasis = nullptr;
			}
			parseSsmlContext(reader, aSubject, events, aGraph, reader.context());
		}
		break;
	}

	events.end_context();
}

void cainteoir::parseSsmlDocument(xml::reader &reader, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	reader.set_nodes(xmlns::ssml, ssml_nodes);
	reader.set_attrs(xmlns::ssml, ssml_attrs);
	reader.set_attrs(xmlns::xml,  xml::attrs);

	const xml::context::entry *ctx = &ssml::speak_node;
	std::string name;
	std::string content;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (ctx == &ssml::speak_node)
		{
			if (reader.context() == &xml::lang_attr)
				aGraph.statement(aSubject, rdf::dc("language"), rdf::literal(reader.nodeValue().str()));
		}
		else if (ctx == &ssml::meta_node)
		{
			if (reader.context() == &ssml::name_attr)
				name = reader.nodeValue().str();
			else if (reader.context() == &ssml::content_attr)
				content = reader.nodeValue().str();
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		{
			std::shared_ptr<cainteoir::buffer> text = reader.nodeValue().content();
			if (text)
				events.text(text);
		}
		break;
	case xml::reader::endTagNode:
		if (ctx == &ssml::meta_node)
		{
			if (name == "seeAlso" && !content.empty())
			{
				aGraph.statement(aSubject, rdf::rdfs("seeAlso"), aGraph.href(content));
				name.clear();
				content.clear();
			}
		}
		break;
	case xml::reader::beginTagNode:
		ctx = reader.context();
		if (ctx == &ssml::p_node)
			parseSsmlContext(reader, aSubject, events, aGraph, ctx);
		break;
	}
}
