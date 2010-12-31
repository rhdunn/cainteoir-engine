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

void parseRdfXmlMetadata(const xml::node &rdfxml, const rdf::uri &subject, rdf::model &metadata)
{
	std::string lang = rdfxml.attr(rdf::xml("lang")).content();

	for (xml::attribute attr = rdfxml.firstAttribute(); attr.isValid(); attr.next())
	{
		if (attr != rdf::rdf("about"))
		{
			std::string value = attr.content();
			metadata.push_back(rdf::statement(subject, rdf::uri(attr.namespaceURI(), attr.name()), rdf::literal(value, lang)));
		}
	}

	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() != XML_ELEMENT_NODE)
			continue;

		std::string lang = node.attr(rdf::xml("lang")).content();
		std::string value = node.content();

		std::string resource = node.attr(rdf::rdf("resource")).content();

		const rdf::uri predicate = rdf::uri(node.namespaceURI(), node.name());

		if (!resource.empty())
			metadata.push_back(rdf::statement(subject, predicate, rdf::href(resource)));
		else if (!value.empty())
			metadata.push_back(rdf::statement(subject, predicate, rdf::literal(value, lang)));
	}
}

void cainteoir::parseRdfXmlDocument(const xml::node &rdfxml, const rdf::uri &subject, rdf::model &metadata)
{
	if (rdfxml != rdf::rdf("RDF"))
		throw std::runtime_error("RDF/XML document is not of a recognised format.");

	for (xml::node node = rdfxml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			const rdf::uri about = rdf::href(node.attr(rdf::rdf("about")).content());
			if (node != rdf::rdf("Description"))
				metadata.push_back(rdf::statement(about, rdf::rdf("type"), rdf::uri(node.namespaceURI(), node.name())));

			parseRdfXmlMetadata(node, about, metadata);
		}
	}
}
