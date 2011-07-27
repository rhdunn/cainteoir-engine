/* XML DOM API.
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

#ifndef CAINTEOIR_ENGINE_XML_HPP
#define CAINTEOIR_ENGINE_XML_HPP

#include <cainteoir/buffer.hpp>
#include <cainteoir/metadata.hpp>
#include <stdexcept>

#include <libxml/parser.h>

namespace cainteoir { namespace xmldom
{
	template <typename T>
	class xmlnode
	{
	public:
		xmlnode(T aItem) : item(aItem)
		{
		}

		void next()
		{
			item = item->next;
		}

		bool isValid() const
		{
			return item != NULL;
		}

		const char *name() const
		{
			if (!item) return "";
			return (const char *)item->name;
		}

		std::string namespaceURI() const
		{
			if (!item->ns) return "";
			std::string ns = (const char *)item->ns->href;
			if (*(--ns.end()) != '#' && *(--ns.end()) != '/')
				ns.push_back('#');
			return ns;
		}

		const rdf::uri uri() const
		{
			return rdf::uri(namespaceURI(), name());
		}
	protected:
		T item;
	};

	class attribute : public xmlnode<xmlAttrPtr>
	{
	public:
		attribute(xmlAttrPtr aAttr)
			: xmlnode<xmlAttrPtr>(aAttr)
		{
		}

		std::string content() const;
	};

	inline bool operator==(const attribute &a, const rdf::uri &b)
	{
		return a.uri() == b;
	}

	inline bool operator==(const rdf::uri &a, const attribute &b)
	{
		return a == b.uri();
	}

	inline bool operator!=(const attribute &a, const rdf::uri &b)
	{
		return !(a.uri() == b);
	}

	inline bool operator!=(const rdf::uri &a, const attribute &b)
	{
		return !(a == b.uri());
	}

	class node : public xmlnode<xmlNodePtr>
	{
	public:
		node(xmlNodePtr aNode)
			: xmlnode<xmlNodePtr>(aNode)
		{
		}

		node firstChild() const
		{
			return node(item->children);
		}

		attribute firstAttribute() const
		{
			return attribute(item->properties);
		}

		attribute attr(const rdf::uri &aUri) const
		{
			for (attribute attr = firstAttribute(); attr.isValid(); attr.next())
			{
				if (attr == aUri)
					return attr;
			}
			return attribute(NULL);
		}

		xmlElementType type() const
		{
			return item->type;
		}

		std::tr1::shared_ptr<cainteoir::buffer> content(bool normalize = true) const;
	};

	inline bool operator==(const node &a, const rdf::uri &b)
	{
		return a.uri() == b;
	}

	inline bool operator==(const rdf::uri &a, const node &b)
	{
		return a == b.uri();
	}

	inline bool operator!=(const node &a, const rdf::uri &b)
	{
		return !(a.uri() == b);
	}

	inline bool operator!=(const rdf::uri &a, const node &b)
	{
		return !(a == b.uri());
	}

	class document
	{
	public:
		document(std::tr1::shared_ptr<buffer> aBuffer)
			: data(aBuffer)
		{
			int substitute = xmlSubstituteEntitiesDefault(1);
			doc = xmlParseMemory(data->begin(), data->size());
			xmlSubstituteEntitiesDefault(substitute);

			if (doc == NULL)
				throw std::runtime_error("unable to parse XML document");
		}

		~document()
		{
			xmlFreeDoc(doc);
		}

		node root()
		{
			return node(xmlDocGetRootElement(doc));
		}
	private:
		std::tr1::shared_ptr<buffer> data;
		xmlDocPtr doc;
	};
}}

#endif
