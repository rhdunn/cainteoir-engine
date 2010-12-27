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
#include <string>
#include <list>

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
	public:
		const std::string value; /**< @brief The full path of the URI resource. */
		const std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		const std::string ref;   /**< @brief The URI reference. */

		uri(const std::string &aNS, const std::string &aRef)
			: value(aNS + aRef)
			, ns(aNS)
			, ref(aRef)
		{
		}

		const resource *clone() const
		{
			return new uri(*this);
		}
	};

	/** @brief rdf namespace node
	  *
	  * @param aRef The URI reference beonging to the rdf resource namespace.
	  */
	inline const uri rdf(const std::string &aRef)
	{
		return uri("http://www.w3.org/1999/02/22-rdf-syntax-ns#", aRef);
	}

	/** @brief rdfs namespace node
	  *
	  * @param aRef The URI reference beonging to the rdfs resource namespace.
	  */
	inline const uri rdfs(const std::string &aRef)
	{
		return uri("http://www.w3.org/2000/01/rdf-schema#", aRef);
	}

	/** @brief xsd namespace node
	  *
	  * @param aRef The URI reference beonging to the xsd (XMLSchema) resource namespace.
	  */
	inline const uri xsd(const std::string &aRef)
	{
		return uri("http://www.w3.org/2001/XMLSchema#", aRef);
	}

	/** @brief dc namespace node
	  *
	  * @param aRef The URI reference beonging to the dc (Dublin Core: Elements) resource namespace.
	  */
	inline const uri dc(const std::string &aRef)
	{
		return uri("http://purl.org/dc/elements/1.1/", aRef);
	}

	/** @brief dcterms namespace node
	  *
	  * @param aRef The URI reference beonging to the dcterms (Dublin Core: Terms) resource namespace.
	  */
	inline const uri dcterms(const std::string &aRef)
	{
		return uri("http://purl.org/dc/terms/", aRef);
	}

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
	typedef std::list<statement> model;

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

		virtual formatter &add_namespace(const std::string &aPrefix, const std::string &aNS) = 0;

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
