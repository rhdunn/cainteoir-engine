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

#include <cainteoir/document.hpp>
#include "xml.hpp"
#include <list>
#include <map>

namespace cainteoir
{
	struct fileinfo
	{
		std::string filename; /**< @brief The name of the file. */
		std::string mimetype; /**< @brief The mime type of the file */

		fileinfo(const std::string &aFileName, const std::string &aMimeType)
			: filename(aFileName)
			, mimetype(aMimeType)
		{
		}

		fileinfo()
		{
		}
	};

	struct opffiles
	{
		fileinfo toc;
		std::list<fileinfo> spine;
	};

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
	void parseOpfDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata, opffiles &aOpfFiles);

	/** @brief XML encoded HTML (XHTML)
	  * @see   http://www.w3.org/TR/xhtml1/
	  *
	  * @param aRoot     The root node of the XHTML XML document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param aMetadata The RDF model to add any metadata to.
	  * @param aEvents   The speech events that make up the XHTML document.
	  */
	void parseXHtmlDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, rdf::model &aMetadata, std::list<cainteoir::event> &aEvents);

	/** @brief RDF/XML
	  * @see   http://www.w3.org/TR/2004/REC-rdf-syntax-grammar-20040210/
	  *
	  * @param aRoot     The root node of the RDF/XML document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param aMetadata The RDF model to add any metadata to.
	  *
	  * Conformance/Implementation Notes:
	  *
	  *     -  xml:lang attributes preserve case, so xml:lang="en-US" resolves to "en-US", not "en-us".
	  *
	  *        This is so that the language attributes conform with their ISO specifications for the
	  *        5-letter language codes.
	  *
	  *     -  RDF literals contents is normalised so that it does not contain spaces at the beginning
	  *        and end, and spaces in the middle only has one space for any given run of spaces.
	  *
	  *        This is to normalise the representation of values that are written to span multiple lines
	  *        in the RDF/XML file.
	  *
	  *     -  rdf:parseType="Collection" on http://www.w3.org/TR/REC-rdf-syntax/#example19 generates
	  *        the same triples, but in a different order.
	  *
	  *        This is due to the algorithm used to process the collection nodes.
	  *
	  *     -  Parsing the skos:member rdfs:range (S32) property does not handle the types on the generated
	  *        node, nor the items in its owl:unionOf relation.
	  *
	  *        This is a bug/limitation of the existing RDF/XML parser.
	  *
	  * The following examples in http://www.w3.org/TR/REC-rdf-syntax/ are not currently parsed correctly:
	  *     -  example08.rdf
	  *     -  example09.rdf
	  *     -  example13.rdf
	  *     -  example20.rdf
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
	  * @param aEvents   The speech events that make up the ePub document.
	  */
	void parseEpubDocument(const char *aFilename, rdf::model &aMetadata, std::list<event> &aEvents);
}

#endif
