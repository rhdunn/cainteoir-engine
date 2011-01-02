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

bool cainteoir::parseDocument(const char *aFilename, rdf::model &aMetadata, std::list<event> &aEvents)
{
	std::string type = cainteoir::mimetypes()(aFilename);
	if (type == "application/xml")
	{
		const rdf::uri subject = rdf::uri(aFilename, std::string());

		xml::document doc(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(aFilename)));
		xml::node root = doc.root();

		if (root == rdf::opf("package"))
		{
			cainteoir::opffiles files;
			cainteoir::parseOpfDocument(root, subject, aMetadata, files);
		}
		else if (root == rdf::rdf("RDF"))
			cainteoir::parseRdfXmlDocument(root, subject, aMetadata);
		else if (root == rdf::smil("smil"))
			cainteoir::parseSmilDocument(root, subject, aMetadata);
		else
			return false;
	}
	else if (type == "application/epub+zip")
		cainteoir::parseEpubDocument(aFilename, aMetadata);
	else if (type == "text/plain")
		aEvents.push_back(event(text_event, std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(aFilename))));
	else
		return false;

	return true;
}
