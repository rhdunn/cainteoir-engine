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

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

namespace cainteoir { namespace xmldom
{
	class string
	{
	public:
		string(xmlChar *aString)
			: data(aString)
		{
		}

		~string()
		{
			xmlFree(data);
		}

		std::string str() const
		{
			return data ? (const char *)data : std::string();
		}
	private:
		xmlChar *data;
	};

	class attribute
	{
	public:
		attribute(xmlAttrPtr aAttr)
			: attr(aAttr)
		{
		}

		void next()
		{
			attr = attr->next;
		}

		bool isValid() const
		{
			return attr != NULL;
		}

		const char *name() const
		{
			if (!attr) return "";
			return (const char *)attr->name;
		}

		const char *namespaceURI() const
		{
			if (!attr->ns) return "";
			return (const char *)attr->ns->href;
		}

		std::string content() const
		{
			if (!attr) return std::string();
			return string(xmlNodeListGetString(attr->doc, attr->children, 1)).str();
		}
	private:
		xmlAttrPtr attr;
	};

	inline bool operator==(const attribute &a, const rdf::uri &b)
	{
		return b.ns == a.namespaceURI() && b.ref == a.name();
	}

	inline bool operator==(const rdf::uri &a, const attribute &b)
	{
		return b == a;
	}

	inline bool operator!=(const attribute &a, const rdf::uri &b)
	{
		return !(a == b);
	}

	inline bool operator!=(const rdf::uri &a, const attribute &b)
	{
		return !(b == a);
	}

	class node
	{
	public:
		node(xmlNodePtr aNode)
			: item(aNode)
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

		const char *name() const
		{
			if (!item) return "";
			return (const char *)item->name;
		}

		const char *namespaceURI() const
		{
			if (!item->ns) return "";
			return (const char *)item->ns->href;
		}

		std::string content() const
		{
			if (!item) return std::string();
			return string(xmlNodeGetContent(item)).str();
		}
	private:
		xmlNodePtr item;
	};

	inline bool operator==(const node &a, const rdf::uri &b)
	{
		return b.ns == a.namespaceURI() && b.ref == a.name();
	}

	inline bool operator==(const rdf::uri &a, const node &b)
	{
		return b == a;
	}

	inline bool operator!=(const node &a, const rdf::uri &b)
	{
		return !(a == b);
	}

	inline bool operator!=(const rdf::uri &a, const node &b)
	{
		return !(b == a);
	}

	class document
	{
	public:
		document(std::tr1::shared_ptr<buffer> aBuffer)
			: data(aBuffer)
		{
			doc = xmlParseMemory(data->begin(), data->size());
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
