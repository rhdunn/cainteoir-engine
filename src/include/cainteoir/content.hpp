/* Document Content Rendering Model.
 *
 * Copyright (C) 2012-2013 Reece H. Dunn
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
#include <map>

namespace cainteoir { namespace css
{
	// CSS Values and Units:

	struct length
	{
		enum type
		{
			inherit,
			millimeters,
			centimeters,
			inches,
			points,
			picas,
			pixels,
		};

		length()
			: mValue(0)
			, mUnits(type::inherit)
		{
		}

		length(float aValue, const type aUnits)
			: mValue(aValue)
			, mUnits(aUnits)
		{
		}

		length as(const type aUnits) const;

		float value() const { return mValue; }
		type  units() const { return mUnits; }
	private:
		float mValue;
		type  mUnits;
	};

	// Cascading Style Sheets

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
		paragraph,
		sentence,
	};

	struct box
	{
		length left;
		length top;
		length right;
		length bottom;
	};

	// CSS Fonts

	enum class font_style : uint8_t
	{
		inherit,
		normal,
		italic,
		oblique,
	};

	enum class font_variant_caps : uint8_t
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

	// CSS Counter Styles

	enum class counter_system : uint8_t
	{
		/** @brief Repeatedly cycles through the specified symbols.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#cyclic-system
		  */
		cyclic,

		/** @brief Runs through the symbols once, then falls back.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#fixed-system
		  */
		fixed,

		/** @brief As cyclic, but doubles, triples, etc. the symbols on each pass.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#symbolic-system
		  */
		symbolic,

		/** @brief As with numeric, but using an alphabet instead of numeric digits.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#alphabetic-system
		  */
		alphabetic,

		/** @brief Interprets the symbols as digits to a place value numbering system.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#numeric-system
		  */
		numeric,

		/** @brief Uses a combination of symbols where their sum is the counter value.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#additive-system
		  */
		additive,
	};

	struct counter_style
	{
		/** @brief The type used for counter values.
		  */
		typedef int value_t;

		/** @brief The type used to specify counter ranges.
		  */
		typedef std::pair<value_t, value_t> range_t;

		/** @brief The algorithm to use to format the counter value.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-system
		  */
		cainteoir::css::counter_system system;

		/** @brief The value of the first symbol (for the fixed system).
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#fixed-system
		  */
		int initial_symbol_value;

		/** @brief The set of symbols to use for non-additive systems.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-symbols
		  */
		std::vector<std::string> symbols;

		/** @brief The set of symbols to use for the additive system.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-symbols
		  */
		std::vector<std::pair<value_t, std::string>> additive_symbols;

		/** @brief The string to use to denote negative numbers (before the number).
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-negative
		  */
		std::string negative_prefix;

		/** @brief The string to use to denote negative numbers (after the number).
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-negative
		  */
		std::string negative_suffix;

		/** @brief The string to use at the start of the formatted counter value.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-prefix
		  */
		std::string prefix;

		/** @brief The string to use at the end of the formatted counter value.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-suffix
		  */
		std::string suffix;

		/** @brief Defines the range over which the counter style is defined.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-range
		  */
		range_t range;

		/** @brief The counter to use when this counter cannot represent a value.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-fallback
		  */
		const counter_style *fallback;

		/** @brief The value for the 'infinite infinite' range.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-range
		  */
		static const range_t infinite;

		/** @brief The range to use for auto ranges based on the counter system.
		  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-range
		  *
		  * @param system The counter system to get the auto range for.
		  * @return       The range to use for the auto range.
		  */
		static const range_t get_auto_range(counter_system system);

		/** @brief Construct a counter style with default values.
		  */
		counter_style()
			: system(counter_system::symbolic)
			, initial_symbol_value(1)
			, negative_prefix("-")
			, suffix(".")
			, range(get_auto_range(counter_system::symbolic))
			, fallback(nullptr)
		{
		}

		/** @brief Format a count value using the counter style.
		  *
		  * @param count The numeric value of the counter to format.
		  * @return      The formatted value.
		  */
		std::string marker(value_t count) const;
	};

	// Cascading Style Sheets

	struct styles
	{
		std::string name;
		cainteoir::css::display display;
		cainteoir::css::vertical_align vertical_align;
		cainteoir::css::text_align text_align;
		cainteoir::css::text_decoration text_decoration;
		cainteoir::css::font_style font_style;
		cainteoir::css::font_variant_caps font_variant_caps;
		cainteoir::css::font_weight font_weight;
		std::string list_style_type;
		std::string font_family;
		cainteoir::css::length font_size;
		cainteoir::css::box margin;

		// Cainteoir Text-to-Speech specific styles (not in CSS spec):

		cainteoir::css::text_structure text_structure;
		int toc_level;

		styles(const std::string &aName)
			: name(aName)
			, display(cainteoir::css::display::inherit)
			, vertical_align(cainteoir::css::vertical_align::inherit)
			, text_align(cainteoir::css::text_align::inherit)
			, text_decoration(cainteoir::css::text_decoration::inherit)
			, font_style(cainteoir::css::font_style::inherit)
			, font_variant_caps(cainteoir::css::font_variant_caps::inherit)
			, font_weight(cainteoir::css::font_weight::inherit)
			, text_structure(cainteoir::css::text_structure::none)
			, toc_level(0)
		{
		}

		styles(const std::string &aName,
		       cainteoir::css::display aDisplay,
		       cainteoir::css::vertical_align aVerticalAlign,
		       cainteoir::css::text_align aTextAlign,
		       cainteoir::css::text_decoration aTextDecoration,
		       cainteoir::css::font_style aFontStyle,
		       cainteoir::css::font_variant_caps aFontVariantCaps,
		       cainteoir::css::font_weight aFontWeight,
		       const std::string &aListStyleType,
		       const std::string &aFontFamily,
		       const cainteoir::css::length &aFontSize,
		       const cainteoir::css::box &aMargin,
		       const cainteoir::css::text_structure aTextStructure,
		       int aTocLevel)
			: name(aName)
			, display(aDisplay)
			, vertical_align(aVerticalAlign)
			, text_align(aTextAlign)
			, text_decoration(aTextDecoration)
			, font_style(aFontStyle)
			, font_variant_caps(aFontVariantCaps)
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

	struct style_manager
	{
		const counter_style *get_counter_style(const std::string &aName) const;

		counter_style *create_counter_style(const std::string &aName);

		const std::map<std::string, const counter_style *> &counter_styles() const
		{
			return mCounterStyles;
		}

		void parse(const char *css_file);

		void parse(const std::shared_ptr<buffer> &style);
	private:
		std::list<std::shared_ptr<counter_style>>    mCounterStyleRegistry;
		std::map<std::string, const counter_style *> mCounterStyles;
	};
}}

#ifndef DOXYGEN
namespace cainteoir
{
	// These style definitions are defined here temporarily. They will be
	// removed once the CSS infrastructure is in place and a common XML
	// document reader is using that CSS infrastructure to process all the
	// supported XML document formats.

	extern const css::styles unknown;
	extern const css::styles paragraph;
	extern const css::styles heading0;
	extern const css::styles heading1;
	extern const css::styles heading2;
	extern const css::styles heading3;
	extern const css::styles heading4;
	extern const css::styles heading5;
	extern const css::styles heading6;
	extern const css::styles span;
	extern const css::styles sentence;
	extern const css::styles superscript;
	extern const css::styles subscript;
	extern const css::styles emphasized;
	extern const css::styles emphasized_block;
	extern const css::styles strong;
	extern const css::styles reduced;
	extern const css::styles underlined;
	extern const css::styles monospace;
	extern const css::styles monospace_block;
	extern const css::styles bullet_list;
	extern const css::styles number_list;
	extern const css::styles list_item;
	extern const css::styles table;
	extern const css::styles table_row;
	extern const css::styles table_cell;
}
#endif

#endif
