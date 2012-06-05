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

void parseTextBuffer(const std::shared_ptr<cainteoir::buffer> &aText)
{
	const uint8_t *begin = (const uint8_t *)aText->begin();
	const uint8_t *end   = (const uint8_t *)aText->end();
	const uint8_t *start = begin;
	state s = state_root;

	uint8_t quote = 0;

	while (begin != end)
	{
		switch (s)
		{
		case state_root:
			switch (*begin)
			{
			case '"':
				switch (quote)
				{
				case 0:
					fprintf(stdout, ".begin-quote   %c\n", (char)*begin);
					quote = '"';
					break;
				case '"':
					fprintf(stdout, ".end-quote     %c\n", (char)*begin);
					quote = 0;
					break;
				default:
					fprintf(stdout, ".symbol        %c\n", (char)*begin);
					break;
				}
				break;
			// alpha:
			case 'a': case 'b': case 'c': case 'd':
			case 'e': case 'f': case 'g': case 'h':
			case 'i': case 'j': case 'k': case 'l': case 'm':
			case 'n': case 'o': case 'p': case 'q':
			case 'r': case 's': case 't': case 'u':
			case 'v': case 'w': case 'x': case 'y': case 'z':
			case 'A': case 'B': case 'C': case 'D':
			case 'E': case 'F': case 'G': case 'H':
			case 'I': case 'J': case 'K': case 'L': case 'M':
			case 'N': case 'O': case 'P': case 'Q':
			case 'R': case 'S': case 'T': case 'U':
			case 'V': case 'W': case 'X': case 'Y': case 'Z':
			case '\'':
				start = begin;
				s = state_word;
				break;
			// number:
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				start = begin;
				s = state_number;
				break;
			// space:
			case ' ': case '\t': case '\r': case '\n':
			case '_': // treat as space for programming and escaped file names
				break;
			// punctuation:
			case '.': case '!': case '?': case ',': case ':': case ';':
				fprintf(stdout, ".punctuation   %c\n", (char)*begin);
				break;
			// other:
			default:
				if (*begin < 0x80)
					fprintf(stdout, ".symbol        %c\n", (char)*begin);
				break;
			}
			break;
		case state_word:
			switch (*begin)
			{
			// alpha:
			case 'a': case 'b': case 'c': case 'd':
			case 'e': case 'f': case 'g': case 'h':
			case 'i': case 'j': case 'k': case 'l': case 'm':
			case 'n': case 'o': case 'p': case 'q':
			case 'r': case 's': case 't': case 'u':
			case 'v': case 'w': case 'x': case 'y': case 'z':
			case 'A': case 'B': case 'C': case 'D':
			case 'E': case 'F': case 'G': case 'H':
			case 'I': case 'J': case 'K': case 'L': case 'M':
			case 'N': case 'O': case 'P': case 'Q':
			case 'R': case 'S': case 'T': case 'U':
			case 'V': case 'W': case 'X': case 'Y': case 'Z':
			case '\'':
				break;
			default:
				fputs(".word          ", stdout);
				fwrite(start, 1, begin-start, stdout);
				fputs("\n", stdout);
				s = state_root;
				continue;
			}
			break;
		case state_number:
			switch (*begin)
			{
			// number:
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				break;
			default:
				fputs(".number        ", stdout);
				fwrite(start, 1, begin-start, stdout);
				fputs("\n", stdout);
				s = state_root;
				continue;
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
		break;
	case state_number:
		fputs(".number        ", stdout);
		fwrite(start, 1, begin-start, stdout);
		fputs("\n", stdout);
		break;
	}
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

		while (reader->read())
		{
			if (reader->type & cainteoir::events::text)
				parseTextBuffer(reader->text);
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
