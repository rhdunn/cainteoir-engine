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

TEST_CASE("reader -- no input")
{
	std::shared_ptr<tts::prosody_reader> reader = tts::createPhoReader(tts::createPhonemeReader("cxs"));
	assert(reader.get());
	match(*reader, { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	reader->reset(std::shared_ptr<cainteoir::buffer>());

	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });

	auto test = std::make_shared<cainteoir::buffer>(nullptr, nullptr);
	reader->reset(test);

	assert(!reader->read());
	match(*reader, { ipa::unspecified, ipa::unspecified, { 0, css::time::inherit }, {} });
}

TEST_CASE("writer -- no input")
{
	std::shared_ptr<tts::prosody_writer> writer = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));
	assert(writer.get());

	assert(!writer->write({ ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, {}, {}}));

	cainteoir::memory_file output;
	writer->reset(output);

	auto data = output.buffer();
	assert(data->empty());

	writer->reset(nullptr);
	assert(!writer->write({ ipa::voiced | ipa::bilabial | ipa::plosive, ipa::unspecified, {}, {}}));
}

TEST_CASE("writer -- basic phonemes")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::voiced | ipa::bilabial | ipa::plosive,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "b 80\n", 5);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i 120\n", 6);
}

TEST_CASE("writer -- diacritics")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::bilabial | ipa::plosive | ipa::aspirated,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "p_h 80\n", 7);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel | ipa::lowered,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i_o 120\n", 8);
}

TEST_CASE("writer -- suprasegmentals")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::bilabial | ipa::plosive | ipa::primary_stress,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "p 80\n", 5);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel | ipa::long_,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i: 120\n", 7);
}

TEST_CASE("writer -- tones")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::bilabial | ipa::plosive | ipa::tone_start_high,
	                    ipa::unspecified,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "p 80\n", 5);

	match(to_str(pho, { ipa::high | ipa::front | ipa::vowel | ipa::tone_start_mid,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "i 120\n", 6);
}

TEST_CASE("writer -- diphthongs and affricates")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::alveolar | ipa::plosive,
	                    ipa::palato_alveolar | ipa::sibilant | ipa::fricative,
	                    { 80, css::time::milliseconds },
	                    {}}),
	      "tS 80\n", 6);

	match(to_str(pho, { ipa::semi_high | ipa::front | ipa::vowel,
	                    ipa::mid | ipa::center | ipa::vowel,
	                    { 120, css::time::milliseconds },
	                    {}}),
	      "I@ 120\n", 7);
}

TEST_CASE("writer -- pitch envelope")
{
	std::shared_ptr<tts::prosody_writer> pho = tts::createPhoWriter(tts::createPhonemeWriter("cxs"));

	match(to_str(pho, { ipa::low | ipa::back | ipa::vowel,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {
	                      { 0, { 95, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95\n", 11);

	match(to_str(pho, { ipa::low | ipa::back | ipa::vowel,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {
	                      {   0, { 95,   css::frequency::hertz }},
	                      { 100, { 55.3, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95 100 55.3\n", 20);

	match(to_str(pho, { ipa::low | ipa::back | ipa::vowel,
	                    ipa::unspecified,
	                    { 120, css::time::milliseconds },
	                    {
	                      {   0, { 95, css::frequency::hertz }},
	                      {  50, { 60, css::frequency::hertz }},
	                      { 100, { 55, css::frequency::hertz }},
	                    }}),
	      "A 120 0 95 50 60 100 55\n", 24);
}
