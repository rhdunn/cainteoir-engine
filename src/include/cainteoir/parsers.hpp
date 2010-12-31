/* Document Parser API.
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

#ifndef CAINTEOIR_ENGINE_PARSERS_HPP
#define CAINTEOIR_ENGINE_PARSERS_HPP

#include <cainteoir/metadata.hpp>
#include <cainteoir/xml.hpp>

#include <map>

namespace cainteoir
{
	std::map<std::string, std::string> parseOcfDocument(const xmldom::node &aRoot);

	void parseOpfDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata);

	void parseRdfXmlDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata);

	void parseSmilDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata);
}

#endif
