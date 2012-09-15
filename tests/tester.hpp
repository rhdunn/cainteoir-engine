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

namespace tester
{
	template <typename T> struct is_printable { static const bool value = false; };

	template <typename T>
	struct value_t
	{
		const T &value;

		value_t(const T &v) : value(v)
		{
		}

		const T &operator()() const { return value; }
	};

	template <typename T> struct is_printable<value_t<T>>
	{
		static const bool value = is_printable<T>::value;
	};

	template <typename T>
	std::ostream &operator<<(std::ostream &os, const value_t<T> &value)
	{
		return os << value.value;
	}

	template <>
	std::ostream &operator<<(std::ostream &os, const value_t<bool> &value)
	{
		return os << (value.value ? "true" : "false");
	}

	template <>
	std::ostream &operator<<(std::ostream &os, const value_t<char> &value)
	{
		return os << "'" << value.value << "'";
	}

	template <>
	std::ostream &operator<<(std::ostream &os, const value_t<const char *> &value)
	{
		return os << '"' << value.value << '"';
	}

	template <>
	std::ostream &operator<<(std::ostream &os, const value_t<std::string> &value)
	{
		return os << '"' << value.value << '"';
	}

	template <>
	std::ostream &operator<<(std::ostream &os, const value_t<unsigned char> &value)
	{
		os << "\\x" << std::hex << (int)value.value << std::dec;
		if (value.value >= 0x20 && value.value < 0x80)
			os << ' ' << '(' << (char)value.value << ')';
		return os;
	}

#	define BINARY_OP(op,name) \
		template <typename T1, typename T2> \
		struct name##_t \
		{ \
			const T1 lhs; \
			const T2 rhs; \
			name##_t(const T1 &a, const T2 &b) : lhs(a) , rhs(b) {} \
			bool operator()() const { return lhs() op rhs(); } \
		}; \
		\
		template <typename T1, typename T2> struct is_printable<name##_t<T1, T2>> \
		{ \
			static const bool value = is_printable<T1>::value && is_printable<T2>::value; \
		}; \
		\
		template <typename T1, typename T2> \
		std::ostream &operator<<(std::ostream &os, const name##_t<T1, T2> &e) \
		{ \
			return os << e.lhs << ' ' << #op << ' ' << e.rhs; \
		} \
		\
		template <typename T1, typename T2> \
		name##_t<value_t<T1>, value_t<T2>> operator op(const value_t<T1> &a, const T2 &b) \
		{ \
			return name##_t<value_t<T1>, value_t<T2>>(a, value_t<T2>(b)); \
		}

	BINARY_OP(==,equal)
	BINARY_OP(!=,not_equal)

#	undef BINARY_OP

	struct capture
	{
		template <typename T>
		value_t<T> operator->*(const T &c)
		{
			return value_t<T>(c);
		}
	};

	template <bool printable>
	struct expression_failed
	{
		template <typename E>
		static void print(const char *expr, const E &e)
		{
			std::cout << "expression `" << expr << "` failed with `" << e << "`";
		}
	};

	template <>
	struct expression_failed<false>
	{
		template <typename E>
		static void print(const char *expr, const E &e)
		{
			std::cout << "expression `" << expr << "` failed";
		}
	};
}

#define CAPTURE(expr) tester::capture() ->* expr

int passed;
int failed;

template <typename E>
bool assert_(E expression, const char *ref, bool expected, const char *file, int lineno)
{
	if (expression() ? expected : !expected)
	{
		++passed;
		return true;
	}

	std::cout << file << "@" << lineno << " : ";
	tester::expression_failed<tester::is_printable<E>::value>::print(ref, expression);
	std::cout << std::endl;
	++failed;
	return false;
}

#undef  assert
#define assert(e)       assert_(CAPTURE(e), "assert(" #e ")", true, __FILE__, __LINE__)
#define assert_false(e) assert_(CAPTURE(e), "assert_false(" #e ")", false, __FILE__, __LINE__)

#define assert_location(e, file, line) assert_(CAPTURE(e), "assert(" #e ")", true, file, line)

#define PRINTABLE(type) \
	namespace tester \
	{ \
		template <> struct is_printable<type> \
		{ \
			static const bool value = true; \
		}; \
	}

PRINTABLE(std::string);
PRINTABLE(const char *);
PRINTABLE(char);
PRINTABLE(unsigned char);

PRINTABLE(bool);

PRINTABLE(short);
PRINTABLE(unsigned short);

PRINTABLE(int);
PRINTABLE(unsigned int);

PRINTABLE(long);
PRINTABLE(unsigned long);

PRINTABLE(long long);
PRINTABLE(unsigned long long);

PRINTABLE(float);
PRINTABLE(double);
PRINTABLE(long double);

// test cases //////////////////////////////////////////////////////////////////

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
