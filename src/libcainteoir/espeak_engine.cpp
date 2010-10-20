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
	char m_frequency[20];
public:
	espeak_engine()
	{
		int frequency = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0);
		espeak_SetSynthCallback(espeak_tts_callback);

		snprintf(m_frequency, 20, "%dHz", frequency);
	}

	~espeak_engine()
	{
		espeak_Terminate();
	}

	/** @name cainteoir::metadata */
	//@{

	const char *get_metadata(cainteoir::metadata_tag tag) const
	{
		switch (tag)
		{
		case cainteoir::title:        return "espeak";
		case cainteoir::channels:     return "mono";
		case cainteoir::frequency:    return m_frequency;
		case cainteoir::audio_format: return "S16_LE";
		default:                      return NULL;
		}
	}

	//@}
	/** @name cainteoir::tts_engine */
	//@{

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

