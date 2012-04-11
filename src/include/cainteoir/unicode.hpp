/* Unicode Support.
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

#ifndef CAINTEOIR_ENGINE_UNICODE_HPP
#define CAINTEOIR_ENGINE_UNICODE_HPP

#include "buffer.hpp"

namespace cainteoir { namespace utf8
{
	/** @brief Write a UTF-8 character to an output buffer.
	  *
	  * @param[in] out The output buffer to write the UTF-8 character to.
	  * @param[in] c   The UTF-32 unicode code-point.
	  *
	  * @return A pointer to the new position in out after writing the UTF-8 character.
	  *
	  * This function does not null-terminate out and does not check that there is
	  * enough space in out to write all the UTF-8 characters (up-to 4 characters).
	  */
	char *write(char *out, uint32_t c);
}}

#endif
