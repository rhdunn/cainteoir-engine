/* ZIP File Reader API.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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

#include <cainteoir/archive.hpp>
#include <stdexcept>

#define ZIP_HEADER_MAGIC      0x04034b50
#define DATA_DESCRIPTOR_MAGIC 0x08074b50

#define DATA_DESCRIPTOR_FLAG  0x0008

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

struct zip_data_descriptor
{
	uint32_t magic;
	uint32_t crc32;
	uint32_t compressed;
	uint32_t uncompressed;
};

#pragma pack(pop)

struct zip_data
{
	uint16_t compression_type;
	const char *begin;
	uint32_t compressed;
	uint32_t uncompressed;
	std::shared_ptr<cainteoir::buffer> cached;
};

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

class zip_archive : public cainteoir::archive
{
public:
	zip_archive(std::shared_ptr<cainteoir::buffer> aData, const cainteoir::rdf::uri &aSubject);

	const cainteoir::rdf::uri location(const std::string &aFilename, const std::string &aRef) const;

	std::shared_ptr<cainteoir::buffer> read(const char *aFilename) const;

	const std::list<std::string> &files() const;
private:
	std::shared_ptr<cainteoir::buffer> mData;
	std::map<std::string, zip_data> data;
	std::list<std::string> filelist;
	std::string base;
};

zip_archive::zip_archive(std::shared_ptr<cainteoir::buffer> aData, const cainteoir::rdf::uri &aSubject)
	: base(aSubject.str() + "!/")
	, mData(aData)
{
	const zip_header * hdr = (const zip_header *)aData->begin();
	while ((const char *)hdr < aData->end() && hdr->magic == ZIP_HEADER_MAGIC)
	{
		const char *ptr = (const char *)hdr + sizeof(zip_header);
		std::string filename(ptr, ptr + hdr->len_filename);

		zip_data &item = data[filename];
		filelist.push_back(filename);

		item.compression_type = hdr->compression_type;
		item.begin = ptr + hdr->len_filename + hdr->len_extra;
		if ((hdr->flags & DATA_DESCRIPTOR_FLAG) == DATA_DESCRIPTOR_FLAG)
		{
			const char *magic = item.begin;
			while (magic < aData->end() && *(const uint32_t *)magic != DATA_DESCRIPTOR_MAGIC)
				++magic;

			const zip_data_descriptor *descriptor = (const zip_data_descriptor *)magic;
			if (magic >= aData->end())
				throw std::runtime_error(i18n("zip file data descriptor entry not found"));

			item.compressed = descriptor->compressed;
			item.uncompressed = descriptor->uncompressed;
			hdr = (const zip_header *)(item.begin + item.compressed + sizeof(zip_data_descriptor));
		}
		else
		{
			item.compressed = hdr->compressed;
			item.uncompressed = hdr->uncompressed;
			hdr = (const zip_header *)(item.begin + item.compressed);
		}
	}
}

const cainteoir::rdf::uri zip_archive::location(const std::string &aFilename, const std::string &aRef) const
{
	return cainteoir::rdf::uri(base + aFilename, aRef);
}

std::shared_ptr<cainteoir::buffer> zip_archive::read(const char *aFilename) const
{
	std::string filename = aFilename;
	auto entry = data.find(filename);
	if (entry == data.end())
	{
		size_t pos = 0;
		while ((pos = filename.find("%20", pos)) != std::string::npos)
		{
			filename.replace(pos, 3, " ");
			++pos;
		}
		entry = data.find(filename);
	}
	if (entry == data.end())
		return std::shared_ptr<cainteoir::buffer>();

	const zip_data &item = entry->second;
	if (item.cached) return item.cached;

	auto decoder = *(zip_compression.begin() + item.compression_type);
	if (item.compression_type >= zip_compression.size() || decoder == nullptr)
		throw std::runtime_error(i18n("decompression failed (unsupported compression type)"));

	cainteoir::buffer compressed { item.begin, item.begin + item.compressed };

	return const_cast<zip_data &>(item).cached = decoder(compressed, item.uncompressed);
}

const std::list<std::string> &zip_archive::files() const
{
	return filelist;
}

std::shared_ptr<cainteoir::archive>
cainteoir::create_zip_archive(std::shared_ptr<buffer> aData, const rdf::uri &aSubject)
{
	return std::make_shared<zip_archive>(aData, aSubject);
}
