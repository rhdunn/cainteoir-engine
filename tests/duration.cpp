/* Test for the phoneme duration model.
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

REGISTER_TESTSUITE("duration");

namespace tts = cainteoir::tts;
namespace css = cainteoir::css;

template <typename UnitT> struct unit_type;
template <> struct unit_type<css::time::type> { typedef css::time type; };

template <typename UnitT>
static void test_equal_(const typename unit_type<UnitT>::type aObject,
                        float aValue, const UnitT aUnits,
                        const char *location, int line)
{
	assert_location(aObject.units() == aUnits, location, line);
	assert_location(aObject.value() == aValue, location, line);
}

#define test_equal(a, b, c) test_equal_(a, b, c, __FILE__, __LINE__)

TEST_CASE("default values")
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

TEST_CASE("(mean, std-dev) in seconds") // [1]
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

TEST_CASE("(mean, std-dev) in milliseconds") // [2]
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

TEST_CASE("(mean, std-dev) with mismatched units")
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
