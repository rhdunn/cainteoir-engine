/* MetaData API.
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

#ifndef CAINTEOIR_ENGINE_METADATA_HPP
#define CAINTEOIR_ENGINE_METADATA_HPP

#include <cainteoir/xmlreader.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <list>
#include <set>
#include <map>

namespace cainteoir { namespace rdf
{
	struct resource
	{
		virtual std::shared_ptr<const resource> clone() const = 0;

		virtual ~resource() {}
	};

	class uri : public resource
	{
	public:
		std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		std::string ref;   /**< @brief The URI reference. */

		uri(const std::string &aNS = std::string(), const std::string &aRef = std::string());

		bool empty() const;

		std::string str() const;

		std::shared_ptr<const resource> clone() const;
	};

	inline bool operator==(const uri &a, const uri &b)
	{
		return a.ns == b.ns && a.ref == b.ref;
	}

	inline bool operator!=(const uri &a, const uri &b)
	{
		return !(a == b);
	}

	class ns : public cainteoir::xml::ns
	{
	public:
		ns(const std::string &aPrefix, const std::string &aHref)
			: cainteoir::xml::ns(aPrefix, aHref)
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

	inline bool operator==(const std::string &a, const ns &b)
	{
		return a == b.href;
	}

	inline bool operator==(const ns &a, const std::string &b)
	{
		return a.href == b;
	}

	extern const ns bnode;    /**< @brief RDF blank node. */

	extern const ns rdf;     /**< @brief RDF syntax namespace. */
	extern const ns rdfa;    /**< @brief RDF attributes (RDFa) namespace. */
	extern const ns rdfs;    /**< @brief RDF schema namespace. */
	extern const ns xsd;     /**< @brief XMLSchema namespace. */
	extern const ns xml;     /**< @brief XML namespace. */
	extern const ns owl;     /**< @brief OWL Ontology namespace. */

	extern const ns dc;      /**< @brief Dublin Core: Elements namespace. */
	extern const ns dcterms; /**< @brief Dublin Core: Terms namespace. */
	extern const ns dcam;    /**< @brief DCMI Abstract Model namespace. */

	extern const ns dtb;     /**< @brief Daisy Talking Book (DTB) namespace. */
	extern const ns ncx;     /**< @brief Navigation Control File (NCX) namespace. */

	extern const ns epub;    /**< @brief ePub 3.0 (OPS) namespace. */
	extern const ns ocf;     /**< @brief Open Container Format (OCF) namespace. */
	extern const ns opf;     /**< @brief Open Publication Format (OPF) namespace. */
	extern const ns pkg;     /**< @brief ePub 3.0 package vocabulary namespace. */
	extern const ns media;   /**< @brief ePub 3.0 media overlay vocabulary namespace. */

	extern const ns ssml;    /**< @brief SSML namespace. */
	extern const ns smil;    /**< @brief SMIL namespace. */
	extern const ns xhtml;   /**< @brief XHTML namespace. */

	extern const ns skos;    /**< @brief SKOS namespace. */
	extern const ns foaf;    /**< @brief Friend of a Friend (FOAF) namespace. */
	extern const ns tts;     /**< @brief Cainteoir Text-to-Speech RDF namespace. */

	/** @brief RDF literal node
	  */
	class literal : public resource
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
		std::string value;    /**< @brief The content of the literal string. */
		std::string language; /**< @brief The language the literal string is written in [optional]. */
		uri type;             /**< @brief The type of the literal string [optional]. */

		literal()
			: type(std::string(), std::string())
		{
		}

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

		template<typename T>
		literal & operator=(const T &aValue)
		{
			value = to_string(aValue);
			return *this;
		}

		template<typename T>
		T as() const;

		std::shared_ptr<const resource> clone() const;
	};

	template<typename T>
	inline T literal::as() const
	{
		std::istringstream ss(value);
		T value;
		ss >> value;
		return value;
	}

	template<>
	inline std::string literal::as<std::string>() const
	{
		return value;
	}

	inline bool operator==(const literal &lhs, const literal &rhs)
	{
		return lhs.value == rhs.value && lhs.language == rhs.language && lhs.type == rhs.type;
	}

	/** @brief An RDF statement (triple)
	  */
	class triple
	{
	public:
		const uri subject;
		const uri predicate;
		const std::shared_ptr<const resource> object;

		triple(const uri &aSubject,
		       const uri &aPredicate,
		       const std::shared_ptr<const resource> &aObject)
			: subject(aSubject)
			, predicate(aPredicate)
			, object(aObject)
		{
		}
	};

	namespace query
	{
		namespace detail
		{
			struct subject_t
				: public std::unary_function<const rdf::uri &,
				                             const std::shared_ptr<const rdf::triple> &>
			{
				const rdf::uri &operator()(const std::shared_ptr<const rdf::triple> &statement) const
				{
					return statement->subject;
				}
			};

			struct predicate_t
				: public std::unary_function<const rdf::uri &,
				                             const std::shared_ptr<const rdf::triple> &>
			{
				const rdf::uri &operator()(const std::shared_ptr<const rdf::triple> &statement) const
				{
					return statement->predicate;
				}
			};

			struct object_t
				: public std::unary_function<const rdf::uri &,
				                             const std::shared_ptr<const rdf::triple> &>
			{
				const rdf::uri &operator()(const std::shared_ptr<const rdf::triple> &statement) const
				{
					static const rdf::uri nulluri{ std::string(), std::string() };
					const rdf::uri *uri = dynamic_cast<const rdf::uri *>(statement->object.get());
					return uri ? *uri : nulluri;
				}
			};
		}

		static const detail::subject_t subject;

		static const detail::predicate_t predicate;

		static const detail::object_t object;

		inline const std::string &value(const std::shared_ptr<const rdf::triple> &statement)
		{
			static const std::string nil;
			const rdf::literal *literal = dynamic_cast<const rdf::literal *>(statement->object.get());
			return literal ? literal->value : nil;
		}
	}

	typedef std::list< std::shared_ptr<const triple> >
	        subgraph;

	/** @brief RDF graph
	  */
	class graph : public subgraph , public cainteoir::xml::namespaces
	{
	public:
		graph();

		/** @name Namespaces */
		//@{

		bool contains(const ns &uri) const;

		rdf::graph &set_base(const std::string &aBase);

		rdf::graph &add_namespace(const std::string &aPrefix, const std::string &aHref);

		rdf::graph &add_namespace(const ns &ns)
		{
			return add_namespace(ns.prefix, ns.href);
		}

		rdf::graph &operator<<(const ns &ns)
		{
			return add_namespace(ns.prefix, ns.href);
		}

		/** @brief Add namespaces in an RDFa @prefix attribute.
		  */
		rdf::graph &add_prefix(const std::string &aPrefix);

		//@}
		/** @name URIs */
		//@{

		const uri href(const std::string &aHref);

		const rdf::uri genid();

		std::shared_ptr<const uri>
		curie(const std::string &aCurie);

		//@}
		/** @name Statements */
		//@{

		bool statement(const rdf::uri &aSubject, const rdf::uri &aPredicate, const rdf::uri &aObject);

		bool statement(const rdf::uri &aSubject, const rdf::uri &aPredicate, const rdf::literal &aObject);

		//@}
	private:
		std::string mBaseUri;
		std::set<std::string> namespaces;
		int nextid;
	};

	namespace query
	{
		typedef rdf::subgraph results;

		namespace detail
		{
			template<typename Selector, typename Value>
			class matches_t : public std::unary_function<bool, const std::shared_ptr<const rdf::triple> &>
			{
			public:
				matches_t(const Selector &aSelector, const Value &aValue)
					: selector(aSelector)
					, value(aValue)
				{
				}

				bool operator()(const std::shared_ptr<const rdf::triple> &s) const
				{
					return selector(s) == value;
				}
			private:
				Selector selector;
				const Value &value;
			};
		}

		/** @brief Match statements whos selector matches the value.
		  *
		  * @param aSelector The selector to extract data from the statement.
		  * @param aValue    The value to match against.
		  *
		  * @return The selector functor.
		  */
		template<typename Selector, typename Value>
		detail::matches_t<Selector, Value> matches(const Selector &aSelector, const Value &aValue)
		{
			return detail::matches_t<Selector, Value>(aSelector, aValue);
		}

		namespace detail
		{
			template<typename Selector1, typename Selector2>
			class both_t : public std::unary_function< bool, const std::shared_ptr<const rdf::triple> & >
			{
			public:
				both_t(const Selector1 &s1, const Selector2 &s2)
					: a(s1)
					, b(s2)
				{
				}

				bool operator()(const std::shared_ptr<const rdf::triple> &s) const
				{
					return a(s) && b(s);
				}
			private:
				Selector1 a;
				Selector2 b;
			};
		}

		/** @brief Match statements that match both selectors.
		  *
		  * @param a The first selector to match a statement against.
		  * @param b The second selector to match a statement against.
		  *
		  * @return The selector functor.
		  */
		template<typename Selector1, typename Selector2>
		detail::both_t<Selector1, Selector2> both(const Selector1 &a, const Selector2 &b)
		{
			return detail::both_t<Selector1, Selector2>(a, b);
		}

		namespace detail
		{
			template<typename Selector1, typename Selector2>
			class either_t : public std::unary_function< bool, const std::shared_ptr<const rdf::triple> & >
			{
			public:
				either_t(const Selector1 &s1, const Selector2 &s2)
					: a(s1)
					, b(s2)
				{
				}

				bool operator()(const std::shared_ptr<const rdf::triple> &s) const
				{
					return a(s) || b(s);
				}
			private:
				Selector1 a;
				Selector2 b;
			};
		}

		/** @brief Match statements that match either selector.
		  *
		  * @param a The first selector to match a statement against.
		  * @param b The second selector to match a statement against.
		  *
		  * @return The selector functor.
		  */
		template<typename Selector1, typename Selector2>
		detail::either_t<Selector1, Selector2> either(const Selector1 &a, const Selector2 &b)
		{
			return detail::either_t<Selector1, Selector2>(a, b);
		}

		/** @brief Select statements matching the selector.
		  *
		  * @param metadata The subgraph to select statements from.
		  * @param selector The selector used to choose statements in the graph.
		  *
		  * @return A subgraph containing all matching statements.
		  */
		template<typename Selector>
		inline results select(const rdf::subgraph &metadata, const Selector &selector)
		{
			results ret;
			for (auto &query : metadata)
			{
				if (selector(query))
					ret.push_back(query);
			}
			return ret;
		}

		/** @brief Check if the graph contains any of the specified statements.
		  *
		  * @param metadata The subgraph to check.
		  * @param selector The selector used to choose statements in the graph.
		  *
		  * @retval true  If the graph contains a statement matching the selector.
		  * @retval false If the graph does not contain a statement matching the selector.
		  */
		template<typename Selector>
		inline bool contains(const rdf::subgraph &metadata, const Selector &selector)
		{
			for (auto &query : metadata)
			{
				if (selector(query))
					return true;
			}
			return false;
		}

		namespace detail
		{
			template<typename T>
			struct value_cast
			{
				static T cast(const std::string &value)
				{
					T ret;
					std::istringstream(value) >> ret;
					return ret;
				}
			};

			template<>
			struct value_cast<std::string>
			{
				static const std::string & cast(const std::string &value)
				{
					return value;
				}
			};
		}

		/** @brief Select a value matching the selector.
		  *
		  * @param metadata The subgraph to select statements from.
		  * @param selector The selector used to choose statements in the graph.
		  *
		  * @return The first literal value matching the selector.
		  */
		template<typename T, typename Selector>
		T select_value(const rdf::subgraph &metadata, const Selector &selector)
		{
			for (auto &query : metadata)
			{
				if (selector(query))
					return detail::value_cast<T>::cast(value(query));
			}
			return T();
		}
	}

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
		virtual formatter &operator<<(const uri &uri) = 0;
		virtual formatter &operator<<(const literal &literal) = 0;
		virtual formatter &operator<<(const std::shared_ptr<const triple> &statement) = 0;
		virtual formatter &operator<<(const graph &aGraph) = 0;

		virtual ~formatter() {}
	};

	std::shared_ptr<formatter> create_formatter(std::ostream &aStream, formatter::format_type aFormatType);
}}

#endif
