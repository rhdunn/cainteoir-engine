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

#ifndef CAINTEOIR_ENGINE_MIMETYPE_HPP
#define CAINTEOIR_ENGINE_MIMETYPE_HPP

#include "metadata.hpp"

namespace cainteoir { namespace mime
{
	struct mimetype
	{
		const char *name;
		const char *mime_type;

		mimetype(const char *aName, const char *aMimeType, const void *aInfo = nullptr)
			: name(aName)
			, mime_type(aMimeType)
			, info(aInfo)
		{
		}

		bool match(const std::shared_ptr<cainteoir::buffer> &data) const;

		bool match(const std::string &uri, const std::string &name) const;

		void metadata(rdf::graph &aGraph, const std::string &baseuri, const rdf::uri &type) const;
	private:
		const void *info;
	};

	extern const mimetype cainteoir;
	extern const mimetype cmudict;
	extern const mimetype email;
	extern const mimetype epub;
	extern const mimetype gzip;
	extern const mimetype html;
	extern const mimetype mhtml;
	extern const mimetype mime;
	extern const mimetype ncx;
	extern const mimetype ogg;
	extern const mimetype opf;
	extern const mimetype pdf;
	extern const mimetype rdfxml;
	extern const mimetype rtf;
	extern const mimetype smil;
	extern const mimetype ssml;
	extern const mimetype text;
	extern const mimetype wav;
	extern const mimetype xhtml;
	extern const mimetype xml;
	extern const mimetype zip;
}}

#endif
