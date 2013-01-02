/* Encoded Stream (Quoted Printable)
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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

#include <cainteoir/buffer.hpp>

#ifndef DOXYGEN

static inline int hex_to_value(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return (c - 'a') + 10;
	if (c >= 'A' && c <= 'F')
		return (c - 'A') + 10;
	return 0;
}

struct quoted_printable : public cainteoir::data_buffer
{
	quoted_printable(const cainteoir::buffer &stream)
		: cainteoir::data_buffer(stream.size())
	{
		memcpy((char *)first, stream.begin(), stream.size());

		char * current = (char *)first;
		char * next    = (char *)first;

		while (next < last)
		{
			if (*next == '=')
			{
				++next;
				if (*next == '\n')
					++next;
				else if (next[0] == '\r' && next[1] == '\n')
					next += 2;
				else
				{
					*current = (hex_to_value(next[0]) << 4) | hex_to_value(next[1]);
					++current;
					next += 2;
				}
			}
			else
			{
				*current = *next;
				++current;
				++next;
			}
		}

		last = current;
	}
};

#endif

/// @addtogroup decoders
/// @{

/** @brief Decode a quoted printable encoded data buffer.
  *
  * @param[in] data The data buffer to be decoded/decompressed.
  * @param[in] size The size of the decoded/decompressed data buffer.
  *
  * @return The decoded data buffer.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::decode_quoted_printable(const cainteoir::buffer &data, uint32_t size)
{
	return std::make_shared<quoted_printable>(data);
}

/// @}
