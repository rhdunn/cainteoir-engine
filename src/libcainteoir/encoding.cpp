/* Encoding Conversion Support.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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
#include "i18n.h"

#include <cainteoir/encoding.hpp>
#include <stdexcept>
#include <errno.h>

static const std::initializer_list<std::pair<int, const char *>> codepages = {
	{   437, "cp437" },
	{   737, "cp737" },
	{   775, "cp775" },
	{   850, "cp850" },
	{   851, "cp851" },
	{   852, "cp852" },
	{   855, "cp855" },
	{   857, "cp857" },
	{   860, "cp860" },
	{   861, "cp861" },
	{   862, "cp862" },
	{   863, "cp863" },
	{   865, "cp865" },
	{   866, "cp866" },
	{   869, "cp869" },
	{   874, "cp874" },
	{  1200, "utf-16le" },
	{  1201, "utf-16be" },
	{  1250, "windows-1250" },
	{  1251, "windows-1251" },
	{  1252, "windows-1252" },
	{  1253, "windows-1253" },
	{  1254, "windows-1254" },
	{  1255, "windows-1255" },
	{  1256, "windows-1256" },
	{  1257, "windows-1257" },
	{  1258, "windows-1258" },
	{ 10000, "macintosh" },
	{ 10006, "mac-greek" },
	{ 10007, "ms-mac-cyrillic" },
	{ 10029, "mac-centraleurope" },
	{ 10079, "mac-is" },
	{ 10081, "mac-turkish" },
	{ 20127, "us-ascii" },
	{ 28591, "iso-8859-1" },
	{ 28592, "iso-8859-2" },
	{ 28593, "iso-8859-3" },
	{ 28594, "iso-8859-4" },
	{ 28595, "iso-8859-5" },
	{ 28596, "iso-8859-6" },
	{ 28597, "iso-8859-7" },
	{ 28598, "iso-8859-8" },
	{ 28599, "iso-8859-9" },
	{ 28600, "iso-8859-10" },
	{ 28601, "iso-8859-11" },
	{ 28602, "iso-8859-12" },
	{ 28603, "iso-8859-13" },
	{ 28604, "iso-8859-14" }, 
	{ 28605, "iso-8859-15" },
	{ 65000, "utf-7" },
	{ 65001, "utf-8" },
};

static inline bool is_native_encoding(const std::string &encoding)
{
	return !strcasecmp(encoding.c_str(), "utf-8") || !strcasecmp(encoding.c_str(), "us-ascii");
}

#ifdef HAVE_ICONV_H

#include <iconv.h>

struct native_decoder : public cainteoir::detail::decoder
{
	native_decoder(const char *aEncoding)
	{
		cvt = iconv_open("UTF-8", aEncoding);
		if (cvt == (iconv_t)-1)
		{
			if (errno == EINVAL)
				throw std::runtime_error(i18n("unsupported character set (no conversion found)"));
			throw std::runtime_error(i18n("unsupported character set (failed to initialise iconv)"));
		}
	}

	~native_decoder()
	{
		if (cvt != (iconv_t)-1)
			iconv_close(cvt);
	}

	void decode(const cainteoir::buffer &data, cainteoir::rope &decoded) const
	{
		char *in = (char *)data.begin();
		size_t inlen = data.size();

		char buffer[1024] = {0};
		while (inlen != 0)
		{
			char *out = buffer;
			size_t outlen = sizeof(buffer);

			if (iconv(cvt, &in, &inlen, &out, &outlen) == (size_t)-1) switch (errno)
			{
			case E2BIG: // output buffer too small
				// this is ok ... process in chunks
				break;
			case EILSEQ: // illegal character (multi-byte sequence)
			case EINVAL: // incomplete multi-byte sequence
				// NOTE: iconv writes the byte as utf-8 to `out`, and places
				// in at the invalid byte
				++in;
				--inlen;
				break;
			default:
				throw std::runtime_error(strerror(errno));
			}

			if (outlen != sizeof(buffer))
				decoded += cainteoir::make_buffer(buffer, sizeof(buffer) - outlen);
		}
	}

	iconv_t cvt;
};

#else

struct native_decoder : public cainteoir::detail::decoder
{
	native_decoder(const char *aEncoding)
	{
	}

	void decode(const cainteoir::buffer &data, cainteoir::rope &decoded) const
	{
		// Throw here and not in the constructor to support utf-8 and
		// ascii pass-through.
		throw std::runtime_error(i18n("unsupported character set"));
	}
};

#endif

cainteoir::encoding::encoding(int aCodepage)
{
	set_encoding(aCodepage);
}

cainteoir::encoding::encoding(const char *aEncoding)
{
	set_encoding(aEncoding);
}

bool cainteoir::encoding::set_encoding(int aCodepage)
{
	for (auto &codepage : codepages)
	{
		if (codepage.first == aCodepage)
		{
			return set_encoding(codepage.second);
		}
	}

	throw std::runtime_error(i18n("unsupported character set (codepage not recognised)"));
}

bool cainteoir::encoding::set_encoding(const char *aEncoding)
{
	if (mEncoding == aEncoding)
		return false;

	mDecoder  = std::make_shared<native_decoder>(aEncoding);
	mEncoding = aEncoding;
	return true;
}

std::shared_ptr<cainteoir::buffer> cainteoir::encoding::lookup(uint8_t c) const
{
	cainteoir::rope ret;
	mDecoder->decode(cainteoir::buffer((char *)&c, (char *)&c + 1), ret);
	return ret.buffer();
}

std::shared_ptr<cainteoir::buffer> cainteoir::encoding::decode(const std::shared_ptr<cainteoir::buffer> &data) const
{
	if (!data.get() || is_native_encoding(mEncoding))
		return data;

	cainteoir::rope ret;
	mDecoder->decode(*data, ret);
	return ret.buffer();
}

void cainteoir::encoding::decode(const std::shared_ptr<cainteoir::buffer> &data, cainteoir::rope &decoded) const
{
	if (!data.get())
		return;

	if (is_native_encoding(mEncoding))
	{
		decoded += data;
		return;
	}

	mDecoder->decode(*data, decoded);
}
