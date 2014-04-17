/* Rope Buffer API.
 *
 * Copyright (C) 2010-2014 Reece H. Dunn
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
#include <unistd.h>

void cainteoir::rope::clear()
{
	data.clear();
	len = 0;
}

cainteoir::rope &cainteoir::rope::operator=(const std::shared_ptr<cainteoir::buffer> &item)
{
	clear();
	return *this += item;
}

cainteoir::rope &cainteoir::rope::operator+=(const std::shared_ptr<cainteoir::buffer> &item)
{
	data.push_back(item);
	len += item->size();
	return *this;
}

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

std::shared_ptr<cainteoir::buffer> cainteoir::rope::content() const
{
	std::shared_ptr<cainteoir::buffer> text = buffer();

	const char * str = text->begin();
	const char * end = text->end();
	while (str != end && (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'))
		++str;

	return (str == end) ? std::shared_ptr<cainteoir::buffer>() : text;
}

std::shared_ptr<cainteoir::buffer> cainteoir::make_file_buffer(FILE *f)
{
	cainteoir::rope data;
	char buffer[1024];

	size_t read = 0;
	while ((read = fread(buffer, 1, sizeof(buffer), f)) != 0)
		data += cainteoir::make_buffer(buffer, read);

	return data.buffer();
}

std::shared_ptr<cainteoir::buffer> cainteoir::make_file_buffer(int fd)
{
	cainteoir::rope data;
	char buffer[1024];

	ssize_t read = 0;
	while ((read = ::read(fd, buffer, sizeof(buffer))) > 0)
		data += cainteoir::make_buffer(buffer, read);

	return data.buffer();
}
