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

static const char * encoding_iso8859_1[128] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 8
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 9
	// Ax
	"\xc2\xa0", "\xc2\xa1", "\xc2\xa2", "\xc2\xa3", "\xc2\xa4", "\xc2\xa5", "\xc2\xa6", "\xc2\xa7",
	"\xc2\xa8", "\xc2\xa9", "\xc2\xaa", "\xc2\xab", "\xc2\xac", "\xc2\xad", "\xc2\xae", "\xc2\xaf",
	// Bx
	"\xc2\xb0", "\xc2\xb1", "\xc2\xb2", "\xc2\xb3", "\xc2\xb4", "\xc2\xb5", "\xc2\xb6", "\xc2\xb7",
	"\xc2\xb8", "\xc2\xb9", "\xc2\xba", "\xc2\xbb", "\xc2\xbc", "\xc2\xbd", "\xc2\xbe", "\xc2\xbf",
	// Cx
	"\xc3\x80", "\xc3\x81", "\xc3\x82", "\xc3\x83", "\xc3\x84", "\xc3\x85", "\xc3\x86", "\xc3\x87",
	"\xc3\x88", "\xc3\x89", "\xc3\x8a", "\xc3\x8b", "\xc3\x8c", "\xc3\x8d", "\xc3\x8e", "\xc3\x8f",
	// Dx
	"\xc3\x90", "\xc3\x91", "\xc3\x92", "\xc3\x93", "\xc3\x94", "\xc3\x95", "\xc3\x96", "\xc3\x97",
	"\xc3\x98", "\xc3\x99", "\xc3\x9a", "\xc3\x9b", "\xc3\x9c", "\xc3\x9d", "\xc3\x9e", "\xc3\x9f",
	// Ex
	"\xc3\xa0", "\xc3\xa1", "\xc3\xa2", "\xc3\xa3", "\xc3\xa4", "\xc3\xa5", "\xc3\xa6", "\xc3\xa7",
	"\xc3\xa8", "\xc3\xa9", "\xc3\xaa", "\xc3\xab", "\xc3\xac", "\xc3\xad", "\xc3\xae", "\xc3\xaf",
	// Fx
	"\xc3\xb0", "\xc3\xb1", "\xc3\xb2", "\xc3\xb3", "\xc3\xb4", "\xc3\xb5", "\xc3\xb6", "\xc3\xb7",
	"\xc3\xb8", "\xc3\xb9", "\xc3\xba", "\xc3\xbb", "\xc3\xbc", "\xc3\xbd", "\xc3\xbe", "\xc3\xbf",
};

static const char * encoding_cp1252[128] = { // http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
	// 8x
	"\xe2\x82\xac", NULL,           "\xe2\x80\x9a", "\xc6\x92",     "\xe2\x80\x9e", "\xe2\x80\xa6", "\xe2\x80\xa0", "\xe2\x80\xa1",
	"\xcb\x86",     "\xe2\x80\xb0", "\xc5\xa0",     "\xe2\x80\xb9", "\xc5\x92",     NULL,           "\xc5\xbd",     NULL,
	// 9x
	NULL,           "\xe2\x80\x98", "\xe2\x80\x99", "\xe2\x80\x9c", "\xe2\x80\x9d", "\xe2\x80\xa2", "\xe2\x80\x93", "\xe2\x80\x94",
	"\xcb\x9c",     "\xe2\x84\xa2", "\xc5\xa1",     "\xe2\x80\xba", "\xc5\x93",     NULL,           "\xc5\xbe",     "\xc5\xb8",
	// Ax
	"\xc2\xa0",     "\xc2\xa1",     "\xc2\xa2",     "\xc2\xa3",     "\xc2\xa4",     "\xc2\xa5",     "\xc2\xa6",     "\xc2\xa7",
	"\xc2\xa8",     "\xc2\xa9",     "\xc2\xaa",     "\xc2\xab",     "\xc2\xac",     "\xc2\xad",     "\xc2\xae",     "\xc2\xaf",
	// Bx
	"\xc2\xb0",     "\xc2\xb1",     "\xc2\xb2",     "\xc2\xb3",     "\xc2\xb4",     "\xc2\xb5",     "\xc2\xb6",     "\xc2\xb7",
	"\xc2\xb8",     "\xc2\xb9",     "\xc2\xba",     "\xc2\xbb",     "\xc2\xbc",     "\xc2\xbd",     "\xc2\xbe",     "\xc2\xbf",
	// Cx
	"\xc3\x80",     "\xc3\x81",     "\xc3\x82",     "\xc3\x83",     "\xc3\x84",     "\xc3\x85",     "\xc3\x86",     "\xc3\x87",
	"\xc3\x88",     "\xc3\x89",     "\xc3\x8a",     "\xc3\x8b",     "\xc3\x8c",     "\xc3\x8d",     "\xc3\x8e",     "\xc3\x8f",
	// Dx
	"\xc3\x90",     "\xc3\x91",     "\xc3\x92",     "\xc3\x93",     "\xc3\x94",     "\xc3\x95",     "\xc3\x96",     "\xc3\x97",
	"\xc3\x98",     "\xc3\x99",     "\xc3\x9a",     "\xc3\x9b",     "\xc3\x9c",     "\xc3\x9d",     "\xc3\x9e",     "\xc3\x9f",
	// Ex
	"\xc3\xa0",     "\xc3\xa1",     "\xc3\xa2",     "\xc3\xa3",     "\xc3\xa4",     "\xc3\xa5",     "\xc3\xa6",     "\xc3\xa7",
	"\xc3\xa8",     "\xc3\xa9",     "\xc3\xaa",     "\xc3\xab",     "\xc3\xac",     "\xc3\xad",     "\xc3\xae",     "\xc3\xaf",
	// Fx
	"\xc3\xb0",     "\xc3\xb1",     "\xc3\xb2",     "\xc3\xb3",     "\xc3\xb4",     "\xc3\xb5",     "\xc3\xb6",     "\xc3\xb7",
	"\xc3\xb8",     "\xc3\xb9",     "\xc3\xba",     "\xc3\xbb",     "\xc3\xbc",     "\xc3\xbd",     "\xc3\xbe",     "\xc3\xbf",
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
{
	set_encoding(aCodepage);
}

void cainteoir::encoding::set_encoding(int aCodepage)
{
	mDecoder = std::tr1::shared_ptr<cainteoir::decoder>(new single_byte_decoder(get_encoding_table(aCodepage)));
}
