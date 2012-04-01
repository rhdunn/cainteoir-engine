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
				throw std::runtime_error(i18n("unsupported character set (no conversion found)"));
			throw std::runtime_error(i18n("unsupported character set (failed to initialise iconv)"));
		}
	}

	~iconv_decoder()
	{
		if (cvt != (iconv_t)-1)
			iconv_close(cvt);
	}

	std::shared_ptr<cainteoir::buffer> decode(const cainteoir::buffer &data) const
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
				std::shared_ptr<cainteoir::buffer> fiber(new cainteoir::data_buffer(sizeof(buffer) - outlen));
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
	case   437: set_encoding("cp437"); break;
	case   737: set_encoding("cp737"); break;
	case   775: set_encoding("cp775"); break;
	case   850: set_encoding("cp850"); break;
	case   851: set_encoding("cp851"); break;
	case   852: set_encoding("cp852"); break;
	case   855: set_encoding("cp855"); break;
	case   857: set_encoding("cp857"); break;
	case   860: set_encoding("cp860"); break;
	case   861: set_encoding("cp861"); break;
	case   862: set_encoding("cp862"); break;
	case   863: set_encoding("cp863"); break;
	case   865: set_encoding("cp865"); break;
	case   866: set_encoding("cp866"); break;
	case   869: set_encoding("cp869"); break;
	case  1200: set_encoding("utf-16le"); break;
	case  1201: set_encoding("utf-16be"); break;
	case  1250: set_encoding("windows-1250"); break;
	case  1251: set_encoding("windows-1251"); break;
	case  1252: set_encoding("windows-1252"); break;
	case  1253: set_encoding("windows-1253"); break;
	case  1254: set_encoding("windows-1254"); break;
	case  1255: set_encoding("windows-1255"); break;
	case  1256: set_encoding("windows-1256"); break;
	case  1257: set_encoding("windows-1257"); break;
	case  1258: set_encoding("windows-1258"); break;
	case 10000: set_encoding("macintosh"); break;
	case 10006: set_encoding("mac-greek"); break;
	case 10007: set_encoding("ms-mac-cyrillic"); break;
	case 10029: set_encoding("mac-centraleurope"); break;
	case 10079: set_encoding("mac-is"); break;
	case 10081: set_encoding("mac-turkish"); break;
	case 20127: set_encoding("us-ascii"); break;
	case 28591: set_encoding("iso-8859-1"); break;
	case 28592: set_encoding("iso-8859-2"); break;
	case 28593: set_encoding("iso-8859-3"); break;
	case 28594: set_encoding("iso-8859-4"); break;
	case 28595: set_encoding("iso-8859-5"); break;
	case 28596: set_encoding("iso-8859-6"); break;
	case 28597: set_encoding("iso-8859-7"); break;
	case 28598: set_encoding("iso-8859-8"); break;
	case 28599: set_encoding("iso-8859-9"); break;
	case 28600: set_encoding("iso-8859-10"); break;
	case 28601: set_encoding("iso-8859-11"); break;
	case 28602: set_encoding("iso-8859-12"); break;
	case 28603: set_encoding("iso-8859-13"); break;
	case 28604: set_encoding("iso-8859-14"); break;
	case 28605: set_encoding("iso-8859-15"); break;
	case 65000: set_encoding("utf-7"); break;
	case 65001: set_encoding("utf-8"); break;
	default:    throw std::runtime_error(i18n("unsupported character set (codepage not recognised)"));
	}
}

void cainteoir::encoding::set_encoding(const char *aEncoding)
{
	mDecoder = std::shared_ptr<cainteoir::decoder>(new iconv_decoder(aEncoding));
}
