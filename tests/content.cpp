/* Document Content Rendering Model tests.
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

#include <cainteoir/content.hpp>
#include <cstdio>

#include "tester.hpp"

REGISTER_TESTSUITE("content");

TEST_CASE("style defaults")
{
	cainteoir::styles styles("defaults");
	assert(styles.name == "defaults");
	assert(styles.display == cainteoir::display::inherit);
	assert(styles.vertical_align == cainteoir::vertical_align::inherit);
	assert(styles.text_align == cainteoir::text_align::inherit);
	assert(styles.text_decoration == cainteoir::text_decoration::inherit);
	assert(styles.font_style == cainteoir::font_style::inherit);
	assert(styles.font_variant == cainteoir::font_variant::inherit);
	assert(styles.font_weight == cainteoir::font_weight::inherit);
	assert(styles.font_family.empty());
	assert(styles.font_size == 0);
}
