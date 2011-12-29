/* XML Reader API.
 *
 * Copyright (C) 2010-2011 Reece H. Dunn
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

#include <cainteoir/buffer.hpp>
#include <list>

namespace cainteoir { namespace xml
{
	class uri
	{
	public:
		std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		std::string ref;   /**< @brief The URI reference. */

		uri(const std::string &aNS = std::string(), const std::string &aRef = std::string());

		bool empty() const;

		std::string str() const;
	};

	inline bool operator==(const uri &a, const uri &b)
	{
		return a.ns == b.ns && a.ref == b.ref;
	}

	inline bool operator!=(const uri &a, const uri &b)
	{
		return !(a == b);
	}

	class namespaces
	{
	public:
		namespaces();

		void add_namespace(const cainteoir::buffer &aPrefix, const std::tr1::shared_ptr<cainteoir::buffer> &aUri);

		void push_block();
		void pop_block();

		std::tr1::shared_ptr<cainteoir::buffer> lookup(const cainteoir::buffer &aPrefix) const;
	private:
		struct namespace_item
		{
			long block;
			cainteoir::buffer prefix;
			std::tr1::shared_ptr<cainteoir::buffer> uri;
		};

		std::list<namespace_item> mNamespaces;
		long mBlockNumber;
	};

	class reader
	{
	public:
		enum node_type
		{
			beginTagNode,
			endTagNode,
			processingInstructionNode,
			commentNode,
			cdataNode,
			textNode,
			error,
			doctypeNode,
			attribute,
		};

		reader(std::tr1::shared_ptr<cainteoir::buffer> aData);

		bool read();

		const cainteoir::rope &nodeValue() const { return mNodeValue; }

		const cainteoir::buffer &nodeName() const { return mNodeName; }

		const cainteoir::buffer &nodePrefix() const { return mNodePrefix; }

		cainteoir::buffer namespaceUri() const;

		node_type nodeType() const { return mNodeType; }

		bool isPlainText() const { return mParseAsText; }
	private:
		/** @name parser internals/helpers */
		//@{

		void skip_whitespace();

		bool expect_next(char c);
		bool check_next(char c);

		cainteoir::buffer identifier();

		void read_node_value(char terminator);

		void read_tag(node_type aType);

		std::tr1::shared_ptr<cainteoir::buffer> mData;
		const char * mCurrent;
		bool mParseAsText;
		bool mParseNamespaces;

		namespaces mNamespaces;
		cainteoir::buffer mTagNodeName;
		cainteoir::buffer mTagNodePrefix;

		//@}
		/** @name reader data */
		//@{

		cainteoir::rope mNodeValue;
		cainteoir::buffer mNodeName;
		cainteoir::buffer mNodePrefix;
		node_type mNodeType;

		//@}
	};
}}

#endif
