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

PRINTABLE(cainteoir::overlap_status);

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

#define TEST_RANGE(IntegralType) \
	TEST_CASE("integral range - " # IntegralType) \
	{ \
		cainteoir::range<IntegralType> a(0, 0); \
		assert(a.size() == 0); \
		assert(a.empty()); \
		assert(a.begin() == a.end()); \
		\
		cainteoir::range<IntegralType> b(10, 14); \
		assert(b.size() == 4); \
		assert(!b.empty()); \
		assert(b.begin() != b.end()); \
	}

TEST_RANGE(  signed short)
TEST_RANGE(unsigned short)
TEST_RANGE(  signed int)
TEST_RANGE(unsigned int)
TEST_RANGE(  signed long)
TEST_RANGE(unsigned long)
TEST_RANGE(  signed long long)
TEST_RANGE(unsigned long long)

TEST_RANGE( int16_t)
TEST_RANGE(uint16_t)
TEST_RANGE( int32_t)
TEST_RANGE(uint32_t)
TEST_RANGE( int64_t)
TEST_RANGE(uint64_t)

#undef TEST_RANGE

TEST_CASE("contains - iterator")
{
	cainteoir::range<int32_t> a(5, 10);
	assert(!a.contains(4));
	assert(a.contains(5));
	assert(a.contains(9));
	assert(!a.contains(10));
}

TEST_CASE("contains - range")
{
	/* The following diagram illustrates the different test cases to show
	 * how they relate to each other and cover the different permutations
	 * and edge cases:
	 *
	 *    0  5  10 15 20 25 30 35 40 45 50 +----+-----------------------+
	 *                |========|           |  a | range to test against |
	 *                :        :           +----+---------------------- +
	 *                |========|           |  1 | overlap_inner         |
	 *                |=====|  :           |  2 | overlap_inner         |
	 *                :  |=====|           |  3 | overlap_inner         |
	 *                :  |==|  :           |  4 | overlap_inner         |
	 *          |==|  :        :           |  5 | no_overlap            |
	 *          |=====|        :           |  6 | no_overlap            |
	 *          |========|     :           |  7 | overlap_at_start      |
	 *                :        :  |==|     |  8 | no_overlap            |
	 *                :        |=====|     |  9 | no_overlap            |
	 *                :     |========|     | 10 | overlap_at_end        |
	 *             |==============|        | 11 | overlap_outer         |
	 *    0  5  10 15 20 25 30 35 40 45 50 +----+-----------------------+
	 */

	typedef cainteoir::range<int32_t> range;
	range a(20, 35);

	assert(a.contains(range(20, 35)) == cainteoir::overlap_inner);    //  1
	assert(a.contains(range(20, 30)) == cainteoir::overlap_inner);    //  2
	assert(a.contains(range(25, 35)) == cainteoir::overlap_inner);    //  3
	assert(a.contains(range(25, 30)) == cainteoir::overlap_inner);    //  4
	assert(a.contains(range(10, 15)) == cainteoir::no_overlap);       //  5
	assert(a.contains(range(10, 20)) == cainteoir::no_overlap);       //  6
	assert(a.contains(range(10, 25)) == cainteoir::overlap_at_start); //  7
	assert(a.contains(range(40, 45)) == cainteoir::no_overlap);       //  8
	assert(a.contains(range(35, 45)) == cainteoir::no_overlap);       //  9
	assert(a.contains(range(30, 45)) == cainteoir::overlap_at_end);   // 10
	assert(a.contains(range(15, 40)) == cainteoir::overlap_outer);    // 11
}
