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

static int espeak_tts_callback(short *wav, int numsamples, espeak_EVENT *events)
{
	cainteoir::audio *out = (cainteoir::audio *)events->user_data;

	if (!wav)
	{
		out->close();
		return 0;
	}

	if (numsamples > 0)
		out->write((const char *)wav, numsamples*2);

	return 0;
}

class espeak_engine : public cainteoir::tts_engine
{
	int m_frequency;
public:
	espeak_engine()
	{
		m_frequency = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0);
		espeak_SetSynthCallback(espeak_tts_callback);
	}

	~espeak_engine()
	{
		espeak_Terminate();
	}

	/** @name cainteoir::tts_engine */
	//@{

	int get_channels() const
	{
		return 1;
	}

	int get_frequency() const
	{
		return m_frequency;
	}

	cainteoir::audio_format get_audioformat() const
	{
		return cainteoir::S16_LE;
	}

	bool set_voice_by_name(const char *name)
	{
		if (!name) name = "default";
		return espeak_SetVoiceByName(name) == EE_OK;
	}

	void speak(cainteoir::buffer *text, cainteoir::audio *out)
	{
		out->open();
		espeak_Synth(text->begin(), text->size(), 0, POS_CHARACTER, 0, espeakCHARS_UTF8|espeakENDPAUSE, NULL, out);
		espeak_Synchronize();
	}

	//@}
};

std::auto_ptr<cainteoir::tts_engine> cainteoir::create_espeak_engine()
{
	return std::auto_ptr<cainteoir::tts_engine>(new espeak_engine());
}

