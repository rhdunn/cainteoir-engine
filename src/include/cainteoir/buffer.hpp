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
#include <strings.h>
#include <tr1/memory>
#include <list>

namespace cainteoir
{
	class buffer
	{
	protected:
		const char *first;
		const char *last;
	public:
		buffer(const char *f, const char *l) : first(f), last(l) {}
		buffer(const char *f) : first(f), last(f+strlen(f)) {}
		virtual ~buffer() {}

		typedef const char * iterator;
		typedef std::size_t size_type;

		iterator begin() const { return first; }
		iterator end()   const { return last; }

		size_type size() const { return last - first; }

		bool empty() const { return first == last; }

		int compare(const char *str) const { return compare(str, strlen(str), strncmp); }

		int compare(const buffer & str) const { return compare(str.begin(), str.size(),  strncmp); }

		int comparei(const char *str) const { return compare(str, strlen(str), strncasecmp); }

		int comparei(const buffer & str) const { return compare(str.begin(), str.size(), strncasecmp); }

		std::string str() const
		{
			if (first == last || *first == '\0') return std::string();
			return std::string(first, last);
		}
	private:
		int compare(const char *str, int len, int (*cmp)(const char *s1, const char *s2, size_t n)) const
		{
			int n = cmp(str, first, size());
			if (n == 0)
				return -(size() - len);
			return n;
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

	class rope
	{
		std::list< std::tr1::shared_ptr<cainteoir::buffer> > data;
		std::size_t len;
	public:
		rope(): len(0) {}

		std::size_t size() const { return len; }

		bool empty() const { return len == 0; }

		void clear()
		{
			data.clear();
			len = 0;
		}

		rope &operator=(const std::tr1::shared_ptr<cainteoir::buffer> &item)
		{
			data.clear();
			add(item);
			return *this;
		}

		void add(const std::tr1::shared_ptr<cainteoir::buffer> &item)
		{
			data.push_back(item);
			len += item->size();
		}

		std::tr1::shared_ptr<cainteoir::buffer> buffer() const
		{
			if (data.size() == 0)
				return std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(NULL, NULL));

			std::tr1::shared_ptr<cainteoir::buffer> temp(new cainteoir::data_buffer(len));
			char * startPos = (char *)temp->begin();
			for (auto node = data.begin(), last = data.end(); node != last; ++node)
			{
				memcpy(startPos, (*node)->begin(), (*node)->size());
				startPos += (*node)->size();
			}

			*const_cast<rope *>(this) = temp;
			return temp;
		}

		std::string str() const { return buffer()->str(); }
	};
}

#endif
