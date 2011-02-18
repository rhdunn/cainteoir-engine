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

#include <cainteoir/xmlreader.hpp>
#include <stdexcept>

namespace xml = cainteoir::xml;

const char * node_type_name(xml::reader::node_type type)
{
	switch (type)
	{
	case xml::reader::beginTagNode:              return "begin-tag";
	case xml::reader::endTagNode:                return "end-tag";
	case xml::reader::tagNode:                   return "tag";
	case xml::reader::processingInstructionNode: return "processing-instruction";
	case xml::reader::commentNode:               return "comment";
	case xml::reader::cdataNode:                 return "cdata";
	case xml::reader::textNode:                  return "text";
	default:                                     return "unknown";
	}
}

int main(int argc, char ** argv)
{
	try
	{
		argc -= 1;
		argv += 1;

		if (argc != 1)
			throw std::runtime_error("no document specified");

		xml::reader reader(std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::mmap_buffer(argv[0])));
		while (reader.read())
		{
			switch (reader.nodeType())
			{
			case xml::reader::beginTagNode:
			case xml::reader::endTagNode:
			case xml::reader::tagNode:
			case xml::reader::processingInstructionNode:
				fprintf(stdout, "|%s| %s\n", node_type_name(reader.nodeType()), reader.nodeName().str().c_str());
				break;
			case xml::reader::commentNode:
			case xml::reader::cdataNode:
			case xml::reader::textNode:
				fprintf(stdout, "|%s| \"\"\"%s\"\"\"\n", node_type_name(reader.nodeType()), reader.nodeValue().str().c_str());
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
