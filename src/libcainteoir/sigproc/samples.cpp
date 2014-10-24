/* Extract samples from an audio file.
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

#include <cainteoir/sigproc.hpp>
#include <stdexcept>

cainteoir::audio_data<short>
cainteoir::read_s16_samples(const std::shared_ptr<cainteoir::buffer> &aData,
                            const css::time &aStart,
                            const css::time &aEnd,
                            int aChannel,
                            int aFrequency)
{
	auto player = cainteoir::create_media_reader(aData);
	if (!player)
		throw std::runtime_error("unable to read the audio file");

	cainteoir::audio_data<short> data;
	data.info = cainteoir::create_audio_info(
		rdf::tts("s16le"),
		player->channels(),
		(aFrequency == -1) ? player->frequency() : aFrequency);

	player->set_interval(aStart, aEnd);
	player->set_target(data.info);
	while (player->read())
	{
		const short *current = (const short *)player->data.begin();
		const short *last    = (const short *)player->data.end();
		int channels = player->channels();
		switch (channels)
		{
		case 1:
			data.samples.insert(data.samples.end(), current, last);
			break;
		default:
			{
				uint32_t n = 0;
				for (; current != last; ++current)
				{
					if (n % channels == aChannel)
						data.samples.push_back(*current);
					++n;
				}
			}
			break;
		}
	}

	return data;
}
