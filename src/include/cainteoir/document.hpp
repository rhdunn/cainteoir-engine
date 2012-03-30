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
	namespace events
	{
		/** @brief The rendering context.
		  *
		  * This forms the basis for the abstract document rendering model used by the Cainteoir
		  * Text-to-Speech engine. All document formats (RTF, HTML, SSML, PDF, ...) get converted
		  * to this form.
		  *
		  * The rendering model includes both presentation (display) and text-to-speech (tts)
		  * elements.
		  */
		enum context
		{
			/** @brief An unspecified context.
			  */
			unknown,

			/** @brief Paragraph
			  *
			  * A paragraph is a block of text that is spoken and displayed as a single continuous
			  * unit. This applies to all blocks of text.
			  *
			  * @begincode
			  *   context paragraph +nostyle
			  *     text "This is a paragraph."
			  *   end
			  * @endcode
			  */
			paragraph,

			/** @brief Heading
			  *
			  * A heading is a line of text that denotes a book, part, chapter or section title.
			  *
			  * @begincode
			  *   context heading 1
			  *     text "Frankenstein"
			  *   end
			  * @endcode
			  */
			heading,

			/** @brief Inline Text
			  *
			  * A span is a section of text within a paragraph that is spoken and displayed as
			  * part of that block, as if the span was not present. This is used to control the
			  * presentation and speech of the span text.
			  *
			  * @begincode
			  *   context paragraph +nostyle
			  *     text "This is "
			  *     context span +strong
			  *       text "bold"
			  *     end
			  *     text "text."
			  *   end
			  * @endcode
			  */
			span,

			/** @brief List
			  *
			  * A list is a sequence of list items.
			  *
			  * @begincode
			  *   context list +number
			  *     context list-item
			  *       text "1. "
			  *       text "Lorem"
			  *     end
			  *     context list-item
			  *       text "2. "
			  *       text "ipsum"
			  *     end
			  *   end
			  * @endcode
			  */
			list,

			/** @brief List Item
			  *
			  * A list item consists of a text node that denotes the item gutter label and
			  * the item content. The list item content has an implicit pargraph.
			  *
			  * @begincode
			  *   context list-item
			  *     text "1. "
			  *     text "List item text."
			  *   end
			  * @endcode
			  *
			  * If the item contains explicit paragraphs, the first paragraph is displayed
			  * at the same level as the list item label.
			  *
			  * @begincode
			  *   context list-item
			  *     text "iv. "
			  *     context paragraph
			  *       text "List item text."
			  *     end
			  *   end
			  * @endcode
			  */
			list_item,

			/** @brief Sentence
			  *
			  * A sentence is an explicitly marked up sentence within a paragraph block.
			  * If provided, the text-to-speech processor will not attempt to identify
			  * sentences within that text, but will treat it as part of a single sentence.
			  *
			  * @begincode
			  *   context paragraph
			  *     text "'"
			  *     context sentence
			  *       text "How are you?"
			  *     end
			  *     text "'"
			  *     context sentence
			  *       text "said Mr. Davis."
			  *     end
			  *   end
			  * @endcode
			  */
			sentence,
		};

		/** @brief The style of the paragraph or span context.
		  */
		enum style
		{
			/** @brief No explicit styling
			  *
			  * The context does not provide any specific custom styling.
			  */
			nostyle = 0x00000000,

			/** @brief Superscript
			  *
			  * The context is raised above the text in a smaller font and is
			  * placed to the side of the previous text.
			  *
			  * @begincode
			  *   context paragraph
			  *     text "The 1"
			  *     context span +superscript
			  *       text "st"
			  *     end
			  *     text " of May."
			  *   end
			  * @endcode
			  */
			superscript = 0x00000001,

			/** @brief Subscript
			  *
			  * The context is lowered below the text in a smaller font and is
			  * placed to the side of the previous text.
			  *
			  * @begincode
			  *   context paragraph
			  *     text "a"
			  *     context span +subscript
			  *       text "1"
			  *     end
			  *     text " is the first item."
			  *   end
			  * @endcode
			  */
			subscript = 0x00000002,

			/** @brief Over
			  *
			  * The context is rendered as the superscript text, but is placed
			  * above (over) the previous text. This is used to render ruby
			  * annotations and mathematical markup.
			  *
			  * @begincode
			  *   context paragraph
			  *     context span +overunder
			  *       text "a"
			  *       context span +over
			  *         text "1"
			  *       end
			  *     end
			  *   end
			  * @endcode
			  */
			over = superscript,

			/** @brief Under
			  *
			  * The context is rendered as the subscript text, but is placed
			  * below (under) the previous text. This is used to render ruby
			  * annotations and mathematical markup.
			  *
			  * @begincode
			  *   context paragraph
			  *     context span +overunder
			  *       text "a"
			  *       context span +under
			  *         text "1"
			  *       end
			  *     end
			  *   end
			  * @endcode
			  */
			under = subscript,

			/** @brief Over/Under
			  *
			  * The context denotes a section of text which contains text above
			  * and/or below it. This is used to render ruby annotations and
			  * mathematical markup.
			  *
			  * This context contains a text or context node that is not annotated
			  * with the under or over style. This is the primary text element
			  * that forms the root of the over/under markup.
			  *
			  * In addition to this, the context contains either:
			  *     -  a span with the over style
			  *     -  a span with the under style
			  *     -  a span with the over style and a span with the under style
			  *
			  * @begincode
			  *   context paragraph
			  *     context span +overunder
			  *       text "a"
			  *       context span +over
			  *         text "n"
			  *       end
			  *       context span +under
			  *         text "i = 0"
			  *       end
			  *     end
			  *   end
			  * @endcode
			  */
			overunder = over | under,

			/** @brief Emphasized
			  *
			  * The context is displayed in an italic font and is spoken with
			  * more emphasis.
			  *
			  * @begincode
			  *   context paragraph
			  *     text "This "
			  *     context span +emphasized
			  *       text "and"
			  *     end
			  *     text " that."
			  *   end
			  * @endcode
			  */
			emphasized = 0x00000004,

			/** @brief Strong
			  *
			  * The context is displayed in a bold font and is spoken louder.
			  *
			  * @begincode
			  *   context paragraph
			  *     text "One, "
			  *     context span +strong
			  *       text "two"
			  *     end
			  *     text ", three."
			  *   end
			  * @endcode
			  */
			strong = 0x00000008,

			/** @brief Underline
			  *
			  * The context is displayed with a solid line below the text.
			  *
			  * @begincode
			  *   context paragraph
			  *     text "This "
			  *     context span +underline
			  *       text "and"
			  *     end
			  *     text " that."
			  *   end
			  * @endcode
			  */
			underline = 0x00000010,

			/** @brief Monospace
			  *
			  * The context is displayed in a monospace font.
			  *
			  * @begincode
			  *   context paragraph +monospace
			  *     text "if (a < b) { printf("Hello\n"); }"
			  *   end
			  * @endcode
			  */
			monospace = 0x00000020,

			/** @brief Reduced
			  *
			  * The context is displayed with a normal (non-italic, non-bold) font and
			  * is spoken quieter.
			  *
			  * @begincode
			  *   context paragraph
			  *     text "This and "
			  *     context span +reduced
			  *       text "that."
			  *     end
			  *   end
			  * @endcode
			  */
			reduced = 0x00000040,
		};

		/** @brief The type of the list context.
		  *
		  * This is used to inform the document consumer what style list is presented. This
		  * allows the consumer to process the lists differently.
		  */
		enum list_type
		{
			/** @brief Bullet List
			  *
			  * A bullet list is an unordered list of items. It is typically rendered in
			  * the same way a number (ordered) list is with the bullet appearing in the
			  * list gutter, but the bullet glyphs are not spoken.
			  *
			  * @begincode
			  *   context list +bullet
			  *     context list-item
			  *       text "* "
			  *       text "Lorem"
			  *     end
			  *     context list-item
			  *       text "* "
			  *       text "ipsum"
			  *     end
			  *   end
			  * @endcode
			  */
			bullet = 0x10000000,

			/** @brief Number List
			  *
			  * A number list is an ordered list of items. It is typically rendered in
			  * the same way as a bullet (unordered) list with the number appearing in
			  * the list gutter, and the number is spoken with a short pause afterward.
			  *
			  * @begincode
			  *   context list +number
			  *     context list-item
			  *       text "1. "
			  *       text "Lorem"
			  *     end
			  *     context list-item
			  *       text "2. "
			  *       text "ipsum"
			  *     end
			  *   end
			  * @endcode
			  */
			number = 0x20000000,

			/** @brief Definition List
			  *
			  * A definition list is a list of glossary or terminology definitions. It
			  * is typically rendered with the definition body indented from the label
			  * and both the label and body are spoken, with a short pause after the
			  * label.
			  *
			  * @begincode
			  *   context list +definition
			  *     context list-item
			  *       text "HTML "
			  *       text "HyperText Markup Language"
			  *     end
			  *     context list-item
			  *       text "RDF "
			  *       text "Resource Description Framework"
			  *     end
			  *   end
			  * @endcode
			  */
			definition = 0x30000000,
		};
	}

	struct document_events
	{
		/** @brief A block of text in the document.
		  *
		  * @param aText The text at the current point in the document.
		  */
		virtual void text(std::shared_ptr<cainteoir::buffer> aText) {}

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
		virtual void begin_context(events::context aContext, uint32_t aParameter=0) {}

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
		  * @param mimetype The mimetype of the document if this is a root document,
		  *                 an empty string otherwise.
		  *
		  * The anchor binds to the next document event.
		  */
		virtual void anchor(const rdf::uri &location, const std::string &mimetype) {}

		virtual ~document_events() {}
	};

	class document
	{
	public:
		typedef std::list< std::shared_ptr<cainteoir::buffer> > list_type;
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

		void add(const std::shared_ptr<cainteoir::buffer> &text)
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

	enum capability_types
	{
		metadata_support = 1,
		text_support     = 2,
	};

	/** @brief Get the document formats that are supported by libcainteoir.
	  *
	  * @metadata The RDF graph to write the format support to.
	  */
	void supportedDocumentFormats(rdf::graph &metadata, capability_types capabilities);

	/** @brief Read the contents of a document.
	  *
	  * @param aFilename The path to the ePub document.
	  * @param events    The events callback to handle document events.
	  *
	  * @retval true  If aFilename contains a supported document format.
	  * @retval false If aFilename contains an unsupported document format.
	  */
	bool parseDocument(const char *aFilename, document_events &events, rdf::graph &aGraph);
}

#endif
