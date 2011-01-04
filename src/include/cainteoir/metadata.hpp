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

#define foreach_iter(a, b) for (auto a = b.begin(), last = b.end(); a != last; ++a)

namespace cainteoir { namespace rdf
{
	namespace query
	{
		static const std::string nil;
	}

	/** @brief RDF node
	  */
	struct node
	{
		virtual const node *clone() const = 0;

		virtual ~node() {}
	};

	class any_type
	{
	public:
		explicit any_type(const rdf::node *aValue)
			: value(aValue)
		{
		}

		template<typename T>
		operator const T *() const
		{
			return dynamic_cast<const T *>(value);
		}

		template<typename T>
		bool operator==(const T &rhs) const
		{
			const T * lhs = dynamic_cast<const T *>(value);
			return lhs && *lhs == rhs;
		}
	private:
		const rdf::node *value;
	};

	/** @brief RDF resource
	  */
	struct resource : public node
	{
		virtual const resource *clone() const = 0;
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

	inline bool operator==(const bnode &a, const bnode &b)
	{
		return a.id == b.id;
	}

	inline bool operator!=(const bnode &a, const bnode &b)
	{
		return !(a == b);
	}

	/** @brief RDF URI resource
	  */
	class uri : public resource
	{
		static const std::string normalise_ns(const std::string &aNS)
		{
			if (*(--aNS.end()) == '#')
				return aNS.substr(0, aNS.size()-1);
			return aNS;
		}
	public:
		const std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		const std::string ref;   /**< @brief The URI reference. */
		const char suffix;       /**< @brief The URI suffix type ('#' or '/'). */

		uri(const std::string &aNS, const std::string &aRef)
			: ns(normalise_ns(aNS))
			, ref(aRef)
			, suffix(*(--aNS.end()) == '/' ? '/' : '#')
		{
		}

		bool empty() const
		{
			return ns.empty() && ref.empty();
		}

		std::string str() const
		{
			if (ref.empty())
				return ns;

			if (suffix == '/')
				return ns + ref;
			return ns + "#" + ref;
		}

		const resource *clone() const
		{
			return new uri(*this);
		}
	};

	inline bool operator==(const uri &a, const uri &b)
	{
		return a.ns == b.ns && a.ref == b.ref;
	}

	inline bool operator!=(const uri &a, const uri &b)
	{
		return !(a == b);
	}

	inline const uri href(const std::string &aHref)
	{
		{
			std::string::size_type index = aHref.rfind('#');
			if (index == aHref.size()-1)
				return uri(aHref, std::string());
			else if (index != std::string::npos)
				return uri(aHref.substr(0, index+1), aHref.substr(index+1));
		}
		{
			std::string::size_type index = aHref.rfind('/');
			if (index != std::string::npos && index != aHref.size()-1)
				return uri(aHref.substr(0, index+1), aHref.substr(index+1));
		}
		return uri(aHref, std::string());
	}

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
		return b.href == a;
	}

	inline bool operator!=(const ns &a, const char *b)
	{
		return a.href != b;
	}

	inline bool operator!=(const char *a, const ns &b)
	{
		return b.href != a;
	}

	extern const ns rdf;     /**< @brief RDF syntax namespace. */
	extern const ns rdfs;    /**< @brief RDF schema namespace. */
	extern const ns xsd;     /**< @brief XMLSchema namespace. */
	extern const ns xml;     /**< @brief XML namespace. */
	extern const ns owl;     /**< @brief OWL Ontology namespace. */

	extern const ns dc;      /**< @brief Dublin Core: Elements namespace. */
	extern const ns dcterms; /**< @brief Dublin Core: Terms namespace. */
	extern const ns dcam;    /**< @brief DCMI Abstract Model namespace. */

	extern const ns ocf;     /**< @brief Open Container Format (OCF) namespace. */
	extern const ns opf;     /**< @brief Open Publication Format (OPF) namespace. */
	extern const ns smil;    /**< @brief SMIL namespace. */

	extern const ns skos;    /**< @brief SKOS namespace. */
	extern const ns foaf;    /**< @brief Friend of a Family (FOAF) namespace. */
	extern const ns tts;     /**< @brief Cainteoir Text-to-Speech RDF namespace. */

	/** @brief RDF literal node
	  */
	class literal : public node
	{
		template<typename T>
		static const std::string to_string(const T &value)
		{
			std::ostringstream ss;
			ss << value;
			return ss.str();
		}

		static const std::string &to_string(const std::string &value)
		{
			return value;
		}

		static const char *to_string(const char *value)
		{
			return value;
		}
	public:
		const std::string value;    /**< @brief The content of the literal string. */
		const std::string language; /**< @brief The language the literal string is written in [optional]. */
		const uri type;             /**< @brief The type of the literal string [optional]. */

		template<typename T>
		literal(const T &aValue)
			: value(to_string(aValue))
			, type(std::string(), std::string())
		{
		}

		template<typename T>
		literal(const T &aValue, const std::string &aLanguage)
			: value(to_string(aValue))
			, language(aLanguage)
			, type(std::string(), std::string())
		{
		}

		template<typename T>
		literal(const T &aValue, const uri &aType)
			: value(to_string(aValue))
			, type(aType)
		{
		}

		const node *clone() const
		{
			return new literal(*this);
		}
	};

	namespace query
	{
		inline const std::string &value(const rdf::literal *literal)
		{
			return literal ? literal->value : nil;
		}
	}

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

	namespace query
	{
		inline any_type subject(const rdf::statement &statement)
		{
			return any_type(statement.subject.get());
		}

		inline any_type object(const rdf::statement &statement)
		{
			return any_type(statement.object.get());
		}
	}

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
			id << "genid" << nextid;
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
				const rdf::uri *uri = rdf::query::subject(s);
				if (uri)
					namespaces.insert(uri->ns);
			}
			namespaces.insert(s.predicate.ns);
			{
				const rdf::uri *uri = rdf::query::object(s);
				if (uri)
					namespaces.insert(uri->ns);

				const rdf::literal *literal = rdf::query::object(s);
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
