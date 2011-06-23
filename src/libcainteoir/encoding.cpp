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

static const char * encoding_ascii[128] = {
//	0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "\t", "\n", NULL, "\r", NULL, NULL, NULL, // 0
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 1
	" ",  "!",  "\"", "#",  "$",  "%",  "&",  "'",  "(",  ")",  "*",  "+",  ",",  "-",  ".",  "/",  // 2
	"0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  ":",  ";",  "<",  "=",  ">",  "?",  // 3
	"@",  "A",  "B",  "C",  "D",  "E",  "F",  "G",  "H",  "I",  "J",  "K",  "L",  "M",  "N",  "O",  // 4
	"P",  "Q",  "R",  "S",  "T",  "U",  "V",  "W",  "X",  "Y",  "Z",  "[",  "\\", "]",  "^",  "_",  // 5
	"`",  "a",  "b",  "c",  "d",  "e",  "f",  "g",  "h",  "i",  "j",  "k",  "l",  "m",  "n",  "o",  // 6
	"p",  "q",  "r",  "s",  "t",  "u",  "v",  "w",  "x",  "y",  "z",  "{",  "|",  "}",  "~",  NULL, // 7
};

static const char * encoding_unknown[128] = {
//	0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 8
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 9
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // A
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // B
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // C
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // D
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // E
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // F
};

static const char * encoding_cp1252[128] = { // http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
//	0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	"\xe2\x82\xac", NULL, "\xe2\x80\x9a", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 8
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 9
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // A
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // B
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // C
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // D
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // E
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // F
};

const char ** get_encoding_table(int aCodepage)
{
	switch (aCodepage)
	{
	case  1252: return encoding_cp1252;
	default:    return encoding_unknown;
	}
}

struct single_byte_decoder : public cainteoir::decoder
{
	single_byte_decoder(const char ** aLookupTable)
		: mLookupTable(aLookupTable)
	{
	}

	const char * lookup(uint8_t c) const
	{
		if (c < 0x80) return encoding_ascii[c];
		return mLookupTable[c - 128];
	}

	const char ** mLookupTable;
};

cainteoir::encoding::encoding(int aCodepage)
	: mDecoder(new single_byte_decoder(get_encoding_table(aCodepage)))
{
}
