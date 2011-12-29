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
namespace xml = cainteoir::xml;

std::map<std::string, std::string> parseOcfRootFiles(xml::reader &ocf)
{
	std::map<std::string, std::string> files;
	bool is_rootfile = false;
	std::string path;
	std::string mimetype;

	while (ocf.read()) switch (ocf.nodeType())
	{
	case xml::reader::beginTagNode:
		if (ocf == rdf::ocf("rootfile"))
		{
			is_rootfile = true;
			path.clear();
			mimetype.clear();
		}
		break;
	case xml::reader::endTagNode:
		if (ocf == rdf::ocf("rootfile"))
		{
			files[mimetype] = path;
			is_rootfile = false;
		}
		else if (ocf == rdf::ocf("rootfiles"))
			return files;
		break;
	case xml::reader::attribute:
		if (is_rootfile)
		{
			     if (ocf == rdf::ocf("full-path"))
				path = ocf.nodeValue().str();
			else if (ocf == rdf::ocf("media-type"))
				mimetype = ocf.nodeValue().str();
		}
		break;
	}

	return files;
}

std::map<std::string, std::string> cainteoir::parseOcfDocument(std::tr1::shared_ptr<cainteoir::buffer> aData)
{
	xml::reader ocf(aData);
	while (ocf.read() && ocf.nodeType() != xml::reader::beginTagNode)
		;

	if (ocf != rdf::ocf("container"))
		throw std::runtime_error(_("OCF file is not of a recognised format."));

	do
	{
		if (ocf.nodeType() == xml::reader::beginTagNode && ocf == rdf::ocf("rootfiles"))
			return parseOcfRootFiles(ocf);
	} while (ocf.read());

	return std::map<std::string, std::string>();
}
