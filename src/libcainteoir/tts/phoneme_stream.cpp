/* Phoneme Stream.
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

struct rule_t
{
	const char *left;
	const char *match;
	const char *right;
	const char *phonemes;
};

#define _ nullptr

static const std::initializer_list<rule_t> a_rules =
{
	{ _, "a", _, "æ" },
};

static const std::initializer_list<rule_t> b_rules =
{
	{ _, "b", _, "b" },
};

static const std::initializer_list<rule_t> c_rules =
{
	{ _, "c", _, "k" },
};

static const std::initializer_list<rule_t> d_rules =
{
	{ _, "d", _, "d" },
};

static const std::initializer_list<rule_t> e_rules =
{
	{ _, "e", _, "ɛ" },
};

static const std::initializer_list<rule_t> f_rules =
{
	{ _, "f", _, "f" },
};

static const std::initializer_list<rule_t> g_rules =
{
	{ _, "g", _, "g" },
};

static const std::initializer_list<rule_t> h_rules =
{
	{ _, "h", _, "h" },
};

static const std::initializer_list<rule_t> i_rules =
{
	{ _, "i", _, "ɪ" },
};

static const std::initializer_list<rule_t> j_rules =
{
	{ _, "j", _, "dʒ" },
};

static const std::initializer_list<rule_t> k_rules =
{
	{ _, "k", _, "k" },
};

static const std::initializer_list<rule_t> l_rules =
{
	{ _, "l", _, "l" },
};

static const std::initializer_list<rule_t> m_rules =
{
	{ _, "m", _, "m" },
};

static const std::initializer_list<rule_t> n_rules =
{
	{ _, "n", _, "n" },
};

static const std::initializer_list<rule_t> o_rules =
{
	{ _, "o", _, "ɑ" },
};

static const std::initializer_list<rule_t> p_rules =
{
	{ _, "p", _, "p" },
};

static const std::initializer_list<rule_t> q_rules =
{
	{ _, "q", _, "k" },
};

static const std::initializer_list<rule_t> r_rules =
{
	{ _, "r",  _, "r" },
};

static const std::initializer_list<rule_t> s_rules =
{
	{ _, "s", _, "s" },
};

static const std::initializer_list<rule_t> t_rules =
{
	{ _, "t", _, "t" },
};

static const std::initializer_list<rule_t> u_rules =
{
	{ _, "u", _, "ju" },
};

static const std::initializer_list<rule_t> v_rules =
{
	{ _, "v", _, "v" },
};

static const std::initializer_list<rule_t> w_rules =
{
	{ _, "w", _, "w" },
};

static const std::initializer_list<rule_t> x_rules =
{
	{ _, "x", _, "ks" },
};

static const std::initializer_list<rule_t> y_rules =
{
	{ _, "y", _, "ɪ" },
};

static const std::initializer_list<rule_t> z_rules =
{
	{ _, "z", _, "z" },
};

static const std::initializer_list<rule_t> single_quote_rules =
{
	{ _, "'s", _, "z" },
};

#undef _

static const std::initializer_list<rule_t> en_rules[] =
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

static bool left_match(const char *pattern, const char *context)
{
	if (pattern == nullptr)
		return true;

	return false;
}

static bool right_match(const char *pattern, const char *context)
{
	if (pattern == nullptr)
		return true;

	return false;
}

static const char *find_rule(const char *first,
                             const char *current,
                             const char *last,
                             char *&out,
                             const std::initializer_list<rule_t> &rules)
{
	for (auto &rule : rules)
	{
		const char *match = rule.match;
		const char *s     = current;
		while (*match && s != last && *match == *s)
		{
			++match;
			++s;
		}

		if (*match)
			continue;

		if (!left_match(rule.left, current))
			continue;

		if (!right_match(rule.right, s))
			continue;

		current = rule.phonemes;
		while (*current)
			*out++ = *current++;
		return s;
	}

	return ++current;
}

static std::shared_ptr<cainteoir::buffer> letter_to_phoneme(const std::shared_ptr<cainteoir::buffer> &aText)
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

tts::phoneme_stream::phoneme_stream(const std::shared_ptr<document_reader> &aReader,
                                    const language::tag &aLocale,
                                    tts::word_stream::number_scale aScale)
	: mReader(aReader, aLocale, aScale)
{
}

bool tts::phoneme_stream::read()
{
	if (!mReader.read()) return false;

	const tts::text_event &e = mReader.event();
	switch (e.type)
	{
	case tts::word_uppercase:
	case tts::word_lowercase:
	case tts::word_capitalized:
	case tts::word_mixedcase:
	case tts::word_script:
		mEvent = { letter_to_phoneme(e.text), tts::phonemes, e.range, 0 };
		break;
	case tts::comma:
		mEvent = { {}, tts::pause, e.range, 50 };
		break;
	case tts::semicolon:
	case tts::colon:
	case tts::ellipsis:
		mEvent = { {}, tts::pause, e.range, 75 };
		break;
	case tts::full_stop:
	case tts::exclamation:
	case tts::question:
		mEvent = { {}, tts::pause, e.range, 150 };
		break;
	case tts::paragraph:
		mEvent = { {}, tts::pause, e.range, 200 };
		break;
	default:
		return read();
	};

	return true;
}
