/* Mime Type detector -- libmagic wrapper.
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

#ifndef CAINTEOIR_ENGINE_MIMETYPES_HPP
#define CAINTEOIR_ENGINE_MIMETYPES_HPP

#include "buffer.hpp"
#include <magic.h>
#include <stdexcept>
#include <cstdlib>

namespace cainteoir
{
	class mimetypes
	{
	public:
		mimetypes()
		{
			cookie = magic_open(MAGIC_MIME|MAGIC_CHECK);
			if (cookie == NULL)
				throw std::runtime_error("unable to initialise the mimetype database (libmagic).");

			const char *basedir = getenv("CAINTEOIR_DATADIR");
			if (!basedir)
				throw std::runtime_error("CAINTEOIR_DATADIR environment variable not set.");

			std::string filename = std::string(basedir) + "/magic.mgc";
			if (magic_load(cookie, filename.c_str()) == -1)
				throw std::runtime_error(magic_error(cookie));
		}

		~mimetypes()
		{
			magic_close(cookie);
		}

		std::string operator()(const cainteoir::buffer *data)
		{
			const char * type = magic_buffer(cookie, data->begin(), data->size());
			if (type == NULL)
				throw std::runtime_error(magic_error(cookie));

			std::string mime = type;
			return mime.substr(0, mime.find(';'));
		}

		std::string operator()(const char *aFilename)
		{
			const char * type = magic_file(cookie, aFilename);
			if (type == NULL)
				throw std::runtime_error(magic_error(cookie));

			std::string mime = type;
			return mime.substr(0, mime.find(';'));
		}
	private:
		magic_t cookie;
	};
}

#endif
