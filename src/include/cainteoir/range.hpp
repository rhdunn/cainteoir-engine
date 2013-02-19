/* Iterator Range API.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_RANGE_HPP
#define CAINTEOIR_ENGINE_RANGE_HPP

#include <iterator>

namespace cainteoir
{
	template <typename Iterator>
	class range
	{
	protected:
		Iterator first;
		Iterator last;
	public:
		range(Iterator f, Iterator l) : first(f), last(l) {}
		virtual ~range() {}

		typedef Iterator iterator;
		typedef std::reverse_iterator<Iterator> const_reverse_iterator;
		typedef std::size_t size_type;

		iterator begin() const { return first; }
		iterator end()   const { return last; }

		const_reverse_iterator rbegin() const { return const_reverse_iterator(last); }
		const_reverse_iterator rend()   const { return const_reverse_iterator(first); }

		size_type size() const { return last - first; }

		bool empty() const { return first == last; }
	};

	template <typename Container>
	range<typename Container::const_reverse_iterator> reverse(const Container &c)
	{
		return range<typename Container::const_reverse_iterator>(c.rbegin(), c.rend());
	}
}

#endif
