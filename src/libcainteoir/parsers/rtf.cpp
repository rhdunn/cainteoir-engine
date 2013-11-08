/* RichText Document Parser.
 *
 * Copyright (C) 2011-2013 Reece H. Dunn
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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include "parsers.hpp"
#include <stdexcept>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

struct replacement
{
	const char * token;
	const char * text;
};

static inline int hex_to_int(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'Z') return (c - 'A') + 10;
	if (c >= 'a' && c <= 'z') return (c - 'a') + 10;
	return -1;
}

static const replacement replacements[] = {
	{ "\\",         "\\" },
	{ "{",          "{" },
	{ "}",          "}" },
	{ "\r",         "\r" },
	{ "\n",         "\n" },
	{ "-",          nullptr }, // optional hyphen
	{ "_",          "-" },  // non-breaking hyphen
	{ "~",          "\xC0\xA0" }, // NON-BREAKING SPACE
	{ "bullet",     "\xE2\x80\xA2" },
	{ "emdash",     "\xE2\x80\x94" },
	{ "endash",     "\xE2\x80\x93" },
	{ "ldblquote",  "\xE2\x80\x9C" },
	{ "lquote",     "\xE2\x80\x98" },
	{ "rdblquote",  "\xE2\x80\x9D" },
	{ "rquote",     "\xE2\x80\x99" },
	{ "tab",        "\x09" },
};

static std::shared_ptr<cainteoir::buffer> lookupReplacementText(const cainteoir::encoding & aEncoding, const std::shared_ptr<cainteoir::buffer> & token, int value)
{
	if (!token->compare("'"))
		return aEncoding.lookup((char)value);
	else for (const replacement *first = std::begin(replacements), *last = std::end(replacements); first != last; ++first)
	{
		if (!token->compare(first->token) && first->text)
			return std::make_shared<cainteoir::buffer>(first->text);
	}
	return std::shared_ptr<cainteoir::buffer>();
}

struct rtf_reader : public cainteoir::buffer
{
	enum token_type
	{
		begin_block,
		end_block,
		instruction,
		text,
	};

	rtf_reader(const std::shared_ptr<cainteoir::buffer> &aData)
		: cainteoir::buffer(aData->begin(), aData->end())
		, mCurrent(aData->begin())
		, mData(std::make_shared<cainteoir::buffer>(nullptr, nullptr))
		, mParameter(0)
		, mDepth(0)
	{
	}

	bool read();

	token_type token() const { return mToken; }

	const std::shared_ptr<cainteoir::buffer> & data() const { return mData; }

	int parameter() const { return mParameter; }

	int depth() const { return mDepth; }
private:
	const char * mCurrent;
	token_type mToken;
	std::shared_ptr<cainteoir::buffer> mData;
	int mParameter;
	int mDepth;
};

static void printToken(rtf_reader &rtf)
{
	switch (rtf.token())
	{
	case rtf_reader::begin_block:
		printf("begin-block (depth=%d)\n", rtf.depth());
		break;
	case rtf_reader::end_block:
		printf("end-block (depth=%d)\n", rtf.depth());
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
	{
		mData = std::make_shared<cainteoir::buffer>(nullptr, nullptr);
		return false;
	}

	switch (*mCurrent)
	{
	case '{':
		mToken = begin_block;
		++mCurrent;
		++mDepth;
		break;
	case '}':
		mToken = end_block;
		++mCurrent;
		--mDepth;
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

				mData = std::make_shared<cainteoir::buffer>(control, mCurrent);

				if (mCurrent <= end() && ((*mCurrent >= '0' && *mCurrent <= '9') || *mCurrent == '-'))
				{
					bool isNegative = false;
					if (*mCurrent == '-')
					{
						isNegative = true;
						++mCurrent;
					}

					mParameter = 0;
					while (mCurrent <= end() && (*mCurrent >= '0' && *mCurrent <= '9'))
					{
						mParameter *= 10;
						mParameter += (*mCurrent - '0');
						++mCurrent;
					}

					if (isNegative)
						mParameter = -mParameter;
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
				mData = std::make_shared<cainteoir::buffer>(mCurrent, mCurrent+1);
				if (*mCurrent == '\'') // \'hh token
				{
					++mCurrent;
					if ((mCurrent + 2) <= end())
					{
						int a = hex_to_int(mCurrent[0]);
						int b = hex_to_int(mCurrent[1]);
						if (a > 0 && b > 0)
						{
							mParameter = (a << 4) + b;
							mCurrent += 2;
							return true;
						}
					}

					return read(); // skip invalid \'hh token
				}
				else
					++mCurrent;
			}
		}
		break;
	case '\r':
	case '\n':
		while (mCurrent <= end() && (*mCurrent == '\r' || *mCurrent == '\n'))
			++mCurrent;
		return read();
	default:
		{
			mToken = text;
			const char * text = mCurrent;

			while (mCurrent <= end() && *mCurrent != '{' && *mCurrent != '\\' && *mCurrent != '}' && *mCurrent != '\r' && *mCurrent != '\n')
				++mCurrent;

			mData = std::make_shared<cainteoir::buffer>(text, mCurrent);
		}
		break;
	}

	return true;
}

static void skipRtfBlock(rtf_reader &rtf)
{
	while (rtf.read()) switch (rtf.token())
	{
	case rtf_reader::begin_block:
		skipRtfBlock(rtf);
		break;
	case rtf_reader::end_block:
		return;
	default:
		break;
	}
}

struct rtf_document_reader : public cainteoir::document_reader
{
	enum state
	{
		state_rtf,
		state_title,
		state_text,
		state_info,
		state_eof,
	};

	rtf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle);

	bool read();

	bool internal_read(rdf::graph *aPrimaryMetadata);

	rtf_reader rtf;
	std::shared_ptr<cainteoir::buffer> mData;
	rdf::uri mSubject;
	cainteoir::encoding mCodepage;
	state mState;
	int mBlockCount;
	std::string mTitle;
	cainteoir::rope rtf_text;
};

rtf_document_reader::rtf_document_reader(std::shared_ptr<cainteoir::buffer> &aData, const rdf::uri &aSubject, rdf::graph &aPrimaryMetadata, const std::string &aTitle)
	: rtf(aData)
	, mData(aData)
	, mSubject(aSubject)
	, mCodepage(1252)
	, mState(state_rtf)
	, mBlockCount(0)
	, mTitle(aTitle)
{
	if (rtf.read() && internal_read(&aPrimaryMetadata))
		mState = state_title;
	aPrimaryMetadata.statement(aSubject, rdf::tts("mimetype"), rdf::literal("application/rtf"));
}

bool rtf_document_reader::read()
{
	return internal_read(nullptr);
}

bool rtf_document_reader::internal_read(rdf::graph *aGraph)
{
	if (mState == state_title)
	{
		if (mTitle.empty())
		{
			mTitle = mSubject.str();
			std::string::size_type sep = mTitle.rfind('/');
			if (sep != std::string::npos)
				mTitle = mTitle.substr(sep + 1);
		}

		type    = events::toc_entry | events::anchor;
		styles  = &cainteoir::heading0;
		content = cainteoir::make_buffer(mTitle);
		anchor  = mSubject;
		mState  = state_text;
		return true;
	}

	std::string info_context;
	if (!rtf.data()->compare("par") && !rtf_text.empty())
		goto text_event;

	do switch (rtf.token())
	{
	case rtf_reader::instruction:
		switch (mState)
		{
		case state_rtf:
			if (rtf.data()->comparei("rtf") || rtf.parameter() != 1)
				throw std::runtime_error(i18n("unrecognised rtf data stream or version"));
			mState = state_text;
			break;
		case state_info:
			info_context = rtf.data()->str();
			break;
		default:
			{
				auto replacement = lookupReplacementText(mCodepage, rtf.data(), rtf.parameter());
				if (replacement.get())
					rtf_text += replacement;
				else if (!rtf.data()->compare("par") && !rtf_text.empty())
					goto text_event;
				else if (!rtf.data()->comparei("stylesheet") ||
				         !rtf.data()->comparei("fonttbl") ||
				         !rtf.data()->comparei("colortbl") ||
				         !rtf.data()->comparei("pict") ||
				         !rtf.data()->comparei("header") ||
				         !rtf.data()->comparei("footer") ||
				         !rtf.data()->comparei("field") ||
				         !rtf.data()->comparei("xe") ||
				         !rtf.data()->comparei("tc") ||
				         !rtf.data()->comparei("object") ||
				         !rtf.data()->comparei("*"))
				{
					skipRtfBlock(rtf);
				}
				else if (!rtf.data()->comparei("info"))
				{
					mState = state_info;
					mBlockCount = 1;
				}
			}
			break;
		};
		break;
	case rtf_reader::begin_block:
		if (mState == state_info)
			++mBlockCount;
		break;
	case rtf_reader::end_block:
		if (mState == state_info && --mBlockCount == 0)
			mState = state_text;
		break;
	case rtf_reader::text:
		if (mState == state_info && aGraph)
		{
			if (info_context == "author")
			{
				const rdf::uri temp = aGraph->genid();
				aGraph->statement(mSubject, rdf::dc("creator"), temp);
				aGraph->statement(temp, rdf::rdf("value"), rdf::literal(rtf.data()->str()));
				aGraph->statement(temp, rdf::opf("role"), rdf::literal("aut"));
			}
			else if (info_context == "operator")
			{
				const rdf::uri temp = aGraph->genid();
				aGraph->statement(mSubject, rdf::dc("contributor"), temp);
				aGraph->statement(temp, rdf::rdf("value"), rdf::literal(rtf.data()->str()));
				aGraph->statement(temp, rdf::opf("role"), rdf::literal("edt"));
			}
			else if (info_context == "title")
			{
				mTitle = rtf.data()->str();
				aGraph->statement(mSubject, rdf::dc("title"), rdf::literal(mTitle));
			}
			else if (info_context == "subject")
				aGraph->statement(mSubject, rdf::dc("description"), rdf::literal(rtf.data()->str()));
			else if (info_context == "keywords")
			{
				std::istringstream keywords(rtf.data()->str());
				std::string keyword;
				while (keywords >> keyword)
					aGraph->statement(mSubject, rdf::dc("subject"), rdf::literal(keyword));
			}
		}
		else if (mState == state_text && rtf.depth() > 0)
			rtf_text += rtf.data();
		break;
	} while (rtf.read());

	if (rtf_text.empty())
	{
		type = 0;
		content.reset();
		return false;
	}

text_event:
	type    = events::text | events::begin_context | events::end_context;
	styles  = &cainteoir::paragraph;
	content = rtf_text.buffer();
	anchor  = rdf::uri();
	if (aGraph == nullptr)
		rtf_text.clear();
	return true;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createRtfReader(std::shared_ptr<buffer> &aData,
                           const rdf::uri &aSubject,
                           rdf::graph &aPrimaryMetadata,
                           const std::string &aTitle)
{
	return std::make_shared<rtf_document_reader>(aData, aSubject, aPrimaryMetadata, aTitle);
}
