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
	{ "AElig",  "\xC3\x86" }, // U+00C6
	{ "Aacute", "\xC3\x81" }, // U+00C1
	{ "Acirc",  "\xC3\x82" }, // U+00C2
	{ "Agrave", "\xC3\x80" }, // U+00C0
	{ "Aring",  "\xC3\x85" }, // U+00C5
	{ "Atilde", "\xC3\x83" }, // U+00C3
	{ "Auml",   "\xC3\x84" }, // U+00C4
	{ "Ccedil", "\xC3\x87" }, // U+00C7
	{ "ETH",    "\xC3\x90" }, // U+00D0
	{ "Eacute", "\xC3\x89" }, // U+00C9
	{ "Ecirc",  "\xC3\x8A" }, // U+00CA
	{ "Egrave", "\xC3\x88" }, // U+00C8
	{ "Euml",   "\xC3\x8B" }, // U+00CB
	{ "Iacute", "\xC3\x8D" }, // U+00CD
	{ "Icirc",  "\xC3\x8E" }, // U+00CE
	{ "Igrave", "\xC3\x8C" }, // U+00CC
	{ "Iuml",   "\xC3\x8F" }, // U+00CF
	{ "Ntilde", "\xC3\x91" }, // U+00D1
	{ "Oacute", "\xC3\x93" }, // U+00D3
	{ "Ocirc",  "\xC3\x94" }, // U+00D4
	{ "Ograve", "\xC3\x92" }, // U+00D2
	{ "Oslash", "\xC3\x98" }, // U+00D8
	{ "Otilde", "\xC3\x95" }, // U+00D5
	{ "Ouml",   "\xC3\x96" }, // U+00D6
	{ "THORN",  "\xC3\x9E" }, // U+00DE
	{ "Uacute", "\xC3\x9A" }, // U+00DA
	{ "Ucirc",  "\xC3\x9B" }, // U+00DB
	{ "Ugrave", "\xC3\x99" }, // U+00D9
	{ "Uuml",   "\xC3\x9C" }, // U+00DC
	{ "Yacute", "\xC3\x9D" }, // U+00DD
	{ "aacute", "\xC3\xA1" }, // U+00E1
	{ "acirc",  "\xC3\xA2" }, // U+00E2
	{ "acute",  "\xC2\xB4" }, // U+00B4
	{ "aelig",  "\xC3\xA6" }, // U+00E6
	{ "agrave", "\xC3\xA0" }, // U+00E0
	{ "amp",    "&" },
	{ "apos",   "'" },
	{ "aring",  "\xC3\xA5" }, // U+00E5
	{ "atilde", "\xC3\xA3" }, // U+00E3
	{ "auml",   "\xC3\xA4" }, // U+00E4
	{ "brvbar", "\xC2\xA6" }, // U+00A6
	{ "ccedil", "\xC3\xA7" }, // U+00E7
	{ "cedil",  "\xC2\xB8" }, // U+00B8
	{ "cent",   "\xC2\xA2" }, // U+00A2
	{ "copy",   "\xC2\xA9" }, // U+00A9
	{ "curren", "\xC2\xA4" }, // U+00A4
	{ "deg",    "\xC2\xB0" }, // U+00B0
	{ "divide", "\xC3\xB7" }, // U+00F7
	{ "eacute", "\xC3\xA9" }, // U+00E9
	{ "ecirc",  "\xC3\xAA" }, // U+00EA
	{ "egrave", "\xC3\xA8" }, // U+00E8
	{ "eth",    "\xC3\xB0" }, // U+00F0
	{ "euml",   "\xC3\xAB" }, // U+00EB
	{ "frac12", "\xC2\xBD" }, // U+00BD
	{ "frac14", "\xC2\xBC" }, // U+00BC
	{ "frac34", "\xC2\xBE" }, // U+00BE
	{ "gt",     ">" },
	{ "iacute", "\xC3\xAD" }, // U+00ED
	{ "icirc",  "\xC3\xAE" }, // U+00EE
	{ "iexcl",  "\xC2\xA1" }, // U+00A1
	{ "igrave", "\xC3\xAC" }, // U+00EC
	{ "iquest", "\xC2\xBF" }, // U+00BF
	{ "iuml",   "\xC3\xAF" }, // U+00EF
	{ "laquo",  "\xC2\xAB" }, // U+00AB
	{ "lt",     "<" },
	{ "macr",   "\xC2\xAF" }, // U+00AF
	{ "micro",  "\xC2\xB5" }, // U+00B5
	{ "middot", "\xC2\xB7" }, // U+00B7
	{ "nbsp",   "\xC2\xA0" }, // U+00A0
	{ "not",    "\xC2\xAC" }, // U+00AC
	{ "ntilde", "\xC3\xB1" }, // U+00F1
	{ "oacute", "\xC3\xB3" }, // U+00F3
	{ "ocirc",  "\xC3\xB4" }, // U+00F4
	{ "ograve", "\xC3\xB2" }, // U+00F2
	{ "ordf",   "\xC2\xAA" }, // U+00AA
	{ "ordm",   "\xC2\xBA" }, // U+00BA
	{ "oslash", "\xC3\xB8" }, // U+00F8
	{ "otilde", "\xC3\xB5" }, // U+00F5
	{ "ouml",   "\xC3\xB6" }, // U+00F6
	{ "para",   "\xC2\xB6" }, // U+00B6
	{ "plusmn", "\xC2\xB1" }, // U+00B1
	{ "pound",  "\xC2\xA3" }, // U+00A3
	{ "quot",   "\"" },
	{ "raquo",  "\xC2\xBB" }, // U+00BB
	{ "reg",    "\xC2\xAE" }, // U+00AE
	{ "sect",   "\xC2\xA7" }, // U+00A7
	{ "shy",    "\xC2\xAD" }, // U+00AD
	{ "sup1",   "\xC2\xB9" }, // U+00B9
	{ "sup2",   "\xC2\xB2" }, // U+00B2
	{ "sup3",   "\xC2\xB3" }, // U+00B3
	{ "szlig",  "\xC3\x9F" }, // U+00DF
	{ "thorn",  "\xC3\xBE" }, // U+00FE
	{ "times",  "\xC3\x97" }, // U+00D7
	{ "uacute", "\xC3\xBA" }, // U+00FA
	{ "ucirc",  "\xC3\xBB" }, // U+00FB
	{ "ugrave", "\xC3\xB9" }, // U+00F9
	{ "uml",    "\xC2\xA8" }, // U+00A8
	{ "uuml",   "\xC3\xBC" }, // U+00FC
	{ "yacute", "\xC3\xBD" }, // U+00FD
	{ "yen",    "\xC2\xA5" }, // U+00A5
	{ "yuml",   "\xC3\xBF" }, // U+00FF
};

const char * resolve_entity(const entity *first, const entity *last, const cainteoir::buffer &data)
{
	int begin = 0;
	int end = last - first;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;
		int comp = strncmp((first + pos)->name, data.begin(), data.size());

		if (comp == 0)
			return (first + pos)->value;
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return NULL;
}

cainteoir::xml::reader::reader(std::tr1::shared_ptr<cainteoir::buffer> aData)
	: mData(aData)
	, mNodeValue(NULL, NULL)
{
	mCurrent = mData->begin();
	mNodeType = textNode;

	while (mCurrent != mData->end() && (*mCurrent == ' ' || *mCurrent == '\t' || *mCurrent == '\r' || *mCurrent == '\n'))
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

		std::list<cainteoir::buffer> rope;
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
					cainteoir::buffer entity(startPos+1, mCurrent);
					++mCurrent;

					const char * value = resolve_entity(html_entities, html_entities + (sizeof(html_entities)/sizeof(html_entities[0])), entity);
					if (value)
					{
						rope.push_back(cainteoir::buffer(value));
						len += rope.back().size();
					}
					else
						fprintf(stderr, "unrecognised entity '%s'\n", entity.str().c_str());
				}
			}
			else
			{
				while (mCurrent != mData->end() && !(*mCurrent == '&' || *mCurrent == '<'))
					++mCurrent;
				rope.push_back(cainteoir::buffer(startPos, mCurrent));
				len += rope.back().size();
			}
		} while (mCurrent != mData->end() && *mCurrent != '<');

		switch (rope.size())
		{
		case 0: mNodeValue = cainteoir::buffer(NULL, NULL); break;
		case 1: mNodeValue = rope.back(); break;
		default:
			{
				mNodeValueBuffer = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::data_buffer(len));
				char * startPos = (char *)mNodeValueBuffer->begin();
				for (auto node = rope.begin(), last = rope.end(); node != last; ++node)
				{
					strncpy(startPos, node->begin(), node->size());
					startPos += node->size();
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
		mNodeValue = cainteoir::buffer(startPos, mCurrent - 1);
	}
	else
		mNodeValue = cainteoir::buffer(startPos, mCurrent);
	++mCurrent;
}
