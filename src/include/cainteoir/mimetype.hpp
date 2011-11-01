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

#ifndef CAINTEOIR_ENGINE_MIMETYPE_HPP
#define CAINTEOIR_ENGINE_MIMETYPE_HPP

#include "buffer.hpp"
#include "metadata.hpp"

namespace cainteoir { namespace mime
{
	struct matchlet
	{
		/** @brief The position where the pattern starts. */
		uint32_t offset;

		/** @brief The number of bytes after offset the pattern can occur. */
		uint32_t range;

		/** @brief Number of bytes to match in the pattern string. */
		uint32_t pattern_length;

		/** @brief The pattern to match against. */
		const char *pattern;

		bool match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const;
	};

	struct magic
	{
		/** @brief The number of patterns to match against. */
		uint32_t length;

		/** @brief The patterns to match against. */
		const matchlet *matchlets;

		bool match(const std::tr1::shared_ptr<cainteoir::buffer> &data) const;
	};

	struct mimetype
	{
		/** @brief The name of this mimetype/content. */
		const char *name;

		/** @brief The primary mimetype string. */
		const char *mimetype;

		/** @brief The number of possible content identifiers. */
		uint32_t magic_length;

		/** @brief The magic data that identifies this mimetype/content. */
		const cainteoir::mime::magic *magic;

		/** @brief The XML namespace associated with the mimetype (for XML documents only). */
		const char *xmlns;

		/** @brief The XML local name associated with the mimetype (for XML documents only). */
		const char *localname;

		/** @brief The mimetype description (comment field). */
		const char *label;

		/** @brief The filename patterns for files matching this mimetype (null terminated). */
		const char **globs;

		/** @brief The mimetype aliases for this mimetype (null terminated). */
		const char **aliases;

		bool match(const std::tr1::shared_ptr<cainteoir::buffer> &buffer) const;

		bool match(const std::string &uri, const std::string &name) const;

		void metadata(rdf::graph &aGraph, const std::string &baseuri, const rdf::uri &type) const;
	};

	/** @name Mime Data
	  * @brief The information for document and audio mimetypes used by the engine.
	  */
	//@{

	extern const mimetype email;  /**< @brief email mbox (mime) document */
	extern const mimetype epub;   /**< @brief electronic publication document */
	extern const mimetype gzip;   /**< @brief gzip compressed document */
	extern const mimetype html;   /**< @brief html document */
	extern const mimetype http;   /**< @brief http (mime) headers */
	extern const mimetype mhtml;  /**< @brief single-page html document */
	extern const mimetype mime;   /**< @brief mime headers */
	extern const mimetype ncx;    /**< @brief navigation control document */
	extern const mimetype ogg;    /**< @brief Ogg+Vorbis audio */
	extern const mimetype opf;    /**< @brief open package format document */
	extern const mimetype rdfxml; /**< @brief RDF/XML document */
	extern const mimetype rtf;    /**< @brief rich text document */
	extern const mimetype smil;   /**< @brief smil document */
	extern const mimetype ssml;   /**< @brief speech synthesis markup document */
	extern const mimetype text;   /**< @brief plain text document */
	extern const mimetype wav;    /**< @brief wave audio */
	extern const mimetype xhtml;  /**< @brief xhtml document */
	extern const mimetype xml;    /**< @brief extensible markup document */

	//@}
}}

#endif
