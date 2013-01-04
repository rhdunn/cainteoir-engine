/* RDF formatters.
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

#include <cainteoir/metadata.hpp>
#include <iostream>

namespace rdf = cainteoir::rdf;

#ifndef DOXYGEN

class n3_formatter : public rdf::formatter
{
public:
	n3_formatter(std::ostream &aStream, format_type aFormatType)
		: os(aStream)
		, format(aFormatType)
	{
	}

	rdf::formatter &operator<<(const rdf::ns &aNS)
	{
		if (format == turtle)
			namespaces[aNS.href] = aNS.prefix;
		return *this;
	}

	rdf::formatter &operator<<(const rdf::uri &uri)
	{
		if (uri.ns == "_:")
			os << "_:" << uri.ref;
		else
		{
			std::string &prefix = namespaces[uri.ns];
			if (prefix.empty())
				os << '<' << uri.str() << '>';
			else
				os << prefix << ':' << uri.ref;
		}
		return *this;
	}

	rdf::formatter &operator<<(const rdf::literal &literal)
	{
		os << '"';
		for (auto &s : literal.value)
		{
			switch (s)
			{
			case '"':
				os << "\\\"";
				break;
			default:
				os << s;
				break;
			}
		}
		os << '"';
		if (!literal.language.empty())
			os << '@' << literal.language;
		if (!literal.type.empty())
		{
			os << "^^";
			*this << literal.type;
		}
		return *this;
	}

	rdf::formatter &operator<<(const std::shared_ptr<const rdf::triple> &statement)
	{
		*this << rdf::query::subject(statement);

		os << ' ';
		*this << statement->predicate;
		os << ' ';

		{
			const rdf::literal *literal = dynamic_cast<const rdf::literal *>(statement->object.get());
			if (literal)
				*this << *literal;

			const rdf::uri *uri = dynamic_cast<const rdf::uri *>(statement->object.get());
			if (uri)
				*this << *uri;
		}

		os << " ." << std::endl;

		return *this;
	}

	rdf::formatter &operator<<(const rdf::graph &aGraph)
	{
		if (!namespaces.empty())
		{
			for (auto &ns : namespaces)
			{
				if (aGraph.contains(rdf::ns(ns.second, ns.first)))
				{
					os << "@prefix " << ns.second << ": <" << ns.first << "> ." << std::endl;
				}
			}
			os << std::endl;
		}

		for (auto &statement : aGraph)
			*this << statement;

		return *this;
	}
private:
	std::ostream &os;
	format_type format;
	std::map<std::string, std::string> namespaces;
};

#endif

/// @addtogroup rdf
/// @{

/** @brief Create an RDF serializer.
  *
  * @param[in] aStream     The output stream to serialize the RDF data to.
  * @param[in] aFormatType The format to serialize the RDF data as.
  *
  * @return The formatter object.
  */
std::shared_ptr<rdf::formatter> rdf::create_formatter(std::ostream &aStream, rdf::formatter::format_type aFormatType)
{
	return std::make_shared<n3_formatter>(aStream, aFormatType);
}

/** @struct cainteoir::rdf::formatter
  * @brief  An interface for serializing RDF graphs.
  */

/// @}

/** @fn    cainteoir::rdf::formatter::~formatter
  * @brief Clean up the formatter object.
  */

/** @fn    cainteoir::rdf::formatter &cainteoir::rdf::formatter::operator<<(const cainteoir::rdf::ns &aNS)
  * @brief Serialize a namespace.
  *
  * @param[in] aNS The namespace to serialize.
  *
  * @return The formatter object (to support method chaining).
  */

/** @fn    cainteoir::rdf::formatter &cainteoir::rdf::formatter::operator<<(const cainteoir::rdf::uri &uri)
  * @brief Serialize a URI resource.
  *
  * @param[in] uri The URI resource to serialize.
  *
  * @return The formatter object (to support method chaining).
  */

/** @fn    cainteoir::rdf::formatter &cainteoir::rdf::formatter::operator<<(const cainteoir::rdf::literal &literal)
  * @brief Serialize a literal resource.
  *
  * @param[in] literal The literal resource to serialize.
  *
  * @return The formatter object (to support method chaining).
  */

/** @fn    cainteoir::rdf::formatter &cainteoir::rdf::formatter::operator<<(const std::shared_ptr<const triple> &statement)
  * @brief Serialize an RDF statement.
  *
  * @param[in] statement The statement to serialize.
  *
  * @return The formatter object (to support method chaining).
  */

/** @fn    cainteoir::rdf::formatter &cainteoir::rdf::formatter::operator<<(const cainteoir::rdf::graph &aGraph)
  * @brief Serialize an RDF graph.
  *
  * @param[in] aGraph The RDF graph to serialize.
  *
  * @return The formatter object (to support method chaining).
  */

/** @enum  cainteoir::rdf::formatter::format_type
  * @brief The format used to serialize the RDF statements as.
  * @see   http://www.w3.org/TR/rdf-testcases/#ntriples
  * @see   http://www.w3.org/TeamSubmission/turtle/
  */

/** @var   cainteoir::rdf::formatter::format_type cainteoir::rdf::formatter::ntriple
  * @brief N-Triple serialization format.
  */

/** @var   cainteoir::rdf::formatter::format_type cainteoir::rdf::formatter::turtle
  * @brief Turtle serialization format.
  */
