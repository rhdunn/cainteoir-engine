/* Rope Buffer API.
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

/** @class cainteoir::rope
  * @brief Manage a list of buffers.
  *
  * This class is for situations where many buffers are created and concatenated
  * together. It only concatenates the buffers when requested, performing the
  * operation in one pass.
  */

/** @fn    cainteoir::rope::rope()
  * @brief Create an empty rope object.
  */

/** @fn    std::size_t cainteoir::rope::size() const
  * @brief Get the number of bytes in the rope buffers.
  *
  * @return The number of bytes in the rope buffers.
  */

/** @fn    bool cainteoir::rope::empty() const
  * @brief Is the rope empty?
  *
  * @retval true  If the rope does not contain any buffer objects.
  * @retval false If the rope contains buffer objects.
  */

/** @brief Empty the rope.
  */
void cainteoir::rope::clear()
{
	data.clear();
	len = 0;
}

/** @brief Set the rope to the content of the buffer.
  *
  * @param[in] item The buffer content to set the rope to.
  *
  * @return This rope object (for method chaining).
  */
cainteoir::rope &cainteoir::rope::operator=(const std::shared_ptr<cainteoir::buffer> &item)
{
	clear();
	return *this += item;
}

/** @brief Append the content of the buffer to the rope.
  *
  * @param[in] item The buffer content to append.
  *
  * @return This rope object (for method chaining).
  */
cainteoir::rope &cainteoir::rope::operator+=(const std::shared_ptr<cainteoir::buffer> &item)
{
	data.push_back(item);
	len += item->size();
	return *this;
}

/** @brief Get a buffer to the entire rope content.
  *
  * @return The entire rope content.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::rope::buffer() const
{
	if (data.size() == 0)
		return std::make_shared<cainteoir::buffer>(nullptr, nullptr);

	std::shared_ptr<cainteoir::buffer> temp = std::make_shared<cainteoir::data_buffer>(len);
	char * startPos = (char *)temp->begin();
	for (auto &node : data)
	{
		memcpy(startPos, node->begin(), node->size());
		startPos += node->size();
	}

	*const_cast<rope *>(this) = temp;
	return temp;
}

/** @brief Get the content of the rope.
  *
  * @return The entire rope content.
  *
  * This differs from the buffer method in that if the rope only contains whitespace
  * then an empty buffer here is returned.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::rope::content() const
{
	std::shared_ptr<cainteoir::buffer> text = buffer();

	const char * str = text->begin();
	const char * end = text->end();
	while (str != end && (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'))
		++str;

	return (str == end) ? std::shared_ptr<cainteoir::buffer>() : text;
}

/** @fn    std::shared_ptr<cainteoir::buffer> cainteoir::rope::normalize() const
  * @brief Get the rope buffer in its whitespace-normalized form.
  *
  * @return The rope buffer in its whitespace-normalized form.
  */

/** @fn    std::string cainteoir::rope::str() const
  * @brief Get the buffer as a C++ string.
  *
  * @return The buffer as a C++ string.
  */

/** @brief Create a buffer from a file.
  *
  * @param[in] f The file to read the data from.
  *
  * @return A buffer containing the content of the specified file.
  *
  * This function reads the data from the file in chunks. It is useful for
  * files that are buffered (stdin, pipes, sockets, etc.). For regular
  * files, use the version of this function that takes a path.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::make_file_buffer(FILE *f)
{
	cainteoir::rope data;
	char buffer[1024];

	size_t read = 0;
	while ((read = fread(buffer, 1, sizeof(buffer), f)) != 0)
		data += cainteoir::make_buffer(buffer, read);

	return data.buffer();
}

