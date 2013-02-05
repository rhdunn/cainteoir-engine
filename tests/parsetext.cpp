/* Test for extracting words, numbers and other entries from a document.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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

#include <cainteoir/document.hpp>
#include <cainteoir/text.hpp>

#include <stdexcept>
#include <cstdio>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

static const char *token_name[] = {
	"error",
	"upper",
	"lower",
	"capital",
	"mixed",
	"number",
	"punctuation",
	"symbol",
};

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

		tts::text_reader text;
		while (reader->read())
		{
			text.next_item(*reader);
			while (text.read()) switch (text.type())
			{
			case tts::text_reader::word_uppercase:
			case tts::text_reader::word_lowercase:
			case tts::text_reader::word_capitalized:
			case tts::text_reader::word_mixedcase:
				fprintf(stdout, ".%s.%-8s %s\n",
				        ucd::get_script_string(text.script()),
				        token_name[text.type()],
				        text.match().str().c_str());
				break;
			default:
				fprintf(stdout, ".%-13s %s\n",
				        token_name[text.type()],
				        text.match().str().c_str());
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
