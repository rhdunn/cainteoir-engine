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

std::tr1::shared_ptr<cainteoir::buffer> buffer_from_stdin()
{
	cainteoir::rope data;
	char buffer[1024];

	size_t read;
	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) != 0)
	{
		std::tr1::shared_ptr<cainteoir::buffer> fiber(new cainteoir::data_buffer(read));
		memcpy((void *)fiber->begin(), buffer, read);
		data.add(fiber);
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

	bool parse_headers(std::string &mimetype, const rdf::uri &subject, cainteoir::document_events &events, cainteoir::buffer &boundary)
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

			if (!name.comparei("Content-Type"))
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
				events.metadata(rdf::statement(subject, rdf::dc("title"), rdf::literal(value.str())));
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

				const rdf::bnode from = events.genid();
				events.metadata(rdf::statement(subject, rdf::dc("creator"), from));
				events.metadata(rdf::statement(from, rdf::rdf("type"), rdf::foaf("Person")));
				events.metadata(rdf::statement(from, rdf::rdf("value"), rdf::literal(std::string(name_begin, name_end))));
				events.metadata(rdf::statement(from, rdf::foaf("mbox"), rdf::literal("mailto:" + std::string(mbox_begin, mbox_end))));
			}
		}

		return false;
	}

	mime_headers(std::tr1::shared_ptr<cainteoir::buffer> &data, std::string &mimetype, const rdf::uri &subject, cainteoir::document_events &events)
		: cainteoir::buffer(*data)
		, mOriginal(data)
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
		if (!parse_headers(mimetype, subject, events, boundary))
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

bool parseDocumentBufferWithMimeType(std::tr1::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, cainteoir::document_events &events, std::string type)
{
	if (type == "application/xml")
	{
		xmldom::document doc(data);
		xmldom::node root = doc.root();

		if (root == rdf::opf("package"))
		{
			cainteoir::opffiles files;
			cainteoir::parseOpfDocument(root, subject, events, files);
		}
		else if (root == rdf::xhtml("html"))
			parseXHtmlDocument(data, subject, events);
		else if (root == rdf::rdf("RDF"))
			cainteoir::parseRdfXmlDocument(root, subject, events);
		else if (root == rdf::smil("smil"))
			cainteoir::parseSmilDocument(root, subject, events);
		else
			return false;
	}
	else if (type == "application/epub+zip")
		cainteoir::parseEpubDocument(data, subject, events);
	else if (type == "application/x-gzip")
	{
		std::tr1::shared_ptr<cainteoir::buffer> decompressed = cainteoir::strm_gzip_decompress(*data);
		type = cainteoir::mimetypes()(decompressed);
		return parseDocumentBufferWithMimeType(decompressed, subject, events, type);
	}
	else if (type == "application/octet-stream")
		return false;
	else if (type == "text/rtf")
		cainteoir::parseRtfDocument(data, subject, events);
	else
	{
		std::tr1::shared_ptr<cainteoir::buffer> content(new mime_headers(data, type, subject, events));
		if (content->begin() == data->begin())
			parseXHtmlDocument(data, subject, events);
		else
			return parseDocumentBufferWithMimeType(content, subject, events, type);
	}

	events.metadata(rdf::statement(subject, rdf::tts("mimetype"), rdf::literal(type)));
	return true;
}

bool parseDocumentBuffer(std::tr1::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, cainteoir::document_events &events)
{
	std::string type = cainteoir::mimetypes()(data);
	return parseDocumentBufferWithMimeType(data, subject, events, type);
}

void cainteoir::supportedDocumentFormats(rdf::graph &metadata)
{
	std::string baseuri = "http://rhdunn.github.com/cainteoir/formats/document";

	// only lists filetypes that are properly supported, excluding intermedate/internal formats ...

	rdf::uri text = rdf::uri(baseuri, "text");
	metadata.push_back(rdf::statement(text, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(text, rdf::tts("name"), rdf::literal("text")));
	metadata.push_back(rdf::statement(text, rdf::dc("title"), rdf::literal(_("text document"))));
	metadata.push_back(rdf::statement(text, rdf::dc("description"), rdf::literal(_("plain text document"))));
	metadata.push_back(rdf::statement(text, rdf::tts("mimetype"), rdf::literal("text/plain")));
	metadata.push_back(rdf::statement(text, rdf::tts("extension"), rdf::literal("*.txt")));

	rdf::uri html = rdf::uri(baseuri, "html");
	metadata.push_back(rdf::statement(html, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(html, rdf::tts("name"), rdf::literal("html")));
	metadata.push_back(rdf::statement(html, rdf::dc("title"), rdf::literal(_("(x)html document"))));
	metadata.push_back(rdf::statement(html, rdf::dc("description"), rdf::literal(_("(x)html document"))));
	metadata.push_back(rdf::statement(html, rdf::tts("mimetype"), rdf::literal("text/html")));
	metadata.push_back(rdf::statement(html, rdf::tts("mimetype"), rdf::literal("application/xhtml+xml")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.htm")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.html")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.xhtml")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.xht")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.xml")));

	rdf::uri epub = rdf::uri(baseuri, "epub");
	metadata.push_back(rdf::statement(epub, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(epub, rdf::tts("name"), rdf::literal("epub")));
	metadata.push_back(rdf::statement(epub, rdf::dc("title"), rdf::literal(_("epub document"))));
	metadata.push_back(rdf::statement(epub, rdf::dc("description"), rdf::literal(_("electronic publication (ePub) document"))));
	metadata.push_back(rdf::statement(epub, rdf::tts("mimetype"), rdf::literal("application/epub+zip")));
	metadata.push_back(rdf::statement(epub, rdf::tts("extension"), rdf::literal("*.epub")));

	rdf::uri email = rdf::uri(baseuri, "email");
	metadata.push_back(rdf::statement(email, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(email, rdf::tts("name"), rdf::literal("email")));
	metadata.push_back(rdf::statement(email, rdf::dc("title"), rdf::literal(_("email document"))));
	metadata.push_back(rdf::statement(email, rdf::dc("description"), rdf::literal(_("electronic mail document (raw mbox format)"))));
	metadata.push_back(rdf::statement(email, rdf::tts("mimetype"), rdf::literal("text/x-mail")));
	metadata.push_back(rdf::statement(email, rdf::tts("mimetype"), rdf::literal("message/rfc822")));
	metadata.push_back(rdf::statement(email, rdf::tts("extension"), rdf::literal("*.eml")));
	metadata.push_back(rdf::statement(email, rdf::tts("extension"), rdf::literal("*.emlx")));
	metadata.push_back(rdf::statement(email, rdf::tts("extension"), rdf::literal("*.msg")));
	metadata.push_back(rdf::statement(email, rdf::tts("extension"), rdf::literal("*.mbx")));

	rdf::uri gzip = rdf::uri(baseuri, "gzip");
	metadata.push_back(rdf::statement(gzip, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(gzip, rdf::tts("name"), rdf::literal("gzip")));
	metadata.push_back(rdf::statement(gzip, rdf::dc("title"), rdf::literal(_("gzip compressed document"))));
	metadata.push_back(rdf::statement(gzip, rdf::dc("description"), rdf::literal(_("gzip compressed document"))));
	metadata.push_back(rdf::statement(gzip, rdf::tts("mimetype"), rdf::literal("application/x-gzip")));
	metadata.push_back(rdf::statement(gzip, rdf::tts("extension"), rdf::literal("*.gz")));

	rdf::uri rtf = rdf::uri(baseuri, "rtf");
	metadata.push_back(rdf::statement(rtf, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(rtf, rdf::tts("name"), rdf::literal("rtf")));
	metadata.push_back(rdf::statement(rtf, rdf::dc("title"), rdf::literal(_("rich text document"))));
	metadata.push_back(rdf::statement(rtf, rdf::dc("description"), rdf::literal(_("rich text document"))));
	metadata.push_back(rdf::statement(rtf, rdf::tts("mimetype"), rdf::literal("text/rtf")));
	metadata.push_back(rdf::statement(rtf, rdf::tts("mimetype"), rdf::literal("application/rtf")));
	metadata.push_back(rdf::statement(rtf, rdf::tts("extension"), rdf::literal("*.rtf")));
}

bool cainteoir::parseDocument(const char *aFilename, cainteoir::document_events &events)
{
	const rdf::uri subject = rdf::uri(aFilename ? aFilename : "stdin", std::string());

	std::tr1::shared_ptr<cainteoir::buffer> data;
	if (aFilename)
		data = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(aFilename));
	else
		data = buffer_from_stdin();

	return parseDocumentBuffer(data, subject, events);
}
