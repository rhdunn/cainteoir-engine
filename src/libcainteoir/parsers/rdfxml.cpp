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

#include <cainteoir/parsers.hpp>

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xmldom;

static void parseRdfXmlInnerMetadata(const xml::node &rdfxml, const rdf::resource &subject, rdf::model &metadata, const std::string &base);
static void parseRdfXmlOuterMetadata(const xml::node &rdfxml, const rdf::resource &subject, rdf::model &metadata, const std::string &base);

bool hasSubElements(const xml::node &rdfxml)
{
	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
			return true;
	}
	return false;
}

void parseRdfXmlMetadata(const xml::node &rdfxml, const rdf::resource &subject, rdf::model &metadata, const std::string &base)
{
	if (rdfxml != rdf::rdf("Description"))
		metadata.push_back(rdf::statement(subject, rdf::rdf("type"), rdf::uri(rdfxml.namespaceURI(), rdfxml.name())));

	parseRdfXmlInnerMetadata(rdfxml, subject, metadata, base);
}

rdf::bnode parseRdfXmlCollectionMetadata(xml::node node, const rdf::resource &subject, rdf::model &metadata, const std::string &base)
{
	for (; node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			std::string about = node.attr(rdf::rdf("about")).content();
			if (!about.empty())
			{
				const rdf::bnode temp = metadata.genid();
				metadata.push_back(rdf::statement(temp, rdf::rdf("first"), rdf::href((*about.begin()) == '#' ? base + about : about)));

				node.next();
				if (node.isValid())
				{
					const rdf::bnode rest = parseRdfXmlCollectionMetadata(node, subject, metadata, base);
					if (rest.id.empty())
						metadata.push_back(rdf::statement(temp, rdf::rdf("rest"), rdf::rdf("nil")));
					else
						metadata.push_back(rdf::statement(temp, rdf::rdf("rest"), rest));
				}
				return temp;
			}
		}
	}
	return rdf::bnode(std::string());
}

void parseRdfXmlMetadataFromNode(const xml::node &node, const rdf::resource &subject, const rdf::uri &predicate, rdf::model &metadata, const std::string &base)
{
	std::string resource = node.attr(rdf::rdf("resource")).content();
	std::string nodeID   = node.attr(rdf::rdf("nodeID")).content();
	std::string datatype = node.attr(rdf::rdf("datatype")).content();

	if (!resource.empty())
	{
		if (resource.find("://") == std::string::npos)
			metadata.push_back(rdf::statement(subject, predicate, rdf::href(base + resource)));
		else
			metadata.push_back(rdf::statement(subject, predicate, rdf::href(resource)));
	}
	else if (!nodeID.empty())
		metadata.push_back(rdf::statement(subject, predicate, rdf::bnode(nodeID)));
	else if (hasSubElements(node))
	{
		std::string parseType = node.attr(rdf::rdf("parseType")).content();

		if (parseType == "Resource")
		{
			const rdf::bnode temp = metadata.genid();
			parseRdfXmlInnerMetadata(node, temp, metadata, base);
			metadata.push_back(rdf::statement(subject, predicate, temp));
		}
		else if (parseType == "Collection")
		{
			const rdf::bnode first = parseRdfXmlCollectionMetadata(node.firstChild(), subject, metadata, base);
			metadata.push_back(rdf::statement(subject, predicate, first));
		}
		else
		{
			const rdf::bnode temp = metadata.genid();
			parseRdfXmlOuterMetadata(node, temp, metadata, base);
			metadata.push_back(rdf::statement(subject, predicate, temp));
		}
	}
	else
	{
		std::string lang = node.attr(rdf::xml("lang")).content();
		std::string value = node.content();

		if (!datatype.empty())
			metadata.push_back(rdf::statement(subject, predicate, rdf::literal(value, rdf::href(datatype))));
		else if (!value.empty())
			metadata.push_back(rdf::statement(subject, predicate, rdf::literal(value, lang)));
	}
}

void parseRdfXmlInnerMetadata(const xml::node &rdfxml, const rdf::resource &subject, rdf::model &metadata, const std::string &base)
{
	std::string lang;

	for (xml::attribute attr = rdfxml.firstAttribute(); attr.isValid(); attr.next())
	{
		if (attr != rdf::rdf("about") && attr != rdf::rdf("nodeID") && attr != rdf::rdf("parseType") && attr != rdf::rdf("ID"))
		{
			if (attr == rdf::xml("lang"))
				lang = attr.content();
			else
			{
				std::string value = attr.content();
				metadata.push_back(rdf::statement(subject, rdf::uri(attr.namespaceURI(), attr.name()), rdf::literal(value, lang)));
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

			parseRdfXmlMetadataFromNode(node, subject, rdf::rdf(ss.str()), metadata, base);
		}
		else
			parseRdfXmlMetadataFromNode(node, subject, predicate, metadata, base);
	}
}

void parseRdfXmlOuterMetadata(const xml::node &rdfxml, const rdf::resource &subject, rdf::model &metadata, const std::string &base)
{
	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			const cainteoir::rdf::bnode *bnode = rdf::any_type(&subject);
			if (bnode)
				parseRdfXmlInnerMetadata(node, subject, metadata, base);
			else
			{
				std::string about = node.attr(rdf::rdf("about")).content();
				std::string nodeID = node.attr(rdf::rdf("nodeID")).content();
				std::string ID = node.attr(rdf::rdf("ID")).content();
				if (!about.empty())
					parseRdfXmlMetadata(node, rdf::href((*about.begin()) == '#' ? base + about : about), metadata, base);
				else if (!nodeID.empty())
					parseRdfXmlMetadata(node, rdf::bnode(nodeID), metadata, base);
				else if (!ID.empty())
					parseRdfXmlMetadata(node, rdf::href(base + "#" + ID), metadata, base);
			}
		}
	}
}

void cainteoir::parseRdfXmlDocument(const xml::node &rdfxml, const rdf::uri &subject, rdf::model &metadata)
{
	if (rdfxml != rdf::rdf("RDF"))
		throw std::runtime_error("RDF/XML document is not of a recognised format.");

	parseRdfXmlOuterMetadata(rdfxml, subject, metadata, rdfxml.attr(rdf::xml("base")).content());
}
