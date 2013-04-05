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
#include <cainteoir/metadata.hpp>
#include <cainteoir/document.hpp>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "tester.hpp"

namespace lang = cainteoir::language;
namespace rdf  = cainteoir::rdf;
namespace rql  = cainteoir::rdf::query;

REGISTER_TESTSUITE("Language Tags");

void compare(const lang::tag &a, const lang::tag &b)
{
	assert(a.lang    == b.lang);
	assert(a.extlang == b.extlang);
	assert(a.script  == b.script);
	assert(a.region  == b.region);
	assert(a.variant == b.variant);
	assert(a.private_use == b.private_use);
}

#define lang_match(a, b)    assert(      lang::make_lang(a) == lang::make_lang(b))
#define lang_mismatch(a, b) assert_false(lang::make_lang(a) == lang::make_lang(b))

void localized(const char *locale,
               const char *tag,
               const std::string &language,
               const std::string &script,
               const std::string &region,
               const std::string &display)
{
	if (!setlocale(LC_MESSAGES, locale) || !setenv("LANGUAGE", locale, 1) || !setenv("LANG", locale, 1))
	{
		printf("unable to set locale to '%s' ... skipping '%s' localization test\n", locale, tag);
		return;
	}

	static cainteoir::languages lookup;

	lang::tag l = lang::make_lang(tag);
	assert(lookup.language(l) == language);
	assert(lookup.script(l)   == script);
	assert(lookup.region(l)   == region);
	assert(lookup(tag)        == display);
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
	compare(lang::make_lang("en-US"), { "en", "", "", "US" });
	compare(lang::make_lang("EN-us"), { "en", "", "", "US" });
}

TEST_CASE("language-script codes")
{
	compare(lang::make_lang("zh-Hans"), { "zh", "", "Hans" });
	compare(lang::make_lang("zh-HANS"), { "zh", "", "Hans" });
	compare(lang::make_lang("ZH-hans"), { "zh", "", "Hans" });

	compare(lang::make_lang("jp-Kana"),           { "jp", "", "Kana" });
	compare(lang::make_lang("jp-Kana-Hans"),      { "jp", "", "Kana", "", "Hans" });
	compare(lang::make_lang("jp-Hira-Kana-Hans"), { "jp", "", "Hira", "", "Kana" });
}

TEST_CASE("language-script-region codes")
{
	compare(lang::make_lang("zh-Hant-CN"), { "zh", "", "Hant", "CN" });
	compare(lang::make_lang("zh-HANT-cn"), { "zh", "", "Hant", "CN" });
	compare(lang::make_lang("ZH-hant-CN"), { "zh", "", "Hant", "CN" });
}

TEST_CASE("language-region-script codes")
{
	compare(lang::make_lang("zh-CN-Hant"), { "zh", "", "Hant", "CN" });
	compare(lang::make_lang("zh-cn-HANT"), { "zh", "", "Hant", "CN" });
	compare(lang::make_lang("ZH-CN-hant"), { "zh", "", "Hant", "CN" });

	compare(lang::make_lang("es-419-Latn"), { "es", "", "Latn", "419" });
}

TEST_CASE("language-region-script-variant codes")
{
	compare(lang::make_lang("de-Latn-CH-1901"),   { "de", "", "Latn", "CH", "1901" });
	compare(lang::make_lang("de-Latn-CH-scouse"), { "de", "", "Latn", "CH", "scouse" });
}

TEST_CASE("language-x-privateuse codes")
{
	compare(lang::make_lang("en-x-test"), { "en", "", "", "", "", "test" });
	compare(lang::make_lang("EN-X-TEST"), { "en", "", "", "", "", "test" });
}

TEST_CASE("language-region-x-privateuse codes")
{
	compare(lang::make_lang("en-US-x-test"), { "en", "", "", "US", "", "test" });
	compare(lang::make_lang("EN-us-X-TEST"), { "en", "", "", "US", "", "test" });
}

TEST_CASE("language-script-x-privateuse codes")
{
	compare(lang::make_lang("zh-Hans-x-test"),     { "zh", "", "Hans", "", "", "test" });
	compare(lang::make_lang("zh-HANS-X-TEST"),     { "zh", "", "Hans", "", "", "test" });
	compare(lang::make_lang("ZH-hans-x-testcase"), { "zh", "", "Hans", "", "", "testcase" });

	compare(lang::make_lang("jp-Kana-x-test"),           { "jp", "", "Kana", "", "",     "test" });
	compare(lang::make_lang("jp-Kana-Hans-x-test"),      { "jp", "", "Kana", "", "Hans", "test" });
	compare(lang::make_lang("jp-Hira-Kana-Hans-x-test"), { "jp", "", "Hira", "", "Kana", "test" });
}

TEST_CASE("language-region-script-x-privateuse codes")
{
	compare(lang::make_lang("zh-CN-Hant-x-test"),     { "zh", "", "Hant", "CN", "", "test" });
	compare(lang::make_lang("zh-cn-HANT-x-test"),     { "zh", "", "Hant", "CN", "", "test" });
	compare(lang::make_lang("ZH-CN-hant-x-testcase"), { "zh", "", "Hant", "CN", "", "testcase" });

	compare(lang::make_lang("es-419-Latn-x-test"), { "es", "", "Latn", "419", "", "test" });
}

TEST_CASE("language-region-script-variant codes")
{
	compare(lang::make_lang("de-Latn-CH-1901-x-test"),   { "de", "", "Latn", "CH", "1901",   "test" });
	compare(lang::make_lang("de-Latn-CH-scouse-x-test"), { "de", "", "Latn", "CH", "scouse", "test" });
}

TEST_CASE("grandfathered language codes")
{
	compare(lang::make_lang("art-lojban"),  { "jbo" });
	compare(lang::make_lang("cel-gaulish"), { "cel-gaulish" });
	compare(lang::make_lang("en-GB-oed"),   { "en",  "", "", "GB" });
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
	compare(lang::make_lang("zh-guoyu"),    { "zh", "cmn" });
	compare(lang::make_lang("zh-hakka"),    { "zh", "hak" });
	compare(lang::make_lang("zh-min"),      { "zh", "nan" });
	compare(lang::make_lang("zh-min-nan"),  { "zh", "nan" });
	compare(lang::make_lang("zh-xiang"),    { "zh", "hsn" });
}

TEST_CASE("extlang codes in the IANA language subtag repository")
{
	rdf::graph data;
	auto reader = cainteoir::createDocumentReader("data/languages.rdf.gz", data, std::string());
	for (auto &statement : rql::select(data,
	                                   rql::predicate == rdf::rdf("type") &&
	                                   rql::object    == rdf::iana("ExtLang")))
	{
		rql::results ext = rql::select(data, rql::subject == rql::subject(statement));
		auto extlang  = rql::select_value<std::string>(ext, rql::predicate == rdf::rdf("value"));
		auto prefix   = rql::select_value<std::string>(ext, rql::predicate == rdf::iana("prefix"));
		auto language = prefix + "-" + extlang;

		printf("... ... testing extang %s-%s\n", prefix.c_str(), extlang.c_str());
		compare(lang::make_lang(extlang),  { prefix, extlang });
		compare(lang::make_lang(language), { prefix, extlang });
	}
}

TEST_CASE("posix locale codes from /usr/share/locale")
{
	compare(lang::make_lang("be@latin"),    { "be", "", "Latn" });
	compare(lang::make_lang("ca@valencia"), { "ca", "", "", "", "valencia" });
	compare(lang::make_lang("en@boldquot"), { "en" });
	compare(lang::make_lang("en@quot"),     { "en" });
	compare(lang::make_lang("en@shaw"),     { "en" });
	compare(lang::make_lang("sr@ije"),      { "sr" });
	compare(lang::make_lang("sr@latin"),    { "sr", "", "Latn" });
	compare(lang::make_lang("sr@Latn"),     { "sr", "", "Latn" });
	compare(lang::make_lang("uz@cyrillic"), { "uz", "", "Cyrl" });
}

TEST_CASE("language tag equality")
{
	lang_match(   "en", "en");
	lang_match(   "de", "de");
	lang_mismatch("af", "nl");
}

TEST_CASE("language-region tag equality")
{
	lang_match(   "en-GB", "en-GB");
	lang_mismatch("en-GB", "en-US");

	lang_match("en", "en-US");
	lang_match("en-US", "en");

	lang_mismatch("es", "en-GB");
	lang_mismatch("en-GB", "es");
}

TEST_CASE("language-script tag equality")
{
	lang_match(   "el-Grek", "el-Grek");
	lang_mismatch("el-Grek", "el-Latn");

	lang_match("el", "el-Grek");
	lang_match("el-Grek", "el");

	lang_mismatch("fr", "el-Grek");
	lang_mismatch("el-Grek", "fr");
}

TEST_CASE("language-script-region tag equality")
{
	lang_match(   "zh-Hans-HK", "zh-Hans-HK");
	lang_mismatch("zh-Hans-HK", "zh-Hant-HK");
	lang_mismatch("zh-Hans-CN", "zh-Hans-HK");

	lang_match("zh-Hans-HK", "zh-Hans");
	lang_match("zh-Hans", "zh-Hans-HK");

	lang_match("zh-Hans-HK", "zh");
	lang_match("zh", "zh-Hans-HK");
}

TEST_CASE("language-script-region-variant tag equality")
{
	lang_match(   "de-Latn-CH-1901",  "de-Latn-CH-1901");
	lang_mismatch("de-Latn-CH-1901",  "de-Latn-CH-scouse");
	lang_mismatch("de-Latn-DE-1901",  "de-Latn-CH-1901");
	lang_mismatch("de-Latf-CH-1901",  "de-Latn-CH-1901");
	lang_mismatch("gmh-Latn-CH-1901", "de-Latn-CH-1901");

	lang_match("de-Latn-CH-1901", "de-Latn-CH");
	lang_match("de-Latn-CH", "de-Latn-CH-1901");

	lang_match("de-Latn-CH-1901", "de-Latn");
	lang_match("de-Latn", "de-Latn-CH-1901");

	lang_match("de-Latn-CH-1901", "de");
	lang_match("de", "de-Latn-CH-1901");
}

TEST_CASE("language-*-region filter")
{
	compare(lang::make_lang("en-*-US"), { "en", "", "*", "US" });

	lang_mismatch("en-US", "en-Latn-US");

	lang_match("en-*-US", "en-Latn-US");
	lang_match("en-Latn-US", "en-*-US");
}

TEST_CASE("language localization")
{
	localized("C",          "en", "English",  "", "", "English");
	localized("en_GB.utf8", "en", "English",  "", "", "English");
	localized("en_US.utf8", "en", "English",  "", "", "English");
	localized("de_DE.utf8", "en", "Englisch", "", "", "Englisch");

	localized("en_US.utf8", "pt", "Portuguese",    "", "", "Portuguese");
	localized("de_DE.utf8", "pt", "Portugiesisch", "", "", "Portugiesisch");
}

TEST_CASE("language-extlang localization")
{
	localized("C",          "zh-cmn", "Mandarin Chinese", "", "", "Mandarin Chinese");
	localized("en_GB.utf8", "zh-cmn", "Mandarin Chinese", "", "", "Mandarin Chinese");
	localized("en_US.utf8", "zh-cmn", "Mandarin Chinese", "", "", "Mandarin Chinese");
	localized("de_DE.utf8", "zh-cmn", "Mandarin Chinese", "", "", "Mandarin Chinese");
}

TEST_CASE("language-script localization")
{
	localized("C",          "sga-Ogam", "Irish, Old (to 900)", "Ogham", "", "Irish, Old (to 900)");
	localized("en_GB.utf8", "sga-Ogam", "Irish, Old (to 900)", "Ogham", "", "Irish, Old (to 900)");
	localized("en_US.utf8", "sga-Ogam", "Irish, Old (to 900)", "Ogham", "", "Irish, Old (to 900)");
	localized("de_DE.utf8", "sga-Ogam", "Altirisch (bis 900)", "Ogham", "", "Altirisch (bis 900)");

	localized("en_US.utf8", "ja-Latn", "Japanese",  "Latin",      "", "Japanese");
	localized("de_DE.utf8", "ja-Latn", "Japanisch", "Lateinisch", "", "Japanisch");
}

TEST_CASE("language-region localization")
{
	localized("C",          "pt-PT", "Portuguese",    "", "Portugal", "Portuguese (Portugal)");
	localized("en_GB.utf8", "pt-PT", "Portuguese",    "", "Portugal", "Portuguese (Portugal)");
	localized("en_US.utf8", "pt-PT", "Portuguese",    "", "Portugal", "Portuguese (Portugal)");
	localized("de_DE.utf8", "pt-PT", "Portugiesisch", "", "Portugal", "Portugiesisch (Portugal)");

	localized("en_US.utf8", "pt-BR", "Portuguese",    "", "Brazil",    "Portuguese (Brazil)");
	localized("de_DE.utf8", "pt-BR", "Portugiesisch", "", "Brasilien", "Portugiesisch (Brasilien)");

	localized("en_US.utf8", "en-029", "English",  "", "Caribbean", "English (Caribbean)");
	localized("de_DE.utf8", "en-029", "Englisch", "", "Caribbean", "Englisch (Caribbean)");
}
