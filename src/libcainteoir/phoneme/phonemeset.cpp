/* Phoneme Set Reader/Writer.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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

#include "config.h"
#include "i18n.h"
#include "compatibility.hpp"

#include "phonemeset.hpp"

namespace tts = cainteoir::tts;

void tts::phoneme_writer::flush()
{
}

std::shared_ptr<tts::phoneme_parser> tts::createPhonemeParser(const char *aPhonemeSet)
{
	if (!strcmp(aPhonemeSet, "features"))
		return createExplicitFeaturePhonemeParser();

	phoneme_file_reader phonemes(aPhonemeSet);
	if (phonemes.phoneme_type == "ipa")
		return createIpaPhonemeParser(phonemes, aPhonemeSet);
	if (phonemes.phoneme_type == "kirshenbaum")
		return createKirshenbaumPhonemeParser(phonemes, aPhonemeSet);
	if (phonemes.phoneme_type == "arpabet")
		return createArpabetPhonemeParser(phonemes, aPhonemeSet, arpabet_variant::arpabet);
	if (phonemes.phoneme_type == "festvox")
		return createArpabetPhonemeParser(phonemes, aPhonemeSet, arpabet_variant::festvox);
	if (phonemes.phoneme_type == "espeak")
		return createEspeakPhonemeParser(phonemes, aPhonemeSet);

	throw std::runtime_error("the phonemeset is not supported");
}

std::shared_ptr<tts::phoneme_reader> tts::createPhonemeReader(const char *aPhonemeSet)
{
	if (!strcmp(aPhonemeSet, "features"))
		return createExplicitFeaturePhonemeReader();

	phoneme_file_reader phonemes(aPhonemeSet);
	if (phonemes.phoneme_type == "ipa")
		return createIpaPhonemeReader(phonemes, aPhonemeSet);
	if (phonemes.phoneme_type == "kirshenbaum")
		return createKirshenbaumPhonemeReader(phonemes, aPhonemeSet);
	if (phonemes.phoneme_type == "arpabet")
		return createArpabetPhonemeReader(phonemes, aPhonemeSet, arpabet_variant::arpabet);
	if (phonemes.phoneme_type == "festvox")
		return createArpabetPhonemeReader(phonemes, aPhonemeSet, arpabet_variant::festvox);
	if (phonemes.phoneme_type == "espeak")
		return createEspeakPhonemeReader(phonemes, aPhonemeSet);

	throw std::runtime_error("the phonemeset is not supported");
}

std::shared_ptr<tts::phoneme_writer> tts::createPhonemeWriter(const char *aPhonemeSet)
{
	if (!strcmp(aPhonemeSet, "features"))
		return createExplicitFeaturePhonemeWriter();

	phoneme_file_reader phonemes(aPhonemeSet);
	if (phonemes.phoneme_type == "ipa")
		return createIpaPhonemeWriter(phonemes, aPhonemeSet);
	if (phonemes.phoneme_type == "kirshenbaum")
		return createKirshenbaumPhonemeWriter(phonemes, aPhonemeSet);
	if (phonemes.phoneme_type == "arpabet")
		return createArpabetPhonemeWriter(phonemes, aPhonemeSet, arpabet_variant::arpabet);
	if (phonemes.phoneme_type == "festvox")
		return createArpabetPhonemeWriter(phonemes, aPhonemeSet, arpabet_variant::festvox);
	if (phonemes.phoneme_type == "espeak")
		return createEspeakPhonemeWriter(phonemes, aPhonemeSet);

	throw std::runtime_error("the phonemeset is not supported");
}
