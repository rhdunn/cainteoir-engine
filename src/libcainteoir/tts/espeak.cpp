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

#include <config.h>
#include "tts_engine.hpp"

#if defined(HAVE_ESPEAK_SPEAK_LIB_H)
#include <espeak/speak_lib.h>

#ifndef espeakINITIALIZE_DONT_EXIT
#define espeakINITIALIZE_DONT_EXIT 0x8000
#endif

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

static int espeak_tts_callback(short *wav, int numsamples, espeak_EVENT *event)
{
	cainteoir::tts::engine_callback *callback = (cainteoir::tts::engine_callback *)event->user_data;
	if (!callback) return 0;

	if (callback->state() == cainteoir::tts::stopped) return 1;

	for (; event->type != espeakEVENT_LIST_TERMINATED; ++event) switch (event->type)
	{
	case espeakEVENT_WORD:
		callback->onspeaking(event->text_position - 1, event->length);
		break;
	case espeakEVENT_END:
		callback->onspeaking(event->text_position - 1, 0);
		break;
	}

	if (numsamples > 0)
		callback->onaudiodata(wav, numsamples);

	return 0;
}

class espeak_engine : public cainteoir::tts::engine
{
public:
	espeak_engine(rdf::graph &metadata, std::string &baseuri)
	{
		baseuri = "http://rhdunn.github.com/cainteoir/engines/espeak";

		int frequency = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, espeakINITIALIZE_DONT_EXIT);
		espeak_SetSynthCallback(espeak_tts_callback);

		rdf::uri espeak = rdf::uri(baseuri, std::string());
		rdf::bnode jonsd = metadata.genid();
		std::string info = espeak_Info(NULL);

		metadata.push_back(rdf::statement(espeak, rdf::rdf("type"), rdf::tts("Engine")));
		metadata.push_back(rdf::statement(espeak, rdf::tts("name"), rdf::literal("eSpeak")));
		metadata.push_back(rdf::statement(espeak, rdf::tts("version"), rdf::literal(info.substr(0, info.find(' ')))));
		metadata.push_back(rdf::statement(espeak, rdf::dc("source"), rdf::uri("http://espeak.sourceforge.net/", std::string())));
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
			std::string id = (*data)->name;
			if (id == "french (Belgium)")
				id = "french-belgium";

			rdf::uri voice = rdf::uri(baseuri, id);
			metadata.push_back(rdf::statement(voice, rdf::rdf("type"), rdf::tts("Voice")));
			metadata.push_back(rdf::statement(voice, rdf::dc("language"), rdf::literal((*data)->languages+1)));
			metadata.push_back(rdf::statement(voice, rdf::tts("name"), rdf::literal((*data)->name)));
			metadata.push_back(rdf::statement(voice, rdf::tts("gender"), rdf::tts((*data)->gender == 2 ? "female" : "male")));
			if ((*data)->age)
				metadata.push_back(rdf::statement(voice, rdf::tts("age"), rdf::literal((*data)->age, rdf::xsd("int"))));

			metadata.push_back(rdf::statement(voice, rdf::tts("frequency"), rdf::literal(frequency, rdf::tts("hertz"))));
			metadata.push_back(rdf::statement(voice, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int"))));
			metadata.push_back(rdf::statement(voice, rdf::tts("audio-format"),  rdf::tts("s16le")));

			metadata.push_back(rdf::statement(voice, rdf::tts("voiceOf"), espeak));
			metadata.push_back(rdf::statement(espeak, rdf::tts("hasVoice"), voice));
		}
	}

	~espeak_engine()
	{
		espeak_Terminate();
	}

	/** @name cainteoir::tts_engine */
	//@{

	cainteoir::audio_format get_audioformat() const
	{
		return cainteoir::S16_LE;
	}

	bool select_voice(const char *voicename)
	{
		return espeak_SetVoiceByName(voicename) == EE_OK;
	}

	void speak(cainteoir::buffer *text, size_t offset, cainteoir::tts::engine_callback *callback)
	{
		std::string txt = text->str();
		espeak_Synth(txt.c_str() + offset, txt.size() - offset, 0, POS_CHARACTER, 0, espeakCHARS_UTF8|espeakENDPAUSE, NULL, callback);
		espeak_Synchronize();
	}

	//@}
};

cainteoir::tts::engine *cainteoir::tts::create_espeak_engine(rdf::graph &aMetadata, std::string &uri)
{
	return new espeak_engine(aMetadata, uri);
}

#else

cainteoir::tts::engine *cainteoir::tts::create_espeak_engine(rdf::graph &aMetadata, std::string &uri)
{
	return NULL;
}

#endif
