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

namespace rdf = cainteoir::rdf;

class espeak_engine : public cainteoir::tts_engine
{
	int m_frequency;
	rdf::model metadata;
public:
	espeak_engine()
	{
		std::string baseuri = "http://rhdunn.github.com/cainteoir/engines/espeak";

		m_frequency = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0);
		espeak_SetSynthCallback(espeak_tts_callback);

		rdf::uri espeak = rdf::uri(baseuri, std::string());
		rdf::bnode jonsd = metadata.genid();

		metadata.push_back(rdf::statement(espeak, rdf::rdf("type"), rdf::tts("Engine")));
		metadata.push_back(rdf::statement(espeak, rdf::tts("name"), rdf::literal("eSpeak")));
		metadata.push_back(rdf::statement(espeak, rdf::tts("frequency"), rdf::literal(m_frequency, rdf::tts("hertz"))));
		metadata.push_back(rdf::statement(espeak, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int"))));
		metadata.push_back(rdf::statement(espeak, rdf::tts("audio-format"),  rdf::tts("s16le")));
		metadata.push_back(rdf::statement(espeak, rdf::dc("creator"), jonsd));

		metadata.push_back(rdf::statement(jonsd, rdf::rdf("type"), rdf::foaf("Person")));
		metadata.push_back(rdf::statement(jonsd, rdf::foaf("name"), rdf::literal("Jonathan Duddington")));
		metadata.push_back(rdf::statement(jonsd, rdf::foaf("title"), rdf::literal("Mr.")));
		metadata.push_back(rdf::statement(jonsd, rdf::foaf("givenName"), rdf::literal("Jonathan")));
		metadata.push_back(rdf::statement(jonsd, rdf::foaf("familyName"), rdf::literal("Duddington")));
		metadata.push_back(rdf::statement(jonsd, rdf::foaf("gender"), rdf::literal("male")));
		metadata.push_back(rdf::statement(jonsd, rdf::foaf("isPrimaryTopicOf"), rdf::uri("http://sourceforge.net/users/jonsd", std::string())));

		for (const espeak_VOICE **data = espeak_ListVoices(NULL); *data; ++data)
		{
			const char *lang = (*data)->languages+1;

			rdf::uri voice = rdf::uri(baseuri, lang);
			metadata.push_back(rdf::statement(voice, rdf::rdf("type"), rdf::tts("Voice")));
			metadata.push_back(rdf::statement(voice, rdf::dc("language"), rdf::literal(lang)));
			metadata.push_back(rdf::statement(voice, rdf::tts("name"), rdf::literal(lang)));
			metadata.push_back(rdf::statement(voice, rdf::tts("gender"), rdf::tts((*data)->gender == 2 ? "female" : "male")));
			if ((*data)->age)
				metadata.push_back(rdf::statement(voice, rdf::tts("age"), rdf::literal((*data)->age, rdf::xsd("int"))));

			metadata.push_back(rdf::statement(espeak, rdf::tts("hasVoice"), voice));
			metadata.push_back(rdf::statement(voice, rdf::tts("voiceOf"), espeak));
		}
	}

	~espeak_engine()
	{
		espeak_Terminate();
	}

	/** @name cainteoir::tts_engine */
	//@{

	const rdf::model &get_metadata() const
	{
		return metadata;
	}

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

