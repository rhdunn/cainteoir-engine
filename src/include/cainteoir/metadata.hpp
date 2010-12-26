/* MetaData API.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_METADATA_HPP
#define CAINTEOIR_ENGINE_METADATA_HPP

#include <string>

namespace cainteoir { namespace rdf
{
	/** @brief RDF URI resource
	  */
	class uri
	{
	public:
		const std::string value; /**< @brief The full path of the URI resource. */
		const std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		const std::string ref;   /**< @brief The URI reference. */

		uri(const std::string &aNS, const std::string &aRef)
			: value(aNS + aRef)
			, ns(aNS)
			, ref(aRef)
		{
		}
	};

	/** @brief rdf namespace node
	  *
	  * @param aRef The URI reference beonging to the rdf resource namespace.
	  */
	inline const uri rdf(const std::string &aRef)
	{
		return uri("http://www.w3.org/1999/02/22-rdf-syntax-ns#", aRef);
	}

	/** @brief rdfs namespace node
	  *
	  * @param aRef The URI reference beonging to the rdfs resource namespace.
	  */
	inline const uri rdfs(const std::string &aRef)
	{
		return uri("http://www.w3.org/2000/01/rdf-schema#", aRef);
	}

	/** @brief xsd namespace node
	  *
	  * @param aRef The URI reference beonging to the xsd (XMLSchema) resource namespace.
	  */
	inline const uri xsd(const std::string &aRef)
	{
		return uri("http://www.w3.org/2001/XMLSchema#", aRef);
	}

	/** @brief dc namespace node
	  *
	  * @param aRef The URI reference beonging to the dc (Dublin Core: Elements) resource namespace.
	  */
	inline const uri dc(const std::string &aRef)
	{
		return uri("http://purl.org/dc/elements/1.1/", aRef);
	}

	/** @brief dcterms namespace node
	  *
	  * @param aRef The URI reference beonging to the dcterms (Dublin Core: Terms) resource namespace.
	  */
	inline const uri dcterms(const std::string &aRef)
	{
		return uri("http://purl.org/dc/terms/", aRef);
	}
}}

namespace cainteoir
{
	enum metadata_tag
	{
		/** @brief The title of the document, chapter, voice, text-to-speech engine, etc. */
		title,

		/** @brief The number of audio channels (mono = 1, stereo = 2). */
		channels,

		/** @brief The frequency (in Hz) of the audio data. */
		frequency,

		/** @brief The format of the audio data: S16_LE, S16_BE, S32_LE, etc. */
		audio_format,
	};

	struct metadata
	{
		virtual ~metadata() {}

		virtual const char *get_metadata(metadata_tag tag) const = 0;
	};
}

#endif
