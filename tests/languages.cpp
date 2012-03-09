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
	equal(a.lang,    b.lang);
	equal(a.script,  b.script);
	equal(a.region,  b.region);
	equal(a.variant, b.variant);
}

TEST_CASE("language")
{
	compare(lang::make_lang("en"), { "en" });
	compare(lang::make_lang("EN"), { "en" });
}

TEST_CASE("language-region")
{
	compare(lang::make_lang("en-US"), { "en", "", "US" });
	compare(lang::make_lang("EN-us"), { "en", "", "US" });
}

TEST_CASE("language-script")
{
	compare(lang::make_lang("zh-Hans"), { "zh", "Hans" });
	compare(lang::make_lang("zh-HANS"), { "zh", "Hans" });
	compare(lang::make_lang("ZH-hans"), { "zh", "Hans" });
}

TEST_CASE("language-script-region")
{
	compare(lang::make_lang("zh-Hant-CN"), { "zh", "Hant", "CN" });
	compare(lang::make_lang("zh-HANT-cn"), { "zh", "Hant", "CN" });
	compare(lang::make_lang("ZH-hant-CN"), { "zh", "Hant", "CN" });
}

TEST_CASE("grandfathered")
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

TEST_CASE("redundant")
{
	compare(lang::make_lang("zh-cmn"), { "cmn" });
	compare(lang::make_lang("zh-hak"), { "hak" });
	compare(lang::make_lang("zh-nan"), { "nan" });
	compare(lang::make_lang("zh-yue"), { "yue" });
}

TEST_CASE("eSpeak") // language codes used by eSpeak voices ...
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
