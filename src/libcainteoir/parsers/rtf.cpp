/* RichText Document Parser.
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

#include "parsers.hpp"
#include <cainteoir/platform.hpp>

namespace rdf = cainteoir::rdf;

struct rtf_reader : public cainteoir::buffer
{
	enum token_type
	{
		begin_block,
		end_block,
		instruction,
		text,
	};

	rtf_reader(std::tr1::shared_ptr<cainteoir::buffer> aData)
		: cainteoir::buffer(aData->begin(), aData->end())
		, mCurrent(aData->begin())
		, mData(new cainteoir::buffer(NULL, NULL))
	{
	}

	bool read();

	token_type token() const { return mToken; }

	const std::tr1::shared_ptr<cainteoir::buffer> & data() const { return mData; }

	int parameter() const { return mParameter; }
private:
	const char * mCurrent;
	token_type mToken;
	std::tr1::shared_ptr<cainteoir::buffer> mData;
	int mParameter;
};

void printToken(rtf_reader &rtf)
{
	switch (rtf.token())
	{
	case rtf_reader::begin_block:
		printf("begin-block\n");
		break;
	case rtf_reader::end_block:
		printf("end-block\n");
		break;
	case rtf_reader::instruction:
		printf("instruction: %s (parameter: %d)\n", rtf.data()->str().c_str(), rtf.parameter());
		break;
	case rtf_reader::text:
		printf("text: %s\n", rtf.data()->str().c_str());
		break;
	}
}

bool rtf_reader::read()
{
	if (mCurrent >= end())
		return false;

	switch (*mCurrent)
	{
	case '{':
		mToken = begin_block;
		++mCurrent;
		break;
	case '}':
		mToken = end_block;
		++mCurrent;
		break;
	case '\\':
		{
			mToken = instruction;
			++mCurrent;

			if (mCurrent >= end()) return false;

			if ((*mCurrent >= 'a' && *mCurrent <= 'z') || (*mCurrent >= 'A' && *mCurrent <= 'Z')) // control word
			{
				const char * control = mCurrent;
				while (mCurrent <= end() && ((*mCurrent >= 'a' && *mCurrent <= 'z') || (*mCurrent >= 'A' && *mCurrent <= 'Z')))
					++mCurrent;

				*mData = cainteoir::buffer(control, mCurrent);

				if (mCurrent <= end() && (*mCurrent >= '0' && *mCurrent <= '9'))
				{
					mParameter = 0;
					while (mCurrent <= end() && (*mCurrent >= '0' && *mCurrent <= '9'))
					{
						mParameter *= 10;
						mParameter += (*mCurrent - '0');
						++mCurrent;
					}
				}
				else
					mParameter = 1; // toggle property, e.g. \b

				if (*mCurrent == ' ' || *mCurrent == '\t' || *mCurrent == '\n')
					++mCurrent;
				else if (mCurrent[0] == '\r' && mCurrent[1] == '\n')
					mCurrent += 2;
			}
			else // control symbol
			{
				if (*mCurrent == '\\' || *mCurrent == '{' || *mCurrent == '}')
					mToken = text;

				*mData = cainteoir::buffer(mCurrent, mCurrent+1);
				++mCurrent;
			}
		}
		break;
	default:
		{
			mToken = text;
			const char * text = mCurrent;

			while (mCurrent <= end() && (*mCurrent == ' ' || *mCurrent == '\n' || *mCurrent == '\r' || *mCurrent == '\t'))
				++mCurrent;

			if (mCurrent <= end() && *mCurrent != '{' && *mCurrent != '\\' && *mCurrent != '}')
			{
				while (mCurrent <= end() && *mCurrent != '{' && *mCurrent != '\\' && *mCurrent != '}')
					++mCurrent;

				*mData = cainteoir::buffer(text, mCurrent);
			}
			else
				return read();

		}
		break;
	}

	return true;
}

void skipRtfBlock(rtf_reader &rtf)
{
	while (rtf.read()) switch (rtf.token())
	{
	case rtf_reader::begin_block:
		skipRtfBlock(rtf);
		break;
	case rtf_reader::end_block:
		return;
	}
}

void parseRtfBlock(rtf_reader &rtf, const rdf::uri &aSubject, cainteoir::document_events &events, bool mainRtfBlock)
{
	if (rtf.read() && rtf.token() == rtf_reader::instruction)
	{
		if (mainRtfBlock)
		{
			if (rtf.data()->comparei("rtf") || rtf.parameter() != 1)
				throw std::runtime_error(_("unrecognised rtf data stream or version"));
		}
		else
		{
			if (!rtf.data()->comparei("stylesheet") ||
			    !rtf.data()->comparei("fonttbl") ||
			    !rtf.data()->comparei("colortbl") ||
			    !rtf.data()->comparei("pict") ||
			    !rtf.data()->comparei("header") ||
			    !rtf.data()->comparei("footer") ||
			    !rtf.data()->comparei("field") ||
			    !rtf.data()->comparei("xe") ||
			    !rtf.data()->comparei("tc") ||
			    !rtf.data()->comparei("object") ||
			    !rtf.data()->comparei("info") || // FIXME: Process this block to extract metadata information.
			    !rtf.data()->comparei("*"))
			{
				skipRtfBlock(rtf);
				return;
			}
		}

		while (rtf.read()) switch (rtf.token())
		{
		case rtf_reader::begin_block:
			parseRtfBlock(rtf, aSubject, events, false);
			break;
		case rtf_reader::end_block:
			return;
		case rtf_reader::text:
			events.text(rtf.data());
			break;
		}
	}

	throw std::runtime_error(_("unexpected end of rtf stream"));
}

void cainteoir::parseRtfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	rtf_reader rtf(aData);

	if (rtf.read() && rtf.token() == rtf_reader::begin_block)
		parseRtfBlock(rtf, aSubject, events, true);
	else
		throw std::runtime_error(_("unrecognised rtf data stream"));
}
