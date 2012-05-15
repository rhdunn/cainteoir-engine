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

struct DecodeDocument
{
	const mime::mimetype *mimetype;
	cainteoir::decoder_ptr decoder;
};

static const DecodeDocument decode_handlers[] = {
	{ &mime::gzip, &cainteoir::inflate_gzip },
};

struct ParseZipDocument
{
	const mime::mimetype *mimetype;
	decltype(cainteoir::parseEpubDocument) *parser;
};

// magic = uncompressed mimetype file ...
static const ParseZipDocument zip_handlers[] = {
	{ &mime::epub, &cainteoir::parseEpubDocument },
};

struct ParseXmlDocument
{
	const mime::mimetype *mimetype;
	decltype(cainteoir::parseNcxDocument) *parser;
};

// magic = xml ; match namespaceUri and rootName on XML document ...
static const ParseXmlDocument xml_handlers[] = {
	{ &mime::ncx,    &cainteoir::parseNcxDocument },
	{ &mime::opf,    &cainteoir::parseOpfDocument },
	{ &mime::ssml,   &cainteoir::parseSsmlDocument },
};

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

	if (mime::xml.match(aData))
	{
		cainteoir::xml::reader reader(aData);
		while (reader.read() && reader.nodeType() != cainteoir::xml::reader::beginTagNode)
			;

		std::string namespaceUri = reader.namespaceUri();
		std::string rootName     = reader.nodeName().str();

		if (mime::xhtml.match(namespaceUri, rootName))
			return createXHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::rdfxml.match(namespaceUri, rootName))
			return createRdfXmlReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::smil.match(namespaceUri, rootName))
			return createSmilReader(reader, aSubject, aPrimaryMetadata, aTitle);

		if (mime::html.match(aData))
			return createXHtmlReader(reader, aSubject, aPrimaryMetadata, aTitle);

		return std::shared_ptr<document_reader>();
	}

	if (mime::html.match(aData))
		return createHtmlReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::rtf.match(aData))
		return createRtfReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::email.match(aData) || mime::mime.match(aData))
		return createMimeReader(aData, aSubject, aPrimaryMetadata, aTitle);

	if (mime::pdf.match(aData))
		return createPdfReader(aData, aSubject, aPrimaryMetadata, aTitle);

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

bool parseDocumentBuffer(std::shared_ptr<cainteoir::buffer> &data,
                         const rdf::uri &subject,
                         cainteoir::document_events &events,
                         rdf::graph &aGraph,
                         parser_flags flags)
{
	// Encoded documents ...

	for (const DecodeDocument *decode = std::begin(decode_handlers); decode != std::end(decode_handlers); ++decode)
	{
		if (decode->mimetype->match(data))
		{
			std::shared_ptr<cainteoir::buffer> decompressed = decode->decoder(*data, 0);
			return parseDocumentBuffer(decompressed, subject, events, aGraph, flags);
		}
	}

	// XML documents ...

	if (mime::xml.match(data))
	{
		cainteoir::xml::reader reader(data);
		while (reader.read() && reader.nodeType() != cainteoir::xml::reader::beginTagNode)
			;

		std::string namespaceUri = reader.namespaceUri();
		std::string rootName     = reader.nodeName().str();

		for (const ParseXmlDocument *xml = std::begin(xml_handlers); xml != std::end(xml_handlers); ++xml)
		{
			if (xml->mimetype->match(namespaceUri, rootName))
			{
				xml->parser(reader, subject, events, aGraph);
				if ((flags & include_document_mimetype) == include_document_mimetype)
					aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal(xml->mimetype->mime_type));
				return true;
			}
		}
	}

	// Zip/Compressed documents ...

	if (mime::zip.match(data))
	{
		auto archive = create_zip_archive(data, subject);

		for (const ParseZipDocument *parse = std::begin(zip_handlers); parse != std::end(zip_handlers); ++parse)
		{
			if (parse->mimetype->match(data))
			{
				parse->parser(archive, subject, events, aGraph);
				if ((flags & include_document_mimetype) == include_document_mimetype)
					aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal(parse->mimetype->mime_type));
				return true;
			}
		}

		bool parsed = false;
		const auto &files = archive->files();
		for (auto file = files.begin(), last = files.end(); file != last; ++file)
		{
			auto buffer   = archive->read(file->c_str());
			auto location = archive->location(*file, std::string());
			parsed |= parseDocumentBuffer(buffer, location, events, aGraph, needs_document_title);
		}

		return parsed;
	}

	// Reader-based document parsers (new-style) ...

	auto reader = createDocumentReader(data, subject, aGraph);
	if (!reader) return false;

	while (reader->read())
	{
		if ((flags & needs_document_title) == needs_document_title)
		{
			if (reader->type & cainteoir::events::toc_entry)
				events.toc_entry((int)reader->parameter, reader->anchor, reader->text->str());
			if (reader->type & cainteoir::events::anchor)
				events.anchor(reader->anchor, std::string());
		}
		if (reader->type & cainteoir::events::begin_context)
			events.begin_context(reader->context, reader->parameter);
		if (reader->type & cainteoir::events::text)
			events.text(reader->text);
		if (reader->type & cainteoir::events::end_context)
			events.end_context();
	}

	return true;
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

bool cainteoir::parseDocument(const char *aFilename, cainteoir::document_events &events, rdf::graph &aGraph)
{
	const rdf::uri subject = rdf::uri(aFilename ? aFilename : "stdin", std::string());

	std::shared_ptr<cainteoir::buffer> data;
	if (aFilename)
		data = std::make_shared<cainteoir::mmap_buffer>(aFilename);
	else
		data = buffer_from_stdin();

	return parseDocumentBuffer(data, subject, events, aGraph,
	                           parser_flags(include_document_mimetype|needs_document_title));
}
