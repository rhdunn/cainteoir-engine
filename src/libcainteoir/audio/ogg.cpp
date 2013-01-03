/* Ogg+Vorbis Audio File.
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

#include "config.h"
#include "compatibility.hpp"
#include "i18n.h"

#include <cainteoir/audio.hpp>
#include <stdexcept>
#include <stdio.h>
#include <time.h>

namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

static std::string now()
{
	time_t now;
	time(&now);

	struct tm *d = localtime(&now);

	char date[13];
	strftime(date, 15, "%Y", d);

	return date;
}

/// @addtogroup audio
/// @{

/** @struct cainteoir::vorbis_comment
  * @brief  A vorbis comment.
  * @see    http://www.xiph.org/vorbis/doc/v-comment.html
  */

/// @}

/** @var   cainteoir::vorbis_comment::label
  * @brief The name of the comment, e.g. TITLE.
  */

/** @var   cainteoir::vorbis_comment::value
  * @brief The content of the comment, e.g. "Moonlight Sonata".
  */

/// @addtogroup audio
/// @{

/** @brief Convert an RDF graph to a set of Vorbis Comments.
  * @see   http://www.xiph.org/vorbis/doc/v-comment.html
  * @see   http://wiki.xiph.org/Metadata
  *
  * @param[in] aMetadata The RDF metadata used to create the vorbis comments.
  * @param[in] aDocument The URI of the document in the RDF graph to convert to vorbis comments.
  *
  * @return The vorbis comments in (label, value) form for @a aDocument.
  */
std::list<cainteoir::vorbis_comment>
cainteoir::vorbis_comments(const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	std::list<vorbis_comment> comments;
	std::string year;
	std::string created;
	std::string published;

	for (auto &query : rql::select(aMetadata, rql::subject == aDocument))
	{
		const std::string &object = rql::value(query);
		if (!object.empty())
		{
			if (rql::predicate(query) == rdf::dc("creator"))
				comments.push_back({ "ARTIST", object });
			else if (rql::predicate(query) == rdf::dc("title"))
				comments.push_back({ "ALBUM", object });
			else if (rql::predicate(query) == rdf::dc("description"))
			{
				comments.push_back({ "DESCRIPTION", object });
				comments.push_back({ "COMMENT",     object });
			}
			else if (rql::predicate(query) == rdf::dc("date") && year.empty())
				year = object;
		}
		else if (rql::predicate(query) == rdf::dc("creator") || rql::predicate(query) == rdf::dcterms("creator"))
		{
			std::string role;
			std::string author;

			for (auto &data : rql::select(aMetadata, rql::subject == rql::object(query)))
			{
				if (rql::predicate(data) == rdf::rdf("value"))
					author = rql::value(data);
				else if (rql::predicate(data) == rdf::opf("role"))
					role = rql::value(data);
				else if (rql::predicate(data) == rdf::pkg("role"))
				{
					rql::results selection = rql::select(aMetadata, rql::subject == rql::object(data) && rql::predicate == rdf::rdf("value"));
					if (!selection.empty())
						role = rql::value(selection.front());
				}
			}

			if (!author.empty() && (role == "aut" || role.empty()))
				comments.push_back({ "ARTIST", author });
		}
		else if (rql::predicate(query) == rdf::dc("date"))
		{
			std::string event;
			std::string date;

			for (auto &data : rql::select(aMetadata, rql::subject == rql::object(query)))
			{
				const std::string &object = rql::value(data);
				if (rql::predicate(data) == rdf::rdf("value"))
					date = object;
				else if (rql::predicate(data) == rdf::opf("event"))
					event = object;
			}

			if (!date.empty())
			{
				if (event == "creation")
					created = date;
				else if (event == "publication")
					published = date;
			}
		}
		else if (rql::predicate(query).ns == rdf::dcterms)
		{
			std::string value;
			for (auto &data : rql::select(aMetadata, rql::subject == rql::object(query)))
			{
				if (rql::predicate(data) == rdf::rdf("value"))
					value = rql::value(data);
			}

			if (!value.empty())
			{
				if (rql::predicate(query) == rdf::dcterms("title"))
					comments.push_back({ "ALBUM", value });
			}
		}
	}

	if (!published.empty())
		year = published;
	else if (!created.empty())
		year = created;

	if (year.empty())
		year = now();

	std::string::size_type ym = year.find('-');
	if (ym != std::string::npos)
		year = year.substr(0, ym);

	comments.push_back({ "GENRE", "Vocal" });
	comments.push_back({ "DATE",  year });

	comments.push_back({ "PERFORMER", i18n("Cainteoir(TM) Text-to-Speech") });
	comments.push_back({ "LICENSE",   "http://creativecommons.org/licenses/by-sa/3.0/" });
	comments.push_back({ "CONTACT",   "http://rhdunn.github.com/cainteoir/" });
	return comments;
}

/// @}

#if defined(HAVE_VORBISENC)

#include <vorbis/vorbisenc.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

class ogg_audio : public cainteoir::audio
{
	FILE *m_file;

	ogg_stream_state os;
	ogg_page og;
	ogg_packet op;

	vorbis_info vi;
	vorbis_comment vc;
	vorbis_dsp_state vd;
	vorbis_block vb;

	void write_ogg_data(long i)
	{
		vorbis_analysis_wrote(&vd, i);

		while (vorbis_analysis_blockout(&vd, &vb) == 1)
		{
			vorbis_analysis(&vb, nullptr);
			vorbis_bitrate_addblock(&vb);

			while (vorbis_bitrate_flushpacket(&vd, &op))
			{
				ogg_stream_packetin(&os, &op);

				while (!ogg_page_eos(&og))
				{
					int result = ogg_stream_pageout(&os, &og);
					if (result == 0)
						break;

					fwrite(og.header, 1, og.header_len, m_file);
					fwrite(og.body,   1, og.body_len,   m_file);
				}
			}
		}
	}
public:
	ogg_audio(FILE *f, int channels, int frequency, float quality, const std::list<cainteoir::vorbis_comment> &comments)
		: m_file(f)
	{
		vorbis_info_init(&vi);
		vorbis_encode_init_vbr(&vi, channels, frequency, quality);

		vorbis_comment_init(&vc);
		for (auto &comment : comments)
			vorbis_comment_add_tag(&vc, comment.label.c_str(), comment.value.c_str());

		vorbis_analysis_init(&vd, &vi);
		vorbis_block_init(&vd, &vb);

		srand(time(nullptr));
		ogg_stream_init(&os, rand());
	}

	~ogg_audio()
	{
		close();
	}

	void open()
	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;

		vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
		ogg_stream_packetin(&os, &header);
		ogg_stream_packetin(&os, &header_comm);
		ogg_stream_packetin(&os, &header_code);

		while (true)
		{
			int result = ogg_stream_flush(&os, &og);
			if (result == 0)
				break;

			fwrite(og.header, 1, og.header_len, m_file);
			fwrite(og.body,   1, og.body_len,   m_file);
		}
	}

	void close()
	{
		if (!m_file)
			return;

		write_ogg_data(0);

		ogg_stream_clear(&os);
		vorbis_block_clear(&vb);
		vorbis_dsp_clear(&vd);
		vorbis_comment_clear(&vc);
		vorbis_info_clear(&vi);

		if (m_file != stdout)
			fclose(m_file);
		m_file = nullptr;
	}

	uint32_t write(const char *data, uint32_t len)
	{
		if (len == 0)
			return 0;

		float **buffer = vorbis_analysis_buffer(&vd, len);

		long i;
		for (i = 0; i < len/2; ++i)
			buffer[0][i] = ((data[i*2+1]<<8)|(0x00ff&(int)data[i*2]))/32768.f;

		write_ogg_data(i);

		return len;
	}
};

/// @private
std::shared_ptr<cainteoir::audio>
create_ogg_file(const char *filename, const rdf::uri &format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	FILE *file = filename ? fopen(filename, "wb") : stdout;
	if (format != rdf::tts("s16le"))
		throw std::runtime_error(i18n("unsupported audio format."));
	return std::make_shared<ogg_audio>(file, channels, frequency, quality, cainteoir::vorbis_comments(aMetadata, aDocument));
}

#else

/// @private
std::shared_ptr<cainteoir::audio>
create_ogg_file(const char *filename, const rdf::uri &format, int channels, int frequency, float quality, const rdf::graph &aMetadata, const rdf::uri &aDocument)
{
	return std::shared_ptr<cainteoir::audio>();
}

#endif
