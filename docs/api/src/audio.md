# cainteoir::audio_info {: .doc }

Provide information about an audio file or device.

# cainteoir::audio_info::~audio_info {: .doc }

Cleanup the audio object.

# cainteoir::audio_info::channels {: .doc }

Get the number of channels the audio object has.

@return
: The number of channels the audio object has.

# cainteoir::audio_info::format {: .doc }

Get the audio sample format of the audio object.

@return
: The audio sample format of the audio object.

# cainteoir::audio_info::frequency {: .doc }

Get the audio sample frequency of the audio object.

@return
: The audio sample frequency of the audio object.

# cainteoir::audio {: .doc }

Manage an audio device or file.

# cainteoir::audio::open {: .doc }

Opens a connection to the audio device or creates the audio file.

# cainteoir::audio::close {: .doc }

Closes the connection to the audio device or file.

# cainteoir::audio::write {: .doc }

Write the specified audio data to the device or file.

@data
: The audio data.

@len
: The length of the audio data in bytes.

@return
: The number of bytes written.

# cainteoir::audio_reader {: .doc }

Read audio from an audio data object.

# cainteoir::audio_reader::data {: .doc }

The audio data read from the audio object.

# cainteoir::audio_reader::audio_reader {: .doc }

Initialize the audio reader.

# cainteoir::audio_reader::set_interval {: .doc }

Specify the time range of the audio to read.

@start
: The point in the audio object to start reading from.

@end
: The point in the audio object to end reading at.

# cainteoir::audio_reader::set_target {: .doc }

Specify the target audio format to use.

@info
: The format to read the data as.

# cainteoir::audio_reader::read {: .doc }

Read the next block of audio.

@return
: `true` if an audio block was read, `false` otherwise.

# cainteoir::vorbis_comment {: .doc }

A vorbis comment.

# cainteoir::vorbis_comment::label {: .doc }

The name of the comment, e.g. TITLE.

# cainteoir::vorbis_comment::value {: .doc }

The content of the comment, e.g. "Moonlight Sonata".

# cainteoir::vorbis_comments {: .doc }

Convert an RDF graph to a set of Vorbis Comments.

@aMetadata
: The RDF metadata used to create the vorbis comments.

@aDocument
: The URI of the document in the RDF graph to convert to vorbis comments.

@return
: The vorbis comments in (label, value) form for `aDocument`.

# cainteoir::supported_audio_formats {: .doc }

Get the audio formats that are supported by libcainteoir.
@metadata The RDF graph to write the format support to.

# cainteoir::create_audio_info {: .doc }

Create an [audio_info](^^cainteoir::audio_info) object on its own. This can be
used to specify a custom audio profile for use as the target for a media
reader.

@aFormat
: The sample format for the file.

@aChannels
: The number of channels the audio file will have.

@aFrequency
: The sample frequency for the file.

@return
: An audio information object.

# cainteoir::create_audio_file {: .doc }

Create an audio file to write data to.

@filename
: The name of the audio file.

@type
: The file type (currently either 'wav' or 'ogg').

@quality
: The encoding quality to use (for lossy encodings).

@aDocMetadata
: The RDF graph containing the document metadata.

@aDocument
: The subject to use to extract the document metadata.

@aFormat
: The sample format for the file.

@aChannels
: The number of channels the audio file will have.

@aFrequency
: The sample frequency for the file.

The document metadata is used to create metadata tags on the audio file
(title, author, etc.).

@return
: An audio object associated with the file.

# cainteoir::create_audio_file {: .doc }

Create an audio file to write data to.

@filename
: The name of the audio file.

@type
: The file type (currently either 'wav' or 'ogg').

@quality
: The encoding quality to use (for lossy encodings).

@aDocMetadata
: The RDF graph containing the document metadata.

@aDocument
: The subject to use to extract the document metadata.

@aVoiceMetadata
: The RDF graph containing the voice metadata.

@aVoice
: The subject to use to extract the voice metadata.

The document metadata is used to create metadata tags on the audio file
(title, author, etc.). The voice metadata is used to determine the number
of channels, frequency and sample format of the audio.

@return
: An audio object associated with the file.

# cainteoir::open_audio_device {: .doc }

Open an audio device to write data to.

@device
: The name of the audio device.

@aDocMetadata
: The RDF graph containing the document metadata.

@aDocument
: The subject to use to extract the document metadata.

@aFormat
: The sample format for the file.

@aChannels
: The number of channels the audio file will have.

@aFrequency
: The sample frequency for the file.

The document metadata is used to create metadata tags on the audio device
(title, author, etc.).

@return
: An audio object associated with the device.

# cainteoir::open_audio_device {: .doc }

Open an audio device to write data to.

@device
: The name of the audio device.

@aDocMetadata
: The RDF graph containing the document metadata.

@aDocument
: The subject to use to extract the document metadata.

@aVoiceMetadata
: The RDF graph containing the voice metadata.

@aVoice
: The subject to use to extract the voice metadata.

The document metadata is used to create metadata tags on the audio device
(title, author, etc.). The voice metadata is used to determine the number
of channels, frequency and sample format of the audio.

@return An audio object associated with the device.

# cainteoir::create_media_reader {: .doc }

Create an object that can read audio data.

@data
: The audio data buffer to read.

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

@return
: An audio playback object.

# cainteoir::audio_data {: .doc }

Stores audio sample data.

# cainteoir::audio_data::samples {: .doc }

The samples for the audio.

# cainteoir::audio_data::info {: .doc }

Information about the associated audio samples.

# cainteoir::read_s16_samples {: .doc }

Extract the S16 audio samples from an audio data buffer.

@aData
: The audio data buffer to read.

@aStart
: The point in the audio object to start reading from.

@aEnd
: The point in the audio object to end reading at.

@aChannel
: The index of the audio channel to extract the samples for.

@aFrequency
: The frequency to extract the samples in.

@return
: The samples associated with the audio data buffer.

# cainteoir::cosine_window {: .doc }

Create a first-order cosine window function.

@aAlpha
: The value for the alpha parameter of the function.

@aBeta
: The value for the beta parameter of the function.

@aWindowSize
: The number of samples to use in the window.

This calculates the values of a generalized hamming window (first order cosine
window) of the form:

	w(n) = alpha - beta * cos((2 * pi * n) / (N - 1))

where `N` is the window size.

@return
: The values of the window function at each point in the window.

# cainteoir::window {: .doc }

Create a window function by name.

@aName
: The name of the window function

@aWindowSize
: The number of samples to use in the window.

The `aName` parameter specifies the window function to create, which can be:

| Name     | Description |
|----------|-------------|
| hamming  | A Hamming window. |
| hamming0 | A zero-phase Hamming window. |
| hann     | A Hann/Hanning window. |
| hann0    | A zero-phase Hann/Hanning window. |
| hanning  | A Hann/Hanning window. |
| hanning0 | A zero-phase Hann/Hanning window. |

@return
: The values of the window function at each point in the window.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
