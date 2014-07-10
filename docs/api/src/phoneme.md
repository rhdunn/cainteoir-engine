#<cldoc:cainteoir::tts::phoneme_error>

Indicates an error when reading/writing phonetic transcriptions.

#<cldoc:cainteoir::tts::phoneme_error::phoneme_error>

Create an instance of the exception.
@msg The error message associated with this exception.

#<cldoc:cainteoir::tts::phoneme_parser>

Parse phonemes in a phonetic transcriptions.

#<cldoc:cainteoir::tts::phoneme_parser::parse>

Read the next entry in the content buffer.
@aCurrent The current position in the content buffer.
@aEnd     The end of the content buffer.
@aPhoneme The phoneme that was parsed.

@return `true` if an entry was read, `false` if there are no more entries in the content buffer.

#<cldoc:cainteoir::tts::phoneme_parser::~phoneme_parser>

Clean up the phoneme parser.

#<cldoc:cainteoir::tts::createPhonemeParser>

Create a phoneme parser using the specified transcription scheme.
@aPhonemeSet The name of the phoneme set (transcription scheme) to load.

Transcription schemes other than `features` are loaded from the `.phon`
file with the same name in the \c phonemeset sub-directory of the
cainteoir-engine data directory.

#<cldoc:cainteoir::tts::phoneme_reader>

Support reading phonemes in a given transcription scheme.

For example:

	auto scheme = cainteoir::tts::createPhonemeReader("features");
	scheme->reset(std::make_shared<cainteoir::buffer>("{vcd,alv,stp}"));
	while (scheme->read())
	{
		cainteoir::tts::phoneme p = *scheme;
		// Do something with `p`.
	}

#<cldoc:cainteoir::tts::phoneme_reader::~phoneme_reader>

Cleans up the data associated with the phoneme reader.

#<cldoc:cainteoir::tts::phoneme_reader::reset>

Read phonemes from the specified data buffer.
@aBuffer The text buffer containing the phonemes to read from.

This will reset the phoneme reader so that it will read phonemes from the new
data buffer. It will lose any state from reading the current buffer.

#<cldoc:cainteoir::tts::phoneme_reader::read>

Read the next phoneme.

@return `true` if a phoneme was read, `false` otherwise.

#<cldoc:cainteoir::tts::createPhonemeReader>

Create a phoneme reader using the specified transcription scheme.
@aPhonemeSet The name of the phoneme set (transcription scheme) to load.

Transcription schemes other than `features` are loaded from the `.phon`
file with the same name in the \c phonemeset sub-directory of the
cainteoir-engine data directory.

#<cldoc:cainteoir::tts::phoneme_writer>

Support writing phonemes in a given transcription scheme.

	auto scheme = cainteoir::tts::createPhonemeWriter("features");
	scheme->reset(stdout);
	for (auto phoneme : { ... })
		scheme->write(phoneme);

#<cldoc:cainteoir::tts::phoneme_writer::~phoneme_writer>

Cleans up the data associated with the phoneme writer.

#<cldoc:cainteoir::tts::phoneme_writer::reset>

Set the file object to write the phonemes to.
@aOutput The file object to write the phonemes to.

#<cldoc:cainteoir::tts::phoneme_writer::write>

Writes the phoneme to the selected file.
@aPhoneme The phoneme to write.

'return `true` if the phoneme exists in the transcription scheme and was written to the file,
        `false` if the phoneme does not exist in this transcription scheme.

#<cldoc:cainteoir::tts::phoneme_writer::name>

Get the name of the phonemeset the writer transcribes phonemes as.

@return The name of the phonemeset associated with the writer.

#<cldoc:cainteoir::tts::createPhonemeWriter>

Create a phoneme writer using the specified transcription scheme.
@aPhonemeSet The name of the phoneme set (transcription scheme) to load.

Transcription schemes other than `features` are loaded from the `.phon`
file with the same name in the \c phonemeset sub-directory of the
cainteoir-engine data directory.

#<cldoc:cainteoir::tts::stress_type>

Where stress features should be placed.

#<cldoc:cainteoir::tts::stress_type::as_transcribed>

The stress features should not be modified.

#<cldoc:cainteoir::tts::stress_type::vowel>

The stress features should be placed on stressed vowels.

#<cldoc:cainteoir::tts::stress_type::syllable>

The stress features should be placed on syllable boundaries.

#<cldoc:cainteoir::tts::make_stressed>

Adjust the stress feature placement.
@aPhonemes The phoneme sequence to adjust the stress feature placement of.
@aType     The stress placement to adjust to.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
