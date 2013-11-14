/* FFmpeg/libav media playback support.
 *
 * Copyright (C) 2013 Reece H. Dunn
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

#define __STDC_FORMAT_MACROS

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include <cainteoir/audio.hpp>

namespace rdf = cainteoir::rdf;
namespace css = cainteoir::css;

#ifdef HAVE_FFMPEG

#include <cstdio>
#include <limits>
#include <endian.h>

extern "C"
{
#include <libavformat/avformat.h>
}

static uint64_t time_to_samples(const css::time &time, uint64_t sample_rate, uint64_t default_value)
{
	switch (time.units())
	{
	case css::time::inherit: return default_value;
	default:                 return time.as(css::time::seconds).value() * sample_rate;
	}
}

struct buffer_stream
{
	buffer_stream(const std::shared_ptr<cainteoir::buffer> &aBuffer)
		: mBuffer(aBuffer)
		, mCurrent(aBuffer->begin())
	{
	}

	int read(uint8_t *buf, int size);

	int64_t seek(int64_t offset, int direction);
private:
	std::shared_ptr<cainteoir::buffer> mBuffer;
	const char *mCurrent;
};

int buffer_stream::read(uint8_t *buf, int size)
{
	int avail = mBuffer->end() - mCurrent;
	if (avail <= 0) return 0;

	memcpy(buf, mCurrent, size);
	mCurrent += size;
	return size;
}

int64_t buffer_stream::seek(int64_t offset, int whence)
{
	switch (whence)
	{
	case SEEK_SET:
		mCurrent = mBuffer->begin() + offset;
		break;
	case SEEK_CUR:
		mCurrent += offset;
		break;
	case SEEK_END:
		mCurrent = mBuffer->end() - offset;
		break;
	case AVSEEK_SIZE:
		return mBuffer->size();
	default:
		return -1;
	}
	return mCurrent - mBuffer->begin();
}

static int read_buffer(void *opaque, uint8_t *buf, int size)
{
	return ((buffer_stream *)opaque)->read(buf, size);
}

static int64_t seek_buffer(void *opaque, int64_t offset, int whence)
{
	return ((buffer_stream *)opaque)->seek(offset, whence);
}

struct ffmpeg_player : public cainteoir::audio_player
{
	ffmpeg_player(const std::shared_ptr<cainteoir::buffer> &aData, const char *aFormat);
	~ffmpeg_player();

	void play(const std::shared_ptr<cainteoir::audio> &out, const css::time &start, const css::time &end);

	int channels() const { return mAudio->codec->channels; }

	int frequency() const { return mAudio->codec->sample_rate; }

	const rdf::uri &format() const { return mAudioFormat; }
private:
	std::shared_ptr<buffer_stream> mData;
	uint8_t *mBuffer;
	AVIOContext *mIO;
	AVFormatContext *mFormat;
	AVStream *mAudio;
	AVFrame *mFrame;
	rdf::uri mAudioFormat;
	bool mIsPlanar;
};

ffmpeg_player::ffmpeg_player(const std::shared_ptr<cainteoir::buffer> &aData, const char *aFormat)
	: mData(std::make_shared<buffer_stream>(aData))
	, mBuffer(nullptr)
	, mIO(nullptr)
	, mFormat(nullptr)
	, mAudio(nullptr)
	, mIsPlanar(false)
{
	AVInputFormat *decoder = av_find_input_format(aFormat);
	if (decoder == nullptr)
		return;

	static const int buffer_size = 32768;
	mBuffer = (uint8_t *)av_malloc(buffer_size + FF_INPUT_BUFFER_PADDING_SIZE);
	mIO = avio_alloc_context(mBuffer, buffer_size, 0, mData.get(), read_buffer, nullptr, seek_buffer);

	mFormat = avformat_alloc_context();
	mFormat->pb = mIO;
	if (avformat_open_input(&mFormat, "stream", decoder, nullptr) != 0)
		return;

	if (avformat_find_stream_info(mFormat, nullptr) < 0)
		return;

	AVCodec *codec = nullptr;
	int index = av_find_best_stream(mFormat, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
	if (index < 0)
		return;
	fprintf(stdout, "stream %d = audio (%s)\n", index, codec->name);

	mAudio = mFormat->streams[index];
	mAudio->codec->codec = codec;
	if (avcodec_open2(mAudio->codec, codec, nullptr) != 0)
		return;

	mFrame = avcodec_alloc_frame();

	switch (mAudio->codec->sample_fmt)
	{
	case AV_SAMPLE_FMT_U8: mAudioFormat = rdf::tts("u8"); break;
#if __BYTE_ORDER == __LITTLE_ENDIAN
	case AV_SAMPLE_FMT_S16:  mAudioFormat = rdf::tts("s16le"); break;
	case AV_SAMPLE_FMT_S16P: mAudioFormat = rdf::tts("s16le"); mIsPlanar = true; break;
	case AV_SAMPLE_FMT_S32:  mAudioFormat = rdf::tts("s32le"); break;
	case AV_SAMPLE_FMT_S32P: mAudioFormat = rdf::tts("s32le"); mIsPlanar = true; break;
	case AV_SAMPLE_FMT_FLT:  mAudioFormat = rdf::tts("float32le"); break;
	case AV_SAMPLE_FMT_FLTP: mAudioFormat = rdf::tts("float32le"); mIsPlanar = true; break;
	case AV_SAMPLE_FMT_DBL:  mAudioFormat = rdf::tts("float64le"); break;
	case AV_SAMPLE_FMT_DBLP: mAudioFormat = rdf::tts("float64le"); mIsPlanar = true; break;
#else // big endian
	case AV_SAMPLE_FMT_S16:  mAudioFormat = rdf::tts("s16be"); break;
	case AV_SAMPLE_FMT_S16P: mAudioFormat = rdf::tts("s16be"); mIsPlanar = true; break;
	case AV_SAMPLE_FMT_S32:  mAudioFormat = rdf::tts("s32be"); break;
	case AV_SAMPLE_FMT_S32P: mAudioFormat = rdf::tts("s32be"); mIsPlanar = true; break;
	case AV_SAMPLE_FMT_FLT:  mAudioFormat = rdf::tts("float32be"); break;
	case AV_SAMPLE_FMT_FLTP: mAudioFormat = rdf::tts("float32be"); mIsPlanar = true; break;
	case AV_SAMPLE_FMT_DBL:  mAudioFormat = rdf::tts("float64be"); break;
	case AV_SAMPLE_FMT_DBLP: mAudioFormat = rdf::tts("float64be"); mIsPlanar = true; break;
#endif
	default: throw std::runtime_error("Unsupported sample format in audio file.");
	}

	if (mIsPlanar && mAudio->codec->channels > 1)
		throw std::runtime_error("Multi-channel planar audio is not supported.");
}

ffmpeg_player::~ffmpeg_player()
{
	if (mFrame) av_free(mFrame);
	if (mAudio) avcodec_close(mAudio->codec);
	if (mFormat) avformat_free_context(mFormat);
	if (mIO) av_free(mIO);
	if (mBuffer) av_free(mBuffer);
}

void ffmpeg_player::play(const std::shared_ptr<cainteoir::audio> &out, const css::time &start, const css::time &end)
{
	uint64_t from = time_to_samples(start, mAudio->codec->sample_rate, std::numeric_limits<uint64_t>::min());
	uint64_t to   = time_to_samples(end,   mAudio->codec->sample_rate, std::numeric_limits<uint64_t>::max());

	if (out->channels() != channels() || out->frequency() != frequency() || out->format() != format())
		throw std::runtime_error("cannot convert media to the output format");

	fprintf(stdout, "playing sample %" PRIu64 " to %" PRIu64 "\n", from, to);

	AVPacket reading;
	av_init_packet(&reading);

	int n = 0;
	uint64_t samples = 0;
	while (av_read_frame(mFormat, &reading) == 0)
	{
		if (reading.stream_index != mAudio->index)
			continue;

		AVPacket decoding = reading;
		while (decoding.size > 0)
		{
			int got_frame = 0;
			int length = avcodec_decode_audio4(mAudio->codec, mFrame, &got_frame, &decoding);
			if (length >= 0 && got_frame)
			{
				decoding.size -= length;
				decoding.data += length;
				uint64_t end_samples = samples + mFrame->nb_samples;
				if (samples >= from && end_samples <= to)
				{
					fprintf(stdout, "... frame %d from sample %" PRIu64 " to %" PRIu64 "\r", n++, samples, end_samples);
					int len = av_samples_get_buffer_size(nullptr, mAudio->codec->channels, mFrame->nb_samples, mAudio->codec->sample_fmt, 1);
					out->write((const char *)mFrame->data[0], len);
				}
				samples += mFrame->nb_samples;
			}
			else
			{
				decoding.size = 0;
				decoding.data = nullptr;
			}
		}
		av_free_packet(&reading);
	}
	if (mAudio->codec->codec->capabilities & CODEC_CAP_DELAY)
	{
		av_init_packet(&reading);
		int got_frame = 0;
		while (avcodec_decode_audio4(mAudio->codec, mFrame, &got_frame, &reading) >= 0 && got_frame)
		{
			fprintf(stdout, "... frame %d flushing (samples=%d)\r", n++, mFrame->nb_samples);
		}
	}
	fprintf(stdout, "\n");
}

std::shared_ptr<cainteoir::audio_player>
create_ffmpeg_player(const std::shared_ptr<cainteoir::buffer> &data, const char *decoder)
{
	av_register_all();
	return std::make_shared<ffmpeg_player>(data, decoder);
}

#else

std::shared_ptr<cainteoir::audio_player>
create_ffmpeg_player(const std::shared_ptr<cainteoir::buffer> &data, const char *decoder)
{
	return {};
}

#endif
