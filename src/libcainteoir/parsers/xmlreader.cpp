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
	{ NULL,     NULL }, // End of Entity List
};

static const entity html_entities[] = {
	{ "amp",    "&" },
	{ "apos",   "'" },
	{ "brvbar", "\xC2\xA6" }, // U+00A6
	{ "cent",   "\xC2\xA2" }, // U+00A2
	{ "copy",   "\xC2\xA9" }, // U+00A9
	{ "curren", "\xC2\xA4" }, // U+00A4
	{ "gt",     ">" },
	{ "iexcl",  "\xC2\xA1" }, // U+00A1
	{ "lt",     "<" },
	{ "nbsp",   "\xC2\xA0" }, // U+00A0
	{ "ordf",   "\xC2\xAA" }, // U+00AA
	{ "pound",  "\xC2\xA3" }, // U+00A3
	{ "quot",   "\"" },
	{ "sect",   "\xC2\xA7" }, // U+00A7
	{ "uml",    "\xC2\xA8" }, // U+00A8
	{ "yen",    "\xC2\xA5" }, // U+00A5
	{ NULL,     NULL }, // End of Entity List
};

const char * resolve_entity(const entity *entities, const cainteoir::buffer &data)
{
	while (entities->name)
	{
		if (!strncmp(entities->name, data.begin(), data.size()))
			return entities->value;
		++entities;
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

					const char * value = resolve_entity(html_entities, entity);
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
