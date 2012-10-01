/* Data Buffers Core API.
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

#ifndef CAINTEOIR_ENGINE_BUFFER_HPP
#define CAINTEOIR_ENGINE_BUFFER_HPP

#include "range.hpp"

#include <cstring>
#include <string>
#include <memory>
#include <list>

namespace cainteoir
{
	class buffer : public range<const char *>
	{
	public:
		buffer(const char *f, const char *l) : range<const char *>(f, l) {}
		buffer(const char *f) : range<const char *>(f, f+strlen(f)) {}
		virtual ~buffer() {}

		typedef int (*match_type)(const char *s1, const char *s2, size_t n);
		static const match_type match_case;
		static const match_type ignore_case;

		bool startswith(const char *str, match_type match = match_case) const
		{
			int len = strlen(str);
			if (len > size()) return false;
			return match(str, first, len) == 0;
		}

		int compare(const char *str, match_type match = match_case) const
		{
			return compare(str, strlen(str), match);
		}

		int compare(const buffer &str, match_type match = match_case) const
		{
			return compare(str.begin(), str.size(), match);
		}

		int comparei(const char *str) const
		{
			return compare(str, strlen(str), ignore_case);
		}

		int comparei(const buffer &str) const
		{
			return compare(str.begin(), str.size(), ignore_case);
		}

		std::string str() const
		{
			if (first == last || *first == '\0') return std::string();
			return std::string(first, last);
		}
	private:
		int compare(const char *str, int len, match_type match) const
		{
			int n = match(str, first, size());
			if (n == 0)
				return -(size() - len);
			return n;
		}
	};

	class data_buffer : public buffer
	{
	public:
		data_buffer(unsigned int aSize);
		~data_buffer();
	};

	std::shared_ptr<buffer> make_buffer(const std::string &aString);

	std::shared_ptr<buffer> make_buffer(const char *aString, int aLength);

	std::shared_ptr<buffer> make_file_buffer(const char *path);

	std::shared_ptr<buffer> make_file_buffer(FILE *f);

	std::shared_ptr<buffer> normalize(const std::shared_ptr<buffer> &aBuffer);

	class rope
	{
		std::list<std::shared_ptr<cainteoir::buffer>> data;
		std::size_t len;
	public:
		rope(): len(0) {}

		std::size_t size() const { return len; }

		bool empty() const { return len == 0; }

		void clear();

		rope &operator=(const std::shared_ptr<cainteoir::buffer> &item);

		rope &operator+=(const std::shared_ptr<cainteoir::buffer> &item);

		std::shared_ptr<cainteoir::buffer> buffer() const;

		std::shared_ptr<cainteoir::buffer> content() const;

		std::shared_ptr<cainteoir::buffer> normalize() const
		{
			return cainteoir::normalize(buffer());
		}

		std::string str() const { return buffer()->str(); }
	};

	/** @name Decoding/Decompression API */
	//@{

	typedef std::shared_ptr<buffer> (*decoder_ptr)(const buffer &data, uint32_t size);

	std::shared_ptr<buffer> copy(const buffer &data, uint32_t size);

	std::shared_ptr<buffer> inflate_zlib(const buffer &data, uint32_t size);

	std::shared_ptr<buffer> inflate_gzip(const buffer &data, uint32_t size);

	std::shared_ptr<buffer> decode_quoted_printable(const buffer &data, uint32_t size);

	std::shared_ptr<buffer> decode_base64(const buffer &data, uint32_t size);

	//@}
}

#endif
