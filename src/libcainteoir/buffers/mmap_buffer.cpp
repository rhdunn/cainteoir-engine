/* Memory-Mapped File Buffer.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdexcept>
#include <string.h>
#include <errno.h>

class mmap_buffer : public cainteoir::buffer
{
	int fd;
public:
	mmap_buffer(const char *path);
	~mmap_buffer();
};

mmap_buffer::mmap_buffer(const char *path)
	: buffer(nullptr, nullptr)
	, fd(-1)
{
	fd = open(path, O_RDONLY);
	if (fd == -1) throw std::runtime_error(strerror(errno));

	struct stat sb;
	if (fstat(fd, &sb) == -1) throw std::runtime_error(strerror(errno));

	first = (const char *)mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (first == MAP_FAILED) throw std::runtime_error(strerror(errno));

	last = first + sb.st_size;
}

mmap_buffer::~mmap_buffer()
{
	if (fd != -1) close(fd);
	if (first) munmap((void *)first, size());
}

/** @brief Create a buffer from a file.
  *
  * @param[in] path The path of the file to read the data from.
  *
  * @return A buffer containing the content of the specified file.
  *
  * This function creates a memory mapped view of the file for optimal reading
  * of file contents.
  */
std::shared_ptr<cainteoir::buffer> cainteoir::make_file_buffer(const char *path)
{
	return std::make_shared<mmap_buffer>(path);
}
