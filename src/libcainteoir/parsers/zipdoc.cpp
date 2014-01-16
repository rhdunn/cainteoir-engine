/* Zip Document Parser.
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
#include "i18n.h"
#include "parsers.hpp"

#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace xml = cainteoir::xml;

struct zip_document_reader : public cainteoir::document_reader
{
	zip_document_reader(std::shared_ptr<cainteoir::archive> &aData);

	bool read(rdf::graph *aMetadata);

	std::shared_ptr<cainteoir::document_reader> child;
	std::shared_ptr<cainteoir::archive> mData;
	std::list<std::string> mFiles;
	std::list<std::string>::iterator mCurrent;
};

zip_document_reader::zip_document_reader(std::shared_ptr<cainteoir::archive> &aData)
	: mData(aData)
{
	mFiles = mData->files();
	mCurrent = mFiles.begin();
}

bool zip_document_reader::read(rdf::graph *aMetadata)
{
	if (child)
	{
		if (child->read())
		{
			type    = child->type;
			styles  = child->styles;
			content = child->content;
			anchor  = child->anchor;
			return true;
		}
		child.reset();
	}

	while (mCurrent != mFiles.end())
	{
		auto doc = mData->read((*mCurrent).c_str());
		if (doc)
		{
			auto location = mData->location(*mCurrent, std::string());
			rdf::graph innerMetadata;
			child = createDocumentReader(doc, location, innerMetadata, std::string());
			if (child)
			{
				++mCurrent;
				return read(aMetadata);
			}
		}
		++mCurrent;
	}

	return false;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createZipReader(std::shared_ptr<archive> &aData)
{
	return std::make_shared<zip_document_reader>(aData);
}
