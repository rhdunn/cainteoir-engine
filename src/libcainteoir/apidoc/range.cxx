/* Iterator Range API Documentation.
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

/** @class cainteoir::range
  * @brief A sequence of items between two iterators.
  */

/** @var   Iterator cainteoir::range<Iterator>::first
  * @brief The first item in the range.
  */

/** @var   Iterator cainteoir::range<Iterator>::last
  * @brief One past the last item in the range.
  */

/** @typedef Iterator cainteoir::range<Iterator>::iterator
  * @brief   The iterator type for this range object.
  */

/** @typedef std::size_t cainteoir::range<Iterator>::size_type
  * @brief   The type used to represent sizes.
  */

/** @fn    cainteoir::range<Iterator>::range(Iterator f, Iterator l)
  * @brief Create a new range object.
  *
  * @param[in] f The first item in the range.
  * @param[in] l One past the last item in the range.
  */

/** @fn    cainteoir::range<Iterator>::~range()
  * @brief Clean up the range object.
  */

/** @fn    cainteoir::range<Iterator>::iterator cainteoir::range<Iterator>::begin() const
  * @brief Get the beginning of the range.
  *
  * @return An iterator to the beginning of the range.
  */

/** @fn    cainteoir::range<Iterator>::iterator cainteoir::range<Iterator>::end() const
  * @brief Get the end of the range.
  *
  * @return An iterator to one past the end of the range.
  */

/** @fn    cainteoir::range<Iterator>::size_type cainteoir::range<Iterator>::size() const
  * @brief Get the number of items in the range.
  *
  * @return The number of items in the range.
  */

/** @fn    bool cainteoir::range<Iterator>::empty() const
  * @brief Is the range empty?
  *
  * @retval true  If the range is empty.
  * @retval false If the range is not empty.
  */

/** @fn    cainteoir::range<typename Container::const_reverse_iterator> cainteoir::reverse(const Container &c)
  * @brief Traverse the container in reverse order.
  *
  * @param[in] c The container to traverse in reverse order.
  *
  * @return A range object for traversing the container in reverse order.
  *
  * This is useful in conjunction with the C++11 range-based for loop, for example:
  *
  * @code
  *     std::vector<int> v = { 1, 2, 3, 4 };
  *     for (auto &item : cainteoir::reverse(v))
  *         std::cout << item << std::endl;
  * @endcode
  */
