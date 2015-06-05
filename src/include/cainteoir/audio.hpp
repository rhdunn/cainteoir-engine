/* Audio API.
 *
 * Copyright (C) 2010-2015 Reece H. Dunn
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

#ifndef CAINTEOIR_ENGINE_AUDIO_HPP
#define CAINTEOIR_ENGINE_AUDIO_HPP

#include "metadata.hpp"
#include <stdint.h>

namespace cainteoir
{
	struct audio_info
	{
		virtual ~audio_info() {}

		virtual int channels() const = 0;

		virtual int frequency() const = 0;

		virtual const rdf::uri &format() const = 0;
	};

	struct audio : public audio_info
	{
		virtual void open() = 0;

		virtual void close() = 0;

		virtual uint32_t write(const char *data, uint32_t len) = 0;
	};

	struct audio_reader : public audio_info
	{
		cainteoir::range<uint8_t *> data;

		audio_reader() : data(nullptr, nullptr)
		{
		}

		virtual void set_interval(const css::time &start, const css::time &end) = 0;

		virtual void set_target(const std::shared_ptr<audio_info> &info) = 0;

		virtual bool read() = 0;
	};

	struct vorbis_comment
	{
		const std::string label;
		const std::string value;
	};

	void add_document_metadata(std::list<cainteoir::vorbis_comment> &aComments,
	                           const rdf::graph &aMetadata,
	                           const rdf::uri &aDocument);

	void supported_audio_formats(rdf::graph &metadata);

	std::shared_ptr<audio_info>
	create_audio_info(
		const rdf::uri &aFormat,
		int aChannels,
		int aFrequency);

	std::shared_ptr<audio>
	create_wav_file(const char *filename,
	                const rdf::uri &format,
	                int channels,
	                int frequency);

	std::shared_ptr<audio>
	create_wav_file(const char *aFileName,
	                const rdf::graph &aVoiceMetadata,
	                const rdf::uri &aVoice);

	std::shared_ptr<audio>
	create_ogg_file(const char *aFileName,
	                const std::list<vorbis_comment> &aMetadata,
	                float aQuality,
	                const rdf::uri &aFormat,
	                int aChannels,
	                int aFrequency);

	std::shared_ptr<audio>
	create_ogg_file(const char *aFileName,
	                const std::list<vorbis_comment> &aMetadata,
	                float aQuality,
	                const rdf::graph &aVoiceMetadata,
	                const rdf::uri &aVoice);

	std::shared_ptr<audio>
	open_audio_device(
		const char *device,
		const rdf::graph &aDocMetadata,
		const rdf::uri &aDocument,
		const rdf::graph &aVoiceMetadata,
		const rdf::uri &aVoice);

	std::shared_ptr<audio>
	open_audio_device(
		const char *device,
		const rdf::graph &aDocMetadata,
		const rdf::uri &aDocument,
		const rdf::uri &aFormat,
		int aChannels,
		int aFrequency);

	std::shared_ptr<audio_reader>
	create_media_reader(const std::shared_ptr<cainteoir::buffer> &data);
}

#endif
