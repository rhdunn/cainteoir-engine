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

cainteoir::data_buffer::data_buffer(unsigned int aSize)
	: buffer(nullptr, nullptr)
{
	first = new char[aSize];
	last = first + aSize;
}

cainteoir::data_buffer::~data_buffer()
{
	delete [] first;
}

std::shared_ptr<cainteoir::buffer> cainteoir::make_buffer(const std::string &aString)
{
	std::shared_ptr<cainteoir::buffer> data = std::make_shared<cainteoir::data_buffer>(aString.size() + 1);
	strcpy((char *)data->begin(), aString.c_str());
	return data;
}
