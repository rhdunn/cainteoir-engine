/* Document.
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
#include "compatibility.hpp"

#include <cainteoir/document.hpp>

namespace rdf = cainteoir::rdf;

static inline size_t
anchor(const std::map<std::string, size_t> &anchors, const rdf::uri &anchor)
{
	auto at = anchors.find(anchor.str());
	return (at == anchors.end()) ? size_t(-1) : at->second;
}

static inline cainteoir::document::const_iterator
get_child(const cainteoir::document::list_type &children, size_t index)
{
	if (index == size_t(-1)) return children.end();

	cainteoir::document::const_iterator pos = children.begin();
	std::advance(pos, index);
	return pos;
}

cainteoir::document::document(const std::shared_ptr<document_reader> &aReader)
	: mLength(0)
{
	while (aReader->read())
	{
		mChildren.push_back(*aReader);
		if (aReader->type & cainteoir::events::anchor)
			mAnchors[aReader->anchor.str()] = mChildren.size();
		if (aReader->type & cainteoir::events::toc_entry)
			mToc.push_back({ aReader->styles->aria_level, aReader->anchor, aReader->content->str() });
		if (aReader->type & cainteoir::events::text)
			mLength += aReader->content->size();
	}
}

cainteoir::document::range_type
cainteoir::document::children(const std::pair<const rdf::uri, const rdf::uri> &aAnchors) const
{
	size_t from = anchor(mAnchors, aAnchors.first);
	size_t to   = anchor(mAnchors, aAnchors.second);

	if (from == size_t(-1)) from = 0;
	if (from > to) std::swap(from, to);

	return range_type(get_child(mChildren, from), get_child(mChildren, to));
}

cainteoir::document::range_type
cainteoir::document::children(const std::pair<size_t, size_t> &aTocRange) const
{
	rdf::uri from = (aTocRange.first <= 0 || aTocRange.first > mToc.size())
	              ? rdf::uri()
	              : mToc[aTocRange.first - 1].location;

	rdf::uri to = (aTocRange.second <= 0 || aTocRange.second > mToc.size())
	            ? rdf::uri()
	            : mToc[aTocRange.second - 1].location;

	return children(std::pair<const rdf::uri, const rdf::uri>(from, to));
}

struct document_range : public cainteoir::document_reader
{
	document_range(const cainteoir::document::range_type &aDocumentRange)
		: mCurrent(aDocumentRange.begin())
		, mLast(aDocumentRange.end())
	{
	}

	bool read();

	cainteoir::document::const_iterator mCurrent;
	cainteoir::document::const_iterator mLast;
};

bool document_range::read()
{
	if (mCurrent == mLast) return false;

	*((document_item *)this) = *mCurrent;
	++mCurrent;

	return true;
}

std::shared_ptr<cainteoir::document_reader>
cainteoir::createDocumentReader(const document::range_type &aDocumentRange)
{
	return std::make_shared<document_range>(aDocumentRange);
}
