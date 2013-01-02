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

/** @defgroup buffers Buffers
  * @brief    Magage data or string buffers.
  */

/// @addtogroup buffers
/// @{

/** @class cainteoir::buffer
  * @brief Represents a data or string buffer.
  *
  * This class does not manage the lifetime of the data held within it, however
  * derived classes may manage the lifetime.
  */

/// @}

/** @fn    cainteoir::buffer::buffer(const char *f, const char *l)
  * @brief Create a new data buffer from a range.
  *
  * @param[in] f The start of the buffer.
  * @param[in] l The end of the buffer.
  */

/** @fn    cainteoir::buffer::buffer(const char *f)
  * @brief Create a new data buffer from a string.
  *
  * @param[in] f The null-terminated string to create the buffer from.
  */

/** @fn    cainteoir::buffer::~buffer()
  * @brief Clean up the buffer.
  */

/** @typedef int (*cainteoir::buffer::match_type)(const char *s1, const char *s2, size_t n)
  * @brief   Pointer to a string comparison function.
  *
  * @param[in] s1 The first string to compare.
  * @param[in] s2 The second string to compare.
  * @param[in] n  The number of characters to compare.
  */

/** @brief Perform case-sensitive string comparison.
  */
const cainteoir::buffer::match_type cainteoir::buffer::match_case  = strncmp;

/** @brief Perform case-insensitive string comparison.
  */
const cainteoir::buffer::match_type cainteoir::buffer::ignore_case = strncasecmp;

/** @fn    bool cainteoir::buffer::startswith(const char *str, match_type match) const
  * @brief Does the buffer start with the specified string?
  *
  * @param[in] str   The string to check for.
  * @param[in] match The comparison method to use (case sensitive or insensitive).
  *
  * @retval true  If the buffer starts with the specified string.
  * @retval false If the buffer does not start with the specified string.
  */

/** @fn    int cainteoir::buffer::compare(const char *str, match_type match) const
  * @brief Compare the buffer with a string.
  *
  * @param[in] str   The string to compare with.
  * @param[in] match The comparison method to use (case sensitive or insensitive).
  *
  * @retval  0 If the strings match.
  * @retval <0 If str is less than the buffer.
  * @retval >0 if str is greater than the buffer.
  */

/** @fn    int cainteoir::buffer::compare(const buffer &str, match_type match) const
  * @brief Compare the buffer with a string.
  *
  * @param[in] str   The string to compare with.
  * @param[in] match The comparison method to use (case sensitive or insensitive).
  *
  * @retval  0 If the strings match.
  * @retval <0 If str is less than the buffer.
  * @retval >0 if str is greater than the buffer.
  */

/** @fn    int cainteoir::buffer::comparei(const char *str) const
  * @brief Compare the buffer with a string, ignoring case differences.
  *
  * @param[in] str The string to compare with.
  *
  * @retval  0 If the strings match.
  * @retval <0 If str is less than the buffer.
  * @retval >0 if str is greater than the buffer.
  */

/** @fn    int cainteoir::buffer::comparei(const buffer &str) const
  * @brief Compare the buffer with a string, ignoring case differences.
  *
  * @param[in] str The string to compare with.
  *
  * @retval  0 If the strings match.
  * @retval <0 If str is less than the buffer.
  * @retval >0 if str is greater than the buffer.
  */

/** @fn    std::string cainteoir::buffer::str() const
  * @brief Convert the buffer to a C++ string.
  *
  * @return The buffer as a C++ string.
  */

/** @defgroup decoders Decoding and Decompression
  * @brief    Decode encoded data and decompress compressed data.
  */

/// @addtogroup decoders
/// @{

/** @typedef std::shared_ptr<cainteoir::buffer> (*cainteoir::decoder_ptr)(const buffer &data, uint32_t size)
  * @brief   Pointer to a decoding/decompression algorithm.
  *
  * @param[in] data The data buffer to be decoded/decompressed.
  * @param[in] size The size of the decoded/decompressed data buffer.
  *
  * @return The new data buffer.
  */

/// @}
