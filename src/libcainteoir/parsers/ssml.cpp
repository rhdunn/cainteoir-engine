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

void cainteoir::parseSsmlDocument(const xml::node &ssml, const rdf::uri &subject, cainteoir::document_events &events)
{
	if (ssml != rdf::ssml("speak"))
		throw std::runtime_error(_("SSML document is not of a recognised format."));

	for (xml::attribute attr = ssml.firstAttribute(); attr.isValid(); attr.next())
	{
		if (attr == rdf::xml("lang"))
			events.metadata(rdf::statement(subject, rdf::dc("language"), rdf::literal(attr.content())));
	}

	for (xml::node node = ssml.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_TEXT_NODE)
		{
			events.text(node.content());
		}
	}
}
