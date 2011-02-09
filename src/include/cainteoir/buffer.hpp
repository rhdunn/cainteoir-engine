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

#include <cstring>
#include <string>
#include <tr1/memory>

namespace cainteoir
{
	class buffer
	{
	protected:
		const char *first;
		const char *last;
	public:
		buffer(const char *f, const char *l) : first(f), last(l) {}
		buffer(const char *f) : first(f), last(f+strlen(f)+1) {}
		virtual ~buffer() {}

		typedef const char * iterator;
		typedef std::size_t size_type;

		iterator begin() const { return first; }
		iterator end()   const { return last; }

		size_type size() const { return last - first; }

		bool empty() const { return first == last; }

		std::string str() const
		{
			if (first == last || *first == '\0') return std::string();
			return std::string(first, last);
		}
	};

	class range_buffer : public buffer
	{
		std::tr1::shared_ptr<cainteoir::buffer> anchor;
	public:
		range_buffer(const std::tr1::shared_ptr<cainteoir::buffer> &a, const char *f, const char *l) : buffer(f, l), anchor(a) {}
		range_buffer(const std::tr1::shared_ptr<cainteoir::buffer> &a, const char *f) : buffer(f, a->end()), anchor(a) {}
		range_buffer(const std::tr1::shared_ptr<cainteoir::buffer> &a, const cainteoir::buffer &range) : buffer(range.begin(), range.end()), anchor(a) {}
	};

	class mmap_buffer : public buffer
	{
		int fd;
	public:
		mmap_buffer(const char *path);
		~mmap_buffer();
	};

	class data_buffer : public buffer
	{
	public:
		data_buffer(unsigned int aSize);
		~data_buffer();
	};

	class xmlstring_buffer : public buffer
	{
	public:
		xmlstring_buffer(const char *str);
		~xmlstring_buffer();
	private:
		const char *data;
	};
}

#endif
