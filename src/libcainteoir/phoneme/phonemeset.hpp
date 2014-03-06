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

	enum class placement
	{
		none,
		before,
		primary,
		after,
		error,
	};

	struct phoneme_file_reader
	{
		std::string phoneme_type;

		std::shared_ptr<buffer> transcription;
		std::vector<phoneme> phonemes;
		char conditional;
		char applicator;
		char feature[4];
		char context[4];
		placement type;

		phoneme_file_reader(const std::string &aPhonemeSet);

		bool read();
	private:
		enum class state
		{
			prelude,
			need_transcription,
			have_transcription,
			have_context,
		};

		struct context_t
		{
			std::shared_ptr<buffer> mBuffer;
			const char *mCurrent;
			const char *mLast;

			context_t(const std::string &aPhonemeSet);
		};

		std::stack<context_t> mFiles;
		state mState;

		void read_feature(char (&aFeature)[4]);
	};

	struct transcription_reader
	{
		struct phoneme_rule_t
		{
			char feature[4];
			char context[4];

			phoneme_rule_t(const char (&aFeature)[4], const char (&aContext)[4])
			{
				feature[0] = aFeature[0];
				feature[1] = aFeature[1];
				feature[2] = aFeature[2];
				feature[3] = aFeature[3];
				context[0] = aContext[0];
				context[1] = aContext[1];
				context[2] = aContext[2];
				context[3] = aContext[3];
			}
		};

		struct phoneme_t
		{
			tts::phoneme phoneme;
			placement type;
			std::list<phoneme_rule_t> rule;

			phoneme_t(placement aType = placement::none)
				: phoneme(-1)
				, type(aType)
			{
			}

			phoneme_t(const tts::phoneme &aPhoneme)
				: phoneme(aPhoneme)
				, type(placement::primary)
			{
			}
		};

		transcription_reader(tts::phoneme_file_reader &aPhonemeSet);

		std::pair<bool, tts::phoneme> read(const char * &mCurrent, const char *mEnd) const;
	private:
		cainteoir::trie<phoneme_t> mPhonemes;

		std::pair<const char *, const phoneme_t &>
		next_match(const char *mCurrent, const char *mEnd) const;
	};

	struct transcription_writer
	{
		transcription_writer(tts::phoneme_file_reader &aPhonemeSet);

		bool write(FILE *aOutput, const tts::phoneme &aPhoneme) const;
	private:
		struct feature_rule_t
		{
			char feature[4];
			char context[4];
			std::shared_ptr<cainteoir::buffer> transcription;

			feature_rule_t(const char (&aFeature)[4],
			               const char (&aContext)[4],
			               const std::shared_ptr<cainteoir::buffer> aTranscription)
				: transcription(aTranscription)
			{
				feature[0] = aFeature[0];
				feature[1] = aFeature[1];
				feature[2] = aFeature[2];
				feature[3] = aFeature[3];
				context[0] = aContext[0];
				context[1] = aContext[1];
				context[2] = aContext[2];
				context[3] = aContext[3];
			}
		};

		std::map<tts::phoneme, std::shared_ptr<cainteoir::buffer>> mPhonemes;
		std::list<feature_rule_t> mAfter;
		std::list<feature_rule_t> mModifiers;
	};

	std::shared_ptr<phoneme_reader> createExplicitFeaturePhonemeReader();
	std::shared_ptr<phoneme_writer> createExplicitFeaturePhonemeWriter();

	std::shared_ptr<phoneme_reader> createIpaPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createIpaPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);

	std::shared_ptr<phoneme_reader> createKirshenbaumPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createKirshenbaumPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);
}}

#endif
