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

struct speech_impl : public tts::speech , public tts::callback
{
	tts::engine *engine;
	cainteoir::audio *audio;
	std::list<cainteoir::event> events;

	pthread_t threadId;

	speech_impl(tts::engine *aEngine, cainteoir::audio *aAudio, const std::list<cainteoir::event> &aEvents);
	~speech_impl();

	/** @name tts::speech */
	//@{

	void wait();

	//@}
	/** @name tts::callback */
	//@{

	void onaudiodata(short *data, int nsamples);

	//@}
};

void * speak_tts_thread(void *data)
{
	speech_impl *speak = (speech_impl *)data;

	speak->audio->open();

	foreach_iter(event, speak->events)
	{
		if (event->type == cainteoir::text_event)
			speak->engine->speak(event->data.get(), speak);
	}

	speak->audio->close();
	return NULL;
}

speech_impl::speech_impl(tts::engine *aEngine, cainteoir::audio *aAudio, const std::list<cainteoir::event> &aEvents)
	: engine(aEngine)
	, audio(aAudio)
	, events(aEvents)
{
	int ret = pthread_create(&threadId, NULL, speak_tts_thread, (void *)this);
}

speech_impl::~speech_impl()
{
}

void speech_impl::wait()
{
	pthread_join(threadId, NULL);
}

void speech_impl::onaudiodata(short *data, int nsamples)
{
	audio->write((const char *)data, nsamples*2);
}

///////////////////////////////////////////////////////////////////////////////

tts::engines::engines(rdf::graph &metadata)
{
	std::string uri;
	active = tts::create_espeak_engine(metadata, uri);
	enginelist[uri] = active;

	if (!active)
		throw std::runtime_error("no text-to-speech voices found.");
}

tts::engines::~engines()
{
	foreach_iter(engine, enginelist)
		delete engine->second;
}

int tts::engines::get_channels() const
{
	return active->get_channels();
}

int tts::engines::get_frequency() const
{
	return active->get_frequency();
}

cainteoir::audio_format tts::engines::get_audioformat() const
{
	return active->get_audioformat();
}

bool tts::engines::select_voice(const rdf::graph &aMetadata, const rdf::uri &aVoice)
{
	engine *engine = NULL;
	std::string voice;

	foreach_iter(statement, rql::select(aMetadata, rql::subject, aVoice))
	{
		if (rql::predicate(*statement) == rdf::tts("name"))
			voice = rql::value(rql::object(*statement));
		else if (rql::predicate(*statement) == rdf::tts("voiceOf"))
		{
			const rdf::uri *uri = rql::object(*statement);
			if (uri)
				engine = enginelist[ uri->str() ];
		}
	}

	if (engine && !voice.empty() && engine->select_voice(voice.c_str()))
	{
		active = engine;
		return true;
	}

	return false;
}

std::auto_ptr<tts::speech> tts::engines::speak(const std::list<event> &events, audio *out)
{
	return std::auto_ptr<tts::speech>(new speech_impl(active, out, events));
}
