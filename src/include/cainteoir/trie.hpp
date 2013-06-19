/* Trie Data Structure.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_TRIE_HPP
#define CAINTEOIR_ENGINE_TRIE_HPP

#include <list>

namespace cainteoir
{
	template <typename T>
	struct trie_node
	{
		char c;
		T item;
		std::list<trie_node<T>> children;

		trie_node(char ch): c(ch), item() {}

		const trie_node<T> *get(char ch) const
		{
			auto first = children.begin(), last = children.end();
			while (first != last)
			{
				if (first->c == ch) return &*first;
				++first;
			}
			return nullptr;
		}

		trie_node<T> *add(char ch)
		{
			auto first = children.begin(), last = children.end();
			while (first != last)
			{
				if (first->c == ch) return &*first;
				if (first->c > ch)
					break;
				++first;
			}

			first = children.insert(first, ch);
			return &*first;
		}
	};
}

#endif
