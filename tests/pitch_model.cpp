/* Test for the pitch model.
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

#include <cainteoir/synthesizer.hpp>
#include "tester.hpp"

REGISTER_TESTSUITE("pitch model");

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

template <typename UnitT> struct unit_type;
template <> struct unit_type<css::frequency::type> { typedef css::frequency type; };

template <typename UnitT>
static void test_equal_(const typename unit_type<UnitT>::type aObject,
                        float aValue, const UnitT aUnits,
                        const char *location, int line)
{
	assert_location(aObject.units() == aUnits, location, line);
	assert_location(aObject.value() == aValue, location, line);
}

static void test_equal_(const std::vector<tts::envelope_t> &a,
                        const std::vector<tts::envelope_t> &b,
                        const char *location, int line)
{
	assert_location(a.size() == b.size(), location, line);
	if (a.size() == b.size()) for (int i = 0; i < a.size(); ++i)
	{
		assert_location(a[i].offset == b[i].offset, location, line);
		assert_location(a[i].pitch.units() == b[i].pitch.units(), location, line);
		assert_location(a[i].pitch.value() == b[i].pitch.value(), location, line);
	}
}

#define test_equal(...) test_equal_(__VA_ARGS__, __FILE__, __LINE__)

TEST_CASE("constructor")
{
	tts::pitch_model p{ { 100, css::frequency::hertz },
	                    {  20, css::frequency::hertz },
	                    {   5, css::frequency::hertz } };

	test_equal(p.bottom.mean, 100, css::frequency::hertz);
	test_equal(p.bottom.sdev,   5, css::frequency::hertz);

	test_equal(p.low.mean, 120, css::frequency::hertz);
	test_equal(p.low.sdev,   5, css::frequency::hertz);

	test_equal(p.mid.mean, 140, css::frequency::hertz);
	test_equal(p.mid.sdev,   5, css::frequency::hertz);

	test_equal(p.high.mean, 160, css::frequency::hertz);
	test_equal(p.high.sdev,   5, css::frequency::hertz);

	test_equal(p.top.mean, 180, css::frequency::hertz);
	test_equal(p.top.sdev,   5, css::frequency::hertz);
}

TEST_CASE("tone - invalid")
{
	tts::pitch_model p{ { 130, css::frequency::hertz },
	                    {  15, css::frequency::hertz },
	                    { 2.5, css::frequency::hertz } };

	test_equal(p.tone(ipa::tone_start_bottom | ipa::tone_start_mid).mean, 0, css::frequency::inherit);
	test_equal(p.tone(ipa::tone_start_bottom | ipa::tone_start_mid).sdev, 0, css::frequency::inherit);

	test_equal(p.tone(ipa::non_syllabic).mean, 0, css::frequency::inherit);
	test_equal(p.tone(ipa::non_syllabic).sdev, 0, css::frequency::inherit);
}

TEST_CASE("tone - bottom")
{
	tts::pitch_model p{ { 130, css::frequency::hertz },
	                    {  15, css::frequency::hertz },
	                    { 2.5, css::frequency::hertz } };

	test_equal(p.tone(ipa::tone_start_bottom).mean, 130, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_start_bottom).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_middle_bottom).mean, 130, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_middle_bottom).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_end_bottom).mean, 130, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_end_bottom).sdev, 2.5, css::frequency::hertz);
}

TEST_CASE("tone - low")
{
	tts::pitch_model p{ { 130, css::frequency::hertz },
	                    {  15, css::frequency::hertz },
	                    { 2.5, css::frequency::hertz } };

	test_equal(p.tone(ipa::tone_start_low).mean, 145, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_start_low).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_middle_low).mean, 145, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_middle_low).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_end_low).mean, 145, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_end_low).sdev, 2.5, css::frequency::hertz);
}

TEST_CASE("tone - mid")
{
	tts::pitch_model p{ { 130, css::frequency::hertz },
	                    {  15, css::frequency::hertz },
	                    { 2.5, css::frequency::hertz } };

	test_equal(p.tone(ipa::tone_start_mid).mean, 160, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_start_mid).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_middle_mid).mean, 160, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_middle_mid).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_end_mid).mean, 160, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_end_mid).sdev, 2.5, css::frequency::hertz);
}

TEST_CASE("tone - high")
{
	tts::pitch_model p{ { 130, css::frequency::hertz },
	                    {  15, css::frequency::hertz },
	                    { 2.5, css::frequency::hertz } };

	test_equal(p.tone(ipa::tone_start_high).mean, 175, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_start_high).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_middle_high).mean, 175, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_middle_high).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_end_high).mean, 175, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_end_high).sdev, 2.5, css::frequency::hertz);
}

TEST_CASE("tone - top")
{
	tts::pitch_model p{ { 130, css::frequency::hertz },
	                    {  15, css::frequency::hertz },
	                    { 2.5, css::frequency::hertz } };

	test_equal(p.tone(ipa::tone_start_top).mean, 190, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_start_top).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_middle_top).mean, 190, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_middle_top).sdev, 2.5, css::frequency::hertz);

	test_equal(p.tone(ipa::tone_end_top).mean, 190, css::frequency::hertz);
	test_equal(p.tone(ipa::tone_end_top).sdev, 2.5, css::frequency::hertz);
}

TEST_CASE("envelope - empty")
{
	ipa::phoneme ipa_b = ipa::voiced | ipa::bilabial | ipa::plosive;

	tts::pitch_model p{ { 100, css::frequency::hertz },
	                    {  20, css::frequency::hertz },
	                    {   5, css::frequency::hertz } };

	test_equal(p.envelope(ipa_b, tts::zero_distribution), {});
}

TEST_CASE("envelope - (start,-,-) tone")
{
	ipa::phoneme ipa_b = ipa::voiced | ipa::bilabial | ipa::plosive;

	tts::pitch_model p{ { 100, css::frequency::hertz },
	                    {  20, css::frequency::hertz },
	                    {   5, css::frequency::hertz } };

	// low tone

	ipa_b.set(ipa::tone_start_low, ipa::tone_start);
	test_equal(p.envelope(ipa_b, tts::zero_distribution),
	          {{   0, { 120, css::frequency::hertz }},
	           { 100, { 120, css::frequency::hertz }}});

	// top tone

	ipa_b.set(ipa::tone_start_top, ipa::tone_start);
	test_equal(p.envelope(ipa_b, tts::zero_distribution),
	          {{   0, { 180, css::frequency::hertz }},
	           { 100, { 180, css::frequency::hertz }}});
}

TEST_CASE("envelope - (start,-,end) tone")
{
	ipa::phoneme ipa_b = ipa::voiced | ipa::bilabial | ipa::plosive;

	tts::pitch_model p{ { 100, css::frequency::hertz },
	                    {  20, css::frequency::hertz },
	                    {   5, css::frequency::hertz } };

	// rising tone

	ipa_b.set(ipa::tone_start_low, ipa::tone_start);
	ipa_b.set(ipa::tone_end_high, ipa::tone_end);
	test_equal(p.envelope(ipa_b, tts::zero_distribution),
	          {{   0, { 120, css::frequency::hertz }},
	           { 100, { 160, css::frequency::hertz }}});

	// falling tone

	ipa_b.set(ipa::tone_start_top, ipa::tone_start);
	ipa_b.set(ipa::tone_end_mid, ipa::tone_end);
	test_equal(p.envelope(ipa_b, tts::zero_distribution),
	          {{   0, { 180, css::frequency::hertz }},
	           { 100, { 140, css::frequency::hertz }}});
}

TEST_CASE("envelope - (start,middle,end) tone")
{
	ipa::phoneme ipa_b = ipa::voiced | ipa::bilabial | ipa::plosive;

	tts::pitch_model p{ { 100, css::frequency::hertz },
	                    {  20, css::frequency::hertz },
	                    {   5, css::frequency::hertz } };

	// peaking tone

	ipa_b.set(ipa::tone_start_bottom, ipa::tone_start);
	ipa_b.set(ipa::tone_middle_top, ipa::tone_middle);
	ipa_b.set(ipa::tone_end_high, ipa::tone_end);
	test_equal(p.envelope(ipa_b, tts::zero_distribution),
	          {{   0, { 100, css::frequency::hertz }},
	           {  50, { 180, css::frequency::hertz }},
	           { 100, { 160, css::frequency::hertz }}});

	// dipping tone

	ipa_b.set(ipa::tone_start_high, ipa::tone_start);
	ipa_b.set(ipa::tone_middle_low, ipa::tone_middle);
	ipa_b.set(ipa::tone_end_mid, ipa::tone_end);
	test_equal(p.envelope(ipa_b, tts::zero_distribution),
	          {{   0, { 160, css::frequency::hertz }},
	           {  50, { 120, css::frequency::hertz }},
	           { 100, { 140, css::frequency::hertz }}});
}
