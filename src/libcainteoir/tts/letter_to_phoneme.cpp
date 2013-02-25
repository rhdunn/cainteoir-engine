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

#define _    std::shared_ptr<cainteoir::buffer>()
#define x(s) std::make_shared<cainteoir::buffer>(s)

static const std::initializer_list<tts::rule_t> a_rules =
{
	{ _, x("a"), _, x("æ") },
};

static const std::initializer_list<tts::rule_t> b_rules =
{
	{ _, x("b"), _, x("b") },
};

static const std::initializer_list<tts::rule_t> c_rules =
{
	{ _, x("c"), _, x("k") },
};

static const std::initializer_list<tts::rule_t> d_rules =
{
	{ _, x("d"), _, x("d") },
};

static const std::initializer_list<tts::rule_t> e_rules =
{
	{ _, x("e"), _, x("ɛ") },
};

static const std::initializer_list<tts::rule_t> f_rules =
{
	{ _, x("f"), _, x("f") },
};

static const std::initializer_list<tts::rule_t> g_rules =
{
	{ _, x("g"), _, x("g") },
};

static const std::initializer_list<tts::rule_t> h_rules =
{
	{ _, x("h"), _, x("h") },
};

static const std::initializer_list<tts::rule_t> i_rules =
{
	{ _, x("i"), _, x("ɪ") },
};

static const std::initializer_list<tts::rule_t> j_rules =
{
	{ _, x("j"), _, x("dʒ") },
};

static const std::initializer_list<tts::rule_t> k_rules =
{
	{ _, x("k"), _, x("k") },
};

static const std::initializer_list<tts::rule_t> l_rules =
{
	{ _, x("l"), _, x("l") },
};

static const std::initializer_list<tts::rule_t> m_rules =
{
	{ _, x("m"), _, x("m") },
};

static const std::initializer_list<tts::rule_t> n_rules =
{
	{ _, x("n"), _, x("n") },
};

static const std::initializer_list<tts::rule_t> o_rules =
{
	{ _, x("o"), _, x("ɑ") },
};

static const std::initializer_list<tts::rule_t> p_rules =
{
	{ _, x("p"), _, x("p") },
};

static const std::initializer_list<tts::rule_t> q_rules =
{
	{ _, x("q"), _, x("k") },
};

static const std::initializer_list<tts::rule_t> r_rules =
{
	{ _, x("r"),  _, x("r") },
};

static const std::initializer_list<tts::rule_t> s_rules =
{
	{ _, x("s"), _, x("s") },
};

static const std::initializer_list<tts::rule_t> t_rules =
{
	{ _, x("t"), _, x("t") },
};

static const std::initializer_list<tts::rule_t> u_rules =
{
	{ _, x("u"), _, x("ju") },
};

static const std::initializer_list<tts::rule_t> v_rules =
{
	{ _, x("v"), _, x("v") },
};

static const std::initializer_list<tts::rule_t> w_rules =
{
	{ _, x("w"), _, x("w") },
};

static const std::initializer_list<tts::rule_t> x_rules =
{
	{ _, x("x"), _, x("ks") },
};

static const std::initializer_list<tts::rule_t> y_rules =
{
	{ _, x("y"), _, x("ɪ") },
};

static const std::initializer_list<tts::rule_t> z_rules =
{
	{ _, x("z"), _, x("z") },
};

static const std::initializer_list<tts::rule_t> single_quote_rules =
{
	{ _, x("'s"), _, x("z") },
};

#undef _

static const std::initializer_list<tts::rule_t> en_rules[] =
{
	a_rules,
	b_rules,
	c_rules,
	d_rules,
	e_rules,
	f_rules,
	g_rules,
	h_rules,
	i_rules,
	j_rules,
	k_rules,
	l_rules,
	m_rules,
	n_rules,
	o_rules,
	p_rules,
	q_rules,
	r_rules,
	s_rules,
	t_rules,
	u_rules,
	v_rules,
	w_rules,
	x_rules,
	y_rules,
	z_rules,
	single_quote_rules,
};

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
                             const std::initializer_list<tts::rule_t> &rules)
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

std::shared_ptr<cainteoir::buffer> tts::letter_to_phoneme(const std::shared_ptr<cainteoir::buffer> &aText)
{
	char phonemes[512];
	char *out = phonemes;

	const char *first   = aText->begin();
	const char *last    = aText->end();
	const char *current = first;
	while (current != last)
	{
		int rule_index = -1;
		if (*current >= 'a' && *current <= 'z')
			rule_index = *current - 'a';
		else if (*current == '\'')
			rule_index = 26;

		if (rule_index == -1)
			++current; // Rule table not found ... skip the character.
		else
			current = find_rule(first, current, last, out, en_rules[rule_index]);
	}

	*out = '\0';
	return cainteoir::make_buffer(phonemes, out - phonemes);
}
