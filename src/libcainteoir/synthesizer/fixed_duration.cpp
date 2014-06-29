/* A duration model using fixed durations.
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
namespace css = cainteoir::css;

struct fixed_duration_model : public tts::duration_model
{
	fixed_duration_model(css::time aDuration)
		: mDuration(aDuration, { 0, aDuration.units() })
	{
	}

	const tts::duration &lookup(const tts::phone &p) const;
private:
	tts::duration mDuration;
};

const tts::duration &fixed_duration_model::lookup(const tts::phone &p) const
{
	return mDuration;
}

std::shared_ptr<tts::duration_model>
tts::createFixedDurationModel(css::time aDuration)
{
	return std::make_shared<fixed_duration_model>(aDuration);
}
