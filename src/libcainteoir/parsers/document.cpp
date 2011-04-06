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

bool parseDocumentBuffer(std::tr1::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, cainteoir::document_events &events)
{
	std::string type = cainteoir::mimetypes()(data);

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
		return parseDocumentBuffer(decompressed, subject, events);
	}
	else if (type == "application/octet-stream")
		return false;
	else
		parseXHtmlDocument(data, subject, events);

	return true;
}

void cainteoir::supportedDocumentFormats(rdf::graph &metadata)
{
	std::string baseuri = "http://rhdunn.github.com/cainteoir/formats/document";

	// only lists filetypes that are properly supported, excluding intermedate/internal formats ...

	rdf::uri text = rdf::uri(baseuri, "text");
	metadata.push_back(rdf::statement(text, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(text, rdf::dc("title"), rdf::literal("text document")));
	metadata.push_back(rdf::statement(text, rdf::tts("mimetype"), rdf::literal("text/plain")));
	metadata.push_back(rdf::statement(text, rdf::tts("extension"), rdf::literal("*.txt")));

	rdf::uri html = rdf::uri(baseuri, "html");
	metadata.push_back(rdf::statement(html, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(html, rdf::dc("title"), rdf::literal("html document")));
	metadata.push_back(rdf::statement(html, rdf::tts("mimetype"), rdf::literal("text/html")));
	metadata.push_back(rdf::statement(html, rdf::tts("mimetype"), rdf::literal("application/xhtml+xml")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.htm")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.html")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.xhtml")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.xht")));
	metadata.push_back(rdf::statement(html, rdf::tts("extension"), rdf::literal("*.xml")));

	rdf::uri epub = rdf::uri(baseuri, "epub");
	metadata.push_back(rdf::statement(epub, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(epub, rdf::dc("title"), rdf::literal("epub document")));
	metadata.push_back(rdf::statement(epub, rdf::tts("mimetype"), rdf::literal("application/epub+zip")));
	metadata.push_back(rdf::statement(epub, rdf::tts("extension"), rdf::literal("*.epub")));

	rdf::uri gzip = rdf::uri(baseuri, "gzip");
	metadata.push_back(rdf::statement(gzip, rdf::rdf("type"), rdf::tts("DocumentFormat")));
	metadata.push_back(rdf::statement(gzip, rdf::dc("title"), rdf::literal("gzip compressed document")));
	metadata.push_back(rdf::statement(gzip, rdf::tts("mimetype"), rdf::literal("application/x-gzip")));
	metadata.push_back(rdf::statement(gzip, rdf::tts("extension"), rdf::literal("*.gz")));
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
