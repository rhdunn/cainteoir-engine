/* Document.
 *
 * Copyright (C) 2010-2015 Reece H. Dunn
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
#include <cainteoir/unicode.hpp>

#include <iostream>

namespace rdf  = cainteoir::rdf;
namespace rql  = cainteoir::rdf::query;
namespace utf8 = cainteoir::utf8;

static inline cainteoir::document::const_iterator
get_child(const cainteoir::document::list_type &children, size_t index)
{
	if (index == size_t(-1)) return children.end();

	cainteoir::document::const_iterator pos = children.begin();
	std::advance(pos, index);
	return pos;
}

cainteoir::document_item &
cainteoir::document_item::clear()
{
	type = 0;
	return *this;
}

cainteoir::document_item &
cainteoir::document_item::copy(const document_item &aItem)
{
	clear();

	switch (aItem.type & (cainteoir::events::begin_context | cainteoir::events::end_context))
	{
	case cainteoir::events::begin_context | cainteoir::events::end_context:
		begin_end_context_event(aItem.styles);
		break;
	case cainteoir::events::begin_context:
		begin_context_event(aItem.styles);
		break;
	case cainteoir::events::end_context:
		end_context_event(aItem.styles);
		break;
	}

	if (aItem.type & cainteoir::events::text)
		text_event(aItem.content);

	if (aItem.type & cainteoir::events::anchor)
		anchor_event(aItem.anchor);

	if (aItem.type & cainteoir::events::text_ref)
		text_ref_event(aItem.anchor);

	if (aItem.type & cainteoir::events::media_ref)
		media_ref_event(aItem.anchor, aItem.content, aItem.media_begin, aItem.media_end);

	return *this;
}

cainteoir::document_item &
cainteoir::document_item::begin_context_event(const cainteoir::css::styles *aStyles)
{
	type |= cainteoir::events::begin_context;
	styles = aStyles;
	return *this;
}

cainteoir::document_item &
cainteoir::document_item::end_context_event(const cainteoir::css::styles *aStyles)
{
	type |= cainteoir::events::end_context;
	styles = aStyles;
	return *this;
}

cainteoir::document_item &
cainteoir::document_item::begin_end_context_event(const cainteoir::css::styles *aStyles)
{
	type |= (cainteoir::events::begin_context | cainteoir::events::end_context);
	styles = aStyles;
	return *this;
}

cainteoir::document_item &
cainteoir::document_item::text_event(const std::shared_ptr<buffer> &aText)
{
	if (!aText) return *this;

	uint32_t length = utf8::codepoints(aText->begin(), aText->end());
	uint32_t start  = range.end();

	type |= cainteoir::events::text;
	content = aText;
	range = { start, start + length };
	return *this;
}

cainteoir::document_item &
cainteoir::document_item::anchor_event(const rdf::uri &aAnchor)
{
	type |= cainteoir::events::anchor;
	anchor = aAnchor;
	return *this;
}

cainteoir::document_item &
cainteoir::document_item::text_ref_event(const rdf::uri &aTextRef)
{
	type |= cainteoir::events::text_ref;
	anchor = aTextRef;
	return *this;
}

cainteoir::document_item &
cainteoir::document_item::media_ref_event(const rdf::uri &aMediaFilePath,
                                          const std::shared_ptr<buffer> &aAudioData,
                                          const cainteoir::css::time &aMediaBegin,
                                          const cainteoir::css::time &aMediaEnd)
{
	type |= cainteoir::events::media_ref;
	anchor = aMediaFilePath;
	content = aAudioData;
	media_begin = aMediaBegin;
	media_end = aMediaEnd;
	return *this;
}

cainteoir::ref_entry::ref_entry(const rql::results &aEntry)
	: depth(0)
{
	for (auto &item : aEntry)
	{
		if (rql::predicate(item) == rdf::ref("level"))
			depth = rql::literal(item).as<int>();
		else if (rql::predicate(item) == rdf::ref("target"))
			location = rql::object(item);
		else if (rql::predicate(item) == rdf::dc("title"))
			title = rql::value(item);
	}
}

cainteoir::document::document(const std::shared_ptr<document_reader> &aReader, rdf::graph &aMetadata)
	: mLength(0)
{
	while (aReader->read(&aMetadata))
	{
		mChildren.push_back(*aReader);
		if (aReader->type & cainteoir::events::anchor)
			mAnchors[aReader->anchor.str()] = mChildren.size();
		if (aReader->type & cainteoir::events::text)
			mLength = aReader->range.end();
	}
}

cainteoir::document::range_type
cainteoir::document::children(const std::pair<const rdf::uri, const rdf::uri> &aAnchors) const
{
	size_t from = indexof(aAnchors.first);
	size_t to   = indexof(aAnchors.second);

	if (from == size_t(-1)) from = 0;
	if (from > to) std::swap(from, to);

	return range_type(get_child(mChildren, from), get_child(mChildren, to));
}

cainteoir::document::range_type
cainteoir::document::children(const std::vector<ref_entry> &aListing,
                              const std::pair<size_t, size_t> &aRange) const
{
	rdf::uri from = (aRange.first <= 0 || aRange.first > aListing.size())
	              ? rdf::uri()
	              : aListing[aRange.first - 1].location;

	rdf::uri to = (aRange.second <= 0 || aRange.second > aListing.size())
	            ? rdf::uri()
	            : aListing[aRange.second - 1].location;

	return children(std::pair<const rdf::uri, const rdf::uri>(from, to));
}

std::vector<cainteoir::ref_entry>
cainteoir::navigation(const rdf::graph &aMetadata,
                      const rdf::uri &aSubject,
                      const rdf::uri &aListing)
{
	const rdf::uri *toc_entries = nullptr;

	auto listings = rql::select(aMetadata,
	                            rql::subject == aSubject && rql::predicate == rdf::ref("listing"));
	for (auto &query : listings)
	{
		auto listing = rql::select(aMetadata, rql::subject == rql::object(query));
		if (rql::contains(listing, rql::predicate == rdf::ref("type") && rql::object == aListing))
		{
			toc_entries = &rql::object(query);
			break;
		}
	}

	if (!toc_entries) return {};

	std::vector<cainteoir::ref_entry> nav;
	rql::rdf_list_items(aMetadata, *toc_entries, rdf::ref("entries"),
	                    [&aMetadata, &nav](const std::shared_ptr<const rdf::triple> &item)
	{
		nav.push_back({ rql::select(aMetadata, rql::subject == rql::object(item)) });
	});
	return nav;
}

size_t cainteoir::document::indexof(const rdf::uri &aAnchor) const
{
	auto at = mAnchors.find(aAnchor.str());
	return (at == mAnchors.end()) ? size_t(-1) : at->second;
}

struct document_range : public cainteoir::document_reader
{
	document_range(const cainteoir::document::range_type &aDocumentRange)
		: mCurrent(aDocumentRange.begin())
		, mLast(aDocumentRange.end())
	{
	}

	bool read(rdf::graph *aMetadata);

	cainteoir::document::const_iterator mCurrent;
	cainteoir::document::const_iterator mLast;
};

bool document_range::read(rdf::graph *aMetadata)
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
