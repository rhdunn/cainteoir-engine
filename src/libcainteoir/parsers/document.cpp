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

#include <cainteoir/mimetype.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/platform.hpp>
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

struct ParseXmlDocument
{
	const mime::mimetype *mimetype;
	decltype(cainteoir::parseNcxDocument) *parser;
};

// magic = xml ; match namespaceUri and rootName on XML document ...
static const ParseXmlDocument xml_handlers[] = {
	{ &mime::ncx,    &cainteoir::parseNcxDocument },
	{ &mime::opf,    &cainteoir::parseOpfDocument },
	{ &mime::rdfxml, &cainteoir::parseRdfXmlDocument },
	{ &mime::smil,   &cainteoir::parseSmilDocument },
	{ &mime::ssml,   &cainteoir::parseSsmlDocument },
	{ &mime::xhtml,  &cainteoir::parseXHtmlDocument },
	{ &mime::html,   &cainteoir::parseXHtmlDocument },
};

struct ParseDocument
{
	const mime::mimetype *mimetype;
	decltype(cainteoir::parseEpubDocument) *parser;
};

// magic = document specific ...
static const ParseDocument doc_handlers[] = {
	{ &mime::epub, &cainteoir::parseEpubDocument },
	{ &mime::html, &cainteoir::parseHtmlDocument },
	{ &mime::rtf,  &cainteoir::parseRtfDocument },
};

#define countof(a) (sizeof(a)/sizeof(a[0]))

std::tr1::shared_ptr<cainteoir::buffer> buffer_from_stdin()
{
	cainteoir::rope data;
	char buffer[1024];

	size_t read;
	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) != 0)
	{
		std::tr1::shared_ptr<cainteoir::buffer> fiber(new cainteoir::data_buffer(read));
		memcpy((void *)fiber->begin(), buffer, read);
		data += fiber;
	}

	return data.buffer();
}

inline bool is_mime_header_char(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-';
}

struct mime_headers : public cainteoir::buffer
{
	std::tr1::shared_ptr<cainteoir::buffer> mOriginal;
	std::string encoding;
	std::string mimetype;

	bool parse_headers(const rdf::uri &subject, cainteoir::document_events &events, rdf::graph &aGraph, cainteoir::buffer &boundary)
	{
		while (first <= last)
		{
			cainteoir::buffer name(first, first);
			cainteoir::buffer value(first, first);

			while (first <= last && is_mime_header_char(*first))
				++first;

			name = cainteoir::buffer(name.begin(), first);
			if (name.empty())
			{
				if (*first == '\r' || *first == '\n')
				{
					++first;
					if (*first == '\n')
						++first;
					return true;
				}
				return false;
			}

			if (first[0] == ':' && first[1] == ' ')
			{
				const char * start = first;
				while (first <= last && !(first[0] == '\n' && first[1] != ' ' && first[1] != '\t'))
					++first;

				value = cainteoir::buffer(start + 2, *(first-1) == '\r' ? first-1 : first);
				++first;
			}
			else
				return false;

			if (!name.comparei("Content-Transfer-Encoding"))
			{
				const char * type = value.begin();
				while (type <= value.end() && !(*type == ';' || *type == '\n'))
					++type;
				encoding = std::string(value.begin(), *(type-1) == '\r' ? type-1 : type);
			}
			else if (!name.comparei("Content-Type"))
			{
				const char * type = value.begin();
				while (type <= value.end() && !(*type == ';' || *type == '\n'))
					++type;
				mimetype = std::string(value.begin(), type);

				if (mimetype == "multipart/mixed" || mimetype == "multipart/related")
				{
					++type;
					while (type <= value.end() && (*type == ' ' || *type == '\t'))
						++type;

					const char * name = type;
					while (type <= value.end() && *type != '=')
						++type;

					cainteoir::buffer arg(name, type);
					++type;

					if (*type != '"') continue;
					++type;

					const char * bounds = type;
					while (type <= value.end() && *type != '"')
						++type;
					boundary = cainteoir::buffer(bounds, type);
				}
			}
			else if (!name.comparei("Subject"))
				aGraph.statement(subject, rdf::dc("title"), rdf::literal(value.str()));
			else if (!name.comparei("From"))
			{
				// name ...

				const char * name_begin = value.begin();
				const char * name_end = value.begin();

				while (name_end <= value.end() && *name_end == ' ')
					++name_end;

				while (name_end <= value.end() && *name_end != '<')
					++name_end;

				// email address

				const char * mbox_begin = name_end + 1;
				const char * mbox_end = value.end();

				while (mbox_end > mbox_begin && *mbox_end != '>')
					--mbox_end;

				// clean-up name ...

				--name_end;
				while (name_end > value.begin() && *name_end == ' ')
					--name_end;
				++name_end;

				// metadata ...

				const rdf::uri from = aGraph.genid();
				aGraph.statement(subject, rdf::dc("creator"), from);
				aGraph.statement(from, rdf::rdf("type"), rdf::foaf("Person"));
				aGraph.statement(from, rdf::rdf("value"), rdf::literal(std::string(name_begin, name_end)));
				aGraph.statement(from, rdf::foaf("mbox"), rdf::literal("mailto:" + std::string(mbox_begin, mbox_end)));
			}
		}

		return false;
	}

	mime_headers(std::tr1::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, cainteoir::document_events &events, rdf::graph &aGraph)
		: cainteoir::buffer(*data)
		, mOriginal(data)
		, encoding("8bit")
	{
		while (first <= last && (*first == ' ' || *first == '\t' || *first == '\r' || *first == '\n'))
			++first;

		if (!strncmp(first, "HTTP/1.0 ", 9) || !strncmp(first, "HTTP/1.1 ", 9))
		{
			while (first <= last && *first != '\n')
				++first;
			++first;
		}

		cainteoir::buffer boundary(NULL, NULL);
		if (!parse_headers(subject, events, aGraph, boundary))
			first = mOriginal->begin();
		else if (!boundary.empty())
		{
			const char * begin = NULL;

			while (first <= last)
			{
				if (first[0] == '-' && first[1] == '-' && !strncmp(first + 2, boundary.begin(), boundary.size()))
				{
					if (begin == NULL)
					{
						first += 2;
						first += boundary.size();
						begin = first;
					}
					else
					{
						last = first;
						first = begin;
						return;
					}
				}
				++first;
			}
		}
	}
};

bool parseDocumentBuffer(std::tr1::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, cainteoir::document_events &events, rdf::graph &aGraph, bool includeMimetypeMetadata)
{
	// Encoded documents ...

	for (const DecodeDocument *decode = decode_handlers; decode != decode_handlers + countof(decode_handlers); ++decode)
	{
		if (decode->mimetype->match(data))
		{
			std::tr1::shared_ptr<cainteoir::buffer> decompressed = decode->decoder(*data, 0);
			return parseDocumentBuffer(decompressed, subject, events, aGraph, includeMimetypeMetadata);
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

		for (const ParseXmlDocument *xml = xml_handlers; xml != xml_handlers + countof(xml_handlers); ++xml)
		{
			if (xml->mimetype->match(namespaceUri, rootName))
			{
				xml->parser(reader, subject, events, aGraph);
				if (includeMimetypeMetadata)
					aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal(xml->mimetype->mime_type));
				return true;
			}
		}
		return false;
	}

	// Documents with MIME headers ...

	if (mime::email.match(data) || mime::mime.match(data))
	{
		std::tr1::shared_ptr<mime_headers> mime(new mime_headers(data, subject, events, aGraph));

		std::tr1::shared_ptr<cainteoir::buffer> decoded;
		if (mime->encoding == "quoted-printable")
			decoded = cainteoir::decode_quoted_printable(*mime, 0);
		else if (mime->encoding == "base64")
			decoded = cainteoir::decode_base64(*mime, 0);
		else if (mime->encoding == "7bit" || mime->encoding == "7BIT")
			decoded = mime;
		else if (mime->encoding == "8bit" || mime->encoding == "8BIT")
			decoded = mime;
		else if (mime->encoding == "binary")
			decoded = mime;
		else
			throw std::runtime_error(_("unsupported content-transfer-encoding"));

		if (mime->begin() != data->begin()) // Avoid an infinite loop when there is just the mime header.
			return parseDocumentBuffer(decoded, subject, events, aGraph, includeMimetypeMetadata);
	}

	// Other documents ...

	for (const ParseDocument *parse = doc_handlers; parse != doc_handlers + countof(doc_handlers); ++parse)
	{
		if (parse->mimetype->match(data))
		{
			parse->parser(data, subject, events, aGraph);
			if (includeMimetypeMetadata)
				aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal(parse->mimetype->mime_type));
			return true;
		}
	}

	// Octet Stream ...

	const char *begin = data->begin();
	const char *end   = data->end();
	if (begin + 101 < end) // only check the first 100 bytes
		end = begin + 101;

	while (begin < end)
	{
		if (*begin < 0x20 && !(*begin == '\r' || *begin == '\n' || *begin == '\t'))
			return false; // looks like an octet/binary stream
		++begin;
	}

	// Plain Text ...

	events.text(data);
	if (includeMimetypeMetadata)
		aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal("text/plain"));
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

	std::tr1::shared_ptr<cainteoir::buffer> data;
	if (aFilename)
		data = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(aFilename));
	else
		data = buffer_from_stdin();

	return parseDocumentBuffer(data, subject, events, aGraph, true);
}
