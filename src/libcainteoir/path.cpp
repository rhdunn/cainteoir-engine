/* Path API.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/path.hpp>

cainteoir::path &cainteoir::path::operator/=(const char *aPath)
{
	if (mPath.back() != '/' && *aPath != '/')
		mPath.push_back('/');
	mPath += aPath;
	return *this;
}

cainteoir::path cainteoir::path::parent() const
{
	return path(mPath.substr(0, mPath.rfind('/')));
}

cainteoir::path cainteoir::get_data_path()
{
	const char *datadir = getenv("CAINTEOIR_DATA_DIR");
	if (!datadir)
		datadir = DATADIR "/" PACKAGE;
	return path(datadir);
}
