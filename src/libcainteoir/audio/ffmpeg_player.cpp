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
#include <libavutil/channel_layout.h>
#ifdef HAVE_LIBAVRESAMPLE
#include <libavresample/avresample.h>
#include <libavutil/opt.h>
#endif
}

struct format_info_t
{
	const char *format;
	AVSampleFormat av_format;
};

static const format_info_t format_info[] =
{
	{ "u8",        AV_SAMPLE_FMT_U8 },
#if __BYTE_ORDER == __LITTLE_ENDIAN
	{ "s16le",     AV_SAMPLE_FMT_S16 },
	{ "s16le",     AV_SAMPLE_FMT_S16P },
	{ "s32le",     AV_SAMPLE_FMT_S32 },
	{ "s32le",     AV_SAMPLE_FMT_S32P },
	{ "float32le", AV_SAMPLE_FMT_FLT },
	{ "float32le", AV_SAMPLE_FMT_FLTP },
	{ "float64le", AV_SAMPLE_FMT_DBL },
	{ "float64le", AV_SAMPLE_FMT_DBLP },
#else // big endian
	{ "s16be",     AV_SAMPLE_FMT_S16 },
	{ "s16be",     AV_SAMPLE_FMT_S16P },
	{ "s32be",     AV_SAMPLE_FMT_S32 },
	{ "s32be",     AV_SAMPLE_FMT_S32P },
	{ "float32be", AV_SAMPLE_FMT_FLT },
	{ "float32be", AV_SAMPLE_FMT_FLTP },
	{ "float64be", AV_SAMPLE_FMT_DBL },
	{ "float64be", AV_SAMPLE_FMT_DBLP },
#endif
};

static rdf::uri get_format_uri(AVSampleFormat format)
{
	for (const auto &info : format_info)
	{
		if (info.av_format == format)
			return rdf::tts(info.format);
	}
	throw std::runtime_error("unsupported audio sample format.");
}

static AVSampleFormat get_av_sample_format(const rdf::uri &format)
{
	if (format.ns == rdf::tts.href) for (const auto &info : format_info)
	{
		if (format.ref == info.format)
			return info.av_format;
	}
	throw std::runtime_error("unsupported audio sample format.");
}

static uint64_t get_channel_layout(int channels)
{
	switch (channels)
	{
	case 1:  return AV_CH_LAYOUT_MONO;
	case 2:  return AV_CH_LAYOUT_STEREO;
	default: throw std::runtime_error("unsupported channel count");
	}
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

struct resampler
{
	resampler(AVCodecContext *codec, const std::shared_ptr<cainteoir::audio> &out);
	~resampler();

	cainteoir::buffer resample(AVFrame *frame);
private:
	AVCodecContext *mCodec;
#ifdef HAVE_LIBAVRESAMPLE
	AVAudioResampleContext *mContext;
	std::vector<uint8_t> mBuffer;
#endif
};

resampler::resampler(AVCodecContext *codec, const std::shared_ptr<cainteoir::audio> &out)
	: mCodec(codec)
#ifdef HAVE_LIBAVRESAMPLE
	, mContext(avresample_alloc_context())
#endif
{
#ifdef HAVE_LIBAVRESAMPLE
	av_opt_set_int(mContext, "in_channel_layout",  get_channel_layout(codec->channels), 0);
	av_opt_set_int(mContext, "in_sample_rate",     codec->sample_rate, 0);
	av_opt_set_int(mContext, "in_sample_fmt",      codec->sample_fmt, 0);
	av_opt_set_int(mContext, "out_channel_layout", get_channel_layout(out->channels()), 0);
	av_opt_set_int(mContext, "out_sample_rate",    out->frequency(), 0);
	av_opt_set_int(mContext, "out_sample_fmt",     get_av_sample_format(out->format()), 0);

	if (avresample_open(mContext) != 0)
		throw std::runtime_error("unable to initialize libavresample instance");
#else
	if (out->channels() != 1 && av_sample_fmt_is_planar(mAudio->codec->sample_fmt))
		throw std::runtime_error("cannot play planar-layout multi-channel audio");
	if (out->channels() != ctx->channels ||
	    out->frequency() != ctx->sample_rate ||
	    out->format() != get_format_uri(ctx->sample_fmt))
		throw std::runtime_error("cannot convert media to the output format");
#endif
}

resampler::~resampler()
{
#ifdef HAVE_LIBAVRESAMPLE
	if (mContext) avresample_free(&mContext);
#endif
}

cainteoir::buffer resampler::resample(AVFrame *frame)
{
	int len = av_samples_get_buffer_size(nullptr, mCodec->channels, frame->nb_samples, mCodec->sample_fmt, 1);
#ifdef HAVE_LIBAVRESAMPLE
	if (len > mBuffer.size())
		mBuffer.resize(len);
	char *data = (char *)&mBuffer[0];
	int ret = avresample_convert(mContext, (uint8_t **)&data, mBuffer.size(), frame->nb_samples, frame->extended_data, frame->linesize[0], frame->nb_samples);
	return cainteoir::buffer(data, data + mBuffer.size());
#else
	const char *data = (const char *)frame->data[0];
	return cainteoir::buffer(data, data + len);
#endif
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
};

ffmpeg_player::ffmpeg_player(const std::shared_ptr<cainteoir::buffer> &aData, const char *aFormat)
	: mData(std::make_shared<buffer_stream>(aData))
	, mBuffer(nullptr)
	, mIO(nullptr)
	, mFormat(nullptr)
	, mAudio(nullptr)
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
	mAudioFormat = get_format_uri(mAudio->codec->sample_fmt);
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

	resampler converter(mAudio->codec, out);

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
					cainteoir::buffer data = converter.resample(mFrame);
					out->write((const char *)data.begin(), data.size());
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
