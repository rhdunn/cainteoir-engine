/* Document Parser.
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

#include <cainteoir/document.hpp>
#include "parsers.hpp"
#include "mimetypes.hpp"

namespace xml = cainteoir::xmldom;
namespace rdf = cainteoir::rdf;

struct rdfgraph_builder : public cainteoir::document_events
{
	rdfgraph_builder(rdf::graph &aMetadata) : m_graph(aMetadata)
	{
	}

	void metadata(const rdf::statement &aStatement)
	{
		m_graph.push_back(aStatement);
	}

	const rdf::bnode genid()
	{
		return m_graph.genid();
	}
private:
	rdf::graph &m_graph;
};

bool cainteoir::parseDocument(const char *aFilename, rdf::graph &aMetadata, std::list<event> &aEvents)
{
	rdfgraph_builder events(aMetadata);

	std::string type = cainteoir::mimetypes()(aFilename);
	if (type == "application/xml")
	{
		const rdf::uri subject = rdf::uri(aFilename, std::string());

		xml::document doc(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(aFilename)));
		xml::node root = doc.root();

		if (root == rdf::opf("package"))
		{
			cainteoir::opffiles files;
			cainteoir::parseOpfDocument(root, subject, events, files);
		}
		else if (root == rdf::xhtml("html"))
			cainteoir::parseXHtmlDocument(root, subject, events, aEvents);
		else if (root == rdf::rdf("RDF"))
			cainteoir::parseRdfXmlDocument(root, subject, events);
		else if (root == rdf::smil("smil"))
			cainteoir::parseSmilDocument(root, subject, events);
		else
			return false;
	}
	else if (type == "application/epub+zip")
		cainteoir::parseEpubDocument(aFilename, events, aEvents);
	else if (type == "text/plain")
		aEvents.push_back(event(text_event, std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(aFilename))));
	else
		return false;

	return true;
}
