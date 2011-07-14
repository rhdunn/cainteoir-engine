/* MetaData API.
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

#ifndef CAINTEOIR_ENGINE_METADATA_HPP
#define CAINTEOIR_ENGINE_METADATA_HPP

#include <tr1/memory>
#include <sstream>
#include <string>
#include <list>
#include <set>
#include <map>

#define join_strings2(a, b) a ## b
#define join_strings(a, b) join_strings2(a, b)

#define foreach_iter_impl(a, b, c) \
	const auto &c = b; \
	for (auto a = c.begin(), last = c.end(); a != last; ++a)
#define foreach_iter(a, b) \
	foreach_iter_impl(a, b, join_strings(__c, __LINE__))

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

		bool operator!() const
		{
			return !value;
		}

		template<typename T>
		const T * as() const
		{
			return dynamic_cast<const T *>(value);
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

		explicit bnode(const std::string &aID)
			: id(aID)
		{
		}

		const resource *clone() const;
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
	public:
		std::string ns;    /**< @brief The namespace to which the URI resource belongs. */
		std::string ref;   /**< @brief The URI reference. */

		uri(const std::string &aNS = std::string(), const std::string &aRef = std::string());

		bool empty() const;

		std::string str() const;

		const resource *clone() const;
	};

	inline bool operator==(const uri &a, const uri &b)
	{
		return a.ns == b.ns && a.ref == b.ref;
	}

	inline bool operator!=(const uri &a, const uri &b)
	{
		return !(a == b);
	}

	const uri href(const std::string &aHref);

	/** @brief RDF namespace.
	  */
	class ns
	{
	public:
		std::string href;   /**< @brief The path of the namespace. */
		std::string prefix; /**< @brief The default prefix for the namespace. */

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

	inline bool operator==(const ns &a, const std::string &b)
	{
		return a.href == b;
	}

	template <typename T>
	inline bool operator==(const T & a, const ns &b)
	{
		return b == a;
	}

	template <typename T>
	inline bool operator!=(const ns &a, const T &b)
	{
		return !(a == b);
	}

	template <typename T>
	inline bool operator!=(const T &a, const ns &b)
	{
		return !(b == a);
	}

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

	extern const ns smil;    /**< @brief SMIL namespace. */
	extern const ns xhtml;   /**< @brief XHTML namespace. */

	extern const ns skos;    /**< @brief SKOS namespace. */
	extern const ns foaf;    /**< @brief Friend of a Friend (FOAF) namespace. */
	extern const ns tts;     /**< @brief Cainteoir Text-to-Speech RDF namespace. */

	/** @brief CURIE/XML namespaces.
	  *
	  * Defines a set of namespaces. This has two applications:
	  *    -  xmlns and xml:base URI expansion in XML documents;
	  *    -  curies and prefix data in RDFa data.
	  */
	class namespaces
	{
	public:
		namespaces();

		namespaces &set_base(const std::string &aBase);

		namespaces &add_namespace(const std::string &aPrefix, const std::string &aHref);

		namespaces &add_namespace(const ns &aNS)
		{
			return add_namespace(aNS.prefix, aNS.href);
		}

		namespaces &operator<<(const ns &aNS)
		{
			return add_namespace(aNS.prefix, aNS.href);
		}

		namespaces &add_prefix(const std::string &aPrefix);

		std::tr1::shared_ptr<const resource>
		operator()(const std::string &aCurie) const;
	private:
		std::map<std::string, std::string> mNamespaces;
		std::string mBaseUri;
	};

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

		const node *clone() const;
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

	namespace query
	{
		inline const std::string &value(const rdf::literal *literal)
		{
			return literal ? literal->value : nil;
		}

		inline const std::string &value(const rdf::literal &literal)
		{
			return literal.value;
		}

		inline const std::string &value(const any_type &literal)
		{
			return value(literal.as<rdf::literal>());
		}
	}

	/** @brief An RDF statement (triple)
	  */
	class triple
	{
	public:
		const std::tr1::shared_ptr<const resource> subject;
		const uri predicate;
		const std::tr1::shared_ptr<const node> object;

		triple(const std::tr1::shared_ptr<const resource> &aSubject, const uri &aPredicate, const std::tr1::shared_ptr<const node> &aObject)
			: subject(aSubject)
			, predicate(aPredicate)
			, object(aObject)
		{
		}
	};

	template<typename Resource, typename Object>
	std::tr1::shared_ptr<const triple>
	statement(const Resource &aSubject, const uri &aPredicate, const Object &aObject)
	{
		return std::tr1::shared_ptr<const triple>(new triple(std::tr1::shared_ptr<const resource>(aSubject.clone()),
		                                                     aPredicate,
		                                                     std::tr1::shared_ptr<const node>(aObject.clone())));
	}

	template<typename Resource, typename Object>
	std::tr1::shared_ptr<const triple>
	statement(const Resource &aSubject, const std::tr1::shared_ptr<const resource> &aPredicate, const Object &aObject)
	{
		const uri *predicate = dynamic_cast<const uri *>(aPredicate.get());
		if (!predicate)
			return std::tr1::shared_ptr<const triple>();

		return std::tr1::shared_ptr<const triple>(new triple(std::tr1::shared_ptr<const resource>(aSubject.clone()),
		                                                     *predicate,
		                                                     std::tr1::shared_ptr<const node>(aObject.clone())));
	}

	namespace query
	{
		inline any_type subject(const std::tr1::shared_ptr<const rdf::triple> &statement)
		{
			return any_type(statement->subject.get());
		}

		inline any_type predicate(const std::tr1::shared_ptr<const rdf::triple> &statement)
		{
			return any_type(&statement->predicate);
		}

		inline any_type object(const std::tr1::shared_ptr<const rdf::triple> &statement)
		{
			return any_type(statement->object.get());
		}

		inline const std::string &value(const std::tr1::shared_ptr<const rdf::triple> &statement)
		{
			return value(object(statement));
		}
	}

	typedef std::list< std::tr1::shared_ptr<const triple> >
	        subgraph;

	/** @brief RDF graph
	  */
	class graph : public subgraph
	{
	public:
		graph()
			: nextid(1)
		{
		}

		const rdf::bnode genid();

		bool contains(const ns &uri) const;

		void push_back(const std::tr1::shared_ptr<const triple> &s);
	private:
		std::set<std::string> namespaces;
		int nextid;
	};

	namespace query
	{
		typedef rdf::subgraph results;

		namespace detail
		{
			template<typename Value>
			class matches_t : public std::unary_function< bool, const std::tr1::shared_ptr<const rdf::triple> & >
			{
			public:
				matches_t(any_type (*aSelector)(const std::tr1::shared_ptr<const rdf::triple> &), const Value &aValue)
					: selector(aSelector)
					, value(aValue)
				{
				}

				bool operator()(const std::tr1::shared_ptr<const rdf::triple> &s) const
				{
					return selector(s) == value;
				}
			private:
				any_type (*selector)(const std::tr1::shared_ptr<const rdf::triple> &);
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
		template<typename Value>
		detail::matches_t<Value> matches(any_type (*aSelector)(const std::tr1::shared_ptr<const rdf::triple> &), const Value &aValue)
		{
			return detail::matches_t<Value>(aSelector, aValue);
		}

		namespace detail
		{
			template<typename Selector1, typename Selector2>
			class both_t : public std::unary_function< bool, const std::tr1::shared_ptr<const rdf::triple> & >
			{
			public:
				both_t(const Selector1 &s1, const Selector2 &s2)
					: a(s1)
					, b(s2)
				{
				}

				bool operator()(const std::tr1::shared_ptr<const rdf::triple> &s) const
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
			class either_t : public std::unary_function< bool, const std::tr1::shared_ptr<const rdf::triple> & >
			{
			public:
				either_t(const Selector1 &s1, const Selector2 &s2)
					: a(s1)
					, b(s2)
				{
				}

				bool operator()(const std::tr1::shared_ptr<const rdf::triple> &s) const
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
		  * @param results  The subgraph to add matching statements to.
		  */
		template<typename Selector>
		inline void select(const rdf::subgraph &metadata, const Selector &selector, rdf::subgraph &results)
		{
			foreach_iter(query, metadata)
			{
				if (selector(*query))
					results.push_back(*query);
			}
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
			select(metadata, selector, ret);
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
			foreach_iter(query, metadata)
			{
				if (selector(*query))
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
			foreach_iter(query, metadata)
			{
				if (selector(*query))
					return detail::value_cast<T>::cast(value(*query));
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
		virtual formatter &operator<<(const bnode &bnode) = 0;
		virtual formatter &operator<<(const uri &uri) = 0;
		virtual formatter &operator<<(const literal &literal) = 0;
		virtual formatter &operator<<(const std::tr1::shared_ptr<const triple> &statement) = 0;
		virtual formatter &operator<<(const graph &aGraph) = 0;

		virtual ~formatter() {}
	};

	std::tr1::shared_ptr<formatter> create_formatter(std::ostream &aStream, formatter::format_type aFormatType);
}}

#endif
