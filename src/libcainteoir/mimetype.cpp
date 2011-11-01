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

bool cainteoir::mime::matchlet::match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
{
	const char *begin = data->begin() + offset;
	for (const char *start = begin; start != begin + range + 1; ++start)
	{
		if (start + pattern_length >= data->end())
			return false; // out of range!

		if (!strncmp(start, pattern, pattern_length))
			return true;
	}
	return false;
}

bool cainteoir::mime::magic::match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
{
	for (const cainteoir::mime::matchlet *matchlet = matchlets; matchlet != matchlets + length; ++matchlet)
	{
		if (!matchlet->match(data))
			return false;
	}

	return true;
}

bool cainteoir::mime::mimetype::match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const
{
	for (const cainteoir::mime::magic *matchlet = magic; matchlet != magic + magic_length; ++matchlet)
	{
		if (matchlet->match(data))
			return true;
	}

	return false;
}

bool cainteoir::mime::mimetype::match(const std::string &uri, const std::string &name) const
{
	if (xmlns)
	{
		if (!(uri == xmlns || uri == std::string(xmlns) + "#"))
			return false;
	}
	return localname == name;
}

void cainteoir::mime::mimetype::metadata(rdf::graph &aGraph, const std::string &baseuri, const rdf::uri &type) const
{
	rdf::uri ref = rdf::uri(baseuri, name);
	aGraph.statement(ref, rdf::rdf("type"), type);
	aGraph.statement(ref, rdf::tts("name"), rdf::literal(name));
	aGraph.statement(ref, rdf::dc("title"), rdf::literal(label));
	aGraph.statement(ref, rdf::dc("description"), rdf::literal(label));
	aGraph.statement(ref, rdf::tts("mimetype"), rdf::literal(mimetype));
	if (aliases)
	{
		for (const char **alias = aliases; *alias; ++alias)
			aGraph.statement(ref, rdf::tts("mimetype"), rdf::literal(*alias));
	}
	for (const char **glob = globs; *glob; ++glob)
		aGraph.statement(ref, rdf::tts("extension"), rdf::literal(*glob));
}

namespace m = cainteoir::mime;

/** @name Mime Data
  *
  * This will be refactored to load the mime data from the shared-mime-info database.
  */
//@{

static const m::matchlet email_pattern1[] = { { 0, 0, 6, "From: " } };
static const m::matchlet email_pattern2[] = { { 0, 0, 9, "Subject: " } };
static const m::magic    email_magic[] = { { 1, email_pattern1 }, { 1, email_pattern2 } };
static const char *      email_aliases[] = { "text/x-mail", NULL };
static const char *      email_globs[] = { "*.eml", "*.emlx", "*.msg", "*.mbx", NULL };

static const m::matchlet epub_pattern[] = { { 0, 0, 2, "PK" }, { 30, 0, 8, "mimetype" }, { 38, 0, 20, "application/epub+zip" } };
static const m::magic    epub_magic[] = { { 3, epub_pattern } };
static const char *      epub_globs[] = { "*.epub", NULL };

static const m::matchlet gzip_pattern[] = { { 0, 0, 2, "\037\213" } };
static const m::magic    gzip_magic[] = { { 1, gzip_pattern } };
static const char *      gzip_globs[] = { "*.gz", NULL };

static const m::matchlet html_pattern1[] = { { 0, 0, 5, "<html" } };
static const m::matchlet html_pattern2[] = { { 0, 0, 5, "<HTML" } };
static const m::matchlet html_pattern3[] = { { 0, 0, 4, "<!--" } };
static const m::magic    html_magic[] = { { 1, html_pattern1 }, { 1, html_pattern2 }, { 1, html_pattern3 } };
static const char *      html_globs[] = { "*.html", "*.htm", NULL };

static const m::matchlet http_pattern1[] = { { 0, 0, 8, "HTTP/1.0" } };
static const m::matchlet http_pattern2[] = { { 0, 0, 8, "HTTP/1.1" } };
static const m::magic    http_magic[] = { { 1, http_pattern1 }, { 1, http_pattern2 } };

static const char * mhtml_globs[] = { "*.mht", NULL };

static const m::matchlet mime_pattern1[] = { { 0,  4, 14, "Content-Type: " } }; // for multipart mime documents (e.g. mhtml)
static const m::matchlet mime_pattern2[] = { { 0, 80, 17, "MIME-Version: 1.0" }, { 18, 80, 14, "Content-Type: " } };
static const m::magic    mime_magic[] = { { 1, mime_pattern1 }, { 2, mime_pattern2 } };

static const char * ncx_globs[] = { "*.ncx", NULL };

static const char * ogg_aliases[] = { "audio/ogg", NULL };
static const char * ogg_globs[] = { "*.ogg", NULL };

static const char * opf_globs[] = { "*.opf", NULL };

static const char * rdfxml_globs[] = { "*.rdf", NULL };

static const m::matchlet rtf_pattern[] = { { 0, 0, 5, "{\\rtf" } };
static const m::magic    rtf_magic[] = { { 1, rtf_pattern } };
static const char *      rtf_aliases[] = { "text/rtf", NULL };
static const char *      rtf_globs[] = { "*.rtf", NULL };

static const char * smil_globs[] = { "*.smil", NULL };

static const char * ssml_globs[] = { "*.ssml", NULL };

static const char * text_globs[] = { "*.txt", NULL };

static const char * wav_aliases[] = { "audio/vnd.wav", "audio/wav", "audio/wave", NULL };
static const char * wav_globs[] = { "*.wav", NULL };

static const char * xhtml_globs[] = { "*.xhtml", "*.xht", NULL };

static const m::matchlet xml_pattern[] = { { 0, 0, 14, "<?xml version=" } };
static const m::magic    xml_magic[] = { { 1, xml_pattern } };

//@}

const m::mimetype m::email = { "email", "message/rfc822", 2, email_magic, NULL, NULL, _("electronic mail document"), email_globs, email_aliases };

const m::mimetype m::epub = { "epub", "application/epub+zip", 1, epub_magic, NULL, NULL, _("electronic publication document"), epub_globs, NULL };

const m::mimetype m::gzip = { "gzip", "application/x-gzip", 1, gzip_magic, NULL, NULL, _("gzip compressed document"), gzip_globs, NULL };

const m::mimetype m::html = { "html", "text/html", 3, html_magic, NULL, "html", _("html document"), html_globs, NULL };

const m::mimetype m::http = { "http", NULL, 2, http_magic, NULL, NULL };

const m::mimetype m::mhtml = { "mhtml", "multipart/related", 0, NULL, NULL, NULL, _("mhtml document"), mhtml_globs, NULL };

const m::mimetype m::mime = { "mime", NULL, 2, mime_magic, NULL, NULL };

const m::mimetype m::ncx = { "ncx", "application/x-dtbncx+xml", 0, NULL, "http://www.daisy.org/z3986/2005/ncx/", "ncx", _("navigation control document"), ncx_globs, NULL };

const m::mimetype m::ogg = { "ogg", "application/ogg", 0, NULL, NULL, NULL, _("ogg vorbis audio"), ogg_globs, ogg_aliases };

const m::mimetype m::opf = { "opf", "application/oebps-package+xml", 0, NULL, "http://www.idpf.org/2007/opf", "package", _("open package format document"), opf_globs, NULL };

const m::mimetype m::rdfxml = { "rdf", "application/rdf+xml", 0, NULL, "http://www.w3.org/1999/02/22-rdf-syntax-ns", "RDF", _("RDF/XML document"), rdfxml_globs, NULL };

const m::mimetype m::rtf = { "rtf", "application/rtf", 1, rtf_magic, NULL, NULL, _("rich text document"), rtf_globs, rtf_aliases };

const m::mimetype m::smil = { "smil", "application/smil", 0, NULL, "http://www.w3.org/ns/SMIL", "smil", _("SMIL document"), smil_globs, NULL };

const m::mimetype m::ssml = { "ssml", "application/ssml+xml", 0, NULL, "http://www.w3.org/2001/10/synthesis", "speak", _("speech synthesis markup document"), ssml_globs, NULL };

const m::mimetype m::text = { "text", "text/plain", 0, NULL, NULL, NULL, _("plain text document"), text_globs, NULL };

const m::mimetype m::wav = { "wav", "audio/x-wav", 0, NULL, NULL, NULL, _("wave audio"), wav_globs, wav_aliases };

const m::mimetype m::xhtml = { "xhtml", "application/html+xml", 0, NULL, "http://www.w3.org/1999/xhtml", "html", _("xhtml document"), xhtml_globs, NULL };

const m::mimetype m::xml = { "xml", "application/xml", 1, xml_magic, NULL, NULL };
