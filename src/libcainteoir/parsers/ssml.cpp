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

#include "config.h"
#include "compatibility.hpp"

#include "parsers.hpp"
#include <stdexcept>

namespace xml    = cainteoir::xml;
namespace xmlns  = cainteoir::xml::xmlns;
namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

#ifndef DOXYGEN
namespace ssml
{
	static const xml::context::entry emphasis_node  = { &cainteoir::emphasized };
	static const xml::context::entry meta_node      = {};
	static const xml::context::entry p_node         = { &cainteoir::paragraph };
	static const xml::context::entry s_node         = { events::sentence };
	static const xml::context::entry speak_node     = {};

	static const xml::context::entry content_attr = {};
	static const xml::context::entry level_attr   = {};
	static const xml::context::entry name_attr    = {};

	static const xml::context::entry emphasis_strong  = { &cainteoir::strong };
	static const xml::context::entry emphasis_reduced = { &cainteoir::reduced };
	static const xml::context::entry emphasis_none    = { &cainteoir::span };
}
#endif

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

struct ssml_document_reader : public cainteoir::document_reader
{
	ssml_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle);

	bool read();

	std::shared_ptr<xml::reader> reader;
};

ssml_document_reader::ssml_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle)
	: reader(aReader)
{
	reader->set_nodes(xmlns::ssml, ssml_nodes);
	reader->set_attrs(xmlns::ssml, ssml_attrs);
	reader->set_attrs(xmlns::xml,  xml::attrs);

	const xml::context::entry *current = reader->context();
	std::string name;
	std::string content;

	bool in_header = true;
	while (in_header && reader->read()) switch (reader->nodeType())
	{
	case xml::reader::attribute:
		if (current == &ssml::speak_node)
		{
			if (reader->context() == &xml::lang_attr)
				aPrimaryMetadata.statement(aSubject, rdf::dc("language"), rdf::literal(reader->nodeValue().str()));
		}
		else if (current == &ssml::meta_node)
		{
			if (reader->context() == &ssml::name_attr)
				name = reader->nodeValue().str();
			else if (reader->context() == &ssml::content_attr)
				content = reader->nodeValue().str();
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		text = reader->nodeValue().normalize();
		in_header = text->empty();
		break;
	case xml::reader::beginTagNode:
		current = reader->context();
		if (current != &ssml::meta_node)
			in_header = false;
		break;
	case xml::reader::endTagNode:
		if (current == &ssml::meta_node)
		{
			if (name == "seeAlso" && !content.empty())
			{
				aPrimaryMetadata.statement(aSubject, rdf::rdfs("seeAlso"), aPrimaryMetadata.href(content));
				name.clear();
				content.clear();
			}
		}
		break;
	}

	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("application/ssml+xml"));
}

bool ssml_document_reader::read()
{
	const xml::context::entry *current = nullptr;

	do switch (reader->nodeType())
	{
	case xml::reader::attribute:
		if (current == &ssml::emphasis_node && reader->context() == &ssml::level_attr)
		{
			std::string value = reader->nodeValue().str();
			if (value == "strong")
				current = &ssml::emphasis_strong;
			else if (value == "reduced")
				current = &ssml::emphasis_reduced;
			else if (value == "none")
				current = &ssml::emphasis_none;
		}
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		text = reader->nodeValue().content();
		type = 0;
		if (text)
		{
			type     |= events::text;
			context   = events::span;
			parameter = 0;
		}
		if (current != nullptr && current->context != events::unknown)
		{
			type     |= events::begin_context;
			context   = (events::context)current->context;
			parameter = current->parameter;
			styles    = nullptr;
			current = nullptr;
		}
		else if (current != nullptr && current->styles)
		{
			type     |= events::begin_context;
			context   = events::unknown;
			parameter = 0;
			styles    = current->styles;
			current = nullptr;
		}
		if (type != 0)
		{
			anchor    = rdf::uri();
			reader->read();
			return true;
		}
		break;
	case xml::reader::beginTagNode:
		current = reader->context();
		break;
	case xml::reader::endTagNode:
		current = nullptr;
		if (reader->context()->context != events::unknown)
		{
			type      = events::end_context;
			context   = (events::context)reader->context()->context;
			parameter = reader->context()->parameter;
			reader->read();
			return true;
		}
		else if (reader->context()->styles)
		{
			type      = events::end_context;
			context   = events::unknown;
			parameter = 0;
			styles    = reader->context()->styles;
			reader->read();
			return true;
		}
		break;
	} while (reader->read());

	return false;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createSsmlReader(const std::shared_ptr<xml::reader> &aReader,
                            const rdf::uri &aSubject,
                            rdf::graph &aPrimaryMetadata,
                            const std::string &aTitle)
{
	if (!aReader)
		return std::shared_ptr<document_reader>();

	return std::make_shared<ssml_document_reader>(aReader, aSubject, aPrimaryMetadata, aTitle);
}
