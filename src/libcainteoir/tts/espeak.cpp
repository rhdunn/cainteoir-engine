/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010-2012 Reece H. Dunn
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
#include <cainteoir/platform.hpp>
#include "tts_engine.hpp"

#if defined(HAVE_ESPEAK_SPEAK_LIB_H)
#include <espeak/speak_lib.h>

#ifndef espeakINITIALIZE_DONT_EXIT
#define espeakINITIALIZE_DONT_EXIT 0x8000
#endif

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;

struct mbrola_voice
{
	const char *name;
	const char *language;
	int frequency;
	const char *gender;
};

const std::initializer_list<const mbrola_voice> mbrola_voices = {
	{ "mb-af1",     "af",    16000, "male" },
	{ "mb-af1-en",  "en",    16000, "male" },
	{ "mb-br1",     "pt-br", 16000, "male" },
	{ "mb-br3",     "pt-br", 22050, "male" },
	{ "mb-br4",     "pt-br", 22050, "female" },
	{ "mb-cr1",     "hr",    16000, "male" },
	{ "mb-cz1",     "cs",    16000, "male" },
	{ "mb-de4",     "de",    16000, "male" },
	{ "mb-de4-en",  "en",    16000, "male" },
	{ "mb-de5",     "de",    22050, "female" },
	{ "mb-de5-en",  "en",    22050, "female" },
	{ "mb-de6",     "de",    22050, "male" },
	{ "mb-de6-grc", "el",    22050, "male" },
	{ "mb-de7",     "de",    22050, "female" },
	{ "mb-en1",     "en-uk", 16000, "male" },
	{ "mb-es1",     "es",    16000, "male" },
	{ "mb-es2",     "es",    22050, "male" },
	{ "mb-fr1",     "fr",    16000, "male" },
	{ "mb-fr1-en",  "en",    16000, "male" },
	{ "mb-fr4",     "fr",    16000, "female" },
	{ "mb-fr4-en",  "en",    16000, "female" },
	{ "mb-gr2",     "el",    22050, "male" },
	{ "mb-gr2-en",  "en",    22050, "male" },
	{ "mb-hu1",     "hu",    16000, "female" },
	{ "mb-hu1-en",  "en",    16000, "female" },
	{ "mb-id1",     "id",    16000, "male" },
	{ "mb-it3",     "it",    16000, "male" },
	{ "mb-it4",     "it",    16000, "female" },
	{ "mb-la1",     "la",    16000, "male" },
	{ "mb-nl2",     "nl",    16000, "male" },
	{ "mb-nl2-en",  "en",    16000, "male" },
	{ "mb-pl1",     "pl",    16000, "female" },
	{ "mb-pl1-en",  "en",    16000, "female" },
	{ "mb-pt1",     "pt-pt", 22050, "female" },
	{ "mb-ro1",     "ro",    16000, "male" },
	{ "mb-ro1-en",  "en",    16000, "male" },
	{ "mb-sw1",     "sv",    16000, "male" },
	{ "mb-sw1-en",  "en",    16000, "male" },
	{ "mb-sw2",     "sv",    16000, "female" },
	{ "mb-sw2-en",  "en",    16000, "female" },
	{ "mb-us1",     "en-us", 16000, "female" },
	{ "mb-us2",     "en-us", 16000, "male" },
	{ "mb-us3",     "en-us", 16000, "male" },
};

static int espeak_tts_callback(short *wav, int numsamples, espeak_EVENT *event)
{
	tts::engine_callback *callback = (tts::engine_callback *)event->user_data;
	if (!callback) return 0;

	if (callback->state() == tts::stopped) return 1;

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

class espeak_parameter : public tts::parameter
{
public:
	espeak_parameter(const char *aName, const char *aUnits, espeak_PARAMETER aParameter, int aMinimum, int aMaximum, int aScale)
		: mName(aName)
		, mUnits(aUnits)
		, mParameter(aParameter)
		, mMinimum(aMinimum)
		, mMaximum(aMaximum)
		, mScale(aScale)
	{
	}

	const char *name() const
	{
		return mName;
	}

	const char *units() const
	{
		return mUnits;
	}

	int minimum() const
	{
		return mMinimum;
	}

	int maximum() const
	{
		return mMaximum;
	}

	int default_value() const
	{
		return espeak_GetParameter(mParameter, 0) * mScale;
	}

	int value() const
	{
		return espeak_GetParameter(mParameter, 1) * mScale;
	}

	bool set_value(int value)
	{
		return espeak_SetParameter(mParameter, value / mScale, 0);
	}
private:
	const char *mName;
	const char *mUnits;
	espeak_PARAMETER mParameter;
	int mMinimum;
	int mMaximum;
	int mScale;
};

class espeak_engine : public tts::engine
{
public:
	espeak_engine(rdf::graph &metadata, std::string &baseuri)
		: mRate(std::make_shared<espeak_parameter>(_("Speed"), "wpm", espeakRATE, 80, 450, 1))
		, mVolume(std::make_shared<espeak_parameter>(_("Volume"), "%", espeakVOLUME, 0, 200, 1))
		, mPitch(std::make_shared<espeak_parameter>(_("Base Pitch"), "", espeakPITCH, 0, 100, 1))
		, mPitchRange(std::make_shared<espeak_parameter>(_("Pitch Variation"), "", espeakRANGE, 0, 100, 1))
		, mWordGap(std::make_shared<espeak_parameter>(_("Word Gap"), "ms", espeakWORDGAP, 0, 500, 10))
	{
		baseuri = "http://rhdunn.github.com/cainteoir/engines/espeak";

		int frequency = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, nullptr, espeakINITIALIZE_DONT_EXIT);
		espeak_SetSynthCallback(espeak_tts_callback);

		rdf::uri    espeak = rdf::uri(baseuri, std::string());
		rdf::uri    jonsd  = metadata.genid();
		std::string info   = espeak_Info(nullptr);

		metadata.statement(jonsd, rdf::rdf("type"), rdf::foaf("Person"));
		metadata.statement(jonsd, rdf::foaf("name"), rdf::literal("Jonathan Duddington"));
		metadata.statement(jonsd, rdf::foaf("title"), rdf::literal("Mr."));
		metadata.statement(jonsd, rdf::foaf("givenName"), rdf::literal("Jonathan"));
		metadata.statement(jonsd, rdf::foaf("familyName"), rdf::literal("Duddington"));
		metadata.statement(jonsd, rdf::foaf("gender"), rdf::literal("male"));
		metadata.statement(jonsd, rdf::foaf("isPrimaryTopicOf"), rdf::uri("http://sourceforge.net/users/jonsd", std::string()));

		metadata.statement(espeak, rdf::rdf("type"), rdf::tts("Engine"));
		metadata.statement(espeak, rdf::tts("name"), rdf::literal("eSpeak"));
		metadata.statement(espeak, rdf::tts("version"), rdf::literal(info.substr(0, info.find(' '))));
		metadata.statement(espeak, rdf::dc("source"), rdf::uri("http://espeak.sourceforge.net/", std::string()));
		metadata.statement(espeak, rdf::dc("creator"), jonsd);

		for (const espeak_VOICE **data = espeak_ListVoices(nullptr); *data; ++data)
		{
			std::string id = (*data)->name;
			if (id == "french (Belgium)")
				id = "french-belgium";

			rdf::uri voice = rdf::uri(baseuri, id);
			metadata.statement(voice, rdf::rdf("type"), rdf::tts("Voice"));
			metadata.statement(voice, rdf::dc("language"), rdf::literal((*data)->languages+1));
			metadata.statement(voice, rdf::tts("name"), rdf::literal((*data)->name));
			metadata.statement(voice, rdf::tts("gender"), rdf::tts((*data)->gender == 2 ? "female" : "male"));
			if ((*data)->age)
				metadata.statement(voice, rdf::tts("age"), rdf::literal((*data)->age, rdf::xsd("int")));

			metadata.statement(voice, rdf::tts("frequency"), rdf::literal(frequency, rdf::tts("hertz")));
			metadata.statement(voice, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int")));
			metadata.statement(voice, rdf::tts("audio-format"),  rdf::tts("s16le"));

			metadata.statement(voice, rdf::tts("voiceOf"), espeak);
			metadata.statement(espeak, rdf::tts("hasVoice"), voice);
		}

		for (auto mbrola = mbrola_voices.begin(), last = mbrola_voices.end(); mbrola != last; ++mbrola)
		{
			rdf::uri voice = rdf::uri(baseuri, mbrola->name);
			metadata.statement(voice, rdf::rdf("type"), rdf::tts("Voice"));
			metadata.statement(voice, rdf::dc("language"), rdf::literal(mbrola->language));
			metadata.statement(voice, rdf::tts("name"), rdf::literal(mbrola->name));
			metadata.statement(voice, rdf::tts("gender"), rdf::tts(mbrola->gender));

			metadata.statement(voice, rdf::tts("frequency"), rdf::literal(mbrola->frequency, rdf::tts("hertz")));
			metadata.statement(voice, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int")));
			metadata.statement(voice, rdf::tts("audio-format"),  rdf::tts("s16le"));

			metadata.statement(voice, rdf::tts("voiceOf"), espeak);
		}
	}

	~espeak_engine()
	{
		espeak_Terminate();
	}

	/** @name cainteoir::tts_engine */
	//@{

	bool select_voice(const char *voicename)
	{
		return espeak_SetVoiceByName(voicename) == EE_OK;
	}

	void speak(cainteoir::buffer *text, size_t offset, tts::engine_callback *callback)
	{
		std::string txt = text->str();
		espeak_Synth(txt.c_str() + offset, txt.size() - offset, 0, POS_CHARACTER, 0, espeakCHARS_UTF8|espeakENDPAUSE, nullptr, callback);
		espeak_Synchronize();
	}

	std::shared_ptr<tts::parameter>
	parameter(tts::parameter::type aType)
	{
		switch (aType)
		{
		case tts::parameter::rate:        return mRate;
		case tts::parameter::volume:      return mVolume;
		case tts::parameter::pitch:       return mPitch;
		case tts::parameter::pitch_range: return mPitchRange;
		case tts::parameter::word_gap:    return mWordGap;
		default:                          return std::shared_ptr<tts::parameter>();
		}
	}

	//@}
private:
	std::shared_ptr<tts::parameter> mRate;
	std::shared_ptr<tts::parameter> mVolume;
	std::shared_ptr<tts::parameter> mPitch;
	std::shared_ptr<tts::parameter> mPitchRange;
	std::shared_ptr<tts::parameter> mWordGap;
};

tts::engine *tts::create_espeak_engine(rdf::graph &aMetadata, std::string &uri)
{
	return new espeak_engine(aMetadata, uri);
}

#else

tts::engine *tts::create_espeak_engine(rdf::graph &aMetadata, std::string &uri)
{
	return nullptr;
}

#endif
