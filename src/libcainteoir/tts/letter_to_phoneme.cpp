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

static const char *find_rule(const char *first,
                             const char *current,
                             const char *last,
                             char *&out,
                             const std::list<tts::ruleset::rule_t> &rules)
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

		for (auto c : *rule.phonemes)
			*out++ = c;
		return s;
	}

	fprintf(stderr, "error: cannot pronounce \"%s\" ... no matching rule for '%c' found.\n",
	        std::string(first, last).c_str(),
	        *current);
	fprintf(stderr, "                         %*c\n", (current - first + 1), '^');

	return ++current;
}

void tts::ruleset::add_rule(const std::shared_ptr<buffer> &aLeft,
                            const std::shared_ptr<buffer> &aMatch,
                            const std::shared_ptr<buffer> &aRight,
                            const std::shared_ptr<buffer> &aPhonemes)
{
	mRules[*aMatch->begin()].push_back({ aLeft, aMatch, aRight, aPhonemes });
}

bool tts::ruleset::add_rules(const path &aRulesPath)
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

std::shared_ptr<cainteoir::buffer> tts::ruleset::pronounce(const std::shared_ptr<cainteoir::buffer> &aText) const
{
	char phonemes[512];
	char *out = phonemes;

	const char *first   = aText->begin();
	const char *last    = aText->end();
	const char *current = first;
	while (current != last)
		current = find_rule(first, current, last, out, mRules[*current]);

	*out = '\0';
	return cainteoir::make_buffer(phonemes, out - phonemes);
}

void tts::ruleset::add_rules(const std::shared_ptr<buffer> &aRules)
{
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
			add_rule(std::shared_ptr<buffer>(), // left
			         make_buffer(begin_entry, end_entry - begin_entry),
			         std::shared_ptr<buffer>(), // right
			         make_buffer(begin_phonemes, end_phonemes - begin_phonemes));
		}
	}
}

