/* A duration model from a file.
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
#include "i18n.h"
#include "compatibility.hpp"

#include <cainteoir/synthesizer.hpp>
#include "../cainteoir_file_reader.hpp"
#include "synth.hpp"

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

struct duration_model_t : public tts::duration_model
{
	duration_model_t(const std::shared_ptr<cainteoir::buffer> &aDurationModel);

	duration_model_t(cainteoir::native_endian_buffer &aData);

	const tts::duration &lookup(const tts::phone &p) const;
private:
	struct phone_compare
	{
		bool operator()(const tts::phone &a, const tts::phone &b) const
		{
			if (a.phoneme1 <  b.phoneme1) return true;
			if (a.phoneme1 == b.phoneme1) return a.phoneme2 < b.phoneme2;
			return false;
		}
	};

	std::map<tts::phone, tts::duration, phone_compare> mDurations;
};

duration_model_t::duration_model_t(const std::shared_ptr<cainteoir::buffer> &aDurationModel)
{
	cainteoir_file_reader reader(aDurationModel);
	std::shared_ptr<tts::phoneme_parser> phonemeset;
	while (reader.read())
	{
		if (reader.type() == cainteoir_file_reader::directive)
		{
			if (reader.match().compare(".type") == 0)
			{
				reader.read();
				if (reader.match().compare("tabular") != 0)
					throw std::runtime_error("unsupported duration model type");
			}
			else if (reader.match().compare(".phonemeset") == 0)
			{
				reader.read();
				phonemeset = tts::createPhonemeParser(reader.match().str().c_str());
			}
		}
		else if (reader.type() == cainteoir_file_reader::phonemes)
		{
			auto phoneme = reader.match();

			if (!reader.read() || reader.type() != cainteoir_file_reader::text)
				throw std::runtime_error("missing mean duration value");
			auto mean = css::parse_smil_time(reader.match());

			if (!reader.read() || reader.type() != cainteoir_file_reader::text)
				throw std::runtime_error("missing standard deviation duration value");
			auto sdev = css::parse_smil_time(reader.match());

			auto begin_phoneme = phoneme.begin();
			tts::phone p;
			if (phonemeset->parse(begin_phoneme, phoneme.end(), p.phoneme1))
			{
				if (!phonemeset->parse(begin_phoneme, phoneme.end(), p.phoneme2))
					p.phoneme2 = ipa::unspecified;
			}

			if (phoneme.empty() || p.phoneme1 == ipa::unspecified)
				throw std::runtime_error("unrecognised phoneme");

			mDurations.insert({ p, tts::duration{ mean, sdev }});
		}
		else
			throw std::runtime_error("invalid duration model contents");
	}
}

duration_model_t::duration_model_t(cainteoir::native_endian_buffer &aData)
{
	for (auto i : cainteoir::range<uint16_t>(0, aData.u16()))
	{
		tts::phone p;
		p.phoneme1 = ipa::phoneme{ aData.u64() };
		p.phoneme2 = ipa::phoneme{ aData.u64() };

		css::time mean{ (float)aData.u8(), css::time::milliseconds };
		css::time sdev{ (float)aData.u8(), css::time::milliseconds };
		mDurations.insert({ p, tts::duration{ mean, sdev }});
	}
}

const tts::duration &duration_model_t::lookup(const tts::phone &p) const
{
	const auto mask = ipa::main | ipa::diacritics | ipa::length; // no tone or stress

	static const tts::duration null_duration;
	static const tts::duration utterance_pause = { {  25, css::time::milliseconds }, {  5, css::time::milliseconds } };
	static const tts::duration clause_pause    = { { 200, css::time::milliseconds }, { 50, css::time::milliseconds } };

	auto match = mDurations.find({ p.phoneme1.get(mask), p.phoneme2.get(mask), {}});
	if (match != mDurations.end())
		return match->second;

	if (p.phoneme2 == ipa::unspecified) switch (p.phoneme1.get(ipa::phoneme_type))
	{
	case ipa::foot_break:
		return utterance_pause;
	case ipa::intonation_break:
		return clause_pause;
	}

	return null_duration;
}

std::shared_ptr<tts::duration_model>
tts::createDurationModel(const std::shared_ptr<cainteoir::buffer> &aDurationModel)
{
	if (!aDurationModel.get()) return {};
	return std::make_shared<duration_model_t>(aDurationModel);
}

std::shared_ptr<tts::duration_model>
tts::createDurationModel(cainteoir::native_endian_buffer &aData)
{
	return std::make_shared<duration_model_t>(aData);
}
