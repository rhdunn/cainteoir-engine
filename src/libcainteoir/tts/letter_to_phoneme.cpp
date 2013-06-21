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

struct ruleset : public tts::pronunciation
{
	struct rule_t
	{
		const std::shared_ptr<cainteoir::buffer> left;
		const std::shared_ptr<cainteoir::buffer> match;
		const std::shared_ptr<cainteoir::buffer> right;
		const std::list<tts::phoneme> phonemes;
	};

	void add_rule(const std::shared_ptr<cainteoir::buffer> &aLeft,
	              const std::shared_ptr<cainteoir::buffer> &aMatch,
	              const std::shared_ptr<cainteoir::buffer> &aRight,
	              const std::list<tts::phoneme> &aPhonemes);

	bool add_rules(const cainteoir::path &aRulesPath);

	std::list<tts::phoneme> pronounce(const std::shared_ptr<cainteoir::buffer> &aText) const;
private:
	void add_rules(const std::shared_ptr<cainteoir::buffer> &aDictionary);

	std::list<rule_t> mRules[256];
};

static const char *find_rule(const char *first,
                             const char *current,
                             const char *last,
                             std::list<tts::phoneme> &out,
                             const std::list<ruleset::rule_t> &rules)
{
	for (auto &rule : rules)
	{
		const char *s = mid_match(rule.match, current);
		if (s == nullptr)
			continue;

		if (!left_match(rule.left, current))
			continue;

		if (!right_match(rule.right, s))
			continue;

		for (auto p : rule.phonemes)
			out.push_back(p);
		return s;
	}

	fprintf(stderr, "error: cannot pronounce \"%s\" ... no matching rule for '%c' found.\n",
	        std::string(first, last).c_str(),
	        *current);
	fprintf(stderr, "                         %*c\n", (current - first + 1), '^');

	return ++current;
}

void ruleset::add_rule(const std::shared_ptr<cainteoir::buffer> &aLeft,
                       const std::shared_ptr<cainteoir::buffer> &aMatch,
                       const std::shared_ptr<cainteoir::buffer> &aRight,
                       const std::list<tts::phoneme> &aPhonemes)
{
	mRules[*aMatch->begin()].push_back({ aLeft, aMatch, aRight, aPhonemes });
}

bool ruleset::add_rules(const cainteoir::path &aRulesPath)
{
	try
	{
		add_rules(make_file_buffer(aRulesPath));
	}
	catch (const std::exception &)
	{
		return false;
	}
	return true;
}

std::list<tts::phoneme> ruleset::pronounce(const std::shared_ptr<cainteoir::buffer> &aText) const
{
	std::list<tts::phoneme> out;

	const char *first   = aText->begin();
	const char *last    = aText->end();
	const char *current = first;
	while (current != last)
		current = find_rule(first, current, last, out, mRules[*current]);

	return out;
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

			std::list<tts::phoneme> phon;
			while (phonemes->read())
				phon.push_back(*phonemes);

			add_rule(std::shared_ptr<cainteoir::buffer>(), // left
			         cainteoir::make_buffer(begin_entry, end_entry - begin_entry),
			         std::shared_ptr<cainteoir::buffer>(), // right
			         phon);
		}
	}
}

std::shared_ptr<tts::pronunciation> tts::createPronunciationRules(const path &aRuleSetPath)
{
	auto rules = std::make_shared<ruleset>();
	if (!rules->add_rules(aRuleSetPath))
		return {};
	return rules;
}
