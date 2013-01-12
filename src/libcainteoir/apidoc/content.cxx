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

/// @addtogroup css_units
/// @{

/** @struct cainteoir::css::length
  * @brief  Represents a CSS length.
  * @see    http://www.w3.org/TR/css3-values/#lengths
  */

/// @}

/** @enum  cainteoir::css::length::type
  * @brief The units in which the length is specified.
  */

/** @var   cainteoir::css::length::type cainteoir::css::length::inherit
  * @brief The length value is inherited from the parent object.
  */

/** @var   cainteoir::css::length::type cainteoir::css::length::millimeters
  * @brief The mm unit type.
  */

/** @var   cainteoir::css::length::type cainteoir::css::length::centimeters
  * @brief The cm unit type.
  */

/** @var   cainteoir::css::length::type cainteoir::css::length::inches
  * @brief The in unit type.
  */

/** @var   cainteoir::css::length::type cainteoir::css::length::points
  * @brief The pt unit type.
  */

/** @var   cainteoir::css::length::type cainteoir::css::length::picas
  * @brief The pc unit type.
  */

/** @var   cainteoir::css::length::type cainteoir::css::length::pixels
  * @brief The px unit type.
  */

/** @fn    cainteoir::css::length::length()
  * @brief Create a default valued length object.
  */

/** @fn    cainteoir::css::length::length(float aValue, type aUnits)
  * @brief Create a length object.
  *
  * @param[in] aValue The size of the length.
  * @param[in] aUnits The units the length is specified in.
  */

/** @fn    cainteoir::css::length cainteoir::css::length::as(const type aUnits) const
  * @brief Convert the length to the specified units.
  *
  * @param[in] aUnits The units to convert to.
  *
  * @return A new length object in the specified units.
  */

/** @fn    cainteoir::css::length::type cainteoir::css::length::units() const
  * @brief Gets the units the length is specified in.
  *
  * @return The units the length is specified in.
  */

/** @fn    float cainteoir::css::length::value() const
  * @brief Gets the scalar value of the length.
  *
  * @return The scalar value of the length.
  */

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
