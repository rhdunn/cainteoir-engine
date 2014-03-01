/* Phonemeset API.
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

#ifndef CAINTEOIR_ENGINE_PHONEME_PHONEMESET_HPP
#define CAINTEOIR_ENGINE_PHONEME_PHONEMESET_HPP

#include <cainteoir/phoneme.hpp>
#include <vector>

namespace cainteoir { namespace tts
{
	std::pair<bool, phoneme> read_explicit_feature(const char * &mCurrent, const char *mEnd);

	struct phoneme_file_reader
	{
		std::string phoneme_type;

		std::shared_ptr<buffer> transcription;
		std::vector<phoneme> phonemes;

		phoneme_file_reader(const std::string &aPhonemeSet);

		bool read();
	private:
		std::shared_ptr<phoneme_reader> mFeatures;
		std::shared_ptr<buffer> mBuffer;
		const char *mCurrent;
		const char *mLast;
	};

	std::shared_ptr<phoneme_reader> createExplicitFeaturePhonemeReader();
	std::shared_ptr<phoneme_writer> createExplicitFeaturePhonemeWriter();

	std::shared_ptr<phoneme_reader> createIpaPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createIpaPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);
}}

#endif
