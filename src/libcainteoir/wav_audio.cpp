/* WAVE Audio File.
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

#include <cainteoir/audio.hpp>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

struct WaveHeader
{
	// RIFF header
	char id[4];
	uint32_t size;
	char format[4];
	// fmt header
	char fmt_id[4];
	uint32_t fmt_size;
	uint16_t type;
	uint16_t channels;
	uint32_t frequency;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t sample_size;
	// data header
	char data_id[4];
	uint32_t data_size;
};

class wav_audio : public cainteoir::audio
{
	FILE *m_file;
	WaveHeader m_header;
public:
	wav_audio(FILE *f, cainteoir::metadata *data) : m_file(f)
	{
		WaveHeader header = {
			{ 'R', 'I', 'F', 'F' }, 0x7FFFFFFF, { 'W', 'A', 'V', 'E' },
			{ 'f', 'm', 't', ' ' }, 16, 1, 0, 0, 0, 0, 0,
			{ 'd', 'a', 't', 'a' }, 0x7FFFFFFF - sizeof(WaveHeader)
		};

		header.channels = !strcmp(data->get_metadata(cainteoir::channels), "mono") ? 1 : 2;
		sscanf(data->get_metadata(cainteoir::frequency), "%dHz", &header.frequency);

		if (!strcmp(data->get_metadata(cainteoir::audio_format), "S16_LE"))
			header.sample_size = 16;

		header.byte_rate = header.frequency * header.channels * (header.sample_size / 8);
		header.block_align = header.channels * (header.sample_size / 8);

		m_header = header;

		fwrite(&m_header, 1, sizeof(m_header), m_file);
		m_header.size = sizeof(WaveHeader);
		m_header.data_size = 0;
	}

	~wav_audio()
	{
		close();
	}

	uint32_t write(const void *data, uint32_t len)
	{
		m_header.size += len;
		m_header.data_size += len;

		return fwrite(data, 1, len, m_file);
	}

	void close()
	{
		if (!m_file || m_file == stdout)
			return;

		fseek(m_file, 0, SEEK_SET);
		fwrite(&m_header, 1, sizeof(m_header), m_file);

		fclose(m_file);
		m_file = NULL;
	}
};

std::auto_ptr<cainteoir::audio> cainteoir::create_wav_file(const char *filename, cainteoir::metadata *data)
{
	FILE *file = filename ? fopen(filename, "wb") : stdout;
	return std::auto_ptr<cainteoir::audio>(new wav_audio(file, data));
}

