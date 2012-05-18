/* RDF/XML Document Parser.
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
#include <stdexcept>

namespace xml   = cainteoir::xml;
namespace xmlns = cainteoir::xml::xmlns;
namespace rdf   = cainteoir::rdf;

namespace cainteoir { namespace rdf
{
	namespace events = cainteoir::events;

	static const xml::context::entry li_node          = { events::unknown, 0 };
	static const xml::context::entry type_node        = { events::unknown, 0 };
	static const xml::context::entry Alt_node         = { events::unknown, 0 };
	static const xml::context::entry Bag_node         = { events::unknown, 0 };
	static const xml::context::entry Description_node = { events::unknown, 0 };
	static const xml::context::entry Seq_node         = { events::unknown, 0 };
	static const xml::context::entry RDF_node         = { events::unknown, 0 };

	static const xml::context::entry about_attr     = { events::unknown, 0 };
	static const xml::context::entry datatype_attr  = { events::unknown, 0 };
	static const xml::context::entry nodeID_attr    = { events::unknown, 0 };
	static const xml::context::entry parseType_attr = { events::unknown, 0 };
	static const xml::context::entry resource_attr  = { events::unknown, 0 };
	static const xml::context::entry ID_attr        = { events::unknown, 0 };
}}

static const std::initializer_list<const xml::context::entry_ref> rdf_nodes =
{
	{ "Alt",         &rdf::Alt_node },
	{ "Bag",         &rdf::Bag_node },
	{ "Description", &rdf::Description_node },
	{ "RDF",         &rdf::RDF_node },
	{ "Seq",         &rdf::Seq_node },
	//{ "_nnn",      &rdf::_nnn_node }, // _1, _2, _3, ... -- "_(:int)"
	{ "li",          &rdf::li_node },
	{ "type",        &rdf::type_node },
};

static const std::initializer_list<const xml::context::entry_ref> rdf_attrs =
{
	{ "ID",        &rdf::ID_attr },
	{ "about",     &rdf::about_attr },
	{ "datatype",  &rdf::datatype_attr },
	{ "nodeID",    &rdf::nodeID_attr },
	{ "parseType", &rdf::parseType_attr },
	{ "resource",  &rdf::resource_attr },
};

inline rdf::uri uri(xml::reader &reader)
{
	return rdf::uri(reader.namespaceUri(), reader.nodeName().str());
}

static void parseInnerRdfXml(xml::reader &reader, const rdf::uri &aSubject, rdf::graph &aGraph, const rdf::uri &self, const std::string &base, std::string lang);

static rdf::uri parseOuterRdfXml(xml::reader &reader, rdf::graph &aGraph, const rdf::uri &self, const std::string &base, std::string lang);

void parseRdfXmlCollection(xml::reader &reader, rdf::uri first, rdf::graph &aGraph, const rdf::uri &self, const std::string &base)
{
	rdf::uri subject;
	rdf::uri prev;
	rdf::uri current = uri(reader);
	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &rdf::about_attr)
		{
			std::string about = reader.nodeValue().str();
			subject = aGraph.href((*about.begin()) == '#' ? base + about : about);
		}
		break;
	case xml::reader::beginTagNode:
		prev  = first;
		first = aGraph.genid();
		aGraph.statement(prev, rdf::rdf("rest"), first);
		current = uri(reader);
		break;
	case xml::reader::endTagNode:
		if (uri(reader) == self)
		{
			aGraph.statement(first, rdf::rdf("rest"), rdf::rdf("nil"));
			return;
		}
		aGraph.statement(first, rdf::rdf("first"), subject);
		if (current != rdf::rdf("Description"))
			aGraph.statement(subject, rdf::rdf("type"), current);
		break;
	}
}

void parseRdfXmlContainer(xml::reader &reader, rdf::graph &aGraph, const rdf::uri &self, const std::string &base)
{
	rdf::uri subject;
	rdf::uri ref;
	long item = 1;

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (reader.context() == &rdf::about_attr)
		{
			std::string about = reader.nodeValue().str();
			subject = aGraph.href((*about.begin()) == '#' ? base + about : about);
			aGraph.statement(subject, rdf::rdf("type"), self);
		}
		else if (reader.context() == &rdf::resource_attr)
		{
			std::string resource = reader.nodeValue().str();
			if (resource.find("://") == std::string::npos)
				aGraph.statement(subject, ref, aGraph.href(base + resource));
			else
				aGraph.statement(subject, ref, aGraph.href(resource));
		}
		break;
	case xml::reader::beginTagNode:
		if (reader.context() == &rdf::li_node)
		{
			std::ostringstream id;
			id << '_' << item;
			ref = rdf::rdf(id.str());
			++item;
		}
		else
			ref = uri(reader);
		break;
	case xml::reader::endTagNode:
		if (uri(reader) == self)
		{
			return;
		}
		break;
	}
}

void parseInnerRdfXml(xml::reader &reader, const rdf::uri &aSubject, rdf::graph &aGraph, const rdf::uri &self, const std::string &base, std::string lang)
{
	std::string resource;
	std::string nodeID;
	std::string parseType;
	std::string datatype;
	std::string value;

	rdf::uri context;

	std::list<std::pair<rdf::uri, rdf::literal>> inline_metadata;
	bool processed_metadata = false;

	while (reader.read())
	{
		if (reader.nodeType() == xml::reader::attribute)
		{
			if (reader.context() == &rdf::datatype_attr)
				datatype = reader.nodeValue().str();
			else if (reader.context() == &rdf::resource_attr)
				resource = reader.nodeValue().str();
			else if (reader.context() == &rdf::nodeID_attr)
				nodeID = reader.nodeValue().str();
			else if (reader.context() == &xml::lang_attr)
				lang = reader.nodeValue().str();
			else if (reader.context() == &rdf::parseType_attr)
			{
				parseType = reader.nodeValue().str();
				context = aGraph.genid();
			}
			else
				inline_metadata.push_back(std::make_pair(uri(reader), rdf::literal(reader.nodeValue().str(), lang)));
		}
		else
		{
			if (!processed_metadata && !inline_metadata.empty())
			{
				processed_metadata = true;
				rdf::uri subject = aGraph.genid();
				foreach_iter (metadata, inline_metadata)
					aGraph.statement(subject, metadata->first, metadata->second);
				aGraph.statement(aSubject, self, subject);
			}

			switch (reader.nodeType())
			{
			case xml::reader::textNode:
			case xml::reader::cdataNode:
				value = reader.nodeValue().normalize()->str();
				break;
			case xml::reader::beginTagNode:
				if (parseType == "Resource")
					parseInnerRdfXml(reader, context, aGraph, uri(reader), base, lang);
				else if (parseType == "Collection")
				{
					parseRdfXmlCollection(reader, context, aGraph, self, base);
					aGraph.statement(aSubject, self, context);
					return;
				}
				else
				{
					rdf::uri context = parseOuterRdfXml(reader, aGraph, uri(reader), base, lang);
					aGraph.statement(aSubject, self, context);
				}
				break;
			case xml::reader::endTagNode:
				if (uri(reader) == self)
				{
					if (parseType == "Resource")
						aGraph.statement(aSubject, self, context);
					if (!resource.empty())
					{
						if (resource.find("://") == std::string::npos)
							aGraph.statement(aSubject, self, aGraph.href(base + resource));
						else
							aGraph.statement(aSubject, self, aGraph.href(resource));
					}
					else if (!nodeID.empty())
						aGraph.statement(aSubject, self, rdf::bnode(nodeID));
					else if (!datatype.empty())
						aGraph.statement(aSubject, self, rdf::literal(value, aGraph.href(datatype)));
					else if (!value.empty())
						aGraph.statement(aSubject, self, rdf::literal(value, lang));
					return;
				}
				break;
			}
		}
	}
}

rdf::uri parseOuterRdfXml(xml::reader &reader, rdf::graph &aGraph, const rdf::uri &self, const std::string &base, std::string lang)
{
	rdf::uri subject;

	std::list<std::pair<rdf::uri, rdf::literal>> inline_metadata;
	bool processed_metadata = false;

	if (reader.context() == &rdf::Seq_node)
	{
		parseRdfXmlContainer(reader, aGraph, self, base);
		return self;
	}
	else while (reader.read())
	{
		if (reader.nodeType() == xml::reader::attribute)
		{
			if (reader.context() == &rdf::about_attr)
			{
				std::string about = reader.nodeValue().str();
				subject = aGraph.href((*about.begin()) == '#' ? base + about : about);
			}
			else if (reader.context() == &rdf::nodeID_attr)
			{
				std::string nodeID = reader.nodeValue().str();
				subject = rdf::bnode(nodeID);
			}
			else if (reader.context() == &rdf::ID_attr)
			{
				std::string ID = reader.nodeValue().str();
				subject = aGraph.href(base + '#' + ID);
			}
			else if (reader.context() == &xml::lang_attr)
				lang = reader.nodeValue().str();
			else
				inline_metadata.push_back(std::make_pair(uri(reader), rdf::literal(reader.nodeValue().str(), lang)));
		}
		else
		{
			if (!processed_metadata)
			{
				processed_metadata = true;
				if (subject.empty())
					subject = aGraph.genid();
				if (self != rdf::rdf("Description"))
					aGraph.statement(subject, rdf::rdf("type"), self);
				foreach_iter (metadata, inline_metadata)
					aGraph.statement(subject, metadata->first, metadata->second);
			}

			switch (reader.nodeType())
			{
			case xml::reader::beginTagNode:
				parseInnerRdfXml(reader, subject, aGraph, uri(reader), base, lang);
				break;
			case xml::reader::endTagNode:
				if (uri(reader) == self)
					return subject;
				break;
			}
		}
	}

	return subject;
}

struct rdfxml_document_reader : public cainteoir::document_reader
{
	bool read()
	{
		return false;
	}
};

std::shared_ptr<cainteoir::document_reader>
cainteoir::createRdfXmlReader(xml::reader &aReader,
                              const rdf::uri &aSubject,
                              rdf::graph &aPrimaryMetadata)
{
	aReader.set_nodes(xmlns::rdf, rdf_nodes);
	aReader.set_attrs(xmlns::rdf, rdf_attrs);
	aReader.set_attrs(xmlns::xml, xml::attrs);

	std::string base;

	while (aReader.read()) switch (aReader.nodeType())
	{
	case xml::reader::attribute:
		if (aReader.context() == &xml::base_attr)
			base = aReader.nodeValue().str();
		break;
	case xml::reader::textNode:
	case xml::reader::cdataNode:
		break;
	case xml::reader::beginTagNode:
		parseOuterRdfXml(aReader, aPrimaryMetadata, uri(aReader), base, std::string());
		break;
	case xml::reader::endTagNode:
		break;
	}

	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("application/rdf+xml"));
	return std::make_shared<rdfxml_document_reader>();
}
