/* ZIP File Reader API.
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

#include "zip.hpp"
#include "parsers.hpp"
#include <cainteoir/platform.hpp>

#include <stdexcept>

#define ZIP_HEADER_MAGIC 0x04034b50

#pragma pack(push, 1)

struct zip_header
{
	uint32_t magic;
	uint8_t  zip_version;
	uint8_t  os_version;
	uint16_t flags;
	uint16_t compression_type;
	uint16_t mod_filetime;
	uint16_t mod_filedate;
	uint32_t crc32;
	uint32_t compressed;
	uint32_t uncompressed;
	uint16_t len_filename;
	uint16_t len_extra;
};

#pragma pack(pop)

enum zip_compression
{
	zip_uncompressed = 0,
	zip_imploded = 6,
	zip_deflated = 8,
	zip_bzip2compressed = 12,
};

cainteoir::zip::archive::archive(std::tr1::shared_ptr<cainteoir::buffer> aData)
{
	const zip_header * hdr = (const zip_header *)aData->begin();
	while ((const char *)hdr < aData->end() && hdr->magic == ZIP_HEADER_MAGIC)
	{
		const char *ptr = (const char *)hdr + sizeof(zip_header);

		(*this)[ std::string(ptr, ptr + hdr->len_filename) ] = hdr;

		hdr = (const zip_header *)(ptr + hdr->len_filename + hdr->len_extra + hdr->compressed);
	}
}

std::tr1::shared_ptr<cainteoir::buffer> cainteoir::zip::archive::read(const char *aFilename)
{
	const zip_header * hdr = (const zip_header *)((*this)[aFilename]);
	if (!hdr)
		return std::tr1::shared_ptr<cainteoir::buffer>();

	const char *ptr = (const char *)hdr + sizeof(zip_header) + hdr->len_filename + hdr->len_extra;

	cainteoir::buffer compressed(ptr, ptr + hdr->compressed);
	switch (hdr->compression_type)
	{
	case zip_uncompressed:
		if (hdr->compressed != hdr->uncompressed)
			throw std::runtime_error(_("uncompressed zip stream mismatch between compressed and decompressed size"));
		return strm_copy(compressed);
	case zip_deflated:
		return strm_inflate(compressed, hdr->uncompressed);
	default:
		throw std::runtime_error(_("decompression failed (unsupported compression type)"));
	}
}
