/* ZIP File API.
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

#ifndef CAINTEOIR_ENGINE_ZIP_HPP
#define CAINTEOIR_ENGINE_ZIP_HPP

#include <cainteoir/buffer.hpp>
#include <stdexcept>

#include <zip.h>

namespace cainteoir { namespace zip
{
	class file
	{
	public:
		file(::zip *aArchive, const char *aFilename)
		{
			zf = zip_fopen(aArchive, aFilename, 0);
			if (!zf)
				throw std::runtime_error("error reading ePub document (internal file not found).");
		}

		~file()
		{
			zip_fclose(zf);
		}

		int read(void *aBuffer, int aCount)
		{
			return zip_fread(zf, aBuffer, aCount);
			
		}
	private:
		zip_file *zf;
	};

	class archive
	{
	public:
		archive(const char *aFilename)
		{
			int error = 0;
			z = zip_open(aFilename, ZIP_CHECKCONS, &error);
			if (!z)
				throw std::runtime_error("error reading ePub document (invalid ZIP file).");
		}

		~archive()
		{
			zip_close(z);
		}

		std::tr1::shared_ptr<cainteoir::buffer> read(const char *aFilename)
		{
			struct zip_stat sb;
			if (zip_stat(z, aFilename, 0, &sb) == -1)
				throw std::runtime_error("error reading ePub document (internal file not found).");

			std::tr1::shared_ptr<cainteoir::buffer> data = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::data_buffer(sb.size));

			if (file(z, aFilename).read((void *)data->begin(), data->size()) != data->size())
				throw std::runtime_error("error reading ePub document (unable to read file data).");

			return data;
		}
	private:
		::zip *z;
	};
}}

#endif
