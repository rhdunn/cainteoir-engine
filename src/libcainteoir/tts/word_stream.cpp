/* Word Stream.
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

#include "config.h"
#include "compatibility.hpp"

#include <cainteoir/text.hpp>
#include <cainteoir/path.hpp>
#include <stack>

namespace tts = cainteoir::tts;

static bool parseCainteoirDictionary(tts::dictionary &dict, const cainteoir::path &path)
{
	try
	{
		auto reader = tts::createCainteoirDictionaryReader(path);
		while (reader->read())
			dict.add_entry(reader->word, reader->entry);
	}
	catch (const std::exception &e)
	{
		return false;
	}
	return true;
}

#define _(x) std::make_shared<cainteoir::buffer>(x)

static const std::vector<std::shared_ptr<cainteoir::buffer>> groups =
{
	_("_10^3"),
	_("_10^6"),
	_("_10^9"),
	_("_10^12"),
	_("_10^15"),
	_("_10^18"),
	_("_10^21"),
	_("_10^24"),
	_("_10^27"),
	_("_10^30"),
};

static const std::vector<std::shared_ptr<cainteoir::buffer>> ties =
{
	_("_2x"),
	_("_3x"),
	_("_4x"),
	_("_5x"),
	_("_6x"),
	_("_7x"),
	_("_8x"),
	_("_9x"),
};

static const std::vector<std::shared_ptr<cainteoir::buffer>> single_digits =
{
	_("0"),
	_("1"),
	_("2"),
	_("3"),
	_("4"),
	_("5"),
	_("6"),
	_("7"),
	_("8"),
	_("9"),
	_("10"),
	_("11"),
	_("12"),
	_("13"),
	_("14"),
	_("15"),
	_("16"),
	_("17"),
	_("18"),
	_("19"),
};

static const std::shared_ptr<cainteoir::buffer> hundred              = _("_10^2");
static const std::shared_ptr<cainteoir::buffer> tens_units_separator = _("_andDD");
static const std::shared_ptr<cainteoir::buffer> units_tens_separator = _("_DandDD");

#undef _

struct number_block
{
	uint16_t rank;
	uint16_t value;
};

static std::stack<number_block> parse_number(const tts::text_event &number, uint16_t max_block)
{
	std::stack<number_block> blocks;
	uint16_t value = 0; // The numeric value of the current group of 3 digits.
	uint16_t scale = 1; // The scale factor to apply to the current digit.
	uint16_t n     = 0; // The current group order -- use 10^3n for thousands, millions, etc.
	for (char c : cainteoir::reverse(*number.text))
	{
		if (c >= '0' && c <= '9')
		{
			value += ((c - '0') * scale);
			scale *= 10;
			if (scale == 1000)
			{
				blocks.push({ n, value });
				scale = 1;
				value = 0;
				++n;
				if (n > max_block)
				{
					fprintf(stdout, "error: number exceeded maximum block count %d.\n", n);
					return blocks;
				}
			}
		}
		else
			fprintf(stdout, "error: unsupported number character '%c'.\n", c);
	}
	if (scale != 1)
		blocks.push({ n, value });
	return blocks;
}

static void push_word_(std::list<tts::text_event> &events,
                       std::list<tts::text_event>::iterator &current,
                       const tts::dictionary &words,
                       const std::shared_ptr<cainteoir::buffer> &entry,
                       cainteoir::range<uint32_t> range)
{
	auto word = words.lookup(entry);
	if (word.type != tts::dictionary::say_as)
		return;

	current = events.insert(current, tts::text_event(word.text, tts::word_lowercase, range, 0));
	++current;
}

static void parse_number(std::list<tts::text_event> &events,
                         std::list<tts::text_event>::iterator &current,
                         const tts::dictionary &cardinals,
                         const tts::dictionary &ordinals)
{
	std::stack<number_block> blocks = parse_number(*current, groups.size());
	auto range = current->range;
	auto text  = current->text;
	current = events.erase(current);

	bool swap_tens_units = cardinals.lookup(units_tens_separator).type == tts::dictionary::say_as;
	bool need_zero = true;
	bool need_and = false;
	while (!blocks.empty())
	{
		#define push_cardinal(w) push_word_(events, current, cardinals, w, range)
		#define push_ordinal(w)  push_word_(events, current, ordinals,  w, range)

		auto item = blocks.top();
		blocks.pop();

		bool have_rank = item.rank == 0 ||
		                 cardinals.lookup(groups[item.rank - 1]).type == tts::dictionary::say_as;

		if (!have_rank)
		{
			fflush(stdout);
			fprintf(stderr, "language does not support 10^%d numbers ... speaking digits instead\n", item.rank * 3);
			fflush(stderr);

			for (char c : *text)
			{
				push_cardinal(single_digits[c - '0']);
			}
			return;
		}

		bool need_group = item.value != 0;

		if (item.value >= 100)
		{
			int hundreds = item.value / 100;
			item.value %= 100;
			need_zero = false;
			need_and  = false;

			if (item.value == 0)
			{
				push_cardinal(single_digits[hundreds]);
				push_ordinal(hundred);
			}
			else
			{
				push_cardinal(single_digits[hundreds]);
				push_cardinal(hundred);
				push_cardinal(tens_units_separator);
			}
		}

		if (need_and && item.rank == 0)
		{
			if (item.value != 0)
				push_cardinal(tens_units_separator);
			need_and = false;
		}

		if (item.value >= 20)
		{
			int tens   = (item.value - 20) / 10;
			int digits = item.value % 10;
			need_zero = false;

			if (digits == 0)
				push_ordinal(ties[tens]);
			else if (swap_tens_units)
			{
				// e.g. drei und vierzig (three and fourty) in German
				push_cardinal(single_digits[digits]);
				push_cardinal(units_tens_separator);
				push_cardinal(ties[tens]);
			}
			else
			{
				// e.g. fourty three in English
				push_cardinal(ties[tens]);
				push_ordinal(single_digits[digits]);
			}
		}
		else if (item.value == 0)
		{
			if (item.rank == 0 && need_zero)
				push_ordinal(single_digits[item.value]);
		}
		else
		{
			if (item.rank == 0)
				push_ordinal(single_digits[item.value]);
			else
				push_cardinal(single_digits[item.value]);
			need_zero = false;
		}

		if (item.rank > 0 && need_group)
		{
			push_cardinal(groups[item.rank - 1]);
			need_and = true;
		}

		#undef push_cardinal
		#undef push_ordinal
	}
}

static const std::string number_scale_str[] =
{
	"x-shtscale",
	"x-lngscale",
};

struct numbers_to_words_t : public tts::text_reader
{
public:
	numbers_to_words_t(const cainteoir::language::tag &aLocale,
	                   tts::number_scale aScale);

	void bind(const std::shared_ptr<tts::text_reader> &aReader);

	const tts::text_event &event() const { return mEntries.front(); }

	bool read();
private:
	std::shared_ptr<tts::text_reader> mReader;
	std::list<tts::text_event> mEntries;

	tts::dictionary mCardinals;
	tts::dictionary mOrdinals;
};

numbers_to_words_t::numbers_to_words_t(const cainteoir::language::tag &aLocale,
                                       tts::number_scale aScale)
{
	auto locale_path = cainteoir::get_data_path() / "locale";

	if (!parseCainteoirDictionary(mCardinals, locale_path / (aLocale.lang + '-' + aLocale.region) / "cardinal.dict"))
		parseCainteoirDictionary(mCardinals, locale_path / aLocale.lang / "cardinal.dict");
	parseCainteoirDictionary(mCardinals, locale_path / (aLocale.lang + '-' + number_scale_str[aScale]) / "cardinal.dict");

	if (!parseCainteoirDictionary(mOrdinals, locale_path / (aLocale.lang + '-' + aLocale.region) / "ordinal.dict"))
		parseCainteoirDictionary(mOrdinals, locale_path / aLocale.lang / "ordinal.dict");
	parseCainteoirDictionary(mOrdinals, locale_path / (aLocale.lang + '-' + number_scale_str[aScale]) / "ordinal.dict");
}

void numbers_to_words_t::bind(const std::shared_ptr<tts::text_reader> &aReader)
{
	mReader = aReader;
}

bool numbers_to_words_t::read()
{
	if (mEntries.empty())
	{
		if (mReader && mReader->read())
		{
			mEntries.push_back(mReader->event());
			auto current = --mEntries.end();

			switch (current->type)
			{
			case tts::number:
				parse_number(mEntries, current, mCardinals, mCardinals);
				break;
			case tts::ordinal_number:
				if (!mOrdinals.empty())
					parse_number(mEntries, current, mCardinals, mOrdinals);
				else
					parse_number(mEntries, current, mCardinals, mCardinals);
				break;
			}
		}

		// Don't pop the first matching item:
		return !mEntries.empty();
	}

	mEntries.pop_front();
	if (mEntries.empty())
		return read();

	return true;
}

std::shared_ptr<tts::text_reader>
tts::numbers_to_words(const language::tag &aLocale,
                      number_scale aScale)
{
	return std::make_shared<numbers_to_words_t>(aLocale, aScale);
}
