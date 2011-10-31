/* Document Parser.
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

#include <cainteoir/mimetype.hpp>
#include <cainteoir/document.hpp>
#include <cainteoir/platform.hpp>
#include "parsers.hpp"

namespace xmldom = cainteoir::xmldom;
namespace rdf = cainteoir::rdf;
namespace mime = cainteoir::mime;

/** @brief Mime Data
  *
  * This will be refactored to load the mime data from the shared-mime-info database.
  */
namespace cainteoir { namespace mime
{
	const matchlet email_pattern1[] = { { 0, 0, 6, "From: " } };
	const matchlet email_pattern2[] = { { 0, 0, 9, "Subject: " } };
	const magic    email_magic[] = { { 1, email_pattern1 }, { 1, email_pattern2 } };
	const mimetype email = { "email", "message/rfc822", 2, email_magic, NULL, NULL };

	const matchlet epub_pattern[] = { { 0, 0, 2, "PK" }, { 30, 0, 8, "mimetype" }, { 38, 0, 20, "application/epub+zip" } };
	const magic    epub_magic[] = { { 3, epub_pattern } };
	const mimetype epub = { "epub", "application/epub+zip", 1, epub_magic, NULL, NULL };

	const matchlet gzip_pattern[] = { { 0, 0, 2, "\037\213" } };
	const magic    gzip_magic[] = { { 1, gzip_pattern } };
	const mimetype gzip = { "gzip", "application/x-gzip", 1, gzip_magic, NULL, NULL };

	const matchlet html_pattern1[] = { { 0, 0, 5, "<html" } };
	const matchlet html_pattern2[] = { { 0, 0, 5, "<HTML" } };
	const matchlet html_pattern3[] = { { 0, 0, 4, "<!--" } };
	const magic    html_magic[] = { { 1, html_pattern1 }, { 1, html_pattern2 }, { 1, html_pattern3 } };
	const mimetype html = { "html", "text/html", 3, html_magic, NULL, "html" };

	const matchlet http_pattern1[] = { { 0, 0, 8, "HTTP/1.0" } };
	const matchlet http_pattern2[] = { { 0, 0, 8, "HTTP/1.1" } };
	const magic    http_magic[] = { { 1, http_pattern1 }, { 1, http_pattern2 } };
	const mimetype http = { "http", NULL, 2, http_magic, NULL, NULL };

	const matchlet mime_pattern1[] = { { 0,  4, 14, "Content-Type: " } }; // for multipart mime documents (e.g. mhtml)
	const matchlet mime_pattern2[] = { { 0, 80, 17, "MIME-Version: 1.0" }, { 18, 80, 14, "Content-Type: " } };
	const magic    mime_magic[] = { { 1, mime_pattern1 }, { 2, mime_pattern2 } };
	const mimetype mime = { "mime", NULL, 2, mime_magic, NULL, NULL };

	const mimetype ncx = { "ncx", "application/x-dtbncx+xml", 0, NULL, "http://www.daisy.org/z3986/2005/ncx/", "ncx" };

	const mimetype opf = { "opf", "application/oebps-package+xml", 0, NULL, "http://www.idpf.org/2007/opf", "package" };

	const mimetype rdfml = { "rdf", "application/rdf+xml", 0, NULL, "http://www.w3.org/1999/02/22-rdf-syntax-ns", "RDF" };

	const matchlet rtf_pattern[] = { { 0, 0, 5, "{\\rtf" } };
	const magic    rtf_magic[] = { { 1, rtf_pattern } };
	const mimetype rtf = { "rtf", "application/rtf", 1, rtf_magic, NULL, NULL };

	const mimetype smil = { "smil", "application/smil", 0, NULL, "http://www.w3.org/ns/SMIL", "smil" };

	const mimetype ssml = { "ssml", "application/ssml+xml", 0, NULL, "http://www.w3.org/2001/10/synthesis", "speak" };

	const mimetype xhtml = { "xhtml", "application/html+xml", 0, NULL, "http://www.w3.org/1999/xhtml", "html" };

	const matchlet xml_pattern[] = { { 0, 0, 14, "<?xml version=" } };
	const magic    xml_magic[] = { { 1, xml_pattern } };
	const mimetype xml = { "xml", "application/xml", 1, xml_magic, NULL, NULL };
}}

struct DecodeDocument
{
	const mime::mimetype *mimetype;
	cainteoir::decoder_ptr decoder;
};

static const DecodeDocument decode_handlers[] = {
	{ &mime::gzip, &cainteoir::inflate_gzip },
};

typedef void (*parsedoc_ptr)(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph);

struct ParseDocument
{
	const mime::mimetype *mimetype;
	parsedoc_ptr parser;
};

static const ParseDocument doc_handlers[] = {
	{ &mime::epub, &cainteoir::parseEpubDocument },
	{ &mime::html, &cainteoir::parseXHtmlDocument },
	{ &mime::rtf,  &cainteoir::parseRtfDocument },
};

typedef void (*parsexml_ptr)(const xmldom::node &aRoot, const rdf::uri &aSubject, cainteoir::document_events &events, rdf::graph &aGraph);

struct XmlDocument
{
	const mime::mimetype *mimetype;
	parsexml_ptr parser;
};

static const XmlDocument xml_handlers[] = {
	{ &mime::ncx,   &cainteoir::parseNcxDocument },
	{ &mime::opf,   NULL }, // FIXME: Align the OPF parser with the rest of the XML parsers.
	{ &mime::rdfml, &cainteoir::parseRdfXmlDocument },
	{ &mime::smil,  &cainteoir::parseSmilDocument },
	{ &mime::ssml,  &cainteoir::parseSsmlDocument },
	{ &mime::xhtml, NULL }, // FIXME: Align the (X)HTML parser with the rest of the XML parsers.
	{ &mime::html,  NULL }, // FIXME: Align the (X)HTML parser with the rest of the XML parsers.
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

bool parseDocumentBuffer(std::tr1::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, cainteoir::document_events &events, rdf::graph &aGraph)
{
	// Encoded documents ...

	for (const DecodeDocument *decode = decode_handlers; decode != decode_handlers + countof(decode_handlers); ++decode)
	{
		if (decode->mimetype->match(data))
		{
			std::tr1::shared_ptr<cainteoir::buffer> decompressed = decode->decoder(*data, 0);
			return parseDocumentBuffer(decompressed, subject, events, aGraph);
		}
	}

	// XML documents ...

	if (mime::xml.match(data))
	{
		xmldom::document doc(data);
		xmldom::node root = doc.root();

		for (const XmlDocument *xml = xml_handlers; xml != xml_handlers + countof(xml_handlers); ++xml)
		{
			if (xml->mimetype->match(root.namespaceURI(), root.name()))
			{
				if (!xml->parser) // FIXME: Temporary cludge for parsers that don't follow the xml parser signature ...
				{
					if (root == rdf::opf("package"))
					{
						cainteoir::opffiles files;
						cainteoir::parseOpfDocument(root, subject, events, aGraph, files);
					}
					else if (root == rdf::xhtml("html") || root == rdf::uri(std::string(), "html"))
						parseXHtmlDocument(data, subject, events, aGraph);
				}
				else
					xml->parser(root, subject, events, aGraph);
				aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal(xml->mimetype->mimetype));
				return true;
			}
		}
		return false;
	}

	// Documents with MIME headers ...

	if (mime::email.match(data) || mime::mime.match(data) || mime::http.match(data))
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
			return parseDocumentBuffer(decoded, subject, events, aGraph);
	}

	// Other documents ...

	for (const ParseDocument *parse = doc_handlers; parse != doc_handlers + countof(doc_handlers); ++parse)
	{
		if (parse->mimetype->match(data))
		{
			parse->parser(data, subject, events, aGraph);
			aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal(parse->mimetype->mimetype));
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
	aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal("text/plain"));
	return true;
}

void cainteoir::supportedDocumentFormats(rdf::graph &metadata)
{
	std::string baseuri = "http://rhdunn.github.com/cainteoir/formats/document";

	// only lists filetypes that are properly supported, excluding intermedate/internal formats ...

	rdf::uri text = rdf::uri(baseuri, "text");
	metadata.statement(text, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(text, rdf::tts("name"), rdf::literal("text"));
	metadata.statement(text, rdf::dc("title"), rdf::literal(_("text document")));
	metadata.statement(text, rdf::dc("description"), rdf::literal(_("plain text document")));
	metadata.statement(text, rdf::tts("mimetype"), rdf::literal("text/plain"));
	metadata.statement(text, rdf::tts("extension"), rdf::literal("*.txt"));

	rdf::uri html = rdf::uri(baseuri, "html");
	metadata.statement(html, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(html, rdf::tts("name"), rdf::literal("html"));
	metadata.statement(html, rdf::dc("title"), rdf::literal(_("(x)html document")));
	metadata.statement(html, rdf::dc("description"), rdf::literal(_("(x)html document")));
	metadata.statement(html, rdf::tts("mimetype"), rdf::literal("text/html"));
	metadata.statement(html, rdf::tts("mimetype"), rdf::literal("application/xhtml+xml"));
	metadata.statement(html, rdf::tts("extension"), rdf::literal("*.htm"));
	metadata.statement(html, rdf::tts("extension"), rdf::literal("*.html"));
	metadata.statement(html, rdf::tts("extension"), rdf::literal("*.xhtml"));
	metadata.statement(html, rdf::tts("extension"), rdf::literal("*.xht"));
	metadata.statement(html, rdf::tts("extension"), rdf::literal("*.xml"));

	rdf::uri mhtml = rdf::uri(baseuri, "mhtml");
	metadata.statement(mhtml, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(mhtml, rdf::tts("name"), rdf::literal("mhtml"));
	metadata.statement(mhtml, rdf::dc("title"), rdf::literal(_("mhtml document")));
	metadata.statement(mhtml, rdf::dc("description"), rdf::literal(_("single-file HTML document")));
	metadata.statement(mhtml, rdf::tts("mimetype"), rdf::literal("multipart/related"));
	metadata.statement(mhtml, rdf::tts("extension"), rdf::literal("*.mht"));

	rdf::uri epub = rdf::uri(baseuri, "epub");
	metadata.statement(epub, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(epub, rdf::tts("name"), rdf::literal("epub"));
	metadata.statement(epub, rdf::dc("title"), rdf::literal(_("epub document")));
	metadata.statement(epub, rdf::dc("description"), rdf::literal(_("electronic publication (ePub) document")));
	metadata.statement(epub, rdf::tts("mimetype"), rdf::literal("application/epub+zip"));
	metadata.statement(epub, rdf::tts("extension"), rdf::literal("*.epub"));

	rdf::uri email = rdf::uri(baseuri, "email");
	metadata.statement(email, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(email, rdf::tts("name"), rdf::literal("email"));
	metadata.statement(email, rdf::dc("title"), rdf::literal(_("email document")));
	metadata.statement(email, rdf::dc("description"), rdf::literal(_("electronic mail document (raw mbox format)")));
	metadata.statement(email, rdf::tts("mimetype"), rdf::literal("text/x-mail"));
	metadata.statement(email, rdf::tts("mimetype"), rdf::literal("message/rfc822"));
	metadata.statement(email, rdf::tts("extension"), rdf::literal("*.eml"));
	metadata.statement(email, rdf::tts("extension"), rdf::literal("*.emlx"));
	metadata.statement(email, rdf::tts("extension"), rdf::literal("*.msg"));
	metadata.statement(email, rdf::tts("extension"), rdf::literal("*.mbx"));

	rdf::uri gzip = rdf::uri(baseuri, "gzip");
	metadata.statement(gzip, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(gzip, rdf::tts("name"), rdf::literal("gzip"));
	metadata.statement(gzip, rdf::dc("title"), rdf::literal(_("gzip compressed document")));
	metadata.statement(gzip, rdf::dc("description"), rdf::literal(_("gzip compressed document")));
	metadata.statement(gzip, rdf::tts("mimetype"), rdf::literal("application/x-gzip"));
	metadata.statement(gzip, rdf::tts("extension"), rdf::literal("*.gz"));

	rdf::uri rtf = rdf::uri(baseuri, "rtf");
	metadata.statement(rtf, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(rtf, rdf::tts("name"), rdf::literal("rtf"));
	metadata.statement(rtf, rdf::dc("title"), rdf::literal(_("rich text document")));
	metadata.statement(rtf, rdf::dc("description"), rdf::literal(_("rich text document")));
	metadata.statement(rtf, rdf::tts("mimetype"), rdf::literal("text/rtf"));
	metadata.statement(rtf, rdf::tts("mimetype"), rdf::literal("application/rtf"));
	metadata.statement(rtf, rdf::tts("extension"), rdf::literal("*.rtf"));

	rdf::uri ssml = rdf::uri(baseuri, "ssml");
	metadata.statement(ssml, rdf::rdf("type"), rdf::tts("DocumentFormat"));
	metadata.statement(ssml, rdf::tts("name"), rdf::literal("ssml"));
	metadata.statement(ssml, rdf::dc("title"), rdf::literal(_("speech synthesis markup document")));
	metadata.statement(ssml, rdf::dc("description"), rdf::literal(_("speech synthesis markup document")));
	metadata.statement(ssml, rdf::tts("mimetype"), rdf::literal("application/ssml+xml"));
	metadata.statement(ssml, rdf::tts("extension"), rdf::literal("*.ssml"));
}

bool cainteoir::parseDocument(const char *aFilename, cainteoir::document_events &events, rdf::graph &aGraph)
{
	const rdf::uri subject = rdf::uri(aFilename ? aFilename : "stdin", std::string());

	std::tr1::shared_ptr<cainteoir::buffer> data;
	if (aFilename)
		data = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(aFilename));
	else
		data = buffer_from_stdin();

	return parseDocumentBuffer(data, subject, events, aGraph);
}
