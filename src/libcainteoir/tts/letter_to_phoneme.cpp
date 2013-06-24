/* Letter-to-Phoneme Rule Processor.
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

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/text.hpp>

namespace tts = cainteoir::tts;

static bool left_match(const std::shared_ptr<cainteoir::buffer> &pattern, const char *context)
{
	if (!pattern.get())
		return true;

	return false;
}

static bool right_match(const std::shared_ptr<cainteoir::buffer> &pattern, const char *context)
{
	if (!pattern.get())
		return true;

	return false;
}

static const char *mid_match(const std::shared_ptr<cainteoir::buffer> &pattern, const char *context)
{
	for (auto c : *pattern)
	{
		if (c != *context)
			return nullptr;
		++context;
	}
	return context;
}

struct ruleset : public tts::phoneme_reader
{
	struct rule_t
	{
		const std::shared_ptr<cainteoir::buffer> left;
		const std::shared_ptr<cainteoir::buffer> match;
		const std::shared_ptr<cainteoir::buffer> right;
		const tts::phoneme phoneme;
	};

	ruleset();

	void add_rule(const std::shared_ptr<cainteoir::buffer> &aLeft,
	              const std::shared_ptr<cainteoir::buffer> &aMatch,
	              const std::shared_ptr<cainteoir::buffer> &aRight,
	              const tts::phoneme aPhoneme);

	bool add_rules(const char *aRulesPath);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	void add_rules(const std::shared_ptr<cainteoir::buffer> &aDictionary);

	std::list<rule_t> mRules[256];
	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mFirst;
	const char *mCurrent;
	const char *mLast;
};

ruleset::ruleset()
	: mFirst(nullptr)
	, mCurrent(nullptr)
	, mLast(nullptr)
{
}

void ruleset::add_rule(const std::shared_ptr<cainteoir::buffer> &aLeft,
                       const std::shared_ptr<cainteoir::buffer> &aMatch,
                       const std::shared_ptr<cainteoir::buffer> &aRight,
                       const tts::phoneme aPhoneme)
{
	mRules[*aMatch->begin()].push_back({ aLeft, aMatch, aRight, aPhoneme });
}

bool ruleset::add_rules(const char *aRulesPath)
{
	try
	{
		add_rules(cainteoir::make_file_buffer(aRulesPath));
	}
	catch (const std::exception &)
	{
		return false;
	}
	return true;
}

void ruleset::add_rules(const std::shared_ptr<cainteoir::buffer> &aRules)
{
	auto phonemes = tts::createPhonemeReader("ipa");

	const char *current = aRules->begin();
	const char *last    = aRules->end();
	while (current != last)
	{
		if (*current == '#' || *current == '.')
		{
			while (current != last && *current != '\r' && *current != '\n')
				++current;

			while (current != last && (*current == '\r' || *current == '\n'))
				++current;

			continue;
		}

		const char *begin_entry = current;
		const char *end_entry   = current;
		while (end_entry != last && *end_entry != '\t')
			++end_entry;

		current = end_entry;
		while (current != last && *current == '\t')
			++current;

		if (current == last) return;

		const char *begin_phonemes = current;
		const char *end_phonemes   = current;
		while (end_phonemes != last && *end_phonemes != '\r' && *end_phonemes != '\n')
			++end_phonemes;

		current = end_phonemes;
		while (current != last && (*current == '\r' || *current == '\n'))
			++current;

		if (*begin_phonemes == '/' && *(end_phonemes - 1) == '/')
		{
			++begin_phonemes;
			--end_phonemes;
			phonemes->reset(std::make_shared<cainteoir::buffer>(begin_phonemes, end_phonemes));

			phonemes->read();
			add_rule(std::shared_ptr<cainteoir::buffer>(), // left
			         cainteoir::make_buffer(begin_entry, end_entry - begin_entry),
			         std::shared_ptr<cainteoir::buffer>(), // right
			         *phonemes);
		}
	}
}

void ruleset::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mBuffer = aBuffer;
	if (mBuffer.get())
	{
		mFirst = mCurrent = mBuffer->begin();
		mLast = mBuffer->end();
	}
	else
	{
		mFirst = mCurrent = mLast = nullptr;
	}
}

bool ruleset::read()
{
	while (mCurrent != mLast)
	{
		for (auto &rule : mRules[*mCurrent])
		{
			const char *s = mid_match(rule.match, mCurrent);
			if (s == nullptr)
				continue;

			if (!left_match(rule.left, mCurrent))
				continue;

			if (!right_match(rule.right, s))
				continue;

			*(tts::phoneme *)this = rule.phoneme;
			mCurrent = s;
			return true;
		}

		fprintf(stderr, "error: cannot pronounce \"%s\" ... no matching rule for '%c' found.\n",
		        std::string(mFirst, mLast).c_str(),
		        *mCurrent);
		fprintf(stderr, "                         %*c\n", (mCurrent - mFirst + 1), '^');
		++mCurrent;
	}
	return false;
}

std::shared_ptr<tts::phoneme_reader> tts::createPronunciationRules(const char *aRuleSetPath)
{
	auto rules = std::make_shared<ruleset>();
	if (!rules->add_rules(aRuleSetPath))
		return {};
	return rules;
}
