/* Phoneme File Parser.
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
#include "i18n.h"
#include "compatibility.hpp"

#include "phonemeset.hpp"

#include <cainteoir/unicode.hpp>
#include <cainteoir/path.hpp>
#include <ucd/ucd.h>

namespace tts = cainteoir::tts;

enum class type : uint8_t
{
	unknown,
	hexadecimal,
	whitespace,
};

#define _ type::unknown
#define H type::hexadecimal
#define S type::whitespace

static const type classification[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, S, S, _, _, S, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ S, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 3x */ H, H, H, H, H, H, H, H, H, H, _, _, _, _, _, _,
	/* 4x */ _, H, H, H, H, H, H, _, _, _, _, _, _, _, _, _,
	/* 5x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 6x */ _, H, H, H, H, H, H, _, _, _, _, _, _, _, _, _,
	/* 7x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 8x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 9x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ax */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Bx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Cx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Dx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ex */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Fx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
};

#undef _
#undef H
#undef S

#define _ 0

static const uint8_t feature_char[256] = {
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
	/* 0x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 1x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 2x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 3x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, _, _, _, _, _, _,
	/* 4x */ _, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 5x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, _, _, _, _, _,
	/* 6x */ _, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	/* 7x */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, _, _, _, _, _,
	/* 8x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* 9x */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ax */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Bx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Cx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Dx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Ex */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	/* Fx */ _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
	//////// x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
};

#undef _

static ucd::codepoint_t hex_to_unicode(const char * &current, const char *last)
{
	char escaped[9] = {0};
	char *end = escaped;
	while (current <= last && end <= end + 9 &&
	       classification[*current] == ::type::hexadecimal)
		*end++ = *current++;
	return strtoul(escaped, nullptr, 16);
}

tts::phoneme_file_reader::context_t::context_t(const std::string &aPhonemeSet)
{
	auto location = get_data_path() / "phonemeset" / (aPhonemeSet + ".phon");
	mBuffer = make_file_buffer(location);
	mCurrent = mBuffer->begin();
	mLast = mBuffer->end();
}

tts::phoneme_file_reader::phoneme_file_reader(const std::string &aPhonemeSet)
	: applicator(0)
	, type(placement::primary)
	, mState(state::prelude)
{
	feature[0] = feature[1] = feature[2] = feature[3] = 0;
	context[0] = context[1] = context[2] = context[3] = 0;

	mFiles.push({ aPhonemeSet });
	read();
	if (phoneme_type.empty())
		throw std::runtime_error("phonemeset type is not specified");
}

bool tts::phoneme_file_reader::read()
{
	feature[0] = feature[1] = feature[2] = feature[3] = 0;
	context[0] = context[1] = context[2] = context[3] = 0;

	context_t *top = &mFiles.top();
	while (top)
	{
		if (top->mCurrent == top->mLast)
		{
			mFiles.pop();
			top = mFiles.empty() ? nullptr : &mFiles.top();
			continue;
		}

		const char *start = nullptr;
		while (top->mCurrent < top->mLast) switch (*top->mCurrent)
		{
		case '#':
			while (top->mCurrent <= top->mLast && (*top->mCurrent != '\n'))
				++top->mCurrent;
			break;
		case ' ': case '\t': case '\r': case '\n':
			++top->mCurrent;
			break;
		case '/':
			switch (mState)
			{
			case state::have_transcription:
				++top->mCurrent;
				start = top->mCurrent;
				while (top->mCurrent <= top->mLast && (*top->mCurrent != '/'))
					++top->mCurrent;
				if (top->mCurrent == top->mLast)
					throw std::runtime_error("unterminated phoneme group (expecting '/')");
				phonemes.clear();
				while (start < top->mCurrent)
				{
					auto ret = tts::read_explicit_feature(start, top->mCurrent);
					if (ret.first)
						phonemes.push_back(ret.second);
				}
				++top->mCurrent;
				type = placement::primary;
				mState = state::need_transcription;
				return true;
			default:
				throw std::runtime_error("expected transcription before phonemes");
			}
			break;
		case '?':
			switch (mState)
			{
			case state::have_transcription:
				++top->mCurrent;
				read_feature(context);
				mState = state::have_context;
				break;
			default:
				throw std::runtime_error("unexpected context rule");
			}
			break;
		case '<':
		case '>':
			switch (mState)
			{
			case state::have_transcription:
			case state::have_context:
				type = (*top->mCurrent == '<') ? placement::before : placement::after;
				++top->mCurrent;
				while (top->mCurrent != top->mLast && *top->mCurrent == '\t')
					++top->mCurrent;
				switch (*top->mCurrent)
				{
				case '=':
				case '+':
				case '-':
					applicator = *top->mCurrent++;
					break;
				default:
					throw std::runtime_error("unrecognised combiner syntax");
				}
				read_feature(feature);
				mState = state::need_transcription;
				return true;
			default:
				throw std::runtime_error("expected transcription before combiner");
			}
			break;
		case '.':
			{
				const char *begin_entry = top->mCurrent;
				const char *end_entry   = top->mCurrent;
				while (end_entry != top->mLast &&
				       classification[*end_entry] != ::type::whitespace)
					++end_entry;

				top->mCurrent = end_entry;
				while (top->mCurrent != top->mLast && *top->mCurrent == '\t')
					++top->mCurrent;

				const char *begin_definition = top->mCurrent;
				const char *end_definition   = top->mCurrent;
				while (end_definition != top->mLast &&
				       classification[*end_definition] != ::type::whitespace)
					++end_definition;

				top->mCurrent = end_definition;
				while (top->mCurrent != top->mLast &&
				      (*top->mCurrent == '\r' || *top->mCurrent == '\n'))
					++top->mCurrent;

				std::string entry(begin_entry, end_entry);
				if (entry == ".import")
				{
					std::string definition(begin_definition, end_definition);
					mFiles.push({ definition });
					top = &mFiles.top();
				}
				else if (entry == ".type")
					phoneme_type = std::string(begin_definition, end_definition);
			}
			break;
		default:
			switch (mState)
			{
			case state::prelude:
				mState = state::need_transcription;
				return true;
			case state::need_transcription:
				{
					char data[64];
					char *end = data;
					while (top->mCurrent <= top->mLast && end <= data + 60 &&
					       classification[*top->mCurrent] != ::type::whitespace)
					{
						if (*top->mCurrent == '\\')
						{
							++top->mCurrent;
							if (top->mCurrent <= top->mLast && *top->mCurrent == 'u')
							{
								++top->mCurrent;
								end = cainteoir::utf8::write(end, hex_to_unicode(top->mCurrent, top->mLast));
								continue;
							}
						}
						*end++ = *top->mCurrent++;
					}
					if (top->mCurrent <= top->mLast)
						transcription = cainteoir::make_buffer(data, end-data);
					mState = state::have_transcription;
				}
				break;
			default:
				throw std::runtime_error("transcription without phoneme or diacritic rule");
			}
			break;
		}
	}
	return false;
}

void tts::phoneme_file_reader::read_feature(char (&aFeature)[4])
{
	context_t *top = &mFiles.top();

	char *end = aFeature;
	while (top->mCurrent <= top->mLast && end <= aFeature + 3 &&
	       feature_char[*top->mCurrent])
		*end++ = *top->mCurrent++;

	// check if the feature is valid ...
	ipa::phoneme().get(aFeature);
}

tts::transcription_reader::transcription_reader(tts::phoneme_file_reader &aPhonemeSet)
{
	while (aPhonemeSet.read()) switch (aPhonemeSet.type)
	{
	case tts::placement::primary:
		if (aPhonemeSet.phonemes.size() != 1)
			throw std::runtime_error("ipa-style phonemesets only support mapping to one phoneme");

		mPhonemes.insert(*aPhonemeSet.transcription, { aPhonemeSet.phonemes.front() });
		break;
	case tts::placement::after:
		switch (aPhonemeSet.applicator)
		{
		case '=':
			auto &phoneme = mPhonemes.insert(*aPhonemeSet.transcription);
			phoneme.type = aPhonemeSet.type;
			phoneme.rule.push_back({ aPhonemeSet.feature, aPhonemeSet.context });
			break;
		}
		break;
	}
}

std::pair<bool, tts::phoneme> tts::transcription_reader::read(const char * &mCurrent, const char *mEnd) const
{
	placement state = placement::before;
	const char *pos = mCurrent;
	tts::phoneme p;
	while (true)
	{
		auto match = next_match(mCurrent, mEnd);
		switch (state)
		{
		case placement::before:
			switch (match.second.type)
			{
			case placement::error:
				{
					mCurrent = match.first;
					uint8_t c = *mCurrent;
					++mCurrent;

					char msg[64];
					if (c <= 0x20 || c >= 0x80)
						sprintf(msg, i18n("unrecognised character 0x%02X"), c);
					else
						sprintf(msg, i18n("unrecognised character %c"), c);
					throw tts::phoneme_error(msg);
				}
				break;
			case placement::before:
				throw std::runtime_error("placement before not supported in transcription_reader");
			case placement::primary:
				p = match.second.phoneme;
				state = placement::after;
				pos = match.first;
				break;
			case placement::after:
				throw tts::phoneme_error("no phoneme before post-phoneme modifiers");
			case placement::none:
				return { false, tts::phoneme(-1) };
			}
			break;
		case placement::after:
			switch (match.second.type)
			{
			case placement::none:
			case placement::error:
			case placement::before:
			case placement::primary:
				mCurrent = pos;
				return { true, p };
			case placement::after:
				for (const auto &rule : match.second.rule)
				{
					if (rule.context[0] == 0 || p.get(rule.context))
					{
						p.set(rule.feature);
						pos = match.first;
						break;
					}
				}
				break;
			}
			break;
		}
		mCurrent = match.first;
	}
}

std::pair<const char *, const tts::transcription_reader::phoneme_t &>
tts::transcription_reader::next_match(const char *mCurrent, const char *mEnd) const
{
	static const phoneme_t no_match = {};
	static const phoneme_t error = { placement::error };

	const auto *entry = mPhonemes.root();
	decltype(mPhonemes.root()) match = nullptr;
	const char *pos = mCurrent;
	while (mCurrent < mEnd)
	{
		const auto *next = entry->get(*mCurrent);
		if (next == nullptr)
		{
			if (match)
				return { pos, match->item };

			return { pos, error };
		}
		else
		{
			entry = next;
			++mCurrent;
			if (entry->item.type != placement::none)
			{
				match = entry;
				pos = mCurrent;
			}
		}
	}
	if (match)
		return { pos, match->item };
	return { mEnd, no_match };
}

tts::transcription_writer::transcription_writer(tts::phoneme_file_reader &aPhonemeSet)
{
	while (aPhonemeSet.read()) switch (aPhonemeSet.type)
	{
	case tts::placement::primary:
		if (aPhonemeSet.phonemes.size() != 1)
			throw std::runtime_error("ipa-style phonemesets only support mapping to one phoneme");

		mPhonemes[aPhonemeSet.phonemes.front()] = aPhonemeSet.transcription;
		break;
	case tts::placement::after:
		switch (aPhonemeSet.applicator)
		{
		case '=':
			mAfter.push_back({ aPhonemeSet.feature, aPhonemeSet.context, aPhonemeSet.transcription });
			break;
		}
		break;
	}
}

bool tts::transcription_writer::write(FILE *aOutput, const tts::phoneme &aPhoneme) const
{
	tts::phoneme main{ aPhoneme.get(ipa::main) };

	auto match = mPhonemes.find(main);
	if (match != mPhonemes.end())
	{
		fwrite(match->second->begin(), 1, match->second->size(), aOutput);
		for (auto && rule : mAfter)
		{
			if ((rule.context[0] == 0 || aPhoneme.get(rule.context)) && aPhoneme.get(rule.feature))
				fwrite(rule.transcription->begin(), 1, rule.transcription->size(), aOutput);
		}
		return true;
	}
	return false;
}
