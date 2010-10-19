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

namespace cainteoir
{
	namespace dc
	{
		static const char *title = "http://purl.org/dc/elements/1.1/title";
	}

	namespace dcterms
	{
		static const char *title = "http://purl.org/dc/terms/title";
	}

	struct metadata
	{
		virtual ~metadata() {}

		virtual std::string get_metadata(const char *uri) const = 0;
	};
}

#endif
