/* MBROLA pho file.
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

#include <cainteoir/prosody.hpp>

namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace css = cainteoir::css;

std::shared_ptr<tts::prosody_reader>
tts::createPhoReader(const std::shared_ptr<phoneme_reader> &aPhonemeSet)
{
	return {};
}

struct pho_writer : public tts::prosody_writer
{
	pho_writer(const std::shared_ptr<tts::phoneme_writer> &aPhonemeSet);

	void reset(FILE *aOutput);

	bool write(const tts::prosody &aProsody);
private:
	std::shared_ptr<tts::phoneme_writer> mPhonemeSet;
	FILE *mOutput;
};

pho_writer::pho_writer(const std::shared_ptr<tts::phoneme_writer> &aPhonemeSet)
	: mPhonemeSet(aPhonemeSet)
	, mOutput(nullptr)
{
}

void pho_writer::reset(FILE *aOutput)
{
	mPhonemeSet->reset(aOutput);
	mOutput = aOutput;
}

bool pho_writer::write(const tts::prosody &aProsody)
{
	if (!mOutput) return false;

	if (!mPhonemeSet->write(aProsody.phoneme.get(ipa::main | ipa::diacritics | ipa::length)))
		return false;

	fprintf(mOutput, " %G", aProsody.duration.as(css::time::milliseconds).value());
	for (auto &entry : aProsody.envelope)
		fprintf(mOutput, " %d %G", entry.offset, entry.pitch.as(css::frequency::hertz).value());
	fprintf(mOutput, "\n");
	return true;
}

std::shared_ptr<tts::prosody_writer>
tts::createPhoWriter(const std::shared_ptr<phoneme_writer> &aPhonemeSet)
{
	return std::make_shared<pho_writer>(aPhonemeSet);
}
