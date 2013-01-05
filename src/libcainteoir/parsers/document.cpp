/* Document Parser.
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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include <cainteoir/mimetype.hpp>
#include "parsers.hpp"
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace mime = cainteoir::mime;

std::shared_ptr<cainteoir::xml::reader>
cainteoir::createXmlReader(const std::shared_ptr<buffer> &aData, const char *aDefaultEncoding)
{
	if (!aData)
		return std::shared_ptr<xml::reader>();

	std::shared_ptr<xml::reader> reader = std::make_shared<xml::reader>(aData, aDefaultEncoding);
	while (reader->read() && reader->nodeType() != cainteoir::xml::reader::beginTagNode)
		;
	return reader;
}

/// @addtogroup document_reader
/// @{

/** @brief Create a document content reader.
  *
  * @param[in]  aData            The document content.
  * @param[in]  aSubject         The RDF subject for the document metadata.
  * @param[out] aPrimaryMetadata The main metadata that describes the document.
  * @param[in]  aTitle           The document title to use if none is specified.
  * @param[in]  aDefaultEncoding The default character encoding to use.
  *
  * @return A reader over the document contents, or a null pointer if the document is not supported.
  *
  * The top-level ToC entry is determined as follows (in order of preference):
  *    -  the title specified by the document;
  *    -  the title specified in aTitle;
  *    -  the filename of the document.
  */
std::shared_ptr<cainteoir::document_reader>
cainteoir::createDocumentReader(std::shared_ptr<buffer> &aData,
                                const rdf::uri &aSubject,
                                rdf::graph &aPrimaryMetadata,
                                const std::string &aTitle,
                                const char *aDefaultEncoding)
{
	if (!aData || aData->empty())
		return std::shared_ptr<document_reader>();

	if (mime::gzip.match(aData))
	{
		std::shared_ptr<cainteoir::buffer> decompressed = cainteoir::inflate_gzip(*aData, 0);
		return createDocumentReader(decompressed, aSubject, aPrimaryMetadata, aTitle);
	}

	if (mime::zip.match(aData))
	{
		auto archive = create_zip_archive(aData, aSubject);

		auto mimetype = archive->read("mimetype");
		if (mimetype)
		{
			if (mimetype->startswith(mime::epub.mime_type))
				return createEpubReader(archive, aSubject, aPrimaryMetadata, aDefaultEncoding);

			return std::shared_ptr<document_reader>();
		}

		return createZipReader(archive);
	}

	if (mime::xml.match(aData))
	{
		auto reader = cainteoir::createXmlReader(aData, aDefaultEncoding);
		std::string namespaceUri = reader->namespaceUri();
		std::string rootName     = reader->nodeName().str();

		if (mime::xhtml.match(namespaceUri, rootName))
		{
			auto mime = createMimeInHtmlReader(aData, aSubject, aPrimaryMetadata, aTitle, aDefaultEncoding);
			if (mime)
				return mime;
			return createHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle, "application/xhtml+xml");
		}

		if (mime::ncx.match(namespaceUri, rootName))
			return createNcxReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::opf.match(namespaceUri, rootName))
			return createOpfReader(reader, aSubject, aPrimaryMetadata);

		if (mime::rdfxml.match(namespaceUri, rootName))
			return createRdfXmlReader(reader, aSubject, aPrimaryMetadata);

		if (mime::ssml.match(namespaceUri, rootName))
			return createSsmlReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::smil.match(namespaceUri, rootName))
			return createSmilReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::html.match(aData))
		{
			auto mime = createMimeInHtmlReader(aData, aSubject, aPrimaryMetadata, aTitle, aDefaultEncoding);
			if (mime)
				return mime;
			return createHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle, "application/xhtml+xml");
		}

		return std::shared_ptr<document_reader>();
	}

	if (mime::email.match(aData) || mime::mime.match(aData))
		return createMimeReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::html.match(aData))
	{
		auto mime = createMimeInHtmlReader(aData, aSubject, aPrimaryMetadata, aTitle, aDefaultEncoding);
		if (mime)
			return mime;
		auto reader = cainteoir::createXmlReader(aData, aDefaultEncoding);
		return createHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle, "text/html");
	}

	if (mime::rtf.match(aData))
		return createRtfReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::pdf.match(aData))
		return createPdfReader(aData, aSubject, aPrimaryMetadata, aTitle);

	return createPlainTextReader(aData, aSubject, aPrimaryMetadata, aTitle);
}

/** @brief Create a document content reader.
  *
  * @param[in]  aFilename        The path to the document.
  * @param[out] aPrimaryMetadata The main metadata that describes the document.
  * @param[in]  aTitle           The document title to use if none is specified.
  * @param[in]  aDefaultEncoding The default character encoding to use.
  *
  * @return A reader over the document contents, or a null pointer if the document is not supported.
  *
  * If aFilename is null, the file content is read from stdin.
  *
  * The top-level ToC entry is determined as follows (in order of preference):
  *    -  the title specified by the document;
  *    -  the title specified in aTitle;
  *    -  the filename of the document.
  */
std::shared_ptr<cainteoir::document_reader>
cainteoir::createDocumentReader(const char *aFilename,
                                rdf::graph &aPrimaryMetadata,
                                const std::string &aTitle,
                                const char *aDefaultEncoding)
{
	const rdf::uri subject = rdf::uri(aFilename ? aFilename : "stdin", std::string());

	std::shared_ptr<cainteoir::buffer> data;
	if (aFilename)
		data = cainteoir::make_file_buffer(aFilename);
	else
		data = cainteoir::make_file_buffer(stdin);

	return createDocumentReader(data, subject, aPrimaryMetadata, aTitle, aDefaultEncoding);
}

/** @brief Get the document formats that are supported by libcainteoir.
  *
  * @param[out] metadata     The RDF graph to write the format support to.
  * @param[in]  capabilities The document capabilities to query for.
  */
void cainteoir::supportedDocumentFormats(rdf::graph &metadata, capability_types capabilities)
{
	std::string baseuri = "http://rhdunn.github.com/cainteoir/formats/document";

	if (capabilities & (cainteoir::metadata_support | cainteoir::text_support))
	{
		mime::epub .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::xhtml.metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::html .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::mhtml.metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::email.metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::rtf  .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::ssml .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::gzip .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::zip  .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
#ifdef HAVE_POPPLER
		mime::pdf  .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
#endif
	}

	if (capabilities & cainteoir::metadata_support)
	{
		mime::rdfxml.metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::opf   .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::ncx   .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
		mime::smil  .metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
	}

	if (capabilities & cainteoir::text_support)
	{
		mime::text.metadata(metadata, baseuri, rdf::tts("DocumentFormat"));
	}
}

/** @struct cainteoir::document
  * @brief  Stores the text from a document.
  */

/** @struct cainteoir::document_item
  * @brief Holds information about a part of a document.
  *
  * Objects of this type are created by instances of the document_reader interface.
  */

/// @}

/** @fn    cainteoir::document_item::document_item()
  * @brief Initialize the document item object.
  */

/** @var   uint32_t cainteoir::document_item::type
  * @brief The type of the event.
  *
  * @see cainteoir::events::event_type
  */

/** @var   const cainteoir::css::styles *cainteoir::document_item::styles
  * @brief The @ref css "CSS" styles associated with this document item.
  */

/** @var   std::shared_ptr<cainteoir::buffer> cainteoir::document_item::text
  * @brief The text associated with the document event.
  */

/** @var   cainteoir::rdf::uri cainteoir::document_item::anchor
  * @brief A uri that references the start of this event.
  */

/// @addtogroup document_reader
/// @{

/** @struct cainteoir::document_reader
  * @brief  Provides a reader-style API to the events corresponding to the document.
  */

/// @}

/** @fn    cainteoir::document_reader::~document_reader()
  * @brief Destroy the document reader object.
  */

/** @fn    bool cainteoir::document_reader::read()
  * @brief Read the next event in the document.
  *
  * @retval true  If an event was read.
  * @retval false If there are no more events in the document.
  */

/// @addtogroup document_reader
/// @{

/** @enum  cainteoir::events::event_type
  * @brief The type of the document event.
  */

/// @}

/** @var   cainteoir::events::event_type cainteoir::events::begin_context
  * @brief The start of a rendering context.
  *
  * @code
  *   context <context> <parameter>
  * @endcode
  */

/** @var   cainteoir::events::event_type cainteoir::events::end_context
  * @brief The end of a rendering context.
  *
  * @code
  *   end
  * @endcode
  */

/** @var   cainteoir::events::event_type cainteoir::events::text
  * @brief Text data.
  *
  * @code
  *   text <text>
  * @endcode
  */

/** @var   cainteoir::events::event_type cainteoir::events::toc_entry
  * @brief An entry in the table of contents.
  *
  * The parameter is the depth of the entry. This corresponds to the depth from the
  * heading rendering context.
  *
  * @code
  *   toc-entry <parameter> <anchor> <text>
  * @endcode
  */

/** @var   cainteoir::events::event_type cainteoir::events::anchor
  * @brief An anchor point in the document.
  *
  * The anchor corresponds to the associated toc-entry.
  *
  * @code
  *   anchor <anchor>
  * @endcode
  */

/// @addtogroup document_reader
/// @{

/** @enum  cainteoir::capability_types
  * @brief The capabilities provided by different document types.
  */

/// @}

/** @var   cainteoir::capability_types cainteoir::metadata_support
  * @brief The document type provides metadata information that can be extracted.
  */

/** @var   cainteoir::capability_types cainteoir::text_support
  * @brief The document type contains text that can be extracted.
  */
