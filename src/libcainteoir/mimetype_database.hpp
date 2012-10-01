/* Shared Mime Type Information Database Support.
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

#ifndef CAINTEOIR_ENGINE_MIMETYPE_DATABASE_HPP
#define CAINTEOIR_ENGINE_MIMETYPE_DATABASE_HPP

#include <cainteoir/buffer.hpp>

#include <netinet/in.h> // for ntohs and ntohl
#include <vector>
#include <map>

namespace cainteoir { namespace mime
{
	static const char *email_mimetype  = "message/rfc822";
	static const char *epub_mimetype   = "application/epub+zip";
	static const char *gzip_mimetype   = "application/x-gzip";
	static const char *html_mimetype   = "text/html";
	static const char *mhtml_mimetype  = "multipart/related";
	static const char *ncx_mimetype    = "application/x-dtbncx+xml";
	static const char *ogg_mimetype    = "audio/x-vorbis+ogg";
	static const char *opf_mimetype    = "application/oebps-package+xml";
	static const char *pdf_mimetype    = "application/pdf";
	static const char *rdfxml_mimetype = "application/rdf+xml";
	static const char *rtf_mimetype    = "application/rtf";
	static const char *smil_mimetype   = "application/smil";
	static const char *ssml_mimetype   = "application/ssml+xml";
	static const char *text_mimetype   = "text/plain";
	static const char *wav_mimetype    = "audio/x-wav";
	static const char *xhtml_mimetype  = "application/xhtml+xml";
	static const char *xml_mimetype    = "application/xml";
	static const char *zip_mimetype    = "application/zip";

	struct matchlet
	{
		/** @brief The position where the pattern starts. */
		uint32_t offset;

		/** @brief The number of bytes after offset the pattern can occur. */
		uint32_t range;

		/** @brief The pattern to match against. */
		std::string pattern;

		matchlet &operator=(const matchlet &m);

		bool match(const std::shared_ptr<cainteoir::buffer> &data) const;
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

		bool match(const std::shared_ptr<cainteoir::buffer> &data) const;
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

	struct mime_cache
	{
		mime_cache(const std::string &filename)
			: cache(cainteoir::make_file_buffer(filename.c_str()))
		{
		}

		uint16_t u16(uint32_t offset) const { return ntohs(*(uint16_t *)(cache->begin() + offset)); }
		uint32_t u32(uint32_t offset) const { return ntohl(*(uint32_t *)(cache->begin() + offset)); }

		const char *str(uint32_t offset) const { return cache->begin() + u32(offset); }

		uint16_t major() const { return u16(0); }
		uint16_t minor() const { return u16(2); }
	private:
		std::shared_ptr<cainteoir::buffer> cache;
	};

	class mimetype_database
	{
		std::map<std::string, mime_info> database;

		void read_aliases_from_cache(mime_cache &cache);

		void read_reverse_suffix_tree_from_cache(mime_cache &cache, uint32_t offset, const std::string &suffix);

		void read_globs_from_cache(mime_cache &cache);

		void read_matchlets_from_cache(mime_cache &cache, uint32_t count, uint32_t offset, std::vector<magic> &magic_list, const std::vector<matchlet> &matchlets);

		void read_magic_from_cache(mime_cache &cache);

		void read_xmlns_from_cache(mime_cache &cache);

		std::string read_comment_from_mimeinfo_file(const std::string &filename);
	public:
		mimetype_database();

		const mime_info &operator[](const char *mimetype) const;
	};

	extern mimetype_database mimetypes;

	extern const mime_info mime_data;
}}

/** References
  *
  * 	1.	http://standards.freedesktop.org/shared-mime-info-spec/0.20/
  */

#endif
