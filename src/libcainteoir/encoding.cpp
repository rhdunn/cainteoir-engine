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
#include "encoding/encodings.h"

#define countof(a) (sizeof(a)/sizeof(a[0]))

static const char ** encoding_unknown = encoding_ascii;

struct single_byte_decoder : public cainteoir::decoder
{
	single_byte_decoder(const char ** aLookupTable)
		: mLookupTable(aLookupTable)
	{
	}

	const char * lookup(uint8_t c) const
	{
		return mLookupTable[c];
	}

	const char ** mLookupTable;
};

cainteoir::encoding::encoding(int aCodepage)
{
	set_encoding(aCodepage);
}

cainteoir::encoding::encoding(buffer aName)
{
	set_encoding(aName);
}

void cainteoir::encoding::set_encoding(int aCodepage)
{
	for (const codepage_t * first = codepages, * last = codepages + countof(codepages); first != last; ++first)
	{
		if (aCodepage == first->codepage)
		{
			mDecoder = std::tr1::shared_ptr<cainteoir::decoder>(new single_byte_decoder(first->table));
			return;
		}
	}
	mDecoder = std::tr1::shared_ptr<cainteoir::decoder>(new single_byte_decoder(encoding_unknown));
}

void cainteoir::encoding::set_encoding(buffer aName)
{
	for (const encoding_t * first = encodings, * last = encodings + countof(encodings); first != last; ++first)
	{
		if (aName.comparei(first->name))
		{
			mDecoder = std::tr1::shared_ptr<cainteoir::decoder>(new single_byte_decoder(first->table));
			return;
		}
	}
	mDecoder = std::tr1::shared_ptr<cainteoir::decoder>(new single_byte_decoder(encoding_unknown));
}
