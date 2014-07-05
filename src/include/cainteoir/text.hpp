/* Text Processing API.
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
		en_dash,
		em_dash,
	};

	struct text_event
	{
		std::shared_ptr<buffer> text;
		std::list<ipa::phoneme> phonemes;
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
		virtual void bind(const std::shared_ptr<tts::text_reader> &aReader) = 0;

		virtual const text_event &event() const = 0;

		virtual bool read() = 0;

		virtual ~text_reader() {}
	};

	enum number_scale
	{
		short_scale,
		long_scale,
	};

	std::shared_ptr<text_reader>
	create_text_reader(const std::shared_ptr<document_reader> &aReader);

	bool
	next_clause(const std::shared_ptr<text_reader> &aReader,
	            std::list<text_event> &aClause);

	struct clause_processor
	{
		virtual void chain(const std::shared_ptr<tts::clause_processor> &aProcessor) = 0;

		virtual void process(std::list<text_event> &aClause) = 0;

		virtual ~clause_processor() {}
	};

	std::shared_ptr<text_reader>
	context_analysis();

	std::shared_ptr<text_reader>
	numbers_to_words(const language::tag &aLocale,
	                 number_scale aScale);

	std::shared_ptr<text_reader>
	words_to_phonemes(const std::shared_ptr<phoneme_reader> &aRules,
	                  const std::shared_ptr<dictionary_reader> &aExceptionDictionary);

	std::shared_ptr<tts::text_reader>
	adjust_stress();

	inline std::shared_ptr<text_reader>
	operator|(const std::shared_ptr<text_reader> &a, const std::shared_ptr<text_reader> &b)
	{
		b->bind(a);
		return b;
	}

	void generate_phonemes(const std::shared_ptr<tts::text_reader> &reader,
	                       FILE *out,
	                       const char *phonemeset,
	                       tts::stress_type stress,
	                       const char *open,
	                       const char *close,
	                       const char *phrase = " ");
}}

#endif
