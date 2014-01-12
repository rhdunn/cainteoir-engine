/* Text Processing API.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_TEXT_HPP
#define CAINTEOIR_ENGINE_TEXT_HPP

#include "document.hpp"
#include "languages.hpp"
#include "dictionary.hpp"
#include "phoneme.hpp"

#include <queue>
#include <list>
#include <unordered_map>

namespace cainteoir { namespace tts
{
	std::shared_ptr<phoneme_reader> createPronunciationRules(const char *aRuleSetPath);

	enum event_type
	{
		error,
		word_uppercase,
		word_lowercase,
		word_capitalized,
		word_mixedcase,
		word_script,
		number,
		ordinal_number,
		punctuation,
		comma,
		semicolon,
		colon,
		ellipsis,
		full_stop,
		double_stop,
		exclamation,
		question,
		symbol,
		paragraph,
		phonemes,
		en_dash,
		em_dash,
	};

	struct text_event
	{
		std::shared_ptr<buffer> text;
		std::list<phoneme> phonemes;
		event_type type;
		cainteoir::range<uint32_t> range;
		uint32_t codepoint;

		text_event(const std::shared_ptr<buffer> &aText,
		           event_type aType,
		           const cainteoir::range<uint32_t> &aRange,
		           uint32_t aCodePoint)
			: text(aText)
			, type(aType)
			, range(aRange)
			, codepoint(aCodePoint)
		{
		}

		text_event(event_type aType,
		           const cainteoir::range<uint32_t> &aRange,
		           uint32_t aCodePoint)
			: type(aType)
			, range(aRange)
			, codepoint(aCodePoint)
		{
		}

		text_event()
			: range(0, 0)
		{
		}
	};

	struct text_reader
	{
		text_reader(const std::shared_ptr<document_reader> &aReader);

		const text_event &event() const { return mMatch; }

		bool read();
	private:
		bool matched();

		enum class reader_state;

		std::shared_ptr<document_reader> mReader;
		text_event mMatch;

		char mMatchBuffer[512];
		char *mMatchCurrent;
		uint32_t mMatchNext;
		uint32_t mMatchLast;

		bool mNeedEndPara;
		const char *mCurrent;
		const char *mLast;
		reader_state mReaderState;
		uint8_t mState;
	};

	struct context_analysis
	{
	public:
		context_analysis(const std::shared_ptr<document_reader> &aReader);

		const text_event &event() const { return mClause.front(); }

		bool read();
	private:
		bool read_clause();

		bool mHaveEvent;
		text_reader mReader;
		std::queue<text_event> mClause;
	};

	struct word_stream
	{
	public:
		enum number_scale
		{
			short_scale,
			long_scale,
		};

		word_stream(const std::shared_ptr<document_reader> &aReader,
		            const language::tag &aLocale,
		            number_scale aScale);

		const text_event &event() const { return mEntries.front(); }

		bool read();
	private:
		context_analysis mReader;
		std::queue<text_event> mEntries;

		dictionary mCardinals;
		dictionary mOrdinals;
	};

	struct phoneme_stream
	{
	public:
		phoneme_stream(const std::shared_ptr<document_reader> &aReader,
		               const language::tag &aLocale,
		               word_stream::number_scale aScale,
		               const std::shared_ptr<phoneme_reader> &aRules,
		               const std::shared_ptr<dictionary_reader> &aExceptionDictionary);

		const text_event &event() const { return mEvent; }

		bool read();
	private:
		bool pronounce(const std::shared_ptr<buffer> &aText, const range<uint32_t> &aRange);

		word_stream mReader;
		text_event mEvent;
		std::shared_ptr<phoneme_reader> mRules;
		dictionary mExceptionDictionary;
	};

	void generate_phonemes(tts::phoneme_stream &reader,
	                       const char *phonemeset,
	                       tts::stress_type stress,
	                       const char *open,
	                       const char *close,
	                       const char *phrase = " ");
}}

#endif
