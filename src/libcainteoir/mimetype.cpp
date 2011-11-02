/* Mime Type Support.
 *
 * Copyright (C) 2011 Reece H. Dunn
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
#include <cainteoir/platform.hpp>

#include <netinet/in.h> // for ntohs and ntohl
#include <stdexcept>
#include <vector>
#include <map>

static const char *email_mimetype  = "message/rfc822";
static const char *epub_mimetype   = "application/epub+zip";
static const char *gzip_mimetype   = "application/x-gzip";
static const char *html_mimetype   = "text/html";
static const char *mhtml_mimetype  = "multipart/related";
static const char *ncx_mimetype    = "application/x-dtbncx+xml";
static const char *ogg_mimetype    = "audio/x-vorbis+ogg";
static const char *opf_mimetype    = "application/oebps-package+xml";
static const char *rdfxml_mimetype = "application/rdf+xml";
static const char *rtf_mimetype    = "application/rtf";
static const char *smil_mimetype   = "application/smil";
static const char *ssml_mimetype   = "application/ssml+xml";
static const char *text_mimetype   = "text/plain";
static const char *wav_mimetype    = "audio/x-wav";
static const char *xhtml_mimetype  = "application/xhtml+xml";
static const char *xml_mimetype    = "application/xml";

static std::initializer_list<const char *> mimetype_list = {
	email_mimetype,
	epub_mimetype,
	gzip_mimetype,
	html_mimetype,
	mhtml_mimetype,
	ncx_mimetype,
	ogg_mimetype,
	opf_mimetype,
	rdfxml_mimetype,
	rtf_mimetype,
	smil_mimetype,
	ssml_mimetype,
	text_mimetype,
	wav_mimetype,
	xhtml_mimetype,
	xml_mimetype
};

struct matchlet
{
	/** @brief The position where the pattern starts. */
	uint32_t offset;

	/** @brief The number of bytes after offset the pattern can occur. */
	uint32_t range;

	/** @brief The pattern to match against. */
	std::string pattern;

	matchlet &operator=(const matchlet &m)
	{
		offset  = m.offset;
		range   = m.range;
		pattern = m.pattern;
		return *this;
	}

	bool match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
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
};

struct magic : public std::vector<matchlet>
{
	magic(const std::initializer_list<matchlet> &matchlets)
		: std::vector<matchlet>(matchlets)
	{
	}

	magic(const std::vector<matchlet> &matchlets)
		: std::vector<matchlet>(matchlets)
	{
	}

	bool match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
	{
		foreach_iter (matchlet, *this)
		{
			if (!matchlet->match(data))
				return false;
		}
		return !empty();
	}
};

struct mime_info
{
	/** @brief The magic data that identifies this mimetype/content. */
	std::vector<struct magic> magic;

	/** @brief The XML namespace associated with the mimetype (for XML documents only). */
	std::string xmlns;

	/** @brief The XML local name associated with the mimetype (for XML documents only). */
	std::string localname;

	/** @brief The mimetype description (comment field). */
	std::string label;

	/** @brief The filename patterns for files matching this mimetype. */
	std::vector<std::string> globs;

	/** @brief The mimetype aliases for this mimetype. */
	std::vector<std::string> aliases;
};

std::string get_mime_dir(std::string basedir)
{
	// Handle paths like "/usr/share/" that have a trailing '/' ...
	if (basedir.back() == '/')
		return basedir + "mime/";
	// Handle paths like "/usr/share/ubuntu" that don't have a trailing '/' ...
	return basedir + "/mime/";
}

std::vector<std::string> get_mime_dirs()
{
	std::vector<std::string> dirs;

	const char *home = getenv("XDG_DATA_HOME");
	if (home)
		dirs.push_back(get_mime_dir(home));

	const char *start = getenv("XDG_DATA_DIRS"); // start of the next path
	const char *end   = start;
	if (start)
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

	return dirs;
}

struct mime_cache
{
	mime_cache(const std::string &filename) : cache(filename.c_str())
	{
	}

	uint16_t u16(uint32_t offset) const { return ntohs(*(uint16_t *)(cache.begin() + offset)); }
	uint32_t u32(uint32_t offset) const { return ntohl(*(uint32_t *)(cache.begin() + offset)); }

	const char *str(uint32_t offset) const { return cache.begin() + u32(offset); }

	uint16_t major() const { return u16(0); }
	uint16_t minor() const { return u16(2); }
private:
	cainteoir::mmap_buffer cache;
};

struct mimetype_database : public std::map<std::string, mime_info>
{
	void read_aliases_from_cache(mime_cache &cache)
	{
		uint32_t offset = cache.u32(4);
		uint32_t count  = cache.u32(offset);
		offset += 4;

		for (; count > 0; --count)
		{
			const char *alias = cache.str(offset);
			const char *mime  = cache.str(offset + 4);

			foreach_iter (mimetype, mimetype_list)
			{
				if (!strcmp(*mimetype, mime))
					(*this)[mime].aliases.push_back(alias);
			}
			offset += 8;
		}
	}

	void read_reverse_suffix_tree_from_cache(mime_cache &cache, uint32_t offset, const std::string &suffix)
	{
		uint32_t count = cache.u32(offset);
		offset = cache.u32(offset + 4);

		for (; count > 0; --count)
		{
			uint32_t c = cache.u32(offset);
			if (c == 0) // leaf node
			{
				const char *mime = cache.str(offset + 4);
				foreach_iter (mimetype, mimetype_list)
				{
					if (!strcmp(*mimetype, mime))
					{
						std::string s = '*' + suffix;
						(*this)[mime].globs.push_back(s);
					}
				}
			}
			else
			{
				std::string s = (char)c + suffix;
				read_reverse_suffix_tree_from_cache(cache, offset + 4, s);
			}
			offset += 12;
		}
	}

	void read_globs_from_cache(mime_cache &cache)
	{
		read_reverse_suffix_tree_from_cache(cache, cache.u32(16), std::string());
	}

	void read_matchlets_from_cache(mime_cache &cache, uint32_t count, uint32_t offset, std::vector<magic> &magic_list, const std::vector<matchlet> &matchlets)
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

	void read_magic_from_cache(mime_cache &cache)
	{
		uint32_t offset = cache.u32(24);
		uint32_t count  = cache.u32(offset);
		offset = cache.u32(offset + 8);

		for (; count > 0; --count)
		{
			const char *mime  = cache.str(offset + 4);
			foreach_iter (mimetype, mimetype_list)
			{
				if (!strcmp(*mimetype, mime))
				{
					std::vector<magic> &magic_list = (*this)[mime].magic;
					read_matchlets_from_cache(cache, cache.u32(offset + 8), cache.u32(offset + 12), magic_list, std::vector<matchlet>());
				}
			}
			offset += 16;
		}
	}

	void read_xmlns_from_cache(mime_cache &cache)
	{
		uint32_t offset = cache.u32(28);
		uint32_t count  = cache.u32(offset);
		offset += 4;

		for (; count > 0; --count)
		{
			const char *ns   = cache.str(offset);
			const char *name = cache.str(offset + 4);
			const char *mime = cache.str(offset + 8);

			foreach_iter (mimetype, mimetype_list)
			{
				if (!strcmp(*mimetype, mime))
				{
					(*this)[mime].xmlns = ns;
					(*this)[mime].localname = name;
				}
			}
			offset += 12;
		}
	}

	mimetype_database()
	{
		foreach_iter (dir, get_mime_dirs())
		{
			try
			{
				mime_cache cache(*dir + "mime.cache");
				if (cache.major() != 1 || cache.minor() != 2)
					throw std::runtime_error("unsupported mime.cache version.");

				read_aliases_from_cache(cache);
				read_globs_from_cache(cache);
				read_magic_from_cache(cache);
				read_xmlns_from_cache(cache);
			}
			catch (const std::runtime_error &)
			{
				// Some paths in XDG_DATA_DIRS do not hold any mimetype information,
				// e.g. /usr/share/gnome, so just ignore that path.
			}
		}

		/** @name Mime Data
		  *
		  * This will be refactored to load the mime data from the shared-mime-info database.
		  */
		//@{

		(*this)[email_mimetype].label = _("electronic mail document");

		(*this)[epub_mimetype].label = _("electronic publication document");

		(*this)[gzip_mimetype].label = _("gzip compressed document");

		(*this)[html_mimetype].localname = "html";
		(*this)[html_mimetype].label = _("html document");

		(*this)[mhtml_mimetype].label = _("mhtml document");
		(*this)[mhtml_mimetype].globs = { "*.mht" };

		(*this)[ncx_mimetype].xmlns = "http://www.daisy.org/z3986/2005/ncx/";
		(*this)[ncx_mimetype].localname = "ncx";
		(*this)[ncx_mimetype].label = _("navigation control document");
		(*this)[ncx_mimetype].globs = { "*.ncx" };

		(*this)[ogg_mimetype].label = _("ogg vorbis audio");

		(*this)[opf_mimetype].xmlns = "http://www.idpf.org/2007/opf";
		(*this)[opf_mimetype].localname = "package";
		(*this)[opf_mimetype].label = _("open package format document");
		(*this)[opf_mimetype].globs = { "*.opf" };

		(*this)[rdfxml_mimetype].xmlns = "http://www.w3.org/1999/02/22-rdf-syntax-ns";
		(*this)[rdfxml_mimetype].localname = "RDF";
		(*this)[rdfxml_mimetype].label = _("RDF/XML document");

		(*this)[rtf_mimetype].label = _("rich text document");

		(*this)[smil_mimetype].xmlns = "http://www.w3.org/ns/SMIL";
		(*this)[smil_mimetype].localname = "smil";
		(*this)[smil_mimetype].label = _("SMIL document");

		(*this)[ssml_mimetype].xmlns = "http://www.w3.org/2001/10/synthesis";
		(*this)[ssml_mimetype].localname = "speak";
		(*this)[ssml_mimetype].label = _("speech synthesis markup document");
		(*this)[ssml_mimetype].globs = { "*.ssml" };

		(*this)[text_mimetype].label = _("plain text document");

		(*this)[wav_mimetype].label = _("wave audio");

		(*this)[xhtml_mimetype].label = _("xhtml document");

		//@}
	}
};

mimetype_database mimetypes;

bool cainteoir::mime::mimetype::match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
{
	const mime_info *mime = (const mime_info *)info;
	foreach_iter (magic, mime->magic)
	{
		if (magic->match(data))
			return true;
	}
	return false;
}

bool cainteoir::mime::mimetype::match(const std::string &uri, const std::string &name) const
{
	const mime_info *mime = (const mime_info *)info;
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

	rdf::uri ref = rdf::uri(baseuri, name);
	aGraph.statement(ref, rdf::rdf("type"), type);
	aGraph.statement(ref, rdf::tts("name"), rdf::literal(name));
	aGraph.statement(ref, rdf::dc("title"), rdf::literal(mime->label));
	aGraph.statement(ref, rdf::dc("description"), rdf::literal(mime->label));
	aGraph.statement(ref, rdf::tts("mimetype"), rdf::literal(mime_type));
	foreach_iter (alias, mime->aliases)
		aGraph.statement(ref, rdf::tts("mimetype"), rdf::literal(*alias));
	foreach_iter (glob, mime->globs)
		aGraph.statement(ref, rdf::tts("extension"), rdf::literal(*glob));
}

namespace m = cainteoir::mime;

/** === MIME Header Handling ===
  * The magic to detect MIME header content from HTTP, SMTP and MHTML documents.
  *
  * This information is stored statically here and not loaded from the shared-mime-info
  * database as the MIME document headers don't have a mimetype themselves (the mimetype
  * governs the content).
  */
//{{{

static const std::initializer_list<matchlet> http_pattern1 = { { 0,  1, "HTTP/1.0" },          {  8, 81, "\nContent-Type:" } };
static const std::initializer_list<matchlet> http_pattern2 = { { 0,  1, "HTTP/1.1" },          {  8, 81, "\nContent-Type:" } };
static const std::initializer_list<matchlet> mime_pattern1 = { { 0, 81, "MIME-Version: 1.0" }, { 16, 81, "\nContent-Type:" } }; // for multipart/mhtml documents
static const std::initializer_list<matchlet> mime_pattern2 = { { 0,  4, "\nContent-Type:" } }; // for multipart mime documents (e.g. mhtml)
static const mime_info mime_data = { { mime_pattern1, mime_pattern2, http_pattern1, http_pattern2 }, "", "", "", {}, {} };

//}}}

const m::mimetype m::mime("mime",  NULL, &mime_data);

const m::mimetype m::email( "email", email_mimetype,  &mimetypes[email_mimetype]);
const m::mimetype m::epub(  "epub",  epub_mimetype,   &mimetypes[epub_mimetype]);
const m::mimetype m::gzip(  "gzip",  gzip_mimetype,   &mimetypes[gzip_mimetype]);
const m::mimetype m::html(  "html",  html_mimetype,   &mimetypes[html_mimetype]);
const m::mimetype m::mhtml( "mhtml", mhtml_mimetype,  &mimetypes[mhtml_mimetype]);
const m::mimetype m::ncx(   "ncx",   ncx_mimetype,    &mimetypes[ncx_mimetype]);
const m::mimetype m::ogg(   "ogg",   ogg_mimetype,    &mimetypes[ogg_mimetype]);
const m::mimetype m::opf(   "opf",   opf_mimetype,    &mimetypes[opf_mimetype]);
const m::mimetype m::rdfxml("rdf",   rdfxml_mimetype, &mimetypes[rdfxml_mimetype]);
const m::mimetype m::rtf(   "rtf",   rtf_mimetype,    &mimetypes[rtf_mimetype]);
const m::mimetype m::smil(  "smil",  smil_mimetype,   &mimetypes[smil_mimetype]);
const m::mimetype m::ssml(  "ssml",  ssml_mimetype,   &mimetypes[ssml_mimetype]);
const m::mimetype m::text(  "text",  text_mimetype,   &mimetypes[text_mimetype]);
const m::mimetype m::wav(   "wav",   wav_mimetype,    &mimetypes[wav_mimetype]);
const m::mimetype m::xhtml( "xhtml", xhtml_mimetype,  &mimetypes[xhtml_mimetype]);
const m::mimetype m::xml(   "xml",   xml_mimetype,    &mimetypes[xml_mimetype]);

/** References
  *
  * 	1.	http://standards.freedesktop.org/shared-mime-info-spec/0.20/
  */
