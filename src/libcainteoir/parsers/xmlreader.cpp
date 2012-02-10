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

#include <cainteoir/xmlreader.hpp>
#include <cainteoir/platform.hpp>

using cainteoir::xml::entity;
using cainteoir::xml::entity_set;

#include "xml-entities.h"
#include "html-entities.h"

const char * cainteoir::xml::lookup_entity(const entity_set **entities, const cainteoir::buffer &data)
{
	char c = *data.begin();

	const entity_set * ent = NULL;
	if (c >= 'a' && c <= 'z')
		ent = entities[c - 'a' + 26];
	else if (c >= 'A' && c <= 'Z')
		ent = entities[c - 'A'];

	if (!ent)
		return NULL;

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

	return NULL;
}

void write_utf8(char * out, long c)
{
	if (c < 0x80)
		*out++ = c;
	else if (c < 0x800)
	{
		*out++ = 0xC0 | (c >> 6);
		*out++ = 0x80 + (c & 0x3F);
	}
	else if (c < 0x10000)
	{
		*out++ = 0xE0 | (c >> 12);
		*out++ = 0x80 + ((c >> 6) & 0x3F);
		*out++ = 0x80 + (c & 0x3F);
	}
	else if (c < 0x200000)
	{
		*out++ = 0xF0 | (c >> 18);
		*out++ = 0x80 + ((c >> 12) & 0x3F);
		*out++ = 0x80 + ((c >>  6) & 0x3F);
		*out++ = 0x80 + (c & 0x3F);
	}
	*out = '\0';
}

std::tr1::shared_ptr<cainteoir::buffer> parse_entity(const cainteoir::buffer &entity, const cainteoir::xml::entity_set **entities, const std::map<std::string, std::string> &doctypeEntities)
{
	const char * str = entity.begin();
	if (*str == '#')
	{
		char utf8[10];
		if (*++str == 'x')
			write_utf8(utf8, strtol(++str, NULL, 16));
		else
			write_utf8(utf8, strtol(str, NULL, 10));

		if (utf8[0])
		{
			std::tr1::shared_ptr<cainteoir::buffer> data(new cainteoir::data_buffer(strlen(utf8)));
			strcpy((char *)data->begin(), utf8);
			return data;
		}
	}
	else
	{
		auto match = doctypeEntities.find(entity.str());
		if (match != doctypeEntities.end())
		{
			std::tr1::shared_ptr<cainteoir::buffer> data(new cainteoir::data_buffer(match->second.size()));
			strcpy((char *)data->begin(), match->second.c_str());
			return data;
		}

		const char * value = cainteoir::xml::lookup_entity(entities, entity);
		if (value)
			return std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(value));
	}

	fprintf(stderr, _("unrecognised entity '%s'\n"), entity.str().c_str());
	return std::tr1::shared_ptr<cainteoir::buffer>();
}

inline bool xmlalnum(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

inline bool xmlspace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

cainteoir::xml::uri::uri(const std::string &aNS, const std::string &aRef)
	: ns(aNS)
	, ref(aRef)
{
	auto last = --ns.end();
	if (!ns.empty() && !ref.empty() && *last != '#' && *last != '/' && *last != ':')
		ns.push_back('#');
}

bool cainteoir::xml::uri::empty() const
{
	return ns.empty() && ref.empty();
}

std::string cainteoir::xml::uri::str() const
{
	if (ref.empty())
		return ns;

	return ns + ref;
}

const cainteoir::xml::resource *cainteoir::xml::uri::clone() const
{
	return new uri(*this);
}

const cainteoir::xml::ns cainteoir::xml::xmlns::dc(   "dc",    "http://purl.org/dc/elements/1.1/");
const cainteoir::xml::ns cainteoir::xml::xmlns::dcam( "dcam",  "http://purl.org/dc/dcam/");
const cainteoir::xml::ns cainteoir::xml::xmlns::dct(  "dct",   "http://purl.org/dc/terms/");
const cainteoir::xml::ns cainteoir::xml::xmlns::dtb(  "dtb",   "http://www.daisy.org/z3986/2005/dtbook/");
const cainteoir::xml::ns cainteoir::xml::xmlns::epub( "epub",  "http://www.idpf.org/2007/ops");
const cainteoir::xml::ns cainteoir::xml::xmlns::foaf( "foaf",  "http://xmlns.com/foaf/0.1/");
const cainteoir::xml::ns cainteoir::xml::xmlns::media("media", "http://www.idpf.org/epub/vocab/overlays/");
const cainteoir::xml::ns cainteoir::xml::xmlns::ncx(  "ncx",   "http://www.daisy.org/z3986/2005/ncx/");
const cainteoir::xml::ns cainteoir::xml::xmlns::ocf(  "ocf",   "urn:oasis:names:tc:opendocument:xmlns:container");
const cainteoir::xml::ns cainteoir::xml::xmlns::opf(  "opf",   "http://www.idpf.org/2007/opf");
const cainteoir::xml::ns cainteoir::xml::xmlns::owl(  "owl",   "http://www.w3.org/2002/07/owl");
const cainteoir::xml::ns cainteoir::xml::xmlns::pkg(  "pkg",   "http://www.idpf.org/epub/vocab/package/");
const cainteoir::xml::ns cainteoir::xml::xmlns::rdf(  "rdf",   "http://www.w3.org/1999/02/22-rdf-syntax-ns");
const cainteoir::xml::ns cainteoir::xml::xmlns::rdfa( "rdfa",  "http://www.w3.org/ns/rdfa");
const cainteoir::xml::ns cainteoir::xml::xmlns::rdfs( "rdfs",  "http://www.w3.org/2000/01/rdf-schema");
const cainteoir::xml::ns cainteoir::xml::xmlns::skos( "skos",  "http://www.w3.org/2004/02/skos/core");
const cainteoir::xml::ns cainteoir::xml::xmlns::smil( "smil",  "http://www.w3.org/ns/SMIL");
const cainteoir::xml::ns cainteoir::xml::xmlns::ssml( "ssml",  "http://www.w3.org/2001/10/synthesis");
const cainteoir::xml::ns cainteoir::xml::xmlns::tts(  "tts",   "http://rhdunn.github.com/2010/12/text-to-speech");
const cainteoir::xml::ns cainteoir::xml::xmlns::xhtml("h",     "http://www.w3.org/1999/xhtml");
const cainteoir::xml::ns cainteoir::xml::xmlns::xml(  "xml",   "http://www.w3.org/XML/1998/namespace");
const cainteoir::xml::ns cainteoir::xml::xmlns::xsd(  "xsd",   "http://www.w3.org/2001/XMLSchema");

cainteoir::xml::namespaces::namespaces()
	: mBlockNumber(-1)
{
	add_namespace("xml", "http://www.w3.org/XML/1998/namespace");
}

cainteoir::xml::namespaces &cainteoir::xml::namespaces::add_namespace(const std::string &aPrefix, const std::string &aHref)
{
	mNamespaces.push_back(namespace_item(mBlockNumber, ns(aPrefix, aHref)));
	return *this;
}

void cainteoir::xml::namespaces::push_block()
{
	++mBlockNumber;
}

void cainteoir::xml::namespaces::pop_block()
{
	--mBlockNumber;
	while (mNamespaces.back().block > mBlockNumber && !mNamespaces.empty())
	{
		mNamespaces.pop_back();
	}
}

std::string cainteoir::xml::namespaces::lookup(const std::string &aPrefix) const
{
	for (auto ns = mNamespaces.rbegin(), last = mNamespaces.rend(); ns != last; ++ns)
	{
		if (ns->item.prefix == aPrefix && ns->block <= mBlockNumber)
			return ns->item.href;
	}
	return std::string();
}

const cainteoir::xml::context::entry cainteoir::xml::unknown_context = { 0, 0 };

const cainteoir::xml::context::entry cainteoir::xml::base_attr  = { 0, 0 };
const cainteoir::xml::context::entry cainteoir::xml::id_attr    = { 0, 0 };
const cainteoir::xml::context::entry cainteoir::xml::lang_attr  = { 0, 0 };
const cainteoir::xml::context::entry cainteoir::xml::space_attr = { 0, 0 };

const std::initializer_list<const cainteoir::xml::context::entry_ref> cainteoir::xml::attrs =
{
	{ "base",  &xml::base_attr },
	{ "id",    &xml::id_attr },
	{ "lang",  &xml::lang_attr },
	{ "space", &xml::space_attr },
};

const cainteoir::xml::context::entry *cainteoir::xml::context::lookup(const std::string &aNS, const cainteoir::buffer &aNode, const entries &aEntries) const
{
	auto entryset = aEntries.find(aNS);
	if (entryset == aEntries.end() && aNS.back() == '#')
		entryset = aEntries.find(aNS.substr(0, aNS.size() - 1));

	if (entryset == aEntries.end())
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

cainteoir::xml::reader::reader(std::tr1::shared_ptr<cainteoir::buffer> aData, const entity_set *aPredefinedEntities[52])
	: mData(aData)
	, mTagNodeName(NULL, NULL)
	, mTagNodePrefix(NULL, NULL)
	, mState(ParsingXml, aData->begin())
	, mSavedState(ParsingXml, aData->begin())
	, mContext(&unknown_context)
	, mImplicitEndTag(false)
	, mPredefinedEntities(aPredefinedEntities)
{
	mNodeType = textNode;

	skip_whitespace();

	const char * startPos = mState.current;
	if (*mState.current == '<' && read() && mNodeType != error)
	{
		if (mNodeType == beginTagNode)
			mNamespaces.pop_block();

		mState.current = startPos;
		mTagNodeName = cainteoir::buffer(NULL, NULL);
		mNodeType = textNode;
	}
	else
	{
		mState.current = mData->begin();
		mState.state = ParsingText; // looks like text
	}
}

bool cainteoir::xml::reader::read()
{
	mState.nodeName = cainteoir::buffer(NULL, NULL);
	mState.nodePrefix = cainteoir::buffer(NULL, NULL);
	mContext = &unknown_context;

	if (mState.current >= mData->end())
		return false;

	cainteoir::buffer oldName = mState.nodeName;

	if (mNodeType == endTagNode && mState.state != ParsingXmlNamespaces)
		mNamespaces.pop_block();

	if (mState.state == ParsingText)
	{
		mNodeType = textNode;
		mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(mState.current, mData->end()));
		mState.current = mData->end();
		return true;
	}

	const char * startPos = NULL;

	if (mTagNodeName.begin())
	{
		skip_whitespace();

		if ((mState.current[0] == '/' && mState.current[1] == '>') || // XML§3.1     -- Empty-Element Tag
		    (mState.current[0] == '>' && mImplicitEndTag))            // HTML§12.1.2 -- void elements
		{
			mState.nodeName = mTagNodeName;
			mState.nodePrefix = mTagNodePrefix;
			mNodeType = endTagNode;
			mContext = lookup_node(namespaceUri(), nodeName());
			if (mImplicitEndTag)
			{
				mImplicitEndTag = false;
				mTagNodeName = cainteoir::buffer(NULL, NULL);
			}
			++mState.current;
			return true;
		}

		if (xmlalnum(*mState.current)) // XML§3.1 ; HTML§12.1.2.3
		{
			read_tag(attribute);
			mContext = lookup_attr(namespaceUri(), nodeName());
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
					mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(identifier()));
			}
			else // HTML§12.1.2.3 -- empty attribute
				mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(mState.nodeName));
			return true;
		}

		if (*mState.current == '>') // XML§3.1 ; HTML§12.1.2.1-2 -- end of start/end tag
		{
			++mState.current;
			mTagNodeName = cainteoir::buffer(NULL, NULL);
		}
		else // error -- skip to end of element tag
		{
			while (mState.current != mData->end() && *mState.current != '>')
				++mState.current;
			++mState.current;

			mNodeType = error;
			mTagNodeName = cainteoir::buffer(NULL, NULL);
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
				mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mState.current));
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
				mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mState.current));
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
			mNodeType = processingInstructionNode;
			startPos = ++mState.current;
			while (mState.current != mData->end() && (mState.current[0] != '?' && mState.current[1] != '>'))
				++mState.current;
			mNodeValue = std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mState.current));
			++mState.current;
			++mState.current;
			break;
		case '/': // XML§3.1 ; HTML§12.1.2.2 -- End Tag
			++mState.current;
			read_tag(endTagNode);
			mContext = lookup_node(namespaceUri(), nodeName());
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
						mNamespaces.add_namespace(cainteoir::buffer(NULL, NULL), mNodeValue.buffer());
				}

				mState = mSavedState;
				mTagNodeName = mState.nodeName;
				mTagNodePrefix = mState.nodePrefix;

				mNodeType = beginTagNode;
				mContext = lookup_node(namespaceUri(), nodeName());
				if (mContext->parse_type == xml::context::implicit_end_tag)
					mImplicitEndTag = true;
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

std::string cainteoir::xml::reader::namespaceUri() const
{
	if (mState.nodeName.compare("xmlns"))
		return mNamespaces.lookup(mState.nodePrefix);
	return std::string();
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

	while (mState.current != mData->end() && (xmlalnum(*mState.current)) || *mState.current == '-')
		++mState.current;

	return cainteoir::buffer(startPos, mState.current);
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
				std::tr1::shared_ptr<cainteoir::buffer> entity = parse_entity(cainteoir::buffer(startPos+1, mState.current), mPredefinedEntities, mDoctypeEntities);
				if (entity)
					mNodeValue += entity;
				++mState.current;
				continue;
			}
		}

		while (mState.current != mData->end() && !(*mState.current == '&' || *mState.current == terminator1 || *mState.current == terminator2))
			++mState.current;
		mNodeValue += std::tr1::shared_ptr<cainteoir::buffer>(new cainteoir::buffer(startPos, mState.current));
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

/** References
  *
  *    XML  [http://www.w3.org/TR/2008/REC-xml-20081126/] -- Extensible Markup Language (XML) 1.0 (Fifth Edition)
  *    HTML [http://www.whatwg.org/specs/web-apps/current-work/multipage/] -- HTML Living Standard
  */
