/* ZIP File API.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_ZIP_HPP
#define CAINTEOIR_ENGINE_ZIP_HPP

#include <cainteoir/buffer.hpp>
#include <cainteoir/metadata.hpp>
#include <map>

namespace cainteoir
{
	struct archive
	{
		virtual ~archive() {}

		virtual const rdf::uri location(const std::string &aFilename, const std::string &aRef) const = 0;

		virtual std::shared_ptr<buffer> read(const char *aFilename) const = 0;
	};

	std::shared_ptr<archive> create_zip_archive(std::shared_ptr<buffer> aData, const rdf::uri &aSubject);
}

#endif
