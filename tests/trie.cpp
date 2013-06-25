/* Test for the trie data structure.
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

#include <cainteoir/trie.hpp>
#include <cainteoir/buffer.hpp>

#include "tester.hpp"

REGISTER_TESTSUITE("trie");

template <typename T>
void add_entry(cainteoir::trie_node<T> *node, const cainteoir::buffer str, T value)
{
	for (char c : str)
		node = node->add(c);
	node->item = value;
}

template <typename T>
T get_entry(const cainteoir::trie_node<T> *node, const cainteoir::buffer str)
{
	for (char c : str)
	{
		node = node->get(c);
		assert(node != nullptr);
		assert(node->c == c);
	}
	return node->item;
}

TEST_CASE("trie node: root")
{
	cainteoir::trie_node<int> n('\0');
	assert(n.c == '\0');
	assert(n.item == 0);
	assert(n.children.empty());
}

TEST_CASE("trie node: insert node")
{
	cainteoir::trie_node<int> n('\0');

	assert(n.get('d') == nullptr);
	assert(n.children.size() == 0);

	assert(n.add('d') != nullptr);
	assert(n.children.size() == 1);
	assert(n.add('d') == n.add('d'));
	assert(n.children.size() == 1);
	assert(n.get('d') == n.add('d'));
	assert(n.children.size() == 1);

	assert(n.add('e') != nullptr);
	assert(n.get('d') != n.get('e'));

	assert(n.get('d')->c == 'd');
	assert(n.get('e')->c == 'e');
}

TEST_CASE("trie node: string insertion")
{
	static const std::initializer_list<std::pair<cainteoir::buffer, int>> words =
	{
		{ "test", 1 },
		{ "tear", 2 },
		{ "tome", 3 },
		{ "boast", 4 },
		{ "view", 5 },
		{ "even", 6 },
		{ "viewing", 7 },
	};

	cainteoir::trie_node<int> n('\0');
	for (const auto &word : words)
		add_entry(&n, word.first, word.second);

	for (const auto &word : words)
		assert(get_entry(&n, word.first) == word.second);
}
