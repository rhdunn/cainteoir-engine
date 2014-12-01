/* Espeak Text-to-Speech Engine.
 *
 * Copyright (C) 2010-2014 Reece H. Dunn
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

#include <cainteoir/engines.hpp>
#include <cainteoir/stopwatch.hpp>
#include "tts_engine.hpp"
#include <stdexcept>

static const int CHARACTERS_PER_WORD = 6;

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;
namespace tts = cainteoir::tts;
namespace css = cainteoir::css;

static const decltype(tts::create_espeak_engine) *create_engines[] =
{
	tts::create_espeak_engine,
	tts::create_pico_engine,
};

static inline double percentageof(size_t a, size_t b)
{
	return (double(a) / b) * 100.0;
}

struct speech_impl : public tts::speech , public tts::synthesis_callback
{
	tts::engine *engine;
	std::shared_ptr<cainteoir::audio> audio;
	tts::media_overlays_mode mMediaOverlays;

	cainteoir::document::const_iterator mFrom;
	cainteoir::document::const_iterator mTo;

	std::vector<cainteoir::ref_entry>::const_iterator mRefEntryFrom;
	std::vector<cainteoir::ref_entry>::const_iterator mRefEntryTo;
	const cainteoir::ref_entry *mRefEntry;

	tts::state_t speechState;
	pthread_t threadId;
	std::string mErrorMessage;

	cainteoir::stopwatch mTimer; /* The time taken to read the document. */
	double mElapsedTime; /* The amount of time elapsed since |mStartTime|. */
	double mTotalTime; /* The (estimated) total amount of time to read the document. */

	double mProgress;  /* The percentage of the document read. */

	size_t currentOffset; /* The current offset from the beginning to the current block being read. */
	size_t speakingPos;   /* The position within the block where the speaking is upto. */
	size_t speakingLen;   /* The length of the word/fragment being spoken. */
	size_t textLen;       /* The length of the text range being read. */
	int wordsPerMinute;   /* The speech rate of the current voice. */

	speech_impl(tts::engine *aEngine,
	            std::shared_ptr<cainteoir::audio> aAudio,
	            const std::vector<cainteoir::ref_entry> &aListing,
	            const cainteoir::document::range_type &aRange,
	            std::shared_ptr<tts::parameter> aRate,
	            tts::media_overlays_mode aMediaOverlays);
	~speech_impl();

	cainteoir::document::const_iterator begin() const { return mFrom; }
	cainteoir::document::const_iterator end()   const { return mTo; }

	void started();
	void progress(size_t n);
	void finished();

	// tts::speech 

	bool is_speaking() const;

	void stop();
	void wait();

	double elapsedTime() const;
	double totalTime() const;

	double completed() const;

	size_t position() const;

	std::string error_message() const;

	const cainteoir::ref_entry &context() const;

	// tts::synthesizer_callback

	tts::state_t state() const;

	void onaudiodata(short *data, int nsamples);

	void ontextrange(size_t pos, size_t len);
};

static void * speak_tts_thread(void *data)
{
	speech_impl *speak = (speech_impl *)data;
	auto mode = speak->mMediaOverlays;

	try
	{
		speak->started();

		size_t n = 0;
		int depth = 0;
		int media_overlay_depth = -1;
		for (auto &node : *speak)
		{
			if (node.type & cainteoir::events::begin_context)
			{
				++depth;
			}

			if (node.type & cainteoir::events::end_context)
			{
				if (depth == media_overlay_depth)
					media_overlay_depth = -1;
				--depth;
			}

			if (node.type & cainteoir::events::media_ref)
			{
				if (mode == tts::media_overlays_mode::media_overlays_only ||
				    mode == tts::media_overlays_mode::tts_and_media_overlays)
				{
					auto audio = cainteoir::create_media_reader(node.content);
					if (audio)
					{
						audio->set_interval(node.media_begin, node.media_end);
						audio->set_target(speak->audio);
						while (audio->read())
							speak->audio->write((const char *)audio->data.begin(), audio->data.size());
						media_overlay_depth = depth;
					}
				}
			}

			if (node.type & cainteoir::events::text)
			{
				switch (mode)
				{
				case tts::media_overlays_mode::tts_only:
					speak->engine->speak(node.content.get(), 0, speak);
					break;
				case tts::media_overlays_mode::tts_and_media_overlays:
					if (media_overlay_depth == -1)
						speak->engine->speak(node.content.get(), 0, speak);
					break;
				}
				n += node.content->size();
				speak->progress(n);
			}

			if (node.type & cainteoir::events::anchor && speak->mRefEntryFrom != speak->mRefEntryTo)
			{
				const auto &ref = *speak->mRefEntryFrom;
				if (ref.location == node.anchor)
				{
					speak->mRefEntry = &ref;
					++speak->mRefEntryFrom;
				}
			}

			if (speak->state() == tts::stopped)
				break;
		}
	}
	catch (const std::exception &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
		speak->mErrorMessage = e.what();
	}

	speak->audio->close();
	speak->finished();
	return nullptr;
}

speech_impl::speech_impl(tts::engine *aEngine,
                         std::shared_ptr<cainteoir::audio> aAudio,
                         const std::vector<cainteoir::ref_entry> &aListing,
                         const cainteoir::document::range_type &aRange,
                         std::shared_ptr<tts::parameter> aRate,
                         tts::media_overlays_mode aMediaOverlays)
	: engine(aEngine)
	, audio(aAudio)
	, speechState(cainteoir::tts::speaking)
	, speakingPos(0)
	, speakingLen(0)
	, textLen(0)
	, wordsPerMinute(aRate ? aRate->value() : 170)
	, mFrom(aRange.begin())
	, mTo(aRange.end())
	, mRefEntryFrom(aListing.begin())
	, mRefEntryTo(aListing.end())
	, mRefEntry(nullptr)
	, mMediaOverlays(aMediaOverlays)
{
	for (auto &node : *this)
	{
		if (node.type & cainteoir::events::text)
			textLen += node.content->size();
	}

	if (mRefEntryFrom != mRefEntryTo)
	{
		// The first TOC entry points to the root document, so skip it ...
		mRefEntry = &*mRefEntryFrom;
		++mRefEntryFrom;
	}

	started();
	int ret = pthread_create(&threadId, nullptr, speak_tts_thread, (void *)this);
}

speech_impl::~speech_impl()
{
}

void speech_impl::started()
{
	audio->open();

	mElapsedTime = 0.0;
	mTotalTime = (double(textLen) / CHARACTERS_PER_WORD / wordsPerMinute * 60.0);
	mProgress = 0.0;
	currentOffset = 0;
}

void speech_impl::progress(size_t n)
{
	currentOffset = n;
	ontextrange(0, 0);
}

void speech_impl::finished()
{
	speechState = cainteoir::tts::stopped;
}

bool speech_impl::is_speaking() const
{
	return state() == cainteoir::tts::speaking;
}

void speech_impl::stop()
{
	finished();
	pthread_join(threadId, nullptr);
}

void speech_impl::wait()
{
	pthread_join(threadId, nullptr);
	finished();
}

double speech_impl::elapsedTime() const
{
	if (is_speaking())
		const_cast<speech_impl *>(this)->mElapsedTime = mTimer.elapsed();

	return mElapsedTime;
}

double speech_impl::totalTime() const
{
	return mTotalTime;
}

double speech_impl::completed() const
{
	return mProgress;
}

size_t speech_impl::position() const
{
	return currentOffset + speakingPos;
}

std::string speech_impl::error_message() const
{
	return mErrorMessage;
}

const cainteoir::ref_entry &speech_impl::context() const
{
	return *mRefEntry;
}

tts::state_t speech_impl::state() const
{
	return speechState;
}

void speech_impl::onaudiodata(short *data, int nsamples)
{
	audio->write((const char *)data, nsamples*2);
}

void speech_impl::ontextrange(size_t pos, size_t len)
{
	speakingPos = pos;
	speakingLen = len;

	size_t actualPos = currentOffset + speakingPos;

	mElapsedTime = mTimer.elapsed();
	mProgress = percentageof(actualPos, textLen);

	if (mElapsedTime > 0.1 && mProgress > 0.1)
	{
		mTotalTime = (mElapsedTime / mProgress) * 100.0;
	}
}

tts::engines::engines(rdf::graph &metadata)
	: selectedVoice(nullptr)
	, active(nullptr)
{
	std::string uri;
	std::string default_voice;
	for (const auto &create_engine : create_engines)
	{
		auto engine = create_engine(metadata, uri, default_voice);
		if (engine)
		{
			enginelist[uri] = engine;
			if (!active)
			{
				if (!select_voice(metadata, rdf::uri(uri, default_voice)))
					throw std::runtime_error(i18n("default voice not found."));
			}
		}
	}

	if (!active)
		throw std::runtime_error(i18n("no text-to-speech voices found."));
}

tts::engines::~engines()
{
	for (auto &engine : enginelist)
		delete engine.second;
}

bool tts::engines::select_voice(const rdf::graph &aMetadata, const rdf::uri &aVoice)
{
	engine *engine = nullptr;
	std::string voice;
	std::string phonemeset = "ipa";
	const rdf::uri * voiceUri = nullptr;

	for (auto &statement : rql::select(aMetadata, rql::subject == aVoice))
	{
		if (rql::predicate(statement) == rdf::tts("name"))
		{
			voice = rql::value(statement);
			voiceUri = &rql::subject(statement);
		}
		else if (rql::predicate(statement) == rdf::tts("voiceOf"))
		{
			const rdf::uri &uri = rql::object(statement);
			if (!uri.empty())
				engine = enginelist[ uri.str() ];
		}
		else if (rql::predicate(statement) == rdf::tts("phonemeset"))
			phonemeset = rql::value(statement);
	}

	if (engine && !voice.empty() && engine->select_voice(voice.c_str(), phonemeset))
	{
		active = engine;
		selectedVoice = voiceUri;
		return true;
	}

	return false;
}

std::shared_ptr<tts::speech>
tts::engines::speak(std::shared_ptr<audio> out,
                    const std::vector<cainteoir::ref_entry> &aListing,
                    const cainteoir::document::range_type &aRange,
                    media_overlays_mode aMediaOverlays)
{
	return std::make_shared<speech_impl>(active, out, aListing, aRange, parameter(tts::parameter::rate), aMediaOverlays);
}

std::shared_ptr<tts::phoneme_reader>
tts::engines::pronunciation()
{
	return active->pronunciation();
}

std::shared_ptr<tts::parameter>
tts::engines::parameter(tts::parameter::type aType)
{
	if (!active)
		return std::shared_ptr<tts::parameter>();
	return active->parameter(aType);
}

const rdf::uri *tts::get_voice_uri(const rdf::graph &aMetadata,
                                   const rdf::uri &predicate,
                                   const std::string &value)
{
	for (auto &voice : rql::select(aMetadata,
	                               rql::predicate == rdf::rdf("type") && rql::object == rdf::tts("Voice")))
	{
		const rdf::uri &uri = rql::subject(voice);
		for (auto &statement : rql::select(aMetadata, rql::subject == uri))
		{
			if (rql::predicate(statement) == predicate && rql::value(statement) == value)
				return &uri;
		}
	}

	return nullptr;
}
