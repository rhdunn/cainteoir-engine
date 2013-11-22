/* Test for the range API.
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

#include <cainteoir/range.hpp>

#include "tester.hpp"

REGISTER_TESTSUITE("range");

TEST_CASE("iterator range")
{
	cainteoir::range<int *> a(nullptr, nullptr);
	assert(a.size() == 0);
	assert(a.empty());
	assert(a.begin() == a.end());

	int data[] = { 1, 2, 3, 4 };

	cainteoir::range<int *> b(data, data + 4);
	assert(b.size() == 4);
	assert(!b.empty());
	assert(b.begin() != b.end());
}

TEST_CASE("integral range")
{
	cainteoir::range<int> a(0, 0);
	assert(a.size() == 0);
	assert(a.empty());
	assert(a.begin() == a.end());

	cainteoir::range<int> b(10, 14);
	assert(b.size() == 4);
	assert(!b.empty());
	assert(b.begin() != b.end());
}
