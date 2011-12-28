/* XML Reader API.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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
#include <cainteoir/platform.hpp>

struct entity
{
	const char * name;
	const char * value;
};

struct entity_set
{
	const entity * first;
	const entity * last;
};

#include "html-entities.h"

const char * resolve_entity(const entity_set **entities, const cainteoir::buffer &data)
{
	char c = *data.begin();

	const entity_set * ent = NULL;
	if (c >= 'a' && c <= 'z')
		ent = entities[c - 'a' + 26];
	else if (c >= 'A' && c <= 'Z')
		ent = entities[c - 'A'];
	else
		return NULL;

	int begin = 0;
	int end = ent->last - ent->first;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;

		int comp = data.compare((ent->first + pos)->name);
		if (comp == 0)
			return (ent->first + pos)->value;
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
		const char * value = resolve_entity(html_entities, entity);
		if (value)
			return std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(value));
	}

	fprintf(stderr, _("unrecognised entity '%s'\n"), entity.str().c_str());
	return std::tr1::shared_ptr<cainteoir::buffer>();
}

inline bool xmlalnum(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

inline bool xmlspace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

cainteoir::xml::reader::reader(std::tr1::shared_ptr<cainteoir::buffer> aData)
	: mData(aData)
	, mNodeName(NULL, NULL)
	, mTagNodeName(NULL, NULL)
	, mParseAsText(false)
{
	mCurrent = mData->begin();
	mNodeType = textNode;

	skip_whitespace();

	const char * startPos = mCurrent;
	if (*mCurrent == '<' && read() && mNodeType != error)
	{
		mCurrent = startPos;
		mTagNodeName = cainteoir::buffer(NULL, NULL);
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
		mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(mCurrent, mData->end()));
		mCurrent = mData->end();
		return true;
	}

	const char * startPos = NULL;

	if (mTagNodeName.begin())
	{
		skip_whitespace();

		if (mCurrent[0] == '/' && mCurrent[1] == '>')
		{
			mNodeName = mTagNodeName;
			mNodeType = endTagNode;
			++mCurrent;
			return true;
		}

		if (xmlalnum(*mCurrent))
		{
			const char * startPos = mCurrent;

			while (mCurrent != mData->end() && (xmlalnum(*mCurrent) || *mCurrent == ':' || *mCurrent == '-'))
				++mCurrent;

			mNodeType = attribute;
			mNodeName = cainteoir::buffer(startPos, mCurrent);

			if (expect_next('=') && expect_next('"'))
			{
				const char * startPos = mCurrent;

				while (mCurrent != mData->end() && *mCurrent != '"')
					++mCurrent;

				mNodeType = attribute;
				mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mCurrent));
				++mCurrent;
			}

			return true;
		}

		if (*mCurrent == '>')
		{
			++mCurrent;
			mTagNodeName = cainteoir::buffer(NULL, NULL);
		}
		else
		{
			while (mCurrent != mData->end() && *mCurrent != '>')
				++mCurrent;
			++mCurrent;

			mNodeType = error;
			mTagNodeName = cainteoir::buffer(NULL, NULL);
			return true;
		}
	}

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
				mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mCurrent));
				mCurrent += 3;
			}
			else if (mCurrent[1] == '[' && mCurrent[2] == 'C' && mCurrent[3] == 'D' && mCurrent[4] == 'A' && mCurrent[5] == 'T' && mCurrent[6] == 'A' && mCurrent[7] == '[')
			{
				mCurrent += 8;
				mNodeType = cdataNode;
				startPos = mCurrent;
				while (mCurrent != mData->end() && !(mCurrent[0] == ']' && mCurrent[1] == ']' && mCurrent[2] == '>'))
					++mCurrent;
				mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mCurrent));
				mCurrent += 3;
			}
			else
			{
				++mCurrent;
				cainteoir::buffer type = identifier();

				if (!type.comparei("DOCTYPE"))
				{
					mNodeName = identifier();
					mNodeType = doctypeNode;
				}
				else
					mNodeType = error;

				while (mCurrent != mData->end() && *mCurrent != '>')
					++mCurrent;
				++mCurrent;
			}
			break;
		case '?':
			mNodeType = processingInstructionNode;
			startPos = ++mCurrent;
			while (mCurrent != mData->end() && (mCurrent[0] != '?' && mCurrent[1] != '>'))
				++mCurrent;
			mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mCurrent));
			++mCurrent;
			++mCurrent;
			break;
		case '/':
			++mCurrent;
			mNodeType = endTagNode;
			mTagNodeName = mNodeName = identifier();
			break;
		default:
			mNodeType = beginTagNode;
			mTagNodeName = mNodeName = identifier();
			break;
		}
	}
	else
	{
		mNodeType = textNode;
		mNodeValue.clear();

		do
		{
			startPos = mCurrent;
			if (*mCurrent == '&')
			{
				++mCurrent;
				if (*mCurrent == '#')
					++mCurrent;

				while (mCurrent != mData->end() && xmlalnum(*mCurrent))
					++mCurrent;

				if (*mCurrent == ';')
				{
					std::tr1::shared_ptr<cainteoir::buffer> entity = parse_entity(cainteoir::buffer(startPos+1, mCurrent));
					if (entity)
						mNodeValue += entity;
					++mCurrent;
					continue;
				}
			}

			while (mCurrent != mData->end() && !(*mCurrent == '&' || *mCurrent == '<'))
				++mCurrent;
			mNodeValue += std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mCurrent));
		} while (mCurrent != mData->end() && *mCurrent != '<');
	}

	return true;
}

void cainteoir::xml::reader::skip_whitespace()
{
	while (mCurrent != mData->end() && xmlspace(*mCurrent))
		++mCurrent;
}

bool cainteoir::xml::reader::expect_next(char c)
{
	skip_whitespace();

	if (*mCurrent == c)
	{
		++mCurrent;
		return true;
	}

	mNodeType = error;
	return false;
}

cainteoir::buffer cainteoir::xml::reader::identifier()
{
	skip_whitespace();

	const char * startPos = mCurrent;

	while (mCurrent != mData->end() && xmlalnum(*mCurrent))
		++mCurrent;

	return cainteoir::buffer(startPos, mCurrent);
}

/** References
  *
  *    [1] http://www.w3.org/TR/2008/REC-xml-20081126/
  *        Extensible Markup Language (XML) 1.0 (Fifth Edition)
  */
