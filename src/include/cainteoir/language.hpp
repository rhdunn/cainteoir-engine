/* Language Support.
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

#ifndef CAINTEOIR_ENGINE_LANGUAGE_HPP
#define CAINTEOIR_ENGINE_LANGUAGE_HPP

#include "phoneme.hpp"
#include "locale.hpp"

namespace cainteoir { namespace tts
{
	std::shared_ptr<phoneme_reader>
	createPronunciationRules(const char *aRuleSetPath,
	                         const cainteoir::language::tag &aLocale);

	void compile_language(const char *aFileName, FILE *aOutput);
}}

#endif
