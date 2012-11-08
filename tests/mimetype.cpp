/* Test/diagnostic program for mimetype detection.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#include "../src/libcainteoir/mimetype_database.hpp"
#include "../src/libcainteoir/parsers/parsers.hpp"
#include <cainteoir/mimetype.hpp>
#include <stdexcept>
#include <cstdio>

namespace mime = cainteoir::mime;

bool test_mimetype(const std::shared_ptr<cainteoir::buffer> &aData,
                   const mime::mimetype &aMimeType)
{
	const mime::mime_info info = aMimeType.mime_type ? mime::mimetypes[aMimeType.mime_type] : mime::mime_data;
	fprintf(stderr, "Checking mimetype %s (%s) ...\n", aMimeType.mime_type, aMimeType.name);

	bool matched = false;
	for (auto &magic : info.magic)
	{
		fprintf(stderr, "... Checking magic block ... %s\n", magic.match(aData) ? "yes" : "no");
		matched |= magic.match(aData);
		for (auto &matchlet : magic)
		{
			fprintf(stderr, "... ... Checking matchlet |");
			for (auto &c : matchlet.pattern)
			{
				uint8_t ch = c & 0xFF;
				if (ch < 0x20 || ch >= 0x80)
					fprintf(stderr, "\\x%02X", ch);
				else
					fputc(ch, stderr);
			}
			fprintf(stderr, "| (%d..%d) ... %s\n", matchlet.offset, matchlet.offset + matchlet.range, matchlet.match(aData) ? "yes" : "no");
		}
	}

	if ((!info.xmlns.empty() || !info.localname.empty()) && mime::xml.match(aData))
	{
		auto reader = cainteoir::createXmlReader(aData, "windows-1252");
		std::string namespaceUri = reader->namespaceUri();
		std::string rootName     = reader->nodeName().str();

		fprintf(stderr, "... Checking xmlns [%s]%s ... %s\n", info.xmlns.c_str(), info.localname.c_str(), aMimeType.match(namespaceUri, rootName) ? "yes" : "no");
		matched |= aMimeType.match(namespaceUri, rootName);
	}

	if (aMimeType.mime_type && mime::zip.match(aData))
	{
		auto archive = create_zip_archive(aData, cainteoir::rdf::uri(std::string(), std::string()));

		auto mimetype = archive->read("mimetype");
		fprintf(stderr, "... Checking mimetype file in zip ... %s\n", mimetype ? "yes" : "no");
		if (mimetype)
		{
			bool match = mimetype->startswith(aMimeType.mime_type);
			fprintf(stderr, "... ... Checking mimetype |%s| ... %s\n", aMimeType.mime_type, match ? "yes" : "no");
			matched |= match;
		}
	}

	fprintf(stdout, "Checking mimetype %s (%s) ... %s\n", aMimeType.mime_type, aMimeType.name, matched ? "yes" : "no");
	fputc('\n', stderr);

	return matched;
}

enum class content_type
{
	zip,
	mime,
	other,
};

content_type test_mimetypes(const std::shared_ptr<cainteoir::buffer> &data)
{
	content_type type = content_type::other;
	test_mimetype(data, mime::email);
	test_mimetype(data, mime::epub);
	test_mimetype(data, mime::gzip);
	test_mimetype(data, mime::html);
	test_mimetype(data, mime::mhtml);
	if (test_mimetype(data, mime::mime))
		type = content_type::mime;
	test_mimetype(data, mime::ncx);
	test_mimetype(data, mime::ogg);
	test_mimetype(data, mime::opf);
	test_mimetype(data, mime::pdf);
	test_mimetype(data, mime::rdfxml);
	test_mimetype(data, mime::rtf);
	test_mimetype(data, mime::smil);
	test_mimetype(data, mime::ssml);
	test_mimetype(data, mime::text);
	test_mimetype(data, mime::wav);
	test_mimetype(data, mime::xhtml);
	test_mimetype(data, mime::xml);
	if (test_mimetype(data, mime::zip))
		type = content_type::zip;
	return type;
}

void test_file(std::shared_ptr<cainteoir::buffer> &data,
               const cainteoir::rdf::uri &subject)
{
	std::string title = subject.str();
	fprintf(stdout, "file path: %s\n", title.c_str());
	fprintf(stdout, "\n");
	content_type type = test_mimetypes(data);
	if (type == content_type::mime)
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "MIME headers detected ... checking content:\n");

		cainteoir::rdf::graph metadata;
		auto mime = cainteoir::parseMimeHeaders(data, subject, metadata, title);
		if (!mime.first)
		{
			fprintf(stdout, "... will parse MIME content as text/plain\n");
			fprintf(stdout, "\n");
		}
		test_file(mime.second, subject);
	}
	else if (type == content_type::zip)
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "ZIP archive detected ... checking content:\n");

		auto zip = cainteoir::create_zip_archive(data, subject);
		fprintf(stdout, "... file count: %d\n", zip->files().size());
		fprintf(stdout, "\n");

		for (auto &file : zip->files())
		{
			auto content = zip->read(file.c_str());
			test_file(content, zip->location(file, std::string()));
		}
	}
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		cainteoir::rdf::uri subject(argv[0], std::string());
		auto data = cainteoir::make_file_buffer(argv[0]);
		test_file(data, subject);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
