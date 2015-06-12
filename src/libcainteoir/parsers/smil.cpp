/* SMIL Document Parser.
 *
 * Copyright (C) 2010-2015 Reece H. Dunn
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
#include <cainteoir/path.hpp>
#include <stdexcept>

namespace xml    = cainteoir::xml;
namespace xmlns  = cainteoir::xml::xmlns;
namespace events = cainteoir::events;
namespace rdf    = cainteoir::rdf;
namespace css    = cainteoir::css;

namespace smil
{
	namespace events = cainteoir::events;

	static const xml::context::entry audio_node    = {};
	static const xml::context::entry body_node     = {};
	static const xml::context::entry head_node     = {};
	static const xml::context::entry metadata_node = {};
	static const xml::context::entry par_node      = { &cainteoir::parallel };
	static const xml::context::entry seq_node      = { &cainteoir::sequential };
	static const xml::context::entry smil_node     = {};
	static const xml::context::entry text_node     = {};

	static const xml::context::entry clipBegin_attr = {};
	static const xml::context::entry clipEnd_attr   = {};
	static const xml::context::entry src_attr       = {};
}

static const std::initializer_list<const xml::context::entry_ref> smil_nodes =
{
	{ "audio",    &smil::audio_node },
	{ "body",     &smil::body_node },
	{ "head",     &smil::head_node },
	{ "metadata", &smil::metadata_node },
	{ "par",      &smil::par_node },
	{ "seq",      &smil::seq_node },
	{ "smil",     &smil::smil_node },
	{ "text",     &smil::text_node },
};

static const std::initializer_list<const xml::context::entry_ref> smil_attrs =
{
	{ "clipBegin", &smil::clipBegin_attr },
	{ "clipEnd",   &smil::clipEnd_attr },
	{ "src",       &smil::src_attr },
};

struct smil_document_reader : public cainteoir::document_reader
{
	smil_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata);

	bool read(rdf::graph *aMetadata);

	std::shared_ptr<xml::reader> reader;
	cainteoir::path mBasePath;
};

smil_document_reader::smil_document_reader(const std::shared_ptr<xml::reader> &aReader, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata)
	: reader(aReader)
	, mBasePath(cainteoir::path(aSubject.str()).parent())
{
	aReader->set_nodes(xmlns::smil, smil_nodes);
	aReader->set_attrs(xmlns::smil, smil_attrs);
	aReader->set_attrs(xmlns::xml,  xml::attrs);

	const xml::context::entry *current = aReader->context();

	while (aReader->read() && current != &smil::body_node) switch (aReader->nodeType())
	{
	case xml::reader::attribute:
		if (current == &smil::smil_node)
		{
			if (aReader->context() == &xml::lang_attr)
				aPrimaryMetadata.statement(aSubject, rdf::dc("language"), rdf::literal(aReader->nodeValue().str()));
		}
		break;
	case xml::reader::beginTagNode:
		current = aReader->context();
		break;
	default:
		break;
	}

	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("application/smil"));
}

bool smil_document_reader::read(rdf::graph *aMetadata)
{
	const xml::context::entry *current = nullptr;
	cainteoir::path src;
	css::time clipBegin;
	css::time clipEnd;

	while (reader->read()) switch (reader->nodeType())
	{
	case xml::reader::beginTagNode:
		current = reader->context();
		if (reader->context()->styles)
		{
			clear().begin_context_event(reader->context()->styles);
			return true;
		}
		break;
	case xml::reader::attribute:
		if (current == &smil::text_node)
		{
			if (reader->context() == &smil::src_attr)
				src = mBasePath / reader->nodeValue().str();
		}
		else if (current == &smil::audio_node)
		{
			if (reader->context() == &smil::src_attr)
				src = mBasePath / reader->nodeValue().str();
			else if (reader->context() == &smil::clipBegin_attr)
				clipBegin = css::parse_smil_time(*reader->nodeValue().buffer());
			else if (reader->context() == &smil::clipEnd_attr)
				clipEnd = css::parse_smil_time(*reader->nodeValue().buffer());
		}
		break;
	case xml::reader::endTagNode:
		if (reader->context()->styles)
		{
			clear().end_context_event(reader->context()->styles);
			return true;
		}
		else if (current == &smil::text_node)
		{
			clear().text_ref_event(rdf::href(src.str()));
			return true;
		}
		else if (current == &smil::audio_node)
		{
			clear().media_ref_event(rdf::href(src.str()), {}, clipBegin, clipEnd);
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createSmilReader(const std::shared_ptr<xml::reader> &aReader,
                            const rdf::uri &aSubject,
                            rdf::graph &aPrimaryMetadata,
                            const std::string &aTitle)
{
	if (!aReader)
		return std::shared_ptr<document_reader>();

	return std::make_shared<smil_document_reader>(aReader, aSubject, aPrimaryMetadata);
}
