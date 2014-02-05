/* Path API.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_PATH_HPP
#define CAINTEOIR_ENGINE_PATH_HPP

#include <string>

namespace cainteoir
{
	struct path
	{
		path() {}

		explicit path(const std::string &aPath)
			: mPath(aPath)
		{
		}

		explicit path(const char *aPath)
			: mPath(aPath ? aPath : "")
		{
		}

		path &operator/=(const char *aPath);
		path &operator/=(const std::string &aPath);

		operator const char *() const { return mPath.c_str(); }

		const std::string &str() const { return mPath; }

		std::string::size_type size()  const { return mPath.size(); }

		bool empty() const { return mPath.empty(); }

		path parent() const;

		path zip_file() const;
		path zip_path() const;
	private:
		std::string mPath;
	};

	inline path operator/(const path &a, const char *b)
	{
		path temp(a);
		temp /= b;
		return temp;
	}

	inline path operator/(const path &a, const std::string &b)
	{
		path temp(a);
		temp /= b;
		return temp;
	}

	path get_data_path();

	FILE *create_temp_file(const char *mode);
}

#endif
