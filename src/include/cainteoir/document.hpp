/* Document Parser API.
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
			toc_entry = 0x0008,
			anchor = 0x0010,
		};

		enum context
		{
			unknown,
			heading,
			sentence,
		};
	}

	struct document_item
	{
		document_item()
			: type(0)
			, context(events::unknown)
			, parameter(0)
			, styles(nullptr)
		{
		}

		uint32_t type;
		events::context context;
		uint32_t parameter;
		const cainteoir::styles *styles;
		std::shared_ptr<buffer> text;
		rdf::uri anchor;
	};

	class document
	{
	public:
		typedef std::list<document_item> list_type;
		typedef list_type::const_iterator const_iterator;
		typedef std::pair<const_iterator, const_iterator> range_type;

		document() : mLength(0) {}

		void clear()
		{
			mLength = 0;
			mChildren.clear();
			mAnchors.clear();
		}

		size_t text_length() const { return mLength; }

		void add(const document_item &aItem)
		{
			mChildren.push_back(aItem);
			if (aItem.type & cainteoir::events::anchor)
				mAnchors[aItem.anchor.str()] = mChildren.size();
			if (aItem.type & cainteoir::events::text)
				mLength += aItem.text->size();
		}

		size_t anchor(const rdf::uri &aAnchor) const
		{
			auto at = mAnchors.find(aAnchor.str());
			return (at == mAnchors.end()) ? size_t(-1) : at->second;
		}

		const list_type & children() const
		{
			return mChildren;
		}

		range_type children(const rdf::uri &aFrom, const rdf::uri &aTo) const
		{
			size_t from = anchor(aFrom);
			size_t to   = anchor(aTo);

			if (from == size_t(-1)) from = 0;
			if (from > to) std::swap(from, to);

			return range_type(get_child(from), get_child(to));
		}

		range_type children(const std::pair<const rdf::uri, const rdf::uri> &aAnchors) const
		{
			return children(aAnchors.first, aAnchors.second);
		}
	private:
		const_iterator get_child(size_t index) const
		{
			if (index == size_t(-1)) return children().end();

			const_iterator pos = children().begin();
			std::advance(pos, index);
			return pos;
		}

		size_t mLength;
		list_type mChildren;
		std::map<std::string, size_t> mAnchors;
	};

	struct document_reader : public document_item
	{
		virtual bool read() = 0;

		virtual ~document_reader() {}
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
}

#endif
