/* Language Mapping Support.
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

#ifndef CAINTEOIR_ENGINE_LANGUAGES_HPP
#define CAINTEOIR_ENGINE_LANGUAGES_HPP

#include <string.h>
#include <string>
#include <map>

namespace cainteoir
{
	namespace ci /**< @name Case Insensitive Operations */
	{
		struct less
		{
			bool operator()(const std::string &a, const std::string &b)
			{
				return strcasecmp(a.c_str(), b.c_str()) < 0;
			}
		};
	}

	class languages
	{
	public:
		languages(const char * locale);

		std::string operator()(const std::string & langid);
	private:
		std::map<std::string, std::string, ci::less> m_languages;
	};
}

#endif
