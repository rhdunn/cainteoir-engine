/* SMIL Document Parser.
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

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xmldom;

bool have_dclanguage(const rdf::model &aMetadata, const rdf::uri &aDocument)
{
	foreach_iter(query, aMetadata)
	{
		if (rdf::query::subject(*query) == aDocument)
		{
			if (query->predicate == rdf::dc("language"))
				return true;
		}
	}

	return false;
}

void parseSmilMetadata(const xml::node &smil, const rdf::uri &subject, rdf::model &metadata)
{
	for (xml::node node = smil.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node == rdf::rdf("RDF"))
			cainteoir::parseRdfXmlDocument(node, subject, metadata);
	}
}

void parseSmilData(const xml::node &smil, const rdf::uri &subject, rdf::model &metadata)
{
	for (xml::node node = smil.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node.namespaceURI() == rdf::smil)
		{
			if (node == rdf::smil("metadata"))
				parseSmilMetadata(node, subject, metadata);
			else
				parseSmilData(node, subject, metadata);
		}
	}
}

void cainteoir::parseSmilDocument(const xml::node &smil, const rdf::uri &subject, rdf::model &metadata)
{
	if (smil != rdf::smil("smil"))
		throw std::runtime_error("SMIL document is not of a recognised format.");

	parseSmilData(smil, subject, metadata);

	if (!have_dclanguage(metadata, subject))
	{
		for (xml::attribute attr = smil.firstAttribute(); attr.isValid(); attr.next())
		{
			if (attr == rdf::xml("lang"))
				metadata.push_back(rdf::statement(subject, rdf::dc("language"), rdf::literal(attr.content())));
		}
	}
}
