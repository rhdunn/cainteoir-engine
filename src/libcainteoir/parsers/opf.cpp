/* OPF Document Parser.
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

void parseOpfMetadata(xml::node &opf, const rdf::uri &subject, rdf::model &metadata)
{
	for (xml::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node.namespaceURI() == rdf::dc)
		{
			std::string lang;

			for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (attr == rdf::xml("lang"))
					lang = attr.content().c_str();
			}

			rdf::uri predicate = rdf::uri(node.namespaceURI(), node.name());
			rdf::literal value = rdf::literal(node.content().c_str(), lang);
			metadata.push_back(rdf::statement(subject, predicate, value));
		}
	}
}

void cainteoir::parseOpfDocument(xml::node &opf, const rdf::uri &subject, rdf::model &metadata)
{
	for (xml::node section = opf.firstChild(); section.isValid(); section.next())
	{
		if (section.type() == XML_ELEMENT_NODE)
		{
			if (section == rdf::opf("metadata"))
				parseOpfMetadata(section, subject, metadata);
		}
	}
}

