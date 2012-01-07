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

		typedef int (*match_type)(const char *s1, const char *s2, size_t n);
		static const match_type match_case;
		static const match_type ignore_case;

		int compare(const char   *str, match_type match = match_case) const { return compare(str,         strlen(str), match); }
		int compare(const buffer &str, match_type match = match_case) const { return compare(str.begin(), str.size(),  match); }

		int comparei(const char   *str) const { return compare(str,         strlen(str), ignore_case); }
		int comparei(const buffer &str) const { return compare(str.begin(), str.size(),  ignore_case); }

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

	class normalized_text_buffer : public buffer
	{
	public:
		normalized_text_buffer(const char *f, const char *l);
		normalized_text_buffer(const char *str);
		normalized_text_buffer(const std::tr1::shared_ptr<cainteoir::buffer> &str);

		~normalized_text_buffer();
	private:
		void normalize(const char *f, const char *l);
	};

	class rope
	{
		std::list< std::tr1::shared_ptr<cainteoir::buffer> > data;
		std::size_t len;
	public:
		rope(): len(0) {}

		std::size_t size() const { return len; }

		bool empty() const { return len == 0; }

		void clear();

		rope &operator=(const std::tr1::shared_ptr<cainteoir::buffer> &item);

		rope &operator+=(const std::tr1::shared_ptr<cainteoir::buffer> &item);

		std::tr1::shared_ptr<cainteoir::buffer> buffer() const;

		std::tr1::shared_ptr<cainteoir::buffer> normalize() const
		{
			return std::tr1::shared_ptr<cainteoir::buffer>(new normalized_text_buffer(buffer()));
		}

		std::string str() const { return buffer()->str(); }
	};

	/** @name Decoding/Decompression API */
	//@{

	/** @brief Pointer to a decoding/decompression algorithm.
	  *
	  * @param data The data buffer to be decoded/decompressed.
	  * @param size The size of the decoded/decompressed data buffer.
	  *
	  * @return The new data buffer.
	  */
	typedef std::tr1::shared_ptr<buffer> (*decoder_ptr)(const buffer &data, uint32_t size);

	/** @brief Copy the data in buffer to a memory buffer.
	  *
	  * @param data The data buffer to be decoded/decompressed.
	  * @param size The size of the decoded/decompressed data buffer.
	  *
	  * @return The new data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> copy(const cainteoir::buffer &data, uint32_t size);

	/** @brief Inflate a zlib compressed data buffer.
	  *
	  * @param data The data buffer to be decoded/decompressed.
	  * @param size The size of the decoded/decompressed data buffer.
	  *
	  * @return The uncompressed data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> inflate_zlib(const cainteoir::buffer &data, uint32_t size);

	/** @brief Inflate a gzip compressed data buffer.
	  *
	  * @param data The data buffer to be decoded/decompressed.
	  * @param size The size of the decoded/decompressed data buffer.
	  *
	  * @return The uncompressed data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> inflate_gzip(const cainteoir::buffer &data, uint32_t size);

	/** @brief Decode a quoted printable encoded data buffer.
	  *
	  * @param data The data buffer to be decoded/decompressed.
	  * @param size The size of the decoded/decompressed data buffer.
	  *
	  * @return The decoded data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> decode_quoted_printable(const cainteoir::buffer &data, uint32_t size);

	/** @brief Decode a base64 encoded data buffer.
	  *
	  * @param data The data buffer to be decoded/decompressed.
	  * @param size The size of the decoded/decompressed data buffer.
	  *
	  * @return The decoded data buffer.
	  */
	std::tr1::shared_ptr<cainteoir::buffer> decode_base64(const cainteoir::buffer &data, uint32_t size);

	//@}
}

#endif
