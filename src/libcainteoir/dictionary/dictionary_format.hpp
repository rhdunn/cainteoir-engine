/* Phonemeset API.
 *
 * Copyright (C) 2015 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_DICTIONARY_DICTIONARY_FORMAT_HPP
#define CAINTEOIR_ENGINE_DICTIONARY_DICTIONARY_FORMAT_HPP

#include <cainteoir/dictionary.hpp>

namespace cainteoir { namespace tts
{
	// readers

	std::shared_ptr<dictionary_reader> createCainteoirDictionaryReader(const char *aDictionaryPath);

	std::shared_ptr<dictionary_reader> createCMUDictionaryReader(const char *aDictionaryPath, const char *aPreferredPhonemeSet);

	// formatters

	std::shared_ptr<dictionary_formatter> createCainteoirDictionaryFormatter(FILE *out);

	std::shared_ptr<dictionary_formatter> createCMUDictionaryFormatter(FILE *out);

	std::shared_ptr<dictionary_formatter> createEspeakDictionaryFormatter(FILE *out);

	std::shared_ptr<dictionary_formatter> createDictionaryEntryFormatter(FILE *out);
}}

#endif
