/* Document Parser API.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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
		std::string mimetype; /**< @brief The mime type of the file. */
		std::string id;       /**< @brief The unique id for the file within the current document. */

		fileinfo(const std::string &aFileName, const std::string &aMimeType, const std::string &aId)
			: filename(aFileName)
			, mimetype(aMimeType)
			, id(aId)
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

	/** @brief Copy the data in buffer to a memory buffer.
	  *
	  * @param data The data buffer to be copied.
	  * @return     The new data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> strm_copy(const cainteoir::buffer &data);

	/** @brief Inflate a zlib compressed data buffer.
	  *
	  * @param data The compressed data buffer.
	  * @param uncompressed The size of the uncompressed data buffer.
	  * @return The uncompressed data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> strm_inflate(const cainteoir::buffer &data, uint32_t uncompressed);

	/** @brief Decompress a gzip buffer.
	  *
	  * @param data The gzip compressed data.
	  * @return     The uncompressed data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> strm_gzip_decompress(const cainteoir::buffer &data);

	/** @brief Open Container Format (OCF)
	  * @see   http://www.idpf.org/ocf/ocf1.0/download/ocf10.htm
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aRoot The root node of the OCF XML document.
	  * @return      The list of (media-type => full-path) entries.
	  */
	std::map<std::string, std::string> parseOcfDocument(const xmldom::node &aRoot);

	/** @brief Navigation Control File (NCX)
	  * @see   http://www.niso.org/workrooms/daisy/Z39-86-2005.html#NCX
	  *
	  * @param aRoot     The root node of the NCX XML document.
	  * @param aSubject  The subject to use for any metadata.
	  * @param events    The events callback to handle document events.
	  */
	void parseNcxDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, document_events &events);

	/** @brief Open Publication Format (OPF)
	  * @see   http://www.idpf.org/2007/opf/opf2.0/download/
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aRoot     The root node of the OPF XML document.
	  * @param aSubject  The subject to use for any Dublin Core metadata.
	  * @param events    The events callback to handle document events.
	  */
	void parseOpfDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, document_events &events, opffiles &aOpfFiles);

	/** @brief XML encoded HTML (XHTML)
	  * @see   http://www.w3.org/TR/xhtml1/
	  *
	  * @param aData     The document content.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param events    The events callback to handle document events.
	  */
	void parseXHtmlDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events);

	/** @brief RDF/XML
	  * @see   http://www.w3.org/TR/2004/REC-rdf-syntax-grammar-20040210/
	  *
	  * @param aRoot     The root node of the RDF/XML document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param events    The events callback to handle document events.
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
	void parseRdfXmlDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, document_events &events);

	/** @brief Synchronized Multimedia Integration Language (SMIL)
	  * @see   http://www.w3.org/TR/2008/REC-SMIL3-20081201/
	  *
	  * @param aRoot     The root node of the SMIL XML document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param events    The events callback to handle document events.
	  */
	void parseSmilDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, document_events &events);

	/** @brief Speech Synthesis Markup Language (SSML)
	  * @see   http://www.w3.org/TR/speech-synthesis/
	  *
	  * @param aRoot     The root node of the SSML XML document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param events    The events callback to handle document events.
	  */
	void parseSsmlDocument(const xmldom::node &aRoot, const rdf::uri &aSubject, document_events &events);

	/** @brief ePub
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aData  The ePub document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param events The events callback to handle document events.
	  */
	void parseEpubDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events);

	/** @brief RTF
	  *
	  * @param aData  The RichText document.
	  * @param aSubject  The base to use for any relative URIs.
	  * @param events The events callback to handle document events.
	  */
	void parseRtfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, document_events &events);
}

#endif
