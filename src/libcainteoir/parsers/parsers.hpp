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

namespace cainteoir
{
	/** @brief Electronic Publication (ePub)
	  * @private
	  *
	  * @param[in]  aData            The document multi-document container.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aDefaultEncoding The default character encoding to use.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createEpubReader(std::shared_ptr<archive> &aData,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const char *aDefaultEncoding);

	/** @brief (XML-based) HyperText Markup Language ((X)HTML)
	  * @private
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  * @param[in]  aMimeType        The mimetype to use in the metadata (defaults to the OPF mimetype).
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createHtmlReader(const std::shared_ptr<xml::reader> &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle,
	                 const char *aMimeType);

	/** @brief Miltipurpose Internet Mail Extensions (MIME)
	  * @private
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

	/** @brief MIME Document Embedded Within a HTML Document
	  * @private
	  *
	  * This creates a reader for a mime document that is embedded within a HTML
	  * document that has the following events:
	  *
	  * @code
	  *     toc-entry [...] depth=0 title="""..."""
	  *     begin-context paragraph +monospace
	  *     text(...): """
	  *     EMBEDDED MIME DOCUMENT
	  *     """
	  *     end-context paragraph +monospace
	  * @endcode
	  *
	  * Where the |end-context| is optional.
	  *
	  * @param[in]  aData            The document content.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createMimeInHtmlReader(std::shared_ptr<buffer> &aData,
	                       const rdf::uri &aSubject,
	                       rdf::graph &aPrimaryMetadata,
	                       const std::string &aTitle,
                               const char *aDefaultEncoding);

	/** @brief Navigation Control File (NCX)
	  * @private
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createNcxReader(const std::shared_ptr<xml::reader> &aReader,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const std::string &aTitle);

	/** @brief Open Container Format (OCF)
	  * @private
	  *
	  * @param[in]  aReader          The XML document reader.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createOcfReader(const std::shared_ptr<xml::reader> &aReader);

	/** @brief Open Package Format (OPF)
	  * @private
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aMimeType        The mimetype to use in the metadata (defaults to the OPF mimetype).
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createOpfReader(const std::shared_ptr<xml::reader> &aReader,
	                const rdf::uri &aSubject,
	                rdf::graph &aPrimaryMetadata,
	                const char *aMimeType = "application/oebps-package+xml");

	/** @brief Portable Document Format (PDF)
	  * @private
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
	  * @private
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
	  * @private
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createRdfXmlReader(const std::shared_ptr<xml::reader> &aReader,
	                   const rdf::uri &aSubject,
	                   rdf::graph &aPrimaryMetadata);

	/** @brief Rich Text Format (RTF)
	  * @private
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
	  * @private
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createSmilReader(const std::shared_ptr<xml::reader> &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	/** @brief Speech Synthesis Markup Language (SSML)
	  * @private
	  *
	  * @param[in]  aReader          The XML document reader.
	  * @param[in]  aSubject         The RDF subject for the document metadata.
	  * @param[out] aPrimaryMetadata The main metadata that describes the document.
	  * @param[in]  aTitle           The document title to use if none is specified.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createSsmlReader(const std::shared_ptr<xml::reader> &aReader,
	                 const rdf::uri &aSubject,
	                 rdf::graph &aPrimaryMetadata,
	                 const std::string &aTitle);

	/** @brief eXtensible Markup Language (XML)
	  * @private
	  *
	  * @param[in]  aData            The document multi-document container.
	  * @param[in]  aDefaultEncoding The default character encoding to use.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<xml::reader>
	createXmlReader(const std::shared_ptr<buffer> &aData,
	                const char *aDefaultEncoding);

	/** @brief ZIP archive
	  * @private
	  *
	  * @param[in]  aData            The document multi-document container.
	  *
	  * @return A reader over the document contents.
	  */
	std::shared_ptr<document_reader>
	createZipReader(std::shared_ptr<archive> &aData);

	/** @brief Parse a MIME header and body
	  * @private
	  */
	std::pair<bool, std::shared_ptr<buffer>>
	parseMimeHeaders(std::shared_ptr<buffer> &aData,
                         const rdf::uri &aSubject,
                         rdf::graph &aPrimaryMetadata,
                         std::string &aTitle);
}

#endif
