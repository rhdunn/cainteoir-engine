/* OCF Document Parser.
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

#include "parsers.hpp"
#include <cainteoir/platform.hpp>

namespace rdf = cainteoir::rdf;
namespace xmldom = cainteoir::xmldom;

std::map<std::string, std::string> parseOcfRootFiles(const xmldom::node &ocf)
{
	std::map<std::string, std::string> files;

	for (xmldom::node node = ocf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE && node == rdf::ocf("rootfile"))
		{
			std::string path = node.attr(rdf::uri(std::string(), "full-path")).content();
			std::string mimetype = node.attr(rdf::uri(std::string(), "media-type")).content();
			files[mimetype] = path;
		}
	}

	return files;
}

std::map<std::string, std::string> cainteoir::parseOcfDocument(const xmldom::node &ocf)
{
	if (ocf != rdf::ocf("container"))
		throw std::runtime_error(_("OCF file is not of a recognised format."));

	for (xmldom::node node = ocf.firstChild(); node.isValid(); node.next())
	{
		if (node.type() == XML_ELEMENT_NODE)
		{
			if (node == rdf::ocf("rootfiles"))
				return parseOcfRootFiles(node);
		}
	}

	return std::map<std::string, std::string>();
}

