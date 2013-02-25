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

tts::ruleset tts::en_rules()
{
	#define _    std::shared_ptr<cainteoir::buffer>()
	#define x(s) std::make_shared<cainteoir::buffer>(s)

	tts::ruleset en;

	en.add_rule(_, x("a"), _, x("æ"));

	en.add_rule(_, x("b"), _, x("b"));

	en.add_rule(_, x("c"), _, x("k"));

	en.add_rule(_, x("d"), _, x("d"));

	en.add_rule(_, x("e"), _, x("ɛ"));

	en.add_rule(_, x("f"), _, x("f"));

	en.add_rule(_, x("g"), _, x("g"));

	en.add_rule(_, x("h"), _, x("h"));

	en.add_rule(_, x("i"), _, x("ɪ"));

	en.add_rule(_, x("j"), _, x("dʒ"));

	en.add_rule(_, x("k"), _, x("k"));

	en.add_rule(_, x("l"), _, x("l"));

	en.add_rule(_, x("m"), _, x("m"));

	en.add_rule(_, x("n"), _, x("n"));

	en.add_rule(_, x("o"), _, x("ɑ"));

	en.add_rule(_, x("p"), _, x("p"));

	en.add_rule(_, x("q"), _, x("k"));

	en.add_rule(_, x("r"),  _, x("r"));

	en.add_rule(_, x("s"), _, x("s"));

	en.add_rule(_, x("t"), _, x("t"));

	en.add_rule(_, x("u"), _, x("ju"));

	en.add_rule(_, x("v"), _, x("v"));

	en.add_rule(_, x("w"), _, x("w"));

	en.add_rule(_, x("x"), _, x("ks"));

	en.add_rule(_, x("y"), _, x("ɪ"));

	en.add_rule(_, x("z"), _, x("z"));

	en.add_rule(_, x("'s"), _, x("z"));

	#undef x
	#undef _

	return en;
}

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

	return ++current;
}

void tts::ruleset::add_rule(const std::shared_ptr<buffer> &aLeft,
                            const std::shared_ptr<buffer> &aMatch,
                            const std::shared_ptr<buffer> &aRight,
                            const std::shared_ptr<buffer> &aPhonemes)
{
	mRules[*aMatch->begin()].push_back({ aLeft, aMatch, aRight, aPhonemes });
}

std::shared_ptr<cainteoir::buffer> tts::ruleset::pronounce(const std::shared_ptr<cainteoir::buffer> &aText)
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
