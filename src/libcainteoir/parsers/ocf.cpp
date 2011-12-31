/* OCF Document Parser.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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

cainteoir::ocf_reader::ocf_reader(std::tr1::shared_ptr<cainteoir::buffer> aData)
	: mReader(aData)
{
	while (mReader.read() && mReader.nodeType() != xml::reader::beginTagNode)
		;

	if (mReader != rdf::ocf("container"))
		throw std::runtime_error(_("OCF file is not of a recognised format."));

	do {
		if (mReader.nodeType() == xml::reader::beginTagNode && mReader == rdf::ocf("rootfiles"))
			break;
	} while (mReader.read());
}

bool cainteoir::ocf_reader::read()
{
	bool is_rootfile = false;
	mPath.clear();
	mMediaType.clear();

	while (mReader.read()) switch (mReader.nodeType())
	{
	case xml::reader::beginTagNode:
		is_rootfile = mReader == rdf::ocf("rootfile");
		break;
	case xml::reader::endTagNode:
		if (mReader == rdf::ocf("rootfile"))
			return true;
		if (mReader == rdf::ocf("rootfiles"))
			return false;
		break;
	case xml::reader::attribute:
		if (is_rootfile)
		{
			     if (mReader == rdf::ocf("full-path"))
				mPath = mReader.nodeValue().str();
			else if (mReader == rdf::ocf("media-type"))
				mMediaType = mReader.nodeValue().str();
		}
		break;
	}

	return false;
}
