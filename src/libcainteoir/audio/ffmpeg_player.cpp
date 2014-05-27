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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include <cainteoir/audio.hpp>

namespace rdf = cainteoir::rdf;
namespace css = cainteoir::css;

#ifdef HAVE_FFMPEG

#include <stdexcept>
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
	if (size > avail) size = avail;

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
	resampler(AVCodecContext *codec, const std::shared_ptr<cainteoir::audio_info> &out);
	~resampler();

	cainteoir::range<uint8_t *> resample(AVFrame *frame, size_t delta_start, size_t delta_end);
private:
	AVCodecContext *mCodec;
#ifdef HAVE_LIBAVRESAMPLE
	AVAudioResampleContext *mContext;
	std::vector<uint8_t> mBuffer;
	int mBytesPerSample;
	int mChannels;
	float mFrequencyScale; // The ratio of output:input frequencies.
	AVSampleFormat mFormat;
#endif
};

resampler::resampler(AVCodecContext *codec, const std::shared_ptr<cainteoir::audio_info> &out)
	: mCodec(codec)
#ifdef HAVE_LIBAVRESAMPLE
	, mContext(nullptr)
	, mBytesPerSample(0)
	, mChannels(out->channels())
	, mFrequencyScale(0)
	, mFormat(get_av_sample_format(out->format()))
#endif
{
#ifdef HAVE_LIBAVRESAMPLE
	auto in_channels   = get_channel_layout(codec->channels);
	auto in_frequency  = codec->sample_rate;
	auto in_format     = codec->sample_fmt;
	auto out_channels  = get_channel_layout(out->channels());
	auto out_frequency = out->frequency();

	// NOTE: planar mono channel streams are compatible with non-planar mono
	//       channel streams.
	bool is_compatible = in_format == mFormat;
	if (in_channels == AV_CH_LAYOUT_MONO && out_channels == AV_CH_LAYOUT_MONO) switch (in_format)
	{
	case AV_SAMPLE_FMT_S16P: is_compatible = mFormat == AV_SAMPLE_FMT_S16; break;
	case AV_SAMPLE_FMT_S32P: is_compatible = mFormat == AV_SAMPLE_FMT_S32; break;
	case AV_SAMPLE_FMT_FLTP: is_compatible = mFormat == AV_SAMPLE_FMT_FLT; break;
	case AV_SAMPLE_FMT_DBLP: is_compatible = mFormat == AV_SAMPLE_FMT_DBL; break;
	}

	if (in_channels != out_channels || in_frequency != out_frequency || !is_compatible)
	{
		mContext = avresample_alloc_context();
		av_opt_set_int(mContext, "in_channel_layout",  in_channels, 0);
		av_opt_set_int(mContext, "in_sample_rate",     in_frequency, 0);
		av_opt_set_int(mContext, "in_sample_fmt",      in_format, 0);
		av_opt_set_int(mContext, "out_channel_layout", out_channels, 0);
		av_opt_set_int(mContext, "out_sample_rate",    out_frequency, 0);
		av_opt_set_int(mContext, "out_sample_fmt",     mFormat, 0);

		if (avresample_open(mContext) != 0)
			throw std::runtime_error("unable to initialize libavresample instance");

		mBytesPerSample = av_get_bytes_per_sample(mFormat) * mChannels;
		mFrequencyScale = float(out_frequency) / in_frequency;
	}
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

cainteoir::range<uint8_t *> resampler::resample(AVFrame *frame, size_t delta_start, size_t delta_end)
{
	uint8_t *data;
	int len;

#ifdef HAVE_LIBAVRESAMPLE
	if (mContext)
	{
		/*
		 * The av_samples_get_buffer_size and avresample_convert functions require the
		 * number of samples in the output stream. This is not provided by ffmpeg/libav.
		 *
		 * The number of output samples is dependent on the number of samples in the
		 * frame which can vary from frame to frame. The following relationship holds:
		 *
		 *     out_samples / in_samples =  out_frequency / in_frequency
		 * ==> out_samples              = (out_frequency / in_frequency) * in_samples
		 */
		int out_samples = frame->nb_samples * mFrequencyScale;

		int out_linesize = 0;
		int out_len = av_samples_get_buffer_size(&out_linesize, mChannels, out_samples, mFormat, 0);
		if (out_len > mBuffer.size())
			mBuffer.resize(out_len);

		data = &mBuffer[0];
		len = avresample_convert(mContext, &data, out_linesize, out_samples, frame->extended_data, frame->linesize[0], frame->nb_samples);
		len *= mBytesPerSample;
	}
	else
	{
#endif
		len = av_samples_get_buffer_size(nullptr, mCodec->channels, frame->nb_samples, mCodec->sample_fmt, 1);
		data = frame->data[0];
#ifdef HAVE_LIBAVRESAMPLE
	}
#endif

	if (delta_start)
	{
		delta_start *= mBytesPerSample;
		data += delta_start;
		len  -= delta_start;
	}

	if (delta_end)
		len -= (delta_end * mBytesPerSample);

	if (len <= 0) return { nullptr, nullptr };
	return { data, data + len };
}

struct ffmpeg_audio_reader : public cainteoir::audio_reader
{
	ffmpeg_audio_reader(const std::shared_ptr<cainteoir::buffer> &data);
	~ffmpeg_audio_reader();

	int channels() const { return mAudio ? mAudio->codec->channels : 0; }

	int frequency() const { return mAudio ? mAudio->codec->sample_rate : 0; }

	const rdf::uri &format() const { return mAudioFormat; }

	void set_interval(const css::time &start, const css::time &end);

	void set_target(const std::shared_ptr<cainteoir::audio_info> &info);

	bool read();
private:
	std::shared_ptr<buffer_stream> mData;
	AVIOContext *mIO;
	AVFormatContext *mFormat;
	AVStream *mAudio;
	AVFrame *mFrame;
	rdf::uri mAudioFormat;

	cainteoir::range<uint64_t> mWindow;
	std::shared_ptr<resampler> mConverter;
	uint64_t mSamples;
	AVPacket mReading;
	AVPacket mDecoding;
};

ffmpeg_audio_reader::ffmpeg_audio_reader(const std::shared_ptr<cainteoir::buffer> &data)
	: mData(std::make_shared<buffer_stream>(data))
	, mIO(nullptr)
	, mFormat(nullptr)
	, mAudio(nullptr)
	, mFrame(nullptr)
	, mWindow(0, std::numeric_limits<uint64_t>::max())
	, mSamples(0)
{
	mReading.size = 0;
	mReading.data = nullptr;

	mDecoding.size = 0;
	mDecoding.data = nullptr;

	static const int buffer_size = 32768;
	uint8_t *buffer = (uint8_t *)av_malloc(buffer_size + FF_INPUT_BUFFER_PADDING_SIZE);
	mIO = avio_alloc_context(buffer, buffer_size, 0, mData.get(), read_buffer, nullptr, seek_buffer);

	mFormat = avformat_alloc_context();
	mFormat->pb = mIO;
	if (avformat_open_input(&mFormat, "stream", nullptr, nullptr) != 0)
	{
		// NOTE: Calling avformat_open_input with a nullptr AVInputFormat causes
		// buffer to be managed by the mFormat object, so it does not need to be
		// freed in the destructor -- doing so causes a double-free memory
		// corruption error.
		av_free(buffer);
		return;
	}

	// Avoid the `[mp3 @ 0x...] max_analyze_duration reached` warning...
	mFormat->max_analyze_duration = INT_MAX;
	if (avformat_find_stream_info(mFormat, nullptr) < 0)
		return;

	AVCodec *codec = nullptr;
	int index = av_find_best_stream(mFormat, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
	if (index < 0)
		return;

	mAudio = mFormat->streams[index];
	mAudio->codec->codec = codec;
	if (avcodec_open2(mAudio->codec, codec, nullptr) != 0)
		return;

	mAudioFormat = get_format_uri(mAudio->codec->sample_fmt);

#ifdef HAVE_AV_FRAME_ALLOC
	mFrame = av_frame_alloc();
#else
	mFrame = avcodec_alloc_frame();
#endif
}

ffmpeg_audio_reader::~ffmpeg_audio_reader()
{
	if (mFrame) av_free(mFrame);
	if (mAudio) avcodec_close(mAudio->codec);
	if (mFormat) avformat_free_context(mFormat);
	if (mIO) av_free(mIO);
}

void ffmpeg_audio_reader::set_interval(const css::time &start, const css::time &end)
{
	mWindow = {
		time_to_samples(start, mAudio->codec->sample_rate, std::numeric_limits<uint64_t>::min()),
		time_to_samples(end,   mAudio->codec->sample_rate, std::numeric_limits<uint64_t>::max() - 1) + 1
	};
}

void ffmpeg_audio_reader::set_target(const std::shared_ptr<cainteoir::audio_info> &info)
{
	mConverter = std::make_shared<resampler>(mAudio->codec, info);
}

bool ffmpeg_audio_reader::read()
{
	if (!mConverter) return false;

	while (true)
	{
		if (mDecoding.size == 0)
		{
			if (mReading.data)
			{
				av_free_packet(&mReading);
				mReading.data = nullptr;
			}

			if (av_read_frame(mFormat, &mReading) != 0)
				return false;

			if (mReading.stream_index != mAudio->index)
				continue;

			mDecoding = mReading;
		}

		int got_frame = 0;
		int length = avcodec_decode_audio4(mAudio->codec, mFrame, &got_frame, &mDecoding);
		if (length >= 0 && got_frame)
		{
			mDecoding.size -= length;
			mDecoding.data += length;

			/*
			 *       |=====|     : window
			 * |===| :     :     : ... contains(frame) == no_overlap
			 *     |===|   :     : ... contains(frame) == overlap_at_start
			 *       :   |===|   : ... contains(frame) == overlap_at_end
			 *     |=========|   : ... contains(frame) == overlap_outer
			 *       : |=| :     : ... contains(frame) == overlap_inner
			 */
			cainteoir::range<uint64_t> frame = { mSamples, mSamples + mFrame->nb_samples };
			mSamples += mFrame->nb_samples;

			size_t delta_start = 0;
			size_t delta_end   = 0;
			switch (mWindow.contains(frame))
			{
			case cainteoir::overlap_at_start:
				delta_start = mWindow.begin() - frame.begin();
				break;
			case cainteoir::overlap_at_end:
				delta_end = frame.end() - mWindow.end();
				break;
			case cainteoir::overlap_outer:
				delta_start = mWindow.begin() - frame.begin();
				delta_end = frame.end() - mWindow.end();
				break;
			case cainteoir::overlap_inner:
				break;
			case cainteoir::no_overlap:
				continue;
			}

			data = mConverter->resample(mFrame, delta_start, delta_end);
			if (!data.empty())
				return true;
		}
		else
		{
			mDecoding.size = 0;
			mDecoding.data = nullptr;
		}
	}
}

std::shared_ptr<cainteoir::audio_reader>
cainteoir::create_media_reader(const std::shared_ptr<cainteoir::buffer> &data)
{
	static bool registered = false;
	if (!registered)
	{
		registered = true;
		av_register_all();
	}
	return std::make_shared<ffmpeg_audio_reader>(data);
}

#else

std::shared_ptr<cainteoir::audio_reader>
cainteoir::create_media_reader(const std::shared_ptr<cainteoir::buffer> &data)
{
	return {};
}

#endif
