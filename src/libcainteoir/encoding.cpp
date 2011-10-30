/* Encoding Conversion Support.
 *
 * Copyright (C) 2011 Reece H. Dunn
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

#include <cainteoir/encoding.hpp>
#include <cainteoir/platform.hpp>
#include <stdexcept>
#include <iconv.h>
#include <errno.h>

struct iconv_decoder : public cainteoir::decoder
{
	iconv_decoder(const char *aEncoding)
	{
		cvt = iconv_open("UTF-8", aEncoding);
		if (cvt == (iconv_t)-1)
		{
			if (errno == EINVAL)
				throw std::runtime_error(_("unsupported character set (no conversion found)"));
			throw std::runtime_error(_("unsupported character set (failed to initialise iconv)"));
		}
	}

	~iconv_decoder()
	{
		if (cvt != (iconv_t)-1)
			iconv_close(cvt);
	}

	std::tr1::shared_ptr<cainteoir::buffer> decode(const cainteoir::buffer &data) const
	{
		cainteoir::rope ret;

		char *in = (char *)data.begin();
		size_t inlen = data.size();

		char buffer[1024] = {0};
		while (inlen != 0)
		{
			char *out = buffer;
			size_t outlen = sizeof(buffer);

			if (iconv(cvt, &in, &inlen, &out, &outlen) == (size_t)-1)
				throw std::runtime_error(strerror(errno));

			if (outlen != sizeof(buffer))
			{
				std::tr1::shared_ptr<cainteoir::buffer> fiber(new cainteoir::data_buffer(sizeof(buffer) - outlen));
				memcpy((void *)fiber->begin(), buffer, sizeof(buffer) - outlen);
				ret += fiber;
			}
		}

		return ret.buffer();
	}

	iconv_t cvt;
};

cainteoir::encoding::encoding(int aCodepage)
{
	set_encoding(aCodepage);
}

cainteoir::encoding::encoding(const char *aEncoding)
{
	set_encoding(aEncoding);
}

void cainteoir::encoding::set_encoding(int aCodepage)
{
	switch (aCodepage)
	{
	case   437: set_encoding("437"); break;
	case   850: set_encoding("850"); break;
	case  1252: set_encoding("windows-1252"); break;
	case 10000: set_encoding("macintosh"); break;
	default:    throw std::runtime_error(_("unsupported character set (codepage not recognised)"));
	}
}

void cainteoir::encoding::set_encoding(const char *aEncoding)
{
	mDecoder = std::tr1::shared_ptr<cainteoir::decoder>(new iconv_decoder(aEncoding));
}
