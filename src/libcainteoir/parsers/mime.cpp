/* MIME Header Parser.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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
#include <cainteoir/unicode.hpp>
#include "parsers.hpp"
#include <stdexcept>

namespace rdf    = cainteoir::rdf;
namespace mime   = cainteoir::mime;
namespace events = cainteoir::events;

namespace cainteoir { namespace utf8
{
	static bool isspace(uint32_t c)
	{
		switch (c)
		{
		case 0x000009: // HORIZONTAL TAB
		case 0x00000A: // LINE FEED
		case 0x00000D: // CARRIDGE RETURN
		case 0x000020: // SPACE
		case 0x0000A0: // NON-BREAKING SPACE
			return true;
		}
		return false;
	}
}}

static inline bool is_mime_header_char(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-';
}

struct mime_headers : public cainteoir::buffer
{
	std::shared_ptr<cainteoir::buffer> mOriginal;
	std::string encoding;
	std::string mimetype;
	std::string title;
	std::string charset;

	bool parse_headers(const rdf::uri &subject, rdf::graph &aGraph, cainteoir::buffer &boundary)
	{
		while (first <= last)
		{
			if (*first == '~')
				++first;

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

				if (type <= value.end() && *type == ';')
				{
					++type;
					while (type <= value.end() && (*type == ' ' || *type == '\t' || *type == '\r' || *type == '\n'))
						++type;

					const char * name = type;
					while (type <= value.end() && *type != '=')
						++type;

					cainteoir::buffer arg(name, type);
					++type;

					if (*type != '"') continue;
					++type;

					const char * content = type;
					while (type <= value.end() && *type != '"')
						++type;

					if (!arg.compare("boundary"))
						boundary = cainteoir::buffer(content, type);
					else if (!arg.compare("charset"))
						charset = cainteoir::buffer(content, type).str();
				}
			}
			else if (!name.comparei("Subject") || !name.comparei("Title") || !name.comparei("Story"))
			{
				title = value.str();
				aGraph.statement(subject, rdf::dc("title"), rdf::literal(title));
			}
			else if (!name.comparei("From") || !name.comparei("Author"))
			{
				// name ...

				const char *name_begin = value.begin();
				const char *name_end   = value.begin();

				const char *mbox_begin = nullptr;
				const char *mbox_end   = nullptr;

				bool mbox_is_name = false;

				while (name_end <= value.end() && *name_end == ' ')
					++name_end;

				name_begin = name_end;

				while (name_end <= value.end() && *name_end != '<' && *name_end != '(' && !(name_end[0] == '&' && name_end[1] == 'l' && name_end[2] == 't' && name_end[3] == ';'))
				{
					if (*name_end == '@') // email only ...
					{
						mbox_begin = name_begin;
						mbox_end   = name_end;
						mbox_is_name = true;
					}

					++name_end;
				}

				if (name_end < value.end()) // email address ...
				{
					if (*name_end == '&') // &lt;...&gt;
					{
						mbox_is_name = false;
						mbox_begin = name_end + 4;
						mbox_end = value.end();
						while (mbox_end > mbox_begin && !(mbox_end[0] == '&' && mbox_end[1] == 'g' && mbox_end[2] == 't' && mbox_end[3] == ';'))
							--mbox_end;
					}
					else if (*name_end == '<') // <...>
					{
						mbox_is_name = false;
						mbox_begin = name_end + 1;
						mbox_end = value.end();
						while (mbox_end > mbox_begin && *mbox_end != '>')
							--mbox_end;
					}
					else // (...)
					{
						mbox_is_name = true;
						mbox_begin = name_end + 1;
						mbox_end = value.end();
						while (mbox_end > mbox_begin && *mbox_end != ')')
							--mbox_end;
					}
				}

				// clean-up name ...

				--name_end;
				while (name_end > value.begin() && (*name_end == ' ' || *name_end == '\r' || *name_end == '\n'))
					--name_end;
				++name_end;

				if (*name_begin == '"' && *(name_end-1) == '"')
				{
					++name_begin;
					--name_end;
				}

				if (mbox_begin == nullptr) // name only ...
					aGraph.statement(subject, rdf::dc("creator"), rdf::literal(std::string(name_begin, value.end())));
				else // name and email address ...
				{
					const rdf::uri from = aGraph.genid();
					aGraph.statement(subject, rdf::dc("creator"), from);
					aGraph.statement(from, rdf::rdf("type"), rdf::foaf("Person"));
					if (mbox_is_name)
					{
						aGraph.statement(from, rdf::rdf("value"), rdf::literal(std::string(mbox_begin, mbox_end)));
						aGraph.statement(from, rdf::foaf("mbox"), rdf::literal("mailto:" + std::string(name_begin, name_end)));
					}
					else
					{
						aGraph.statement(from, rdf::rdf("value"), rdf::literal(std::string(name_begin, name_end)));
						aGraph.statement(from, rdf::foaf("mbox"), rdf::literal("mailto:" + std::string(mbox_begin, mbox_end)));
					}
				}
			}
			else if (!name.comparei("Newsgroups"))
				aGraph.statement(subject, rdf::dc("publisher"), rdf::literal(value.str()));
			else if (!name.comparei("Keywords"))
			{
				const char *a = value.begin();
				const char *b = value.begin();
				while (a != value.end())
				{
					a = b;
					while (a != value.end() && (*a == ',' || *a == ' '))
						++a;

					b = a;
					while (b != value.end() && *b != ',')
						++b;

					if (b == value.end())
						--b;

					while (b != a && (*b == ',' || *b == ' '))
						--b;
					++b;

					if (b > a)
						aGraph.statement(subject, rdf::dc("subject"), rdf::literal(std::string(a, b)));
				}
			}
		}

		return false;
	}

	mime_headers(std::shared_ptr<cainteoir::buffer> &data, const rdf::uri &subject, rdf::graph &aGraph, const std::string &aTitle)
		: cainteoir::buffer(*data)
		, mOriginal(data)
		, encoding("8bit")
		, title(aTitle)
	{
		while (first <= last && (*first == ' ' || *first == '\t' || *first == '\r' || *first == '\n'))
			++first;

		if (!strncmp(first, "HTTP/1.0 ", 9) ||
		    !strncmp(first, "HTTP/1.1 ", 9) ||
		    !strncmp(first, "Article ", 8)  ||
		    !strncmp(first, "From ", 5))
		{
			while (first <= last && *first != '\n')
				++first;
			++first;
		}

		cainteoir::buffer boundary(nullptr, nullptr);
		if (!parse_headers(subject, aGraph, boundary))
			first = mOriginal->begin();
		else if (!boundary.empty())
		{
			const char * begin = nullptr;

			while (first <= last)
			{
				if (first[0] == '-' && first[1] == '-' && !strncmp(first + 2, boundary.begin(), boundary.size()))
				{
					if (begin == nullptr)
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

std::shared_ptr<cainteoir::document_reader>
cainteoir::createMimeReader(std::shared_ptr<buffer> &aData,
                            const rdf::uri &aSubject,
                            rdf::graph &aPrimaryMetadata,
                            const std::string &aTitle)
{
	std::shared_ptr<mime_headers> mime = std::make_shared<mime_headers>(aData, aSubject, aPrimaryMetadata, aTitle);

	std::shared_ptr<cainteoir::buffer> decoded;
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
		throw std::runtime_error(i18n("unsupported content-transfer-encoding"));

	if (!mime->charset.empty())
		decoded = cainteoir::encoding(mime->charset.c_str()).decode(decoded);

	if (mime->begin() == aData->begin()) // Avoid an infinite loop when there is just the mime header.
		return createPlainTextReader(decoded, aSubject, aPrimaryMetadata, mime->title);

	return createDocumentReader(decoded, aSubject, aPrimaryMetadata, mime->title);
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createMimeInHtmlReader(std::shared_ptr<cainteoir::buffer> &aData,
                                  const rdf::uri &aSubject,
                                  rdf::graph &aPrimaryMetadata,
                                  const std::string &aTitle,
                                  const char *aDefaultEncoding)
{
	auto reader = cainteoir::createXmlReader(aData, aDefaultEncoding);
	reader->set_predefined_entities(xml::html_entities);

	const char *first = nullptr;
	const char *last  = nullptr;
	bool processing   = true;
	do switch (reader->nodeType())
	{
	case xml::reader::beginTagNode:
		if (!reader->nodeName().comparei("pre"))
			first = reader->current() + 1;
		break;
	case xml::reader::endTagNode:
		if (first && reader->nodeName().comparei("pre"))
			processing = false;
		break;
	case xml::reader::attribute:
	case xml::reader::commentNode:
		break;
	case xml::reader::textNode:
	case xml::reader::error: // email address as tag -- Joseph <joe@world.net>
		last = reader->current();
		break;
	default:
		return std::shared_ptr<cainteoir::document_reader>();
	} while (processing && reader->read());

	if (!first || !last || last < first)
		return std::shared_ptr<cainteoir::document_reader>();

	uint32_t ch = 0;
	const char *next = first;
	while ((next = utf8::read(first, ch)) && utf8::isspace(ch))
		first = next;

	auto text = std::make_shared<buffer>(first, last);

	if (!mime::email.match(text) && !mime::mime.match(text))
		return std::shared_ptr<cainteoir::document_reader>();

	auto data = cainteoir::copy(*text, 0);
	return createMimeReader(data, aSubject, aPrimaryMetadata, aTitle);
}
