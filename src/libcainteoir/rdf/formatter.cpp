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

class n3_formatter : public cainteoir::rdf::formatter
{
public:
	n3_formatter(std::ostream &aStream, format_type aFormatType)
		: os(aStream)
		, format(aFormatType)
	{
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::ns &aNS)
	{
		if (format == turtle)
			namespaces[aNS.href] = aNS.prefix;
		return *this;
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::bnode &bnode)
	{
		os << "_:" << bnode.id;
		return *this;
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::uri &uri)
	{
		std::string &prefix = namespaces[uri.ns];
		if (prefix.empty())
			os << '<' << uri.value << '>';
		else
			os << prefix << ':' << uri.ref;
		return *this;
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::literal &literal)
	{
		os << '"' << literal.value << '"';
		if (!literal.language.empty())
			os << '@' << literal.language;
		if (!literal.type.value.empty())
		{
			os << "^^";
			*this << literal.type;
		}
		return *this;
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::statement &statement)
	{
		{
			const cainteoir::rdf::bnode *bnode = dynamic_cast<const cainteoir::rdf::bnode *>(statement.subject.get());
			if (bnode)
				*this << *bnode;

			const cainteoir::rdf::uri *uri = dynamic_cast<const cainteoir::rdf::uri *>(statement.subject.get());
			if (uri)
				*this << *uri;
		}

		os << ' ';
		*this << statement.predicate;
		os << ' ';

		{
			const cainteoir::rdf::bnode *bnode = dynamic_cast<const cainteoir::rdf::bnode *>(statement.object.get());
			if (bnode)
				*this << *bnode;

			const cainteoir::rdf::literal *literal = dynamic_cast<const cainteoir::rdf::literal *>(statement.object.get());
			if (literal)
				*this << *literal;

			const cainteoir::rdf::uri *uri = dynamic_cast<const cainteoir::rdf::uri *>(statement.object.get());
			if (uri)
				*this << *uri;
		}

		os << " ." << std::endl;

		return *this;
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::model &model)
	{
		if (!namespaces.empty())
		{
			for (std::map<std::string, std::string>::const_iterator ns = namespaces.begin(), last = namespaces.end(); ns != last; ++ns)
				if (model.contains(cainteoir::rdf::ns(ns->second, ns->first)))
					os << "@prefix " << ns->second << ": <" << ns->first << "> ." << std::endl;
			os << std::endl;
		}

		for (cainteoir::rdf::model::const_iterator statement = model.begin(), last = model.end(); statement != last; ++statement)
			*this << *statement;

		return *this;
	}
private:
	std::ostream &os;
	format_type format;
	std::map<std::string, std::string> namespaces;
};

std::tr1::shared_ptr<cainteoir::rdf::formatter> cainteoir::rdf::create_formatter(std::ostream &aStream, cainteoir::rdf::formatter::format_type aFormatType)
{
	return std::tr1::shared_ptr<cainteoir::rdf::formatter>(new ::n3_formatter(aStream, aFormatType));
}
