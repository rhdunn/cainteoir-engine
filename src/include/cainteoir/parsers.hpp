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
	/** @name  Document Parsers
	  * @brief An API for reading supported documents.
	  *
	  * The following document formats require adding parsers to support them:
	  *    -  Atom:    extract syndication information from Atom feeds.
	  *    -  DocBook: extract information from DocBook 4.0 (SGML) and DocBook 5.0 (XML) documents.
	  *    -  (X)HTML: extract information from HTML4, HTML5 and XHTML documents (including meta+DC and RDF/a metadata).
	  *    -  NCX:     extract Table of Content information from NCX documents (needed for complete ePub support).
	  *    -  ODT:     extract information from ODF/text documents.
	  *    -  PLS:     extract pronunciation information from Pronunciation Lexicon Specification documents.
	  *    -  RSS:     extract syndication information from RSS 1.0 (RSS/RDF/XML) and 2.0 (RSS/XML) feeds.
	  *    -  RTF:     extract information from Rich Text Format documents.
	  *    -  SSML:    extract information from Speech Synthesis Markup Language documents.
	  *    -  TXT:     extract information from plain text documents.
	  *
	  * The following document formats will not be supported:
	  *    -  DOC:     Microsoft Word document format.
	  *    -  DOCX:    Microsoft Word document format (XML-based).
	  *
	  * The following existing parsers need work to be complete:
	  *    -  OPF:     extract the information in the manifest and spine sections.
	  *    -  RDF/XML: complete the parser so that it is to spec (use test cases from the spec?).
	  *    -  SMIL:    extract text information from the SMIL document.
	  *    -  ePub:    extract text information from the ePub document (requires complete OPF, NCX and (X)HTML parsers).
	  */
	//@{

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
	  */
	void parseEpubDocument(const char *aFilename, rdf::model &aMetadata);

	/** @brief Any supported document type.
	  *
	  * This is the main API to use to extract information from a document.
	  *
	  * @param aFilename The path to the ePub document.
	  * @param aMetadata The RDF model to add any metadata to.
	  *
	  * @retval true  If aFilename contains a supported document format.
	  * @retval false If aFilename contains an unsupported document format.
	  */
	bool parseDocument(const char *aFilename, rdf::model &aMetadata);

	//@}
}

#endif
