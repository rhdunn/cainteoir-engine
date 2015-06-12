/* Document Parser API.
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

#ifndef CAINTEOIR_ENGINE_DOCUMENT_HPP
#define CAINTEOIR_ENGINE_DOCUMENT_HPP

#include "metadata.hpp"

namespace cainteoir
{
	namespace events
	{
		enum event_type
		{
			begin_context = 0x0001,
			end_context = 0x0002,
			text = 0x0004,
			anchor = 0x0008,
			text_ref = 0x0010,
			media_ref = 0x0020,
		};
	}

	struct document_item
	{
		document_item()
			: type(0)
			, styles(nullptr)
		{
		}

		uint32_t type;
		const cainteoir::css::styles *styles;
		std::shared_ptr<buffer> content;
		rdf::uri anchor;
		cainteoir::css::time media_begin;
		cainteoir::css::time media_end;

		document_item &clear();

		document_item &copy(const document_item &aItem);

		document_item &begin_context_event(const cainteoir::css::styles *aStyles);

		document_item &end_context_event(const cainteoir::css::styles *aStyles);

		document_item &begin_end_context_event(const cainteoir::css::styles *aStyles);

		document_item &text_event(const std::shared_ptr<buffer> &aText);

		document_item &anchor_event(const rdf::uri &aAnchor);

		document_item &text_ref_event(const rdf::uri &aTextRef);

		document_item &media_ref_event(const rdf::uri &aMediaFilePath,
		                               const std::shared_ptr<buffer> &aAudioData,
		                               const cainteoir::css::time &aMediaBegin,
		                               const cainteoir::css::time &aMediaEnd);
	};

	struct document_reader : public document_item
	{
		virtual bool read(rdf::graph *aMetadata=nullptr) = 0;

		virtual ~document_reader() {}
	};

	struct ref_entry
	{
		int depth;
		rdf::uri location;
		std::string title;

		ref_entry(const rdf::query::results &aEntry);
	};

	std::vector<ref_entry>
	navigation(const rdf::graph &aMetadata,
	           const rdf::uri &aSubject,
	           const rdf::uri &aListing);

	class document
	{
	public:
		typedef std::list<document_item> list_type;
		typedef list_type::const_iterator const_iterator;
		typedef range<const_iterator> range_type;

		document(const std::shared_ptr<document_reader> &aReader, rdf::graph &aMetadata);

		size_t text_length() const { return mLength; }

		range_type children(const std::pair<const rdf::uri, const rdf::uri> &aAnchors) const;

		range_type children(const std::vector<ref_entry> &aListing,
		                    const std::pair<size_t, size_t> &aRange) const;

		range_type children() const { return range_type(mChildren.begin(), mChildren.end()); }

		size_t indexof(const rdf::uri &aAnchor) const;
	private:
		size_t mLength;
		list_type mChildren;
		std::map<std::string, size_t> mAnchors;
	};

	enum capability_types
	{
		metadata_support = 1,
		text_support = 2,
	};

	void supportedDocumentFormats(rdf::graph &metadata, capability_types capabilities);

	std::shared_ptr<document_reader>
	createDocumentReader(const char *aFilename,
	                     rdf::graph &aPrimaryMetadata,
	                     const std::string &aTitle = std::string(),
	                     const char *aDefaultEncoding = "windows-1252");

	std::shared_ptr<document_reader>
	createDocumentReader(std::shared_ptr<buffer> &aData,
	                     const rdf::uri &aSubject,
	                     rdf::graph &aPrimaryMetadata,
	                     const std::string &aTitle = std::string(),
	                     const char *aDefaultEncoding = "windows-1252");

	std::shared_ptr<document_reader>
	createDocumentReader(const document::range_type &aDocumentRange);
}

#endif
