# cainteoir::tts::phoneme_error
{: .doc }

Indicates an error when reading/writing phonetic transcriptions.

# cainteoir::tts::phoneme_error::phoneme_error
{: .doc }

Create an instance of the exception.

@msg
: The error message associated with this exception.

# cainteoir::tts::phoneme_parser
{: .doc }

Parse phonemes in a phonetic transcriptions.

# cainteoir::tts::phoneme_parser::parse
{: .doc }

Read the next entry in the content buffer.

@aCurrent
: The current position in the content buffer.

@aEnd
: The end of the content buffer.

@aPhoneme
: The phoneme that was parsed.

@return
: `true` if an entry was read, `false` if there are no more entries in the content buffer.

# cainteoir::tts::phoneme_parser::~phoneme_parser
{: .doc }

Clean up the phoneme parser.

# cainteoir::tts::createPhonemeParser
{: .doc }

Create a phoneme parser using the specified transcription scheme.

@aPhonemeSet
: The name of the phoneme set (transcription scheme) to load.

Transcription schemes other than `features` are loaded from the `.phon`
file with the same name in the `phonemeset` sub-directory of the
cainteoir-engine data directory.

# cainteoir::tts::phoneme_reader
{: .doc }

Support reading phonemes in a given transcription scheme.

For example:

	auto scheme = cainteoir::tts::createPhonemeReader("features");
	scheme->reset(std::make_shared<cainteoir::buffer>("{vcd,alv,stp}"));
	while (scheme->read())
	{
		cainteoir::tts::phoneme p = *scheme;
		// Do something with `p`.
	}

# cainteoir::tts::phoneme_reader::~phoneme_reader
{: .doc }

Cleans up the data associated with the phoneme reader.

# cainteoir::tts::phoneme_reader::reset
{: .doc }

Read phonemes from the specified data buffer.

@aBuffer
: The text buffer containing the phonemes to read from.

This will reset the phoneme reader so that it will read phonemes from the new
data buffer. It will lose any state from reading the current buffer.

# cainteoir::tts::phoneme_reader::read
{: .doc }

Read the next phoneme.

@return
: `true` if a phoneme was read, `false` otherwise.

# cainteoir::tts::createPhonemeReader
{: .doc }

Create a phoneme reader using the specified transcription scheme.

@aPhonemeSet
: The name of the phoneme set (transcription scheme) to load.

Transcription schemes other than `features` are loaded from the `.phon`
file with the same name in the `phonemeset` sub-directory of the
cainteoir-engine data directory.

# cainteoir::tts::phoneme_writer
{: .doc }

Support writing phonemes in a given transcription scheme.

	auto scheme = cainteoir::tts::createPhonemeWriter("features");
	scheme->reset(stdout);
	for (auto phoneme : { ... })
		scheme->write(phoneme);

# cainteoir::tts::phoneme_writer::~phoneme_writer
{: .doc }

Cleans up the data associated with the phoneme writer.

# cainteoir::tts::phoneme_writer::reset
{: .doc }

Set the file object to write the phonemes to.

@aOutput
: The file object to write the phonemes to.

# cainteoir::tts::phoneme_writer::write
{: .doc }

Writes the phoneme to the selected file.

@aPhoneme
: The phoneme to write.

@return
: `true` if the phoneme exists in the transcription scheme and was written to
  the file, `false` if the phoneme does not exist in this transcription scheme.

# cainteoir::tts::phoneme_writer::name
{: .doc }

Get the name of the phonemeset the writer transcribes phonemes as.

@return The name of the phonemeset associated with the writer.

# cainteoir::tts::createPhonemeWriter
{: .doc }

Create a phoneme writer using the specified transcription scheme.
@aPhonemeSet The name of the phoneme set (transcription scheme) to load.

Transcription schemes other than `features` are loaded from the `.phon`
file with the same name in the `phonemeset` sub-directory of the
cainteoir-engine data directory.

# cainteoir::tts::stress_type
{: .doc }

Where stress features should be placed.

# cainteoir::tts::stress_type::as_transcribed
{: .doc }

The stress features should not be modified.

# cainteoir::tts::stress_type::vowel
{: .doc }

The stress features should be placed on stressed vowels.

# cainteoir::tts::stress_type::syllable
{: .doc }

The stress features should be placed on syllable boundaries.

# cainteoir::tts::make_stressed
{: .doc }

Adjust the stress feature placement.

@aPhonemes
: The phoneme sequence to adjust the stress feature placement of.

@aType
: The stress placement to adjust to.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
