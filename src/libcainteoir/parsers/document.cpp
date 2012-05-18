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
#include <cainteoir/document.hpp>
#include "parsers.hpp"
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace mime = cainteoir::mime;

std::shared_ptr<cainteoir::buffer> buffer_from_stdin()
{
	cainteoir::rope data;
	char buffer[1024];

	size_t read;
	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) != 0)
	{
		std::shared_ptr<cainteoir::buffer> fiber = std::make_shared<cainteoir::data_buffer>(read);
		memcpy((void *)fiber->begin(), buffer, read);
		data += fiber;
	}

	return data.buffer();
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createDocumentReader(std::shared_ptr<buffer> &aData,
                                const rdf::uri &aSubject,
                                rdf::graph &aPrimaryMetadata,
                                const std::string &aTitle)
{
	if (!aData || aData->empty())
		return std::shared_ptr<document_reader>();

	if (mime::gzip.match(aData))
	{
		std::shared_ptr<cainteoir::buffer> decompressed = cainteoir::inflate_gzip(*aData, 0);
		return createDocumentReader(decompressed, aSubject, aPrimaryMetadata, aTitle);
	}

	if (mime::epub.match(aData))
	{
		auto archive = create_zip_archive(aData, aSubject);
		return createEpubReader(archive, aSubject, aPrimaryMetadata);
	}

	if (mime::xml.match(aData))
	{
		cainteoir::xml::reader reader(aData);
		while (reader.read() && reader.nodeType() != cainteoir::xml::reader::beginTagNode)
			;

		std::string namespaceUri = reader.namespaceUri();
		std::string rootName     = reader.nodeName().str();

		if (mime::xhtml.match(namespaceUri, rootName))
			return createHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle, "application/xhtml+xml");

		if (mime::ncx.match(namespaceUri, rootName))
			return createNcxReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::opf.match(namespaceUri, rootName))
			return createOpfReader(reader, aSubject, aPrimaryMetadata);

		if (mime::rdfxml.match(namespaceUri, rootName))
			return createRdfXmlReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::ssml.match(namespaceUri, rootName))
			return createSsmlReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::smil.match(namespaceUri, rootName))
			return createSmilReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::html.match(aData))
			return createHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle, "application/xhtml+xml");

		return std::shared_ptr<document_reader>();
	}

	if (mime::html.match(aData))
	{
		cainteoir::xml::reader reader(aData);
		while (reader.read() && reader.nodeType() != cainteoir::xml::reader::beginTagNode)
			;

		return createHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle, "text/html");
	}

	if (mime::rtf.match(aData))
		return createRtfReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::email.match(aData) || mime::mime.match(aData))
		return createMimeReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::pdf.match(aData))
		return createPdfReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::zip.match(aData))
	{
		auto archive = create_zip_archive(aData, aSubject);
		return createZipReader(archive);
	}

	return createPlainTextReader(aData, aSubject, aPrimaryMetadata, aTitle);
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createDocumentReader(const char *aFilename,
                                rdf::graph &aPrimaryMetadata,
                                const std::string &aTitle)
{
	const rdf::uri subject = rdf::uri(aFilename ? aFilename : "stdin", std::string());

	std::shared_ptr<cainteoir::buffer> data;
	if (aFilename)
		data = std::make_shared<cainteoir::mmap_buffer>(aFilename);
	else
		data = buffer_from_stdin();

	return createDocumentReader(data, subject, aPrimaryMetadata, aTitle);
}

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
