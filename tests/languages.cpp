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
	assert(a.extlang == b.extlang);
	assert(a.script  == b.script);
	assert(a.region  == b.region);
	assert(a.variant == b.variant);
}

#define lang_match(a, b)    assert(      lang::make_lang(a) == lang::make_lang(b))
#define lang_mismatch(a, b) assert_false(lang::make_lang(a) == lang::make_lang(b))

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

TEST_CASE("extlang codes for ar (Arabic)")
{
	compare(lang::make_lang("ar-aao"), { "ar", "aao" });
	compare(lang::make_lang("ar-abh"), { "ar", "abh" });
	compare(lang::make_lang("ar-abv"), { "ar", "abv" });
	compare(lang::make_lang("ar-acm"), { "ar", "acm" });
	compare(lang::make_lang("ar-acq"), { "ar", "acq" });
	compare(lang::make_lang("ar-acw"), { "ar", "acw" });
	compare(lang::make_lang("ar-acx"), { "ar", "acx" });
	compare(lang::make_lang("ar-acy"), { "ar", "acy" });
	compare(lang::make_lang("ar-adf"), { "ar", "adf" });
	compare(lang::make_lang("ar-aeb"), { "ar", "aeb" });
	compare(lang::make_lang("ar-aec"), { "ar", "aec" });
	compare(lang::make_lang("ar-afb"), { "ar", "afb" });
	compare(lang::make_lang("ar-ajp"), { "ar", "ajp" });
	compare(lang::make_lang("ar-apc"), { "ar", "apc" });
	compare(lang::make_lang("ar-apd"), { "ar", "apd" });
	compare(lang::make_lang("ar-arb"), { "ar", "arb" });
	compare(lang::make_lang("ar-arq"), { "ar", "arq" });
	compare(lang::make_lang("ar-ars"), { "ar", "ars" });
	compare(lang::make_lang("ar-ary"), { "ar", "ary" });
	compare(lang::make_lang("ar-arz"), { "ar", "arz" });
	compare(lang::make_lang("ar-auz"), { "ar", "auz" });
	compare(lang::make_lang("ar-avl"), { "ar", "avl" });
	compare(lang::make_lang("ar-ayh"), { "ar", "ayh" });
	compare(lang::make_lang("ar-ayl"), { "ar", "ayl" });
	compare(lang::make_lang("ar-ayn"), { "ar", "ayn" });
	compare(lang::make_lang("ar-ayp"), { "ar", "ayp" });
	compare(lang::make_lang("ar-bbz"), { "ar", "bbz" });
	compare(lang::make_lang("ar-pga"), { "ar", "pga" });
	compare(lang::make_lang("ar-shu"), { "ar", "shu" });
	compare(lang::make_lang("ar-ssh"), { "ar", "ssh" });

	compare(lang::make_lang("aao"), { "ar", "aao" });
	compare(lang::make_lang("abh"), { "ar", "abh" });
	compare(lang::make_lang("abv"), { "ar", "abv" });
	compare(lang::make_lang("acm"), { "ar", "acm" });
	compare(lang::make_lang("acq"), { "ar", "acq" });
	compare(lang::make_lang("acw"), { "ar", "acw" });
	compare(lang::make_lang("acx"), { "ar", "acx" });
	compare(lang::make_lang("acy"), { "ar", "acy" });
	compare(lang::make_lang("adf"), { "ar", "adf" });
	compare(lang::make_lang("aeb"), { "ar", "aeb" });
	compare(lang::make_lang("aec"), { "ar", "aec" });
	compare(lang::make_lang("afb"), { "ar", "afb" });
	compare(lang::make_lang("ajp"), { "ar", "ajp" });
	compare(lang::make_lang("apc"), { "ar", "apc" });
	compare(lang::make_lang("apd"), { "ar", "apd" });
	compare(lang::make_lang("arb"), { "ar", "arb" });
	compare(lang::make_lang("arq"), { "ar", "arq" });
	compare(lang::make_lang("ars"), { "ar", "ars" });
	compare(lang::make_lang("ary"), { "ar", "ary" });
	compare(lang::make_lang("arz"), { "ar", "arz" });
	compare(lang::make_lang("auz"), { "ar", "auz" });
	compare(lang::make_lang("avl"), { "ar", "avl" });
	compare(lang::make_lang("ayh"), { "ar", "ayh" });
	compare(lang::make_lang("ayl"), { "ar", "ayl" });
	compare(lang::make_lang("ayn"), { "ar", "ayn" });
	compare(lang::make_lang("ayp"), { "ar", "ayp" });
	compare(lang::make_lang("bbz"), { "ar", "bbz" });
	compare(lang::make_lang("pga"), { "ar", "pga" });
	compare(lang::make_lang("shu"), { "ar", "shu" });
	compare(lang::make_lang("ssh"), { "ar", "ssh" });
}

TEST_CASE("extlang codes for kok (Konkani)")
{
	compare(lang::make_lang("kok-gom"), { "kok", "gom" });
	compare(lang::make_lang("kok-knn"), { "kok", "knn" });

	compare(lang::make_lang("gom"), { "kok", "gom" });
	compare(lang::make_lang("knn"), { "kok", "knn" });
}

TEST_CASE("extlang codes for lv (Latvian)")
{
	compare(lang::make_lang("lv-ltg"), { "lv", "ltg" });
	compare(lang::make_lang("lv-lvs"), { "lv", "lvs" });

	compare(lang::make_lang("ltg"), { "lv", "ltg" });
	compare(lang::make_lang("lvs"), { "lv", "lvs" });
}

TEST_CASE("extlang codes for ms (Malay)")
{
	compare(lang::make_lang("ms-bjn"), { "ms", "bjn" });
	compare(lang::make_lang("ms-btj"), { "ms", "btj" });
	compare(lang::make_lang("ms-bve"), { "ms", "bve" });
	compare(lang::make_lang("ms-bvu"), { "ms", "bvu" });
	compare(lang::make_lang("ms-coa"), { "ms", "coa" });
	compare(lang::make_lang("ms-dup"), { "ms", "dup" });
	compare(lang::make_lang("ms-hji"), { "ms", "hji" });
	compare(lang::make_lang("ms-jak"), { "ms", "jak" });
	compare(lang::make_lang("ms-jax"), { "ms", "jax" });
	compare(lang::make_lang("ms-kvb"), { "ms", "kvb" });
	compare(lang::make_lang("ms-kvr"), { "ms", "kvr" });
	compare(lang::make_lang("ms-kxd"), { "ms", "kxd" });
	compare(lang::make_lang("ms-lce"), { "ms", "lce" });
	compare(lang::make_lang("ms-lcf"), { "ms", "lcf" });
	compare(lang::make_lang("ms-liw"), { "ms", "liw" });
	compare(lang::make_lang("ms-max"), { "ms", "max" });
	compare(lang::make_lang("ms-meo"), { "ms", "meo" });
	compare(lang::make_lang("ms-mfa"), { "ms", "mfa" });
	compare(lang::make_lang("ms-mfb"), { "ms", "mfb" });
	compare(lang::make_lang("ms-min"), { "ms", "min" });
	compare(lang::make_lang("ms-mqg"), { "ms", "mqg" });
	compare(lang::make_lang("ms-msi"), { "ms", "msi" });
	compare(lang::make_lang("ms-mui"), { "ms", "mui" });
	compare(lang::make_lang("ms-orn"), { "ms", "orn" });
	compare(lang::make_lang("ms-ors"), { "ms", "ors" });
	compare(lang::make_lang("ms-pel"), { "ms", "pel" });
	compare(lang::make_lang("ms-pse"), { "ms", "pse" });
	compare(lang::make_lang("ms-tmw"), { "ms", "tmw" });
	compare(lang::make_lang("ms-urk"), { "ms", "urk" });
	compare(lang::make_lang("ms-vkk"), { "ms", "vkk" });
	compare(lang::make_lang("ms-vkt"), { "ms", "vkt" });
	compare(lang::make_lang("ms-xmm"), { "ms", "xmm" });
	compare(lang::make_lang("ms-zlm"), { "ms", "zlm" });
	compare(lang::make_lang("ms-zmi"), { "ms", "zmi" });
	compare(lang::make_lang("ms-zsm"), { "ms", "zsm" });

	compare(lang::make_lang("bjn"), { "ms", "bjn" });
	compare(lang::make_lang("btj"), { "ms", "btj" });
	compare(lang::make_lang("bve"), { "ms", "bve" });
	compare(lang::make_lang("bvu"), { "ms", "bvu" });
	compare(lang::make_lang("coa"), { "ms", "coa" });
	compare(lang::make_lang("dup"), { "ms", "dup" });
	compare(lang::make_lang("hji"), { "ms", "hji" });
	compare(lang::make_lang("jak"), { "ms", "jak" });
	compare(lang::make_lang("jax"), { "ms", "jax" });
	compare(lang::make_lang("kvb"), { "ms", "kvb" });
	compare(lang::make_lang("kvr"), { "ms", "kvr" });
	compare(lang::make_lang("kxd"), { "ms", "kxd" });
	compare(lang::make_lang("lce"), { "ms", "lce" });
	compare(lang::make_lang("lcf"), { "ms", "lcf" });
	compare(lang::make_lang("liw"), { "ms", "liw" });
	compare(lang::make_lang("max"), { "ms", "max" });
	compare(lang::make_lang("meo"), { "ms", "meo" });
	compare(lang::make_lang("mfa"), { "ms", "mfa" });
	compare(lang::make_lang("mfb"), { "ms", "mfb" });
	compare(lang::make_lang("min"), { "ms", "min" });
	compare(lang::make_lang("mqg"), { "ms", "mqg" });
	compare(lang::make_lang("msi"), { "ms", "msi" });
	compare(lang::make_lang("mui"), { "ms", "mui" });
	compare(lang::make_lang("orn"), { "ms", "orn" });
	compare(lang::make_lang("ors"), { "ms", "ors" });
	compare(lang::make_lang("pel"), { "ms", "pel" });
	compare(lang::make_lang("pse"), { "ms", "pse" });
	compare(lang::make_lang("tmw"), { "ms", "tmw" });
	compare(lang::make_lang("urk"), { "ms", "urk" });
	compare(lang::make_lang("vkk"), { "ms", "vkk" });
	compare(lang::make_lang("vkt"), { "ms", "vkt" });
	compare(lang::make_lang("xmm"), { "ms", "xmm" });
	compare(lang::make_lang("zlm"), { "ms", "zlm" });
	compare(lang::make_lang("zmi"), { "ms", "zmi" });
	compare(lang::make_lang("zsm"), { "ms", "zsm" });
}

TEST_CASE("extlang codes for sgn (Sign Language)")
{
	compare(lang::make_lang("sgn-ads"), { "sgn", "ads" });
	compare(lang::make_lang("sgn-aed"), { "sgn", "aed" });
	compare(lang::make_lang("sgn-aen"), { "sgn", "aen" });
	compare(lang::make_lang("sgn-afg"), { "sgn", "afg" });
	compare(lang::make_lang("sgn-ase"), { "sgn", "ase" });
	compare(lang::make_lang("sgn-asf"), { "sgn", "asf" });
	compare(lang::make_lang("sgn-asp"), { "sgn", "asp" });
	compare(lang::make_lang("sgn-asq"), { "sgn", "asq" });
	compare(lang::make_lang("sgn-asw"), { "sgn", "asw" });
	compare(lang::make_lang("sgn-bfi"), { "sgn", "bfi" });
	compare(lang::make_lang("sgn-bfk"), { "sgn", "bfk" });
	compare(lang::make_lang("sgn-bog"), { "sgn", "bog" });
	compare(lang::make_lang("sgn-bqn"), { "sgn", "bqn" });
	compare(lang::make_lang("sgn-bqy"), { "sgn", "bqy" });
	compare(lang::make_lang("sgn-bvl"), { "sgn", "bvl" });
	compare(lang::make_lang("sgn-bzs"), { "sgn", "bzs" });
	compare(lang::make_lang("sgn-cds"), { "sgn", "cds" });
	compare(lang::make_lang("sgn-csc"), { "sgn", "csc" });
	compare(lang::make_lang("sgn-csd"), { "sgn", "csd" });
	compare(lang::make_lang("sgn-cse"), { "sgn", "cse" });
	compare(lang::make_lang("sgn-csf"), { "sgn", "csf" });
	compare(lang::make_lang("sgn-csg"), { "sgn", "csg" });
	compare(lang::make_lang("sgn-csl"), { "sgn", "csl" });
	compare(lang::make_lang("sgn-csn"), { "sgn", "csn" });
	compare(lang::make_lang("sgn-csq"), { "sgn", "csq" });
	compare(lang::make_lang("sgn-csr"), { "sgn", "csr" });
	compare(lang::make_lang("sgn-doq"), { "sgn", "doq" });
	compare(lang::make_lang("sgn-dse"), { "sgn", "dse" });
	compare(lang::make_lang("sgn-dsl"), { "sgn", "dsl" });
	compare(lang::make_lang("sgn-ecs"), { "sgn", "ecs" });
	compare(lang::make_lang("sgn-esl"), { "sgn", "esl" });
	compare(lang::make_lang("sgn-esn"), { "sgn", "esn" });
	compare(lang::make_lang("sgn-eso"), { "sgn", "eso" });
	compare(lang::make_lang("sgn-eth"), { "sgn", "eth" });
	compare(lang::make_lang("sgn-fcs"), { "sgn", "fcs" });
	compare(lang::make_lang("sgn-fse"), { "sgn", "fse" });
	compare(lang::make_lang("sgn-fsl"), { "sgn", "fsl" });
	compare(lang::make_lang("sgn-fss"), { "sgn", "fss" });
	compare(lang::make_lang("sgn-gse"), { "sgn", "gse" });
	compare(lang::make_lang("sgn-gsg"), { "sgn", "gsg" });
	compare(lang::make_lang("sgn-gsm"), { "sgn", "gsm" });
	compare(lang::make_lang("sgn-gss"), { "sgn", "gss" });
	compare(lang::make_lang("sgn-gus"), { "sgn", "gus" });
	compare(lang::make_lang("sgn-hab"), { "sgn", "hab" });
	compare(lang::make_lang("sgn-haf"), { "sgn", "haf" });
	compare(lang::make_lang("sgn-hds"), { "sgn", "hds" });
	compare(lang::make_lang("sgn-hks"), { "sgn", "hks" });
	compare(lang::make_lang("sgn-hos"), { "sgn", "hos" });
	compare(lang::make_lang("sgn-hps"), { "sgn", "hps" });
	compare(lang::make_lang("sgn-hsh"), { "sgn", "hsh" });
	compare(lang::make_lang("sgn-hsl"), { "sgn", "hsl" });
	compare(lang::make_lang("sgn-icl"), { "sgn", "icl" });
	compare(lang::make_lang("sgn-ils"), { "sgn", "ils" });
	compare(lang::make_lang("sgn-inl"), { "sgn", "inl" });
	compare(lang::make_lang("sgn-ins"), { "sgn", "ins" });
	compare(lang::make_lang("sgn-ise"), { "sgn", "ise" });
	compare(lang::make_lang("sgn-isg"), { "sgn", "isg" });
	compare(lang::make_lang("sgn-isr"), { "sgn", "isr" });
	compare(lang::make_lang("sgn-jcs"), { "sgn", "jcs" });
	compare(lang::make_lang("sgn-jhs"), { "sgn", "jhs" });
	compare(lang::make_lang("sgn-jls"), { "sgn", "jls" });
	compare(lang::make_lang("sgn-jos"), { "sgn", "jos" });
	compare(lang::make_lang("sgn-jsl"), { "sgn", "jsl" });
	compare(lang::make_lang("sgn-jus"), { "sgn", "jus" });
	compare(lang::make_lang("sgn-kgi"), { "sgn", "kgi" });
	compare(lang::make_lang("sgn-kvk"), { "sgn", "kvk" });
	compare(lang::make_lang("sgn-lbs"), { "sgn", "lbs" });
	compare(lang::make_lang("sgn-lls"), { "sgn", "lls" });
	compare(lang::make_lang("sgn-lsg"), { "sgn", "lsg" });
	compare(lang::make_lang("sgn-lsl"), { "sgn", "lsl" });
	compare(lang::make_lang("sgn-lso"), { "sgn", "lso" });
	compare(lang::make_lang("sgn-lsp"), { "sgn", "lsp" });
	compare(lang::make_lang("sgn-lst"), { "sgn", "lst" });
	compare(lang::make_lang("sgn-lsy"), { "sgn", "lsy" });
	compare(lang::make_lang("sgn-mdl"), { "sgn", "mdl" });
	compare(lang::make_lang("sgn-mfs"), { "sgn", "mfs" });
	compare(lang::make_lang("sgn-mre"), { "sgn", "mre" });
	compare(lang::make_lang("sgn-msd"), { "sgn", "msd" });
	compare(lang::make_lang("sgn-msr"), { "sgn", "msr" });
	compare(lang::make_lang("sgn-mzc"), { "sgn", "mzc" });
	compare(lang::make_lang("sgn-mzg"), { "sgn", "mzg" });
	compare(lang::make_lang("sgn-mzy"), { "sgn", "mzy" });
	compare(lang::make_lang("sgn-nbs"), { "sgn", "nbs" });
	compare(lang::make_lang("sgn-ncs"), { "sgn", "ncs" });
	compare(lang::make_lang("sgn-nsi"), { "sgn", "nsi" });
	compare(lang::make_lang("sgn-nsl"), { "sgn", "nsl" });
	compare(lang::make_lang("sgn-nsp"), { "sgn", "nsp" });
	compare(lang::make_lang("sgn-nsr"), { "sgn", "nsr" });
	compare(lang::make_lang("sgn-nzs"), { "sgn", "nzs" });
	compare(lang::make_lang("sgn-okl"), { "sgn", "okl" });
	compare(lang::make_lang("sgn-pks"), { "sgn", "pks" });
	compare(lang::make_lang("sgn-prl"), { "sgn", "prl" });
	compare(lang::make_lang("sgn-prz"), { "sgn", "prz" });
	compare(lang::make_lang("sgn-psc"), { "sgn", "psc" });
	compare(lang::make_lang("sgn-psd"), { "sgn", "psd" });
	compare(lang::make_lang("sgn-psg"), { "sgn", "psg" });
	compare(lang::make_lang("sgn-psl"), { "sgn", "psl" });
	compare(lang::make_lang("sgn-pso"), { "sgn", "pso" });
	compare(lang::make_lang("sgn-psp"), { "sgn", "psp" });
	compare(lang::make_lang("sgn-psr"), { "sgn", "psr" });
	compare(lang::make_lang("sgn-pys"), { "sgn", "pys" });
	compare(lang::make_lang("sgn-rms"), { "sgn", "rms" });
	compare(lang::make_lang("sgn-rsi"), { "sgn", "rsi" });
	compare(lang::make_lang("sgn-rsl"), { "sgn", "rsl" });
	compare(lang::make_lang("sgn-sdl"), { "sgn", "sdl" });
	compare(lang::make_lang("sgn-sfb"), { "sgn", "sfb" });
	compare(lang::make_lang("sgn-sfs"), { "sgn", "sfs" });
	compare(lang::make_lang("sgn-sgg"), { "sgn", "sgg" });
	compare(lang::make_lang("sgn-sgx"), { "sgn", "sgx" });
	compare(lang::make_lang("sgn-slf"), { "sgn", "slf" });
	compare(lang::make_lang("sgn-sls"), { "sgn", "sls" });
	compare(lang::make_lang("sgn-sqs"), { "sgn", "sqs" });
	compare(lang::make_lang("sgn-ssp"), { "sgn", "ssp" });
	compare(lang::make_lang("sgn-ssr"), { "sgn", "ssr" });
	compare(lang::make_lang("sgn-svk"), { "sgn", "svk" });
	compare(lang::make_lang("sgn-swl"), { "sgn", "swl" });
	compare(lang::make_lang("sgn-syy"), { "sgn", "syy" });
	compare(lang::make_lang("sgn-tse"), { "sgn", "tse" });
	compare(lang::make_lang("sgn-tsm"), { "sgn", "tsm" });
	compare(lang::make_lang("sgn-tsq"), { "sgn", "tsq" });
	compare(lang::make_lang("sgn-tss"), { "sgn", "tss" });
	compare(lang::make_lang("sgn-tsy"), { "sgn", "tsy" });
	compare(lang::make_lang("sgn-tza"), { "sgn", "tza" });
	compare(lang::make_lang("sgn-ugn"), { "sgn", "ugn" });
	compare(lang::make_lang("sgn-ugy"), { "sgn", "ugy" });
	compare(lang::make_lang("sgn-ukl"), { "sgn", "ukl" });
	compare(lang::make_lang("sgn-uks"), { "sgn", "uks" });
	compare(lang::make_lang("sgn-vgt"), { "sgn", "vgt" });
	compare(lang::make_lang("sgn-vsi"), { "sgn", "vsi" });
	compare(lang::make_lang("sgn-vsl"), { "sgn", "vsl" });
	compare(lang::make_lang("sgn-vsv"), { "sgn", "vsv" });
	compare(lang::make_lang("sgn-xki"), { "sgn", "xki" });
	compare(lang::make_lang("sgn-xml"), { "sgn", "xml" });
	compare(lang::make_lang("sgn-xms"), { "sgn", "xms" });
	compare(lang::make_lang("sgn-yds"), { "sgn", "yds" });
	compare(lang::make_lang("sgn-ysl"), { "sgn", "ysl" });
	compare(lang::make_lang("sgn-zib"), { "sgn", "zib" });
	compare(lang::make_lang("sgn-zsl"), { "sgn", "zsl" });

	compare(lang::make_lang("ads"), { "sgn", "ads" });
	compare(lang::make_lang("aed"), { "sgn", "aed" });
	compare(lang::make_lang("aen"), { "sgn", "aen" });
	compare(lang::make_lang("afg"), { "sgn", "afg" });
	compare(lang::make_lang("ase"), { "sgn", "ase" });
	compare(lang::make_lang("asf"), { "sgn", "asf" });
	compare(lang::make_lang("asp"), { "sgn", "asp" });
	compare(lang::make_lang("asq"), { "sgn", "asq" });
	compare(lang::make_lang("asw"), { "sgn", "asw" });
	compare(lang::make_lang("bfi"), { "sgn", "bfi" });
	compare(lang::make_lang("bfk"), { "sgn", "bfk" });
	compare(lang::make_lang("bog"), { "sgn", "bog" });
	compare(lang::make_lang("bqn"), { "sgn", "bqn" });
	compare(lang::make_lang("bqy"), { "sgn", "bqy" });
	compare(lang::make_lang("bvl"), { "sgn", "bvl" });
	compare(lang::make_lang("bzs"), { "sgn", "bzs" });
	compare(lang::make_lang("cds"), { "sgn", "cds" });
	compare(lang::make_lang("csc"), { "sgn", "csc" });
	compare(lang::make_lang("csd"), { "sgn", "csd" });
	compare(lang::make_lang("cse"), { "sgn", "cse" });
	compare(lang::make_lang("csf"), { "sgn", "csf" });
	compare(lang::make_lang("csg"), { "sgn", "csg" });
	compare(lang::make_lang("csl"), { "sgn", "csl" });
	compare(lang::make_lang("csn"), { "sgn", "csn" });
	compare(lang::make_lang("csq"), { "sgn", "csq" });
	compare(lang::make_lang("csr"), { "sgn", "csr" });
	compare(lang::make_lang("doq"), { "sgn", "doq" });
	compare(lang::make_lang("dse"), { "sgn", "dse" });
	compare(lang::make_lang("dsl"), { "sgn", "dsl" });
	compare(lang::make_lang("ecs"), { "sgn", "ecs" });
	compare(lang::make_lang("esl"), { "sgn", "esl" });
	compare(lang::make_lang("esn"), { "sgn", "esn" });
	compare(lang::make_lang("eso"), { "sgn", "eso" });
	compare(lang::make_lang("eth"), { "sgn", "eth" });
	compare(lang::make_lang("fcs"), { "sgn", "fcs" });
	compare(lang::make_lang("fse"), { "sgn", "fse" });
	compare(lang::make_lang("fsl"), { "sgn", "fsl" });
	compare(lang::make_lang("fss"), { "sgn", "fss" });
	compare(lang::make_lang("gse"), { "sgn", "gse" });
	compare(lang::make_lang("gsg"), { "sgn", "gsg" });
	compare(lang::make_lang("gsm"), { "sgn", "gsm" });
	compare(lang::make_lang("gss"), { "sgn", "gss" });
	compare(lang::make_lang("gus"), { "sgn", "gus" });
	compare(lang::make_lang("hab"), { "sgn", "hab" });
	compare(lang::make_lang("haf"), { "sgn", "haf" });
	compare(lang::make_lang("hds"), { "sgn", "hds" });
	compare(lang::make_lang("hks"), { "sgn", "hks" });
	compare(lang::make_lang("hos"), { "sgn", "hos" });
	compare(lang::make_lang("hps"), { "sgn", "hps" });
	compare(lang::make_lang("hsh"), { "sgn", "hsh" });
	compare(lang::make_lang("hsl"), { "sgn", "hsl" });
	compare(lang::make_lang("icl"), { "sgn", "icl" });
	compare(lang::make_lang("ils"), { "sgn", "ils" });
	compare(lang::make_lang("inl"), { "sgn", "inl" });
	compare(lang::make_lang("ins"), { "sgn", "ins" });
	compare(lang::make_lang("ise"), { "sgn", "ise" });
	compare(lang::make_lang("isg"), { "sgn", "isg" });
	compare(lang::make_lang("isr"), { "sgn", "isr" });
	compare(lang::make_lang("jcs"), { "sgn", "jcs" });
	compare(lang::make_lang("jhs"), { "sgn", "jhs" });
	compare(lang::make_lang("jls"), { "sgn", "jls" });
	compare(lang::make_lang("jos"), { "sgn", "jos" });
	compare(lang::make_lang("jsl"), { "sgn", "jsl" });
	compare(lang::make_lang("jus"), { "sgn", "jus" });
	compare(lang::make_lang("kgi"), { "sgn", "kgi" });
	compare(lang::make_lang("kvk"), { "sgn", "kvk" });
	compare(lang::make_lang("lbs"), { "sgn", "lbs" });
	compare(lang::make_lang("lls"), { "sgn", "lls" });
	compare(lang::make_lang("lsg"), { "sgn", "lsg" });
	compare(lang::make_lang("lsl"), { "sgn", "lsl" });
	compare(lang::make_lang("lso"), { "sgn", "lso" });
	compare(lang::make_lang("lsp"), { "sgn", "lsp" });
	compare(lang::make_lang("lst"), { "sgn", "lst" });
	compare(lang::make_lang("lsy"), { "sgn", "lsy" });
	compare(lang::make_lang("mdl"), { "sgn", "mdl" });
	compare(lang::make_lang("mfs"), { "sgn", "mfs" });
	compare(lang::make_lang("mre"), { "sgn", "mre" });
	compare(lang::make_lang("msd"), { "sgn", "msd" });
	compare(lang::make_lang("msr"), { "sgn", "msr" });
	compare(lang::make_lang("mzc"), { "sgn", "mzc" });
	compare(lang::make_lang("mzg"), { "sgn", "mzg" });
	compare(lang::make_lang("mzy"), { "sgn", "mzy" });
	compare(lang::make_lang("nbs"), { "sgn", "nbs" });
	compare(lang::make_lang("ncs"), { "sgn", "ncs" });
	compare(lang::make_lang("nsi"), { "sgn", "nsi" });
	compare(lang::make_lang("nsl"), { "sgn", "nsl" });
	compare(lang::make_lang("nsp"), { "sgn", "nsp" });
	compare(lang::make_lang("nsr"), { "sgn", "nsr" });
	compare(lang::make_lang("nzs"), { "sgn", "nzs" });
	compare(lang::make_lang("okl"), { "sgn", "okl" });
	compare(lang::make_lang("pks"), { "sgn", "pks" });
	compare(lang::make_lang("prl"), { "sgn", "prl" });
	compare(lang::make_lang("prz"), { "sgn", "prz" });
	compare(lang::make_lang("psc"), { "sgn", "psc" });
	compare(lang::make_lang("psd"), { "sgn", "psd" });
	compare(lang::make_lang("psg"), { "sgn", "psg" });
	compare(lang::make_lang("psl"), { "sgn", "psl" });
	compare(lang::make_lang("pso"), { "sgn", "pso" });
	compare(lang::make_lang("psp"), { "sgn", "psp" });
	compare(lang::make_lang("psr"), { "sgn", "psr" });
	compare(lang::make_lang("pys"), { "sgn", "pys" });
	compare(lang::make_lang("rms"), { "sgn", "rms" });
	compare(lang::make_lang("rsi"), { "sgn", "rsi" });
	compare(lang::make_lang("rsl"), { "sgn", "rsl" });
	compare(lang::make_lang("sdl"), { "sgn", "sdl" });
	compare(lang::make_lang("sfb"), { "sgn", "sfb" });
	compare(lang::make_lang("sfs"), { "sgn", "sfs" });
	compare(lang::make_lang("sgg"), { "sgn", "sgg" });
	compare(lang::make_lang("sgx"), { "sgn", "sgx" });
	compare(lang::make_lang("slf"), { "sgn", "slf" });
	compare(lang::make_lang("sls"), { "sgn", "sls" });
	compare(lang::make_lang("sqs"), { "sgn", "sqs" });
	compare(lang::make_lang("ssp"), { "sgn", "ssp" });
	compare(lang::make_lang("ssr"), { "sgn", "ssr" });
	compare(lang::make_lang("svk"), { "sgn", "svk" });
	compare(lang::make_lang("swl"), { "sgn", "swl" });
	compare(lang::make_lang("syy"), { "sgn", "syy" });
	compare(lang::make_lang("tse"), { "sgn", "tse" });
	compare(lang::make_lang("tsm"), { "sgn", "tsm" });
	compare(lang::make_lang("tsq"), { "sgn", "tsq" });
	compare(lang::make_lang("tss"), { "sgn", "tss" });
	compare(lang::make_lang("tsy"), { "sgn", "tsy" });
	compare(lang::make_lang("tza"), { "sgn", "tza" });
	compare(lang::make_lang("ugn"), { "sgn", "ugn" });
	compare(lang::make_lang("ugy"), { "sgn", "ugy" });
	compare(lang::make_lang("ukl"), { "sgn", "ukl" });
	compare(lang::make_lang("uks"), { "sgn", "uks" });
	compare(lang::make_lang("vgt"), { "sgn", "vgt" });
	compare(lang::make_lang("vsi"), { "sgn", "vsi" });
	compare(lang::make_lang("vsl"), { "sgn", "vsl" });
	compare(lang::make_lang("vsv"), { "sgn", "vsv" });
	compare(lang::make_lang("xki"), { "sgn", "xki" });
	compare(lang::make_lang("xml"), { "sgn", "xml" });
	compare(lang::make_lang("xms"), { "sgn", "xms" });
	compare(lang::make_lang("yds"), { "sgn", "yds" });
	compare(lang::make_lang("ysl"), { "sgn", "ysl" });
	compare(lang::make_lang("zib"), { "sgn", "zib" });
	compare(lang::make_lang("zsl"), { "sgn", "zsl" });
}

TEST_CASE("extlang codes for sw (Swahili)")
{
	compare(lang::make_lang("sw-swc"), { "sw", "swc" });
	compare(lang::make_lang("sw-swh"), { "sw", "swh" });

	compare(lang::make_lang("swc"), { "sw", "swc" });
	compare(lang::make_lang("swh"), { "sw", "swh" });
}

TEST_CASE("extlang codes for uz (Uzbek)")
{
	compare(lang::make_lang("uz-uzn"), { "uz", "uzn" });
	compare(lang::make_lang("uz-uzs"), { "uz", "uzs" });

	compare(lang::make_lang("uzn"), { "uz", "uzn" });
	compare(lang::make_lang("uzs"), { "uz", "uzs" });
}

TEST_CASE("extlang codes for zh (Chinese)")
{
	compare(lang::make_lang("zh-cdo"), { "zh", "cdo" });
	compare(lang::make_lang("zh-cjy"), { "zh", "cjy" });
	compare(lang::make_lang("zh-cmn"), { "zh", "cmn" });
	compare(lang::make_lang("zh-cpx"), { "zh", "cpx" });
	compare(lang::make_lang("zh-czh"), { "zh", "czh" });
	compare(lang::make_lang("zh-czo"), { "zh", "czo" });
	compare(lang::make_lang("zh-gan"), { "zh", "gan" });
	compare(lang::make_lang("zh-hak"), { "zh", "hak" });
	compare(lang::make_lang("zh-hsn"), { "zh", "hsn" });
	compare(lang::make_lang("zh-lzh"), { "zh", "lzh" });
	compare(lang::make_lang("zh-mnp"), { "zh", "mnp" });
	compare(lang::make_lang("zh-nan"), { "zh", "nan" });
	compare(lang::make_lang("zh-wuu"), { "zh", "wuu" });
	compare(lang::make_lang("zh-yue"), { "zh", "yue" });

	compare(lang::make_lang("zh-cdo-TW"),      { "zh", "cdo", "", "TW" });
	compare(lang::make_lang("zh-cjy-CN"),      { "zh", "cjy", "", "CN" });
	compare(lang::make_lang("zh-cmn-Hans"),    { "zh", "cmn", "Hans" });
	compare(lang::make_lang("zh-cpx-Hant"),    { "zh", "cpx", "Hant" });
	compare(lang::make_lang("zh-czh-tw"),      { "zh", "czh", "", "TW" });
	compare(lang::make_lang("zh-czo-cn"),      { "zh", "czo", "", "CN" });
	compare(lang::make_lang("zh-gan-hant"),    { "zh", "gan", "Hant" });
	compare(lang::make_lang("zh-hak-hans"),    { "zh", "hak", "Hans" });
	compare(lang::make_lang("zh-hsn-Tw"),      { "zh", "hsn", "", "TW" });
	compare(lang::make_lang("zh-lzh-Cn"),      { "zh", "lzh", "", "CN" });
	compare(lang::make_lang("zh-mnp-HANT"),    { "zh", "mnp", "Hant" });
	compare(lang::make_lang("zh-nan-HANS"),    { "zh", "nan", "Hans" });
	compare(lang::make_lang("zh-wuu-Hans-TW"), { "zh", "wuu", "Hans", "TW" });
	compare(lang::make_lang("zh-yue-Hant-cN"), { "zh", "yue", "Hant", "CN" });

	compare(lang::make_lang("cdo"), { "zh", "cdo" });
	compare(lang::make_lang("cjy"), { "zh", "cjy" });
	compare(lang::make_lang("cmn"), { "zh", "cmn" });
	compare(lang::make_lang("cpx"), { "zh", "cpx" });
	compare(lang::make_lang("czh"), { "zh", "czh" });
	compare(lang::make_lang("czo"), { "zh", "czo" });
	compare(lang::make_lang("gan"), { "zh", "gan" });
	compare(lang::make_lang("hak"), { "zh", "hak" });
	compare(lang::make_lang("hsn"), { "zh", "hsn" });
	compare(lang::make_lang("lzh"), { "zh", "lzh" });
	compare(lang::make_lang("mnp"), { "zh", "mnp" });
	compare(lang::make_lang("nan"), { "zh", "nan" });
	compare(lang::make_lang("wuu"), { "zh", "wuu" });
	compare(lang::make_lang("yue"), { "zh", "yue" });

	compare(lang::make_lang("cdo-TW"),      { "zh", "cdo", "", "TW" });
	compare(lang::make_lang("cjy-CN"),      { "zh", "cjy", "", "CN" });
	compare(lang::make_lang("cmn-Hans"),    { "zh", "cmn", "Hans" });
	compare(lang::make_lang("cpx-Hant"),    { "zh", "cpx", "Hant" });
	compare(lang::make_lang("czh-tw"),      { "zh", "czh", "", "TW" });
	compare(lang::make_lang("czo-cn"),      { "zh", "czo", "", "CN" });
	compare(lang::make_lang("gan-hant"),    { "zh", "gan", "Hant" });
	compare(lang::make_lang("hak-hans"),    { "zh", "hak", "Hans" });
	compare(lang::make_lang("hsn-Tw"),      { "zh", "hsn", "", "TW" });
	compare(lang::make_lang("lzh-Cn"),      { "zh", "lzh", "", "CN" });
	compare(lang::make_lang("mnp-HANT"),    { "zh", "mnp", "Hant" });
	compare(lang::make_lang("nan-HANS"),    { "zh", "nan", "Hans" });
	compare(lang::make_lang("wuu-Hans-TW"), { "zh", "wuu", "Hans", "TW" });
	compare(lang::make_lang("yue-Hant-cN"), { "zh", "yue", "Hant", "CN" });
}

TEST_CASE("eSpeak voice language codes")
{
	compare(lang::make_lang("es-la"),       { "es", "", "", "419" });
	compare(lang::make_lang("en-sc"),       { "en", "", "", "", "scotland" });
	compare(lang::make_lang("en-uk"),       { "en", "", "", "GB" });
	compare(lang::make_lang("en-uk-north"), { "en", "", "", "GB" });
	compare(lang::make_lang("en-uk-rp"),    { "en", "", "", "GB" });
	compare(lang::make_lang("en-uk-wmids"), { "en", "", "", "GB" });
	compare(lang::make_lang("en-wi"),       { "en", "", "", "029" });
	compare(lang::make_lang("hy-west"),     { "hy" });
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
	lang_match(   "en-GB", "en-UK");
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
	//lang_mismatch("de-Latn-CH-1901",  "de-Latn-CH-scouse");
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
