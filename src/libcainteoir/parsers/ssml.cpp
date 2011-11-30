/* SSML Document Parser.
 *
 * Copyright (C) 2011 Reece H. Dunn
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

void parseSsmlContext(const xml::node &ssml,
                      const rdf::uri &subject,
                      cainteoir::document_events &events,
                      cainteoir::document_events::context context,
                      int32_t parameter = 0)
{
	events.begin_context(context, parameter);
	for (xml::node node = ssml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_TEXT_NODE)
		{
			std::tr1::shared_ptr<cainteoir::buffer> text = node.content(false);
			if (!text->empty())
				events.text(text);
		}
		else if (node.type() == XML_ELEMENT_NODE && node.namespaceURI() == rdf::ssml)
		{
			if (node == rdf::ssml("s"))
				parseSsmlContext(node, subject, events, cainteoir::document_events::sentence);
			else if (node == rdf::ssml("emphasis"))
			{
				int32_t style = cainteoir::document_events::emphasized;
				for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
				{
					if (!strcmp(attr.name(), "level"))
					{
						std::string value = attr.content();
						if (value == "strong")
							style = cainteoir::document_events::strong;
						else if (value == "reduced")
							style = cainteoir::document_events::reduced;
						else if (value == "none")
							style = cainteoir::document_events::nostyle;
					}
				}
				parseSsmlContext(node, subject, events, cainteoir::document_events::span, style);
			}
		}
	}
	events.end_context();
}

void cainteoir::parseSsmlDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	xmldom::document doc(aData);
	xmldom::node ssml = doc.root();

	if (ssml != rdf::ssml("speak"))
		throw std::runtime_error(_("SSML document is not of a recognised format."));

	for (xml::attribute attr = ssml.firstAttribute(); attr.isValid(); attr.next())
	{
		if (attr == rdf::xml("lang"))
			aGraph.statement(aSubject, rdf::dc("language"), rdf::literal(attr.content()));
	}

	for (xml::node node = ssml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_TEXT_NODE)
		{
			std::tr1::shared_ptr<cainteoir::buffer> text = node.content(false);
			if (!text->empty())
				events.text(text);
		}
		else if (node.type() == XML_ELEMENT_NODE && node.namespaceURI() == rdf::ssml)
		{
			if (node == rdf::ssml("meta"))
			{
				std::string name;
				std::string content;
				for (xml::attribute attr = node.firstAttribute(); attr.isValid(); attr.next())
				{
					if (!strcmp(attr.name(), "name"))
						name = attr.content();
					else if (!strcmp(attr.name(), "content"))
						content = attr.content();
				}

				if (name == "seeAlso" && !content.empty())
					aGraph.statement(aSubject, rdf::rdfs("seeAlso"), aGraph.href(content));
			}
			else if (node == rdf::ssml("p"))
				parseSsmlContext(node, aSubject, events, cainteoir::document_events::paragraph);
		}
	}
}
