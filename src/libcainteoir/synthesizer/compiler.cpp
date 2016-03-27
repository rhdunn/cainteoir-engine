/* Voice Database File Builder.
 *
 * Copyright (C) 2014-2016 Reece H. Dunn
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

#include <cainteoir/synthesizer.hpp>
#include <cainteoir/dictionary.hpp>
#include <cainteoir/language.hpp>
#include "../cainteoir_file_reader.hpp"

#include "synth.hpp"

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

struct conditional_t
{
	uint8_t c;
	uint8_t type;
	std::string value;
};

void tts::read_phoneme_units(cainteoir::native_endian_buffer &data, std::vector<unit_t> &units)
{
	uint16_t n = data.u16();
	units.reserve(n);
	for (auto i : cainteoir::range<uint16_t>(0, n))
	{
		unit_t unit;
		unit.name = data.pstr();
		unit.phoneme_start = data.u8();
		unit.unit_start = data.u8();
		unit.unit_end = data.u8();
		units.push_back(unit);
	}
}

struct binary_file_writer
{
	binary_file_writer(FILE *aOutput)
		: mOutput(aOutput)
		, mOffset(0)
	{
	}

	void begin_section(const char *magic, uint32_t section_size, bool has_pstr_fields);
	void end_section();

	void u8(uint8_t u)   { fputc(u, mOutput); }
	void u16(uint16_t u) { fwrite(&u, sizeof(u), 1, mOutput); }
	void u32(uint32_t u) { fwrite(&u, sizeof(u), 1, mOutput); }
	void u64(uint64_t u) { fwrite(&u, sizeof(u), 1, mOutput); }

	void f8_8(float f);
	void f16_16(float f);

	void str(const cainteoir::buffer &data) { str(data.str()); }
	void str(const std::string &data);

	void pstr(const cainteoir::buffer &data) { pstr(data.str()); }
	void pstr(const std::string &data);
private:
	FILE *mOutput;
	uint32_t mOffset;
	std::list<std::string> mStrings;
	std::map<std::string, uint32_t> mStringTable;
};

void binary_file_writer::begin_section(const char *magic, uint32_t section_size, bool has_pstr_fields)
{
	mOffset += section_size;
	if (has_pstr_fields)
		mOffset += tts::STRING_TABLE_HEADER_SIZE;
	fputs(magic, mOutput);
}

void binary_file_writer::end_section()
{
	if (!mStrings.empty())
	{
		fputs("STR", mOutput);
		u32(mOffset);
		for (const auto &s : mStrings)
			str(s);
		mStrings.clear();
		mStringTable.clear();
	}
}

void binary_file_writer::f8_8(float f)
{
	u8((int8_t)f);
	u8(f * 256);
}

void binary_file_writer::f16_16(float f)
{
	u16((int16_t)f);
	u16(f * 65536);
}

void binary_file_writer::str(const std::string &data)
{
	fwrite(data.c_str(), data.size(), 1, mOutput);
	fputc('\0', mOutput);
}

void binary_file_writer::pstr(const std::string &data)
{
	auto &entry = mStringTable[data];
	if (entry == 0)
	{
		entry = mOffset;
		u32(mOffset);
		mOffset += data.size() + 1;
		mStrings.push_back(data);
	}
	else
		u32(entry);
}

struct unit_t
{
	cainteoir::buffer name;
	uint8_t phoneme_start;
	uint8_t unit_start;
	uint8_t unit_end;

	unit_t()
		: name(nullptr, nullptr)
		, phoneme_start(0)
		, unit_start(0)
		, unit_end(100)
	{
	}
};

struct duration_t
{
	css::time mean;
	css::time sdev;
	ipa::phoneme phoneme1;
	ipa::phoneme phoneme2;

	duration_t()
		: sdev(5, css::time::milliseconds)
		, phoneme1(ipa::unspecified)
		, phoneme2(ipa::unspecified)
	{
	}
};

struct phoneme_t
{
	ipa::phoneme phoneme1;
	ipa::phoneme phoneme2;
	std::list<duration_t> durations;
	std::list<unit_t> units;
};

static phoneme_t
parse_phoneme(cainteoir_file_reader &reader, const std::shared_ptr<tts::phoneme_parser> &parser)
{
	enum state_t
	{
		in_phoneme,
		in_phoneme_body,
		in_duration_mean,
		in_duration_stdev,
		in_duration_conditional,
		in_unit_offset,
		in_unit_name,
		in_unit_start,
		in_unit_end,
	};

	state_t state = in_phoneme;
	phoneme_t entry;
	unit_t *unit = nullptr;
	duration_t *duration = nullptr;

	while (reader.read()) switch (reader.type())
	{
	case cainteoir_file_reader::text:
		if (reader.match().compare("end") == 0)
			return entry;
		else if (reader.match().compare("unit") == 0)
		{
			entry.units.push_back({});
			unit = &entry.units.back();
			state = in_unit_offset;
		}
		else if (reader.match().compare("duration") == 0)
		{
			entry.durations.push_back({});
			duration = &entry.durations.back();
			duration->phoneme1 = entry.phoneme1;
			duration->phoneme2 = entry.phoneme2;
			state = in_duration_mean;
		}
		else if (reader.match().compare("if") == 0)
		{
			if (duration != nullptr)
				state = in_duration_conditional;
		}
		else switch (state)
		{
		case in_duration_mean:
			duration->mean = css::parse_smil_time(reader.match());
			state = in_duration_stdev;
			break;
		case in_duration_stdev:
			duration->sdev = css::parse_smil_time(reader.match());
			state = in_phoneme_body;
			break;
		case in_unit_offset:
			unit->phoneme_start = strtol(reader.match().str().c_str(), nullptr, 10);
			state = in_unit_name;
			break;
		case in_unit_start:
			unit->unit_start = strtol(reader.match().str().c_str(), nullptr, 10);
			state = in_unit_end;
			break;
		case in_unit_end:
			unit->unit_end = strtol(reader.match().str().c_str(), nullptr, 10);
			state = in_phoneme_body;
			break;
		}
		break;
	case cainteoir_file_reader::phonemes:
		switch (state)
		{
		case in_phoneme:
			{
				auto begin = reader.match().begin();
				if (parser->parse(begin, reader.match().end(), entry.phoneme1))
				{
					if (!parser->parse(begin, reader.match().end(), entry.phoneme2))
						entry.phoneme2 = ipa::unspecified;
				}
				state = in_phoneme_body;
			}
			break;
		case in_duration_conditional:
			{
				auto begin = reader.match().begin();
				if (parser->parse(begin, reader.match().end(), duration->phoneme1))
				{
					if (!parser->parse(begin, reader.match().end(), duration->phoneme2))
						duration->phoneme2 = ipa::unspecified;
				}
				state = in_phoneme_body;
			}
			break;
		}
		break;
	case cainteoir_file_reader::string:
		switch (state)
		{
		case in_unit_offset:
		case in_unit_name:
			unit->name = reader.match();
			state = in_unit_start;
			break;
		}
		break;
	}

	return entry;
}

void
tts::compile_voice(const char *aFileName, FILE *aOutput)
{
	if (!aOutput) return;

	auto parser = createPhonemeParser("features");

	cainteoir::buffer rdfns{ nullptr, nullptr };
	cainteoir::buffer id{ nullptr, nullptr };
	cainteoir::buffer name{ nullptr, nullptr };
	cainteoir::buffer synthesizer{ nullptr, nullptr };
	cainteoir::buffer author{ nullptr, nullptr };
	cainteoir::buffer locale{ nullptr, nullptr };
	uint8_t gender = 0;
	float volume_scale = 1.0;
	uint16_t frequency = 0;
	uint8_t channels = 0;
	cainteoir::buffer sample_format{ nullptr, nullptr };

	css::frequency pitch_base;
	css::frequency pitch_step;
	css::frequency pitch_sdev;

	std::list<phoneme_t> phonemes;
	uint16_t duration_entries = 0;
	uint16_t unit_entries = 0;

	auto reader = cainteoir_file_reader(cainteoir::path(aFileName));
	while (reader.read())
	{
		if (reader.type() == cainteoir_file_reader::directive)
		{
			if (reader.match().compare(".rdfns") == 0)
			{
				reader.read();
				rdfns = reader.match();
			}
			else if (reader.match().compare(".id") == 0)
			{
				reader.read();
				id = reader.match();
			}
			else if (reader.match().compare(".name") == 0)
			{
				reader.read();
				name = reader.match();
			}
			else if (reader.match().compare(".synthesizer") == 0)
			{
				reader.read();
				synthesizer = reader.match();
			}
			else if (reader.match().compare(".voice-author") == 0)
			{
				reader.read();
				author = reader.match();
			}
			else if (reader.match().compare(".locale") == 0)
			{
				reader.read();
				locale = reader.match();
			}
			else if (reader.match().compare(".gender") == 0)
			{
				reader.read();
				if (reader.match().compare("male") == 0)
					gender = 'M';
				else if (reader.match().compare("female") == 0)
					gender = 'F';
				else
					throw std::runtime_error("unsupported gender value");
			}
			else if (reader.match().compare(".volume-scale") == 0)
			{
				reader.read();
				volume_scale = strtof(reader.match().begin(), nullptr);
			}
			else if (reader.match().compare(".frequency") == 0)
			{
				reader.read();
				frequency = strtol(reader.match().begin(), nullptr, 10);
			}
			else if (reader.match().compare(".channels") == 0)
			{
				reader.read();
				channels = strtol(reader.match().begin(), nullptr, 10);
			}
			else if (reader.match().compare(".sample-format") == 0)
			{
				reader.read();
				sample_format = reader.match();
			}
			else if (reader.match().compare(".phonemeset") == 0)
			{
				reader.read();
				parser = createPhonemeParser(reader.match().str().c_str());
			}
		}
		else if (reader.type() == cainteoir_file_reader::text)
		{
			if (reader.match().compare("phoneme") == 0)
			{
				phonemes.push_back(parse_phoneme(reader, parser));
				duration_entries += phonemes.back().durations.size();
				unit_entries += phonemes.back().units.size();
			}
			else if (reader.match().compare("pitch") == 0)
			{
				if (!reader.read() || reader.type() != cainteoir_file_reader::text)
					throw std::runtime_error("expected baseline pitch value");

				pitch_base = css::parse_frequency(reader.match());

				if (!reader.read() || reader.type() != cainteoir_file_reader::text)
					throw std::runtime_error("expected pitch step value");

				pitch_step = css::parse_frequency(reader.match());

				if (!reader.read() || reader.type() != cainteoir_file_reader::text)
					throw std::runtime_error("expected pitch standard deviation value");

				pitch_sdev = css::parse_frequency(reader.match());
			}
		}
	}

	binary_file_writer out(aOutput);

	// Voice Database Header
	out.begin_section("VOICEDB", tts::VOICEDB_HEADER_SIZE, true);
	out.u16(0x3031); // endianness
	out.pstr(rdfns);
	out.pstr(id);
	out.pstr(name);
	out.pstr(synthesizer);
	out.pstr(author);
	out.pstr(locale);
	out.u8(gender);
	out.f8_8(volume_scale);
	out.u16(frequency);
	out.u8(channels);
	out.pstr(sample_format);
	out.end_section();

	if (pitch_base.units() != css::frequency::inherit &&
	    pitch_step.units() != css::frequency::inherit &&
	    pitch_sdev.units() != css::frequency::inherit)
	{
		out.begin_section("PTC", tts::PITCH_DATA_SECTION_SIZE, false);
		out.f16_16(pitch_base.as(css::frequency::hertz).value());
		out.f16_16(pitch_step.as(css::frequency::hertz).value());
		out.f16_16(pitch_sdev.as(css::frequency::hertz).value());
		out.end_section();
	}

	if (duration_entries != 0)
	{
		out.begin_section("DUR", tts::TABLE_SECTION_SIZE + (duration_entries * tts::DURATION_TABLE_ENTRY_SIZE), false);
		out.u16(duration_entries);
		for (const auto &entry : phonemes) for (const auto &duration : entry.durations)
		{
			out.u64(duration.phoneme1.get(ipa::main | ipa::diacritics | ipa::suprasegmentals));
			out.u64(duration.phoneme2.get(ipa::main | ipa::diacritics | ipa::suprasegmentals));
			out.u8((int)duration.mean.as(css::time::milliseconds).value());
			if (duration.sdev.units() == css::time::inherit)
				out.u8(0);
			else
				out.u8((int)duration.sdev.as(css::time::milliseconds).value());
		}
		out.end_section();
	}

	if (unit_entries != 0)
	{
		out.begin_section("PUT", tts::TABLE_SECTION_SIZE + (unit_entries * tts::PHONEME_UNIT_TABLE_ENTRY_SIZE), true);
		out.u16(unit_entries);
		for (const auto &entry : phonemes) for (const auto &unit : entry.units)
		{
			out.pstr(unit.name);
			out.u8(unit.phoneme_start);
			out.u8(unit.unit_start);
			out.u8(unit.unit_end);
		}
		out.end_section();
	}

	if (!phonemes.empty())
	{
		out.begin_section("PHO", tts::TABLE_SECTION_SIZE + (phonemes.size() * tts::PHONEME_TABLE_ENTRY_SIZE), false);
		out.u16(phonemes.size());
		uint16_t unit_offset = 0;
		for (const auto &entry : phonemes)
		{
			out.u64(entry.phoneme1.get(ipa::main | ipa::diacritics | ipa::suprasegmentals));
			out.u64(entry.phoneme2.get(ipa::main | ipa::diacritics | ipa::suprasegmentals));
			out.u16(unit_offset);
			out.u8(entry.units.size());
			unit_offset += entry.units.size();
		}
		out.end_section();
	}
}

void
parse_rewrite(cainteoir_file_reader &reader,
              std::map<uint8_t, std::list<std::pair<std::string, cainteoir::buffer>>> &rewrite_rules)
{
	cainteoir::buffer context{ nullptr, nullptr };
	cainteoir::buffer left{ nullptr, nullptr };
	cainteoir::buffer right{ nullptr, nullptr };
	while (reader.read()) switch (reader.type())
	{
	case cainteoir_file_reader::text:
		{
			if (reader.match().compare("end") == 0)
				return;

			auto last = reader.match().end();
			if (*--last == ')')
				left = reader.match();
			else if (*reader.match().begin() == '(')
				right = reader.match();
			else if (context.empty())
				context = reader.match();
			else
			{
				auto &group = rewrite_rules[*context.begin()];

				std::string pattern = context.str();
				for (auto c : cainteoir::reverse(left))
					pattern.push_back(c);
				for (auto c : right)
					pattern.push_back(c);

				group.push_back({ pattern, reader.match() });
				context = { nullptr, nullptr };
				left = { nullptr, nullptr };
				right = { nullptr, nullptr };
			}
		}
		break;
	case cainteoir_file_reader::string:
		if (!context.empty())
		{
			auto &group = rewrite_rules[*context.begin()];

			std::string pattern = context.str();
			for (auto c : cainteoir::reverse(left))
				pattern.push_back(c);
			for (auto c : right)
				pattern.push_back(c);

			group.push_back({ pattern, reader.match() });
			context = { nullptr, nullptr };
			left = { nullptr, nullptr };
			right = { nullptr, nullptr };
		}
		break;
	}
}

void
parse_rules(cainteoir_file_reader &reader,
            std::map<uint8_t, std::list<std::pair<std::string, cainteoir::buffer>>> &l2p_rules,
            std::map<std::string, uint8_t> &conditional_names)
{
	cainteoir::buffer context{ nullptr, nullptr };
	cainteoir::buffer left{ nullptr, nullptr };
	cainteoir::buffer right{ nullptr, nullptr };
	std::pair<std::string, cainteoir::buffer> *rule = nullptr;
	while (reader.read()) switch (reader.type())
	{
	case cainteoir_file_reader::text:
		{
			if (reader.match().compare("end") == 0 && left.empty())
				return;

			if ((*reader.match().begin() == '@' || *reader.match().begin() == '!') && rule)
			{
				std::string name{ reader.match().begin() + 1, reader.match().end() };
				auto match = conditional_names.find(name);
				if (match == conditional_names.end())
					throw std::runtime_error("rule conditional not found");
				char conditional[3] = { *reader.match().begin(), (char)match->second, 0 };
				rule->first = conditional + rule->first;
			}

			auto last = reader.match().end();
			if (*--last == ')')
				left = reader.match();
			else if (*reader.match().begin() == '(')
				right = reader.match();
			else
				context = reader.match();
		}
		break;
	case cainteoir_file_reader::phonemes:
		if (!context.empty())
		{
			auto &group = l2p_rules[*context.begin()];

			std::string pattern = context.str();
			for (auto c : cainteoir::reverse(left))
				pattern.push_back(c);
			for (auto c : right)
				pattern.push_back(c);

			group.push_back({ pattern, reader.match() });
			rule = &group.back();
			context = { nullptr, nullptr };
			left = { nullptr, nullptr };
			right = { nullptr, nullptr };
		}
		break;
	}
}

uint8_t
parse_conditional(cainteoir_file_reader &reader,
                  std::list<conditional_t> &conditionals,
                  std::map<std::string, uint8_t> &conditional_names,
                  uint8_t current)
{
	if (current == 255)
		throw std::runtime_error("too many conditional variables");

	// set @conditional if type=value
	// set !conditional if type=value

	if (!reader.read() || reader.type() != cainteoir_file_reader::text)
		throw std::runtime_error("invalid conditional expression");

	if (reader.match().size() <= 1)
		throw std::runtime_error("invalid conditional expression");

	uint8_t set = tts::LANGDB_CONDRULE_SET;
	if (*reader.match().begin() == '!')
		set = tts::LANGDB_CONDRULE_UNSET;
	else if (*reader.match().begin() != '@')
		throw std::runtime_error("invalid conditional expression");

	std::string name{ reader.match().begin() + 1, reader.match().end() };

	if (!reader.read() || reader.type() != cainteoir_file_reader::text)
		throw std::runtime_error("invalid conditional expression");

	if (reader.match().compare("if") != 0)
		throw std::runtime_error("invalid conditional expression");

	if (!reader.read() || reader.type() != cainteoir_file_reader::text)
		throw std::runtime_error("invalid conditional expression");

	std::string expression = reader.match().str();
	if (expression.find("locale=") == 0)
	{
		uint8_t c;

		auto match = conditional_names.find(name);
		if (match == conditional_names.end())
			c = conditional_names[name] = current++;
		else
			c = match->second;

		std::string locale = expression.substr(7);
		conditionals.push_back({ c, (uint8_t)(tts::LANGDB_CONDRULE_LOCALE | set), locale });
	}
	else
		throw std::runtime_error("unsupported conditional expression");

	return current;
}

void
parse_classdef(cainteoir_file_reader &reader,
               std::map<uint8_t, std::list<cainteoir::buffer>> &classdefs)
{
	if (!reader.read() || reader.type() != cainteoir_file_reader::text || reader.match().size() != 1)
		return;

	uint8_t c = *reader.match().begin();
	if (c < 'A' || c > 'Z')
		fprintf(stderr, "unsupported class definition '%c'\n", c);

	std::list<cainteoir::buffer> matches;
	while (reader.read()) switch (reader.type())
	{
	case cainteoir_file_reader::text:
		if (reader.match().compare("end") == 0)
		{
			if (c >= 'A' && c <= 'Z')
				classdefs[c] = matches;
			return;
		}
		matches.push_back(reader.match());
		break;
	}
}

void
tts::compile_language(const char *aFileName, FILE *aOutput)
{
	if (!aOutput) return;

	cainteoir::buffer locale{ nullptr, nullptr };
	cainteoir::buffer phonemeset{ nullptr, nullptr };
	std::map<uint8_t, std::list<cainteoir::buffer>> classdefs;
	std::map<uint8_t, std::list<std::pair<std::string, cainteoir::buffer>>> rewrite_rules;
	std::map<uint8_t, std::list<std::pair<std::string, cainteoir::buffer>>> l2p_rules;
	std::list<conditional_t> conditionals;
	std::map<std::string, uint8_t> conditional_names;
	uint8_t current_conditional = 1;
	tts::dictionary dict;
	uint8_t boundary = ' ';

	auto reader = cainteoir_file_reader(cainteoir::path(aFileName));
	while (reader.read())
	{
		if (reader.type() == cainteoir_file_reader::directive)
		{
			if (reader.match().compare(".locale") == 0)
			{
				reader.read();
				locale = reader.match();
			}
			else if (reader.match().compare(".phonemeset") == 0)
			{
				reader.read();
				phonemeset = reader.match();
			}
		}
		else if (reader.type() == cainteoir_file_reader::text)
		{
			if (reader.match().compare("rules") == 0)
			{
				parse_rules(reader, l2p_rules, conditional_names);
			}
			else if (reader.match().compare("classdef") == 0)
			{
				parse_classdef(reader, classdefs);
			}
			else if (reader.match().compare("set") == 0)
			{
				current_conditional = parse_conditional(reader, conditionals, conditional_names, current_conditional);
			}
			else if (reader.match().compare("dictionary") == 0)
			{
				reader.read();
				auto dict_reader = tts::createDictionaryReader(reader.match().str().c_str());
				if (!dict_reader)
					throw std::runtime_error("unsupported dictionary format");
				while (dict_reader->read())
					dict.add_entry(dict_reader->word, dict_reader->entry);
			}
			else if (reader.match().compare("rewrite") == 0)
			{
				parse_rewrite(reader, rewrite_rules);
			}
			else if (reader.match().compare("boundary") == 0)
			{
				if (reader.read() && reader.type() == cainteoir_file_reader::text)
					boundary = *reader.match().begin();
			}
		}
	}

	binary_file_writer out(aOutput);

	// Language Database Header
	out.begin_section("LANGDB", tts::LANGDB_HEADER_SIZE, true);
	out.u16(0x3031); // endianness
	out.pstr(locale);
	out.pstr(phonemeset);
	out.u8(boundary);
	out.end_section();

	if (!conditionals.empty())
	{
		uint32_t entries = conditionals.size() * tts::CONDRULE_TABLE_ENTRY_SIZE;
		out.begin_section("CND", tts::CONDRULE_TABLE_SIZE + entries, true);
		out.u16(conditionals.size());
		for (const auto &entry : conditionals)
		{
			out.u8(entry.c);
			out.u8(entry.type);
			out.pstr(entry.value);
		}
		out.end_section();
	}

	for (auto &classdef : classdefs)
	{
		uint32_t entries = (classdef.second.size() + 1) * tts::CLASSDEF_TABLE_ENTRY_SIZE;
		out.begin_section("CLS", tts::CLASSDEF_TABLE_SIZE + entries, true);
		out.u16(classdef.second.size() + 1);
		out.u8(classdef.first);
		for (const auto &entry : classdef.second)
			out.pstr(entry);
		out.u32(0); // end of classdef
		out.end_section();
	}

	for (auto &group : rewrite_rules)
	{
		group.second.push_back({ {}, { nullptr, nullptr }});

		uint32_t entries = group.second.size() * tts::LEXICAL_REWRITE_RULES_TABLE_ENTRY_SIZE;
		out.begin_section("LRR", tts::LEXICAL_REWRITE_RULES_TABLE_SIZE + entries, true);
		out.u16(group.second.size());
		out.u8(group.first);
		for (const auto &entry : group.second)
		{
			out.pstr(entry.first);
			out.pstr(entry.second);
		}
		out.end_section();
	}

	for (auto &group : l2p_rules)
	{
		group.second.push_back({ {}, { nullptr, nullptr }});

		uint32_t entries = group.second.size() * tts::LETTER_TO_PHONEME_TABLE_ENTRY_SIZE;
		out.begin_section("L2P", tts::LETTER_TO_PHONEME_TABLE_SIZE + entries, true);
		out.u16(group.second.size());
		out.u8(group.first);
		for (const auto &entry : group.second)
		{
			out.pstr(entry.first);
			out.pstr(entry.second);
		}
		out.end_section();
	}

	auto writer = tts::createPhonemeWriter(phonemeset.str().c_str());
	std::map<std::string, std::string> words;
	for (const auto &entry : dict)
	{
		ipa::phonemes phonemes;
		if (dict.pronounce(entry.first, {}, phonemes))
		{
			cainteoir::memory_file out;
			writer->reset(out);
			for (const auto &p : phonemes)
				writer->write(p);

			words[entry.first->str()] = out.buffer()->str();
		}
	}

	if (!words.empty())
	{
		uint32_t entries = words.size() * tts::DICTIONARY_TABLE_ENTRY_SIZE;
		out.begin_section("DIC", tts::DICTIONARY_TABLE_SIZE + entries, true);
		out.u16(words.size());
		for (const auto &entry : words)
		{
			out.pstr(entry.first);
			out.pstr(entry.second);
		}
		out.end_section();
	}
}
