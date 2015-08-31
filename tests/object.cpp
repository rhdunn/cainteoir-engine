/* Multi-type Object API tests.
 *
 * Copyright (C) 2015 Reece H. Dunn
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

#include "tester.hpp"

#include <cainteoir/object.hpp>
#include <cstring>

REGISTER_TESTSUITE("object");

TEST_CASE("null")
{
	cainteoir::object o;
	assert(o.type() == cainteoir::object_type::null);
	assert(o.integer() == 0);

	cainteoir::object p(o);
	assert(p.type() == cainteoir::object_type::null);
	assert(p.integer() == 0);

	cainteoir::object q(4);
	q = o;
	assert(q.type() == cainteoir::object_type::null);
	assert(q.integer() == 0);
}

TEST_CASE("boolean")
{
	cainteoir::object b(true);
	assert(b.type() == cainteoir::object_type::boolean);
	assert(b.boolean());

	cainteoir::object c(b);
	assert(c.type() == cainteoir::object_type::boolean);
	assert(c.boolean());

	cainteoir::object d;
	d = b;
	assert(d.type() == cainteoir::object_type::boolean);
	assert(d.boolean());
}

TEST_CASE("integer  (8-bit)")
{
	cainteoir::object i((int8_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
}

TEST_CASE("integer (16-bit)")
{
	cainteoir::object i((int16_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
}

TEST_CASE("integer (32-bit)")
{
	cainteoir::object i((int32_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
TEST_CASE("integer (64-bit)")
{
	cainteoir::object i((int64_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
}
#endif

TEST_CASE("real (32-bit)")
{
	cainteoir::object r(2.63f);
	assert(r.type() == cainteoir::object_type::real);
	assert(r.real() == 2.63f);

	cainteoir::object s(r);
	assert(s.type() == cainteoir::object_type::real);
	assert(s.real() == 2.63f);

	cainteoir::object t;
	t = r;
	assert(t.type() == cainteoir::object_type::real);
	assert(t.real() == 2.63f);
}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
TEST_CASE("real (64-bit)")
{
	cainteoir::object r(2.63);
	assert(r.type() == cainteoir::object_type::real);
	assert(r.real() == 2.63);

	cainteoir::object s(r);
	assert(s.type() == cainteoir::object_type::real);
	assert(s.real() == 2.63);

	cainteoir::object t;
	t = r;
	assert(t.type() == cainteoir::object_type::real);
	assert(t.real() == 2.63);
}
#endif

TEST_CASE("string")
{
	cainteoir::object s("hello");
	assert(s.type() == cainteoir::object_type::string);
	assert(strcmp(s.string(), "hello") == 0);

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::string);
	assert(strcmp(t.string(), "hello") == 0);

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::string);
	assert(strcmp(u.string(), "hello") == 0);
}