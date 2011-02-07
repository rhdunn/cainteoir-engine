/* Test for parsing XML/HTML documents.
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

#include <cainteoir/buffer.hpp>
#include <stdexcept>
#include <memory>

namespace cainteoir { namespace xml
{
	class reader
	{
	public:
		enum node_type
		{
			beginTagNode,
			endTagNode,
			tagNode, // isolated -- begin and end tag node
			processingInstructionNode,
			commentNode,
			textNode,
		};

		reader(std::auto_ptr<cainteoir::buffer> aData)
			: mData(aData)
		{
			mCurrent = mBeginMatch = mEndMatch = mData->begin();
			mNodeType = textNode;
		}

		bool read();

		std::string nodeValue() const { return std::string(mBeginMatch, mEndMatch); }

		node_type nodeType() const { return mNodeType; }
	private:
		std::auto_ptr<cainteoir::buffer> mData;
		const char * mCurrent;
		const char * mBeginMatch;
		const char * mEndMatch;
		node_type mNodeType;
	};
}}

bool cainteoir::xml::reader::read()
{
	if (mCurrent == mData->end())
		return false;

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
				mBeginMatch = ++mCurrent;
				while (mCurrent != mData->end() && (mCurrent[0] != '-' && mCurrent[1] != '-' && mCurrent[2] != '>'))
					++mCurrent;
				mEndMatch = mCurrent;
				++mCurrent;
				++mCurrent;
				++mCurrent;
			}
			else
			{
				mNodeType = beginTagNode;
				mBeginMatch = ++mCurrent;
				while (mCurrent != mData->end() && *mCurrent != '>')
					++mCurrent;
				mEndMatch = mCurrent;
			}
			break;
		case '?':
			mNodeType = processingInstructionNode;
			mBeginMatch = ++mCurrent;
			while (mCurrent != mData->end() && (mCurrent[0] != '?' && mCurrent[1] != '>'))
				++mCurrent;
			mEndMatch = mCurrent;
			++mCurrent;
			++mCurrent;
			break;
		case '/':
			mNodeType = endTagNode;
			mBeginMatch = ++mCurrent;
			while (mCurrent != mData->end() && *mCurrent != '>')
				++mCurrent;
			mEndMatch = mCurrent;
			++mCurrent;
			break;
		default:
			mNodeType = beginTagNode;
			mBeginMatch = mCurrent;
			while (mCurrent != mData->end() && *mCurrent != '>')
				++mCurrent;
			mEndMatch = mCurrent;
			if (*(mCurrent - 1) == '/')
			{
				mNodeType = tagNode;
				--mEndMatch;
			}
			++mCurrent;
			break;
		}
	}
	else
	{
		mNodeType = textNode;
		mBeginMatch = mCurrent;
		while (mCurrent != mData->end() && *mCurrent != '<')
			++mCurrent;
		mEndMatch = mCurrent;
	}

	return true;
}

namespace xml = cainteoir::xml;

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		xml::reader reader(std::auto_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[0])));
		while (reader.read())
		{
			switch (reader.nodeType())
			{
			case xml::reader::beginTagNode:
				fprintf(stdout, "|begin-tag|%s\n", reader.nodeValue().c_str());
				break;
			case xml::reader::endTagNode:
				fprintf(stdout, "|end-tag|%s\n", reader.nodeValue().c_str());
				break;
			case xml::reader::tagNode:
				fprintf(stdout, "|tag|%s\n", reader.nodeValue().c_str());
				break;
			case xml::reader::processingInstructionNode:
				fprintf(stdout, "|processing-instruction|%s\n", reader.nodeValue().c_str());
				break;
			case xml::reader::commentNode:
				fprintf(stdout, "|comment|%s\n", reader.nodeValue().c_str());
				break;
			case xml::reader::textNode:
				fprintf(stdout, "|text|%s\n", reader.nodeValue().c_str());
				break;
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
