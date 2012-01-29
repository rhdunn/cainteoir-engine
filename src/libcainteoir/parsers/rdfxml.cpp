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

static void parseRdfXmlInnerMetadata(const xml::node &rdfxml, const rdf::uri &subject, rdf::graph &aGraph, const std::string &base, std::string lang);
static void parseRdfXmlOuterMetadata(const xml::node &rdfxml, const rdf::uri &subject, rdf::graph &aGraph, const std::string &base, std::string lang);

bool hasSubElements(const xml::node &rdfxml)
{
	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
			return true;
	}
	return false;
}

void parseRdfXmlMetadata(const xml::node &rdfxml, const rdf::uri &subject, rdf::graph &aGraph, const std::string &base, const std::string &lang)
{
	if (rdfxml != rdf::rdf("Description"))
		aGraph.statement(subject, rdf::rdf("type"), rdf::uri(rdfxml.namespaceURI(), rdfxml.name()));

	parseRdfXmlInnerMetadata(rdfxml, subject, aGraph, base, lang);
}

rdf::uri parseRdfXmlCollectionMetadata(xml::node node, const rdf::uri &subject, rdf::graph &aGraph, const std::string &base, const std::string &lang)
{
	for (; node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			std::string about = node.attr(rdf::rdf("about")).content();
			if (!about.empty())
			{
				const rdf::uri temp = aGraph.genid();
				aGraph.statement(temp, rdf::rdf("first"), aGraph.href((*about.begin()) == '#' ? base + about : about));

				node.next();
				if (node.isValid())
				{
					const rdf::uri rest = parseRdfXmlCollectionMetadata(node, subject, aGraph, base, lang);
					if (rest.ref.empty())
						aGraph.statement(temp, rdf::rdf("rest"), rdf::rdf("nil"));
					else
						aGraph.statement(temp, rdf::rdf("rest"), rest);
				}
				return temp;
			}
		}
	}
	return aGraph.bnode(std::string());
}

void parseRdfXmlMetadataFromNode(
	const xml::node &node,
	const rdf::uri &subject,
	const rdf::uri &predicate,
	rdf::graph &aGraph,
	const std::string &base,
	const std::string &baseLang)
{
	std::string resource = node.attr(rdf::rdf("resource")).content();
	std::string nodeID   = node.attr(rdf::rdf("nodeID")).content();
	std::string datatype = node.attr(rdf::rdf("datatype")).content();

	if (!resource.empty())
	{
		if (resource.find("://") == std::string::npos)
			aGraph.statement(subject, predicate, aGraph.href(base + resource));
		else
			aGraph.statement(subject, predicate, aGraph.href(resource));
	}
	else if (!nodeID.empty())
		aGraph.statement(subject, predicate, aGraph.bnode(nodeID));
	else if (hasSubElements(node))
	{
		std::string parseType = node.attr(rdf::rdf("parseType")).content();

		if (parseType == "Resource")
		{
			const rdf::uri temp = aGraph.genid();
			parseRdfXmlInnerMetadata(node, temp, aGraph, base, baseLang);
			aGraph.statement(subject, predicate, temp);
		}
		else if (parseType == "Collection")
		{
			const rdf::uri first = parseRdfXmlCollectionMetadata(node.firstChild(), subject, aGraph, base, baseLang);
			aGraph.statement(subject, predicate, first);
		}
		else
		{
			const rdf::uri temp = aGraph.genid();
			parseRdfXmlOuterMetadata(node, temp, aGraph, base, baseLang);
			aGraph.statement(subject, predicate, temp);
		}
	}
	else
	{
		std::string lang = node.attr(rdf::xml("lang")).content();
		std::string value = node.content()->str();

		if (!datatype.empty())
			aGraph.statement(subject, predicate, rdf::literal(value, aGraph.href(datatype)));
		else if (!value.empty())
		{
			if (!lang.empty())
				aGraph.statement(subject, predicate, rdf::literal(value, lang));
			else
				aGraph.statement(subject, predicate, rdf::literal(value, baseLang));
		}
	}
}

void parseRdfXmlInnerMetadata(const xml::node &rdfxml, const rdf::uri &subject, rdf::graph &aGraph, const std::string &base, std::string lang)
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
				aGraph.statement(subject, rdf::uri(attr.namespaceURI(), attr.name()), rdf::literal(value, lang));
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

			parseRdfXmlMetadataFromNode(node, subject, rdf::rdf(ss.str()), aGraph, base, lang);
		}
		else
			parseRdfXmlMetadataFromNode(node, subject, predicate, aGraph, base, lang);
	}
}

void parseRdfXmlOuterMetadata(const xml::node &rdfxml, const rdf::uri &subject, rdf::graph &aGraph, const std::string &base, std::string lang)
{
	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (subject.ns.empty())
				parseRdfXmlInnerMetadata(node, subject, aGraph, base, lang);
			else
			{
				std::string about = node.attr(rdf::rdf("about")).content();
				std::string nodeID = node.attr(rdf::rdf("nodeID")).content();
				std::string ID = node.attr(rdf::rdf("ID")).content();
				if (!about.empty())
					parseRdfXmlMetadata(node, aGraph.href((*about.begin()) == '#' ? base + about : about), aGraph, base, lang);
				else if (!nodeID.empty())
					parseRdfXmlMetadata(node, aGraph.bnode(nodeID), aGraph, base, lang);
				else if (!ID.empty())
					parseRdfXmlMetadata(node, aGraph.href(base + "#" + ID), aGraph, base, lang);
			}
		}
	}
}

void cainteoir::parseRdfXmlDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	xmldom::document doc(aData);
	xmldom::node rdfxml = doc.root();

	if (rdfxml != rdf::rdf("RDF"))
		throw std::runtime_error(_("RDF/XML document is not of a recognised format."));

	parseRdfXmlOuterMetadata(rdfxml, aSubject, aGraph, rdfxml.attr(rdf::xml("base")).content(), std::string());
}
