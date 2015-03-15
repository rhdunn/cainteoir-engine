# cainteoir::tts::envelope_t
{: .doc }

A point in a pitch envelope.

# cainteoir::tts::envelope_t::offset
{: .doc }

The percentage offset.

# cainteoir::tts::envelope_t::pitch
{: .doc }

The pitch at the given offset.

# cainteoir::tts::prosody
{: .doc }

The prosody information for a given phoneme.

# cainteoir::tts::prosody::first
{: .doc }

The phoneme the prosodic information is to be applied to.

# cainteoir::tts::prosody::second
{: .doc }

The phoneme the prosodic information is to be applied to.

This phoneme is used to represent the second phoneme in a diphone.

# cainteoir::tts::prosody::envelope
{: .doc }

The pitch envelope of the phoneme.

# cainteoir::tts::prosody_reader
{: .doc }

Read prosody data from a data buffer.

# cainteoir::tts::prosody_reader::~prosody_reader
{: .doc }

Clean up the prosody reader object.

# cainteoir::tts::prosody_reader::read
{: .doc }

Read the next prosodic entry.

@return
: `true` if a prosodic entry was read, `false` otherwise.

# cainteoir::tts::createPhoReader
{: .doc }

Create an MBROLA pho file reader.

@aPhonemeSet
: The phoneme set (transcription scheme) used to read the phonemes in the pho file.

@return
: A prosody reader that can read MBROLA pho file entries.

# cainteoir::tts::createDiphoneReader
{: .doc }

Create a phoneme to diphone converter.

@aProsody
: The prosody data for the phonemes to convert.

@return
: A prosody reader that creates the diphone sequences from `aProsody`.

# cainteoir::tts::prosody_writer
{: .doc }

Write prosody data to a file.

# cainteoir::tts::prosody_writer::~prosody_writer
{: .doc }

Cleans up the data associated with the prosody writer.

# cainteoir::tts::prosody_writer::reset
{: .doc }

Set the file object to write the prosodic entries to.

@aOutput
: The file object to write the prosodic entries to.

# cainteoir::tts::prosody_writer::write
{: .doc }

Writes the prosodic entries to the selected file.

@aProsody
: The prosodic entries to write.

@return
: `true` if the prosodic entry was written to the file, `false` otherwise.

# cainteoir::tts::createPhoWriter
{: .doc }

Create an MBROLA pho file writer.

@aPhonemeSet
: The phoneme set (transcription scheme) used to write the phonemes in the pho file.

@return
: A prosody writer that can write MBROLA pho file entries.

# cainteoir::tts::synthesizer
{: .doc }

Manages a phoneme to audio synthesizer.

# cainteoir::tts::synthesizer::bind
{: .doc }

Associate the synthesizer with the phonemes to be synthesized.

@aProsody
: The phonemes and associated prosody.

# cainteoir::tts::synthesizer::synthesize
{: .doc }

Synthesize the pending phonemes to audio.

@out
: The audio file or device to send the audio data to.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
