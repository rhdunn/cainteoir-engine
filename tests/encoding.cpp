/* Test for text encodings.
 *
 * Copyright (C) 2011-2012 Reece H. Dunn
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

#include <cainteoir/encoding.hpp>
#include <stdexcept>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("encoding");

void match(const std::shared_ptr<cainteoir::buffer> &got, const char *expected)
{
	assert(strlen(expected) == got->size());
	const char *buffer = got->begin();
	for (int i = 0; i < got->size(); ++i)
		assert(uint8_t(expected[i]) == uint8_t(buffer[i]));
}

#define throws(x, msg) try { x; assert(false); } catch (std::exception &e) { assert(msg == e.what()); }

TEST_CASE("looking up characters in codepage 1252")
{
	cainteoir::encoding e(1252);
	match(e.lookup('g'), "g");
	match(e.lookup('\x85'), "\xE2\x80\xA6"); // U+2026 ELLIPSIS
	match(e.lookup('\x91'), "\xE2\x80\x98"); // U+2018 SINGLE LEFT CURLY QUOTE
	match(e.lookup('\x92'), "\xE2\x80\x99"); // U+2019 SINGLE RIGHT CURLY QUOTE
	match(e.lookup('\x93'), "\xE2\x80\x9C"); // U+201C DOUBLE LEFT CURLY QUOTE
	match(e.lookup('\x94'), "\xE2\x80\x9D"); // U+201D DOUBLE RIGHT CURLY QUOTE
	match(e.lookup('\x95'), "\xE2\x80\xA2"); // U+2022 BULLET
	match(e.lookup('\x96'), "\xE2\x80\x93"); // U+2013 EN DASH
	match(e.lookup('\x97'), "\xE2\x80\x94"); // U+2014 EM DASH
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xA1'), "\xC2\xA1");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("looking up unsupported character set")
{
	throws(cainteoir::encoding(1), std::string("unsupported character set (codepage not recognised)"));
}

TEST_CASE("looking up characters in the windows-1252 encoding")
{
	cainteoir::encoding e("windows-1252");
	match(e.lookup('g'), "g");
	match(e.lookup('\x85'), "\xE2\x80\xA6"); // U+2026 ELLIPSIS
	match(e.lookup('\x91'), "\xE2\x80\x98"); // U+2018 SINGLE LEFT CURLY QUOTE
	match(e.lookup('\x92'), "\xE2\x80\x99"); // U+2019 SINGLE RIGHT CURLY QUOTE
	match(e.lookup('\x93'), "\xE2\x80\x9C"); // U+201C DOUBLE LEFT CURLY QUOTE
	match(e.lookup('\x94'), "\xE2\x80\x9D"); // U+201D DOUBLE RIGHT CURLY QUOTE
	match(e.lookup('\x95'), "\xE2\x80\xA2"); // U+2022 BULLET
	match(e.lookup('\x96'), "\xE2\x80\x93"); // U+2013 EN DASH
	match(e.lookup('\x97'), "\xE2\x80\x94"); // U+2014 EM DASH
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("looking up characters in latin1")
{
	cainteoir::encoding e("latin1");
	match(e.lookup('g'), "g");
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xA1'), "\xC2\xA1");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("looking up characters in latin2")
{
	cainteoir::encoding e("latin2");
	match(e.lookup('g'), "g");
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xA1'), "\xC4\x84");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("looking up characters in LATIN2 (case-insensitive lookup)")
{
	cainteoir::encoding e("LATIN2");
	match(e.lookup('g'), "g");
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xA1'), "\xC4\x84");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("looking up unsupported character set")
{
	throws(cainteoir::encoding("latin-1"), std::string("unsupported character set (no conversion found)"));
}

TEST_CASE("selecting codepage 1252 from codepage 1250")
{
	cainteoir::encoding e(1250);
	match(e.lookup('\xA1'), "\xCB\x87");

	assert(e.set_encoding(1252));
	match(e.lookup('g'), "g");
	match(e.lookup('\x85'), "\xE2\x80\xA6"); // U+2026 ELLIPSIS
	match(e.lookup('\x91'), "\xE2\x80\x98"); // U+2018 SINGLE LEFT CURLY QUOTE
	match(e.lookup('\x92'), "\xE2\x80\x99"); // U+2019 SINGLE RIGHT CURLY QUOTE
	match(e.lookup('\x93'), "\xE2\x80\x9C"); // U+201C DOUBLE LEFT CURLY QUOTE
	match(e.lookup('\x94'), "\xE2\x80\x9D"); // U+201D DOUBLE RIGHT CURLY QUOTE
	match(e.lookup('\x95'), "\xE2\x80\xA2"); // U+2022 BULLET
	match(e.lookup('\x96'), "\xE2\x80\x93"); // U+2013 EN DASH
	match(e.lookup('\x97'), "\xE2\x80\x94"); // U+2014 EM DASH
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xA1'), "\xC2\xA1");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("selecting an unsupported codepage")
{
	cainteoir::encoding e(1250);
	throws(e.set_encoding(123), std::string("unsupported character set (codepage not recognised)"));
}

TEST_CASE("selecting encoding latin2")
{
	cainteoir::encoding e(1250);
	match(e.lookup('\xA1'), "\xCB\x87");

	assert(e.set_encoding("latin2"));
	match(e.lookup('g'), "g");
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xA1'), "\xC4\x84");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("selecting encoding LATIN2 (case-insensitive lookup)")
{
	cainteoir::encoding e(1250);
	match(e.lookup('\xA1'), "\xCB\x87");

	assert(e.set_encoding("latin2"));
	match(e.lookup('g'), "g");
	match(e.lookup('\xA0'), "\xC2\xA0");
	match(e.lookup('\xA1'), "\xC4\x84");
	match(e.lookup('\xDF'), "\xC3\x9F");
}

TEST_CASE("selecting an unsupported character set")
{
	cainteoir::encoding e(1250);
	throws(e.set_encoding("latin-99"), std::string("unsupported character set (no conversion found)"));
}

TEST_CASE("selecting the same codepage")
{
	cainteoir::encoding e(1250);
	match(e.lookup('\xA1'), "\xCB\x87");

	assert(!e.set_encoding(1250));
	match(e.lookup('\xA1'), "\xCB\x87");
}

TEST_CASE("selecting the same character set")
{
	cainteoir::encoding e("latin2");
	match(e.lookup('\xA1'), "\xC4\x84");

	assert(!e.set_encoding("latin2"));
	match(e.lookup('\xA1'), "\xC4\x84");
}

TEST_CASE("decoding a null buffer")
{
	cainteoir::encoding e(1252);
	assert(e.decode(std::shared_ptr<cainteoir::buffer>()) == std::shared_ptr<cainteoir::buffer>());

	cainteoir::rope ret;
	e.decode(std::shared_ptr<cainteoir::buffer>(), ret);
	assert(ret.empty());
}

TEST_CASE("decoding a buffer")
{
	cainteoir::encoding e(1252);
	auto data = std::make_shared<cainteoir::buffer>("\x93x, y, \x85\x94");
	match(e.decode(data), "\xE2\x80\x9Cx, y, \xE2\x80\xA6\xE2\x80\x9D");

	cainteoir::rope ret;
	e.decode(data, ret);
	match(ret.buffer(), "\xE2\x80\x9Cx, y, \xE2\x80\xA6\xE2\x80\x9D");
}

TEST_CASE("decoding a utf-8 buffer (no conversion)")
{
	cainteoir::encoding e("utf-8");
	auto data = std::make_shared<cainteoir::buffer>("\xE2\x80\x9Cx, y, \xE2\x80\xA6\xE2\x80\x9D");
	assert(e.decode(data).get() == data.get());

	cainteoir::rope ret;
	e.decode(data, ret);
	match(ret.buffer(), "\xE2\x80\x9Cx, y, \xE2\x80\xA6\xE2\x80\x9D");
}

TEST_CASE("decoding a us-ascii buffer (no conversion)")
{
	cainteoir::encoding e("us-ascii");
	auto data = std::make_shared<cainteoir::buffer>("This is a test string!");
	assert(e.decode(data).get() == data.get());

	cainteoir::rope ret;
	e.decode(data, ret);
	match(ret.buffer(), "This is a test string!");
}
