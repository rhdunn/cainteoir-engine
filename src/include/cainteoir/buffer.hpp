/* Data Buffers Core API.
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

#ifndef CAINTEOIR_ENGINE_BUFFER_HPP
#define CAINTEOIR_ENGINE_BUFFER_HPP

#include <utility>

namespace cainteoir
{
	class buffer
	{
	protected:
		const char *first;
		const char *last;
	public:
		buffer(const char *f, const char *l) : first(f), last(l) {}
		virtual ~buffer() {}

		typedef const char * iterator;
		typedef std::size_t size_type;

		iterator begin() const { return first; }
		iterator end()   const { return last; }

		size_type size() const { return last - first; }

		bool empty() const { return first == last; }
	};

	class mmap_buffer : public buffer
	{
		int fd;
	public:
		mmap_buffer(const char *path);
		~mmap_buffer();
	};
}

#endif
