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
#include <map>

static const char *email_mimetype  = "message/rfc822";
static const char *epub_mimetype   = "application/epub+zip";
static const char *gzip_mimetype   = "application/x-gzip";
static const char *html_mimetype   = "text/html";
static const char *mhtml_mimetype  = "multipart/related";
static const char *ncx_mimetype    = "application/x-dtbncx+xml";
static const char *ogg_mimetype    = "application/ogg";
static const char *opf_mimetype    = "application/oebps-package+xml";
static const char *rdfxml_mimetype = "application/rdf+xml";
static const char *rtf_mimetype    = "application/rtf";
static const char *smil_mimetype   = "application/smil";
static const char *ssml_mimetype   = "application/ssml+xml";
static const char *text_mimetype   = "text/plain";
static const char *wav_mimetype    = "audio/x-wav";
static const char *xhtml_mimetype  = "application/html+xml";
static const char *xml_mimetype    = "application/xml";

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

struct magic : public std::vector<matchlet>
{
	magic(const std::initializer_list<matchlet> &matchlets)
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
		return true;
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

struct mimetype_database : public std::map<std::string, mime_info>
{
	mimetype_database()
	{
		/** @name Mime Data
		  *
		  * This will be refactored to load the mime data from the shared-mime-info database.
		  */
		//@{

		const std::initializer_list<matchlet> email_pattern1 = { { 0, 0, "From: " } };
		const std::initializer_list<matchlet> email_pattern2 = { { 0, 0, "Subject: " } };
		(*this)[email_mimetype].label = _("electronic mail document");
		(*this)[email_mimetype].globs = { "*.eml", "*.emlx", "*.msg", "*.mbx" };
		(*this)[email_mimetype].aliases = { "text/x-mail" };
		(*this)[email_mimetype].magic = { email_pattern1, email_pattern2 };

		const std::initializer_list<matchlet> epub_pattern = { { 0, 0, "PK" }, { 30, 0, "mimetype" }, { 38, 0, "application/epub+zip" } };
		(*this)[epub_mimetype].label = _("electronic publication document");
		(*this)[epub_mimetype].globs = { "*.epub" };
		(*this)[epub_mimetype].magic = { epub_pattern };

		const std::initializer_list<matchlet> gzip_pattern = { { 0, 0, "\037\213" } };
		(*this)[gzip_mimetype].label = _("gzip compressed document");
		(*this)[gzip_mimetype].globs = { "*.gz" };
		(*this)[gzip_mimetype].magic = { gzip_pattern };

		const std::initializer_list<matchlet> html_pattern1 = { { 0, 0, "<html" } };
		const std::initializer_list<matchlet> html_pattern2 = { { 0, 0, "<HTML" } };
		const std::initializer_list<matchlet> html_pattern3 = { { 0, 0, "<!--" } };
		(*this)[html_mimetype].localname = "html";
		(*this)[html_mimetype].label = _("html document");
		(*this)[html_mimetype].globs = { "*.html", "*.htm" };
		(*this)[html_mimetype].magic = { html_pattern1, html_pattern2, html_pattern3 };

		(*this)[mhtml_mimetype].label = _("mhtml document");
		(*this)[mhtml_mimetype].globs = { "*.mht" };

		(*this)[ncx_mimetype].xmlns = "http://www.daisy.org/z3986/2005/ncx/";
		(*this)[ncx_mimetype].localname = "ncx";
		(*this)[ncx_mimetype].label = _("navigation control document");
		(*this)[ncx_mimetype].globs = { "*.ncx" };

		(*this)[ogg_mimetype].label = _("ogg vorbis audio");
		(*this)[ogg_mimetype].globs = { "*.ogg" };
		(*this)[ogg_mimetype].aliases = { "audio/ogg" };

		(*this)[opf_mimetype].xmlns = "http://www.idpf.org/2007/opf";
		(*this)[opf_mimetype].localname = "package";
		(*this)[opf_mimetype].label = _("open package format document");
		(*this)[opf_mimetype].globs = { "*.opf" };

		(*this)[rdfxml_mimetype].xmlns = "http://www.w3.org/1999/02/22-rdf-syntax-ns";
		(*this)[rdfxml_mimetype].localname = "RDF";
		(*this)[rdfxml_mimetype].label = _("RDF/XML document");
		(*this)[rdfxml_mimetype].globs = { "*.rdf" };

		const std::initializer_list<matchlet> rtf_pattern = { { 0, 0, "{\\rtf" } };
		(*this)[rtf_mimetype].label = _("rich text document");
		(*this)[rtf_mimetype].globs = { "*.rtf" };
		(*this)[rtf_mimetype].aliases = { "text/rtf" };
		(*this)[rtf_mimetype].magic = { rtf_pattern };

		(*this)[smil_mimetype].xmlns = "http://www.w3.org/ns/SMIL";
		(*this)[smil_mimetype].localname = "smil";
		(*this)[smil_mimetype].label = _("SMIL document");
		(*this)[smil_mimetype].globs = { "*.smil" };

		(*this)[ssml_mimetype].xmlns = "http://www.w3.org/2001/10/synthesis";
		(*this)[ssml_mimetype].localname = "speak";
		(*this)[ssml_mimetype].label = _("speech synthesis markup document");
		(*this)[ssml_mimetype].globs = { "*.ssml" };

		(*this)[text_mimetype].label = _("plain text document");
		(*this)[text_mimetype].globs = { "*.txt" };

		(*this)[wav_mimetype].label = _("wave audio");
		(*this)[wav_mimetype].globs = { "*.wav" };
		(*this)[wav_mimetype].aliases = { "audio/vnd.wav", "audio/wav", "audio/wave" };

		(*this)[xhtml_mimetype].xmlns = "http://www.w3.org/1999/xhtml";
		(*this)[xhtml_mimetype].localname = "html";
		(*this)[xhtml_mimetype].label = _("xhtml document");
		(*this)[xhtml_mimetype].globs = { "*.xhtml", "*.xht" };

		const std::initializer_list<matchlet> xml_pattern = { { 0, 0, "<?xml version=" } };
		(*this)[xml_mimetype].magic = { xml_pattern };

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

static const std::initializer_list<matchlet> http_pattern1 = { { 0,  0, "HTTP/1.0" },          {  8, 80, "\nContent-Type:" } };
static const std::initializer_list<matchlet> http_pattern2 = { { 0,  0, "HTTP/1.1" },          {  8, 80, "\nContent-Type:" } };
static const std::initializer_list<matchlet> mime_pattern1 = { { 0, 80, "MIME-Version: 1.0" }, { 16, 80, "\nContent-Type:" } }; // for multipart/mhtml documents
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
