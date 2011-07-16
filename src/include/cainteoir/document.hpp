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
#include <map>

namespace cainteoir
{
	struct document_events
	{
		enum context
		{
			paragraph, /** @brief A paragraph of text (display, parsing). */
			heading,   /** @brief The text forms a section heading (display). */
			span,      /** @brief A span of text within a paragraph (display). */
			list,      /** @brief A sequence of numbered or unnumbered items (display). */
			list_item, /** @brief An item in a list (display). */
			sentence,  /** @brief A sentence of text (display, parsing). */
		};

		enum style
		{
			nostyle     = 0x00000000, /** @brief The text does not have any special styling. */
			superscript = 0x00000001, /** @brief The text is aligned below the line (display). */
			subscript   = 0x00000002, /** @brief The text is aligned above the line (display). */
			emphasized  = 0x00000004, /** @brief The text is emphasized in the document (display=italic, prosody). */
			strong      = 0x00000008, /** @brief The text should stand out (display=bold, prosody). */
			underline   = 0x00000010, /** @brief The text is underlined (display). */
			monospace   = 0x00000020, /** @brief The text is formatted using a monospace font (display). */
		};

		/** @brief A metadata item was encountered in the document.
		  *
		  * @param aStatement The RDF statement for the metadata item.
		  */
		virtual void metadata(const std::tr1::shared_ptr<const rdf::triple> &aStatement) = 0;

		/** @brief Generate a new RDF BNode id.
		  *
		  * @return The new BNode id.
		  */
		virtual const rdf::bnode genid() = 0;

		/** @brief A block of text in the document.
		  *
		  * @param aText The text at the current point in the document.
		  */
		virtual void text(std::tr1::shared_ptr<cainteoir::buffer> aText) {}

		/** @brief The start of a new context.
		  *
		  * @param aContext   The type of the context encountered.
		  * @param aParameter A context-dependent parameter.
		  *
		  * A context defines a range of text in the document that is to be
		  * interpreted in the way defined by the context. Contexts can nest
		  * to form complex documents.
		  *
		  * If |aContext| is |heading| then |aParameter| is the heading depth,
		  * otherwise it is a set of |style| flags that apply to this context.
		  */
		virtual void begin_context(context aContext, uint32_t aParameter=0) {}

		/** @brief The end of the current context.
		  */
		virtual void end_context() {}

		/** @brief A table of contents entry.
		  *
		  * @param depth    The indentation level of the entry.
		  * @param location The anchor to which this entry points to.
		  * @param title    The text to display for this entry.
		  */
		virtual void toc_entry(int depth, const rdf::uri &location, const std::string &title) {}

		/** @brief An anchor point in the document.
		  *
		  * @param location The uri for this point in the document.
		  *
		  * The anchor binds to the next document event.
		  */
		virtual void anchor(const rdf::uri &location) {}

		virtual ~document_events() {}
	};

	class document
	{
	public:
		typedef std::list< std::tr1::shared_ptr<cainteoir::buffer> > list_type;
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

		void add(const std::tr1::shared_ptr<cainteoir::buffer> &text)
		{
			mLength += text->size();
			mChildren.push_back(text);
		}

		void add_anchor(const rdf::uri &aAnchor)
		{
			mAnchors[aAnchor.str()] = mChildren.size();
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
