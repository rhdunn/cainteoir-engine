/* Example program for interacting with voice synthesizers.
 *
 * Copyright (C) 2013-2014 Reece H. Dunn
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
#include "options.hpp"

#include <cainteoir/synthesizer.hpp>
#include <cainteoir/engines.hpp>
#include <stdexcept>
#include <iostream>

namespace css = cainteoir::css;
namespace tts = cainteoir::tts;
namespace ipa = cainteoir::ipa;
namespace rdf = cainteoir::rdf;
namespace rql = cainteoir::rdf::query;

enum class actions
{
	show_metadata,
	print_pho,
	print_diphones,
	synthesize,
};

static void show_metadata(rdf::graph &metadata)
{
	(*rdf::create_formatter(std::cout, rdf::formatter::turtle))
		<< rdf::rdf
		<< rdf::rdfa
		<< rdf::rdfs
		<< rdf::xsd
		<< rdf::xml
		<< rdf::owl
		<< rdf::dc
		<< rdf::dcterms
		<< rdf::dcam
		<< rdf::epub
		<< rdf::opf
		<< rdf::ocf
		<< rdf::pkg
		<< rdf::media
		<< rdf::ncx
		<< rdf::dtb
		<< rdf::smil
		<< rdf::xhtml
		<< rdf::skos
		<< rdf::foaf
		<< rdf::tts
		<< rdf::iana
		<< rdf::subtag
		<< metadata;
}

static std::shared_ptr<tts::prosody_reader>
create_reader(const char *filename, const char *phonemeset)
{
	auto data = filename ? cainteoir::make_file_buffer(filename) : cainteoir::make_file_buffer(stdin);
	return tts::createPhoReader(tts::createPhonemeParser(phonemeset), data);
}

static void
print(const char *filename,
      actions action,
      const char *src_phonemeset,
      const char *dst_phonemeset)
{
	auto pho = create_reader(filename, src_phonemeset);
	if (action == actions::print_diphones)
		pho = tts::createDiphoneReader(pho);

	auto out = tts::createPhoWriter(tts::createPhonemeWriter(dst_phonemeset ? dst_phonemeset : src_phonemeset));
	out->reset(stdout);

	while (pho->read())
		out->write(*pho);
}

static void
synthesize(rdf::graph &metadata,
           const char *voicename,
           const char *filename,
           actions action,
           const char *outformat,
           const char *outfile,
           const char *device_name)
{
	const rdf::uri *voiceref = nullptr;
	if (voicename)
		voiceref = tts::get_voice_uri(metadata, rdf::tts("name"), voicename);

	auto voice = tts::create_voice_synthesizer(metadata, voiceref);
	if (!voice) throw std::runtime_error("cannot find the specified voice");

	const std::string phonemeset = rql::select_value<std::string>(metadata,
		rql::subject == *voiceref && rql::predicate == rdf::tts("phonemeset"));
	auto pho = create_reader(filename, phonemeset.c_str());

	rdf::uri doc;
	std::shared_ptr<cainteoir::audio> out;
	if (outformat || outfile)
		out = cainteoir::create_audio_file(outfile, outformat ? outformat : "wav", 0.3, metadata, doc,
			voice->format(),
			voice->channels(),
			voice->frequency());
	else
		out = cainteoir::open_audio_device(device_name, metadata, doc,
			voice->format(),
			voice->channels(),
			voice->frequency());

	out->open();
	voice->bind(pho);
	voice->synthesize(out.get());
	out->close();
}

int main(int argc, char **argv)
{
	try
	{
		const char *outfile = nullptr;
		const char *outformat = nullptr;
		const char *device_name = nullptr;
		const char *voicename = nullptr;
		const char *src_phonemeset = "ipa";
		const char *dst_phonemeset = nullptr;
		actions action = actions::synthesize;

		const option_group general_options = { nullptr, {
			{ 'M', "metadata", bind_value(action, actions::show_metadata),
			  i18n("Show the RDF metadata for the engine and voices") },
			{ 0, "pho", bind_value(action, actions::print_pho),
			  i18n("Output the PHO file contents to stdout") },
			{ 0, "diphones", bind_value(action, actions::print_diphones),
			  i18n("Output the PHO file contents to stdout as diphones") },
			{ 'P', "phonemeset", src_phonemeset, "PHONEMESET",
			  i18n("Use PHONEMESET to read phonemes in (default: ipa)") },
			{ 0, "output-phonemeset", dst_phonemeset, "PHONEMESET",
			  i18n("Use PHONEMESET to display phonemes in (default: phonemeset)") },
			{ 'D', "device", device_name, "DEVICE",
			  i18n("Use DEVICE for audio output (ALSA/pulseaudio device name)") },
		}};

		const option_group speech_options = { i18n("Speech:"), {
			{ 'v', "voice", voicename, "VOICE",
			  i18n("Use the voice named VOICE") },
		}};

		const option_group recording_options = { i18n("Recording:"), {
			{ 'o', "output", outfile, "FILE",
			  i18n("Recorded audio is written to FILE") },
			{ 0, "stdout", bind_value(outfile, "-"),
			  i18n("Recorded audio is written to the standard output") },
			{ 'r', "record", outformat, "FORMAT",
			  i18n("Record the audio as a FORMAT file (default: wav)") },
		}};

		const std::initializer_list<option_group> options = {
			general_options,
			speech_options,
			recording_options,
		};

		const std::initializer_list<const char *> usage = {
			i18n("voice-synthesizer [OPTION..] PHONEME_FILE"),
			i18n("voice-synthesizer [OPTION..]"),
		};

		if (!parse_command_line(options, usage, argc, argv))
			return 0;

		rdf::graph metadata;
		tts::read_voice_metadata(metadata);

		switch (action)
		{
		case actions::show_metadata:
			show_metadata(metadata);
			break;
		case actions::print_pho:
		case actions::print_diphones:
			print(argc == 1 ? argv[0] : nullptr, action, src_phonemeset, dst_phonemeset);
			break;
		case actions::synthesize:
			synthesize(metadata, voicename, argc == 1 ? argv[0] : nullptr, action, outformat, outfile, device_name);
			break;
		}
	}
	catch (std::runtime_error &e)
	{
		fprintf(stderr, "error: %s\n", e.what());
	}

	return 0;
}
