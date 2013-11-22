/* Iterator Range API.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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
	namespace detail
	{
		// Helper class for making integral types iterable.
		// NOTE: This cannot be random-access because operator[] does not work.
		template <typename T>
		struct integral_iterator
		{
			typedef std::ptrdiff_t difference_type;
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef T value_type;
			typedef T pointer;
			typedef T reference;

			integral_iterator(T aValue) : value(aValue) {}
			integral_iterator(const integral_iterator<T> &other) : value(other.value) {}

			// Access to the underlying integral value to support using the class as
			// a simple value type.
			operator T() const { return value; }

			reference operator*()  const { return value; }
			pointer   operator->() const { return value; }

			integral_iterator<T> &operator=(const integral_iterator<T> &other)
			{
				value = other.value;
				return *this;
			}

			integral_iterator<T> &operator++()
			{
				++value;
				return *this;
			}

			integral_iterator<T> operator++(int)
			{
				auto ret = *this;
				++ret.value;
				return ret;
			}

			integral_iterator<T> &operator--()
			{
				--value;
				return *this;
			}

			integral_iterator<T> operator--(int)
			{
				auto ret = *this;
				--ret.value;
				return ret;
			}

			bool operator==(const integral_iterator<T> &other) const { return value == other.value; }
			bool operator!=(const integral_iterator<T> &other) const { return value != other.value; }
		private:
			T value;
		};

#define		MAKE_INTEGRAL_ITERATOR(IntegralType) \
			template <> struct iterator_type<IntegralType> \
			{ \
				typedef integral_iterator<IntegralType> type; \
			};

		template <typename Iterator> struct iterator_type { typedef Iterator type; };
		MAKE_INTEGRAL_ITERATOR(  signed short)
		MAKE_INTEGRAL_ITERATOR(unsigned short)
		MAKE_INTEGRAL_ITERATOR(  signed int)
		MAKE_INTEGRAL_ITERATOR(unsigned int)
		MAKE_INTEGRAL_ITERATOR(  signed long)
		MAKE_INTEGRAL_ITERATOR(unsigned long)
		MAKE_INTEGRAL_ITERATOR(  signed long long)
		MAKE_INTEGRAL_ITERATOR(unsigned long long)

#undef		MAKE_INTEGRAL_ITERATOR
	}

	template <typename Iterator>
	class range
	{
	public:
		typedef typename detail::iterator_type<Iterator>::type iterator;
		typedef std::reverse_iterator<iterator> const_reverse_iterator;
		typedef std::size_t size_type;

		range(iterator f, iterator l) : first(f), last(l) {}
		virtual ~range() {}

		iterator begin() const { return first; }
		iterator end()   const { return last; }

		const_reverse_iterator rbegin() const { return const_reverse_iterator(last); }
		const_reverse_iterator rend()   const { return const_reverse_iterator(first); }

		size_type size() const { return std::distance(first, last); }

		bool empty() const { return first == last; }
	protected:
		iterator first;
		iterator last;
	};

	template <typename Container>
	range<typename Container::const_reverse_iterator> reverse(const Container &c)
	{
		return range<typename Container::const_reverse_iterator>(c.rbegin(), c.rend());
	}
}

#endif
