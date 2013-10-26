/* Document Parser API.
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
		std::shared_ptr<buffer> text;
		rdf::uri anchor;
	};

	struct document_reader : public document_item
	{
		virtual bool read() = 0;

		virtual ~document_reader() {}
	};

	class document
	{
	public:
		struct toc_entry
		{
			int depth;
			rdf::uri location;
			std::string title;
		};

		typedef std::list<document_item> list_type;
		typedef list_type::const_iterator const_iterator;
		typedef range<const_iterator> range_type;
		typedef std::vector<toc_entry> toc_type;

		document(const std::shared_ptr<document_reader> &aReader);

		size_t text_length() const { return mLength; }

		range_type children(const std::pair<const rdf::uri, const rdf::uri> &aAnchors) const;

		range_type children(const std::pair<size_t, size_t> &aTocRange) const;

		range_type children() const { return range_type(mChildren.begin(), mChildren.end()); }

		const toc_type &toc() const { return mToc; }
	private:
		size_t mLength;
		list_type mChildren;
		std::vector<toc_entry> mToc;
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
