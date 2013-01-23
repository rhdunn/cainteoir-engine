/* OCF Document Parser.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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
#include "i18n.h"

#include "parsers.hpp"
#include <stdexcept>

namespace rdf    = cainteoir::rdf;
namespace xmlns  = cainteoir::xml::xmlns;
namespace xml    = cainteoir::xml;

namespace ocf
{
	static const xml::context::entry container_node = {};
	static const xml::context::entry rootfile_node  = {};
	static const xml::context::entry rootfiles_node = {};

	static const xml::context::entry fullpath_attr  = {};
	static const xml::context::entry mediatype_attr = {};
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

struct ocf_reader : public cainteoir::document_reader
{
	ocf_reader(const std::shared_ptr<xml::reader> &aReader);

	bool read();

	std::shared_ptr<xml::reader> mReader;
};

ocf_reader::ocf_reader(const std::shared_ptr<xml::reader> &aReader)
	: mReader(aReader)
{
	mReader->set_nodes(xmlns::ocf, ocf_nodes);
	mReader->set_attrs(xmlns::ocf, ocf_attrs);

	if (mReader->context() != &ocf::container_node)
		throw std::runtime_error(i18n("OCF file is not of a recognised format."));

	do {
		if (mReader->nodeType() == xml::reader::beginTagNode && mReader->context() == &ocf::rootfiles_node)
			break;
	} while (mReader->read());

	styles  = &cainteoir::heading1;
}

bool ocf_reader::read()
{
	text.reset();
	anchor = rdf::uri();

	const xml::context::entry *ctx = &xml::unknown_context;

	while (mReader->read()) switch (mReader->nodeType())
	{
	case xml::reader::beginTagNode:
		ctx = mReader->context();
		break;
	case xml::reader::endTagNode:
		if (mReader->context() == &ocf::rootfile_node)
			return true;
		if (mReader->context() == &ocf::rootfiles_node)
			return false;
		break;
	case xml::reader::attribute:
		if (ctx == &ocf::rootfile_node)
		{
			if (mReader->context() == &ocf::fullpath_attr)
				anchor = rdf::uri(mReader->nodeValue().str(), std::string());
			else if (mReader->context() == &ocf::mediatype_attr)
				text = mReader->nodeValue().buffer();
		}
		break;
	default:
		break;
	}

	return false;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createOcfReader(const std::shared_ptr<xml::reader> &aReader)
{
	if (!aReader)
		return std::shared_ptr<document_reader>();

	return std::make_shared<ocf_reader>(aReader);
}
