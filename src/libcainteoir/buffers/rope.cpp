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

#include <cainteoir/buffer.hpp>

void cainteoir::rope::clear()
{
	data.clear();
	len = 0;
}

cainteoir::rope &cainteoir::rope::operator=(const std::tr1::shared_ptr<cainteoir::buffer> &item)
{
	clear();
	return *this += item;
}

cainteoir::rope &cainteoir::rope::operator+=(const std::tr1::shared_ptr<cainteoir::buffer> &item)
{
	data.push_back(item);
	len += item->size();
	return *this;
}

std::tr1::shared_ptr<cainteoir::buffer> cainteoir::rope::buffer() const
{
	if (data.size() == 0)
		return std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(NULL, NULL));

	std::tr1::shared_ptr<cainteoir::buffer> temp(new cainteoir::data_buffer(len));
	char * startPos = (char *)temp->begin();
	for (auto node = data.begin(), last = data.end(); node != last; ++node)
	{
		memcpy(startPos, (*node)->begin(), (*node)->size());
		startPos += (*node)->size();
	}

	*const_cast<rope *>(this) = temp;
	return temp;
}

std::tr1::shared_ptr<cainteoir::buffer> cainteoir::rope::content() const
{
	std::tr1::shared_ptr<cainteoir::buffer> text = buffer();

	const char * str = text->begin();
	const char * end = text->end();
	while (str != end && (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n'))
		++str;

	return (str == end) ? std::tr1::shared_ptr<cainteoir::buffer>() : text;
}
