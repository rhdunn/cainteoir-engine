/* Document Parser API.
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

#ifndef CAINTEOIR_ENGINE_PARSERS_HPP
#define CAINTEOIR_ENGINE_PARSERS_HPP

#include <cainteoir/document.hpp>
#include <cainteoir/archive.hpp>
#include <list>
#include <map>

enum parser_flags
{
	no_document_flags = 0,
	include_document_mimetype = 1,
	needs_document_title = 2,
};

bool parseDocumentBuffer(std::shared_ptr<cainteoir::buffer> &data,
                         const cainteoir::rdf::uri &subject,
                         cainteoir::document_events &events,
                         cainteoir::rdf::graph &aGraph,
                         parser_flags flags);

namespace cainteoir
{
	/** @brief Open Publication Format (OPF)
	  * @see   http://www.idpf.org/2007/opf/opf2.0/download/
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param reader   The XML document reader.
	  * @param aSubject The subject to use for any Dublin Core metadata.
	  * @param events   The events callback to handle document events.
	  */
	void parseOpfDocument(xml::reader &reader,
	                      const rdf::uri &aSubject,
	                      document_events &events,
	                      rdf::graph &aGraph);

	/** @brief ePub
	  * @see   http://www.idpf.org/specs.htm
	  *
	  * @param aData    The document data.
	  * @param aSubject The base to use for any relative URIs.
	  * @param events   The events callback to handle document events.
	  */
	void parseEpubDocument(std::shared_ptr<archive> aData,
	                       const rdf::uri &aSubject,
	                       document_events &events,
	                       rdf::graph &aGraph);

	/** @name Document Readers
	  */
	//@{

	/** @brief HyperText Markup Language (HTML)
	  *
	  * @param[in]  aData            The document content.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createHtmlReader(std::shared_ptr<buffer> &aData,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	/** @brief HyperText Markup Language (HTML)
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createHtmlReader(xml::reader &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	/** @brief Miltipurpose Internet Mail Extensions (MIME)
	  *
	  * @param[in]  aData            The document content.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createMimeReader(std::shared_ptr<buffer> &aData,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	/** @brief Navigation Control File (NCX)
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createNcxReader(xml::reader &aReader,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle);

	/** @brief Open Container Format (OCF)
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createOcfReader(xml::reader &aReader,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle);

	/** @brief Portable Document Format (PDF)
	  *
	  * @param[in]  aData            The document content.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createPdfReader(std::shared_ptr<buffer> &aData,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle);

	/** @brief Plain Text
	  *
	  * @param[in]  aData            The document content.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createPlainTextReader(std::shared_ptr<buffer> &aData,
	                      const rdf::uri &aSubject,
	                      rdf::graph &aPrimaryMetadata,
	                      const std::string &aTitle);

	/** @brief RDF/XML
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createRdfXmlReader(xml::reader &aReader,
	                   const rdf::uri &aSubject,
	                   rdf::graph &aPrimaryMetadata,
	                   const std::string &aTitle);

	/** @brief Rich Text Format (RTF)
	  *
	  * @param[in]  aData            The document content.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createRtfReader(std::shared_ptr<buffer> &aData,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle);

	/** @brief Synchronized Multimedia Integration Language (SMIL)
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createSmilReader(xml::reader &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	/** @brief Speech Synthesis Markup Language (SSML)
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createSsmlReader(xml::reader &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	/** @brief XML-based HyperText Markup Language (XHTML)
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createXHtmlReader(xml::reader &aReader,
	                  const rdf::uri &aSubject,
	                  rdf::graph &aPrimaryMetadata,
	                  const std::string &aTitle);

	//@}
}

#endif
