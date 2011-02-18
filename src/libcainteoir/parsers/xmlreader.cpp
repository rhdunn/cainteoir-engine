/* XML Reader API.
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

#include <cainteoir/xmlreader.hpp>
#include <list>

struct entity
{
	const char * name;
	const char * value;
};

static const entity xml10_entities[] = {
	{ "amp",    "&" },
	{ "apos",   "'" },
	{ "gt",     ">" },
	{ "lt",     "<" },
	{ "quot",   "\"" },
};

static const entity html_entities[] = {
	{ "AElig",    "\xC3\x86" }, // U+00C6
	{ "Aacute",   "\xC3\x81" }, // U+00C1
	{ "Acirc",    "\xC3\x82" }, // U+00C2
	{ "Agrave",   "\xC3\x80" }, // U+00C0
	{ "Alpha",    "\xCE\x91" }, // U+0391
	{ "Aring",    "\xC3\x85" }, // U+00C5
	{ "Atilde",   "\xC3\x83" }, // U+00C3
	{ "Auml",     "\xC3\x84" }, // U+00C4
	{ "Beta",     "\xCE\x92" }, // U+0392
	{ "Ccedil",   "\xC3\x87" }, // U+00C7
	{ "Chi",      "\xCE\xA7" }, // U+0397
	{ "Dagger",   "\xE2\x80\xA1" }, // U+2021
	{ "Delta",    "\xCE\x94" }, // U+0394
	{ "ETH",      "\xC3\x90" }, // U+00D0
	{ "Eacute",   "\xC3\x89" }, // U+00C9
	{ "Ecirc",    "\xC3\x8A" }, // U+00CA
	{ "Egrave",   "\xC3\x88" }, // U+00C8
	{ "Epsilon",  "\xCE\x95" }, // U+0395
	{ "Eta",      "\xCE\x97" }, // U+0397
	{ "Euml",     "\xC3\x8B" }, // U+00CB
	{ "Gamma",    "\xCE\x93" }, // U+0393
	{ "Iacute",   "\xC3\x8D" }, // U+00CD
	{ "Icirc",    "\xC3\x8E" }, // U+00CE
	{ "Igrave",   "\xC3\x8C" }, // U+00CC
	{ "Iota",     "\xCE\x99" }, // U+0399
	{ "Iuml",     "\xC3\x8F" }, // U+00CF
	{ "Kappa",    "\xCE\x9A" }, // U+039A
	{ "Lambda",   "\xCE\x9B" }, // U+039B
	{ "Mu",       "\xCE\x9C" }, // U+039C
	{ "Ntilde",   "\xC3\x91" }, // U+00D1
	{ "Nu",       "\xCE\x9D" }, // U+039D
	{ "OElig",    "\xC5\x92" }, // U+0152
	{ "Oacute",   "\xC3\x93" }, // U+00D3
	{ "Ocirc",    "\xC3\x94" }, // U+00D4
	{ "Ograve",   "\xC3\x92" }, // U+00D2
	{ "Omega",    "\xCE\xA9" }, // U+0399
	{ "Omicron",  "\xCE\x9F" }, // U+039F
	{ "Oslash",   "\xC3\x98" }, // U+00D8
	{ "Otilde",   "\xC3\x95" }, // U+00D5
	{ "Ouml",     "\xC3\x96" }, // U+00D6
	{ "Phi",      "\xCE\xA6" }, // U+0396
	{ "Pi",       "\xCE\xA0" }, // U+03A0
	{ "Prime",    "\xE2\x80\xB3" }, // U+2033
	{ "Psi",      "\xCE\xA8" }, // U+0398
	{ "Rho",      "\xCE\xA1" }, // U+03A1
	{ "Scaron",   "\xC5\xA0" }, // U+0160
	{ "Sigma",    "\xCE\xA3" }, // U+03A3
	{ "THORN",    "\xC3\x9E" }, // U+00DE
	{ "Tau",      "\xCE\xA4" }, // U+03A4
	{ "Theta",    "\xCE\x98" }, // U+0398
	{ "Uacute",   "\xC3\x9A" }, // U+00DA
	{ "Ucirc",    "\xC3\x9B" }, // U+00DB
	{ "Ugrave",   "\xC3\x99" }, // U+00D9
	{ "Upsilon",  "\xCE\xA5" }, // U+0395
	{ "Uuml",     "\xC3\x9C" }, // U+00DC
	{ "Xi",       "\xCE\x9E" }, // U+039E
	{ "Yacute",   "\xC3\x9D" }, // U+00DD
	{ "Yuml",     "\xC5\xB8" }, // U+0178
	{ "Zeta",     "\xCE\x96" }, // U+0396
	{ "aacute",   "\xC3\xA1" }, // U+00E1
	{ "acirc",    "\xC3\xA2" }, // U+00E2
	{ "acute",    "\xC2\xB4" }, // U+00B4
	{ "aelig",    "\xC3\xA6" }, // U+00E6
	{ "agrave",   "\xC3\xA0" }, // U+00E0
	{ "alefsym",  "\xE2\x84\xB5" }, // U+2135
	{ "alpha",    "\xCE\xB1" }, // U+03B1
	{ "amp",      "&" },
	{ "and",      "\xE2\x88\xA7" }, // U+2227
	{ "ang",      "\xE2\x88\xA0" }, // U+2220
	{ "apos",     "'" },
	{ "aring",    "\xC3\xA5" }, // U+00E5
	{ "asymp",    "\xE2\x89\x88" }, // U+2248
	{ "atilde",   "\xC3\xA3" }, // U+00E3
	{ "auml",     "\xC3\xA4" }, // U+00E4
	{ "bdquo",    "\xE2\x80\x9E" }, // U+201E
	{ "beta",     "\xCE\xB2" }, // U+03B2
	{ "brvbar",   "\xC2\xA6" }, // U+00A6
	{ "bull",     "\xE2\x80\xA2" }, // U+2022
	{ "cap",      "\xE2\x88\xA9" }, // U+2229
	{ "ccedil",   "\xC3\xA7" }, // U+00E7
	{ "cedil",    "\xC2\xB8" }, // U+00B8
	{ "cent",     "\xC2\xA2" }, // U+00A2
	{ "chi",      "\xCF\x87" }, // U+03C7
	{ "circ",     "\xCB\x86" }, // U+02C6
	{ "clubs",    "\xE2\x99\xA3" }, // U+2663
	{ "cong",     "\xE2\x89\x85" }, // U+2245
	{ "copy",     "\xC2\xA9" }, // U+00A9
	{ "crarr",    "\xE2\x86\xB5" }, // U+21B5
	{ "cup",      "\xE2\x88\xAA" }, // U+222A
	{ "curren",   "\xC2\xA4" }, // U+00A4
	{ "dArr",     "\xE2\x87\x93" }, // U+21D3
	{ "dagger",   "\xE2\x80\xA0" }, // U+2020
	{ "darr",     "\xE2\x86\x93" }, // U+2193
	{ "deg",      "\xC2\xB0" }, // U+00B0
	{ "delta",    "\xCE\xB4" }, // U+03B4
	{ "diams",    "\xE2\x99\xA6" }, // U+2666
	{ "divide",   "\xC3\xB7" }, // U+00F7
	{ "eacute",   "\xC3\xA9" }, // U+00E9
	{ "ecirc",    "\xC3\xAA" }, // U+00EA
	{ "egrave",   "\xC3\xA8" }, // U+00E8
	{ "empty",    "\xE2\x88\x85" }, // U+2205
	{ "emsp",     "\xE2\x80\x83" }, // U+2003
	{ "ensp",     "\xE2\x80\x82" }, // U+2002
	{ "epsilon",  "\xCE\xB5" }, // U+03B5
	{ "equiv",    "\xE2\x89\xA1" }, // U+2261
	{ "eta",      "\xCE\xB7" }, // U+03B7
	{ "eth",      "\xC3\xB0" }, // U+00F0
	{ "euml",     "\xC3\xAB" }, // U+00EB
	{ "euro",     "\xE2\x82\xAC" }, // U+20AC
	{ "exist",    "\xE2\x88\x83" }, // U+2203
	{ "fnof",     "\xC6\x92" }, // U+0192
	{ "forall",   "\xE2\x88\x80" }, // U+2200
	{ "frac12",   "\xC2\xBD" }, // U+00BD
	{ "frac14",   "\xC2\xBC" }, // U+00BC
	{ "frac34",   "\xC2\xBE" }, // U+00BE
	{ "frasl",    "\xE2\x81\x84" }, // U+2044
	{ "gamma",    "\xCE\xB3" }, // U+03B3
	{ "ge",       "\xE2\x89\xA5" }, // U+2265
	{ "gt",       ">" },
	{ "hArr",     "\xE2\x87\x94" }, // U+21D4
	{ "harr",     "\xE2\x86\x94" }, // U+2194
	{ "hearts",   "\xE2\x99\xA5" }, // U+2665
	{ "hellip",   "\xE2\x80\xA6" }, // U+2026
	{ "iacute",   "\xC3\xAD" }, // U+00ED
	{ "icirc",    "\xC3\xAE" }, // U+00EE
	{ "iexcl",    "\xC2\xA1" }, // U+00A1
	{ "igrave",   "\xC3\xAC" }, // U+00EC
	{ "image",    "\xE2\x84\x91" }, // U+2111
	{ "infin",    "\xE2\x88\x9E" }, // U+221E
	{ "int",      "\xE2\x88\xAB" }, // U+222B
	{ "iota",     "\xCE\xB9" }, // U+03B9
	{ "iquest",   "\xC2\xBF" }, // U+00BF
	{ "isin",     "\xE2\x88\x88" }, // U+2208
	{ "iuml",     "\xC3\xAF" }, // U+00EF
	{ "kappa",    "\xCE\xBA" }, // U+03BA
	{ "lArr",     "\xE2\x87\x90" }, // U+21D0
	{ "lambda",   "\xCE\xBB" }, // U+03BB
	{ "lang",     "\xE2\x9F\xA8" }, // U+27E8
	{ "laquo",    "\xC2\xAB" }, // U+00AB
	{ "larr",     "\xE2\x86\x90" }, // U+2190
	{ "lceil",    "\xE2\x8C\x88" }, // U+2308
	{ "ldquo",    "\xE2\x80\x9C" }, // U+201C
	{ "le",       "\xE2\x89\xA4" }, // U+2264
	{ "lfloor",   "\xE2\x8C\x8A" }, // U+230A
	{ "lowast",   "\xE2\x88\x97" }, // U+2217
	{ "loz",      "\xE2\x97\x8A" }, // U+25CA
	{ "lrm",      "\xE2\x80\x8E" }, // U+200E
	{ "lsaquo",   "\xE2\x80\xB9" }, // U+2039
	{ "lsquo",    "\xE2\x80\x98" }, // U+2018
	{ "lt",       "<" },
	{ "macr",     "\xC2\xAF" }, // U+00AF
	{ "mdash",    "\xE2\x80\x94" }, // U+2014
	{ "micro",    "\xC2\xB5" }, // U+00B5
	{ "middot",   "\xC2\xB7" }, // U+00B7
	{ "minus",    "\xE2\x88\x92" }, // U+2212
	{ "mu",       "\xCE\xBC" }, // U+03BC
	{ "nabla",    "\xE2\x88\x87" }, // U+2207
	{ "nbsp",     "\xC2\xA0" }, // U+00A0
	{ "ndash",    "\xE2\x80\x93" }, // U+2013
	{ "ne",       "\xE2\x89\xA0" }, // U+2260
	{ "ni",       "\xE2\x88\x8B" }, // U+220B
	{ "not",      "\xC2\xAC" }, // U+00AC
	{ "notin",    "\xE2\x88\x89" }, // U+2209
	{ "nsub",     "\xE2\x8A\x84" }, // U+2284
	{ "ntilde",   "\xC3\xB1" }, // U+00F1
	{ "nu",       "\xCE\xBD" }, // U+03BD
	{ "oacute",   "\xC3\xB3" }, // U+00F3
	{ "ocirc",    "\xC3\xB4" }, // U+00F4
	{ "oelig",    "\xC5\x93" }, // U+0153
	{ "ograve",   "\xC3\xB2" }, // U+00F2
	{ "oline",    "\xE2\x80\xBE" }, // U+203E
	{ "omega",    "\xCF\x89" }, // U+03C9
	{ "omicron",  "\xCE\xBF" }, // U+03BF
	{ "oplus",    "\xE2\x8A\x95" }, // U+2295
	{ "or",       "\xE2\x88\xA8" }, // U+2228
	{ "ordf",     "\xC2\xAA" }, // U+00AA
	{ "ordm",     "\xC2\xBA" }, // U+00BA
	{ "oslash",   "\xC3\xB8" }, // U+00F8
	{ "otilde",   "\xC3\xB5" }, // U+00F5
	{ "otimes",   "\xE2\x8A\x97" }, // U+2297
	{ "ouml",     "\xC3\xB6" }, // U+00F6
	{ "para",     "\xC2\xB6" }, // U+00B6
	{ "part",     "\xE2\x88\x82" }, // U+2202
	{ "permil",   "\xE2\x80\xB0" }, // U+2030
	{ "perp",     "\xE2\x8A\xA5" }, // U+22A5
	{ "phi",      "\xCF\x86" }, // U+03C6
	{ "pi",       "\xCF\x80" }, // U+03C0
	{ "piv",      "\xCF\x96" }, // U+03D6
	{ "plusmn",   "\xC2\xB1" }, // U+00B1
	{ "pound",    "\xC2\xA3" }, // U+00A3
	{ "prime",    "\xE2\x80\xB2" }, // U+2032
	{ "prod",     "\xE2\x88\x8F" }, // U+220F
	{ "prop",     "\xE2\x88\x9D" }, // U+221D
	{ "psi",      "\xCF\x88" }, // U+03C8
	{ "quot",     "\"" },
	{ "rArr",     "\xE2\x87\x92" }, // U+21D2
	{ "radic",    "\xE2\x88\x9A" }, // U+221A
	{ "rang",     "\xE2\x9F\xA9" }, // U+27E9
	{ "raquo",    "\xC2\xBB" }, // U+00BB
	{ "rarr",     "\xE2\x86\x92" }, // U+2192
	{ "rceil",    "\xE2\x8C\x89" }, // U+2309
	{ "rdquo",    "\xE2\x80\x9D" }, // U+201D
	{ "real",     "\xE2\x84\x9C" }, // U+211C
	{ "reg",      "\xC2\xAE" }, // U+00AE
	{ "rfloor",   "\xE2\x8C\x8B" }, // U+230B
	{ "rho",      "\xCF\x81" }, // U+03C1
	{ "rlm",      "\xE2\x80\x8F" }, // U+200F
	{ "rsaquo",   "\xE2\x80\xBA" }, // U+203A
	{ "rsquo",    "\xE2\x80\x99" }, // U+2019
	{ "sbquo",    "\xE2\x80\x9A" }, // U+201A
	{ "scaron",   "\xC5\xA1" }, // U+0161
	{ "sdot",     "\xE2\x8B\x85" }, // U+22C5
	{ "sect",     "\xC2\xA7" }, // U+00A7
	{ "shy",      "\xC2\xAD" }, // U+00AD
	{ "sigma",    "\xCF\x83" }, // U+03C3
	{ "sim",      "\xE2\x88\xBC" }, // U+223C
	{ "spades",   "\xE2\x99\xA0" }, // U+2660
	{ "sub",      "\xE2\x8A\x82" }, // U+2282
	{ "sube",     "\xE2\x8A\x86" }, // U+2286
	{ "sum",      "\xE2\x88\x91" }, // U+2211
	{ "sup",      "\xE2\x8A\x83" }, // U+2283
	{ "sup1",     "\xC2\xB9" }, // U+00B9
	{ "sup2",     "\xC2\xB2" }, // U+00B2
	{ "sup3",     "\xC2\xB3" }, // U+00B3
	{ "supe",     "\xE2\x8A\x87" }, // U+2287
	{ "szlig",    "\xC3\x9F" }, // U+00DF
	{ "tau",      "\xCF\x84" }, // U+03C4
	{ "there4",   "\xE2\x88\xB4" }, // U+2234
	{ "theta",    "\xCE\xB8" }, // U+03B8
	{ "thetasym", "\xCF\x91" }, // U+03D1
	{ "thinsp",   "\xE2\x80\x89" }, // U+2009
	{ "thorn",    "\xC3\xBE" }, // U+00FE
	{ "tilde",    "\xCB\x9C" }, // U+02DC
	{ "times",    "\xC3\x97" }, // U+00D7
	{ "trade",    "\xE2\x84\xA2" }, // U+2122
	{ "uArr",     "\xE2\x87\x91" }, // U+21D1
	{ "uacute",   "\xC3\xBA" }, // U+00FA
	{ "uarr",     "\xE2\x86\x91" }, // U+2191
	{ "ucirc",    "\xC3\xBB" }, // U+00FB
	{ "ugrave",   "\xC3\xB9" }, // U+00F9
	{ "uml",      "\xC2\xA8" }, // U+00A8
	{ "upsih",    "\xCF\x92" }, // U+03D2
	{ "upsilon",  "\xCF\x85" }, // U+03B5
	{ "uuml",     "\xC3\xBC" }, // U+00FC
	{ "weierp",   "\xE2\x84\x98" }, // U+2118
	{ "xi",       "\xCE\xBE" }, // U+03BE
	{ "yacute",   "\xC3\xBD" }, // U+00FD
	{ "yen",      "\xC2\xA5" }, // U+00A5
	{ "yuml",     "\xC3\xBF" }, // U+00FF
	{ "zeta",     "\xCE\xB6" }, // U+03B6
	{ "zwj",      "\xE2\x80\x8D" }, // U+200D
	{ "zwnj",     "\xE2\x80\x8C" }, // U+200C
};

const char * resolve_entity(const entity *first, const entity *last, const cainteoir::buffer &data)
{
	int begin = 0;
	int end = last - first;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;
		int comp = strncmp((first + pos)->name, data.begin(), data.size());

		if (comp == 0 && data.size() == strlen((first + pos)->name))
			return (first + pos)->value;
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return NULL;
}

void write_utf8(char * out, long c)
{
	if (c < 0x80)
		*out++ = c;
	else if (c < 0x800)
	{
		*out++ = 0xC0 | (c >> 6);
		*out++ = 0x80 + (c & 0x3F);
	}
	else if (c < 0x10000)
	{
		*out++ = 0xE0 | (c >> 12);
		*out++ = 0x80 + ((c >> 6) & 0x3F);
		*out++ = 0x80 + (c & 0x3F);
	}
	else if (c < 0x200000)
	{
		*out++ = 0xF0 | (c >> 18);
		*out++ = 0x80 + ((c >> 12) & 0x3F);
		*out++ = 0x80 + ((c >>  6) & 0x3F);
		*out++ = 0x80 + (c & 0x3F);
	}
	*out = '\0';
}

std::tr1::shared_ptr<cainteoir::buffer> parse_entity(const cainteoir::buffer &entity)
{
	const char * str = entity.begin();
	if (*str == '#')
	{
		char utf8[10];
		if (*++str == 'x')
			write_utf8(utf8, strtol(++str, NULL, 16));
		else
			write_utf8(utf8, strtol(str, NULL, 10));

		if (utf8[0])
		{
			std::tr1::shared_ptr<cainteoir::buffer> data(new cainteoir::data_buffer(strlen(utf8)));
			strcpy((char *)data->begin(), utf8);
			return data;
		}
	}
	else
	{
		const char * value = resolve_entity(html_entities, html_entities + (sizeof(html_entities)/sizeof(html_entities[0])), entity);
		if (value)
			return std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(value));
	}

	fprintf(stderr, "unrecognised entity '%s'\n", entity.str().c_str());
	return std::tr1::shared_ptr<cainteoir::buffer>();
}

inline bool xmlspace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

cainteoir::xml::reader::reader(std::tr1::shared_ptr<cainteoir::buffer> aData)
	: mData(aData)
	, mNodeValue(NULL, NULL)
	, mNodeName(NULL, NULL)
{
	mCurrent = mData->begin();
	mNodeType = textNode;

	while (mCurrent != mData->end() && xmlspace(*mCurrent))
		++mCurrent;

	const char * startPos = mCurrent;
	if (*mCurrent == '<' && read() && mNodeType != error)
	{
		mCurrent = startPos;
		mParseAsText = false; // looks like XML or HTML
	}
	else
	{
		mCurrent = mData->begin();
		mParseAsText = true; // looks like text
	}
}

bool cainteoir::xml::reader::read()
{
	if (mCurrent >= mData->end())
		return false;

	mNodeName = cainteoir::buffer(NULL, NULL);
	if (mParseAsText)
	{
		mNodeType = textNode;
		mNodeValue = cainteoir::buffer(mCurrent, mData->end());
		mCurrent = mData->end();
		return true;
	}

	const char * startPos = NULL;

	if (*mCurrent == '<')
	{
		switch (*++mCurrent)
		{
		case '!':
			if (mCurrent[1] == '-' && mCurrent[2] == '-')
			{
				++mCurrent;
				++mCurrent;
				mNodeType = commentNode;
				startPos = ++mCurrent;
				while (mCurrent != mData->end() && !(mCurrent[0] == '-' && mCurrent[1] == '-' && mCurrent[2] == '>'))
					++mCurrent;
				mNodeValue = cainteoir::buffer(startPos, mCurrent);
				mCurrent += 3;
			}
			else if (mCurrent[1] == '[' && mCurrent[2] == 'C' && mCurrent[3] == 'D' && mCurrent[4] == 'A' && mCurrent[5] == 'T' && mCurrent[6] == 'A' && mCurrent[7] == '[')
			{
				mCurrent += 8;
				mNodeType = cdataNode;
				startPos = mCurrent;
				while (mCurrent != mData->end() && !(mCurrent[0] == ']' && mCurrent[1] == ']' && mCurrent[2] == '>'))
					++mCurrent;
				mNodeValue = cainteoir::buffer(startPos, mCurrent);
				mCurrent += 3;
			}
			else
			{
				mNodeType = beginTagNode;
				startPos = ++mCurrent;
				while (mCurrent != mData->end() && *mCurrent != '>')
					++mCurrent;
				mNodeValue = cainteoir::buffer(startPos, mCurrent);
			}
			break;
		case '?':
			mNodeType = processingInstructionNode;
			startPos = ++mCurrent;
			while (mCurrent != mData->end() && (mCurrent[0] != '?' && mCurrent[1] != '>'))
				++mCurrent;
			mNodeValue = cainteoir::buffer(startPos, mCurrent);
			++mCurrent;
			++mCurrent;
			break;
		case '/':
			++mCurrent;
			read_tag(endTagNode);
			break;
		default:
			read_tag(beginTagNode);
			break;
		}
	}
	else
	{
		mNodeType = textNode;

		std::list< std::tr1::shared_ptr<cainteoir::buffer> > rope;
		long len = 0;

		do
		{
			startPos = mCurrent;
			if (*mCurrent == '&')
			{
				while (mCurrent != mData->end() && !(*mCurrent == ';' || *mCurrent == '<'))
					++mCurrent;

				if (*mCurrent == ';')
				{
					std::tr1::shared_ptr<cainteoir::buffer> entity = parse_entity(cainteoir::buffer(startPos+1, mCurrent));
					if (entity)
					{
						rope.push_back(entity);
						len += rope.back()->size();
					}
					++mCurrent;
				}
			}
			else
			{
				while (mCurrent != mData->end() && !(*mCurrent == '&' || *mCurrent == '<'))
					++mCurrent;
				rope.push_back(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mCurrent)));
				len += rope.back()->size();
			}
		} while (mCurrent != mData->end() && *mCurrent != '<');

		switch (rope.size())
		{
		case 0:
			mNodeValueBuffer = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(NULL, NULL));
			mNodeValue = *mNodeValueBuffer;
			break;
		case 1:
			mNodeValueBuffer = rope.back();
			mNodeValue = *mNodeValueBuffer;
			break;
		default:
			{
				mNodeValueBuffer = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::data_buffer(len));
				char * startPos = (char *)mNodeValueBuffer->begin();
				for (auto node = rope.begin(), last = rope.end(); node != last; ++node)
				{
					strncpy(startPos, (*node)->begin(), (*node)->size());
					startPos += (*node)->size();
				}
				mNodeValue = *mNodeValueBuffer;
			} break;
		}
	}

	return true;
}

void cainteoir::xml::reader::read_tag(node_type aType)
{
	mNodeType = aType;
	const char * startPos = mCurrent;
	while (mCurrent != mData->end() && *mCurrent != '>')
		++mCurrent;
	if (*(mCurrent - 1) == '/')
	{
		mNodeType = tagNode;
		mNodeName = cainteoir::buffer(startPos, mCurrent - 1);
	}
	else
		mNodeName = cainteoir::buffer(startPos, mCurrent);
	++mCurrent;
}

/** References
  *
  *    [1] http://www.w3.org/TR/2008/REC-xml-20081126/
  *        Extensible Markup Language (XML) 1.0 (Fifth Edition)
  */
