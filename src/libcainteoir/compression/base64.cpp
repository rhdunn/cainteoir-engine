/* Encoded Stream (Base64)
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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

#include <cainteoir/buffer.hpp>

static const char base64_chars[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789"
	"+/"
	;

struct base64 : public cainteoir::data_buffer
{
	base64(const cainteoir::buffer &stream)
		: cainteoir::data_buffer(stream.size())
	{
		memcpy((char *)first, stream.begin(), stream.size());

		char * current = (char *)first;
		char * next    = (char *)first;

		uint8_t in[4];
		int i = 0;

		while (next < last)
		{
			char *idx = strchr((char *)base64_chars, *next);
			if (idx != nullptr)
			{
				in[i++] = (intptr_t)idx - (intptr_t)base64_chars;
				if (i == 4)
				{
					uint32_t b = (in[0] << 18) | (in[1] << 12) | (in[2] << 6) | in[3];
					*current++ = (b >> 16) & 0xFF;
					*current++ = (b >>  8) & 0xFF;
					*current++ = b & 0xFF;
					i = 0;
				}
			}
			++next;
		}

		int padding = 0;
		while (*next-- == '=')
			++padding;

		if (padding == 2)
		{
			uint32_t b = (in[0] << 18) | (in[1] << 12);
			*current++ = b >> 16;
		}
		else if (padding == 1)
		{
			uint32_t b = (in[0] << 18) | (in[1] << 12) | (in[2] << 6);
			*current++ = b >> 16;
			*current++ = (b >> 8) & 0xFF;
		}

		last = current;
	}
};

/** @brief Decode a base64 encoded data buffer.
  *
  * @param[in] data The data buffer to be decoded/decompressed.
  * @param[in] size The size of the decoded/decompressed data buffer.
  *
  * @return The decoded data buffer.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::decode_base64(const cainteoir::buffer &data, uint32_t size)
{
	return std::make_shared<base64>(data);
}
