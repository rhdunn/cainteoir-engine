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
	assert(!o.is_number());
	assert(!o.is_string());
	assert(!o.is_buffer());
	assert(!o.is_phoneme());
	assert(!o.is_range());
	assert(!o.is_dictionary());
	assert(!o.is_codepoint());

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

	cainteoir::object s(o, cainteoir::object::reference);
	assert(s.type() == cainteoir::object_type::null);
	assert(s.integer() == 0);

	assert(o.size() == 0);
	assert(o.empty());

	assert(o.get("test").type() == cainteoir::object_type::null);
	assert(!o.put("test", 24));
	assert(o.get("test").type() == cainteoir::object_type::null);

	assert(o.size() == 0);
	assert(o.empty());
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
	assert(!b.is_number());
	assert(!b.is_string());
	assert(!b.is_buffer());
	assert(!b.is_phoneme());
	assert(!b.is_range());
	assert(!b.is_dictionary());
	assert(!b.is_codepoint());

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

	cainteoir::object e(b, cainteoir::object::reference);
	assert(c.type() == cainteoir::object_type::boolean);
	assert(c.boolean());

	assert(b.size() == 0);
	assert(b.empty());

	assert(b.get("test").type() == cainteoir::object_type::null);
	assert(!b.put("test", 24));
	assert(b.get("test").type() == cainteoir::object_type::null);

	assert(b.size() == 0);
	assert(b.empty());
}

TEST_CASE("integer  (8-bit)")
{
	cainteoir::object i((int8_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);
	assert(i.number() == 5.0);

	assert(i.size() == 0);
	assert(i.empty());

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(i.is_number());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());
	assert(!i.is_codepoint());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);
	assert(j.number() == 5.0);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
	assert(k.number() == 5.0);

	cainteoir::object o(cainteoir::object_type::integer);
	assert(o.type() == cainteoir::object_type::integer);
	assert(o.integer() == 0);
	assert(o.number() == 0.0);

	cainteoir::object l(i, cainteoir::object::reference);
	assert(l.type() == cainteoir::object_type::integer);
	assert(l.integer() == 5);
	assert(l.number() == 5.0);

	assert(i.size() == 0);
	assert(i.empty());

	assert(i.get("test").type() == cainteoir::object_type::null);
	assert(!i.put("test", 24));
	assert(i.get("test").type() == cainteoir::object_type::null);

	assert(i.size() == 0);
	assert(i.empty());
}

TEST_CASE("integer (16-bit)")
{
	cainteoir::object i((int16_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);
	assert(i.number() == 5.0);

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(i.is_number());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());
	assert(!i.is_codepoint());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);
	assert(j.number() == 5.0);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
	assert(k.number() == 5.0);

	cainteoir::object l(i, cainteoir::object::reference);
	assert(l.type() == cainteoir::object_type::integer);
	assert(l.integer() == 5);
	assert(l.number() == 5.0);

	assert(i.size() == 0);
	assert(i.empty());

	assert(i.get("test").type() == cainteoir::object_type::null);
	assert(!i.put("test", 24));
	assert(i.get("test").type() == cainteoir::object_type::null);

	assert(i.size() == 0);
	assert(i.empty());
}

TEST_CASE("integer (32-bit)")
{
	cainteoir::object i((int32_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);
	assert(i.number() == 5.0);

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(i.is_number());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());
	assert(!i.is_codepoint());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);
	assert(j.number() == 5.0);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
	assert(k.number() == 5.0);

	cainteoir::object l(i, cainteoir::object::reference);
	assert(l.type() == cainteoir::object_type::integer);
	assert(l.integer() == 5);
	assert(l.number() == 5.0);

	assert(i.size() == 0);
	assert(i.empty());

	assert(i.get("test").type() == cainteoir::object_type::null);
	assert(!i.put("test", 24));
	assert(i.get("test").type() == cainteoir::object_type::null);

	assert(i.size() == 0);
	assert(i.empty());
}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
TEST_CASE("integer (64-bit)")
{
	cainteoir::object i((int64_t)5);
	assert(i.type() == cainteoir::object_type::integer);
	assert(i.integer() == 5);
	assert(i.number() == 5.0);

	assert(!i.is_null());
	assert(!i.is_boolean());
	assert(i.is_integer());
	assert(!i.is_real());
	assert(i.is_number());
	assert(!i.is_string());
	assert(!i.is_buffer());
	assert(!i.is_phoneme());
	assert(!i.is_range());
	assert(!i.is_dictionary());
	assert(!i.is_codepoint());

	cainteoir::object j(i);
	assert(j.type() == cainteoir::object_type::integer);
	assert(j.integer() == 5);
	assert(j.number() == 5.0);

	cainteoir::object k;
	k = i;
	assert(k.type() == cainteoir::object_type::integer);
	assert(k.integer() == 5);
	assert(k.number() == 5.0);

	cainteoir::object l(i, cainteoir::object::reference);
	assert(l.type() == cainteoir::object_type::integer);
	assert(l.integer() == 5);
	assert(l.number() == 5.0);

	assert(i.size() == 0);
	assert(i.empty());

	assert(i.get("test").type() == cainteoir::object_type::null);
	assert(!i.put("test", 24));
	assert(i.get("test").type() == cainteoir::object_type::null);

	assert(i.size() == 0);
	assert(i.empty());
}
#endif

TEST_CASE("real (32-bit)")
{
	cainteoir::object r(2.63f);
	assert(r.type() == cainteoir::object_type::real);
	assert(r.real() == 2.63f);
	assert(r.number() == 2.63);

	assert(!r.is_null());
	assert(!r.is_boolean());
	assert(!r.is_integer());
	assert(r.is_real());
	assert(r.is_number());
	assert(!r.is_string());
	assert(!r.is_buffer());
	assert(!r.is_phoneme());
	assert(!r.is_range());
	assert(!r.is_dictionary());
	assert(!r.is_codepoint());

	cainteoir::object s(r);
	assert(s.type() == cainteoir::object_type::real);
	assert(s.real() == 2.63f);
	assert(s.number() == 2.63);

	cainteoir::object t;
	t = r;
	assert(t.type() == cainteoir::object_type::real);
	assert(t.real() == 2.63f);
	assert(t.number() == 2.63);

	cainteoir::object o(cainteoir::object_type::real);
	assert(o.type() == cainteoir::object_type::real);
	assert(o.real() == 0.0);
	assert(o.number() == 0.0);

	cainteoir::object u(r, cainteoir::object::reference);
	assert(u.type() == cainteoir::object_type::real);
	assert(u.real() == 2.63f);
	assert(u.number() == 2.63);

	assert(r.size() == 0);
	assert(r.empty());

	assert(r.get("test").type() == cainteoir::object_type::null);
	assert(!r.put("test", 24));
	assert(r.get("test").type() == cainteoir::object_type::null);

	assert(r.size() == 0);
	assert(r.empty());
}

#if (INTPTR_MAX == INT64_MAX) // 64-bit
TEST_CASE("real (64-bit)")
{
	cainteoir::object r(2.63);
	assert(r.type() == cainteoir::object_type::real);
	assert(r.real() == 2.63);
	assert(r.number() == 2.63);

	assert(!r.is_null());
	assert(!r.is_boolean());
	assert(!r.is_integer());
	assert(r.is_real());
	assert(r.is_number());
	assert(!r.is_string());
	assert(!r.is_buffer());
	assert(!r.is_phoneme());
	assert(!r.is_range());
	assert(!r.is_dictionary());
	assert(!r.is_codepoint());

	cainteoir::object s(r);
	assert(s.type() == cainteoir::object_type::real);
	assert(s.real() == 2.63);
	assert(s.number() == 2.63);

	cainteoir::object t;
	t = r;
	assert(t.type() == cainteoir::object_type::real);
	assert(t.real() == 2.63);
	assert(t.number() == 2.63);

	cainteoir::object u(r, cainteoir::object::reference);
	assert(u.type() == cainteoir::object_type::real);
	assert(u.real() == 2.63);
	assert(u.number() == 2.63);

	assert(r.size() == 0);
	assert(r.empty());

	assert(r.get("test").type() == cainteoir::object_type::null);
	assert(!r.put("test", 24));
	assert(r.get("test").type() == cainteoir::object_type::null);

	assert(r.size() == 0);
	assert(r.empty());
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
	assert(!s.is_number());
	assert(s.is_string());
	assert(!s.is_buffer());
	assert(!s.is_phoneme());
	assert(!s.is_range());
	assert(!s.is_dictionary());
	assert(!s.is_codepoint());

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::string);
	assert(strcmp(t.string(), "hello") == 0);

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::string);
	assert(strcmp(u.string(), "hello") == 0);

	cainteoir::object o(cainteoir::object_type::string);
	assert(o.type() == cainteoir::object_type::null);
	assert(!o.string());

	cainteoir::object v(s, cainteoir::object::reference);
	assert(v.type() == cainteoir::object_type::string);
	assert(strcmp(v.string(), "hello") == 0);

	assert(s.size() == 0);
	assert(s.empty());

	assert(s.get("test").type() == cainteoir::object_type::null);
	assert(!s.put("test", 24));
	assert(s.get("test").type() == cainteoir::object_type::null);

	assert(s.size() == 0);
	assert(s.empty());
}

TEST_CASE("string (null)")
{
	cainteoir::object s(nullptr);
	assert(s.type() == cainteoir::object_type::null);
	assert(!s.string());

	assert(s.is_null());
	assert(!s.is_boolean());
	assert(!s.is_integer());
	assert(!s.is_real());
	assert(!s.is_number());
	assert(!s.is_string());
	assert(!s.is_buffer());
	assert(!s.is_phoneme());
	assert(!s.is_range());
	assert(!s.is_dictionary());
	assert(!s.is_codepoint());

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::null);
	assert(!t.string());

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::null);
	assert(!u.string());

	assert(s.get("test").type() == cainteoir::object_type::null);
	assert(!s.put("test", 24));
	assert(s.get("test").type() == cainteoir::object_type::null);
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
	assert(!s.is_number());
	assert(!s.is_string());
	assert(s.is_buffer());
	assert(!s.is_phoneme());
	assert(!s.is_range());
	assert(!s.is_dictionary());
	assert(!s.is_codepoint());

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::buffer);
	assert(t.buffer()->compare("hello") == 0);

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::buffer);
	assert(u.buffer()->compare("hello") == 0);

	cainteoir::object o(cainteoir::object_type::buffer);
	assert(o.type() == cainteoir::object_type::null);
	assert(!o.buffer());

	cainteoir::object v(s, cainteoir::object::reference);
	assert(v.type() == cainteoir::object_type::buffer_ref);
	assert(v.buffer()->compare("hello") == 0);

	assert(s.size() == 0);
	assert(s.empty());

	assert(s.get("test").type() == cainteoir::object_type::null);
	assert(!s.put("test", 24));
	assert(s.get("test").type() == cainteoir::object_type::null);

	assert(s.size() == 0);
	assert(s.empty());
}

TEST_CASE("buffer (null)")
{
	cainteoir::object s(std::shared_ptr<cainteoir::buffer>{});
	assert(s.type() == cainteoir::object_type::null);
	assert(!s.buffer());

	assert(s.is_null());
	assert(!s.is_boolean());
	assert(!s.is_integer());
	assert(!s.is_real());
	assert(!s.is_number());
	assert(!s.is_string());
	assert(!s.is_buffer());
	assert(!s.is_phoneme());
	assert(!s.is_range());
	assert(!s.is_dictionary());
	assert(!s.is_codepoint());

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::null);
	assert(!t.string());

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::null);
	assert(!u.string());

	assert(s.get("test").type() == cainteoir::object_type::null);
	assert(!s.put("test", 24));
	assert(s.get("test").type() == cainteoir::object_type::null);
}

TEST_CASE("buffer ref")
{
	cainteoir::object test(cainteoir::make_buffer("hello", 5));

	cainteoir::object s(test, cainteoir::object::reference);
	assert(s.type() == cainteoir::object_type::buffer_ref);
	assert(s.buffer()->compare("hello") == 0);

	assert(!s.is_null());
	assert(!s.is_boolean());
	assert(!s.is_integer());
	assert(!s.is_real());
	assert(!s.is_number());
	assert(!s.is_string());
	assert(s.is_buffer());
	assert(!s.is_phoneme());
	assert(!s.is_range());
	assert(!s.is_dictionary());
	assert(!s.is_codepoint());

	cainteoir::object t(s);
	assert(t.type() == cainteoir::object_type::buffer_ref);
	assert(t.buffer()->compare("hello") == 0);

	cainteoir::object u;
	u = s;
	assert(u.type() == cainteoir::object_type::buffer_ref);
	assert(u.buffer()->compare("hello") == 0);

	cainteoir::object o(cainteoir::object_type::buffer_ref);
	assert(o.type() == cainteoir::object_type::null);
	assert(!o.buffer());

	cainteoir::object v(s, cainteoir::object::reference);
	assert(v.type() == cainteoir::object_type::buffer_ref);
	assert(v.buffer()->compare("hello") == 0);

	assert(s.size() == 0);
	assert(s.empty());

	assert(s.get("test").type() == cainteoir::object_type::null);
	assert(!s.put("test", 24));
	assert(s.get("test").type() == cainteoir::object_type::null);

	assert(s.size() == 0);
	assert(s.empty());
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
	assert(!p.is_number());
	assert(!p.is_string());
	assert(!p.is_buffer());
	assert(p.is_phoneme());
	assert(!p.is_range());
	assert(!p.is_dictionary());
	assert(!p.is_codepoint());

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

	cainteoir::object s(p, cainteoir::object::reference);
	assert(s.type() == cainteoir::object_type::phoneme);
	assert(s.phoneme() == (ipa::voiced | ipa::alveolar | ipa::plosive));

	assert(p.size() == 0);
	assert(p.empty());

	assert(p.get("test").type() == cainteoir::object_type::null);
	assert(!p.put("test", 24));
	assert(p.get("test").type() == cainteoir::object_type::null);

	assert(p.size() == 0);
	assert(p.empty());
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
	assert(!p.is_number());
	assert(!p.is_string());
	assert(!p.is_buffer());
	assert(!p.is_phoneme());
	assert(p.is_range());
	assert(!p.is_dictionary());
	assert(!p.is_codepoint());

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

	cainteoir::object s(p, cainteoir::object::reference);
	assert(s.type() == cainteoir::object_type::range);
	assert(*s.range().begin() == 3);
	assert(*s.range().end() == 6);

	assert(p.size() == 0);
	assert(p.empty());

	assert(p.get("test").type() == cainteoir::object_type::null);
	assert(!p.put("test", 24));
	assert(p.get("test").type() == cainteoir::object_type::null);

	assert(p.size() == 0);
	assert(p.empty());
}

TEST_CASE("codepoint")
{
	cainteoir::object c((uint32_t)0x20);
	assert(c.type() == cainteoir::object_type::codepoint);
	assert(c.codepoint() == 0x20);

	assert(!c.is_null());
	assert(!c.is_boolean());
	assert(!c.is_integer());
	assert(!c.is_real());
	assert(!c.is_number());
	assert(!c.is_string());
	assert(!c.is_buffer());
	assert(!c.is_phoneme());
	assert(!c.is_range());
	assert(!c.is_dictionary());
	assert(c.is_codepoint());

	cainteoir::object d(c);
	assert(d.type() == cainteoir::object_type::codepoint);
	assert(d.codepoint() == 0x20);

	cainteoir::object e;
	e = c;
	assert(e.type() == cainteoir::object_type::codepoint);
	assert(e.codepoint() == 0x20);

	cainteoir::object f(c, cainteoir::object::reference);
	assert(f.type() == cainteoir::object_type::codepoint);
	assert(f.integer() == 0x20);

	assert(c.size() == 0);
	assert(c.empty());

	assert(c.get("test").type() == cainteoir::object_type::null);
	assert(!c.put("test", 24));
	assert(c.get("test").type() == cainteoir::object_type::null);

	assert(c.size() == 0);
	assert(c.empty());
}

TEST_CASE("dictionary")
{
	cainteoir::object d(cainteoir::object_type::dictionary);
	assert(d.type() == cainteoir::object_type::dictionary);

	assert(d.size() == 0);
	assert(d.empty());

	assert(d.get("test").type() == cainteoir::object_type::null);
	assert(d.get("tester").type() == cainteoir::object_type::null);

	assert(!d.is_null());
	assert(!d.is_boolean());
	assert(!d.is_integer());
	assert(!d.is_real());
	assert(!d.is_number());
	assert(!d.is_string());
	assert(!d.is_buffer());
	assert(!d.is_phoneme());
	assert(!d.is_range());
	assert(d.is_dictionary());
	assert(!d.is_codepoint());

	cainteoir::object e(d);
	assert(e.type() == cainteoir::object_type::dictionary);
	assert(e.size() == 0);
	assert(e.empty());
	assert(e.get("test").type() == cainteoir::object_type::null);
	assert(e.get("tester").type() == cainteoir::object_type::null);

	cainteoir::object f;
	f = d;
	assert(f.type() == cainteoir::object_type::dictionary);
	assert(f.size() == 0);
	assert(f.empty());
	assert(f.get("test").type() == cainteoir::object_type::null);
	assert(f.get("tester").type() == cainteoir::object_type::null);

	cainteoir::object g(d, cainteoir::object::reference);
	assert(g.type() == cainteoir::object_type::dictionary_ref);
	assert(g.size() == 0);
	assert(g.empty());
	assert(g.get("test").type() == cainteoir::object_type::null);
	assert(g.get("tester").type() == cainteoir::object_type::null);
}

TEST_CASE("dictionary ref")
{
	cainteoir::object test(cainteoir::object_type::dictionary);

	cainteoir::object d(test, cainteoir::object::reference);
	assert(d.type() == cainteoir::object_type::dictionary_ref);

	assert(d.size() == 0);
	assert(d.empty());

	assert(d.get("test").type() == cainteoir::object_type::null);
	assert(d.get("tester").type() == cainteoir::object_type::null);

	assert(!d.is_null());
	assert(!d.is_boolean());
	assert(!d.is_integer());
	assert(!d.is_real());
	assert(!d.is_number());
	assert(!d.is_string());
	assert(!d.is_buffer());
	assert(!d.is_phoneme());
	assert(!d.is_range());
	assert(d.is_dictionary());
	assert(!d.is_codepoint());

	cainteoir::object e(d);
	assert(e.type() == cainteoir::object_type::dictionary_ref);
	assert(e.size() == 0);
	assert(e.empty());
	assert(e.get("test").type() == cainteoir::object_type::null);
	assert(e.get("tester").type() == cainteoir::object_type::null);

	cainteoir::object f;
	f = d;
	assert(f.type() == cainteoir::object_type::dictionary_ref);
	assert(f.size() == 0);
	assert(f.empty());
	assert(f.get("test").type() == cainteoir::object_type::null);
	assert(f.get("tester").type() == cainteoir::object_type::null);

	cainteoir::object o(cainteoir::object_type::dictionary_ref);
	assert(o.type() == cainteoir::object_type::null);
	assert(o.number() == 0);

	cainteoir::object g(d, cainteoir::object::reference);
	assert(g.type() == cainteoir::object_type::dictionary_ref);
	assert(g.size() == 0);
	assert(g.empty());
	assert(g.get("test").type() == cainteoir::object_type::null);
	assert(g.get("tester").type() == cainteoir::object_type::null);
}

TEST_CASE("object::put (dictionary)")
{
	cainteoir::object d(cainteoir::object_type::dictionary);

	assert(d.put("test", 24));

	assert(d.size() == 1);
	assert(!d.empty());

	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 24);
	assert(d.get("tester").type() == cainteoir::object_type::null);

	assert(d.size() == 1);
	assert(!d.empty());
}

TEST_CASE("object::put (dictionary_ref)")
{
	cainteoir::object o(cainteoir::object_type::dictionary);
	cainteoir::object d(o, cainteoir::object::reference);

	assert(d.put("test", 24));

	assert(d.size() == 1);
	assert(!d.empty());

	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 24);
	assert(d.get("tester").type() == cainteoir::object_type::null);

	assert(d.size() == 1);
	assert(!d.empty());
}

TEST_CASE("dictionary updates affect all copies and references")
{
	cainteoir::object d(cainteoir::object_type::dictionary);
	cainteoir::object e(d);
	cainteoir::object f;
	f = d;

	cainteoir::object g(d, cainteoir::object::reference);
	cainteoir::object h(g);
	cainteoir::object i;
	i = g;

	assert(d.put("test", 62));
	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 62);
	assert(e.get("test").type() == cainteoir::object_type::integer);
	assert(e.get("test").integer() == 62);
	assert(f.get("test").type() == cainteoir::object_type::integer);
	assert(f.get("test").integer() == 62);
	assert(g.get("test").type() == cainteoir::object_type::integer);
	assert(g.get("test").integer() == 62);
	assert(h.get("test").type() == cainteoir::object_type::integer);
	assert(h.get("test").integer() == 62);
	assert(i.get("test").type() == cainteoir::object_type::integer);
	assert(i.get("test").integer() == 62);

	assert(e.put("test", 44));
	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 44);
	assert(e.get("test").type() == cainteoir::object_type::integer);
	assert(e.get("test").integer() == 44);
	assert(f.get("test").type() == cainteoir::object_type::integer);
	assert(f.get("test").integer() == 44);
	assert(g.get("test").type() == cainteoir::object_type::integer);
	assert(g.get("test").integer() == 44);
	assert(h.get("test").type() == cainteoir::object_type::integer);
	assert(h.get("test").integer() == 44);
	assert(i.get("test").type() == cainteoir::object_type::integer);
	assert(i.get("test").integer() == 44);

	assert(f.put("test", 19));
	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 19);
	assert(e.get("test").type() == cainteoir::object_type::integer);
	assert(e.get("test").integer() == 19);
	assert(f.get("test").type() == cainteoir::object_type::integer);
	assert(f.get("test").integer() == 19);
	assert(g.get("test").type() == cainteoir::object_type::integer);
	assert(g.get("test").integer() == 19);
	assert(h.get("test").type() == cainteoir::object_type::integer);
	assert(h.get("test").integer() == 19);
	assert(i.get("test").type() == cainteoir::object_type::integer);
	assert(i.get("test").integer() == 19);

	assert(g.put("test", 73));
	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 73);
	assert(e.get("test").type() == cainteoir::object_type::integer);
	assert(e.get("test").integer() == 73);
	assert(f.get("test").type() == cainteoir::object_type::integer);
	assert(f.get("test").integer() == 73);
	assert(g.get("test").type() == cainteoir::object_type::integer);
	assert(g.get("test").integer() == 73);
	assert(h.get("test").type() == cainteoir::object_type::integer);
	assert(h.get("test").integer() == 73);
	assert(i.get("test").type() == cainteoir::object_type::integer);
	assert(i.get("test").integer() == 73);

	assert(h.put("test", 57));
	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 57);
	assert(e.get("test").type() == cainteoir::object_type::integer);
	assert(e.get("test").integer() == 57);
	assert(f.get("test").type() == cainteoir::object_type::integer);
	assert(f.get("test").integer() == 57);
	assert(g.get("test").type() == cainteoir::object_type::integer);
	assert(g.get("test").integer() == 57);
	assert(h.get("test").type() == cainteoir::object_type::integer);
	assert(h.get("test").integer() == 57);
	assert(i.get("test").type() == cainteoir::object_type::integer);
	assert(i.get("test").integer() == 57);

	assert(i.put("test", 83));
	assert(d.get("test").type() == cainteoir::object_type::integer);
	assert(d.get("test").integer() == 83);
	assert(e.get("test").type() == cainteoir::object_type::integer);
	assert(e.get("test").integer() == 83);
	assert(f.get("test").type() == cainteoir::object_type::integer);
	assert(f.get("test").integer() == 83);
	assert(g.get("test").type() == cainteoir::object_type::integer);
	assert(g.get("test").integer() == 83);
	assert(h.get("test").type() == cainteoir::object_type::integer);
	assert(h.get("test").integer() == 83);
	assert(i.get("test").type() == cainteoir::object_type::integer);
	assert(i.get("test").integer() == 83);
}
