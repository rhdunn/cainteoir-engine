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

#include <vector>

struct matchlet
{
	/** @brief The position where the pattern starts. */
	uint32_t offset;

	/** @brief The number of bytes after offset the pattern can occur. */
	uint32_t range;

	/** @brief The pattern to match against. */
	const std::string pattern;

	bool match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
	{
		std::string::size_type pattern_length = pattern.size();
		const char *begin = data->begin() + offset;
		for (const char *start = begin; start != begin + range + 1; ++start)
		{
			if (start + pattern_length >= data->end())
				return false; // out of range!

			if (!strncmp(start, pattern.c_str(), pattern_length))
				return true;
		}
		return false;
	}
};

struct magic
{
	/** @brief The patterns to match against. */
	const std::vector<matchlet> matchlets;

	bool match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
	{
		foreach_iter (matchlet, matchlets)
		{
			if (!matchlet->match(data))
				return false;
		}
		return true;
	}
};

struct mime_info
{
	/** @brief The magic data that identifies this mimetype/content. */
	const std::vector<struct magic> magic;

	/** @brief The XML namespace associated with the mimetype (for XML documents only). */
	const std::string xmlns;

	/** @brief The XML local name associated with the mimetype (for XML documents only). */
	const std::string localname;

	/** @brief The mimetype description (comment field). */
	const std::string label;

	/** @brief The filename patterns for files matching this mimetype (null terminated). */
	std::vector<std::string> globs;

	/** @brief The mimetype aliases for this mimetype. */
	std::vector<std::string> aliases;
};

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
		if (!(uri == mime->xmlns || uri == std::string(mime->xmlns) + "#"))
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

/** @name Mime Data
  *
  * This will be refactored to load the mime data from the shared-mime-info database.
  */
//@{

static const std::initializer_list<matchlet> email_pattern1 = { { 0, 0, "From: " } };
static const std::initializer_list<matchlet> email_pattern2 = { { 0, 0, "Subject: " } };
static const std::initializer_list<magic> email_magic = { { email_pattern1 }, { email_pattern2 } };
static const std::initializer_list<std::string> email_aliases = { "text/x-mail" };
static const std::initializer_list<std::string> email_globs = { "*.eml", "*.emlx", "*.msg", "*.mbx" };
static const mime_info email_data = { email_magic, "", "", _("electronic mail document"), email_globs, email_aliases };

static const std::initializer_list<matchlet> epub_pattern = { { 0, 0, "PK" }, { 30, 0, "mimetype" }, { 38, 0, "application/epub+zip" } };
static const std::initializer_list<magic> epub_magic = { { epub_pattern } };
static const std::initializer_list<std::string> epub_globs = { "*.epub" };
static const mime_info epub_data = { epub_magic, "", "", _("electronic publication document"), epub_globs, {} };

static const std::initializer_list<matchlet> gzip_pattern = { { 0, 0, "\037\213" } };
static const std::initializer_list<magic> gzip_magic = { { gzip_pattern } };
static const std::initializer_list<std::string> gzip_globs = { "*.gz" };
static const mime_info gzip_data = { gzip_magic, "", "", _("gzip compressed document"), gzip_globs, {} };

static const std::initializer_list<matchlet> html_pattern1 = { { 0, 0, "<html" } };
static const std::initializer_list<matchlet> html_pattern2 = { { 0, 0, "<HTML" } };
static const std::initializer_list<matchlet> html_pattern3 = { { 0, 0, "<!--" } };
static const std::initializer_list<magic> html_magic = { { html_pattern1 }, { html_pattern2 }, { html_pattern3 } };
static const std::initializer_list<std::string> html_globs = { "*.html", "*.htm" };
static const mime_info html_data = { html_magic, "", "html", _("html document"), html_globs, {} };

static const std::initializer_list<matchlet> http_pattern1 = { { 0, 0, "HTTP/1.0" } };
static const std::initializer_list<matchlet> http_pattern2 = { { 0, 0, "HTTP/1.1" } };
static const std::initializer_list<magic> http_magic = { { http_pattern1 }, { http_pattern2 } };
static const mime_info http_data = { http_magic, "", "", "", {}, {} };

static const std::initializer_list<std::string> mhtml_globs = { "*.mht" };
static const mime_info mhtml_data = { {}, "", "", _("mhtml document"), mhtml_globs, {} };

static const std::initializer_list<matchlet> mime_pattern1 = { { 0,  4, "Content-Type: " } }; // for multipart mime documents (e.g. mhtml)
static const std::initializer_list<matchlet> mime_pattern2 = { { 0, 80, "MIME-Version: 1.0" }, { 18, 80, "Content-Type: " } };
static const std::initializer_list<magic> mime_magic = { { mime_pattern1 }, { mime_pattern2 } };
static const mime_info mime_data = { mime_magic, "", "", "", {}, {} };

static const std::initializer_list<std::string> ncx_globs = { "*.ncx" };
static const mime_info ncx_data = { {}, "http://www.daisy.org/z3986/2005/ncx/", "ncx", _("navigation control document"), ncx_globs, {} };

static const std::initializer_list<std::string> ogg_aliases = { "audio/ogg" };
static const std::initializer_list<std::string> ogg_globs = { "*.ogg" };
static const mime_info ogg_data = { {}, "", "", _("ogg vorbis audio"), ogg_globs, ogg_aliases };

static const std::initializer_list<std::string> opf_globs = { "*.opf" };
static const mime_info opf_data = { {}, "http://www.idpf.org/2007/opf", "package", _("open package format document"), opf_globs, {} };

static const std::initializer_list<std::string> rdfxml_globs = { "*.rdf" };
static const mime_info rdfxml_data = { {}, "http://www.w3.org/1999/02/22-rdf-syntax-ns", "RDF", _("RDF/XML document"), rdfxml_globs, {} };

static const std::initializer_list<matchlet> rtf_pattern = { { 0, 0, "{\\rtf" } };
static const std::initializer_list<magic> rtf_magic = { { rtf_pattern } };
static const std::initializer_list<std::string> rtf_aliases = { "text/rtf" };
static const std::initializer_list<std::string> rtf_globs = { "*.rtf" };
static const mime_info rtf_data = { rtf_magic, "", "", _("rich text document"), rtf_globs, rtf_aliases };

static const std::initializer_list<std::string> smil_globs = { "*.smil" };
static const mime_info smil_data = { {}, "http://www.w3.org/ns/SMIL", "smil", _("SMIL document"), smil_globs, {} };

static const std::initializer_list<std::string> ssml_globs = { "*.ssml" };
static const mime_info ssml_data = { {}, "http://www.w3.org/2001/10/synthesis", "speak", _("speech synthesis markup document"), ssml_globs, {} };

static const std::initializer_list<std::string> text_globs = { "*.txt" };
static const mime_info text_data = { {}, "", "", _("plain text document"), text_globs, {} };

static const std::initializer_list<std::string> wav_aliases = { "audio/vnd.wav", "audio/wav", "audio/wave" };
static const std::initializer_list<std::string> wav_globs = { "*.wav" };
static const mime_info wav_data = { {}, "", "", _("wave audio"), wav_globs, wav_aliases };

static const std::initializer_list<std::string> xhtml_globs = { "*.xhtml", "*.xht" };
static const mime_info xhtml_data = { {}, "http://www.w3.org/1999/xhtml", "html", _("xhtml document"), xhtml_globs, {} };

static const std::initializer_list<matchlet> xml_pattern = { { 0, 0, "<?xml version=" } };
static const std::initializer_list<magic> xml_magic = { { xml_pattern } };
static const mime_info xml_data = { xml_magic, "", "", "", {}, {} };

//@}

const m::mimetype m::email( "email", "message/rfc822",                &email_data);
const m::mimetype m::epub(  "epub",  "application/epub+zip",          &epub_data);
const m::mimetype m::gzip(  "gzip",  "application/x-gzip",            &gzip_data);
const m::mimetype m::html(  "html",  "text/html",                     &html_data);
const m::mimetype m::http(  "http",  NULL,                            &http_data);
const m::mimetype m::mhtml( "mhtml", "multipart/related",             &mhtml_data);
const m::mimetype m::mime(  "mime",  NULL,                            &mime_data);
const m::mimetype m::ncx(   "ncx",   "application/x-dtbncx+xml",      &ncx_data);
const m::mimetype m::ogg(   "ogg",   "application/ogg",               &ogg_data);
const m::mimetype m::opf(   "opf",   "application/oebps-package+xml", &opf_data);
const m::mimetype m::rdfxml("rdf",   "application/rdf+xml",           &rdfxml_data);
const m::mimetype m::rtf(   "rtf",   "application/rtf",               &rtf_data);
const m::mimetype m::smil(  "smil",  "application/smil",              &smil_data);
const m::mimetype m::ssml(  "ssml",  "application/ssml+xml",          &ssml_data);
const m::mimetype m::text(  "text",  "text/plain",                    &text_data);
const m::mimetype m::wav(   "wav",   "audio/x-wav",                   &wav_data);
const m::mimetype m::xhtml( "xhtml", "application/html+xml",          &xhtml_data);
const m::mimetype m::xml(   "xml",   "application/xml",               &xml_data);
