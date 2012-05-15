/* SMIL Document Parser.
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

namespace smil
{
	namespace events = cainteoir::events;

	static const xml::context::entry smil_node = { events::unknown, 0 };

	static const xml::context::entry lang_attr = { events::unknown, 0 };
}

static const std::initializer_list<const xml::context::entry_ref> smil_nodes =
{
	{ "smil", &smil::smil_node },
};

static const std::initializer_list<const xml::context::entry_ref> smil_attrs =
{
	{ "lang", &smil::lang_attr },
};

void cainteoir::parseSmilDocument(xml::reader &reader, const rdf::uri &aSubject, document_events &events, rdf::graph &aGraph)
{
	reader.set_nodes(xmlns::smil, smil_nodes);
	reader.set_attrs(xmlns::smil, smil_attrs);

	const xml::context::entry *current = reader.context();

	while (reader.read()) switch (reader.nodeType())
	{
	case xml::reader::attribute:
		if (current == &smil::smil_node)
		{
			if (reader.context() == &smil::lang_attr)
				aGraph.statement(aSubject, rdf::dc("language"), rdf::literal(reader.nodeValue().str()));
		}
		break;
	case xml::reader::beginTagNode:
		current = reader.context();
		break;
	}
}
