/* ZIP File Reader API.
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
#include "i18n.h"

#include "zip.hpp"
#include "parsers.hpp"

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

static const std::initializer_list<cainteoir::decoder_ptr> zip_compression = {
	&cainteoir::copy, // 0 - uncompressed
	nullptr, // 1
	nullptr, // 2
	nullptr, // 3
	nullptr, // 4
	nullptr, // 5
	nullptr, // 6 - imploded
	nullptr, // 7
	&cainteoir::inflate_zlib, // 8 - deflated
	nullptr, // 9
	nullptr, // 10
	nullptr, // 11
	nullptr, // 12 - bzip2 compressed
};

cainteoir::zip::archive::archive(std::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject)
	: base(aSubject.str() + "!/")
{
	const zip_header * hdr = (const zip_header *)aData->begin();
	while ((const char *)hdr < aData->end() && hdr->magic == ZIP_HEADER_MAGIC)
	{
		const char *ptr = (const char *)hdr + sizeof(zip_header);

		(*this)[ std::string(ptr, ptr + hdr->len_filename) ] = hdr;

		hdr = (const zip_header *)(ptr + hdr->len_filename + hdr->len_extra + hdr->compressed);
	}
}

const cainteoir::rdf::uri cainteoir::zip::archive::location(const std::string &aFilename, const std::string &aRef) const
{
	return rdf::uri(base + aFilename, aRef);
}

std::shared_ptr<cainteoir::buffer> cainteoir::zip::archive::read(const char *aFilename) const
{
	auto entry = find(aFilename);
	if (entry == end())
		return std::shared_ptr<buffer>();

	const zip_header * hdr = (const zip_header *)entry->second;

	if (hdr->compression_type >= zip_compression.size() || *(zip_compression.begin() + hdr->compression_type) == nullptr)
		throw std::runtime_error(i18n("decompression failed (unsupported compression type)"));

	const char *ptr = (const char *)hdr + sizeof(zip_header) + hdr->len_filename + hdr->len_extra;
	cainteoir::buffer compressed { ptr, ptr + hdr->compressed };
	auto decoder = *(zip_compression.begin() + hdr->compression_type);

	return decoder(compressed, hdr->uncompressed);
}
