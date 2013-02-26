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
#include "path.hpp"

#include <ucd/ucd.h>
#include <queue>
#include <unordered_map>

namespace cainteoir { namespace tts
{
	struct ruleset
	{
		struct rule_t
		{
			const std::shared_ptr<buffer> left;
			const std::shared_ptr<buffer> match;
			const std::shared_ptr<buffer> right;
			const std::shared_ptr<buffer> phonemes;
		};

		void add_rule(const std::shared_ptr<buffer> &aLeft,
		              const std::shared_ptr<buffer> &aMatch,
		              const std::shared_ptr<buffer> &aRight,
		              const std::shared_ptr<buffer> &aPhonemes);

		bool add_rules(const path &aRulesPath);

		std::shared_ptr<cainteoir::buffer> pronounce(const std::shared_ptr<cainteoir::buffer> &aText) const;
	private:
		void add_rules(const std::shared_ptr<buffer> &aDictionary);
		std::list<rule_t> mRules[256];
	};

	ruleset en_rules();

	struct dictionary
	{
		enum entry_type
		{
			no_match,
			say_as,
			phonemes,
		};

		struct entry
		{
			entry_type type;
			ucd::script script;
			std::shared_ptr<buffer> text;
		};

		typedef std::unordered_map<std::string, entry> storage_type;
		typedef storage_type::const_iterator const_iterator;

		bool add_entries(const path &aDictionaryPath);

		void add_entry(const std::string &aEntry,
		               entry_type aType,
		               ucd::script aScript,
		               const std::shared_ptr<buffer> &aDefinition);

		const entry &lookup(const std::string &aEntry) const;

		const entry &lookup(const std::shared_ptr<buffer> &aEntry) const
		{
			return lookup(aEntry->str());
		}

		std::size_t size() const { return mEntries.size(); }

		const_iterator begin() const { return mEntries.begin(); }
		const_iterator end()   const { return mEntries.end();   }
	private:
		void add_entries(const path &aBasePath,
		                 const std::shared_ptr<buffer> &aDictionary);

		std::unordered_map<std::string, entry> mEntries;
	};

	enum event_type
	{
		error,
		word_uppercase,
		word_lowercase,
		word_capitalized,
		word_mixedcase,
		word_script,
		number,
		punctuation,
		comma,
		semicolon,
		colon,
		ellipsis,
		full_stop,
		exclamation,
		question,
		symbol,
		paragraph,
		phonemes,
		pause,
	};

	struct text_event
	{
		std::shared_ptr<buffer> text;
		event_type type;
		ucd::script script;
		cainteoir::range<uint32_t> range;
		union
		{
			ucd::codepoint_t codepoint;
			uint16_t duration;
		};

		text_event(const std::shared_ptr<buffer> &aText,
		           event_type aType,
		           ucd::script aScript,
		           const cainteoir::range<uint32_t> &aRange,
		           ucd::codepoint_t aCodePoint)
			: text(aText)
			, type(aType)
			, script(aScript)
			, range(aRange)
			, codepoint(aCodePoint)
		{
		}

		text_event(const std::shared_ptr<buffer> &aText,
		           event_type aType,
		           const cainteoir::range<uint32_t> &aRange,
		           uint16_t aDuration)
			: text(aText)
			, type(aType)
			, script(ucd::Zzzz)
			, range(aRange)
			, duration(aDuration)
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
		text_reader mReader;
		dictionary mCardinals;
		std::queue<text_event> mEntries;
	};

	struct phoneme_stream
	{
	public:
		phoneme_stream(const std::shared_ptr<document_reader> &aReader,
		               const language::tag &aLocale,
		               word_stream::number_scale aScale,
		               const ruleset &aRules,
		               const path &aExceptionDictionaryPath);

		const text_event &event() const { return mEvent; }

		bool read();
	private:
		void pronounce(const std::shared_ptr<buffer> &aText, const range<uint32_t> &aRange);

		word_stream mReader;
		text_event mEvent;
		ruleset mRules;
		dictionary mExceptionDictionary;
	};
}}

#endif
