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

namespace ipa = cainteoir::ipa;

REGISTER_TESTSUITE("object");

TEST_CASE("null")
{
	cainteoir::object o;
	assert(o.type() == cainteoir::object_type::null);
	assert(o.integer() == 0);

	assert(o.is_null());
	assert(!o.is_boolean());
	assert(!o.is_integer());
	assert(!o.is_real());
	assert(!o.is_string());
	assert(!o.is_buffer());
	assert(!o.is_phoneme());
	assert(!o.is_range());
	assert(!o.is_dictionary());

	cainteoir::object p(o);
	assert(p.type() == cainteoir::object_type::null);
	assert(p.integer() == 0);

	cainteoir::object q(4);
	q = o;
	assert(q.type() == cainteoir::object_type::null);
	assert(q.integer() == 0);

	cainteoir::object r(cainteoir::object_type::null);
	assert(r.type() == cainteoir::object_type::null);
	assert(r.integer() == 0);

	assert(o.get("test") == nullptr);
	assert(!o.put("test", 24));
	assert(o.get("test") == nullptr);
}

TEST_CASE("boolean")
{
	cainteoir::object b(true);
	assert(b.type() == cainteoir::object_type::boolean);
	assert(b.boolean());

	assert(!b.is_null());
	assert(b.is_boolean());
	assert(!b.is_integer());
	assert(!b.is_real());
	assert(!b.is_string());
	assert(!b.is_buffer());
	assert(!b.is_phoneme());
	assert(!b.is_range());
	assert(!b.is_dictionary());

	cainteoir::object c(b);
	assert(c.type() == cainteoir::object_type::boolean);
	assert(c.boolean());

	cainteoir::object d;
	d = b;
	assert(d.type() == cainteoir::object_type::boolean);
	assert(d.boolean());

	cainteoir::object o(cainteoir::object_type::boolean);
	assert(o.type() == cainteoir::object_type::boolean);
	assert(!o.boolean());

	assert(b.get("test") == nullptr);
	assert(!b.put("test", 24));
	assert(b.get("test") == nullptr);
}

TEST_CASE("integer  (8-bit)")
{
	cainteoir::object i((int8_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);

	cainteoir::object o(cainteoir::object_type::integer);
	assert(o.type() == cainteoir::object_type::integer);
	assert(o.integer() == 0);

	assert(i.get("test") == nullptr);
	assert(!i.put("test", 24));
	assert(i.get("test") == nullptr);
}

TEST_CASE("integer (16-bit)")
{
	cainteoir::object i((int16_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);

	assert(i.get("test") == nullptr);
	assert(!i.put("test", 24));
	assert(i.get("test") == nullptr);
}

TEST_CASE("integer (32-bit)")
{
	cainteoir::object i((int32_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);

	assert(i.get("test") == nullptr);
	assert(!i.put("test", 24));
	assert(i.get("test") == nullptr);
}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
TEST_CASE("integer (64-bit)")
{
	cainteoir::object i((int64_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);

	assert(i.get("test") == nullptr);
	assert(!i.put("test", 24));
	assert(i.get("test") == nullptr);
}
#endif

TEST_CASE("real (32-bit)")
{
	cainteoir::object r(2.63f);
	assert(r.type() == cainteoir::object_type::real);
	assert(r.real() == 2.63f);

	assert(!r.is_null());
	assert(!r.is_boolean());
	assert(!r.is_integer());
	assert(r.is_real());
	assert(!r.is_string());
	assert(!r.is_buffer());
	assert(!r.is_phoneme());
	assert(!r.is_range());
	assert(!r.is_dictionary());

	cainteoir::object s(r);
	assert(s.type() == cainteoir::object_type::real);
	assert(s.real() == 2.63f);

	cainteoir::object t;
	t = r;
	assert(t.type() == cainteoir::object_type::real);
	assert(t.real() == 2.63f);

	cainteoir::object o(cainteoir::object_type::real);
	assert(o.type() == cainteoir::object_type::real);
	assert(o.real() == 0.0);

	assert(r.get("test") == nullptr);
	assert(!r.put("test", 24));
	assert(r.get("test") == nullptr);
}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
TEST_CASE("real (64-bit)")
{
	cainteoir::object r(2.63);
	assert(r.type() == cainteoir::object_type::real);
	assert(r.real() == 2.63);

	assert(!r.is_null());
	assert(!r.is_boolean());
	assert(!r.is_integer());
	assert(r.is_real());
	assert(!r.is_string());
	assert(!r.is_buffer());
	assert(!r.is_phoneme());
	assert(!r.is_range());
	assert(!r.is_dictionary());

	cainteoir::object s(r);
	assert(s.type() == cainteoir::object_type::real);
	assert(s.real() == 2.63);

	cainteoir::object t;
	t = r;
	assert(t.type() == cainteoir::object_type::real);
	assert(t.real() == 2.63);

	assert(r.get("test") == nullptr);
	assert(!r.put("test", 24));
	assert(r.get("test") == nullptr);
}
#endif

TEST_CASE("string")
{
	cainteoir::object s("hello");
	assert(s.type() == cainteoir::object_type::string);
	assert(strcmp(s.string(), "hello") == 0);

	assert(!s.is_null());
	assert(!s.is_boolean());
	assert(!s.is_integer());
	assert(!s.is_real());
	assert(s.is_string());
	assert(!s.is_buffer());
	assert(!s.is_phoneme());
	assert(!s.is_range());
	assert(!s.is_dictionary());

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::string);
	assert(strcmp(t.string(), "hello") == 0);

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::string);
	assert(strcmp(u.string(), "hello") == 0);

	cainteoir::object o(cainteoir::object_type::string);
	assert(o.type() == cainteoir::object_type::string);
	assert(!o.string());

	assert(s.get("test") == nullptr);
	assert(!s.put("test", 24));
	assert(s.get("test") == nullptr);
}

TEST_CASE("string does not create a copy")
{
	const char *test = "test";

	cainteoir::object s(test);
	assert(s.type() == cainteoir::object_type::string);
	assert(s.string() == test);

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::string);
	assert(t.string() == test);

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::string);
	assert(u.string() == test);
}

TEST_CASE("buffer")
{
	cainteoir::object s(cainteoir::make_buffer("hello", 5));
	assert(s.type() == cainteoir::object_type::buffer);
	assert(s.buffer()->compare("hello") == 0);

	assert(!s.is_null());
	assert(!s.is_boolean());
	assert(!s.is_integer());
	assert(!s.is_real());
	assert(!s.is_string());
	assert(s.is_buffer());
	assert(!s.is_phoneme());
	assert(!s.is_range());
	assert(!s.is_dictionary());

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::buffer);
	assert(t.buffer()->compare("hello") == 0);

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::buffer);
	assert(u.buffer()->compare("hello") == 0);

	cainteoir::object o(cainteoir::object_type::buffer);
	assert(o.type() == cainteoir::object_type::buffer);
	assert(!o.buffer());

	assert(s.get("test") == nullptr);
	assert(!s.put("test", 24));
	assert(s.get("test") == nullptr);
}

TEST_CASE("phoneme")
{
	cainteoir::object p(ipa::phoneme(ipa::voiced | ipa::alveolar | ipa::plosive));
	assert(p.type() == cainteoir::object_type::phoneme);
	assert(p.phoneme() == (ipa::voiced | ipa::alveolar | ipa::plosive));

	assert(!p.is_null());
	assert(!p.is_boolean());
	assert(!p.is_integer());
	assert(!p.is_real());
	assert(!p.is_string());
	assert(!p.is_buffer());
	assert(p.is_phoneme());
	assert(!p.is_range());
	assert(!p.is_dictionary());

	cainteoir::object q(p);
	assert(q.type() == cainteoir::object_type::phoneme);
	assert(q.phoneme() == (ipa::voiced | ipa::alveolar | ipa::plosive));

	cainteoir::object r;
	r = p;
	assert(r.type() == cainteoir::object_type::phoneme);
	assert(r.phoneme() == (ipa::voiced | ipa::alveolar | ipa::plosive));

	cainteoir::object o(cainteoir::object_type::phoneme);
	assert(o.type() == cainteoir::object_type::phoneme);
	assert(o.phoneme() == 0);

	assert(p.get("test") == nullptr);
	assert(!p.put("test", 24));
	assert(p.get("test") == nullptr);
}

TEST_CASE("range")
{
	cainteoir::object p(cainteoir::range<uint32_t>(3, 6));
	assert(p.type() == cainteoir::object_type::range);
	assert(*p.range().begin() == 3);
	assert(*p.range().end() == 6);

	assert(!p.is_null());
	assert(!p.is_boolean());
	assert(!p.is_integer());
	assert(!p.is_real());
	assert(!p.is_string());
	assert(!p.is_buffer());
	assert(!p.is_phoneme());
	assert(p.is_range());
	assert(!p.is_dictionary());

	cainteoir::object q(p);
	assert(q.type() == cainteoir::object_type::range);
	assert(*q.range().begin() == 3);
	assert(*q.range().end() == 6);

	cainteoir::object r;
	r = p;
	assert(r.type() == cainteoir::object_type::range);
	assert(*r.range().begin() == 3);
	assert(*r.range().end() == 6);

	cainteoir::object o(cainteoir::object_type::range);
	assert(o.type() == cainteoir::object_type::range);
	assert(*o.range().begin() == 0);
	assert(*o.range().end() == 0);

	assert(p.get("test") == nullptr);
	assert(!p.put("test", 24));
	assert(p.get("test") == nullptr);
}

TEST_CASE("dictionary")
{
	cainteoir::object d(cainteoir::object_type::dictionary);
	assert(d.type() == cainteoir::object_type::dictionary);

	assert(!d.is_null());
	assert(!d.is_boolean());
	assert(!d.is_integer());
	assert(!d.is_real());
	assert(!d.is_string());
	assert(!d.is_buffer());
	assert(!d.is_phoneme());
	assert(!d.is_range());
	assert(d.is_dictionary());

	assert(d.get("test") == nullptr);
	assert(d.get("tester") == nullptr);

	assert(d.put("test", 24));

	assert(d.get("test") != nullptr);
	assert(d.get("tester") == nullptr);

	assert(d.get("test")->type() == cainteoir::object_type::integer);
	assert(d.get("test")->integer() == 24);
}
