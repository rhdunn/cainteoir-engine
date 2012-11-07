/* XML/HTML Reader API.
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

#ifndef CAINTEOIR_ENGINE_XMLREADER_HPP
#define CAINTEOIR_ENGINE_XMLREADER_HPP

#include <cainteoir/encoding.hpp>
#include <map>

namespace cainteoir { namespace xml
{
	struct ns
	{
		std::string prefix;
		std::string href;

		ns(const std::string &aPrefix, const std::string &aHref)
			: href(aHref)
			, prefix(aPrefix)
		{
		}
	};

	namespace xmlns
	{
		extern const ns dc;
		extern const ns dcam;
		extern const ns dct;
		extern const ns dtb;
		extern const ns epub;
		extern const ns foaf;
		extern const ns html40;
		extern const ns media;
		extern const ns ncx;
		extern const ns ocf;
		extern const ns opf;
		extern const ns owl;
		extern const ns pkg;
		extern const ns rdf;
		extern const ns rdfa;
		extern const ns rdfs;
		extern const ns skos;
		extern const ns smil;
		extern const ns ssml;
		extern const ns tts;
		extern const ns xhtml;
		extern const ns xml;
		extern const ns xsd;
	}

	struct namespaces
	{
		namespaces();

		namespaces &add_namespace(const std::string &aPrefix, const std::string &aHref);

		namespaces &add_namespace(const cainteoir::buffer &aPrefix, const std::shared_ptr<cainteoir::buffer> &aHref)
		{
			return add_namespace(aPrefix.str(), aHref->str());
		}

		namespaces &add_namespace(const ns &aNS)
		{
			return add_namespace(aNS.prefix, aNS.href);
		}

		void push_block();
		void pop_block();

		std::string lookup(const std::string &aPrefix) const;

		std::string lookup(const cainteoir::buffer &aPrefix) const
		{
			return lookup(aPrefix.str());
		}
	private:
#ifndef DOXYGEN
		struct namespace_item
		{
			long block;
			ns   item;

			namespace_item(long aBlock, const ns &aItem)
				: block(aBlock)
				, item(aItem)
			{
			}
		};
#endif

		std::list<namespace_item> mNamespaces;
		long mBlockNumber;
	};

#ifndef DOXYGEN
	namespace detail
	{
		struct entity
		{
			const char * name;
			const char * value;
		};

		struct entity_set
		{
			const entity * first;
			const entity * last;
		};
	}
#endif

	extern const detail::entity_set *xml_entities[52];
	extern const detail::entity_set *html_entities[52];

	const char *lookup_entity(const detail::entity_set **entities, const cainteoir::buffer &data);

	enum class begin_tag_type
	{
		open,
		open_close,
	};

	struct context
	{
		struct entry
		{
			uint32_t context;
			uint32_t parameter;
			xml::begin_tag_type begin_tag_type;

			entry(uint32_t aContext = 0,
			      uint32_t aParameter = 0,
			      xml::begin_tag_type aBeginTagType = xml::begin_tag_type::open)
				: context(aContext)
				, parameter(aParameter)
				, begin_tag_type(aBeginTagType)
			{
			}

			entry(xml::begin_tag_type aBeginTagType)
				: context(0)
				, parameter(0)
				, begin_tag_type(aBeginTagType)
			{
			}
		};

		struct entry_ref
		{
			const char  *name;
			const entry *data;
		};

		context()
		{
		}

		context(const std::string &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			set(aNS, entries, match);
		}

		context(const ns &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			set(aNS, entries, match);
		}

		void set(const std::string &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			mNodes[aNS] = std::make_pair(entries, match);
		}

		void set(const ns &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			mNodes[aNS.href] = std::make_pair(entries, match);
		}

		const entry *lookup(const std::string &aNS, const cainteoir::buffer &aNode) const;
	private:
		typedef std::map<std::string, std::pair<std::initializer_list<const entry_ref>, buffer::match_type>>
		        entries;

		entries mNodes;
	};

	extern const context::entry unknown_context;

	extern const context::entry base_attr;
	extern const context::entry id_attr;
	extern const context::entry lang_attr;
	extern const context::entry space_attr;

	extern const std::initializer_list<const context::entry_ref> attrs;

	struct reader
	{
		enum node_type
		{
			beginTagNode,
			endTagNode,
			beginProcessingInstructionNode,
			endProcessingInstructionNode,
			commentNode,
			cdataNode,
			textNode,
			error,
			doctypeNode,
			attribute,
			endOfData,

			dtdEntity,
		};

		reader(std::shared_ptr<cainteoir::buffer> aData, const char *aDefaultEncoding, const detail::entity_set *aPredefinedEntities[52] = xml_entities);

		void set_predefined_entities(const detail::entity_set *aPredefinedEntities[52]) { mPredefinedEntities = aPredefinedEntities; }

		bool read();

		const cainteoir::rope &nodeValue() const { return mNodeValue; }

		const cainteoir::buffer &nodeName() const { return mState.nodeName; }

		const cainteoir::buffer &nodePrefix() const { return mState.nodePrefix; }

		std::string namespaceUri() const;

		node_type nodeType() const { return mNodeType; }

		bool isPlainText() const { return mState.state == ParsingText; }

		const xml::context::entry *context() const { return mContext; }

		void set_nodes(const std::string &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			mNodes.set(aNS, entries, match);
			reset_context();
		}

		void set_nodes(const ns &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			mNodes.set(aNS, entries, match);
			reset_context();
		}

		void set_attrs(const std::string &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			mAttrs.set(aNS, entries, match);
			reset_context();
		}

		void set_attrs(const ns &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match=buffer::match_case)
		{
			mAttrs.set(aNS, entries, match);
			reset_context();
		}

		bool set_encoding(const char *encoding)
		{
			return mEncoding.set_encoding(encoding);
		}

		const char *current() const { return mState.current; }

		void set_begin_tag_type(begin_tag_type aType);
	private:
		/** @name parser internals/helpers */
		//@{

		void skip_whitespace();

		bool expect_next(char c);
		bool check_next(char c);

		cainteoir::buffer identifier();

		void read_node_value(char terminator1, char terminator2 = '\0');

		void read_tag(node_type aType);

		void reset_context();

		enum ParserState
		{
			ParsingText,
			ParsingXml,
			ParsingXmlTagAttributes,
			ParsingXmlContainedTagAttributes, // implicit end tag
			ParsingXmlProcessingInstructionAttributes,
			ParsingProcessingInstructionAttributes,
			ParsingXmlNamespaces,
			ParsingDtd,
		};

#ifndef DOXYGEN
		struct ParserContext
		{
			ParserState state;
			const char *current;
			cainteoir::buffer nodeName;
			cainteoir::buffer nodePrefix;

			ParserContext(ParserState aState, const char *aCurrent);
		};
#endif

		std::shared_ptr<cainteoir::buffer> mData;
		ParserContext mState;
		ParserContext mSavedState;
		const detail::entity_set **mPredefinedEntities;

		namespaces mNamespaces;
		cainteoir::buffer mTagNodeName;
		cainteoir::buffer mTagNodePrefix;

		std::map<std::string, std::string> mDoctypeEntities;
		encoding mEncoding;

		//@}
		/** @name reader data */
		//@{

		cainteoir::rope mNodeValue;
		node_type mNodeType;

		xml::context mNodes;
		xml::context mAttrs;
		const xml::context::entry *mContext;

		//@}
	};
}}

/** References
  *
  *    XML  [http://www.w3.org/TR/2008/REC-xml-20081126/] -- Extensible Markup Language (XML) 1.0 (Fifth Edition)
  *    HTML [http://www.whatwg.org/specs/web-apps/current-work/multipage/] -- HTML Living Standard
  */

#endif
