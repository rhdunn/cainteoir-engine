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
#include <cainteoir/trie.hpp>
#include <vector>
#include <stack>
#include <map>

namespace cainteoir { namespace tts
{
	/*
	 * The following diagram details the collaboration between the different helper functions
	 * and the phonemeset types:
	 *
	 *========================================================================================*
	 *                                                                                        *
	 * .==============[ READER ]==============.   .===============[ WRITER ]================. *
	 * |                                      |   |                                         | *
	 *                                                                                        *
	 *   [feature]                                                              [feature]     *
	 *       |                                                                      |         *
	 *       v                                                                      v         *
	 * read_explicit_feature <-------- phoneme_file_reader           write_explicit_feature   *
	 *       ^                                  ^                                   ^         *
	 *       |                                  |                                   |         *
	 *       |                    .-------------+------------.                      |         *
	 *       |                    |             |            |                      |         *
	 * [kirshenbaum] ---> transcription_reader  |  transcription_writer <---- [kirshenbaum]   *
	 *                     ^      ^             |            ^      ^                         *
	 *                     |      |         [arpabet]        |      |                         *
	 *                     |      |                          |      |                         *
	 *                     |    [ipa]                      [ipa]    |                         *
	 *                     |                                        |                         *
	 *                  [espeak]                                 [espeak]                     *
	 *                                                                                        *
	 *========================================================================================*
	 */

	std::pair<bool, phoneme> read_explicit_feature(const char * &mCurrent, const char *mEnd);
	void write_explicit_feature(FILE *output, const tts::phoneme &aPhoneme);

	struct phoneme_file_reader
	{
		std::string phoneme_type;

		std::shared_ptr<buffer> transcription;
		std::vector<phoneme> phonemes;

		phoneme_file_reader(const std::string &aPhonemeSet);

		bool read();
	private:
		struct context_t
		{
			std::shared_ptr<buffer> mBuffer;
			const char *mCurrent;
			const char *mLast;

			context_t(const std::string &aPhonemeSet);
		};

		std::stack<context_t> mFiles;
	};

	struct transcription_reader
	{
		struct phoneme_t
		{
			tts::phoneme phoneme;

			phoneme_t(const tts::phoneme &aPhoneme = tts::phoneme(-1))
				: phoneme(aPhoneme)
			{
			}
		};

		transcription_reader(tts::phoneme_file_reader &aPhonemeSet);

		std::pair<bool, tts::phoneme> read(const char * &mCurrent, const char *mEnd) const;
	private:
		cainteoir::trie<phoneme_t> mPhonemes;
	};

	struct transcription_writer
	{
		transcription_writer(tts::phoneme_file_reader &aPhonemeSet);

		bool write(FILE *aOutput, const tts::phoneme &aPhoneme) const;
	private:
		std::map<tts::phoneme, std::shared_ptr<cainteoir::buffer>> mPhonemes;
	};

	std::shared_ptr<phoneme_reader> createExplicitFeaturePhonemeReader();
	std::shared_ptr<phoneme_writer> createExplicitFeaturePhonemeWriter();

	std::shared_ptr<phoneme_reader> createIpaPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createIpaPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);
}}

#endif
