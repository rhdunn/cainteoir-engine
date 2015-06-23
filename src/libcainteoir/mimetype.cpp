/* Mime Type Support.
 *
 * Copyright (C) 2011-2015 Reece H. Dunn
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
#include "mimetype_database.hpp"

#include <cainteoir/mimetype.hpp>
#include <cainteoir/xmlreader.hpp>

#include <stdexcept>

namespace xml = cainteoir::xml;
namespace rdf = cainteoir::rdf;
namespace m   = cainteoir::mime;

static std::initializer_list<const char *> mimetype_list = {
	m::email_mimetype,
	m::epub_mimetype,
	m::gzip_mimetype,
	m::html_mimetype,
	m::mhtml_mimetype,
	m::ncx_mimetype,
	m::ogg_mimetype,
	m::opf_mimetype,
	m::pdf_mimetype,
	m::rdfxml_mimetype,
	m::rtf_mimetype,
	m::smil_mimetype,
	m::ssml_mimetype,
	m::text_mimetype,
	m::wav_mimetype,
	m::xhtml_mimetype,
	m::xml_mimetype,
	m::zip_mimetype,
};

cainteoir::mime::matchlet &cainteoir::mime::matchlet::operator=(const matchlet &m)
{
	offset  = m.offset;
	range   = m.range;
	pattern = m.pattern;
	return *this;
}

bool cainteoir::mime::matchlet::match(const std::shared_ptr<cainteoir::buffer> &data) const
{
	std::string::size_type pattern_length = pattern.size();
	const char *begin = data->begin() + offset;
	for (const char *start = begin; start != begin + range; ++start)
	{
		if (start + pattern_length >= data->end())
			return false; // out of range!

		if (!strncmp(start, pattern.c_str(), pattern_length))
			return true;
	}
	return false;
}

bool cainteoir::mime::magic::match(const std::shared_ptr<cainteoir::buffer> &data) const
{
	for (auto &matchlet : *this)
	{
		if (!matchlet.match(data))
			return false;
	}
	return !empty();
}

static std::string get_mime_dir(std::string basedir)
{
	// Handle paths like "/usr/share/" that have a trailing '/' ...
	if ((*--basedir.end()) == '/')
		return basedir + "mime/";
	// Handle paths like "/usr/share/ubuntu" that don't have a trailing '/' ...
	return basedir + "/mime/";
}

static std::vector<std::string> get_mime_dirs()
{
	std::vector<std::string> dirs;

	const char *home = getenv("XDG_DATA_HOME");
	if (home)
		dirs.push_back(get_mime_dir(home));

	const char *start = getenv("XDG_DATA_DIRS"); // start of the next path
	const char *end   = start;
	if (start && *start)
	{
		while (*end)
		{
			if (*end == ':')
			{
				dirs.push_back(get_mime_dir(std::string(start, end)));
				start = end + 1;
			}
			++end;
		}

		if (start != end)
			dirs.push_back(get_mime_dir(start));
	}
	else
	{
		dirs.push_back("/usr/local/share/mime/");
		dirs.push_back("/usr/share/mime/");
	}

	return dirs;
}

void cainteoir::mime::mimetype_database::read_aliases_from_cache(mime_cache &cache)
{
	uint32_t offset = cache.u32(4);
	uint32_t count  = cache.u32(offset);
	offset += 4;

	for (; count > 0; --count)
	{
		const char *alias = cache.str(offset);
		const char *mime  = cache.str(offset + 4);

		auto &data = (*this)(mime);
		data->mimetypes.push_back(alias);
		database[alias] = data;

		offset += 8;
	}
}

void cainteoir::mime::mimetype_database::read_reverse_suffix_tree_from_cache(mime_cache &cache, uint32_t offset, const std::string &suffix)
{
	uint32_t count = cache.u32(offset);
	offset = cache.u32(offset + 4);

	for (; count > 0; --count)
	{
		uint32_t c = cache.u32(offset);
		if (c == 0) // leaf node
		{
			const char *mime = cache.str(offset + 4);
			std::string s = '*' + suffix;
			(*this)(mime)->globs.push_back(s);
		}
		else
		{
			std::string s = (char)c + suffix;
			read_reverse_suffix_tree_from_cache(cache, offset + 4, s);
		}
		offset += 12;
	}
}

void cainteoir::mime::mimetype_database::read_globs_from_cache(mime_cache &cache)
{
	read_reverse_suffix_tree_from_cache(cache, cache.u32(16), std::string());
}

void cainteoir::mime::mimetype_database::read_matchlets_from_cache(mime_cache &cache, uint32_t count, uint32_t offset, std::vector<magic> &magic_list, const std::vector<matchlet> &matchlets)
{
	for (; count > 0; --count)
	{
		uint32_t range_start  = cache.u32(offset);
		uint32_t range_length = cache.u32(offset +  4);
		uint32_t value_length = cache.u32(offset + 12);
		const char * value    = cache.str(offset + 16);
		uint32_t child_count  = cache.u32(offset + 24);
		uint32_t child_offset = cache.u32(offset + 28);

		std::vector<matchlet> matches = matchlets;
		matchlet match = { range_start, range_length, std::string(value, value + value_length) };
		matches.push_back(match);
		if (child_count == 0)
			magic_list.push_back(magic(matches));
		else
			read_matchlets_from_cache(cache, child_count, child_offset, magic_list, matches);
		offset += 32;
	}
}

void cainteoir::mime::mimetype_database::read_magic_from_cache(mime_cache &cache)
{
	uint32_t offset = cache.u32(24);
	uint32_t count  = cache.u32(offset);
	offset = cache.u32(offset + 8);

	for (; count > 0; --count)
	{
		const char *mime  = cache.str(offset + 4);
		std::vector<magic> &magic_list = (*this)(mime)->magic;
		read_matchlets_from_cache(cache, cache.u32(offset + 8), cache.u32(offset + 12), magic_list, std::vector<matchlet>());
		offset += 16;
	}
}

void cainteoir::mime::mimetype_database::read_xmlns_from_cache(mime_cache &cache)
{
	uint32_t offset = cache.u32(28);
	uint32_t count  = cache.u32(offset);
	offset += 4;

	for (; count > 0; --count)
	{
		const char *ns   = cache.str(offset);
		const char *name = cache.str(offset + 4);
		const char *mime = cache.str(offset + 8);

		(*this)(mime)->xmlns = ns;
		(*this)(mime)->localname = name;
		offset += 12;
	}
}

std::string cainteoir::mime::mimetype_database::read_comment_from_mimeinfo_file(const std::string &filename)
{
	try
	{
		std::map<std::string, std::string> comments;

		xml::reader reader(cainteoir::make_file_buffer(filename.c_str()), "windows-1252");
		bool in_comment = false;
		std::string lang;
		while (reader.read()) switch (reader.nodeType())
		{
		case xml::reader::beginTagNode:
			if (!reader.nodeName().compare("comment"))
			{
				lang = "--";
				in_comment = true;
			}
			break;
		case xml::reader::attribute:
			// FIXME: reader.context() cannot be used here due to static object
			// initialization order -- this gets initialized before xml::attrs
			// so segfaults if that is used.
			if (in_comment && reader.namespaceUri() == "http://www.w3.org/XML/1998/namespace" && reader.nodeName().str() == "lang")
				lang = reader.nodeValue().buffer()->str();
			break;
		case xml::reader::textNode:
			if (in_comment)
				comments[lang] = reader.nodeValue().buffer()->str();
			break;
		case xml::reader::endTagNode:
			if (!reader.nodeName().compare("comment"))
				in_comment = false;
			break;
		default:
			break;
		}

		const char *lang_env = getenv("LANG");
		if (lang_env)
		{
			std::string lang = lang_env;
			if (!comments[lang].empty())
				return comments[lang];
			lang = lang.substr(0, lang.find('.'));
			if (!comments[lang].empty())
				return comments[lang];
			lang = lang.substr(0, lang.find('_'));
			if (!comments[lang].empty())
				return comments[lang];
		}
		return comments["--"];
	}
	catch (std::runtime_error &)
	{
		// The mimetype information file was not found, or there was
		// a problem reading it, so ignore this file data.
	}
	return "";
}

std::shared_ptr<cainteoir::mime::mime_info> &cainteoir::mime::mimetype_database::operator()(const char *mimetype)
{
	auto &mime = database[mimetype];
	if (!mime.get())
	{
		mime = std::make_shared<mime_info>();
		mime->mimetypes.push_back(mimetype);
	}
	return mime;
}

cainteoir::mime::mimetype_database::mimetype_database()
{
	for (auto &dir : get_mime_dirs())
	{
		try
		{
			mime_cache cache(dir + "mime.cache");
			if (cache.major() != 1 || cache.minor() != 2)
				throw std::runtime_error("unsupported mime.cache version.");

			read_aliases_from_cache(cache);
			read_globs_from_cache(cache);
			read_magic_from_cache(cache);
			read_xmlns_from_cache(cache);

			for (auto &mimetype : mimetype_list)
				(*this)(mimetype)->label = read_comment_from_mimeinfo_file(dir + mimetype + ".xml");
		}
		catch (const std::runtime_error &)
		{
			// Some paths in XDG_DATA_DIRS do not hold any mimetype information,
			// e.g. /usr/share/gnome, so just ignore that path.
		}
	}
}

const cainteoir::mime::mime_info &cainteoir::mime::mimetype_database::operator[](const char *mimetype) const
{
	auto entry = database.find(mimetype);
	if (entry == database.end())
		throw std::runtime_error(std::string(mimetype) + ": mimetype not found in the mimetype database.");
	return *entry->second;
}

cainteoir::mime::mimetype_database cainteoir::mime::mimetypes;

bool cainteoir::mime::mimetype::match(const std::shared_ptr<cainteoir::buffer> &data) const
{
	const mime_info *mime = (const mime_info *)info;
	if (!mime)
		mime = &mimetypes[mime_type];

	for (auto &magic : mime->magic)
	{
		if (magic.match(data))
			return true;
	}
	return false;
}

bool cainteoir::mime::mimetype::match(const std::string &uri, const std::string &name) const
{
	const mime_info *mime = (const mime_info *)info;
	if (!mime)
		mime = &mimetypes[mime_type];

	if (!mime->xmlns.empty())
	{
		if (!(uri == mime->xmlns || uri == mime->xmlns + "#"))
			return false;
	}
	return mime->localname == name;
}

void cainteoir::mime::mimetype::metadata(rdf::graph &aGraph, const std::string &baseuri, const rdf::uri &type) const
{
	const mime_info *mime = (const mime_info *)info;
	if (!mime)
		mime = &mimetypes[mime_type];

	rdf::uri ref = rdf::uri(baseuri, name);
	aGraph.statement(ref, rdf::rdf("type"), type);
	aGraph.statement(ref, rdf::tts("name"), rdf::literal(name));
	aGraph.statement(ref, rdf::dc("title"), rdf::literal(mime->label));
	aGraph.statement(ref, rdf::dc("description"), rdf::literal(mime->label));
	for (auto &mimetype : mime->mimetypes)
		aGraph.statement(ref, rdf::tts("mimetype"), rdf::literal(mimetype));
	for (auto &glob : mime->globs)
		aGraph.statement(ref, rdf::tts("extension"), rdf::literal(glob));
}

/* === MIME Header Handling ===
 * The magic to detect MIME header content from HTTP, SMTP and MHTML documents.
 *
 * This information is stored statically here and not loaded from the shared-mime-info
 * database as the MIME document headers don't have a mimetype themselves (the mimetype
 * governs the content).
 */
//{{{

static const std::initializer_list<m::matchlet> http_pattern1 = { { 0,  1, "HTTP/1.0" }, {  8, 81, "\nContent-Type:" } };
static const std::initializer_list<m::matchlet> http_pattern2 = { { 0,  1, "HTTP/1.1" }, {  8, 81, "\nContent-Type:" } };

// for multipart mime documents (e.g. mhtml documents) ...
static const std::initializer_list<m::matchlet> mime_pattern1 = { { 0, 81, "MIME-Version: 1.0" }, { 16, 81, "\nContent-Type:" } };
static const std::initializer_list<m::matchlet> mime_pattern2 = { { 0,  2, "Content-Type:" } };
static const std::initializer_list<m::matchlet> mime_pattern3 = { { 0,  2, "Content-Transfer-Encoding:" } };
static const std::initializer_list<m::matchlet> mime_pattern4 = { { 0,  2, "Content-Location:" } };

// for newsgroup archives ...
static const std::initializer_list<m::matchlet> news_pattern1 = { { 0,  1, "Article " }, { 10, 15, " of " } };
static const std::initializer_list<m::matchlet> news_pattern2 = { { 0,  1, "From "    }, {  5, 80, "\nPath: " } };
static const std::initializer_list<m::matchlet> news_pattern3 = { { 0,  1, "Date: " } };
static const std::initializer_list<m::matchlet> news_pattern4 = { { 0,  1, "Newsgroups: " } };
static const std::initializer_list<m::matchlet> news_pattern5 = { { 0,  1, "MDate: " } };

// for mail archives ...
static const std::initializer_list<m::matchlet> mail_pattern1 = { { 0, 1, "From " }, { 5, 80, "\nFrom: " } };
static const std::initializer_list<m::matchlet> mail_pattern2 = { { 0, 1, "From " }, { 5, 80, "\nSubject: " } };

// for MIME-like story metadata ...
static const std::initializer_list<m::matchlet> story_pattern1 = { { 0,  1, "Title: " } };
static const std::initializer_list<m::matchlet> story_pattern2 = { { 0,  1, "Author: " } };
static const std::initializer_list<m::matchlet> story_pattern3 = { { 0,  1, "Keywords: " } };
static const std::initializer_list<m::matchlet> story_pattern4 = { { 0,  1, "Story: " } };

static const std::initializer_list<m::magic> mime_magic = {
	mime_pattern1,  mime_pattern2, mime_pattern3, mime_pattern4,
	news_pattern1,  news_pattern2, news_pattern3, news_pattern4, news_pattern5,
	mail_pattern1,  mail_pattern2,
	http_pattern1,  http_pattern2,
	story_pattern1, story_pattern2, story_pattern3, story_pattern4,
};

const m::mime_info m::mime_data = { mime_magic, "", "", "", {}, {} };

//}}}
/* === Cainteoir Dictionary/Phoneme Format ===
 */
//{{{

static const std::initializer_list<m::matchlet> cainteoir_pattern1 = { { 0,  1, ".author\t" } };
static const std::initializer_list<m::matchlet> cainteoir_pattern2 = { { 0,  1, ".copyright\t" } };
static const std::initializer_list<m::matchlet> cainteoir_pattern3 = { { 0,  1, ".license\t" } };
static const std::initializer_list<m::matchlet> cainteoir_pattern4 = { { 0,  1, ".description\t" } };
static const std::initializer_list<m::matchlet> cainteoir_pattern5 = { { 0,  1, ".import\t" } };
static const std::initializer_list<m::matchlet> cainteoir_pattern6 = { { 0,  1, ".phonemeset\t" } };

static const std::initializer_list<m::magic> cainteoir_magic = {
	cainteoir_pattern1,
	cainteoir_pattern2,
	cainteoir_pattern3,
	cainteoir_pattern4,
	cainteoir_pattern5,
	cainteoir_pattern6,
};

static const m::mime_info cainteoir_data = { cainteoir_magic, "", "", "", {}, {} };

//}}}
/* === CMU Dictionary Format ===
 */
//{{{

static const std::initializer_list<m::matchlet> cmudict_pattern1 = { { 0,  1, ";;;" } };

static const std::initializer_list<m::magic> cmudict_magic = {
	cmudict_pattern1,
};

static const m::mime_info cmudict_data = { cmudict_magic, "", "", "", {}, {} };

//}}}

const m::mimetype m::cainteoir("cainteoir", nullptr, &cainteoir_data);
const m::mimetype m::cmudict("cmudict", nullptr, &cmudict_data);
const m::mimetype m::email( "email", m::email_mimetype);
const m::mimetype m::epub(  "epub",  m::epub_mimetype);
const m::mimetype m::gzip(  "gzip",  m::gzip_mimetype);
const m::mimetype m::html(  "html",  m::html_mimetype);
const m::mimetype m::mhtml( "mhtml", m::mhtml_mimetype);
const m::mimetype m::mime(  "mime",  nullptr, &mime_data);
const m::mimetype m::ncx(   "ncx",   m::ncx_mimetype);
const m::mimetype m::ogg(   "ogg",   m::ogg_mimetype);
const m::mimetype m::opf(   "opf",   m::opf_mimetype);
const m::mimetype m::pdf(   "pdf",   m::pdf_mimetype);
const m::mimetype m::rdfxml("rdf",   m::rdfxml_mimetype);
const m::mimetype m::rtf(   "rtf",   m::rtf_mimetype);
const m::mimetype m::smil(  "smil",  m::smil_mimetype);
const m::mimetype m::ssml(  "ssml",  m::ssml_mimetype);
const m::mimetype m::text(  "text",  m::text_mimetype);
const m::mimetype m::wav(   "wav",   m::wav_mimetype);
const m::mimetype m::xhtml( "xhtml", m::xhtml_mimetype);
const m::mimetype m::xml(   "xml",   m::xml_mimetype);
const m::mimetype m::zip(   "zip",   m::zip_mimetype);
