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
	struct mimetype
	{
		/** @brief The name of this mimetype/content. */
		const char *name;

		/** @brief The primary mimetype string. */
		const char *mime_type;

		mimetype(const char *aName, const char *aMimeType, const void *aInfo)
			: name(aName)
			, mime_type(aMimeType)
			, info(aInfo)
		{
		}

		bool match(const std::tr1::shared_ptr<cainteoir::buffer> &buffer) const;

		bool match(const std::string &uri, const std::string &name) const;

		void metadata(rdf::graph &aGraph, const std::string &baseuri, const rdf::uri &type) const;
	private:
		/** @brief Internal mimetype information. */
		const void *info;
	};

	/** @name Mime Data
	  * @brief The information for document and audio mimetypes used by the engine.
	  */
	//@{

	extern const mimetype email;  /**< @brief email mbox (mime) document */
	extern const mimetype epub;   /**< @brief electronic publication document */
	extern const mimetype gzip;   /**< @brief gzip compressed document */
	extern const mimetype html;   /**< @brief html document */
	extern const mimetype mhtml;  /**< @brief single-page html document */
	extern const mimetype mime;   /**< @brief mime/http headers */
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
