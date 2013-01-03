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


/** @namespace cainteoir::css
  * @brief     Cascading Style Sheet (CSS) implementation
  */


/** @defgroup css_units CSS Values and Units
  * @brief    Implementation of the CSS Values and Units Level 3 specification.
  * @see      http://www.w3.org/TR/css3-values/
  *
  * This is the C++ API to the various unit types for CSS values.
  */


/** @defgroup css_counter_styles CSS Counter Styles
  * @brief    Implementation of the CSS Counter Styles Level 3 specification.
  * @see      http://www.w3.org/TR/css-counter-styles-3/
  *
  * This is the C++ API to CSS counter styles as defined in the CSS Counter
  * Styles Level 3 specification. The CSS parsing of \@counter-style rules
  * is handled in the cainteoir::css::style_manager::parse method.
  */


/// @addtogroup css_units
/// @{

/** @struct cainteoir::css::length
  * @brief  Represents a CSS length.
  * @see    http://www.w3.org/TR/css3-values/#lengths
  */

/// @}
/// @addtogroup css_counter_styles
/// @{


/** @enum  cainteoir::css::counter_system
  * @brief The algorithm used to construct the counter's representation.
  * @see   http://www.w3.org/TR/css-counter-styles-3/#counter-style-system
  */


/** @struct cainteoir::css::counter_style
  * @brief  Represents a \@counter-style CSS rule.
  * @see    http://www.w3.org/TR/css-counter-styles-3/#the-csscounterstylerule-interface
  */

/// @}
