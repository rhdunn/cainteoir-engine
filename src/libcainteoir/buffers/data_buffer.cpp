/* Memory (Data) Buffer.
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

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/buffer.hpp>

/// @addtogroup buffers
/// @{

/** @class cainteoir::data_buffer
  * @brief A buffer allocated in memory of a given size.
  */

/// @}

/** @brief Create a memory buffer of the requested size.
  *
  * @param[in] aSize The size of the buffer to create.
  */
cainteoir::data_buffer::data_buffer(unsigned int aSize)
	: buffer(nullptr, nullptr)
{
	first = new char[aSize];
	last = first + aSize;
}

/** @brief Clean up the buffer.
  */
cainteoir::data_buffer::~data_buffer()
{
	delete [] first;
}

/// @addtogroup buffers
/// @{

/** @brief Create a data buffer with the content of the string.
  *
  * @param[in] aString The content of the buffer.
  *
  * @return A new data buffer with the content of @a aString.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::make_buffer(const std::string &aString)
{
	std::shared_ptr<cainteoir::buffer> data = std::make_shared<cainteoir::data_buffer>(aString.size() + 1);
	strcpy((char *)data->begin(), aString.c_str());
	return data;
}

/** @brief Create a data buffer with the content of the string.
  *
  * @param[in] aString The content of the buffer.
  * @param[in] aLength The length of the string (or number of bytes of the string to copy).
  *
  * @return A new data buffer with the content of @a aString.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::make_buffer(const char *aString, int aLength)
{
	std::shared_ptr<cainteoir::buffer> data = std::make_shared<cainteoir::data_buffer>(aLength);
	memcpy((void *)data->begin(), aString, aLength);
	return data;
}

/// @}
