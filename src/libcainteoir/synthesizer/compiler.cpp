/* Voice Database File Builder.
 *
 * Copyright (C) 2014 Reece H. Dunn
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
#include "../cainteoir_file_reader.hpp"

#include "synth.hpp"

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

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

struct buffer_compare
{
	bool operator()(const cainteoir::buffer &a, const cainteoir::buffer &b) const
	{
		return a.compare(b) < 0;
	}
};

struct binary_file_writer
{
	binary_file_writer(FILE *aOutput)
		: mOutput(aOutput)
		, mOffset(0)
	{
	}

	void begin_section(const char *magic, uint16_t section_size, bool has_pstr_fields);
	void end_section();

	void u8(uint8_t u)   { fputc(u, mOutput); }
	void u16(uint16_t u) { fwrite(&u, sizeof(u), 1, mOutput); }
	void u32(uint32_t u) { fwrite(&u, sizeof(u), 1, mOutput); }
	void u64(uint64_t u) { fwrite(&u, sizeof(u), 1, mOutput); }

	void f8_8(float f);

	void  str(const cainteoir::buffer &data);
	void pstr(const cainteoir::buffer &data);
private:
	FILE *mOutput;
	uint16_t mOffset;
	std::list<cainteoir::buffer> mStrings;
	std::map<cainteoir::buffer, uint16_t, buffer_compare> mStringTable;
};

void binary_file_writer::begin_section(const char *magic, uint16_t section_size, bool has_pstr_fields)
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
		u16(mOffset);
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

void binary_file_writer::str(const cainteoir::buffer &data)
{
	fwrite(data.begin(), data.size(), 1, mOutput);
	fputc('\0', mOutput);
}

void binary_file_writer::pstr(const cainteoir::buffer &data)
{
	auto &entry = mStringTable[data];
	if (entry == 0)
	{
		entry = mOffset;
		u16(mOffset);
		mOffset += data.size() + 1;
		mStrings.push_back(data);
	}
	else
		u16(entry);
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

struct phoneme_t
{
	ipa::phoneme phoneme1;
	ipa::phoneme phoneme2;
	css::time duration_mean;
	css::time duration_stdev;
	std::list<unit_t> units;

	phoneme_t()
	{
	}
};

static phoneme_t
parse_phoneme(cainteoir_file_reader &reader, const std::shared_ptr<tts::phoneme_parser> &parser)
{
	enum state_t
	{
		in_phoneme,
		pre_duration_mean,
		pre_duration_stdev,
		in_phoneme_body,
		in_unit,
	};

	state_t state = in_phoneme;
	phoneme_t entry;
	unit_t *unit = nullptr;

	while (reader.read()) switch (reader.type())
	{
	case cainteoir_file_reader::text:
		if (reader.match().compare("end") == 0)
			return entry;
		else if (reader.match().compare("unit") == 0)
		{
			entry.units.push_back({});
			unit = &entry.units.back();
			state = in_unit;
		}
		else switch (state)
		{
		case pre_duration_mean:
			entry.duration_mean = css::parse_smil_time(reader.match());
			state = pre_duration_stdev;
			break;
		case pre_duration_stdev:
			entry.duration_mean = css::parse_smil_time(reader.match());
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
				state = pre_duration_mean;
			}
			break;
		}
		break;
	case cainteoir_file_reader::string:
		switch (state)
		{
		case in_unit:
			unit->name = reader.match();
			state = in_phoneme_body;
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
	cainteoir::buffer phonemeset{ nullptr, nullptr };
	uint8_t gender = 0;
	float volume_scale = 1.0;
	uint16_t frequency = 0;
	uint8_t channels = 0;
	cainteoir::buffer sample_format{ nullptr, nullptr };

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
			else if (reader.match().compare(".phonemeset") == 0)
			{
				reader.read();
				phonemeset = reader.match();
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
		}
		else if (reader.type() == cainteoir_file_reader::text)
		{
			if (reader.match().compare("phoneme") == 0)
			{
				phonemes.push_back(parse_phoneme(reader, parser));
				if (phonemes.back().duration_mean.units() != css::time::inherit)
					++duration_entries;
				unit_entries += phonemes.back().units.size();
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
	out.pstr(phonemeset);
	out.u8(gender);
	out.f8_8(volume_scale);
	out.u16(frequency);
	out.u8(channels);
	out.pstr(sample_format);
	out.end_section();

	if (duration_entries != 0)
	{
		out.begin_section("DUR", tts::TABLE_SECTION_SIZE + (duration_entries * tts::DURATION_TABLE_ENTRY_SIZE), false);
		out.u16(duration_entries);
		for (const auto &entry : phonemes)
		{
			if (entry.duration_mean.units() != css::time::inherit)
			{
				out.u64(entry.phoneme1.get(ipa::main | ipa::diacritics | ipa::suprasegmentals));
				out.u64(entry.phoneme2.get(ipa::main | ipa::diacritics | ipa::suprasegmentals));
				out.u8((int)entry.duration_mean.as(css::time::milliseconds).value());
				if (entry.duration_stdev.units() == css::time::inherit)
					out.u8(0);
				else
					out.u8((int)entry.duration_stdev.as(css::time::milliseconds).value());
			}
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
