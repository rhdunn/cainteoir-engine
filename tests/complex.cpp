/* Complex number array API.
 *
 * Copyright (C) 2014 Reece H. Dunn
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

#include <cainteoir/sigproc.hpp>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("complex");

void match_(const cainteoir::complex_array &a, const cainteoir::complex_array &b, const char *file, int line)
{
	assert_location(a.size() == b.size(), file, line);
	for (int i = 0; i < a.size(); ++i)
	{
		assert_location(a[i].re == b[i].re, file, line);
		assert_location(a[i].im == b[i].im, file, line);
	}
}

#define match(a, b) match_(a, b, __FILE__, __LINE__)

TEST_CASE("abs")
{
	cainteoir::complex_array a = {
		{  0.0f,  0.0f },
		{  1.0f,  0.0f },
		{  5.5f,  0.0f },
		{  3.0f,  4.0f },
		{ -3.0f,  4.0f },
		{  3.0f, -4.0f },
		{ -3.0f, -4.0f },
	};

	cainteoir::complex_array b = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 5.5f, 0.0f },
		{ 5.0f, 0.0f },
		{ 5.0f, 0.0f },
		{ 5.0f, 0.0f },
		{ 5.0f, 0.0f },
	};

	cainteoir::abs(a);
	match(a, b);
}

TEST_CASE("abs2")
{
	cainteoir::complex_array a = {
		{  0.0f,  0.0f },
		{  1.0f,  0.0f },
		{  5.5f,  0.0f },
		{  3.0f,  4.0f },
		{ -3.0f,  4.0f },
		{  3.0f, -4.0f },
		{ -3.0f, -4.0f },
	};

	cainteoir::complex_array b = {
		{  0.0f,  0.0f },
		{  1.0f,  0.0f },
		{ 30.25f, 0.0f },
		{ 25.0f,  0.0f },
		{ 25.0f,  0.0f },
		{ 25.0f,  0.0f },
		{ 25.0f,  0.0f },
	};

	cainteoir::abs2(a);
	match(a, b);
}

TEST_CASE("logr")
{
	cainteoir::complex_array a = {
		{  0.0f,  0.0f },
		{  1.0f,  0.0f },
		{  6.0f,  0.0f },
		{  6.0f,  5.0f },
	};

	cainteoir::complex_array b = {
		{  -std::numeric_limits<float>::infinity(), 0.0f },
		{  0.0f, 0.0f },
		{  1.79176f, 0.0f },
		{  1.79176f, 5.0f },
	};

	cainteoir::logr(a);
	match(a, b);
}
