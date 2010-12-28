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

#ifndef CAINTEOIR_ENGINE_INTERNAL_XML_HPP
#define CAINTEOIR_ENGINE_INTERNAL_XML_HPP

#include <cainteoir/buffer.hpp>
#include <stdexcept>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

namespace cainteoir { namespace xml
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

		const char * c_str() const
		{
			return (const char *)data;
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
			return (const char *)attr->name;
		}

		const char *namespaceURI() const
		{
			if (!attr->ns) return "";
			return (const char *)attr->ns->href;
		}

		string content() const
		{
			return string(xmlNodeListGetString(attr->doc, attr->children, 1));
		}
	private:
		xmlAttrPtr attr;
	};

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

		node firstChild()
		{
			return node(item->children);
		}

		attribute firstAttribute()
		{
			return attribute(item->properties);
		}

		xmlElementType type() const
		{
			return item->type;
		}

		const char *name() const
		{
			return (const char *)item->name;
		}

		const char *namespaceURI() const
		{
			if (!item->ns) return "";
			return (const char *)item->ns->href;
		}

		string content() const
		{
			return string(xmlNodeGetContent(item));
		}
	private:
		xmlNodePtr item;
	};

	class document
	{
	public:
		document(buffer *data)
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
		xmlDocPtr doc;
	};
}}

#endif
