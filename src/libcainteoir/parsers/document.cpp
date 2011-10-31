/* Document Parser.
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

#include <cainteoir/document.hpp>
#include <cainteoir/platform.hpp>
#include "parsers.hpp"
#include "mimetypes.hpp"

namespace xmldom = cainteoir::xmldom;
namespace rdf = cainteoir::rdf;

struct DecodeDocument
{
	const char *mimetype;
	cainteoir::decoder_ptr decoder;
};

static const DecodeDocument decode_handlers[] = {
	{ "application/x-gzip", &cainteoir::inflate_gzip },
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

	bool parse_headers(std::string &mimetype, const rdf::uri &subject, cainteoir::document_events &events, rdf::graph &aGraph, cainteoir::buffer &boundary)
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

	mime_headers(std::tr1::shared_ptr<cainteoir::buffer> &data, std::string &mimetype, const rdf::uri &subject, cainteoir::document_events &events, rdf::graph &aGraph)
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
		if (!parse_headers(mimetype, subject, events, aGraph, boundary))
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

bool parseDocumentBuffer(std::tr1::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, cainteoir::document_events &events, std::string type, rdf::graph &aGraph)
{
	// Encoded documents ...

	for (const DecodeDocument *decode = decode_handlers; decode != decode_handlers + countof(decode_handlers); ++decode)
	{
		if (type == decode->mimetype)
		{
			std::tr1::shared_ptr<cainteoir::buffer> decompressed = decode->decoder(*data, 0);
			type = cainteoir::mimetypes()(decompressed);
			return parseDocumentBuffer(decompressed, subject, events, type, aGraph);
		}
	}

	// Other documents ...

	if (type == "application/xml")
	{
		xmldom::document doc(data);
		xmldom::node root = doc.root();

		if (root == rdf::opf("package"))
		{
			cainteoir::opffiles files;
			cainteoir::parseOpfDocument(root, subject, events, aGraph, files);
		}
		else if (root == rdf::xhtml("html") || root == rdf::uri(std::string(), "html"))
			parseXHtmlDocument(data, subject, events, aGraph);
		else if (root == rdf::rdf("RDF"))
			cainteoir::parseRdfXmlDocument(root, subject, events, aGraph);
		else if (root == rdf::smil("smil"))
			cainteoir::parseSmilDocument(root, subject, events, aGraph);
		else if (root == rdf::ssml("speak"))
			cainteoir::parseSsmlDocument(root, subject, events, aGraph);
		else if (root == rdf::ncx("ncx"))
			cainteoir::parseNcxDocument(root, subject, events, aGraph);
		else
			return false;
	}
	else if (type == "application/epub+zip")
		cainteoir::parseEpubDocument(data, subject, events, aGraph);
	else if (type == "application/octet-stream")
		return false;
	else if (type == "text/rtf")
		cainteoir::parseRtfDocument(data, subject, events, aGraph);
	else
	{
		std::tr1::shared_ptr<mime_headers> mime(new mime_headers(data, type, subject, events, aGraph));
		if (mime->begin() == data->begin())
			parseXHtmlDocument(data, subject, events, aGraph);
		else
		{
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

			return parseDocumentBuffer(decoded, subject, events, type, aGraph);
		}
	}

	aGraph.statement(subject, rdf::tts("mimetype"), rdf::literal(type));
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

	std::string type = cainteoir::mimetypes()(data);
	return parseDocumentBuffer(data, subject, events, type, aGraph);
}
