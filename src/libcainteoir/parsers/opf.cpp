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

#define OPF_NS "http://www.idpf.org/2007/opf"

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xml;

void parseOpfMetadata(xml::node &opf, const rdf::uri &subject, rdf::model &metadata)
{
	for (xml::node node = opf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && !strcmp(node.namespaceURI(), "http://purl.org/dc/elements/1.1/"))
		{
			std::string lang;

			for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
			{
				if (!strcmp(attr.namespaceURI(), "http://www.w3.org/XML/1998/namespace") && !strcmp(attr.name(), "lang"))
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
		if (section.type() == XML_ELEMENT_NODE && !strcmp(section.namespaceURI(), OPF_NS))
		{
			if (!strcmp(section.name(), "metadata"))
				parseOpfMetadata(section, subject, metadata);
		}
	}
}

