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
#include <cainteoir/path.hpp>
#include <stdexcept>
#include <errno.h>

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
#ifdef HAVE_OPEN_MEMSTREAM
	f = open_memstream(&data, &length);
#else
#ifndef ANDROID
	f = tmpfile();
#endif
	if (!f)
	{
		data = cainteoir::get_temp_filename();
		f = fopen(data, "w+b");
		if (!f) throw std::runtime_error(std::string(data) + ": " + strerror(errno));
	}
#endif
}

cainteoir::memory_file::~memory_file()
{
	if (f != nullptr)
	{
		fclose(f);
#ifdef HAVE_OPEN_MEMSTREAM
		free(data);
#else
		if (data)
		{
			remove(data);
			free(data);
		}
#endif
	}
}

std::shared_ptr<cainteoir::buffer> cainteoir::memory_file::buffer()
{
	if (f != nullptr)
	{
#ifdef HAVE_OPEN_MEMSTREAM
		fclose(f);
		f = nullptr;
		return std::make_shared<malloced_buffer>(data, length);
#else
		rewind(f);
		auto ret = cainteoir::make_file_buffer(f);
		fclose(f);
		f = nullptr;
		if (data)
		{
			remove(data);
			free(data);
		}
		return ret;
#endif
	}
	return std::shared_ptr<cainteoir::buffer>();
}
