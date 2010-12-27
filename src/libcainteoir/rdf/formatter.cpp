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

class ntriple_formatter : public cainteoir::rdf::formatter
{
private:
	std::ostream &os;
public:
	ntriple_formatter(std::ostream &aStream)
		: os(aStream)
	{
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::uri &uri)
	{
		os << '<' << uri.value << '>';
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
		*this << statement.subject;
		os << ' ';
		*this << statement.predicate;
		os << ' ';

		const cainteoir::rdf::literal *literal = dynamic_cast<const cainteoir::rdf::literal *>(statement.object.get());
		if (literal)
			*this << *literal;
		else
			*this << *dynamic_cast<const cainteoir::rdf::uri *>(statement.object.get());

		os << " ." << std::endl;

		return *this;
	}

	cainteoir::rdf::formatter &operator<<(const cainteoir::rdf::model &model)
	{
		for (cainteoir::rdf::model::const_iterator statement = model.begin(), last = model.end(); statement != last; ++statement)
			*this << *statement;
		return *this;
	}
};

std::tr1::shared_ptr<cainteoir::rdf::formatter> cainteoir::rdf::ntriple_formatter(std::ostream &os)
{
	return std::tr1::shared_ptr<cainteoir::rdf::formatter>(new ::ntriple_formatter(os));
}
