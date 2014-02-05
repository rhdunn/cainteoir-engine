/* Localization Helpers.
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

#ifndef CAINTEOIR_ENGINE_I18N_H
#define CAINTEOIR_ENGINE_I88N_H

#ifdef ENABLE_NLS

#include <libintl.h>

/** @brief Translate a string to the user's language.
  *
  * @param[in] text The text to be translated.
  * @return    The text in the user's language.
  */
#define i18n(text) gettext(text)

/** @brief Translate a string to the user's language.
  *
  * The context is used by translators to help them disambiguate words and
  * phrases with ambiguous meaning.
  *
  * @param[in] context The context in which the string is used.
  * @param[in] text    The text to be translated.
  * @return    The text in the user's language.
  */
#define i18nc(context, text) gettext(text)

#else

#define i18n(text) text
#define i18nc(context, text) text

#endif

#endif
