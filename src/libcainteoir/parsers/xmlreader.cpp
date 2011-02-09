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

bool cainteoir::xml::reader::read()
{
	if (mCurrent >= mData->end())
		return false;

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
		startPos = mCurrent;
		while (mCurrent != mData->end() && *mCurrent != '<')
			++mCurrent;
		mNodeValue = cainteoir::buffer(startPos, mCurrent);
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
