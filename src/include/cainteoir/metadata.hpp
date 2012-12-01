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
#include <sstream>
#include <set>

namespace cainteoir { namespace rdf
{
	struct resource
	{
		virtual std::shared_ptr<const resource> clone() const = 0;

		virtual ~resource() {}
	};

	struct uri : public resource
	{
		std::string ns;
		std::string ref;

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

	struct ns : public cainteoir::xml::ns
	{
		ns(const std::string &aPrefix, const std::string &aHref)
			: cainteoir::xml::ns(aPrefix, aHref)
		{
		}

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

	extern const ns bnode;
	extern const ns rdf;
	extern const ns rdfa;
	extern const ns rdfs;
	extern const ns xsd;
	extern const ns xml;
	extern const ns owl;
	extern const ns dc;
	extern const ns dcterms;
	extern const ns dcam;
	extern const ns dtb;
	extern const ns ncx;
	extern const ns epub;
	extern const ns ocf;
	extern const ns opf;
	extern const ns pkg;
	extern const ns media;
	extern const ns onix;
	extern const ns marc;
	extern const ns ssml;
	extern const ns smil;
	extern const ns xhtml;
	extern const ns skos;
	extern const ns foaf;
	extern const ns tts;
	extern const ns iana;
	extern const ns subtag;

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
		std::string value;
		std::string language;
		uri type;

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

	/// @private
	template<>
	inline std::string literal::as<std::string>() const
	{
		return value;
	}

	inline bool operator==(const literal &lhs, const literal &rhs)
	{
		return lhs.value == rhs.value && lhs.language == rhs.language && lhs.type == rhs.type;
	}

	struct triple
	{
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
#ifndef DOXYGEN
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

				subject_t() {}
			};

			struct predicate_t
				: public std::unary_function<const rdf::uri &,
				                             const std::shared_ptr<const rdf::triple> &>
			{
				const rdf::uri &operator()(const std::shared_ptr<const rdf::triple> &statement) const
				{
					return statement->predicate;
				}

				predicate_t() {}
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

				object_t() {}
			};
		}
#endif

		/** @name Selectors
		  */
		//@{

		extern const detail::subject_t subject;

		extern const detail::predicate_t predicate;

		extern const detail::object_t object;

		inline const std::string &value(const std::shared_ptr<const rdf::triple> &statement)
		{
			static const std::string nil;
			const rdf::literal *literal = dynamic_cast<const rdf::literal *>(statement->object.get());
			return literal ? literal->value : nil;
		}

		//@}
	}

	namespace query
	{
		typedef std::list<std::shared_ptr<const triple>> results;
	}

	struct triplestore : public query::results
	{
		void push_back(const_reference item)
		{
			query::results::push_back(item);
			subjects[query::subject(item).str()].push_back(item);
		}

		const query::results &subject(const rdf::uri &s) const
		{
			static const query::results empty;
			auto ret = subjects.find(s.str());
			if (ret == subjects.end())
				return empty;
			return (*ret).second;
		}
	private:
		std::map<std::string, query::results> subjects;
	};

	struct graph : public cainteoir::xml::namespaces
	{
		typedef triplestore::size_type size_type;
		typedef triplestore::const_iterator const_iterator;
		typedef triplestore::const_reference const_reference;

		graph();

		size_type size()  const { return triples.size(); }
		bool      empty() const { return triples.empty(); }

		const_iterator begin() const { return triples.begin(); }
		const_iterator end()   const { return triples.end(); }

		const_reference front() const { return triples.front(); }
		const_reference back()  const { return triples.back(); }

		const query::results &subject(const rdf::uri &s) const
		{
			return triples.subject(s);
		}

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
		triplestore triples;
	};

	namespace query
	{
		/** @name Selectors
		  */
		//@{

#ifndef DOXYGEN
		namespace detail
		{
			template<typename Selector, typename Value>
			struct matches_t
				: public std::unary_function<bool, const std::shared_ptr<const rdf::triple> &>
			{
				matches_t(const Selector &aSelector, const Value &aValue)
					: selector(aSelector)
					, value(aValue)
				{
				}

				bool operator()(const std::shared_ptr<const rdf::triple> &s) const
				{
					return selector(s) == value;
				}

				Selector selector;
				const Value &value;
			};
		}
#endif

		template<typename Selector, typename Value>
		detail::matches_t<Selector, Value> matches(const Selector &aSelector, const Value &aValue)
		{
			return detail::matches_t<Selector, Value>(aSelector, aValue);
		}

#ifndef DOXYGEN
		namespace detail
		{
			template<typename Selector1, typename Selector2>
			struct both_t
				: public std::unary_function< bool, const std::shared_ptr<const rdf::triple> & >
			{
				both_t(const Selector1 &s1, const Selector2 &s2)
					: a(s1)
					, b(s2)
				{
				}

				bool operator()(const std::shared_ptr<const rdf::triple> &s) const
				{
					return a(s) && b(s);
				}

				Selector1 a;
				Selector2 b;
			};
		}
#endif

		template<typename Selector1, typename Selector2>
		detail::both_t<Selector1, Selector2> both(const Selector1 &a, const Selector2 &b)
		{
			return detail::both_t<Selector1, Selector2>(a, b);
		}

		//@}

		template<typename TripleStore, typename Selector>
		inline results select(const TripleStore &metadata, const Selector &selector)
		{
			results ret;
			for (auto &query : metadata)
			{
				if (selector(query))
					ret.push_back(query);
			}
			return ret;
		}

		// Optimize selecting all triples for a given statement ...
		/// @private
		inline results select(const graph &metadata,
		                      const query::detail::matches_t<query::detail::subject_t, const rdf::uri &> &m)
		{
			return metadata.subject(m.value);
		}

		template<typename TripleStore, typename Selector>
		inline bool contains(const TripleStore &metadata, const Selector &selector)
		{
			for (auto &query : metadata)
			{
				if (selector(query))
					return true;
			}
			return false;
		}

#ifndef DOXYGEN
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
#endif

		/** @brief Select a value matching the selector.
		  *
		  * @param metadata The subgraph to select statements from.
		  * @param selector The selector used to choose statements in the graph.
		  *
		  * @return The first literal value matching the selector.
		  */
		template<typename T, typename TripleStore, typename Selector>
		T select_value(const TripleStore &metadata, const Selector &selector)
		{
			for (auto &query : metadata)
			{
				if (selector(query))
					return detail::value_cast<T>::cast(value(query));
			}
			return T();
		}
	}

#ifndef DOXYGEN
	inline query::detail::matches_t<query::detail::subject_t, const rdf::uri &>
	operator==(const query::detail::subject_t &selector, const rdf::uri &value)
	{
		return query::detail::matches_t<query::detail::subject_t, const rdf::uri &>(selector, value);
	}

	inline query::detail::matches_t<query::detail::predicate_t, const rdf::uri &>
	operator==(const query::detail::predicate_t &selector, const rdf::uri &value)
	{
		return query::detail::matches_t<query::detail::predicate_t, const rdf::uri &>(selector, value);
	}

	inline query::detail::matches_t<query::detail::object_t, const rdf::uri &>
	operator==(const query::detail::object_t &selector, const rdf::uri &value)
	{
		return query::detail::matches_t<query::detail::object_t, const rdf::uri &>(selector, value);
	}

	template<typename Selector1, typename Selector2>
	inline query::detail::both_t<query::detail::matches_t<Selector1, const rdf::uri &>,
	                             query::detail::matches_t<Selector2, const rdf::uri &>>
	operator&&(const query::detail::matches_t<Selector1, const rdf::uri &> &a,
	           const query::detail::matches_t<Selector2, const rdf::uri &> &b)
	{
		return query::detail::both_t<query::detail::matches_t<Selector1, const rdf::uri &>,
		                             query::detail::matches_t<Selector2, const rdf::uri &>>(a, b);
	}
#endif

	struct formatter
	{
		enum format_type
		{
			ntriple,
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
