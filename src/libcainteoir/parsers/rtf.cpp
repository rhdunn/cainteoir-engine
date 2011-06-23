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
#include <cainteoir/encoding.hpp>

namespace rdf = cainteoir::rdf;

struct replacement
{
	const char * token;
	const char * text;
};

inline int hex_to_int(char c)
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
	{ "-",          NULL }, // optional hyphen
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

#define countof(a) (sizeof(a)/sizeof(a[0]))

const char * lookupReplacementText(const cainteoir::encoding & aEncoding, const std::tr1::shared_ptr<cainteoir::buffer> & token, int value)
{
	if (!token->compare("'"))
		return aEncoding.lookup((char)value);
	else for (const replacement * first = replacements, * last = replacements + countof(replacements); first != last; ++first)
	{
		if (!token->compare(first->token))
		{
			return first->text;
		}
	}
	return NULL;
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

				mData = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(control, mCurrent));

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
				mData = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(mCurrent, mCurrent+1));
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

			while (mCurrent <= end() && (*mCurrent == ' ' || *mCurrent == '\t'))
				++mCurrent;

			if (mCurrent <= end() && *mCurrent != '{' && *mCurrent != '\\' && *mCurrent != '}')
			{
				while (mCurrent <= end() && *mCurrent != '{' && *mCurrent != '\\' && *mCurrent != '}' && *mCurrent != '\r' && *mCurrent != '\n')
					++mCurrent;

				mData = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(text, mCurrent));
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

void parseRtfBlock(rtf_reader &rtf, const rdf::uri &aSubject, cainteoir::document_events &events, cainteoir::rope &aText, cainteoir::encoding &codepage, bool mainRtfBlock)
{
	if (!rtf.read()) return;

	if (rtf.token() == rtf_reader::instruction)
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

		if (!rtf.read()) return;
	}

	do switch (rtf.token())
	{
	case rtf_reader::begin_block:
		parseRtfBlock(rtf, aSubject, events, aText, codepage, false);
		break;
	case rtf_reader::end_block:
		return;
	case rtf_reader::instruction:
		{
			const char * text = lookupReplacementText(codepage, rtf.data(), rtf.parameter());
			if (text)
				aText.add(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(text)));
			else if (!rtf.data()->compare("par") && !aText.empty())
			{
				events.text(aText.buffer());
				aText.clear();
			}
		}
		break;
	case rtf_reader::text:
		aText.add(rtf.data());
		break;
	} while (rtf.read());

	throw std::runtime_error(_("warning: unexpected end of rtf stream\n"));
}

void cainteoir::parseRtfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData, const rdf::uri &aSubject, cainteoir::document_events &events)
{
	rtf_reader rtf(aData);
	cainteoir::rope text;
	cainteoir::encoding codepage;

	if (rtf.read() && rtf.token() == rtf_reader::begin_block)
	{
		parseRtfBlock(rtf, aSubject, events, text, codepage, true);
		if (!text.empty())
			events.text(text.buffer());
	}
	else
		throw std::runtime_error(_("unrecognised rtf data stream"));
}
