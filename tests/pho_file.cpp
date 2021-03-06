/* MBROLA pho file tests.
 *
 * Copyright (C) 2014-2015 Reece H. Dunn
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

#include <cainteoir/synthesizer.hpp>

#include "tester.hpp"

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

REGISTER_TESTSUITE("pho-file");

void match_(const cainteoir::buffer &got, const char *expected, int len, const char *file, int line)
{
	bool matched = assert_location(len == got.size(), file, line);
	const char *buffer = got.begin();
	for (int i = 0; i < got.size(); ++i)
		matched &= assert_location(uint8_t(expected[i]) == uint8_t(buffer[i]), file, line);
	if (!matched)
	{
		fprintf(stdout, "==> failed with test string \"");
		for (auto c : cainteoir::buffer(expected, expected + len))
		{
			if (c > 0x20 && c < 0x80)
				fprintf(stdout, "%c", c);
			else
				fprintf(stdout, "\\x%02X", (uint8_t)c);
		}
		fprintf(stdout, "\"\n");
	}
}

void match_(const std::shared_ptr<cainteoir::buffer> &got, const char *expected, int len, const char *file, int line)
{
	assert_location(got.get(), file, len);
	match_(*got, expected, len, file, line);
}

void match_(const tts::prosody &got, const tts::prosody &expected, const char *file, int line)
{
	assert_location(got.first.phoneme1 == expected.first.phoneme1, file, line);
	assert_location(got.first.phoneme2 == expected.first.phoneme2, file, line);
	assert_location(got.first.duration.value() == expected.first.duration.value(), file, line);
	assert_location(got.first.duration.units() == expected.first.duration.units(), file, line);
	assert_location(got.second.phoneme1 == expected.second.phoneme1, file, line);
	assert_location(got.second.phoneme2 == expected.second.phoneme2, file, line);
	assert_location(got.second.duration.value() == expected.second.duration.value(), file, line);
	assert_location(got.second.duration.units() == expected.second.duration.units(), file, line);
	assert_location(got.envelope.size() == expected.envelope.size(), file, line);
	for (int i = 0; i < std::min(got.envelope.size(), expected.envelope.size()); ++i)
	{
		assert_location(got.envelope[i].offset == expected.envelope[i].offset, file, line);
		assert_location(got.envelope[i].pitch.value() == expected.envelope[i].pitch.value(), file, line);
		assert_location(got.envelope[i].pitch.units() == expected.envelope[i].pitch.units(), file, line);
	}
}

#define match(...) match_(__VA_ARGS__, __FILE__, __LINE__)

std::shared_ptr<cainteoir::buffer> to_str(std::shared_ptr<tts::prosody_writer> &writer, const tts::prosody prosody)
{
	cainteoir::memory_file output;
	writer->reset(output);
	assert(writer->write(prosody));
	return output.buffer();
}

const tts::prosody parse(const char *str, const char *phonemeset = "cxs", bool read = true)
{
	auto reader = tts::createPhoReader(tts::createPhonemeParser(phonemeset),
	                                   std::make_shared<cainteoir::buffer>(str));
	assert(reader->read() == read);
	tts::prosody ret = *reader;
	assert(!reader->read());
	return ret;
}

TEST_CASE("reader -- no input")
{
	const tts::prosody unspecified = {
		{ ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
		{ ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
		{}
	};

	assert(!tts::createPhoReader(tts::createPhonemeParser("cxs"), {}).get());

	auto reader = tts::createPhoReader(tts::createPhonemeParser("cxs"),
		std::make_shared<cainteoir::buffer>(nullptr, nullptr));

	assert(!reader->read());
	match(*reader, unspecified);
}

TEST_CASE("reader -- comment")
{
	const tts::prosody unspecified = {
		{ ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
		{ ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
		{}
	};

	match(parse("; this is a comment", "cxs", false), unspecified);
	match(parse("; another comment\n", "cxs", false), unspecified);
	match(parse("; comment 3r\n", "cxs", false), unspecified);
}

TEST_CASE("reader -- |phoneme|")
{
	match(parse("."),
	      { { ipa::syllable_break, ipa::unspecified, { 0, css::time::inherit } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});
}

TEST_CASE("reader -- |phoneme duration| [phonemes]")
{
	match(parse("b 80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b 55.4"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 55.4, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("i 110"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 110, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("p_h 80"),
	      { { ipa::bilabial | ipa::plosive, ipa::glottal | ipa::approximant | ipa::diacritized, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("i_o 120"),
	      { { ipa::high | ipa::front | ipa::vowel | ipa::lowered, ipa::unspecified, { 120, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("i: 120"),
	      { { ipa::high | ipa::front | ipa::vowel | ipa::long_, ipa::unspecified, { 120, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("tS 80"),
	      { { ipa::alveolar | ipa::plosive, ipa::palato_alveolar | ipa::sibilant | ipa::fricative, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("I@ 120"),
	      { { ipa::semi_high | ipa::front | ipa::vowel, ipa::mid | ipa::center | ipa::vowel, { 120, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});
}

TEST_CASE("reader -- |phoneme duration| [phonemes] ~ arpabet")
{
	match(parse("hh 55.4", "radio"),
	      { { ipa::glottal | ipa::approximant, ipa::unspecified, { 55.4, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("aw 58", "radio"),
	      { { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 58, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});
}

TEST_CASE("reader -- |phoneme duration| [diphones]")
{
	match(parse("b-i 80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b-i 60-20"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 60, css::time::milliseconds } },
	        { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 20, css::time::milliseconds } },
	        {}});

	match(parse("i-p_h 80"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::bilabial | ipa::plosive, ipa::glottal | ipa::approximant | ipa::diacritized, { 0, css::time::inherit } },
	        {}});

	match(parse("p-i_o 120"),
	      { { ipa::bilabial | ipa::plosive, ipa::unspecified, { 120, css::time::milliseconds } },
	        { ipa::high | ipa::front | ipa::vowel | ipa::lowered, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("p-i: 120"),
	      { { ipa::bilabial | ipa::plosive, ipa::unspecified, { 120, css::time::milliseconds } },
	        { ipa::high | ipa::front | ipa::vowel | ipa::long_, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("i-tS 80"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::alveolar | ipa::plosive, ipa::palato_alveolar | ipa::sibilant | ipa::fricative, { 0, css::time::inherit } },
	        {}});

	match(parse("p-I@ 120"),
	      { { ipa::bilabial | ipa::plosive, ipa::unspecified, { 120, css::time::milliseconds } },
	        { ipa::semi_high | ipa::front | ipa::vowel, ipa::mid | ipa::center | ipa::vowel, { 0, css::time::inherit } },
	        {}});
}

TEST_CASE("reader -- |phoneme duration| [diphones] ~ arpabet")
{
	match(parse("hh-aw 55.4", "radio"),
	      { { ipa::glottal | ipa::approximant, ipa::unspecified, { 55.4, css::time::milliseconds } },
	        { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 0, css::time::inherit } },
	        {}});

	match(parse("aw-hh 58", "radio"),
	      { { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 58, css::time::milliseconds } },
	        { ipa::glottal | ipa::approximant, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("hh-aw 55.4-11.5", "radio"),
	      { { ipa::glottal | ipa::approximant, ipa::unspecified, { 55.4, css::time::milliseconds } },
	        { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 11.5, css::time::milliseconds } },
	        {}});

	match(parse("aw-hh 58-32", "radio"),
	      { { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 58, css::time::milliseconds } },
	        { ipa::glottal | ipa::approximant, ipa::unspecified, { 32, css::time::milliseconds } },
	        {}});
}

TEST_CASE("reader -- |phoneme duration| [whitespace]")
{
	assert_throws(parse("b80"), tts::phoneme_error, "unrecognised character 0");

	match(parse("b\t80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b  80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b\t\t80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b 80 \r"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b 80 \n"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b 80 "),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});
}

TEST_CASE("reader -- |phoneme duration| [comment]")
{
	match(parse("b 80; a comment"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b 80 ; comment"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){1}| [phonemes]")
{
	assert_throws(parse("b 80 0"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 \r"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 \n"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 "),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 ; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	match(parse("b 80 0 120"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){2}| [phonemes]")
{
	assert_throws(parse("b 80 0 120 100"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 100 \r"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 100 \n"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 100 "),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 100; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 100 ; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	match(parse("b 80 0 120 100 122.5"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){3}| [phonemes]")
{
	assert_throws(parse("b 80 0 124.5 50 100 100"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 124.5 50 100 100 \r"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 124.5 50 100 100 \n"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 124.5 50 100 100 "),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 124.5 50 100 100; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 124.5 50 100 100 ; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	match(parse("b 80 0 124.5 50 100 100 90"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 124.5, css::frequency::hertz }},
	          {  50, { 100,   css::frequency::hertz }},
	          { 100, {  90,   css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){4}| [phonemes]")
{
	assert_throws(parse("b 80 0 120 33 100 66 115 100"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 33 100 66 115 100 \r"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 33 100 66 115 100 \n"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 33 100 66 115 100 "),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 33 100 66 115 100; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 33 100 66 115 100 ; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	match(parse("b 80 0 120 33 100 66 115 100 125"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  33, { 100, css::frequency::hertz }},
	          {  66, { 115, css::frequency::hertz }},
	          { 100, { 125, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){5}| [phonemes]")
{
	assert_throws(parse("b 80 0 120 25 110 50 100 75 95 100"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 25 110 50 100 75 95 100 \r"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 25 110 50 100 75 95 100 \n"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 25 110 50 100 75 95 100 "),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 25 110 50 100 75 95 100; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	assert_throws(parse("b 80 0 120 25 110 50 100 75 95 100 ; comment"),
	              tts::phoneme_error, "expected whitespace after the offset");

	match(parse("b 80 0 120 25 110 50 100 75 95 100 95"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  25, { 110, css::frequency::hertz }},
	          {  50, { 100, css::frequency::hertz }},
	          {  75, {  95, css::frequency::hertz }},
	          { 100, {  95, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch)+| [diphones]")
{
	match(parse("i-b 80 0 120"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("i-b 80 0 120 100 122.5"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("i-b 80 0 124.5 50 100 100 90"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 124.5, css::frequency::hertz }},
	          {  50, { 100,   css::frequency::hertz }},
	          { 100, {  90,   css::frequency::hertz }},
	        }});

	match(parse("i-b 80 0 120 33 100 66 115 100 125"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  33, { 100, css::frequency::hertz }},
	          {  66, { 115, css::frequency::hertz }},
	          { 100, { 125, css::frequency::hertz }},
	        }});

	match(parse("i-b 80 0 120 25 110 50 100 75 95 100 95"),
	      { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  25, { 110, css::frequency::hertz }},
	          {  50, { 100, css::frequency::hertz }},
	          {  75, {  95, css::frequency::hertz }},
	          { 100, {  95, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){1}| [whitespace]")
{
	match(parse("b 80  0  120"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("b 80   0   120"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("b 80\t0\t120"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("b 80\t\t0\t\t120"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 \r"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 \n"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 "),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){1}| [comment]")
{
	match(parse("b 80 0 120; comment"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 ; comment"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){2}| [whitespace]")
{
	match(parse("b 80  0  120  100  122.5"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("b 80   0   120   100   122.5"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("b 80\t0\t120\t100\t122.5"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("b 80\t\t0\t\t120\t\t100\t\t122.5"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 100 122.5 \r"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 100 122.5 \n"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 100 122.5 "),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- |phoneme duration (offset pitch){2}| [comment]")
{
	match(parse("b 80 0 120 100 122.5; comment"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});

	match(parse("b 80 0 120 100 122.5 ; comment"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
}

TEST_CASE("reader -- blank line")
{
	match(parse("\nb 80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("\r\nb 80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("\n\nb 80"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b 80\n"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});

	match(parse("b 80\r\n"),
	      { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	        { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	        {}});
}

TEST_CASE("writer -- no input")
{
	const tts::prosody prosody = {
		{ ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
		{ ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
		{}
	};

	std::shared_ptr<tts::prosody_writer> writer = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));
	assert(writer.get());

	assert(!writer->write(prosody));

	cainteoir::memory_file output;
	writer->reset(output);

	auto data = output.buffer();
	assert(data->empty());

	writer->reset(nullptr);
	assert(!writer->write(prosody));
}

TEST_CASE("writer -- basic phonemes (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::syllable_break, ipa::unspecified, { 0, css::time::inherit } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      ".\n", 2);

	match(to_str(pho, { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "b 80\n", 5);

	match(to_str(pho, { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "i 120\n", 6);
}

TEST_CASE("writer -- basic phonemes (phoneme) ~ arpabet")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("radio"));

	match(to_str(pho, { { ipa::glottal | ipa::approximant, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "hh 80\n", 6);

	match(to_str(pho, { { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 80, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "aw 80\n", 6);
}

TEST_CASE("writer -- basic phonemes (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "b-i 80\n", 7);

	match(to_str(pho, { { ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 30, css::time::milliseconds } },
	                    {}}),
	      "b-i 80-30\n", 10);
}

TEST_CASE("writer -- basic phonemes (diphone) ~ arpabet")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("radio"));

	match(to_str(pho, { { ipa::glottal | ipa::approximant, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 0, css::time::inherit } },
	                    {}}),
	      "hh-aw 80\n", 9);

	match(to_str(pho, { { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 80, css::time::milliseconds } },
	                    { ipa::glottal | ipa::approximant, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "aw-hh 80\n", 9);

	match(to_str(pho, { { ipa::glottal | ipa::approximant, ipa::unspecified, { 50, css::time::milliseconds } },
	                    { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 40, css::time::milliseconds } },
	                    {}}),
	      "hh-aw 50-40\n", 12);

	match(to_str(pho, { { ipa::low | ipa::front | ipa::vowel, ipa::semi_high | ipa::back | ipa::rounded | ipa::vowel | ipa::non_syllabic, { 80, css::time::milliseconds } },
	                    { ipa::glottal | ipa::approximant, ipa::unspecified, { 22, css::time::milliseconds } },
	                    {}}),
	      "aw-hh 80-22\n", 12);
}

TEST_CASE("writer -- diacritics (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::bilabial | ipa::plosive, ipa::glottal | ipa::approximant | ipa::diacritized, { 80, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "p_h 80\n", 7);

	match(to_str(pho, { { ipa::high | ipa::front | ipa::vowel | ipa::lowered, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "i_o 120\n", 8);
}

TEST_CASE("writer -- diacritics (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::bilabial | ipa::plosive, ipa::glottal | ipa::approximant | ipa::diacritized, { 0, css::time::inherit } },
	                    {}}),
	      "i-p_h 80\n", 9);

	match(to_str(pho, { { ipa::bilabial | ipa::plosive, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::high | ipa::front | ipa::vowel | ipa::lowered, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "p-i_o 120\n", 10);
}

TEST_CASE("writer -- suprasegmentals (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::bilabial | ipa::plosive | ipa::primary_stress, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "p 80\n", 5);

	match(to_str(pho, { { ipa::high | ipa::front | ipa::vowel | ipa::long_, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "i: 120\n", 7);
}

TEST_CASE("writer -- suprasegmentals (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::low | ipa::back | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::bilabial | ipa::plosive | ipa::primary_stress, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "A-p 80\n", 7);

	match(to_str(pho, { { ipa::bilabial | ipa::plosive | ipa::voiced, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::high | ipa::front | ipa::vowel | ipa::long_, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "b-i: 120\n", 9);
}

TEST_CASE("writer -- tones (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::bilabial | ipa::plosive | ipa::tone_start_high, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "p 80\n", 5);

	match(to_str(pho, { { ipa::high | ipa::front | ipa::vowel | ipa::tone_start_mid, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "i 120\n", 6);
}

TEST_CASE("writer -- tones (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::bilabial | ipa::plosive | ipa::tone_start_high, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "i-p 80\n", 7);

	match(to_str(pho, { { ipa::bilabial | ipa::plosive, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::high | ipa::front | ipa::vowel | ipa::tone_start_mid, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "p-i 120\n", 8);
}

TEST_CASE("writer -- diphthongs and affricates (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::alveolar | ipa::plosive, ipa::palato_alveolar | ipa::sibilant | ipa::fricative, { 80, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "tS 80\n", 6);

	match(to_str(pho, { { ipa::semi_high | ipa::front | ipa::vowel, ipa::mid | ipa::center | ipa::vowel, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {}}),
	      "I@ 120\n", 7);
}

TEST_CASE("writer -- diphthongs and affricates (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::high | ipa::front | ipa::vowel, ipa::unspecified, { 80, css::time::milliseconds } },
	                    { ipa::alveolar | ipa::plosive, ipa::palato_alveolar | ipa::sibilant | ipa::fricative, { 0, css::time::inherit } },
	                    {}}),
	      "i-tS 80\n", 8);

	match(to_str(pho, { { ipa::bilabial | ipa::plosive, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::semi_high | ipa::front | ipa::vowel, ipa::mid | ipa::center | ipa::vowel, { 0, css::time::inherit } },
	                    {}}),
	      "p-I@ 120\n", 9);
}

TEST_CASE("writer -- pitch envelope")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { { ipa::low | ipa::back | ipa::vowel, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {
	                      { 0, { 95, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95\n", 11);

	match(to_str(pho, { { ipa::low | ipa::back | ipa::vowel, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {
	                      {   0, { 95,   css::frequency::hertz }},
	                      { 100, { 55.3, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95 100 55.3\n", 20);

	match(to_str(pho, { { ipa::low | ipa::back | ipa::vowel, ipa::unspecified, { 120, css::time::milliseconds } },
	                    { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit } },
	                    {
	                      {   0, { 95, css::frequency::hertz }},
	                      {  50, { 60, css::frequency::hertz }},
	                      { 100, { 55, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95 50 60 100 55\n", 24);
}
