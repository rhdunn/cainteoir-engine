/* Music player.
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

/* This is a demonstration of the Cainteoir Text-to-Speech audio APIs used in
 * the listening and recording of text, and the implementation of audio overlay
 * formats such as ePub 3 Media Overlays and SMIL.
 */

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"
#include "options.hpp"

#include <cainteoir/audio.hpp>
#include <cainteoir/mimetype.hpp>
#include <stdexcept>
#include <iostream>
#include <cstdio>

namespace css  = cainteoir::css;
namespace mime = cainteoir::mime;

#ifdef HAVE_FFMPEG

extern "C"
{
#include <libavformat/avformat.h>
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

struct ffmpeg_player
{
	ffmpeg_player(const std::shared_ptr<cainteoir::buffer> &aData, const char *aFormat);
	~ffmpeg_player();
private:
	std::shared_ptr<buffer_stream> mData;
	uint8_t *mBuffer;
	AVIOContext *mIO;
	AVFormatContext *mFormat;
	AVCodecContext *mAudio;
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

	mAudio = mFormat->streams[index]->codec;
	mAudio->codec = codec;
	if (avcodec_open2(mAudio, codec, nullptr) != 0)
		return;

	fprintf(stdout, "channels    : %d\n", mAudio->channels);
	fprintf(stdout, "sample rate : %dHz\n", mAudio->sample_rate);
	fprintf(stdout, "format      : %s\n", av_get_sample_fmt_name(mAudio->sample_fmt));
}

ffmpeg_player::~ffmpeg_player()
{
	if (mAudio) avcodec_close(mAudio);
	if (mFormat) avformat_free_context(mFormat);
	if (mIO) av_free(mIO);
	if (mBuffer) av_free(mBuffer);
}

std::shared_ptr<ffmpeg_player> create_media_player(const std::shared_ptr<cainteoir::buffer> &data)
{
	avcodec_register_all();
	av_register_all();

	if (mime::ogg.match(data)) return std::make_shared<ffmpeg_player>(data, "ogg");
	if (mime::mp3.match(data)) return std::make_shared<ffmpeg_player>(data, "mp3");
	if (mime::mp4.match(data)) return std::make_shared<ffmpeg_player>(data, "mp4");
	if (mime::wav.match(data)) return std::make_shared<ffmpeg_player>(data, "wav");
	return {};
}

#else

std::shared_ptr<void> create_media_player(const std::shared_ptr<cainteoir::buffer> &data)
{
	return {};
}

#endif

static void print_time(const css::time &time, const char *name)
{
	switch (time.units())
	{
	case css::time::inherit:
		fprintf(stdout, "%s : unspecified\n", name);
		break;
	case css::time::seconds:
		fprintf(stdout, "%s : %Gs\n", name, time.value());
		break;
	case css::time::milliseconds:
		fprintf(stdout, "%s : %Gms\n", name, time.value());
		break;
	}
}

int main(int argc, char ** argv)
{
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	try
	{
		const char *start_time = nullptr;
		const char *end_time   = nullptr;

		const option_group general_options = { nullptr, {
			{ 's', "start", start_time, "TIME",
			  i18n("The time to start playback from") },
			{ 'e', "end", end_time, "TIME",
			  i18n("The time to end playback at") },
		}};

		const std::initializer_list<option_group> options = {
			general_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("musicplayer [OPTION..] AUDIO_FILE"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		css::time start = css::parse_smil_time(start_time);
		css::time end   = css::parse_smil_time(end_time);

		print_time(start, "start time ");
		print_time(end,   "end time   ");

		auto data = cainteoir::make_file_buffer(argv[0]);
		auto player = create_media_player(data);
		if (player)
		{
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, i18n("error: %s\n"), e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
