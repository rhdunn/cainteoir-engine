/* RDF/XML Document Parser.
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

static void parseRdfXmlInnerMetadata(const xml::node &rdfxml, const rdf::resource &subject, cainteoir::document_events &events, const std::string &base, std::string lang);
static void parseRdfXmlOuterMetadata(const xml::node &rdfxml, const rdf::resource &subject, cainteoir::document_events &events, const std::string &base, std::string lang);

bool hasSubElements(const xml::node &rdfxml)
{
	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
			return true;
	}
	return false;
}

void parseRdfXmlMetadata(const xml::node &rdfxml, const rdf::resource &subject, cainteoir::document_events &events, const std::string &base, const std::string &lang)
{
	if (rdfxml != rdf::rdf("Description"))
		events.metadata(rdf::statement(subject, rdf::rdf("type"), rdf::uri(rdfxml.namespaceURI(), rdfxml.name())));

	parseRdfXmlInnerMetadata(rdfxml, subject, events, base, lang);
}

rdf::uri parseRdfXmlCollectionMetadata(xml::node node, const rdf::resource &subject, cainteoir::document_events &events, const std::string &base, const std::string &lang)
{
	for (; node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			std::string about = node.attr(rdf::rdf("about")).content();
			if (!about.empty())
			{
				const rdf::uri temp = events.genid();
				events.metadata(rdf::statement(temp, rdf::rdf("first"), rdf::href((*about.begin()) == '#' ? base + about : about)));

				node.next();
				if (node.isValid())
				{
					const rdf::uri rest = parseRdfXmlCollectionMetadata(node, subject, events, base, lang);
					if (rest.ref.empty())
						events.metadata(rdf::statement(temp, rdf::rdf("rest"), rdf::rdf("nil")));
					else
						events.metadata(rdf::statement(temp, rdf::rdf("rest"), rest));
				}
				return temp;
			}
		}
	}
	return rdf::bnode(std::string());
}

void parseRdfXmlMetadataFromNode(
	const xml::node &node,
	const rdf::resource &subject,
	const rdf::uri &predicate,
	cainteoir::document_events &events,
	const std::string &base,
	const std::string &baseLang)
{
	std::string resource = node.attr(rdf::rdf("resource")).content();
	std::string nodeID   = node.attr(rdf::rdf("nodeID")).content();
	std::string datatype = node.attr(rdf::rdf("datatype")).content();

	if (!resource.empty())
	{
		if (resource.find("://") == std::string::npos)
			events.metadata(rdf::statement(subject, predicate, rdf::href(base + resource)));
		else
			events.metadata(rdf::statement(subject, predicate, rdf::href(resource)));
	}
	else if (!nodeID.empty())
		events.metadata(rdf::statement(subject, predicate, rdf::bnode(nodeID)));
	else if (hasSubElements(node))
	{
		std::string parseType = node.attr(rdf::rdf("parseType")).content();

		if (parseType == "Resource")
		{
			const rdf::uri temp = events.genid();
			parseRdfXmlInnerMetadata(node, temp, events, base, baseLang);
			events.metadata(rdf::statement(subject, predicate, temp));
		}
		else if (parseType == "Collection")
		{
			const rdf::uri first = parseRdfXmlCollectionMetadata(node.firstChild(), subject, events, base, baseLang);
			events.metadata(rdf::statement(subject, predicate, first));
		}
		else
		{
			const rdf::uri temp = events.genid();
			parseRdfXmlOuterMetadata(node, temp, events, base, baseLang);
			events.metadata(rdf::statement(subject, predicate, temp));
		}
	}
	else
	{
		std::string lang = node.attr(rdf::xml("lang")).content();
		std::string value = node.content()->str();

		if (!datatype.empty())
			events.metadata(rdf::statement(subject, predicate, rdf::literal(value, rdf::href(datatype))));
		else if (!value.empty())
		{
			if (!lang.empty())
				events.metadata(rdf::statement(subject, predicate, rdf::literal(value, lang)));
			else
				events.metadata(rdf::statement(subject, predicate, rdf::literal(value, baseLang)));
		}
	}
}

void parseRdfXmlInnerMetadata(const xml::node &rdfxml, const rdf::resource &subject, cainteoir::document_events &events, const std::string &base, std::string lang)
{
	for (xml::attribute attr = rdfxml.firstAttribute(); attr.isValid(); attr.next())
	{
		if (attr != rdf::rdf("about") && attr != rdf::rdf("nodeID") && attr != rdf::rdf("parseType") && attr != rdf::rdf("ID"))
		{
			if (attr == rdf::xml("lang"))
				lang = attr.content();
			else
			{
				std::string value = attr.content();
				events.metadata(rdf::statement(subject, rdf::uri(attr.namespaceURI(), attr.name()), rdf::literal(value, lang)));
			}
		}
	}

	bool isseq = (rdfxml == rdf::rdf("Seq"));
	int  index = 1;

	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() != XML_ELEMENT_NODE)
			continue;

		const rdf::uri predicate = rdf::uri(node.namespaceURI(), node.name());
		if (isseq && predicate == rdf::rdf("li"))
		{
			std::ostringstream ss;
			ss << '_' << index;
			++index;

			parseRdfXmlMetadataFromNode(node, subject, rdf::rdf(ss.str()), events, base, lang);
		}
		else
			parseRdfXmlMetadataFromNode(node, subject, predicate, events, base, lang);
	}
}

void parseRdfXmlOuterMetadata(const xml::node &rdfxml, const rdf::resource &subject, cainteoir::document_events &events, const std::string &base, std::string lang)
{
	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			const cainteoir::rdf::uri *bnode = rdf::any_type(&subject);
			if (bnode->ns.empty())
				parseRdfXmlInnerMetadata(node, subject, events, base, lang);
			else
			{
				std::string about = node.attr(rdf::rdf("about")).content();
				std::string nodeID = node.attr(rdf::rdf("nodeID")).content();
				std::string ID = node.attr(rdf::rdf("ID")).content();
				if (!about.empty())
					parseRdfXmlMetadata(node, rdf::href((*about.begin()) == '#' ? base + about : about), events, base, lang);
				else if (!nodeID.empty())
					parseRdfXmlMetadata(node, rdf::bnode(nodeID), events, base, lang);
				else if (!ID.empty())
					parseRdfXmlMetadata(node, rdf::href(base + "#" + ID), events, base, lang);
			}
		}
	}
}

void cainteoir::parseRdfXmlDocument(const xml::node &rdfxml, const rdf::uri &subject, cainteoir::document_events &events)
{
	if (rdfxml != rdf::rdf("RDF"))
		throw std::runtime_error(_("RDF/XML document is not of a recognised format."));

	parseRdfXmlOuterMetadata(rdfxml, subject, events, rdfxml.attr(rdf::xml("base")).content(), std::string());
}
