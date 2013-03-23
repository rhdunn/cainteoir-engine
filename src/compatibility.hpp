/* Language and System Compatibility Helpers.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_COMPATIBILITY_HPP
#define CAINTEOIR_ENGINE_COMPATIBILITY_HPP

#include <stddef.h>

#ifndef HAVE_CXX11_STD_BEGIN
	namespace std
	{
		template <typename C>
		inline auto begin(C &c) -> decltype(c.begin())
		{
			return c.begin();
		}

		template <typename C>
		inline auto begin(const C &c) -> decltype(c.begin())
		{
			return c.begin();
		}

		template <typename T, size_t N>
		inline const T *begin(T (&a)[N])
		{
			return a;
		}
	}
#endif

#ifndef HAVE_CXX11_STD_END
	namespace std
	{
		template <typename C>
		inline auto end(C &c) -> decltype(c.end())
		{
			return c.end();
		}

		template <typename C>
		inline auto end(const C &c) -> decltype(c.end())
		{
			return c.end();
		}

		template <typename T, size_t N>
		inline const T *end(T (&a)[N])
		{
			return a + N;
		}
	}
#endif

#endif
