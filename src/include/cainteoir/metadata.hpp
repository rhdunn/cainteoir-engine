/* MetaData API.
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

#ifndef CAINTEOIR_ENGINE_METADATA_HPP
#define CAINTEOIR_ENGINE_METADATA_HPP

#include <tr1/memory>
#include <sstream>
#include <string>
#include <list>
#include <set>

namespace cainteoir { namespace rdf
{
	/** @brief RDF node
	  */
	struct node
	{
		virtual const node *clone() const = 0;

		virtual ~node() {}
	};

	/** @brief RDF resource
	  */
	struct resource : public node
	{
	};

	/** @brief RDF blank node resource
	  */
	class bnode : public resource
	{
	public:
		const std::string id; /**< @brief The ID for the blank node. */

		bnode(const std::string &aID)
			: id(aID)
		{
		}

		const resource *clone() const
		{
			return new bnode(*this);
		}
	};

	/** @brief RDF URI resource
	  */
	class uri : public resource
	{
	private:
		static const std::string make_uri(const std::string &aNS, const std::string &aRef)
		{
			if (aNS.empty() && aRef.empty())
				return std::string();

			char nstype = *(--aNS.end());
			if (nstype == '#' || nstype == '/')
				return aNS + aRef;
			return aNS + "#" + aRef;
		}
	public:
		const std::string value; /**< @brief The full path of the URI resource. */
		const std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		const std::string ref;   /**< @brief The URI reference. */

		uri(const std::string &aNS, const std::string &aRef)
			: value(make_uri(aNS, aRef))
			, ns(aNS)
			, ref(aRef)
		{
		}

		const resource *clone() const
		{
			return new uri(*this);
		}
	};

	/** @breif RDF namespace.
	  */
	class ns
	{
	public:
		const std::string href;   /**< @brief The path of the namespace. */
		const std::string prefix; /**< @brief The default prefix for the namespace. */

		ns(const std::string &aPrefix, const std::string &aHref)
			: href(aHref)
			, prefix(aPrefix)
		{
		}

		/** @brief Create a URI in the namespace.
		  *
		  * @param aRef The URI reference relative to the namespace.
		  */
		uri operator()(const std::string &aRef) const
		{
			return uri(href, aRef);
		}
	};

	inline bool operator==(const ns &a, const char *b)
	{
		return a.href == b;
	}

	inline bool operator==(const char *a, const ns &b)
	{
		return b == a;
	}

	extern const ns rdf;     /**< @brief RDF syntax namespace. */
	extern const ns rdfs;    /**< @brief RDF schema namespace. */
	extern const ns xsd;     /**< @brief XMLSchema namespace. */
	extern const ns xml;     /**< @brief XML namespace. */

	extern const ns dc;      /**< @brief Dublin Core: Elements namespace. */
	extern const ns dcterms; /**< @brief Dublin Core: Terms namespace. */

	extern const ns ocf;     /**< @brief Open Container Format (OCF) namespace. */
	extern const ns opf;     /**< @brief Open Publication Format (OPF) namespace. */

	extern const ns foaf;    /**< @brief Friend of a Family (FOAF) namespace. */
	extern const ns tts;     /**< @brief Cainteoir Text-to-Speech RDF namespace. */

	/** @brief RDF literal node
	  */
	class literal : public node
	{
	public:
		const std::string value;    /**< @brief The content of the literal string. */
		const std::string language; /**< @brief The language the literal string is written in [optional]. */
		const uri type;             /**< @brief The type of the literal string [optional]. */

		literal(const std::string &aValue)
			: value(aValue)
			, type(std::string(), std::string())
		{
		}

		literal(const std::string &aValue, const std::string &aLanguage)
			: value(aValue)
			, language(aLanguage)
			, type(std::string(), std::string())
		{
		}

		literal(const std::string &aValue, const uri &aType)
			: value(aValue)
			, type(aType)
		{
		}

		const node *clone() const
		{
			return new literal(*this);
		}
	};

	/** @brief An RDF statement (triple)
	  */
	class statement
	{
	public:
		const std::tr1::shared_ptr<const resource> subject;
		const uri predicate;
		const std::tr1::shared_ptr<const node> object;

		template<typename Resource, typename Object>
		statement(const Resource &aSubject, const uri &aPredicate, const Object &aObject)
			: subject(aSubject.clone())
			, predicate(aPredicate)
			, object(aObject.clone())
		{
		}
	};

	/** @brief RDF model
	  */
	class model : public std::list<statement>
	{
	public:
		model()
			: nextid(1)
		{
		}

		const rdf::bnode genid()
		{
			std::ostringstream id;
			id << "id" << nextid;
			++nextid;
			return rdf::bnode(id.str());
		}

		bool contains(const ns &uri) const
		{
			return namespaces.find(uri.href) != namespaces.end();
		}

		void push_back(const statement &s)
		{
			{
				const rdf::uri *uri = dynamic_cast<const rdf::uri *>(s.subject.get());
				if (uri)
					namespaces.insert(uri->ns);
			}
			namespaces.insert(s.predicate.ns);
			{
				const rdf::uri *uri = dynamic_cast<const rdf::uri *>(s.object.get());
				if (uri)
					namespaces.insert(uri->ns);

				const rdf::literal *literal = dynamic_cast<const rdf::literal *>(s.object.get());
				if (literal && !literal->type.ns.empty())
					namespaces.insert(literal->type.ns);
			}
			std::list<statement>::push_back(s);
		}
	private:
		std::set<std::string> namespaces;
		int nextid;
	};

	/** @brief RDF formatter (serialisation support)
	  */
	struct formatter
	{
		enum format_type
		{
			/** @brief N-Triple format
			  * @see   http://www.w3.org/TR/rdf-testcases/#ntriples
			  */
			ntriple,
			/** @brief turtle format
			  * @see   http://www.w3.org/TeamSubmission/turtle/
			  */
			turtle,
		};

		virtual formatter &operator<<(const ns &aNS) = 0;
		virtual formatter &operator<<(const bnode &bnode) = 0;
		virtual formatter &operator<<(const uri &uri) = 0;
		virtual formatter &operator<<(const literal &literal) = 0;
		virtual formatter &operator<<(const statement &statement) = 0;
		virtual formatter &operator<<(const model &model) = 0;

		virtual ~formatter() {}
	};

	std::tr1::shared_ptr<formatter> create_formatter(std::ostream &aStream, formatter::format_type aFormatType);
}}

#endif
