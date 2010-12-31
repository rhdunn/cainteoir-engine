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
	/** @brief Open Container Format (OCF)
	  * @see   http://www.idpf.org/ocf/ocf1.0/download/ocf10.htm
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aRoot The root node of the OCF XML document.
	  * @return      The list of (media-type => full-path) entries.
	  */
	std::map<std::string, std::string> parseOcfDocument(const xmldom::node &aRoot);

	/** @brief Open Publication Format (OPF)
	  * @see   http://www.idpf.org/2007/opf/opf2.0/download/
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aRoot     The root node of the OPF XML document.
	  * @param aSubject  The subject to use for any Dublin Core metadata.
	  * @param aMetadata The RDF model to add any metadata to.
	  */
	void parseOpfDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata);

	/** @brief RDF/XML
	  * @see   http://www.w3.org/TR/2004/REC-rdf-syntax-grammar-20040210/
	  *
	  * @param aRoot     The root node of the RDF/XML document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param aMetadata The RDF model to add any metadata to.
	  */
	void parseRdfXmlDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata);

	/** @brief Synchronized Multimedia Integration Language (SMIL)
	  * @see   http://www.w3.org/TR/2008/REC-SMIL3-20081201/
	  *
	  * @param aRoot     The root node of the SMIL XML document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param aMetadata The RDF model to add any metadata to.
	  */
	void parseSmilDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata);

	/** @brief ePub
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aFilename The path to the ePub document.
	  * @param aMetadata The RDF model to add any metadata to.
	  */
	void parseEpubDocument(const char *aFilename, rdf::model &aMetadata);
}

#endif
