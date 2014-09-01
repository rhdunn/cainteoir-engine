/* Test for the z-score model.
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

REGISTER_TESTSUITE("z-score");

namespace tts = cainteoir::tts;
namespace css = cainteoir::css;

template <typename UnitT> struct unit_type;
template <> struct unit_type<css::time::type>      { typedef css::time      type; };
template <> struct unit_type<css::frequency::type> { typedef css::frequency type; };

template <typename UnitT>
static void test_equal_(const typename unit_type<UnitT>::type aObject,
                        float aValue, const UnitT aUnits,
                        const char *location, int line)
{
	assert_location(aObject.units() == aUnits, location, line);
	assert_location(aObject.value() == aValue, location, line);
}

#define test_equal(a, b, c) test_equal_(a, b, c, __FILE__, __LINE__)

TEST_CASE("duration default values")
{
	tts::duration dur{};
	test_equal(dur.mean, 0, css::time::inherit);
	test_equal(dur.sdev, 0, css::time::inherit);

	test_equal(dur.value( 0), 0, css::time::inherit);
	test_equal(dur.value( 1), 0, css::time::inherit);
	test_equal(dur.value(-1), 0, css::time::inherit);

	test_equal(dur.value(0.0),  0, css::time::inherit);
	test_equal(dur.value(0.25), 0, css::time::inherit);
	test_equal(dur.value(-0.5), 0, css::time::inherit);
}

TEST_CASE("duration (mean, std-dev) in seconds") // [1]
{
	tts::duration dur{ { 0.4, css::time::seconds }, { 0.05, css::time::seconds } };
	test_equal(dur.mean, 0.4,  css::time::seconds);
	test_equal(dur.sdev, 0.05, css::time::seconds);

	test_equal(dur.value( 0), 0.4,  css::time::seconds);
	test_equal(dur.value( 1), 0.45, css::time::seconds);
	test_equal(dur.value(-1), 0.35, css::time::seconds);

	test_equal(dur.value(0.0),  0.4,    css::time::seconds);
	test_equal(dur.value(0.25), 0.4125, css::time::seconds);
	test_equal(dur.value(-0.5), 0.375,  css::time::seconds);
}

TEST_CASE("duration (mean, std-dev) in milliseconds") // [2]
{
	tts::duration dur{ { 200, css::time::milliseconds }, { 50, css::time::milliseconds } };
	test_equal(dur.mean, 200, css::time::milliseconds);
	test_equal(dur.sdev,  50, css::time::milliseconds);

	test_equal(dur.value( 0), 200, css::time::milliseconds);
	test_equal(dur.value( 1), 250, css::time::milliseconds);
	test_equal(dur.value(-1), 150, css::time::milliseconds);

	test_equal(dur.value(0.0),  200,   css::time::milliseconds);
	test_equal(dur.value(0.25), 212.5, css::time::milliseconds);
	test_equal(dur.value(-0.5), 175,   css::time::milliseconds);
}

TEST_CASE("duration (mean, std-dev) with mismatched units")
{
	tts::duration dur1{ { 150, css::time::milliseconds }, { 0.025, css::time::seconds } };
	test_equal(dur1.mean, 150, css::time::milliseconds);
	test_equal(dur1.sdev,  25, css::time::milliseconds);

	// NOTE: dur1.value() tests covered by [2].

	tts::duration dur2{ { 0.2, css::time::seconds }, { 50, css::time::milliseconds } };
	test_equal(dur2.mean, 0.2,  css::time::seconds);
	test_equal(dur2.sdev, 0.05, css::time::seconds);

	// NOTE: dur2.value() tests covered by [1].

	assert_throws(tts::duration({ 10, css::time::milliseconds }, { 0, css::time::inherit }),
	              std::runtime_error,
	              "unable to convert to the specified units");

	assert_throws(tts::duration({ 0, css::time::inherit }, { 50, css::time::milliseconds }),
	              std::runtime_error,
	              "unable to convert to the specified units");
}

TEST_CASE("pitch default values")
{
	tts::pitch p{};
	test_equal(p.mean, 0, css::frequency::inherit);
	test_equal(p.sdev, 0, css::frequency::inherit);

	test_equal(p.value( 0), 0, css::frequency::inherit);
	test_equal(p.value( 1), 0, css::frequency::inherit);
	test_equal(p.value(-1), 0, css::frequency::inherit);

	test_equal(p.value(0.0),  0, css::frequency::inherit);
	test_equal(p.value(0.25), 0, css::frequency::inherit);
	test_equal(p.value(-0.5), 0, css::frequency::inherit);
}

TEST_CASE("pitch (mean, std-dev) in kilohertz") // [3]
{
	tts::pitch p{ { 0.4, css::frequency::kilohertz }, { 0.05, css::frequency::kilohertz } };
	test_equal(p.mean, 0.4,  css::frequency::kilohertz);
	test_equal(p.sdev, 0.05, css::frequency::kilohertz);

	test_equal(p.value( 0), 0.4,  css::frequency::kilohertz);
	test_equal(p.value( 1), 0.45, css::frequency::kilohertz);
	test_equal(p.value(-1), 0.35, css::frequency::kilohertz);

	test_equal(p.value(0.0),  0.4,    css::frequency::kilohertz);
	test_equal(p.value(0.25), 0.4125, css::frequency::kilohertz);
	test_equal(p.value(-0.5), 0.375,  css::frequency::kilohertz);
}

TEST_CASE("pitch (mean, std-dev) in hertz") // [4]
{
	tts::pitch p{ { 200, css::frequency::hertz }, { 50, css::frequency::hertz } };
	test_equal(p.mean, 200, css::frequency::hertz);
	test_equal(p.sdev,  50, css::frequency::hertz);

	test_equal(p.value( 0), 200, css::frequency::hertz);
	test_equal(p.value( 1), 250, css::frequency::hertz);
	test_equal(p.value(-1), 150, css::frequency::hertz);

	test_equal(p.value(0.0),  200,   css::frequency::hertz);
	test_equal(p.value(0.25), 212.5, css::frequency::hertz);
	test_equal(p.value(-0.5), 175,   css::frequency::hertz);
}

TEST_CASE("pitch (mean, std-dev) with mismatched units")
{
	tts::pitch p1{ { 150, css::frequency::hertz }, { 0.025, css::frequency::kilohertz } };
	test_equal(p1.mean, 150, css::frequency::hertz);
	test_equal(p1.sdev,  25, css::frequency::hertz);

	// NOTE: p1.value() tests covered by [4].

	tts::pitch p2{ { 0.2, css::frequency::kilohertz }, { 50, css::frequency::hertz } };
	test_equal(p2.mean, 0.2,  css::frequency::kilohertz);
	test_equal(p2.sdev, 0.05, css::frequency::kilohertz);

	// NOTE: p2.value() tests covered by [3].

	assert_throws(tts::pitch({ 10, css::frequency::hertz }, { 0, css::frequency::inherit }),
	              std::runtime_error,
	              "unable to convert to the specified units");

	assert_throws(tts::pitch({ 0, css::frequency::inherit }, { 50, css::frequency::hertz }),
	              std::runtime_error,
	              "unable to convert to the specified units");
}
