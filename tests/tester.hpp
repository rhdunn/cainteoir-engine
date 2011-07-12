/* Tester -- C++ Test Logic
 *
 * Copyright (C) 2011 Reece H. Dunn
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

#ifndef TESTER_HPP
#define TESTER_HPP

#include <iostream>
#include <cassert>

int passed;
int failed;

bool assert_(bool cond, const char *fn, const char *ref, int lineno)
{
	if (cond)
	{
		++passed;
		return true;
	}

	std::cout << fn << " @ line " << lineno << " : " << ref << " -- assertion failure" << std::endl;
	++failed;
	return false;
}

#undef  assert
#define assert(cond) assert_(cond, __FUNCTION__, #cond, __LINE__)

template<typename T1, typename T2>
void equal_(const T1 &a, const T2 &b, const char *fn, const char *ref, int lineno)
{
	if (!assert_(a == b, fn, ref, lineno))
	{
		std::cout << "    expected: " << b << std::endl
		          << "    actual:   " << a << std::endl
		          ;
	}
}

#define equal(a, b) equal_(a, b, __FUNCTION__, #a " == " #b, __LINE__)

typedef void (*test_function)();

struct test_case;

test_case *test_case_first = NULL;
test_case *test_case_last = NULL;

struct test_case
{
	const char *name;
	test_function test;
	test_case *next;

	test_case(const char *aName, test_function aTest) : name(aName), test(aTest)
	{
		if (!test_case_first)
			test_case_first = test_case_last = this;

		test_case_last->next = this;
		test_case_last = this;

		next = NULL;
	}
};

#define TEST_CASE_IMPL(name, line) \
	void test_fn##line(); \
	static const test_case testcase_data##line (name, test_fn##line); \
	void test_fn##line()
#define TEST_CASE_(name, line) TEST_CASE_IMPL(name, line)

#define TEST_CASE(name) TEST_CASE_(name, __LINE__)

extern const char *testsuite_name;

#define REGISTER_TESTSUITE(name) \
	const char *testsuite_name = name;

int main(int argc, const char ** argv)
{
	for (test_case *test = test_case_first; test; test = test->next)
	{
		std::cout << "... testing " << test->name << std::endl;
		(*test->test)();
	}

	printf("\n========== summary of the %s test results ==========\n", testsuite_name);
	printf(" %5d passed\n", passed);
	printf(" %5d failed\n", failed);
	printf(" %5d total\n\n",  (passed + failed));

	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif
