/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010 Reece H. Dunn
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

#include <cainteoir/engines.hpp>
#include "tts_engine.hpp"
#include <stdexcept>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

tts::engines::engines(rdf::model &metadata)
{
	espeak = tts::create_espeak_engine(metadata);
	if (!espeak)
		throw std::runtime_error("no text-to-speech voices found.");
}

tts::engines::~engines()
{
	delete espeak;
}

int tts::engines::get_channels() const
{
	return espeak->get_channels();
}

int tts::engines::get_frequency() const
{
	return espeak->get_frequency();
}

cainteoir::audio_format tts::engines::get_audioformat() const
{
	return espeak->get_audioformat();
}

bool tts::engines::select_voice(const rdf::model &aMetadata, const rdf::uri &aVoice)
{
	std::string engine;
	std::string voice;

	foreach_iter(statement, rql::select(aMetadata, rql::subject, aVoice))
	{
		if (rql::predicate(*statement) == rdf::tts("name"))
			voice = rql::value(rql::object(*statement));
		else if (rql::predicate(*statement) == rdf::tts("voiceOf"))
		{
			const rdf::uri *uri = rql::subject(*statement);
			if (uri)
				engine = uri->str();
		}
	}

	if (!voice.empty())
		return espeak->select_voice(voice.c_str());

	return false;
}

void tts::engines::speak(buffer *text, audio *out)
{
	espeak->speak(text, out);
}
