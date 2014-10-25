/* Algorithm Performance Tests.
 *
 * Copyright (C) 2014 Reece H. Dunn
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
#include "options.hpp"

#include <cainteoir/sigproc.hpp>
#include <cainteoir/stopwatch.hpp>
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace css = cainteoir::css;

int main(int argc, char ** argv)
{
	try
	{
		uint32_t n = 0;
		decltype(cainteoir::abs) *f = nullptr;

		if (argc == 3)
		{
			if (strcmp(argv[1], "abs") == 0)
				f = cainteoir::abs;
			else if (strcmp(argv[1], "abs2") == 0)
				f = cainteoir::abs2;
			else if (strcmp(argv[1], "logr") == 0)
				f = cainteoir::logr;
			else if (strcmp(argv[1], "fft") == 0)
				f = cainteoir::fft;
			else if (strcmp(argv[1], "ifft") == 0)
				f = cainteoir::ifft;

			n = strtol(argv[2], nullptr, 10);
		}

		if (!f)
		{
			fprintf(stdout, "usage: algorithm abs  NUM_ELEMENTS\n");
			fprintf(stdout, "usage: algorithm abs2 NUM_ELEMENTS\n");
			fprintf(stdout, "usage: algorithm logr NUM_ELEMENTS\n");
			fprintf(stdout, "usage: algorithm fft  NUM_ELEMENTS\n");
			fprintf(stdout, "usage: algorithm ifft NUM_ELEMENTS\n");
			return EXIT_FAILURE;
		}

		srand(time(nullptr));

		cainteoir::complex_array data;
		data.resize(n);
		for (auto & c : data)
		{
			c.re = ((float)rand() / RAND_MAX) * 256;
			c.im = ((float)rand() / RAND_MAX) * 256;
		}

		cainteoir::stopwatch s;
		f(data);
		fprintf(stdout, "%G\n", s.elapsed());
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
