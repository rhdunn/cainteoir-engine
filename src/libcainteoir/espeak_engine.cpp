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

#include <cainteoir/tts_engine.hpp>
#include <espeak/speak_lib.h>

class espeak_engine : public cainteoir::tts_engine
{
public:
	espeak_engine()
	{
		int samplerate = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 0, NULL, 0);
	}

	~espeak_engine()
	{
		espeak_Terminate();
	}

	void speak(cainteoir::buffer *text)
	{
		espeak_Synth(text->begin(), text->size(), 0, POS_CHARACTER, 0, espeakCHARS_UTF8|espeakENDPAUSE, NULL, NULL);
		espeak_Synchronize();
	}
};

std::auto_ptr<cainteoir::tts_engine> cainteoir::create_espeak_engine()
{
	return std::auto_ptr<cainteoir::tts_engine>(new espeak_engine());
}

