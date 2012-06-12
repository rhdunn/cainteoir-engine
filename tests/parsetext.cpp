/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#include <cainteoir/document.hpp>
#include <stdexcept>
#include <cstdio>

namespace rdf    = cainteoir::rdf;
namespace events = cainteoir::events;

enum state
{
	state_root,
	state_word,
	state_number,
};

enum general_category
{
	Category_ControlCode,
	Category_Letter_UpperCase,
	Category_Letter_LowerCase,
	Category_Number_Decimal,
	Category_Punctuation_Close,
	Category_Punctuation_Connector,
	Category_Punctuation_Dash,
	Category_Punctuation_Open,
	Category_Punctuation_Other,
	Category_Separator_Space,
	Category_Symbol_Currency,
	Category_Symbol_Math,
	Category_Symbol_Modifier,
	Category_Symbol_Other,
	Category_Unknown,

	// http://www.unicode.org/reports/tr44/tr44-8.html#General_Category_Values:

	Cc = Category_ControlCode,
	Cn = Category_Unknown,
	Lu = Category_Letter_UpperCase,
	Ll = Category_Letter_LowerCase,
	Nd = Category_Number_Decimal,
	Pc = Category_Punctuation_Connector,
	Pd = Category_Punctuation_Dash,
	Pe = Category_Punctuation_Close,
	Po = Category_Punctuation_Other,
	Ps = Category_Punctuation_Open,
	Sc = Category_Symbol_Currency,
	Sk = Category_Symbol_Modifier,
	Sm = Category_Symbol_Math,
	So = Category_Symbol_Other,
	Zs = Category_Separator_Space,
};

const general_category category[128] = {
//       00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
/* 0x */ Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc,
/* 1x */ Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc, Cc,
/* 2x */ Zs, Po, Po, Po, Sc, Po, Po, Po, Ps, Pe, Po, Sm, Po, Pd, Po, Po,
/* 3x */ Nd, Nd, Nd, Nd, Nd, Nd, Nd, Nd, Nd, Nd, Po, Po, Sm, Sm, Sm, Po,
/* 4x */ Po, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu,
/* 5x */ Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Lu, Ps, Po, Pe, Sk, Pc,
/* 6x */ Sk, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll,
/* 7x */ Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ll, Ps, Sm, Pe, Sm, Cc,
};

class text_reader
{
public:
	text_reader();

	void set_buffer(const std::shared_ptr<cainteoir::buffer> &aText);

	bool read();
private:
	std::shared_ptr<cainteoir::buffer> data;
	const uint8_t *begin;
	const uint8_t *end;
	const uint8_t *start;
	state s;
	uint8_t quote;
};

text_reader::text_reader()
	: begin(nullptr)
	, end(nullptr)
	, start(nullptr)
	, s(state_root)
	, quote(0)
{
}

void text_reader::set_buffer(const std::shared_ptr<cainteoir::buffer> &aText)
{
	data  = aText;
	begin = (const uint8_t *)aText->begin();
	end   = (const uint8_t *)aText->end();
	start = begin;
}

bool text_reader::read()
{
	while (begin != end)
	{
		general_category gc = (*begin < 0x80) ? category[*begin] : Cn;

		switch (s)
		{
		case state_root:
			switch (gc)
			{
			case Category_Punctuation_Other:
				switch (*begin)
				{
				case '\'':
					start = begin;
					s = state_word;
					break;
				case '"':
					switch (quote)
					{
					case 0:
						fprintf(stdout, ".begin-quote   %c\n", (char)*begin);
						quote = '"';
						++begin;
						return true;
					case '"':
						fprintf(stdout, ".end-quote     %c\n", (char)*begin);
						quote = 0;
						++begin;
						return true;
					default:
						fprintf(stdout, ".symbol        %c\n", (char)*begin);
						++begin;
						return true;
					}
					break;
				default:
					fprintf(stdout, ".punctuation   %c\n", (char)*begin);
					++begin;
					return true;
				}
				break;
			case Category_Letter_UpperCase:
			case Category_Letter_LowerCase:
				start = begin;
				s = state_word;
				break;
			case Category_Number_Decimal:
				start = begin;
				s = state_number;
				break;
			case Category_ControlCode:
			case Category_Separator_Space:
			case Category_Punctuation_Connector:
				break;
			case Category_Punctuation_Dash:
			case Category_Punctuation_Open:
			case Category_Punctuation_Close:
				fprintf(stdout, ".punctuation   %c\n", (char)*begin);
				++begin;
				return true;
			case Category_Symbol_Currency:
			case Category_Symbol_Modifier:
			case Category_Symbol_Math:
			case Category_Symbol_Other:
				fprintf(stdout, ".symbol        %c\n", (char)*begin);
				++begin;
				return true;
			case Category_Unknown:
				fprintf(stdout, ".error         \\x%02X\n", *begin);
				++begin;
				return true;
			}
			break;
		case state_word:
			switch (gc)
			{
			case Category_Punctuation_Other:
				switch (*begin)
				{
				case '\'':
					break;
				default:
					s = state_root;
					break;
				}
				break;
			case Category_Letter_UpperCase:
			case Category_Letter_LowerCase:
				break;
			default:
				s = state_root;
				break;
			}
			if (s == state_root)
			{
				fputs(".word          ", stdout);
				fwrite(start, 1, begin-start, stdout);
				fputs("\n", stdout);
				return true;
			}
			break;
		case state_number:
			switch (gc)
			{
			case Category_Punctuation_Other:
				switch (*begin)
				{
				case ',':
					break;
				default:
					s = state_root;
					break;
				}
				break;
			case Category_Number_Decimal:
				break;
			default:
				s = state_root;
				break;
			}
			if (s == state_root)
			{
				fputs(".number        ", stdout);
				fwrite(start, 1, begin-start, stdout);
				fputs("\n", stdout);
				return true;
			}
			break;
		}
		++begin;
	}

	switch (s)
	{
	case state_word:
		fputs(".word          ", stdout);
		fwrite(start, 1, begin-start, stdout);
		fputs("\n", stdout);
		return true;
	case state_number:
		fputs(".number        ", stdout);
		fwrite(start, 1, begin-start, stdout);
		fputs("\n", stdout);
		return true;
	}

	return false;
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc == 0)
			throw std::runtime_error("no document specified");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		text_reader text;
		while (reader->read())
		{
			if (reader->type & cainteoir::events::text)
			{
				text.set_buffer(reader->text);
				while (text.read())
				{
				}
			}
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
