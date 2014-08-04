#<cldoc:cainteoir::tts::envelope_t>

A point in a pitch envelope.

#<cldoc:cainteoir::tts::envelope_t::offset>

The percentage offset.

#<cldoc:cainteoir::tts::envelope_t::pitch>

The pitch at the given offset.

#<cldoc:cainteoir::tts::prosody>

The prosody information for a given phoneme.

#<cldoc:cainteoir::tts::prosody::phoneme1>

The phoneme the prosodic information is to be applied to.

#<cldoc:cainteoir::tts::prosody::phoneme2>

The phoneme the prosodic information is to be applied to.

This phoneme is used to represent diphthongs and affricates.

#<cldoc:cainteoir::tts::prosody::phoneme3>

The phoneme the prosodic information is to be applied to.

This phoneme is used to represent the second phoneme in a diphone.

#<cldoc:cainteoir::tts::prosody::phoneme4>

The phoneme the prosodic information is to be applied to.

This phoneme is used to represent the second phoneme in a diphone for
diphthongs and affricates.

#<cldoc:cainteoir::tts::prosody::duration>

The length of the phoneme.

#<cldoc:cainteoir::tts::prosody::envelope>

The pitch envelope of the phoneme.

#<cldoc:cainteoir::tts::prosody_reader>

Read prosody data from a data buffer.

#<cldoc:cainteoir::tts::prosody_reader::~prosody_reader>

Clean up the prosody reader object.

#<cldoc:cainteoir::tts::prosody_reader::reset>

Read a prosodic entry from the specified data buffer.
@aBuffer The text buffer containing the phonemes to read from.

This will reset the prosody reader so that it will read prosodic entries from the new
data buffer. It will lose any state from reading the current buffer.

#<cldoc:cainteoir::tts::prosody_reader::read>

Read the next prosodic entry.

@return `true` if a prosodic entry was read, `false` otherwise.

#<cldoc:cainteoir::tts::createPhoReader>

Create an MBROLA pho file reader.
@aPhonemeSet The phoneme set (transcription scheme) used to read the phonemes in the pho file.

@return A prosody reader that can read MBROLA pho file entries.

#<cldoc:cainteoir::tts::createDiphoneReader>

Create a phoneme to diphone converter.
@aProsody The prosody data for the phonemes to convert.

@return A prosody reader that creates the diphone sequences from <aProsody>.

#<cldoc:cainteoir::tts::prosody_writer>

Write prosody data to a file.

#<cldoc:cainteoir::tts::prosody_writer::~prosody_writer>

Cleans up the data associated with the prosody writer.

#<cldoc:cainteoir::tts::prosody_writer::reset>

Set the file object to write the prosodic entries to.
@aOutput The file object to write the prosodic entries to.

#<cldoc:cainteoir::tts::prosody_writer::write>

Writes the prosodic entries to the selected file.
@aProsody The prosodic entries to write.

@return `true` if the prosodic entry was written to the file, `false` otherwise.

#<cldoc:cainteoir::tts::createPhoWriter>

Create an MBROLA pho file writer.
@aPhonemeSet The phoneme set (transcription scheme) used to write the phonemes in the pho file.

@return A prosody writer that can write MBROLA pho file entries.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
