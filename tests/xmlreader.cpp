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
		reader(std::auto_ptr<cainteoir::buffer> aData)
			: data(aData)
		{
			current = match = data->begin();
		}

		bool read();

		std::string nodeValue() const { return std::string(match, current); }
	private:
		std::auto_ptr<cainteoir::buffer> data;
		const char * current;
		const char * match;
	};
}}

bool cainteoir::xml::reader::read()
{
	if (current == data->end())
		return false;

	match = current;

	if (*current == '<')
	{
		while (current != data->end() && *current != '>')
			++current;
		++current;
	}
	else
	{
		while (current != data->end() && *current != '<')
			++current;
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
			fprintf(stdout, "|data|%s\n", reader.nodeValue().c_str());
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
