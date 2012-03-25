/* Test for language tag decoding.
 *
 * Copyright (C) 2012 Reece H. Dunn
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

#include <cainteoir/languages.hpp>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "tester.hpp"

namespace lang = cainteoir::language;

REGISTER_TESTSUITE("Language Tags");

void compare(const lang::tag &a, const lang::tag &b)
{
	assert(a.lang    == b.lang);
	assert(a.script  == b.script);
	assert(a.region  == b.region);
	assert(a.variant == b.variant);
}

TEST_CASE("empty language code")
{
	compare(lang::make_lang(std::string()), { "" });
	compare(lang::make_lang(""), { "" });
}

TEST_CASE("language only codes")
{
	compare(lang::make_lang("en"), { "en" });
	compare(lang::make_lang("EN"), { "en" });
}

TEST_CASE("language-region codes")
{
	compare(lang::make_lang("en-US"), { "en", "", "US" });
	compare(lang::make_lang("EN-us"), { "en", "", "US" });
}

TEST_CASE("language-script codes")
{
	compare(lang::make_lang("zh-Hans"), { "zh", "Hans" });
	compare(lang::make_lang("zh-HANS"), { "zh", "Hans" });
	compare(lang::make_lang("ZH-hans"), { "zh", "Hans" });
}

TEST_CASE("language-script-region codes")
{
	compare(lang::make_lang("zh-Hant-CN"), { "zh", "Hant", "CN" });
	compare(lang::make_lang("zh-HANT-cn"), { "zh", "Hant", "CN" });
	compare(lang::make_lang("ZH-hant-CN"), { "zh", "Hant", "CN" });
}

TEST_CASE("language-region-script codes")
{
	compare(lang::make_lang("zh-CN-Hant"), { "zh", "Hant", "CN" });
	compare(lang::make_lang("zh-cn-HANT"), { "zh", "Hant", "CN" });
	compare(lang::make_lang("ZH-CN-hant"), { "zh", "Hant", "CN" });

	compare(lang::make_lang("es-419-Latn"), { "es", "Latn", "419" });
}

TEST_CASE("grandfathered language codes")
{
	compare(lang::make_lang("art-lojban"),  { "jbo" });
	compare(lang::make_lang("cel-gaulish"), { "cel-gaulish" });
	compare(lang::make_lang("en-GB-oed"),   { "en",  "", "GB" });
	compare(lang::make_lang("i-ami"),       { "ami" });
	compare(lang::make_lang("i-bnn"),       { "bnn" });
	compare(lang::make_lang("i-default"),   { "und" });
	compare(lang::make_lang("i-enochian"),  { "i-enochian" });
	compare(lang::make_lang("i-hak"),       { "hak" });
	compare(lang::make_lang("i-klingon"),   { "tlh" });
	compare(lang::make_lang("i-lux"),       { "lb" });
	compare(lang::make_lang("i-mingo"),     { "i-mingo" });
	compare(lang::make_lang("i-navajo"),    { "nv" });
	compare(lang::make_lang("i-pwn"),       { "pwn" });
	compare(lang::make_lang("i-tao"),       { "tao" });
	compare(lang::make_lang("i-tay"),       { "tay" });
	compare(lang::make_lang("i-tsu"),       { "tsu" });
	compare(lang::make_lang("no-bok"),      { "nb" });
	compare(lang::make_lang("no-nyn"),      { "nn" });
	compare(lang::make_lang("sgn-BE-FR"),   { "sfb" });
	compare(lang::make_lang("sgn-BE-NL"),   { "vgt" });
	compare(lang::make_lang("sgn-CH-DE"),   { "sgg" });
	compare(lang::make_lang("zh-guoyu"),    { "cmn" });
	compare(lang::make_lang("zh-hakka"),    { "hak" });
	compare(lang::make_lang("zh-min"),      { "nan" });
	compare(lang::make_lang("zh-min-nan"),  { "nan" });
	compare(lang::make_lang("zh-xiang"),    { "hsn" });
}

TEST_CASE("redundant language-extlang codes")
{
	compare(lang::make_lang("zh-cmn"), { "cmn" });
	compare(lang::make_lang("zh-hak"), { "hak" });
	compare(lang::make_lang("zh-nan"), { "nan" });
	compare(lang::make_lang("zh-yue"), { "yue" });
}

TEST_CASE("eSpeak voice language codes")
{
	compare(lang::make_lang("es-la"),       { "es", "", "419" });
	compare(lang::make_lang("en-sc"),       { "en", "", "", "scotland" });
	compare(lang::make_lang("en-uk"),       { "en", "", "GB" });
	compare(lang::make_lang("en-uk-north"), { "en", "", "GB" });
	compare(lang::make_lang("en-uk-rp"),    { "en", "", "GB" });
	compare(lang::make_lang("en-uk-wmids"), { "en", "", "GB" });
	compare(lang::make_lang("en-wi"),       { "en", "", "029" });
	compare(lang::make_lang("hy-west"),     { "hy" });
}

TEST_CASE("posix locale codes from /usr/share/locale")
{
	compare(lang::make_lang("be@latin"),    { "be", "Latn" });
	compare(lang::make_lang("ca@valencia"), { "ca", "", "", "valencia" });
	compare(lang::make_lang("en@boldquot"), { "en" });
	compare(lang::make_lang("en@quot"),     { "en" });
	compare(lang::make_lang("en@shaw"),     { "en" });
	compare(lang::make_lang("sr@ije"),      { "sr" });
	compare(lang::make_lang("sr@latin"),    { "sr", "Latn" });
	compare(lang::make_lang("sr@Latn"),     { "sr", "Latn" });
	compare(lang::make_lang("uz@cyrillic"), { "uz", "Cyrl" });
}

TEST_CASE("language tag equality")
{
	assert(lang::tag("en") == lang::tag("en"));
	assert(lang::tag("de") == lang::tag("de"));
	assert_false(lang::tag("af") == lang::tag("nl"));
}

TEST_CASE("language-region tag equality")
{
	assert(lang::tag("en", "", "GB") == lang::tag("en", "", "GB"));
	assert_false(lang::tag("en", "", "GB") == lang::tag("en", "", "US"));

	assert(lang::tag("en") == lang::tag("en", "", "US"));
	assert(lang::tag("en", "", "US") == lang::tag("en"));

	assert_false(lang::tag("es") == lang::tag("en", "", "GB"));
	assert_false(lang::tag("en", "", "GB") == lang::tag("es"));
}

TEST_CASE("language-script tag equality")
{
	assert(lang::tag("el", "Grek") == lang::tag("el", "Grek"));
	assert_false(lang::tag("el", "Grek") == lang::tag("el", "Latn"));

	assert(lang::tag("el") == lang::tag("el", "Grek"));
	assert(lang::tag("el", "Grek") == lang::tag("el"));

	assert_false(lang::tag("fr") == lang::tag("el", "Grek"));
	assert_false(lang::tag("el", "Grek") == lang::tag("fr"));
}

TEST_CASE("language-script-region tag equality")
{
	assert(lang::tag("zh", "Hans", "HK") == lang::tag("zh", "Hans", "HK"));
	assert_false(lang::tag("zh", "Hans", "HK") == lang::tag("zh", "Hant", "HK"));
	assert_false(lang::tag("zh", "Hans", "CN") == lang::tag("zh", "Hans", "HK"));

	assert(lang::tag("zh", "Hans", "HK") == lang::tag("zh", "Hans"));
	assert(lang::tag("zh", "Hans") == lang::tag("zh", "Hans", "HK"));

	assert(lang::tag("zh", "Hans", "HK") == lang::tag("zh"));
	assert(lang::tag("zh") == lang::tag("zh", "Hans", "HK"));
}

TEST_CASE("language-script-region-variant tag equality")
{
	assert(lang::tag("de", "Latn", "CH", "1901") == lang::tag("de", "Latn", "CH", "1901"));
	assert_false(lang::tag("de", "Latn", "CH", "1901") == lang::tag("de", "Latn", "CH", "scouse"));

	assert_false(lang::tag("de", "Latn", "DE", "1901") == lang::tag("de", "Latn", "CH", "1901"));
	assert_false(lang::tag("de", "Latf", "CH", "1901") == lang::tag("de", "Latn", "CH", "1901"));

	assert(lang::tag("de", "Latn", "CH", "1901") == lang::tag("de", "Latn", "CH"));
	assert(lang::tag("de", "Latn", "CH") == lang::tag("de", "Latn", "CH", "1901"));

	assert(lang::tag("de", "Latn", "CH", "1901") == lang::tag("de", "Latn"));
	assert(lang::tag("de", "Latn") == lang::tag("de", "Latn", "CH", "1901"));

	assert(lang::tag("de", "Latn", "CH", "1901") == lang::tag("de"));
	assert(lang::tag("de") == lang::tag("de", "Latn", "CH", "1901"));
}
