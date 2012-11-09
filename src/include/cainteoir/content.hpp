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
#include <vector>

namespace cainteoir
{
	enum class display : uint8_t
	{
		inherit,
		block,
		inlined,
		list_item,
		table,
		table_row,
		table_cell,
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

	enum class text_structure : uint8_t
	{
		none,
		heading,
		sentence,
	};

	enum class counter_type : uint8_t
	{
		cyclic,
		numeric,
	};

	enum class size_units : uint8_t
	{
		inherit,
		millimeters,
		centimeters,
		inches,
		points,
		picas,
		pixels,
	};

	struct counter_style
	{
		std::string name;
		cainteoir::counter_type type;
		std::string prefix;
		std::string suffix;
		std::vector<std::string> symbols;
	};

	struct size
	{
		size()
			: mValue(0)
			, mUnits(size_units::inherit)
		{
		}

		size(float aValue, const size_units aUnits)
			: mValue(aValue)
			, mUnits(aUnits)
		{
		}

		size as(const size_units aUnits) const;

		float      value() const { return mValue; }
		size_units units() const { return mUnits; }
	private:
		float      mValue;
		size_units mUnits;
	};

	struct margin
	{
		size left;
		size top;
		size right;
		size bottom;
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
		const cainteoir::counter_style *list_style_type;
		std::string font_family;
		cainteoir::size font_size;
		cainteoir::margin margin;

		// Cainteoir Text-to-Speech specific styles (not in CSS spec):

		cainteoir::text_structure text_structure;
		int toc_level;

		styles(const std::string &aName)
			: name(aName)
			, display(cainteoir::display::inherit)
			, vertical_align(cainteoir::vertical_align::inherit)
			, text_align(cainteoir::text_align::inherit)
			, text_decoration(cainteoir::text_decoration::inherit)
			, font_style(cainteoir::font_style::inherit)
			, font_variant(cainteoir::font_variant::inherit)
			, font_weight(cainteoir::font_weight::inherit)
			, list_style_type(nullptr)
			, text_structure(cainteoir::text_structure::none)
			, toc_level(0)
		{
		}

		styles(const std::string &aName,
		       cainteoir::display aDisplay,
		       cainteoir::vertical_align aVerticalAlign,
		       cainteoir::text_align aTextAlign,
		       cainteoir::text_decoration aTextDecoration,
		       cainteoir::font_style aFontStyle,
		       cainteoir::font_variant aFontVariant,
		       cainteoir::font_weight aFontWeight,
		       const cainteoir::counter_style *aListStyleType,
		       const std::string &aFontFamily,
		       const cainteoir::size &aFontSize,
		       const cainteoir::margin &aMargin,
		       const cainteoir::text_structure aTextStructure,
		       int aTocLevel)
			: name(aName)
			, display(aDisplay)
			, vertical_align(aVerticalAlign)
			, text_align(aTextAlign)
			, text_decoration(aTextDecoration)
			, font_style(aFontStyle)
			, font_variant(aFontVariant)
			, font_weight(aFontWeight)
			, list_style_type(aListStyleType)
			, font_family(aFontFamily)
			, font_size(aFontSize)
			, margin(aMargin)
			, text_structure(aTextStructure)
			, toc_level(aTocLevel)
		{
		}
	};

	namespace counter
	{
		extern const counter_style decimal;
		extern const counter_style disc;
	}

	extern const styles unknown;
	extern const styles paragraph;
	extern const styles heading0;
	extern const styles heading1;
	extern const styles heading2;
	extern const styles heading3;
	extern const styles heading4;
	extern const styles heading5;
	extern const styles heading6;
	extern const styles span;
	extern const styles sentence;
	extern const styles superscript;
	extern const styles subscript;
	extern const styles emphasized;
	extern const styles emphasized_block;
	extern const styles strong;
	extern const styles reduced;
	extern const styles underlined;
	extern const styles monospace;
	extern const styles monospace_block;
	extern const styles bullet_list;
	extern const styles number_list;
	extern const styles list_item;
	extern const styles table;
	extern const styles table_row;
	extern const styles table_cell;
}

#endif
