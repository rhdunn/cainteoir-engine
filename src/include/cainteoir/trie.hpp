/* Trie Data Structure.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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

#include "buffer.hpp"
#include <list>

namespace cainteoir
{
	template <typename T, typename Key = char>
	struct trie_node
	{
		Key c;
		T item;
		std::list<trie_node<T, Key>> children;

		trie_node(const Key &ch): c(ch), item() {}

		const trie_node<T, Key> *get(const Key &ch) const
		{
			auto first = children.begin(), last = children.end();
			while (first != last)
			{
				if (first->c == ch) return &*first;
				++first;
			}
			return nullptr;
		}

		trie_node<T, Key> *add(const Key &ch)
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

	template <typename T, typename Key = char>
	struct trie
	{
		trie() : mRoot(0) {}

		const trie_node<T, Key> *root() const { return &mRoot; }

		template <typename Container>
		T &insert(const Container &str)
		{
			trie_node<T, Key> *node = &mRoot;
			for (auto && c : str)
				node = node->add(c);
			return node->item;
		}

		template <typename Container>
		void insert(const Container &str, const T &value)
		{
			insert(str) = value;
		}

		template <typename Container>
		const T &lookup(const Container &str, const T &no_match = T()) const
		{
			const trie_node<T, Key> *node = &mRoot;
			for (auto && c : str)
			{
				node = node->get(c);
				if (node == nullptr) return no_match;
			}
			return node->item;
		}
	private:
		trie_node<T, Key> mRoot;
	};
}

#endif
