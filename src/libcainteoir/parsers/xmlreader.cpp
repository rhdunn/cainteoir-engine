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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include <cainteoir/xmlreader.hpp>
#include <cainteoir/unicode.hpp>

using cainteoir::xml::detail::entity;
using cainteoir::xml::detail::entity_set;

#include "xml-entities.h"
#include "html-entities.h"

/** @defgroup xml XML
  * @brief    XML document processing.
  */

/// @addtogroup xml
/// @{

/** @brief Find the predefined entity in the entity table.
  *
  * @param[in] entities The entity table to use to resolve the entity.
  * @param[in] data     The name of the entity without the surruonding & and ; characters.
  *
  * @return The UTF-8 encoded characters represented by the named entity, or null if it does not exist.
  */
const char * cainteoir::xml::lookup_entity(const detail::entity_set **entities, const cainteoir::buffer &data)
{
	char c = *data.begin();

	const entity_set * ent = nullptr;
	if (c >= 'a' && c <= 'z')
		ent = entities[c - 'a' + 26];
	else if (c >= 'A' && c <= 'Z')
		ent = entities[c - 'A'];

	if (!ent)
		return nullptr;

	int begin = 0;
	int end = ent->last - ent->first;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;

		int comp = data.compare((ent->first + pos)->name);
		if (comp == 0)
			return (ent->first + pos)->value;
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return nullptr;
}

/// @}

static std::shared_ptr<cainteoir::buffer>
parse_entity(const cainteoir::buffer &entity,
             const cainteoir::xml::detail::entity_set **entities,
             const std::map<std::string, std::string> &doctypeEntities)
{
	const char * str = entity.begin();
	if (*str == '#')
	{
		char utf8[10];
		char *end;
		if (*++str == 'x')
			end = cainteoir::utf8::write(utf8, strtol(++str, nullptr, 16));
		else
			end = cainteoir::utf8::write(utf8, strtol(str, nullptr, 10));
		*end = '\0';

		if (utf8[0])
			return cainteoir::make_buffer(utf8, strlen(utf8));
	}
	else
	{
		auto match = doctypeEntities.find(entity.str());
		if (match != doctypeEntities.end())
			return cainteoir::make_buffer(match->second.c_str(), match->second.size());

		const char * value = cainteoir::xml::lookup_entity(entities, entity);
		if (value)
			return std::make_shared<cainteoir::buffer>(value);
	}

	fprintf(stderr, i18n("unrecognised entity '%s'\n"), entity.str().c_str());
	return std::shared_ptr<cainteoir::buffer>();
}

static inline bool xmlalnum(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

static inline bool xmlspace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/// @addtogroup xml
/// @{

/** @brief Dublin Core Elements namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::dc("dc", "http://purl.org/dc/elements/1.1/");

/** @brief Dublin Core Abstract Model namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::dcam("dcam", "http://purl.org/dc/dcam/");

/** @brief Dublin Core Terms namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::dct("dct", "http://purl.org/dc/terms/");

/** @brief Daisy Talking Book namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::dtb("dtb", "http://www.daisy.org/z3986/2005/dtbook/");

/** @brief ePub (Open Package Specification) namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::epub("epub", "http://www.idpf.org/2007/ops");

/** @brief Friend of a Friend namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::foaf("foaf", "http://xmlns.com/foaf/0.1/");

/** @brief XML-based Hyper-Text Markup Language 4.0 (MS Word HTML) namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::html40("html", "http://www.w3.org/TR/REC-html40");

/** @brief ePub Media Overlays namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::media("media", "http://www.idpf.org/epub/vocab/overlays/");

/** @brief Navigation Control File namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::ncx("ncx", "http://www.daisy.org/z3986/2005/ncx/");

/** @brief Open Container Format namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::ocf("ocf", "urn:oasis:names:tc:opendocument:xmlns:container");

/** @brief Open Packaging Format namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::opf("opf", "http://www.idpf.org/2007/opf");

/** @brief Ontology Web Language namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::owl("owl", "http://www.w3.org/2002/07/owl");

/** @brief ePub Package namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::pkg("pkg", "http://www.idpf.org/epub/vocab/package/");

/** @brief Resource Description Framework namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::rdf("rdf", "http://www.w3.org/1999/02/22-rdf-syntax-ns");

/** @brief RDF/attributes namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::rdfa("rdfa", "http://www.w3.org/ns/rdfa");

/** @brief RDF Schema namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::rdfs("rdfs", "http://www.w3.org/2000/01/rdf-schema");

/** @brief Simple Knowledge Organization System namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::skos("skos", "http://www.w3.org/2004/02/skos/core");

/** @brief Synchronized Multimedia Integration Language namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::smil("smil", "http://www.w3.org/ns/SMIL");

/** @brief Speech Synthesis Markup Language namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::ssml("ssml", "http://www.w3.org/2001/10/synthesis");

/** @brief Cainteoir Text-to-Speech namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::tts("tts", "http://rhdunn.github.com/2010/12/text-to-speech");

/** @brief XML-based Hyper-Text Markup Language namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::xhtml("h", "http://www.w3.org/1999/xhtml");

/** @brief eXtensible Markup Language namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::xml("xml", "http://www.w3.org/XML/1998/namespace");

/** @brief XML Schema namespace.
  */
const cainteoir::xml::ns cainteoir::xml::xmlns::xsd("xsd", "http://www.w3.org/2001/XMLSchema");

/** @struct cainteoir::xml::namespaces
  * @brief  Manage a set of XML namespaces.
  */

/// @}

/** @brief Initialize a new namespace manager object.
  */
cainteoir::xml::namespaces::namespaces()
	: mBlockNumber(-1)
{
	add_namespace("xml", "http://www.w3.org/XML/1998/namespace");
}

/** @brief Add a namespace to the current scope.
  *
  * @param[in] aPrefix The prefix of the namespace.
  * @param[in] aHref   The URI of the namespace.
  *
  * @return This namespace manager object (to support method chaining).
  */
cainteoir::xml::namespaces &cainteoir::xml::namespaces::add_namespace(const std::string &aPrefix, const std::string &aHref)
{
	mNamespaces.push_back(namespace_item(mBlockNumber, ns(aPrefix, aHref)));
	return *this;
}

/** @fn    cainteoir::xml::namespaces &cainteoir::xml::namespaces::add_namespace(const cainteoir::buffer &aPrefix, const std::shared_ptr<cainteoir::buffer> &aHref)
  * @brief Add a namespace to the current scope.
  *
  * @param[in] aPrefix The prefix of the namespace.
  * @param[in] aHref   The URI of the namespace.
  *
  * @return This namespace manager object (to support method chaining).
  */

/** @fn    cainteoir::xml::namespaces &cainteoir::xml::namespaces::add_namespace(const ns &aNS)
  * @brief Add a namespace to the current scope.
  *
  * @param[in] aNS The namespace to add.
  *
  * @return This namespace manager object (to support method chaining).
  */

/** @brief Add a new scope block.
  */
void cainteoir::xml::namespaces::push_block()
{
	++mBlockNumber;
}

/** @brief Remove a new scope block.
  *
  * All namespaces associated with the top-most scope block are removed.
  */
void cainteoir::xml::namespaces::pop_block()
{
	--mBlockNumber;
	while (mNamespaces.back().block > mBlockNumber && !mNamespaces.empty())
	{
		mNamespaces.pop_back();
	}
}

/** @brief Resolve the namespace prefix to a URI.
  *
  * @param[in] aPrefix The namespace prefix to resolve.
  *
  * @return The URI associated with the specified namespace prefix.
  */
std::string cainteoir::xml::namespaces::lookup(const std::string &aPrefix) const
{
	for (auto &ns : cainteoir::reverse(mNamespaces))
	{
		if (ns.item.prefix == aPrefix && ns.block <= mBlockNumber)
			return ns.item.href;
	}
	return std::string();
}

/** @fn    std::string cainteoir::xml::namespaces::lookup(const cainteoir::buffer &aPrefix) const
  * @brief Resolve the namespace prefix to a URI.
  *
  * @param[in] aPrefix The namespace prefix to resolve.
  *
  * @return The URI associated with the specified namespace prefix.
  */

/// @addtogroup xml
/// @{

/** @brief The specified element/attribute was not found.
  */
const cainteoir::xml::context::entry cainteoir::xml::unknown_context = {};

/** @brief The \@xml:base attribute.
  */
const cainteoir::xml::context::entry cainteoir::xml::base_attr = {};

/** @brief The \@xml:id attribute.
  */
const cainteoir::xml::context::entry cainteoir::xml::id_attr = {};

/** @brief The \@xml:lang attribute.
  */
const cainteoir::xml::context::entry cainteoir::xml::lang_attr = {};

/** @brief The \@xml:space attribute.
  */
const cainteoir::xml::context::entry cainteoir::xml::space_attr = {};

/** @var   const std::initializer_list<const cainteoir::xml::context::entry_ref> cainteoir::xml::attrs
  * @brief Attributes in the XML namespace.
  */

/// @}

#ifndef DOXYGEN
const std::initializer_list<const cainteoir::xml::context::entry_ref> cainteoir::xml::attrs =
{
	{ "base",  &xml::base_attr },
	{ "id",    &xml::id_attr },
	{ "lang",  &xml::lang_attr },
	{ "space", &xml::space_attr },
};
#endif

/// @addtogroup xml
/// @{

/** @struct cainteoir::xml::context
  * @brief  Manage looking up XML elements and attributes.
  */

/// @}

/** @fn    cainteoir::xml::context::context()
  * @brief Create an empty context manager object.
  */

/** @fn    cainteoir::xml::context::context(const std::string &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match)
  * @brief Create a context manager object.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The elements or attributes associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @fn    cainteoir::xml::context::context(const ns &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match)
  * @brief Create a context manager object.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The elements or attributes associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @fn    void cainteoir::xml::context::set(const std::string &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match)
  * @brief Set the context entries to use for the specified namespace.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The elements or attributes associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @fn    void cainteoir::xml::context::set(const ns &aNS, const std::initializer_list<const entry_ref> &entries, buffer::match_type match)
  * @brief Set the context entries to use for the specified namespace.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The elements or attributes associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @brief Find the entry for the specified element or attribute.
  *
  * @param[in] aNS   The namespace of the element/attribute.
  * @param[in] aNode The element/attribute name.
  *
  * @return The entry associated with the element/attribute.
  */
const cainteoir::xml::context::entry *cainteoir::xml::context::lookup(const std::string &aNS, const cainteoir::buffer &aNode) const
{
	auto entryset = mNodes.find(aNS);
	if (entryset == mNodes.end() && *(--aNS.end()) == '#')
		entryset = mNodes.find(aNS.substr(0, aNS.size() - 1));

	if (entryset == mNodes.end())
		return &unknown_context;

	int begin = 0;
	int end = entryset->second.first.size() - 1;

	while (begin <= end)
	{
		int pos = (begin + end) / 2;

		int comp = aNode.compare((*(entryset->second.first.begin() + pos)).name, entryset->second.second);
		if (comp == 0)
			return (entryset->second.first.begin() + pos)->data;
		else if (comp < 0)
			begin = pos + 1;
		else
			end = pos - 1;
	}

	return &unknown_context;
}

cainteoir::xml::reader::ParserContext::ParserContext(ParserState aState, const char *aCurrent)
	: state(aState)
	, current(aCurrent)
	, nodeName(nullptr, nullptr)
	, nodePrefix(nullptr, nullptr)
{
}

/// @addtogroup xml
/// @{

/** @struct cainteoir::xml::reader
  * @brief  Parse an XML document.
  */

/// @}

/** @brief Create a new XML document reader.
  *
  * @param[in] aData               The XML document contents.
  * @param[in] aDefaultEncoding    The character encoding to use by default.
  * @param[in] aPredefinedEntities The DTD entities to use by default.
  */
cainteoir::xml::reader::reader(std::shared_ptr<cainteoir::buffer> aData, const char *aDefaultEncoding, const entity_set *aPredefinedEntities[52])
	: mData(aData)
	, mTagNodeName(nullptr, nullptr)
	, mTagNodePrefix(nullptr, nullptr)
	, mState(ParsingXml, aData->begin())
	, mSavedState(ParsingXml, aData->begin())
	, mContext(&unknown_context)
	, mPredefinedEntities(aPredefinedEntities)
	, mEncoding(aDefaultEncoding)
{
	mNodeType = textNode;

	skip_whitespace();

	const char * startPos = mState.current;
	if (*mState.current == '<' && read() && mNodeType != error)
	{
		if (mNodeType == beginTagNode)
			mNamespaces.pop_block();

		mState.current = startPos;
		mTagNodeName = cainteoir::buffer(nullptr, nullptr);
		mNodeType = textNode;
	}
	else
	{
		mState.current = mData->begin();
		mState.state = ParsingText; // looks like text
	}
}

/** @fn    void cainteoir::xml::reader::set_predefined_entities(const entity_set *aPredefinedEntities[52])
  * @brief Set the DTD entities to use by default.
  *
  * @param[in] aPredefinedEntities The DTD entities to use by default.
  */

/** @brief Read the next node in the XML document.
  *
  * @retval true  If the next node was read.
  * @retval false If there are no more nodes to read.
  */
bool cainteoir::xml::reader::read()
{
	mState.nodeName = cainteoir::buffer(nullptr, nullptr);
	mState.nodePrefix = cainteoir::buffer(nullptr, nullptr);
	mContext = &unknown_context;

	if (mState.current >= mData->end())
	{
		mNodeType = endOfData;
		return false;
	}

	cainteoir::buffer oldName = mState.nodeName;

	if (mNodeType == endTagNode && mState.state != ParsingXmlNamespaces)
		mNamespaces.pop_block();

	if (mState.state == ParsingText)
	{
		mNodeType = textNode;
		mNodeValue = mEncoding.decode(std::make_shared<cainteoir::buffer>(mState.current, mData->end()));
		mState.current = mData->end();
		return true;
	}

	const char * startPos = nullptr;

	if (mTagNodeName.begin())
	{
		skip_whitespace();

		if (
		    // XML§3.1     -- Empty-Element Tag
		    (mState.current[0] == '/' && mState.current[1] == '>') ||
		    // XML§2.6     -- processing instruction
		    (mState.current[0] == '?' && mState.current[1] == '>' &&
		     (mState.state == ParsingXmlProcessingInstructionAttributes ||
		      mState.state == ParsingProcessingInstructionAttributes)) ||
		    // HTML§12.1.2 -- void elements
		    (mState.current[0] == '>' &&
		     mState.state == ParsingXmlContainedTagAttributes))
		{
			mState.nodeName = mTagNodeName;
			mState.nodePrefix = mTagNodePrefix;
			mNodeType = (*mState.current == '?') ? endProcessingInstructionNode : endTagNode;
			reset_context();
			if (mState.state == ParsingXmlContainedTagAttributes)
			{
				if (*mState.current == '/')
					++mState.current;
				mTagNodeName = cainteoir::buffer(nullptr, nullptr);
			}
			mState.state = ParsingXml;
			++mState.current;
			return true;
		}

		if (xmlalnum(*mState.current)) // XML§3.1 ; HTML§12.1.2.3
		{
			read_tag(attribute);
			reset_context();
			if (check_next('='))
			{
				if (check_next('"')) // XML§3.1 ; HTML§12.1.2.3 -- double-quoted attribute value
				{
					read_node_value('"');
					++mState.current;
				}
				else if (check_next('\'')) // XML§3.1 ; HTML§12.1.2.3 -- single-quoted attribute value
				{
					read_node_value('\'');
					++mState.current;
				}
				else // HTML§12.1.2.3 -- unquoted attribute value
					mNodeValue = unquoted_node_value();
			}
			else // HTML§12.1.2.3 -- empty attribute
				mNodeValue = mEncoding.decode(std::make_shared<cainteoir::buffer>(mState.nodeName));

			if (mState.state == ParsingXmlProcessingInstructionAttributes)
			{
				if (!mState.nodeName.compare("encoding"))
					set_encoding(mNodeValue.str().c_str());
			}

			return true;
		}

		if (*mState.current == '>') // XML§3.1 ; HTML§12.1.2.1-2 -- end of start/end tag
		{
			++mState.current;
			mTagNodeName = cainteoir::buffer(nullptr, nullptr);
		}
		else // error -- skip to end of element tag
		{
			while (mState.current != mData->end() && *mState.current != '>')
				++mState.current;
			++mState.current;

			mNodeType = error;
			mTagNodeName = cainteoir::buffer(nullptr, nullptr);
			return true;
		}
	}

	if (*mState.current == '<')
	{
		switch (*++mState.current)
		{
		case '!':
			if (mState.current[1] == '-' && mState.current[2] == '-') // XML§2.5 ; HTML§12.1.6 -- comment
			{
				++mState.current;
				++mState.current;
				mNodeType = commentNode;
				startPos = ++mState.current;
				while (mState.current != mData->end() && !(mState.current[0] == '-' && mState.current[1] == '-' && mState.current[2] == '>'))
					++mState.current;
				mNodeValue = mEncoding.decode(std::make_shared<cainteoir::buffer>(startPos, mState.current));
				mState.current += 3;
			}
			else if (mState.current[1] == '[' && mState.current[2] == 'C' && mState.current[3] == 'D' && mState.current[4] == 'A' &&
			         mState.current[5] == 'T' && mState.current[6] == 'A' && mState.current[7] == '[') // XML§2.7 ; HTML§12.1.5 -- CDATA section
			{
				mState.current += 8;
				mNodeType = cdataNode;
				startPos = mState.current;
				while (mState.current != mData->end() && !(mState.current[0] == ']' && mState.current[1] == ']' && mState.current[2] == '>'))
					++mState.current;
				mNodeValue = mEncoding.decode(std::make_shared<cainteoir::buffer>(startPos, mState.current));
				mState.current += 3;
			}
			else // DTD
			{
				++mState.current;
				cainteoir::buffer type = identifier();

				bool skip_dtd_node = false;

				if (!type.comparei("DOCTYPE")) // XML§2.8 ; HTML§12.1.1
				{
					mState.nodeName = identifier();
					if (check_next(':'))
					{
						mState.nodePrefix = mState.nodeName;
						mState.nodeName   = identifier();
					}
					mNodeType = doctypeNode;

					while (mState.current != mData->end() && !(*mState.current == '>' || *mState.current == '['))
						++mState.current;

					if (*mState.current == '[')
					{
						cainteoir::buffer tagName = mState.nodeName;
						cainteoir::buffer tagPrefix = mState.nodePrefix;

						mState.state = ParsingDtd;
						while (read() && mState.state == ParsingDtd) switch (mNodeType)
						{
						case dtdEntity:
							mDoctypeEntities[mState.nodeName.str()] = mNodeValue.str();
							break;
						}
						mState.state = ParsingXml;

						mNodeType = doctypeNode;
						mState.nodeName = tagName;
						mState.nodePrefix = tagPrefix;
					}
					else
						++mState.current;
				}
				else if (!type.comparei("ENTITY")) // XML§4.2
				{
					if (check_next('%'))
						skip_dtd_node = true;
					else
					{
						mState.nodeName = identifier();
						if (check_next('"'))
						{
							mNodeType = dtdEntity;
							read_node_value('"');
							++mState.current;
						}
						else
							skip_dtd_node = true;
					}
				}
				else
					skip_dtd_node = true;

				if (skip_dtd_node)
				{
					mNodeType = error;

					while (mState.current != mData->end() && *mState.current != '>')
						++mState.current;
					++mState.current;
				}
			}
			break;
		case '?': // XML§2.6 -- processing instruction
			++mState.current;
			read_tag(beginProcessingInstructionNode);
			if (!mState.nodeName.compare("xml"))
				mState.state = ParsingXmlProcessingInstructionAttributes;
			else
				mState.state = ParsingProcessingInstructionAttributes;
			break;
		case '/': // XML§3.1 ; HTML§12.1.2.2 -- End Tag
			++mState.current;
			read_tag(endTagNode);
			reset_context();
			break;
		default: // XML§3.1 ; HTML§12.1.2.1 -- Start Tag
			if (mState.state != ParsingXmlNamespaces)
			{
				mNamespaces.push_block();
				read_tag(beginTagNode);

				mSavedState = mState;
				mState.state = ParsingXmlNamespaces;

				while (read() && mNodeType == attribute)
				{
					if (!mState.nodePrefix.compare("xmlns"))
						mNamespaces.add_namespace(mState.nodeName, mNodeValue.buffer());
					else if (!mState.nodeName.compare("xmlns") && mState.nodePrefix.empty())
						mNamespaces.add_namespace(cainteoir::buffer(nullptr, nullptr), mNodeValue.buffer());
				}

				mState = mSavedState;
				mState.state = ParsingXmlTagAttributes;
				mTagNodeName = mState.nodeName;
				mTagNodePrefix = mState.nodePrefix;

				mNodeType = beginTagNode;
				reset_context();
				set_begin_tag_type(mContext->begin_tag_type);
			}
			break;
		}
	}
	else if (mState.state == ParsingDtd) // DOCTYPE DTD
	{
		if (mState.current[0] == ']' && mState.current[1] == '>')
		{
			mState.state = ParsingXml;
			mState.current += 2;
		}
		else
		{
			mNodeType = textNode;
			read_node_value('<', ']');
		}
	}
	else // HTML§12.1.3 -- text
	{
		mNodeType = textNode;
		read_node_value('<');
	}

	return true;
}

/** @fn    const cainteoir::rope &cainteoir::xml::reader::nodeValue() const
  * @brief Get the content of the current node.
  *
  * @return The content of the current node.
  */

/** @fn    const cainteoir::buffer &cainteoir::xml::reader::nodeName() const
  * @brief Get the name of the current node.
  *
  * @return The name of the current node.
  */

/** @fn    const cainteoir::buffer &cainteoir::xml::reader::nodePrefix() const
  * @brief Get the namespace prefix of the current node.
  *
  * @return The namespace prefix of the current node.
  */

/** @brief Get the namespace URI of the current node.
  *
  * @return The namespace URI of the current node.
  */
std::string cainteoir::xml::reader::namespaceUri() const
{
	if (mState.nodeName.compare("xmlns"))
		return mNamespaces.lookup(mState.nodePrefix);
	return std::string();
}

/** @fn    cainteoir::xml::reader::node_type cainteoir::xml::reader::nodeType() const
  * @brief Get the type of the current node.
  *
  * @return The type of the current node.
  */

/** @fn    bool cainteoir::xml::reader::isPlainText() const
  * @brief Is the document plain text?
  *
  * @retval true  If the document is plain text.
  * @retval false If the document is not plain text.
  */

/** @fn    const xml::context::entry *cainteoir::xml::reader::context() const
  * @brief Get the context entry for the current node.
  *
  * @return The context entry for the current node.
  */

/** @fn void cainteoir::xml::reader::set_nodes(const std::string &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match)
  * @brief Set the context entries for resolving the namespace's elements.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The elements associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @fn void cainteoir::xml::reader::set_nodes(const ns &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match)
  * @brief Set the context entries for resolving the namespace's elements.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The elements associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @fn void cainteoir::xml::reader::set_attrs(const std::string &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match)
  * @brief Set the context entries for resolving the namespace's attributes.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The attributes associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @fn void cainteoir::xml::reader::set_attrs(const ns &aNS, const std::initializer_list<const xml::context::entry_ref> &entries, buffer::match_type match)
  * @brief Set the context entries for resolving the namespace's attributes.
  *
  * @param[in] aNS     The namespace to associate the entries with.
  * @param[in] entries The attributes associated with the namespace.
  * @param[in] match   The comparison method to use (case sensitive or insensitive).
  */

/** @fn    bool cainteoir::xml::reader::set_encoding(const char *encoding)
  * @brief Change the character encoding used by the parser.
  *
  * @param[in] encoding The character encoding to change to.
  *
  * @retval true  If the character encoding was changed.
  * @retval false If the character encoding was not changed.
  */

/** @fn    const char *cainteoir::xml::reader::current() const
  * @brief Get the current position in the XML document.
  *
  * @return The current position in the XML document.
  */

/** @brief Set the way begin tags are interpreted.
  * @see   cainteoir::xml::begin_tag_type
  *
  * @param[in] aType The way begin tags are to be interpreted.
  */
void cainteoir::xml::reader::set_begin_tag_type(begin_tag_type aType)
{
	if (mState.state == ParsingXmlTagAttributes) switch (aType)
	{
	case begin_tag_type::open:
		mState.state = ParsingXmlTagAttributes;
		break;
	case begin_tag_type::open_close:
		mState.state = ParsingXmlContainedTagAttributes;
		break;
	}
}

void cainteoir::xml::reader::skip_whitespace()
{
	while (mState.current != mData->end() && xmlspace(*mState.current))
		++mState.current;
}

bool cainteoir::xml::reader::expect_next(char c)
{
	skip_whitespace();

	if (*mState.current == c)
	{
		++mState.current;
		return true;
	}

	mNodeType = error;
	return false;
}

bool cainteoir::xml::reader::check_next(char c)
{
	skip_whitespace();

	if (mState.current != mData->end() && *mState.current == c)
	{
		++mState.current;
		return true;
	}

	return false;
}

cainteoir::buffer cainteoir::xml::reader::identifier()
{
	skip_whitespace();

	const char * startPos = mState.current;

	while (mState.current != mData->end() && (xmlalnum(*mState.current)) || *mState.current == '-' || *mState.current == '_')
		++mState.current;

	return cainteoir::buffer(startPos, mState.current);
}

std::shared_ptr<cainteoir::buffer> cainteoir::xml::reader::unquoted_node_value()
{
	skip_whitespace();

	const char * startPos = mState.current;

	while (mState.current != mData->end() && (xmlalnum(*mState.current)) || *mState.current == '-' || *mState.current == '_' || *mState.current == '%')
		++mState.current;

	return std::make_shared<cainteoir::buffer>(startPos, mState.current);
}

void cainteoir::xml::reader::read_node_value(char terminator1, char terminator2)
{
	mNodeValue.clear();
	do
	{
		const char *startPos = mState.current;
		if (*mState.current == '&') // XML§4.1 ; HTML§12.1.4 -- character and entity references
		{
			++mState.current;
			if (*mState.current == '#')
				++mState.current;

			while (mState.current != mData->end() && xmlalnum(*mState.current))
				++mState.current;

			if (*mState.current == ';')
			{
				std::shared_ptr<cainteoir::buffer> entity = parse_entity(cainteoir::buffer(startPos+1, mState.current), mPredefinedEntities, mDoctypeEntities);
				if (entity)
					mNodeValue += entity;
				++mState.current;
				continue;
			}
		}

		while (mState.current != mData->end() && !(*mState.current == '&' || *mState.current == terminator1 || *mState.current == terminator2))
			++mState.current;
		mEncoding.decode(std::make_shared<cainteoir::buffer>(startPos, mState.current), mNodeValue);
	} while (mState.current != mData->end() && !(*mState.current == terminator1 || *mState.current == terminator2));
}

void cainteoir::xml::reader::read_tag(node_type aType)
{
	skip_whitespace();

	mState.nodeName = identifier();
	if (check_next(':'))
	{
		mState.nodePrefix = mState.nodeName;
		mState.nodeName   = identifier();
	}

	mNodeType = aType;
	if (aType != attribute)
	{
		mTagNodeName = mState.nodeName;
		mTagNodePrefix = mState.nodePrefix;
	}
}

void cainteoir::xml::reader::reset_context()
{
	switch (mNodeType)
	{
	case attribute:
		mContext = mAttrs.lookup(namespaceUri(), nodeName());
		break;
	case beginTagNode:
	case endTagNode:
		mContext = mNodes.lookup(namespaceUri(), nodeName());
		break;
	}
}

/** @namespace cainteoir::xml::xmlns
  * @brief     Predefined XML namespaces.
  */

/// @addtogroup xml
/// @{

/** @struct cainteoir::xml::ns
  * @brief  Manages an XML namespace.
  */

/// @}

/** @var   cainteoir::xml::ns::prefix
  * @brief The default prefix for the namespace.
  */

/** @var   cainteoir::xml::ns::href
  * @brief The URI of the namespace.
  */

/** @var   cainteoir::xml::ns::ns(const std::string &aPrefix, const std::string &aHref)
  * @brief Create a new XML namespace object.
  *
  * @param[in] aPrefix The prefix to use for the namespace.
  * @param[in] aHref   The URI of the namespace.
  */

/// @addtogroup xml
/// @{

/** @var   const cainteoir::xml::detail::entity_set *cainteoir::xml::xml_entities[52]
  * @brief XML 1.0 entities.
  */

/** @var   const cainteoir::xml::detail::entity_set *cainteoir::xml::html_entities[52]
  * @brief HTML 5.0 entities.
  */

/// @}

/** @struct cainteoir::xml::context::entry
  * @brief  Identifies an XML element or attribute.
  */

/** @var   cainteoir::xml::context::entry::context
  * @brief The document context associated with the element/attribute.
  */

/** @var   cainteoir::xml::context::entry::parameter
  * @brief The parameter associated with the document context.
  */

/** @var   cainteoir::xml::context::entry::parse_type
  * @brief Information on how to parse the element/attribute.
  */

/** @struct cainteoir::xml::context::entry_ref
  * @brief  Associates an element/attribute name with its entry object.
  */

/** @var   cainteoir::xml::context::entry_ref::name
  * @brief The name of the element/attribute.
  */

/** @var   cainteoir::xml::context::entry_ref::data
  * @brief The entry data associated with the element/attribute.
  */

/** @enum  cainteoir::xml::reader::node_type
  * @brief Specifies the type of the XML node.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::beginTagNode
  * @brief An start of an element.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::endTagNode
  * @brief An end of an element.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::beginProcessingInstructionNode
  * @brief The start of a processing instruction.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::endProcessingInstructionNode
  * @brief The end of a processing instruction.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::commentNode
  * @brief A comment.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::cdataNode
  * @brief A CDATA section.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::textNode
  * @brief A block of text.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::error
  * @brief An error occurred parsing the XML; the XML may be malformed.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::doctypeNode
  * @brief A DOCTYPE.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::attribute
  * @brief An attribute on an element or processing instruction.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::endOfData
  * @brief The document has no more XML data.
  */

/** @var   cainteoir::xml::reader::node_type cainteoir::xml::reader::dtdEntity
  * @brief An ENTITY reference declaration.
  */

/// @addtogroup xml
/// @{

/** @enum  cainteoir::xml::begin_tag_type
  * @brief Specifies how begin tags are interpreted.
  */

/// @}

/** References
  *
  *    XML  [http://www.w3.org/TR/2008/REC-xml-20081126/] -- Extensible Markup Language (XML) 1.0 (Fifth Edition)
  *    HTML [http://www.whatwg.org/specs/web-apps/current-work/multipage/] -- HTML Living Standard
  */
