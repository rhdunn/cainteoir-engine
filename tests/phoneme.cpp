/* Test for the phoneme model.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#include <cainteoir/phoneme.hpp>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("phoneme");

namespace tts = cainteoir::tts;

typedef tts::feature f;

TEST_CASE("phoneme object size")
{
	assert(sizeof(tts::phoneme) == 5);
}

TEST_CASE("3 feature phoneme")
{
	tts::phoneme p(f::voiceless, f::bilabial, f::plosive);

	assert(p.contains(f::voiceless));
	assert(p.contains(f::bilabial));
	assert(p.contains(f::plosive));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::voiced));
	assert(!p.contains(f::vowel));

	// copy construction ...

	tts::phoneme q(p);

	assert(q.contains(f::voiceless));
	assert(q.contains(f::bilabial));
	assert(q.contains(f::plosive));

	assert(!q.contains(f::unspecified));
	assert(!q.contains(f::voiced));
	assert(!q.contains(f::vowel));

	// copy assignment ...

	tts::phoneme a(f::voiced, f::dental, f::fricative);
	a = tts::phoneme(f::voiceless, f::bilabial, f::plosive);

	assert(a.contains(f::voiceless));
	assert(a.contains(f::bilabial));
	assert(a.contains(f::plosive));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::voiced));
	assert(!a.contains(f::vowel));

	a = { f::voiced, f::dental, f::fricative };

	assert(a.contains(f::voiced));
	assert(a.contains(f::dental));
	assert(a.contains(f::fricative));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::voiceless));
	assert(!a.contains(f::vowel));
}

TEST_CASE("4 feature phoneme")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::rhoticized));

	// copy construction ...

	tts::phoneme q(p);

	assert(q.contains(f::high));
	assert(q.contains(f::front));
	assert(q.contains(f::unrounded));
	assert(q.contains(f::vowel));

	assert(!q.contains(f::unspecified));
	assert(!q.contains(f::rounded));
	assert(!q.contains(f::voiceless));
	assert(!q.contains(f::rhoticized));

	// copy assignment ...

	tts::phoneme a(f::voiced, f::bilabial, f::lateral, f::fricative);
	a = tts::phoneme(f::high, f::front, f::unrounded, f::vowel);

	assert(a.contains(f::high));
	assert(a.contains(f::front));
	assert(a.contains(f::unrounded));
	assert(a.contains(f::vowel));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::rounded));
	assert(!a.contains(f::voiceless));
	assert(!a.contains(f::rhoticized));

	a = { f::low, f::back, f::rounded, f::vowel };

	assert(a.contains(f::low));
	assert(a.contains(f::back));
	assert(a.contains(f::rounded));
	assert(a.contains(f::vowel));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::unrounded));
	assert(!a.contains(f::voiceless));
	assert(!a.contains(f::rhoticized));
}

TEST_CASE("5 feature phoneme")
{
	tts::phoneme p(f::high, f::front, f::unrounded, f::vowel, f::rhoticized);

	assert(p.contains(f::high));
	assert(p.contains(f::front));
	assert(p.contains(f::unrounded));
	assert(p.contains(f::vowel));
	assert(p.contains(f::rhoticized));

	assert(!p.contains(f::unspecified));
	assert(!p.contains(f::rounded));
	assert(!p.contains(f::voiceless));
	assert(!p.contains(f::long_));

	// copy construction ...

	tts::phoneme q(p);

	assert(q.contains(f::high));
	assert(q.contains(f::front));
	assert(q.contains(f::unrounded));
	assert(q.contains(f::vowel));

	assert(!q.contains(f::unspecified));
	assert(!q.contains(f::rounded));
	assert(!q.contains(f::voiceless));
	assert(!q.contains(f::long_));

	// copy assignment ...

	tts::phoneme a(f::voiced, f::bilabial, f::lateral, f::fricative, f::murmured);
	a = tts::phoneme(f::high, f::front, f::unrounded, f::vowel, f::long_);

	assert(a.contains(f::high));
	assert(a.contains(f::front));
	assert(a.contains(f::unrounded));
	assert(a.contains(f::vowel));
	assert(a.contains(f::long_));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::rounded));
	assert(!a.contains(f::voiceless));
	assert(!a.contains(f::rhoticized));

	a = { f::low, f::back, f::rounded, f::vowel, f::rhoticized };

	assert(a.contains(f::low));
	assert(a.contains(f::back));
	assert(a.contains(f::rounded));
	assert(a.contains(f::vowel));
	assert(a.contains(f::rhoticized));

	assert(!a.contains(f::unspecified));
	assert(!a.contains(f::unrounded));
	assert(!a.contains(f::voiceless));
	assert(!a.contains(f::long_));
}

TEST_CASE("phoneme equality")
{
	assert(tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified)
	    == tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    == tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::high,      f::back, f::unrounded, f::vowel, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back,  f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::semi_high, f::front, f::unrounded, f::vowel, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::semi_high, f::back, f::rounded,   f::vowel, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel,   f::rhoticized)
	      == tts::phoneme(f::semi_high, f::back, f::unrounded, f::lateral, f::rhoticized)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      == tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::long_)));
}

TEST_CASE("phoneme inequality")
{
	assert(!(tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified)
	      != tts::phoneme(f::unspecified, f::unspecified, f::unspecified, f::unspecified, f::unspecified)));

	assert(!(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	      != tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::high,      f::back, f::unrounded, f::vowel, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back,  f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::semi_high, f::front, f::unrounded, f::vowel, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::semi_high, f::back, f::rounded,   f::vowel, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel,   f::rhoticized)
	    != tts::phoneme(f::semi_high, f::back, f::unrounded, f::lateral, f::rhoticized));

	assert(tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::rhoticized)
	    != tts::phoneme(f::semi_high, f::back, f::unrounded, f::vowel, f::long_));
}

TEST_CASE("tts::get_feature_id -- invalid abbreviations")
{
	#define _(x) std::pair<bool, tts::feature>(false, x)

	assert(tts::get_feature_id(nullptr) == _(f::unspecified));
	assert(tts::get_feature_id("xyz") == _(f::unspecified));
	assert(tts::get_feature_id("aspirated") == _(f::unspecified));

	#undef _
}

TEST_CASE("tts::get_feature_id -- ascii-ipa abbreviations")
{
	#define _(x) std::pair<bool, tts::feature>(true, x)

	assert(tts::get_feature_id("vcd") == _(f::voiced));
	assert(tts::get_feature_id("vls") == _(f::voiceless));

	assert(tts::get_feature_id("blb") == _(f::bilabial));
	assert(tts::get_feature_id("lbd") == _(f::labio_dental));
	assert(tts::get_feature_id("dnt") == _(f::dental));
	assert(tts::get_feature_id("alv") == _(f::alveolar));
	assert(tts::get_feature_id("rfx") == _(f::retroflex));
	assert(tts::get_feature_id("pla") == _(f::palato_alveolar));
	assert(tts::get_feature_id("pal") == _(f::palatal));
	assert(tts::get_feature_id("vel") == _(f::velar));
	assert(tts::get_feature_id("lbv") == _(f::labio_velar));
	assert(tts::get_feature_id("uvl") == _(f::uvular));
	assert(tts::get_feature_id("phr") == _(f::pharyngeal));
	assert(tts::get_feature_id("glt") == _(f::glottal));

	assert(tts::get_feature_id("stp") == _(f::plosive));
	assert(tts::get_feature_id("frc") == _(f::fricative));
	assert(tts::get_feature_id("nas") == _(f::nasal));
	assert(tts::get_feature_id("apr") == _(f::approximant));
	assert(tts::get_feature_id("vwl") == _(f::vowel));
	assert(tts::get_feature_id("lat") == _(f::lateral));
	assert(tts::get_feature_id("trl") == _(f::trill));
	assert(tts::get_feature_id("flp") == _(f::flap));
	assert(tts::get_feature_id("clk") == _(f::click));
	assert(tts::get_feature_id("ejc") == _(f::ejective));
	assert(tts::get_feature_id("imp") == _(f::implosive));

	assert(tts::get_feature_id("hgh") == _(f::high));
	assert(tts::get_feature_id("smh") == _(f::semi_high));
	assert(tts::get_feature_id("umd") == _(f::upper_mid));
	assert(tts::get_feature_id("mid") == _(f::mid));
	assert(tts::get_feature_id("lmd") == _(f::lower_mid));
	assert(tts::get_feature_id("low") == _(f::low));

	assert(tts::get_feature_id("fnt") == _(f::front));
	assert(tts::get_feature_id("cnt") == _(f::center));
	assert(tts::get_feature_id("bck") == _(f::back));

	assert(tts::get_feature_id("unr") == _(f::unrounded));
	assert(tts::get_feature_id("rnd") == _(f::rounded));

	assert(tts::get_feature_id("asp") == _(f::aspirated));
	assert(tts::get_feature_id("unx") == _(f::unexploded));
	assert(tts::get_feature_id("syl") == _(f::syllabic));
	assert(tts::get_feature_id("mrm") == _(f::murmured));
	assert(tts::get_feature_id("lng") == _(f::long_));
	assert(tts::get_feature_id("vzd") == _(f::velarized));
	assert(tts::get_feature_id("lzd") == _(f::labialized));
	assert(tts::get_feature_id("pzd") == _(f::palatalized));
	assert(tts::get_feature_id("rzd") == _(f::rhoticized));
	assert(tts::get_feature_id("nzd") == _(f::nasalized));
	assert(tts::get_feature_id("fzd") == _(f::pharyngealized));

	// These are properties of consonant phonemes that are not needed to
	// specify phonemes in the IPA chart. They are only referenced in
	// Appendix A of the ASCII-IPA document.

	assert(tts::get_feature_id("orl") == _(f::unspecified));
	assert(tts::get_feature_id("ctl") == _(f::unspecified));

	#undef _
}

TEST_CASE("explicit feature reader -- no input")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader();
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));

	cainteoir::buffer test(nullptr, nullptr);
	reader->reset(test);

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
}

TEST_CASE("explicit feature reader -- single phoneme")
{
	// Test data to cover all feature abbreviations:
	static const std::initializer_list<std::pair<
		const cainteoir::buffer,
		const tts::phoneme
	>> phonemes = {
		// consonants ...
		{ "{vcd,blb,stp,asp}",     { f::voiced, f::bilabial, f::plosive, f::aspirated } },
		{ "{vls,lbd,frc,unx}",     { f::voiceless, f::labio_dental, f::fricative, f::unexploded } },
		{ "{vcd,dnt,nas,syl}",     { f::voiced, f::dental, f::nasal, f::syllabic } },
		{ "{vls,alv,apr,mrm}",     { f::voiceless, f::alveolar, f::approximant, f::murmured } },
		{ "{vcd,lbd,lat,frc}",     { f::voiced, f::labio_dental, f::lateral, f::fricative } },
		{ "{vls,rfx,trl,vzd}",     { f::voiceless, f::retroflex, f::trill, f::velarized } },
		{ "{vcd,pla,flp,lzd}",     { f::voiced, f::palato_alveolar, f::flap, f::labialized } },
		{ "{vls,pal,clk}",         { f::voiceless, f::palatal, f::click } },
		{ "{vcd,vel,ejc}",         { f::voiced, f::velar, f::ejective } },
		{ "{vls,lbv,imp}",         { f::voiceless, f::labio_velar, f::implosive } },
		{ "{vcd,uvl,stp,pzd}",     { f::voiced, f::uvular, f::plosive, f::palatalized } },
		{ "{vls,phr,frc,nzd}",     { f::voiceless, f::pharyngeal, f::fricative, f::nasalized } },
		{ "{vcd,glt,nas,fzd}",     { f::voiced, f::glottal, f::nasal, f::pharyngealized } },
		// ignored phoneme features ...
		{ "{vcd,blb,orl,stp}",     { f::voiced, f::bilabial, f::plosive } },
		{ "{vcd,blb,ctl,stp}",     { f::voiced, f::bilabial, f::plosive } },
		// vowels ...
		{ "{hgh,fnt,unr,vwl,lng}", { f::high, f::front, f::unrounded, f::vowel, f::long_ } },
		{ "{smh,fnt,rnd,vwl}",     { f::semi_high, f::front, f::rounded, f::vowel } },
		{ "{umd,cnt,unr,vwl}",     { f::upper_mid, f::center, f::unrounded, f::vowel } },
		{ "{mid,cnt,rnd,vwl,rzd}", { f::mid, f::center, f::rounded, f::vowel, f::rhoticized } },
		{ "{lmd,bck,unr,vwl}",     { f::lower_mid, f::back, f::unrounded, f::vowel } },
		{ "{low,bck,rnd,vwl}",     { f::low, f::back, f::rounded, f::vowel } },
	};

	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader();

	for (const auto &test : phonemes)
	{
		fprintf(stdout, "... ... testing phoneme %s\n", test.first.begin());
		reader->reset(test.first);

		assert(reader->read());
		assert(*reader == test.second);

		assert(!reader->read());
		assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
	}
}

TEST_CASE("explicit feature reader -- multiple phonemes")
{
	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader();

	cainteoir::buffer test("{vls,alv,stp}{low,fnt,unr,vwl}{vcd,vel,stp}"); // test = /t&g/
	reader->reset(test);

	assert(reader->read());
	assert(*reader == tts::phoneme(f::voiceless, f::alveolar, f::plosive));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::low, f::front, f::unrounded, f::vowel));

	assert(reader->read());
	assert(*reader == tts::phoneme(f::voiced, f::velar, f::plosive));

	assert(!reader->read());
	assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
}

TEST_CASE("explicit feature reader -- phoneme errors")
{
	static const std::initializer_list<std::pair<
		const cainteoir::buffer,
		const std::string
	>> phonemes = {
		// missing '}' at the end of the phoneme ...
		{ "{vcd,alv,frc",              "unexpected end of phoneme (expecting '}')" },
		{ "{vcd,alv,",                 "unexpected end of phoneme (expecting '}')" },
		{ "{",                         "unexpected end of phoneme (expecting '}')" },
		// no '{' to start the phoneme ...
		{ "}",                         "unexpected start of phoneme (expecting '{')" },
		{ "vcd,alv,frc",               "unexpected start of phoneme (expecting '{')" },
		{ ",alv,frc",                  "unexpected start of phoneme (expecting '{')" },
		{ "@",                         "unexpected start of phoneme (expecting '{')" },
		{ "5",                         "unexpected start of phoneme (expecting '{')" },
		// no phoneme feature specified ...
		{ "{}",                        "missing phoneme feature" },
		{ "{vcd,}",                    "missing phoneme feature" },
		{ "{,lbd}",                    "missing phoneme feature" },
		// less than 3 features ...
		{ "{alv}",                     "a phoneme must specify at least 3 features" },
		{ "{vcd,dnt}",                 "a phoneme must specify at least 3 features" },
		// more than 5 features ...
		{ "{vcd,dnt,lat,frc,mrm,asp}", "a phoneme must specify no more than 5 features" },
		// features shorter/longer than 3 characters ...
		{ "{s}",                       "a phoneme feature must be 3 characters long" },
		{ "{st}",                      "a phoneme feature must be 3 characters long" },
		{ "{stop}",                    "a phoneme feature must be 3 characters long" },
		// unknown feature ...
		{ "{aaa}",                     "unknown phoneme feature" },
		{ "{xyz}",                     "unknown phoneme feature" },
	};

	std::shared_ptr<tts::phoneme_reader> reader = tts::createPhonemeReader();

	for (const auto &test : phonemes)
	{
		fprintf(stdout, "... ... testing phoneme %s\n", test.first.begin());
		reader->reset(test.first);

		try
		{
			reader->read();
			assert(false); // reading the phoneme should have thrown an exception
		}
		catch (const tts::phoneme_error &e)
		{
			assert(e.what() == test.second);
		}
		catch (...)
		{
			assert(false); // should be a tts::phoneme_error exception
		}
		assert(*reader == tts::phoneme(f::unspecified, f::unspecified, f::unspecified));
	}
}
