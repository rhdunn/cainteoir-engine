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

#include <zlib.h>
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

std::tr1::shared_ptr<cainteoir::buffer> zip_copy(const cainteoir::buffer & compressed, uint32_t uncompressed)
{
	std::tr1::shared_ptr<cainteoir::buffer> data(new cainteoir::data_buffer(uncompressed));
	memcpy((char *)data->begin(), compressed.begin(), compressed.size());

	return data;
}

std::tr1::shared_ptr<cainteoir::buffer> zip_inflate(const cainteoir::buffer & compressed, uint32_t uncompressed)
{
	std::tr1::shared_ptr<cainteoir::buffer> data(new cainteoir::data_buffer(uncompressed));

	int ret;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit2(&strm, -MAX_WBITS);
	if (ret != Z_OK)
		goto err;

	strm.avail_in = compressed.size();
	strm.next_in = (Bytef *)compressed.begin();

	strm.avail_out = uncompressed;
	strm.next_out = (Bytef *)data->begin();

	ret = inflate(&strm, Z_SYNC_FLUSH);
	(void)inflateEnd(&strm);

	if (ret != Z_STREAM_END)
		goto err;

	return data;

err:
	switch (ret)
	{
	case Z_STREAM_ERROR:
		throw std::runtime_error("decompression failed (invalid compression level)");
	case Z_DATA_ERROR:
		throw std::runtime_error("decompression failed (invalid or incomplete deflate data)");
	case Z_MEM_ERROR:
		throw std::runtime_error("decompression failed (out of memory)");
	case Z_VERSION_ERROR:
		throw std::runtime_error("decompression failed (zlib version mismatch)");
	default:
		throw std::runtime_error("decompression failed (unspecified error)");
	}
}

cainteoir::zip::archive::archive(const char *aFilename)
{
	cainteoir::mmap_buffer zf(aFilename);

	const zip_header * hdr = (const zip_header *)zf.begin();
	while ((const char *)hdr < zf.end() && hdr->magic == ZIP_HEADER_MAGIC)
	{
		const char *ptr = (const char *)hdr;
		ptr += sizeof(zip_header);

		cainteoir::buffer filename(ptr, ptr + hdr->len_filename);

		ptr += hdr->len_filename + hdr->len_extra;

		cainteoir::buffer compressed(ptr, ptr + hdr->compressed);

		ptr += hdr->compressed;

		switch (hdr->compression_type)
		{
		case zip_uncompressed:
			(*this)[ filename.str() ] = zip_copy(compressed, hdr->uncompressed);
			break;
		case zip_deflated:
			(*this)[ filename.str() ] = zip_inflate(compressed, hdr->uncompressed);
			break;
		default:
			throw std::runtime_error("decompression failed (unsupported compression type)");
		}

		hdr = (const zip_header *)ptr;
	}
}
