/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010-2013 Reece H. Dunn
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

#include "tts_engine.hpp"

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;

#if defined(HAVE_ESPEAK_SPEAK_LIB_H)

#include <cainteoir/path.hpp>

#include <espeak/speak_lib.h>
#include <unistd.h>
#include <sstream>

#ifndef espeakINITIALIZE_DONT_EXIT
#define espeakINITIALIZE_DONT_EXIT 0x8000
#endif

static const std::map<std::string, std::string> voice_corrections = {
	{ "en-gb",       "en-GB" },
	{ "en-sc",       "en-GB-scotland" },
	{ "en-uk",       "en-GB" },
	{ "en-uk-north", "en-GB-x-lancastr" },
	{ "en-uk-rp",    "en-GB-x-rp" },
	{ "en-uk-wmids", "en-GB-x-westmids" },
	{ "en-us",       "en-US" },
	{ "en-wi",       "en-029" },
	{ "es-la",       "es-419" },
	{ "fa-pin",      "fa-Latn" },
	{ "hy-west",     "hy-arevmda" },
	{ "vi-hue",      "vi-VN-x-central" },
	{ "vi-sgn",      "vi-VN-x-south" },
};

std::string correct_lang(std::string lang)
{
	auto match = voice_corrections.find(lang);
	if (match != voice_corrections.end())
		return match->second;
	return lang;
}

std::string phonemeset_from_language(const std::string &lang)
{
	auto path = cainteoir::get_data_path() / "phonemeset" / "espeak" / (lang + ".phon");
	if (access(path, R_OK) == 0)
		return "espeak/" + lang;
	return "ipa";
}

#if defined(HAVE_MBROLA)
struct mbrola_voice
{
	const char *name;
	const char *voice;
	const char *language;
	int frequency;
	const char *gender;
};

/* This table describes the MBROLA voices supported by eSpeak (espeak-data/voices/mb/...).
 *
 * This is here for several reasons:
 *    1.  espeak_ListVoices does not list the MBROLA voices;
 *    2.  the MBROLA voices can output at a different frequency to eSpeak;
 *    3.  the MBROLA voice data may not be installed on the system, so they
 *        need to be checked to see if they exist.
 */
static const std::initializer_list<const mbrola_voice> mbrola_voices = {
	{ "mb-af1",     "af1/af1", "af",    16000, "male" },
	{ "mb-af1-en",  "af1/af1", "en",    16000, "male" },
	{ "mb-br1",     "br1/br1", "pt-br", 16000, "male" },
	{ "mb-br3",     "br3/br3", "pt-br", 22050, "male" },
	{ "mb-br4",     "br4/br4", "pt-br", 16000, "female" },
	{ "mb-cr1",     "cr1/cr1", "hr",    16000, "male" },
	{ "mb-cz2",     "cz2/cz2", "cs",    16000, "male" },
	{ "mb-de2",     "de2/de2", "de",    16000, "male" },
	{ "mb-de4",     "de4/de4", "de",    16000, "male" },
	{ "mb-de4-en",  "de4/de4", "en",    16000, "male" },
	{ "mb-de5",     "de5/de5", "de",    22050, "female" },
	{ "mb-de5-en",  "de5/de5", "en",    22050, "female" },
	{ "mb-de6",     "de6/de6", "de",    22050, "male" },
	{ "mb-de6-grc", "de6/de6", "el",    22050, "male" },
	{ "mb-de7",     "de7/de7", "de",    22050, "female" },
	{ "mb-ee1",     "ee1/ee1", "et",    16000, "male" },
	{ "mb-en1",     "en1/en1", "en-gb", 16000, "male" },
	{ "mb-es1",     "es1/es1", "es",    16000, "male" },
	{ "mb-es2",     "es2/es2", "es",    22050, "male" },
	{ "mb-fr1",     "fr1/fr1", "fr",    16000, "male" },
	{ "mb-fr1-en",  "fr1/fr1", "en",    16000, "male" },
	{ "mb-fr4",     "fr4/fr4", "fr",    16000, "female" },
	{ "mb-fr4-en",  "fr4/fr4", "en",    16000, "female" },
	{ "mb-gr2",     "gr2/gr2", "el",    22050, "male" },
	{ "mb-gr2-en",  "gr2/gr2", "en",    22050, "male" },
	{ "mb-hu1",     "hu1/hu1", "hu",    16000, "female" },
	{ "mb-hu1-en",  "hu1/hu1", "en",    16000, "female" },
	{ "mb-ic1",     "ic1/ic1", "is",    16000, "male" },
	{ "mb-id1",     "id1/id1", "id",    16000, "male" },
	{ "mb-it3",     "it3/it3", "it",    16000, "male" },
	{ "mb-it4",     "it4/it4", "it",    16000, "female" },
	{ "mb-la1",     "la1/la1", "la",    16000, "male" },
	{ "mb-mx1",     "mx1/mx1", "es-mx", 16000, "male" },
	{ "mb-mx2",     "mx2/mx2", "es-mx", 16000, "male" },
	{ "mb-nl2",     "nl2/nl2", "nl",    16000, "male" },
	{ "mb-nl2-en",  "nl2/nl2", "en",    16000, "male" },
	{ "mb-pl1",     "pl1/pl1", "pl",    16000, "female" },
	{ "mb-pl1-en",  "pl1/pl1", "en",    16000, "female" },
	{ "mb-pt1",     "pt1/pt1", "pt-pt", 22050, "female" },
	{ "mb-ro1",     "ro1/ro1", "ro",    16000, "male" },
	{ "mb-ro1-en",  "ro1/ro1", "en",    16000, "male" },
	{ "mb-sw1",     "sw1/sw1", "sv",    16000, "male" },
	{ "mb-sw1-en",  "sw1/sw1", "en",    16000, "male" },
	{ "mb-sw2",     "sw2/sw2", "sv",    16000, "female" },
	{ "mb-sw2-en",  "sw2/sw2", "en",    16000, "female" },
	{ "mb-tr1",     "tr1/tr1", "tr",    16000, "male" },
	{ "mb-tr2",     "tr2/tr2", "tr",    22050, "female" },
	{ "mb-us1",     "us1/us1", "en-us", 16000, "female" },
	{ "mb-us2",     "us2/us2", "en-us", 16000, "male" },
	{ "mb-us3",     "us3/us3", "en-us", 16000, "male" },
	{ "mb-vz1",     "vz1/vz1", "es-ve", 16000, "male" },
};

static bool is_mbrola_voice_available(const char *voice)
{
	std::string path = std::string(MBROLA_DIR "/") + voice;
	return access(path.c_str(), R_OK) == 0;
}
#endif

static int espeak_tts_callback(short *wav, int numsamples, espeak_EVENT *event)
{
	tts::engine_callback *callback = (tts::engine_callback *)event->user_data;
	if (!callback) return 0;

	if (callback->state() == tts::stopped) return 1;

	for (; event->type != espeakEVENT_LIST_TERMINATED; ++event) switch (event->type)
	{
	case espeakEVENT_WORD:
		callback->onspeaking(event->text_position, event->length);
		break;
	case espeakEVENT_END:
		callback->onspeaking(event->text_position, 0);
		break;
	default:
		break;
	}

	if (numsamples > 0)
		callback->onaudiodata(wav, numsamples);

	return 0;
}

class espeak_pronunciation : public tts::phoneme_reader
{
public:
	espeak_pronunciation(const char *aPhonemeSet);

	void reset(const std::shared_ptr<cainteoir::buffer> &aBuffer);

	bool read();
private:
	std::shared_ptr<tts::phoneme_reader> mReader;
	bool mIPA;
};

espeak_pronunciation::espeak_pronunciation(const char *aPhonemeSet)
	: mReader(tts::createPhonemeReader(aPhonemeSet))
	, mIPA(!strcmp(aPhonemeSet, "ipa"))
{
}

void espeak_pronunciation::reset(const std::shared_ptr<cainteoir::buffer> &aBuffer)
{
#if defined(HAVE_ESPEAK_TEXTTOPHONEMES)
	static const int TIE_BARS        = 0x0001;
	static const int ESPEAK_PHONEMES = 0x0000;
	static const int IPA_PHONEMES    = 0x0010;
	static const int PHONEME_MODE    = mIPA ? (IPA_PHONEMES | TIE_BARS) : ESPEAK_PHONEMES;

	cainteoir::rope ret;
	std::string txt = aBuffer->str(); // null-terminate the text buffer
	const void *data = txt.c_str();
	while (data != nullptr)
	{
		const char *buffer = espeak_TextToPhonemes(&data, espeakCHARS_UTF8, PHONEME_MODE);
		// NOTE: phoneme output starts with a space, so remove that ...
		int len = strlen(buffer);
		if (len > 1)
			ret += cainteoir::make_buffer(buffer+1, len-1);
	}
	mReader->reset(ret.buffer());
#else
	static const int IPA_PHONEMES    = 4; // IPA + \u0361 ties
	static const int ESPEAK_PHONEMES = 1; // eSpeak

	std::string txt = aBuffer->str(); // null-terminate the text buffer

	cainteoir::memory_file f;
	espeak_SetPhonemeTrace(mIPA ? IPA_PHONEMES : ESPEAK_PHONEMES, f);
	espeak_Synth(txt.c_str(), txt.size(), 0, POS_CHARACTER, 0, espeakCHARS_UTF8|espeakENDPAUSE, nullptr, nullptr);
	espeak_Synchronize();
	espeak_SetPhonemeTrace(0, stdout);
	mReader->reset(cainteoir::normalize(f.buffer()));
#endif
}

bool espeak_pronunciation::read()
{
	bool ret = mReader->read();
	*(ipa::phoneme *)this = *mReader;
	return ret;
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
	espeak_engine(rdf::graph &metadata, std::string &baseuri, std::string &default_voice);
	~espeak_engine();

	/** @name cainteoir::tts_engine */
	//@{

	bool select_voice(const char *voicename, const std::string &phonemeset);

	void speak(cainteoir::buffer *text, size_t offset, tts::engine_callback *callback);

	std::shared_ptr<tts::phoneme_reader> pronunciation();

	std::shared_ptr<tts::parameter>
	parameter(tts::parameter::type aType);

	//@}
private:
	std::shared_ptr<tts::parameter> mRate;
	std::shared_ptr<tts::parameter> mVolume;
	std::shared_ptr<tts::parameter> mPitch;
	std::shared_ptr<tts::parameter> mPitchRange;
	std::shared_ptr<tts::parameter> mWordGap;
	std::string mPhonemeSet;
};

espeak_engine::espeak_engine(rdf::graph &metadata, std::string &baseuri, std::string &default_voice)
	: mRate(std::make_shared<espeak_parameter>(i18n("Speed"), "wpm", espeakRATE, 80, 450, 1))
	, mVolume(std::make_shared<espeak_parameter>(i18n("Volume"), "%", espeakVOLUME, 0, 200, 1))
	, mPitch(std::make_shared<espeak_parameter>(i18n("Base Pitch"), "", espeakPITCH, 0, 100, 1))
	, mPitchRange(std::make_shared<espeak_parameter>(i18n("Pitch Variation"), "", espeakRANGE, 0, 100, 1))
	, mWordGap(std::make_shared<espeak_parameter>(i18n("Word Gap"), "ms", espeakWORDGAP, 0, 500, 10))
	, mPhonemeSet("ipa")
{
	baseuri = "http://rhdunn.github.com/cainteoir/engines/espeak";
	default_voice = "default";

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
		std::string lang = correct_lang((*data)->languages+1);
		std::string id = (*data)->name;
		if (id == "french (Belgium)")
			id = "french-belgium";

		rdf::uri voice = rdf::uri(baseuri, id);
		metadata.statement(voice, rdf::rdf("type"), rdf::tts("Voice"));
		metadata.statement(voice, rdf::dc("language"), rdf::literal(lang));
		metadata.statement(voice, rdf::tts("name"), rdf::literal((*data)->name));
		metadata.statement(voice, rdf::tts("gender"), rdf::tts((*data)->gender == 2 ? "female" : "male"));
		metadata.statement(voice, rdf::tts("phonemeset"), rdf::literal(phonemeset_from_language(lang)));
		if ((*data)->age)
			metadata.statement(voice, rdf::tts("age"), rdf::literal((*data)->age, rdf::xsd("int")));

		metadata.statement(voice, rdf::tts("frequency"), rdf::literal(frequency, rdf::tts("hertz")));
		metadata.statement(voice, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int")));
		metadata.statement(voice, rdf::tts("audio-format"),  rdf::tts("s16le"));

		metadata.statement(voice, rdf::tts("voiceOf"), espeak);
		metadata.statement(espeak, rdf::tts("hasVoice"), voice);
	}

#if defined(HAVE_MBROLA)
	for (auto &mbrola : mbrola_voices)
	{
		if (is_mbrola_voice_available(mbrola.voice))
		{
			rdf::uri voice = rdf::uri(baseuri, mbrola.name);
			metadata.statement(voice, rdf::rdf("type"), rdf::tts("Voice"));
			metadata.statement(voice, rdf::dc("language"), rdf::literal(mbrola.language));
			metadata.statement(voice, rdf::tts("name"), rdf::literal(mbrola.name));
			metadata.statement(voice, rdf::tts("gender"), rdf::tts(mbrola.gender));
			metadata.statement(voice, rdf::tts("phonemeset"), rdf::literal(phonemeset_from_language(mbrola.language)));

			metadata.statement(voice, rdf::tts("frequency"), rdf::literal(mbrola.frequency, rdf::tts("hertz")));
			metadata.statement(voice, rdf::tts("channels"),  rdf::literal(1, rdf::xsd("int")));
			metadata.statement(voice, rdf::tts("audio-format"),  rdf::tts("s16le"));

			metadata.statement(voice, rdf::tts("voiceOf"), espeak);
		}
	}
#endif
}

espeak_engine::~espeak_engine()
{
	espeak_Terminate();
}

bool espeak_engine::select_voice(const char *voicename, const std::string &phonemeset)
{
	if (espeak_SetVoiceByName(voicename) == EE_OK)
	{
		mPhonemeSet = phonemeset;
		return true;
	}
	return false;
}

void espeak_engine::speak(cainteoir::buffer *text, size_t offset, tts::engine_callback *callback)
{
	std::string txt = text->str(); // null-terminate the text buffer
	espeak_Synth(txt.c_str() + offset, txt.size() - offset, 0, POS_CHARACTER, 0, espeakCHARS_UTF8|espeakENDPAUSE, nullptr, callback);
	espeak_Synchronize();
}

std::shared_ptr<tts::phoneme_reader> espeak_engine::pronunciation()
{
	return std::make_shared<espeak_pronunciation>(mPhonemeSet.c_str());
}

std::shared_ptr<tts::parameter>
espeak_engine::parameter(tts::parameter::type aType)
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

tts::engine *tts::create_espeak_engine(rdf::graph &aMetadata, std::string &uri, std::string &default_voice)
{
	return new espeak_engine(aMetadata, uri, default_voice);
}

#else

tts::engine *tts::create_espeak_engine(rdf::graph &aMetadata, std::string &uri, std::string &default_voice)
{
	return nullptr;
}

#endif
