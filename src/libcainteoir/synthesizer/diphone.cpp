/* Phoneme to diphone prosody conversion.
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

struct diphone_reader : public tts::prosody_reader
{
	diphone_reader(const std::shared_ptr<tts::prosody_reader> &aProsody);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	std::shared_ptr<tts::prosody_reader> mProsody;
	float mRemainingDuration;
	std::vector<tts::envelope_t> mRemainingEnvelope;
	bool mLastDiphone;
};

diphone_reader::diphone_reader(const std::shared_ptr<tts::prosody_reader> &aProsody)
	: mProsody(aProsody)
	, mLastDiphone(false)
	, mRemainingDuration(0)
{
	phoneme1 = ipa::unspecified;
	phoneme2 = ipa::unspecified;
	phoneme3 = ipa::pause | ipa::extra_short;
	phoneme4 = ipa::unspecified;
}

void diphone_reader::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
	mProsody->reset(aBuffer);
	mLastDiphone = false;

	phoneme1 = ipa::unspecified;
	phoneme2 = ipa::unspecified;
	phoneme3 = ipa::pause | ipa::extra_short;
	phoneme4 = ipa::unspecified;
}

bool diphone_reader::read()
{
	phoneme1 = phoneme3;
	phoneme2 = phoneme4;

	float currentDuration = mRemainingDuration;

	envelope = mRemainingEnvelope;
	mRemainingEnvelope.clear();

	if (mProsody->read())
	{
		phoneme3 = mProsody->phoneme1;
		phoneme4 = mProsody->phoneme2;

		mRemainingDuration = mProsody->duration.as(css::time::milliseconds).value() / 2;
		currentDuration += mRemainingDuration;

		bool haveMidpoint = false;
		for (auto && env : mProsody->envelope)
		{
			if (env.offset == 0 && !envelope.empty() && envelope.back().offset == 50)
			{
				css::frequency pitch = envelope.back().pitch;
				if (pitch.units() != env.pitch.units() || pitch.value() != env.pitch.value())
				{
					fflush(stdout);
					fprintf(stderr, "warning: mismatched start/end pitch points\n");
					fflush(stderr);
				}
			}
			else if (env.offset < 50)
			{
				envelope.push_back({ env.offset + 50, env.pitch });
			}
			else if (env.offset == 50)
			{
				haveMidpoint = true;
				envelope.push_back({ 100, env.pitch });
				mRemainingEnvelope.push_back({ 0, env.pitch });
			}
			else // if (env.offset >= 50)
			{
				if (!haveMidpoint)
				{
					haveMidpoint = true;
					float pitch2 = env.pitch.value();
					float pitch1 = envelope.empty() ? pitch2 : envelope.back().pitch.value();
					if (pitch1 != pitch2)
					{
						int offset2 = envelope.back().offset - 50;
						int offset1 = env.offset;

						float dPitch = (pitch2 - pitch1) / (offset2 - offset1);
						pitch1 = ((50 - offset1) * dPitch) + pitch1;
					}

					envelope.push_back({ 100, { pitch1, css::frequency::hertz }});
					mRemainingEnvelope.push_back({ 0, { pitch1, css::frequency::hertz }});
				}
				mRemainingEnvelope.push_back({ env.offset - 50, env.pitch });
			}
		}
	}
	else if (mLastDiphone)
		return false;
	else
	{
		mLastDiphone = true;
		phoneme3 = ipa::pause | ipa::extra_short;
		phoneme4 = ipa::unspecified;
	}

	duration = { currentDuration, css::time::milliseconds };

	return true;
}

std::shared_ptr<tts::prosody_reader>
tts::createDiphoneReader(const std::shared_ptr<prosody_reader> &aProsody)
{
	return std::make_shared<diphone_reader>(aProsody);
}
