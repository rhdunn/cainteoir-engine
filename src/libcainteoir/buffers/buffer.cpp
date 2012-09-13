/* Buffer.
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

#include <cainteoir/buffer.hpp>
#include <strings.h>

const cainteoir::buffer::match_type cainteoir::buffer::match_case  = strncmp;
const cainteoir::buffer::match_type cainteoir::buffer::ignore_case = strncasecmp;

/** @typedef std::shared_ptr<cainteoir::buffer> (*cainteoir::decoder_ptr)(const buffer &data, uint32_t size)
  * @brief   Pointer to a decoding/decompression algorithm.
  *
  * @param[in] data The data buffer to be decoded/decompressed.
  * @param[in] size The size of the decoded/decompressed data buffer.
  *
  * @return The new data buffer.
  */
