/* Metadata Extractor.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include <cainteoir/metadata.hpp>
#include <cainteoir/buffer.hpp>
#include <cstdio>
#include <magic.h>

class mimetypes
{
public:
	mimetypes(const char *filename = NULL)
	{
		cookie = magic_open(MAGIC_MIME);
		magic_load(cookie, NULL);
	}

	~mimetypes()
	{
		magic_close(cookie);
	}

	const char * operator()(const cainteoir::buffer *data)
	{
		return magic_buffer(cookie, data->begin(), data->size());
	}
private:
	magic_t cookie;
};

int main(int argc, char ** argv)
{
	try
	{
		++argv;
		--argc;

		if (argc != 1)
		{
			fprintf(stderr, "error: no document specified\n");
			return 0;
		}

		std::auto_ptr<cainteoir::buffer> text_buffer = std::auto_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[0]));

		mimetypes mime;
		fprintf(stderr, "mime: %s\n", mime(text_buffer.get()));
	}
	catch (std::exception &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
