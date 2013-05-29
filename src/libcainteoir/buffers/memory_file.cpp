/* Memory Buffer using the C FILE API.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

struct malloced_buffer : public cainteoir::buffer
{
	malloced_buffer(const char *data, size_t length)
		: cainteoir::buffer(data, data + length)
	{
	}

	~malloced_buffer()
	{
		free((void *)begin());
	}
};

cainteoir::memory_file::memory_file()
	: f(nullptr)
	, data(nullptr)
	, length(0)
{
	f = open_memstream(&data, &length);
}

cainteoir::memory_file::~memory_file()
{
	if (f != nullptr)
	{
		fclose(f);
		free(data);
	}
}

std::shared_ptr<cainteoir::buffer> cainteoir::memory_file::buffer()
{
	if (f != nullptr)
	{
		fclose(f);
		f = nullptr;
		return std::make_shared<malloced_buffer>(data, length);
	}
	return std::shared_ptr<cainteoir::buffer>();
}
