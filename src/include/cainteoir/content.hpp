/* Document Content Rendering Model.
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

#ifndef CAINTEOIR_ENGINE_CONTENT_HPP
#define CAINTEOIR_ENGINE_CONTENT_HPP

#include "buffer.hpp"

namespace cainteoir
{
	enum class display : uint8_t
	{
		inherit,
		block,
		inlined,
		none,
	};

	enum class vertical_align : uint8_t
	{
		inherit,
		baseline,
		sub,
		super,
	};

	enum class font_style : uint8_t
	{
		inherit,
		normal,
		italic,
		oblique,
	};

	enum class font_variant : uint8_t
	{
		inherit,
		normal,
		small_caps,
	};

	enum class font_weight : uint16_t
	{
		inherit,
		normal = 400,
		bold = 700,
	};

	enum class text_align : uint8_t
	{
		inherit,
		left,
		right,
		center,
		justify,
	};

	enum class text_decoration : uint8_t
	{
		inherit,
		none,
		underline,
		line_through,
	};

	struct styles
	{
		std::string name;
		cainteoir::display display;
		cainteoir::vertical_align vertical_align;
		cainteoir::text_align text_align;
		cainteoir::text_decoration text_decoration;
		cainteoir::font_style font_style;
		cainteoir::font_variant font_variant;
		cainteoir::font_weight font_weight;
		std::string font_family;
		int font_size;

		styles(const std::string &aName)
			: name(aName)
			, display(cainteoir::display::inherit)
			, vertical_align(cainteoir::vertical_align::inherit)
			, text_align(cainteoir::text_align::inherit)
			, text_decoration(cainteoir::text_decoration::inherit)
			, font_style(cainteoir::font_style::inherit)
			, font_variant(cainteoir::font_variant::inherit)
			, font_weight(cainteoir::font_weight::inherit)
			, font_size(0)
		{
		}
	};
}

#endif
