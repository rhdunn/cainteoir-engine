/* UTF-8 Unicode Encoding Support.
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

#include "config.h"
#include "i18n.h"

#include <cainteoir/unicode.hpp>

namespace utf8 = cainteoir::utf8;

/// @addtogroup encoding
///@{

/** @brief Write a UTF-8 character to an output buffer.
  *
  * @param[in] out The output buffer to write the UTF-8 character to.
  * @param[in] c   The UTF-32 unicode code-point.
  *
  * @return A pointer to the new position within @a out after writing the UTF-8 character.
  *
  * This function does not null-terminate out and does not check that there is
  * enough space in out to write all the UTF-8 characters (up-to 4 characters).
  */
char *utf8::write(char *out, uint32_t c)
{
	if (c < 0x80)
		*out++ = c;
	else if (c < 0x800)
	{
		*out++ = 0xC0 | (c >> 6);
		*out++ = 0x80 + (c & 0x3F);
	}
	else if (c < 0x10000)
	{
		*out++ = 0xE0 | (c >> 12);
		*out++ = 0x80 + ((c >> 6) & 0x3F);
		*out++ = 0x80 + (c & 0x3F);
	}
	else if (c < 0x200000)
	{
		*out++ = 0xF0 | (c >> 18);
		*out++ = 0x80 + ((c >> 12) & 0x3F);
		*out++ = 0x80 + ((c >>  6) & 0x3F);
		*out++ = 0x80 + (c & 0x3F);
	}
	return out;
}

/** @brief Read a UTF-8 character from an input buffer.
  *
  * @param[in]  in The input buffer to read the UTF-8 character from.
  * @param[out] c  The UTF-32 unicode code-point.
  *
  * @return A pointer to the new position within @a in after reading the UTF-8 character.
  */
const char *utf8::read(const char *in, uint32_t &c)
{
	if (uint8_t(*in) < 0x80)
		c = *in++;
	else switch (uint8_t(*in) & 0xF0)
	{
	default:
		c = uint8_t(*in++) & 0x1F;
		c = (c << 6) + (uint8_t(*in++) & 0x3F);
		break;
	case 0xE0:
		c = uint8_t(*in++) & 0x0F;
		c = (c << 6) + (uint8_t(*in++) & 0x3F);
		c = (c << 6) + (uint8_t(*in++) & 0x3F);
		break;
	case 0xF0:
		c = uint8_t(*in++) & 0x07;
		c = (c << 6) + (uint8_t(*in++) & 0x3F);
		c = (c << 6) + (uint8_t(*in++) & 0x3F);
		c = (c << 6) + (uint8_t(*in++) & 0x3F);
		break;
	}
	return in;
}

/** @brief Get the next UTF-8 character in the sequence.
  *
  * @param[in] c The current position in the UTF-8 string.
  *
  * @return A pointer to the next UTF-8 character.
  */
const char *utf8::next(const char *c)
{
	if (uint8_t(*c) < 0x80) return ++c;
	switch (uint8_t(*c) & 0xF0)
	{
	default:   return c + 2;
	case 0xE0: return c + 3;
	case 0xF0: return c + 4;
	}
}

/** @brief Get the previous UTF-8 character in the sequence.
  *
  * @param[in] c The current position in the UTF-8 string.
  *
  * @return A pointer to the previous UTF-8 character.
  */
const char *utf8::prev(const char *c)
{
	while ((uint8_t(*--c) & 0xC0) == 0x80)
		;
	return c;
}

/// @}
