/* Test for the exception dictionary API.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#include <cainteoir/document.hpp>
#include <cainteoir/text.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace tts = cainteoir::tts;

struct test_results
{
	uint32_t    words;
	std::size_t index_size;
};

test_results parse_words(const std::shared_ptr<cainteoir::document_reader> &reader)
{
	uint32_t words = 0;
	tts::dictionary dict;
	tts::text_reader text(reader);
	while (text.read()) switch (text.event().type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		dict.add_entry(text.event().text->str(), ucd::Zzzz, text.event().text);
		++words;
		break;
	}
	return { words, dict.size() };
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		rdf::graph metadata;
		auto reader = cainteoir::createDocumentReader(argv[0], metadata, std::string());
		if (!reader)
		{
			fprintf(stderr, "unsupported document format for file \"%s\"\n", argv[0]);
			return 0;
		}

		cainteoir::stopwatch timer;
		test_results results = parse_words(reader);
		printf("time:    %G\n", timer.elapsed());
		printf("words:   %d\n", results.words);
		printf("indexed: %d\n", results.index_size);
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
