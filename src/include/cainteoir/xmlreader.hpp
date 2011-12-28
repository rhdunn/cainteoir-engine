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

namespace cainteoir { namespace xml
{
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

		node_type nodeType() const { return mNodeType; }

		bool isPlainText() const { return mParseAsText; }
	private:
		void skip_whitespace();

		bool expect_next(char c);
		bool check_next(char c);

		cainteoir::buffer identifier();

		void read_node_value(char terminator);

		void read_tag(node_type aType);

		std::tr1::shared_ptr<cainteoir::buffer> mData;
		const char * mCurrent;
		bool mParseAsText;

		cainteoir::rope mNodeValue;
		cainteoir::buffer mNodeName;
		cainteoir::buffer mNodePrefix;
		cainteoir::buffer mTagNodeName;
		cainteoir::buffer mTagNodePrefix;
		node_type mNodeType;
	};
}}

#endif
