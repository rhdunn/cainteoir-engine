#<cldoc:cainteoir::audio_info>

Provide information about an audio file or device.

#<cldoc:cainteoir::audio_info::~audio_info>

Cleanup the audio object.

#<cldoc:cainteoir::audio_info::channels>

Get the number of channels the audio object has.

@return The number of channels the audio object has.

#<cldoc:cainteoir::audio_info::format>

Get the audio sample format of the audio object.

@return The audio sample format of the audio object.

#<cldoc:cainteoir::audio_info::frequency>

Get the audio sample frequency of the audio object.

@return The audio sample frequency of the audio object.

#<cldoc:cainteoir::audio>

Manage an audio device or file.

#<cldoc:cainteoir::audio::open>

Opens a connection to the audio device or creates the audio file.

#<cldoc:cainteoir::audio::close>

Closes the connection to the audio device or file.

#<cldoc:cainteoir::audio::write>

Write the specified audio data to the device or file.
@data The audio data.
@len  The length of the audio data in bytes.

@return The number of bytes written.

#<cldoc:cainteoir::audio_reader>

Read audio from an audio data object.

#<cldoc:cainteoir::audio_reader::data>

The audio data read from the audio object.

#<cldoc:cainteoir::audio_reader::audio_reader>

Initialize the audio reader.

#<cldoc:cainteoir::audio_reader::set_interval>

Specify the time range of the audio to read.
@start The point in the audio object to start reading from.
@end   The point in the audio object to end reading at.

#<cldoc:cainteoir::audio_reader::set_target>

Specify the target audio format to use.
@info  The format to read the data as.

#<cldoc:cainteoir::audio_reader::read>

Read the next block of audio.

@return `true` if an audio block was read, `false` otherwise.

#<cldoc:cainteoir::vorbis_comment>

A vorbis comment.

#<cldoc:cainteoir::vorbis_comment::label>

The name of the comment, e.g. TITLE.

#<cldoc:cainteoir::vorbis_comment::value>

The content of the comment, e.g. "Moonlight Sonata".

#<cldoc:cainteoir::vorbis_comments>

Convert an RDF graph to a set of Vorbis Comments.
@aMetadata The RDF metadata used to create the vorbis comments.
@aDocument The URI of the document in the RDF graph to convert to vorbis comments.

@return The vorbis comments in (label, value) form for `aDocument`.

#<cldoc:cainteoir::supported_audio_formats>

Get the audio formats that are supported by libcainteoir.
@metadata The RDF graph to write the format support to.

#<cldoc:cainteoir::create_audio_file>

Create an audio file to write data to.
@filename       The name of the audio file.
@type           The file type (currently either 'wav' or 'ogg').
@quality        The encoding quality to use (for lossy encodings).
@aDocMetadata   The RDF graph containing the document metadata.
@aDocument      The subject to use to extract the document metadata.
@aFormat        The sample format for the file.
@aChannels      The number of channels the audio file will have.
@aFrequency     The sample frequency for the file.

The document metadata is used to create metadata tags on the audio file
(title, author, etc.).

@return An audio object associated with the file.

#<cldoc:cainteoir::create_audio_file>

Create an audio file to write data to.
@filename       The name of the audio file.
@type           The file type (currently either 'wav' or 'ogg').
@quality        The encoding quality to use (for lossy encodings).
@aDocMetadata   The RDF graph containing the document metadata.
@aDocument      The subject to use to extract the document metadata.
@aVoiceMetadata The RDF graph containing the voice metadata.
@aVoice         The subject to use to extract the voice metadata.

The document metadata is used to create metadata tags on the audio file
(title, author, etc.). The voice metadata is used to determine the number
of channels, frequency and sample format of the audio.

@return An audio object associated with the file.

#<cldoc:cainteoir::open_audio_device>

Open an audio device to write data to.
@device         The name of the audio device.
@aDocMetadata   The RDF graph containing the document metadata.
@aDocument      The subject to use to extract the document metadata.
@aFormat        The sample format for the file.
@aChannels      The number of channels the audio file will have.
@aFrequency     The sample frequency for the file.

The document metadata is used to create metadata tags on the audio device
(title, author, etc.).

@return An audio object associated with the device.

#<cldoc:cainteoir::open_audio_device>

Open an audio device to write data to.
@device         The name of the audio device.
@aDocMetadata   The RDF graph containing the document metadata.
@aDocument      The subject to use to extract the document metadata.
@aVoiceMetadata The RDF graph containing the voice metadata.
@aVoice         The subject to use to extract the voice metadata.

The document metadata is used to create metadata tags on the audio device
(title, author, etc.). The voice metadata is used to determine the number
of channels, frequency and sample format of the audio.

@return An audio object associated with the device.

#<cldoc:cainteoir::create_media_reader>

Create an object that can read audio data.
@data The audio data buffer to read.

This encapsulates the ffmpeg/libav logic into an easy to use API that uses the
Cainteoir Engine framework. It is used to implement the Media Overlays
functionality of ePub 3 documents and other things that read audio data.

This API and the associated implementation have no concept of what file
formats are supported. That is entirely dependent on what file formats
ffmpeg/libav is configured with.

For example, to play the test.ogg file you can use:

	auto data = cainteoir::make_file_buffer("test.ogg");
	auto player = cainteoir::create_media_reader(data);
	if (player)
	{
		cainteoir::rdf::graph doc;
		cainteoir::rdf::uri   subject;
		auto out = cainteoir::open_audio_device(nullptr, doc, subject,
			player->format(),
			player->channels(),
			player->frequency());

		out->open();
		player->set_target(out);
		while (player->read())
			out->write((const char *)player->data.begin(), player->data.size());
		out->close();
	}

@return An audio playback object.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
