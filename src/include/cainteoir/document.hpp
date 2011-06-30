/* Document Parser API.
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

#ifndef CAINTEOIR_ENGINE_DOCUMENT_HPP
#define CAINTEOIR_ENGINE_DOCUMENT_HPP

#include <cainteoir/metadata.hpp>
#include <cainteoir/buffer.hpp>

namespace cainteoir
{
	struct document_events
	{
		virtual void metadata(const rdf::statement &aStatement) = 0;

		virtual const rdf::bnode genid() = 0;

		virtual void text(std::tr1::shared_ptr<cainteoir::buffer> aText) = 0;

		virtual ~document_events() {}
	};

	class document
	{
	public:
		typedef std::list< std::tr1::shared_ptr<cainteoir::buffer> > list_type;
		typedef list_type::const_iterator const_iterator;

		document() : mLength(0) {}

		void clear()
		{
			mLength = 0;
			mChildren.clear();
		}

		size_t text_length() const { return mLength; }

		void add(const std::tr1::shared_ptr<cainteoir::buffer> &text)
		{
			mLength += text->size();
			mChildren.push_back(text);
		}

		const list_type & children() const { return mChildren; }
	private:
		size_t mLength;
		list_type mChildren;
	};

	/** @brief Get the document formats that are supported by libcainteoir.
	  *
	  * @metadata The RDF graph to write the format support to.
	  */
	void supportedDocumentFormats(rdf::graph &metadata);

	/** @brief Read the contents of a document.
	  *
	  * @param aFilename The path to the ePub document.
	  * @param events    The events callback to handle document events.
	  *
	  * @retval true  If aFilename contains a supported document format.
	  * @retval false If aFilename contains an unsupported document format.
	  */
	bool parseDocument(const char *aFilename, document_events &events);
}

#endif
