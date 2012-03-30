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
#include <map>

namespace rdf = cainteoir::rdf;

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
		foreach_iter(s, literal.value)
		{
			switch (*s)
			{
			case '"':
				os << "\\\"";
				break;
			default:
				os << *s;
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
			foreach_iter(ns, namespaces)
				if (aGraph.contains(rdf::ns(ns->second, ns->first)))
				{
					os << "@prefix " << ns->second << ": <" << ns->first << "> ." << std::endl;
				}
			os << std::endl;
		}

		foreach_iter(statement, aGraph)
			*this << *statement;

		return *this;
	}
private:
	std::ostream &os;
	format_type format;
	std::map<std::string, std::string> namespaces;
};

std::shared_ptr<rdf::formatter> rdf::create_formatter(std::ostream &aStream, rdf::formatter::format_type aFormatType)
{
	return std::make_shared<n3_formatter>(aStream, aFormatType);
}
