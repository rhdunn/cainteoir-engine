/* A pitch model.
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

tts::pitch_model::pitch_model(const css::frequency &aBase,
                              const css::frequency &aStep,
                              const css::frequency &aSdev)
	: top({  aBase.value() + (aStep.value() * 4), aBase.units() }, aSdev)
	, high({ aBase.value() + (aStep.value() * 3), aBase.units() }, aSdev)
	, mid({  aBase.value() + (aStep.value() * 2), aBase.units() }, aSdev)
	, low({  aBase.value() +  aStep.value(), aBase.units() }, aSdev)
	, bottom(aBase, aSdev)
{
}

const tts::pitch &tts::pitch_model::tone(ipa::phoneme::value_type aTone) const
{
	static const tts::pitch null_pitch;
	switch (aTone)
	{
	case ipa::tone_start_top:
	case ipa::tone_middle_top:
	case ipa::tone_end_top:
		return top;
	case ipa::tone_start_high:
	case ipa::tone_middle_high:
	case ipa::tone_end_high:
		return high;
	case ipa::tone_start_mid:
	case ipa::tone_middle_mid:
	case ipa::tone_end_mid:
		return mid;
	case ipa::tone_start_low:
	case ipa::tone_middle_low:
	case ipa::tone_end_low:
		return low;
	case ipa::tone_start_bottom:
	case ipa::tone_middle_bottom:
	case ipa::tone_end_bottom:
		return bottom;
	default:
		return null_pitch;
	}
}
