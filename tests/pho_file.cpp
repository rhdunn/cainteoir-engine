/* MBROLA pho file tests.
 *
 * Copyright (C) 2014 Reece H. Dunn
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

#include <cainteoir/prosody.hpp>

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
	assert_location(got.phoneme1 == expected.phoneme1, file, line);
	assert_location(got.phoneme2 == expected.phoneme2, file, line);
	assert_location(got.duration.value() == expected.duration.value(), file, line);
	assert_location(got.duration.units() == expected.duration.units(), file, line);
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

const tts::prosody &parse(std::shared_ptr<tts::prosody_reader> &reader, const char *str)
{
	reader->reset(std::make_shared<cainteoir::buffer>(str));
	assert(reader->read());
	return *reader;
}

TEST_CASE("reader -- no input")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));
	assert(reader.get());
	match(*reader, { ipa::unspecified, ipa::unspecified, ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	reader->reset(std::shared_ptr<cainteoir::buffer>());

	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	auto test = std::make_shared<cainteoir::buffer>(nullptr, nullptr);
	reader->reset(test);

	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });
}

TEST_CASE("reader -- |phoneme duration| [phonemes]")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	match(parse(reader, "b 80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "b 55.4"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 55.4, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "i 110"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 110, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "p_h 80"),
	      { ipa::bilabial | ipa::plosive | ipa::aspirated,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "i_o 120"),
	      { ipa::high | ipa::front | ipa::vowel | ipa::lowered,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 120, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "i: 120"),
	      { ipa::high | ipa::front | ipa::vowel | ipa::long_,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 120, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "tS 80"),
	      { ipa::alveolar | ipa::plosive,
	        ipa::palato_alveolar | ipa::sibilant | ipa::fricative,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "I@ 120"),
	      { ipa::semi_high | ipa::front | ipa::vowel,
	        ipa::mid | ipa::center | ipa::vowel,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 120, css::time::milliseconds },
	        {}});
	assert(!reader->read());
}

TEST_CASE("reader -- |phoneme duration| [arpabet]")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("radio"));

	match(parse(reader, "hh 55.4"),
	      { ipa::glottal | ipa::approximant,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 55.4, css::time::milliseconds },
	        {}});
	assert(!reader->read());
}

TEST_CASE("reader -- |phoneme duration| [diphones]")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	match(parse(reader, "b-i 80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "i-p_h 80"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::bilabial | ipa::plosive | ipa::aspirated,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "p-i_o 120"),
	      { ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::high | ipa::front | ipa::vowel | ipa::lowered,
	        ipa::unspecified,
	        { 120, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "p-i: 120"),
	      { ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::high | ipa::front | ipa::vowel | ipa::long_,
	        ipa::unspecified,
	        { 120, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "i-tS 80"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::alveolar | ipa::plosive,
	        ipa::palato_alveolar | ipa::sibilant | ipa::fricative,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "p-I@ 120"),
	      { ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::semi_high | ipa::front | ipa::vowel,
	        ipa::mid | ipa::center | ipa::vowel,
	        { 120, css::time::milliseconds },
	        {}});
	assert(!reader->read());
}

TEST_CASE("reader -- |phoneme duration| [whitespace]")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	assert_throws(parse(reader, "b80"), tts::phoneme_error, "expected whitespace after the phoneme");

	match(parse(reader, "b\t80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "b  80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "b\t\t80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());
}

TEST_CASE("reader -- |phoneme duration (offset pitch)+| [phonemes]")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	match(parse(reader, "b 80 0 120"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80 0 120 100 122.5"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80 0 124.5 50 100 100 90"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 124.5, css::frequency::hertz }},
	          {  50, { 100,   css::frequency::hertz }},
	          { 100, {  90,   css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80 0 120 33 100 66 115 100 125"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  33, { 100, css::frequency::hertz }},
	          {  66, { 115, css::frequency::hertz }},
	          { 100, { 125, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80 0 120 25 110 50 100 75 95 100 95"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  25, { 110, css::frequency::hertz }},
	          {  50, { 100, css::frequency::hertz }},
	          {  75, {  95, css::frequency::hertz }},
	          { 100, {  95, css::frequency::hertz }},
	        }});
	assert(!reader->read());
}

TEST_CASE("reader -- |phoneme duration (offset pitch)+| [diphones]")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	match(parse(reader, "i-b 80 0 120"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "i-b 80 0 120 100 122.5"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "i-b 80 0 124.5 50 100 100 90"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 124.5, css::frequency::hertz }},
	          {  50, { 100,   css::frequency::hertz }},
	          { 100, {  90,   css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "i-b 80 0 120 33 100 66 115 100 125"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  33, { 100, css::frequency::hertz }},
	          {  66, { 115, css::frequency::hertz }},
	          { 100, { 125, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "i-b 80 0 120 25 110 50 100 75 95 100 95"),
	      { ipa::high | ipa::front | ipa::vowel,
	        ipa::unspecified,
	        ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120, css::frequency::hertz }},
	          {  25, { 110, css::frequency::hertz }},
	          {  50, { 100, css::frequency::hertz }},
	          {  75, {  95, css::frequency::hertz }},
	          { 100, {  95, css::frequency::hertz }},
	        }});
	assert(!reader->read());
}

TEST_CASE("reader -- |phoneme duration (offset pitch)+| [whitespace]")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	match(parse(reader, "b 80  0  120"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80   0   120"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80\t0\t120"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80\t\t0\t\t120"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          { 0, { 120, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80  0  120  100  122.5"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80   0   120   100   122.5"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80\t0\t120\t100\t122.5"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
	assert(!reader->read());

	match(parse(reader, "b 80\t\t0\t\t120\t\t100\t\t122.5"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {
	          {   0, { 120,   css::frequency::hertz }},
	          { 100, { 122.5, css::frequency::hertz }},
	        }});
	assert(!reader->read());
}

TEST_CASE("reader -- blank line")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	match(parse(reader, "\nb 80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "\r\nb 80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "\n\nb 80"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "b 80\n"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());

	match(parse(reader, "b 80\r\n"),
	      { ipa::voiced | ipa::bilabial | ipa::plosive,
	        ipa::unspecified,
	        ipa::unspecified,
	        ipa::unspecified,
	        { 80, css::time::milliseconds },
	        {}});
	assert(!reader->read());
}

TEST_CASE("reader -- comment")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeParser("cxs"));

	reader->reset(std::make_shared<cainteoir::buffer>("; this is a comment"));
	match(*reader, { ipa::unspecified, ipa::unspecified, ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	reader->reset(std::make_shared<cainteoir::buffer>("; another comment\n"));
	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	reader->reset(std::make_shared<cainteoir::buffer>("; comment 3\r\n"));
	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });
}

TEST_CASE("writer -- no input")
{
	std::shared_ptr<tts::prosody_writer> writer = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));
	assert(writer.get());

	assert(!writer->write({ ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, ipa::unspecified, ipa::unspecified, {}, {}}));

	cainteoir::memory_file output;
	writer->reset(output);

	auto data = output.buffer();
	assert(data->empty());

	writer->reset(nullptr);
	assert(!writer->write({ ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, ipa::unspecified, ipa::unspecified, {}, {}}));
}

TEST_CASE("writer -- basic phonemes (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::voiced | ipa::bilabial | ipa::plosive,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "b 80\n", 5);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i 120\n", 6);
}

TEST_CASE("writer -- basic phonemes (arpabet)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("radio"));

	match(to_str(pho, { ipa::glottal | ipa::approximant,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "hh 80\n", 6);
}

TEST_CASE("writer -- basic phonemes (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::voiced | ipa::bilabial | ipa::plosive,
	                    ipa::unspecified,
	                    ipa::high | ipa::front | ipa::vowel,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "b-i 80\n", 7);
}

TEST_CASE("writer -- diacritics (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::bilabial | ipa::plosive | ipa::aspirated,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "p_h 80\n", 7);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel | ipa::lowered,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i_o 120\n", 8);
}

TEST_CASE("writer -- diacritics (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::bilabial | ipa::plosive | ipa::aspirated,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "i-p_h 80\n", 9);

	match(to_str(pho, { ipa::bilabial | ipa::plosive,
	                    ipa::unspecified,
	                    ipa::high | ipa::front | ipa::vowel | ipa::lowered,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "p-i_o 120\n", 10);
}

TEST_CASE("writer -- suprasegmentals (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::bilabial | ipa::plosive | ipa::primary_stress,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "p 80\n", 5);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel | ipa::long_,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i: 120\n", 7);
}

TEST_CASE("writer -- suprasegmentals (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::low | ipa::back | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::bilabial | ipa::plosive | ipa::primary_stress,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "A-p 80\n", 7);

	match(to_str(pho, { ipa::bilabial | ipa::plosive | ipa::voiced,
	                    ipa::unspecified,
	                    ipa::high | ipa::front | ipa::vowel | ipa::long_,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "b-i: 120\n", 9);
}

TEST_CASE("writer -- tones (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::bilabial | ipa::plosive | ipa::tone_start_high,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "p 80\n", 5);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel | ipa::tone_start_mid,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i 120\n", 6);
}

TEST_CASE("writer -- tones (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::bilabial | ipa::plosive | ipa::tone_start_high,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "i-p 80\n", 7);

	match(to_str(pho, { ipa::bilabial | ipa::plosive,
	                    ipa::unspecified,
	                    ipa::high | ipa::front | ipa::vowel | ipa::tone_start_mid,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "p-i 120\n", 8);
}

TEST_CASE("writer -- diphthongs and affricates (phoneme)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::alveolar | ipa::plosive,
	                    ipa::palato_alveolar | ipa::sibilant | ipa::fricative,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "tS 80\n", 6);

	match(to_str(pho, { ipa::semi_high | ipa::front | ipa::vowel,
	                    ipa::mid | ipa::center | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "I@ 120\n", 7);
}

TEST_CASE("writer -- diphthongs and affricates (diphone)")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::alveolar | ipa::plosive,
	                    ipa::palato_alveolar | ipa::sibilant | ipa::fricative,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "i-tS 80\n", 8);

	match(to_str(pho, { ipa::bilabial | ipa::plosive,
	                    ipa::unspecified,
	                    ipa::semi_high | ipa::front | ipa::vowel,
			    ipa::mid | ipa::center | ipa::vowel,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "p-I@ 120\n", 9);
}

TEST_CASE("writer -- pitch envelope")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::low | ipa::back | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {
	                      { 0, { 95, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95\n", 11);

	match(to_str(pho, { ipa::low | ipa::back | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {
	                      {   0, { 95,   css::frequency::hertz }},
	                      { 100, { 55.3, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95 100 55.3\n", 20);

	match(to_str(pho, { ipa::low | ipa::back | ipa::vowel,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {
	                      {   0, { 95, css::frequency::hertz }},
	                      {  50, { 60, css::frequency::hertz }},
	                      { 100, { 55, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95 50 60 100 55\n", 24);
}
