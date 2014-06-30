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

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

struct duration_model_t : public tts::duration_model
{
	duration_model_t(const std::shared_ptr<cainteoir::buffer> &aDurationModel);

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
	const char *current = aDurationModel->begin();
	const char *end     = aDurationModel->end();
	std::shared_ptr<tts::phoneme_parser> phonemeset;
	while (current != end) switch (*current)
	{
	case '#':
		while (current != end && *current != '\n') ++current;
		break;
	case '\r': case '\n':
		++current;
		break;
	case '.':
		{
			const char *begin_entry = current;
			const char *end_entry   = current;
			while (end_entry != end &&
			       !(*end_entry == ' ' || *end_entry == '\t'))
				++end_entry;

			current = end_entry;
			while (current != end && (*current == ' ' || *current == '\t'))
				++current;

			const char *begin_definition = current;
			const char *end_definition   = current;
			while (end_definition != end &&
			       !(*end_definition == '\r' || *end_definition == '\n'))
				++end_definition;

			current = end_definition;

			std::string entry(begin_entry, end_entry);
			std::string value(begin_definition, end_definition);
			if (entry == ".type")
			{
				if (value != "tabular")
					throw std::runtime_error("unsupported duration model type");
			}
			else if (entry == ".phonemeset")
				phonemeset = tts::createPhonemeParser(value.c_str());
		}
		break;
	case '/':
		{
			++current;
			const char *begin_phoneme = current;
			while (current != end && *current != '/')
				++current;
			const char *end_phoneme = current;
			++current;

			while (current != end && (*current == ' ' || *current == '\t'))
				++current;

			const char *begin_mean = current;
			while (current != end && !(*current == ' ' || *current == '\t'))
				++current;
			const char *end_mean = current;

			while (current != end && (*current == ' ' || *current == '\t'))
				++current;

			const char *begin_sdev = current;
			while (current != end && !(*current == ' ' || *current == '\t' || *current == '\r' || *current == '\n'))
				++current;
			const char *end_sdev = current;

			while (current != end && (*current == ' ' || *current == '\t'))
				++current;

			tts::phone p;
			if (phonemeset->parse(begin_phoneme, end_phoneme, p.phoneme1))
			{
				if (!phonemeset->parse(begin_phoneme, end_phoneme, p.phoneme2))
					p.phoneme2 = ipa::unspecified;
			}

			if (begin_phoneme != end_phoneme || p.phoneme1 == ipa::unspecified)
				throw std::runtime_error("unrecognised phoneme");

			cainteoir::buffer mean{ begin_mean, end_mean };
			cainteoir::buffer sdev{ begin_sdev, end_sdev };
			mDurations.insert({ p, tts::duration{ css::parse_smil_time(mean), css::parse_smil_time(sdev) }});
		}
		break;
	default:
		throw std::runtime_error("unrecognised character in duration file");
	}
}

const tts::duration &duration_model_t::lookup(const tts::phone &p) const
{
	const auto mask = ipa::main | ipa::diacritics | ipa::length; // no tone or stress

	static const tts::duration null_duration;
	auto match = mDurations.find({ p.phoneme1.get(mask), p.phoneme2.get(mask), {}});
	if (match != mDurations.end())
		return match->second;
	return null_duration;
}

std::shared_ptr<tts::duration_model>
tts::createDurationModel(const std::shared_ptr<cainteoir::buffer> &aDurationModel)
{
	if (!aDurationModel.get()) return {};
	return std::make_shared<duration_model_t>(aDurationModel);
}
