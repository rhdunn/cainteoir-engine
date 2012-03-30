/* OCF Document Parser.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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
#include <stdexcept>

namespace rdf   = cainteoir::rdf;
namespace xmlns = cainteoir::xml::xmlns;
namespace xml   = cainteoir::xml;

namespace ocf
{
	namespace events = cainteoir::events;

	static const xml::context::entry container_node = { events::unknown, 0 };
	static const xml::context::entry rootfile_node  = { events::unknown, 0 };
	static const xml::context::entry rootfiles_node = { events::unknown, 0 };

	static const xml::context::entry fullpath_attr  = { events::unknown, 0 };
	static const xml::context::entry mediatype_attr = { events::unknown, 0 };
}

static const std::initializer_list<const xml::context::entry_ref> ocf_nodes =
{
	{ "container", &ocf::container_node },
	{ "rootfile",  &ocf::rootfile_node },
	{ "rootfiles", &ocf::rootfiles_node },
};

static const std::initializer_list<const xml::context::entry_ref> ocf_attrs =
{
	{ "full-path",  &ocf::fullpath_attr },
	{ "media-type", &ocf::mediatype_attr },
};

cainteoir::ocf_reader::ocf_reader(std::shared_ptr<cainteoir::buffer> aData)
	: mReader(aData)
{
	mReader.set_nodes(xmlns::ocf, ocf_nodes);
	mReader.set_attrs(xmlns::ocf, ocf_attrs);

	while (mReader.read() && mReader.nodeType() != xml::reader::beginTagNode)
		;

	if (mReader.context() != &ocf::container_node)
		throw std::runtime_error(_("OCF file is not of a recognised format."));

	do {
		if (mReader.nodeType() == xml::reader::beginTagNode && mReader.context() == &ocf::rootfiles_node)
			break;
	} while (mReader.read());
}

bool cainteoir::ocf_reader::read()
{
	mPath.clear();
	mMediaType.clear();

	const xml::context::entry *ctx = &xml::unknown_context;

	while (mReader.read()) switch (mReader.nodeType())
	{
	case xml::reader::beginTagNode:
		ctx = mReader.context();
		break;
	case xml::reader::endTagNode:
		if (mReader.context() == &ocf::rootfile_node)
			return true;
		if (mReader.context() == &ocf::rootfiles_node)
			return false;
		break;
	case xml::reader::attribute:
		if (ctx == &ocf::rootfile_node)
		{
			if (mReader.context() == &ocf::fullpath_attr)
				mPath = mReader.nodeValue().str();
			else if (mReader.context() == &ocf::mediatype_attr)
				mMediaType = mReader.nodeValue().str();
		}
		break;
	}

	return false;
}
