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
	 *   [feature]                           [espeak]                           [feature]     *
	 *       |                                  |                                   |         *
	 *       v                                  v                                   v         *
	 * read_explicit_feature <-------- phoneme_file_reader           write_explicit_feature   *
	 *       ^                                  ^                                   ^         *
	 *       |                                  |                                   |         *
	 *       |                    .-------------+------------.                      |         *
	 *       |                    |             |            |                      |         *
	 * [kirshenbaum] ---> transcription_reader  |  transcription_writer <---- [kirshenbaum]   *
	 *                            ^             |            ^                                *
	 *                            |             |            |                                *
	 *                          [ipa]       [arpabet]      [ipa]                              *
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
		tone,
		error,
	};

	struct feature_t
	{
		friend struct phoneme_file_reader;

		feature_t()
			: context(0)
		{
			feature[0] = feature[1] = feature[2] = feature[3] = 0;
		}

		feature_t(char aContext, const char (&aFeature)[4])
			: context(aContext)
		{
			feature[0] = aFeature[0];
			feature[1] = aFeature[1];
			feature[2] = aFeature[2];
			feature[3] = aFeature[3];
		}

		bool in(const tts::phoneme &aPhoneme) const
		{
			return context == 0 || aPhoneme.get(feature);
		}

		operator const char *() const { return feature; }

		const char type() const { return context; }
	private:
		char context;
		char feature[4];
	};

	struct phoneme_file_reader
	{
		std::string phoneme_type;

		std::shared_ptr<buffer> transcription;
		std::vector<phoneme> phonemes;
		feature_t feature;
		feature_t change_to;
		feature_t context;
		placement type;
		uint8_t tone_level;

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

		void read_feature(feature_t &aFeature);
	};

	struct transcription_reader
	{
		transcription_reader(tts::phoneme_file_reader &aPhonemeSet);

		std::pair<bool, tts::phoneme> read(const char * &mCurrent, const char *mEnd) const;
	private:
		struct phoneme_rule_t
		{
			feature_t feature;
			feature_t context1;
			feature_t context2;

			phoneme_rule_t(const feature_t &aFeature, const feature_t &aContext1, const feature_t &aContext2 = {})
				: feature(aFeature)
				, context1(aContext1)
				, context2(aContext2)
			{
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

			phoneme_t(uint8_t aToneLevel)
				: phoneme(aToneLevel)
				, type(placement::tone)
			{
			}
		};

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
			feature_t feature;
			feature_t change_to;
			feature_t context;
			std::shared_ptr<cainteoir::buffer> transcription;

			feature_rule_t(const phoneme_file_reader &aRule)
				: feature(aRule.feature)
				, change_to(aRule.change_to)
				, context(aRule.context)
				, transcription(aRule.transcription)
			{
			}
		};

		std::map<tts::phoneme, std::shared_ptr<cainteoir::buffer>> mPhonemes;
		std::shared_ptr<cainteoir::buffer> mTones[5];
		std::list<feature_rule_t> mBefore;
		std::list<feature_rule_t> mAfter;
		std::list<feature_rule_t> mModifiers;
	};

	std::shared_ptr<phoneme_reader> createExplicitFeaturePhonemeReader();
	std::shared_ptr<phoneme_writer> createExplicitFeaturePhonemeWriter();

	std::shared_ptr<phoneme_reader> createIpaPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createIpaPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);

	std::shared_ptr<phoneme_reader> createKirshenbaumPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createKirshenbaumPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);

	std::shared_ptr<phoneme_reader> createArpabetPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createArpabetPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);

	std::shared_ptr<phoneme_reader> createEspeakPhonemeReader(phoneme_file_reader &aPhonemeSet, const char *aName);
	std::shared_ptr<phoneme_writer> createEspeakPhonemeWriter(phoneme_file_reader &aPhonemeSet, const char *aName);
}}

#endif
