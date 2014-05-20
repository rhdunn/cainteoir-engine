/* Example program for interacting with voice synthesizers.
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
#include "compatibility.hpp"
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/engines.hpp>
#include <stdexcept>

namespace css = cainteoir::css;
namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace rdf = cainteoir::rdf;

int main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
		{
			fprintf(stderr, "usage: voice-synthesizer VOICE_NAME\n");
			return 0;
		}

		auto voice = tts::create_mbrola_voice(argv[1]);
		if (!voice) throw std::runtime_error("cannot find the specified MBROLA voice");

		fprintf(stdout, "channels    : %d\n", voice->channels());
		fprintf(stdout, "format      : %s\n", voice->format().str().c_str());
		fprintf(stdout, "sample rate : %d\n", voice->frequency());

		rdf::graph metadata;
		rdf::uri doc;
		auto out = cainteoir::open_audio_device(nullptr, metadata, doc,
			voice->format(),
			voice->channels(),
			voice->frequency());
		out->open();

		voice->write({ ipa::velar | ipa::plosive, ipa::unspecified, // k
		               { 80, css::time::milliseconds },
		               {}});
		voice->write({ ipa::semi_high | ipa::front | ipa::vowel, ipa::unspecified, // I
		               { 80, css::time::milliseconds },
		               {}});
		voice->write({ ipa::alveolar | ipa::plosive, ipa::unspecified, // t
		               { 80, css::time::milliseconds },
		               {}});

		voice->read(out.get());

		out->close();
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}