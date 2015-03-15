# cainteoir::tts::dictionary::entry_type
{: .doc }

The type of the associated dictionary entry.

# cainteoir::tts::dictionary::no_match
{: .doc }

There is no matching entry for the word in the dictionary.

# cainteoir::tts::dictionary::say_as
{: .doc }

The entry is pronounced as another entry in the dictionary.

# cainteoir::tts::dictionary::phonemes
{: .doc }

The entry is pronounced by the associated sequence of phonemes.

# cainteoir::tts::dictionary::entry
{: .doc }

Provides information about a dictionary entry.

# cainteoir::tts::dictionary::entry::type
{: .doc }

The type of the dictionary entry.

# cainteoir::tts::dictionary::entry::text
{: .doc }

The word to lookup for a [say_as](^^cainteoir::tts::dictionary::say_as) entry.

# cainteoir::tts::dictionary::entry::phonemes
{: .doc }

The phonemes for a [phonemes](^^cainteoir::tts::dictionary::phonemes) entry.

# cainteoir::tts::dictionary::entry::entry
{: .doc }

Create a [no_match](^^cainteoir::tts::dictionary::no_match) entry.

# cainteoir::tts::dictionary::entry::entry
{: .doc }

Create a [say_as](^^cainteoir::tts::dictionary::say_as) entry.

@aSayAs
: The text of the word to pronounce this entry as.

# cainteoir::tts::dictionary::entry::entry
{: .doc }

Create a cainteoir::tts::dictionary::phonemes entry.

@aPhonemes
: The pronunciation of the entry as a phonetic transciption in the given phonemeset.

@aPhonemeSet
: The phonemeset used to read `aPhonemes`.

# cainteoir::tts::dictionary::key_hash
{: .doc }

Generate the hash of a [key_type](^^cainteoir::tts::dictionary::key_type) object.

# cainteoir::tts::dictionary::key_hash::operator()
{: .doc }

Generate the hash of a [key_type](^^cainteoir::tts::dictionary::key_type) object.

@a
: The [key_type](^^cainteoir::tts::dictionary::key_type) object to hash.

@return
: The hash of the [key_type](^^cainteoir::tts::dictionary::key_type) object.

# cainteoir::tts::dictionary::key_equals
{: .doc }

Compare two [key_type](^^cainteoir::tts::dictionary::key_type) objects for equality.

# cainteoir::tts::dictionary::key_equals::operator()
{: .doc }

Compare two [key_type](^^cainteoir::tts::dictionary::key_type) objects for equality.

@a
: The first key value to compare.

@b
: The second key value to compare.

@return
: `true` if the key values are equal, `false` otherwise.

# cainteoir::tts::dictionary
{: .doc }

Manages a pronunciation dictionary.

# cainteoir::tts::dictionary::key_type
{: .doc }

The index type of entries in the dictionary.

# cainteoir::tts::dictionary::storage_type
{: .doc }

The type used to store the entries in the dictionary.

# cainteoir::tts::dictionary::const_iterator
{: .doc }

Iterate over entries in the dictionary.

# cainteoir::tts::dictionary::add_entry
{: .doc }

Add or update a dictionary entry.

@aWord
: The word to add to the dictionary.

@aEntry
: The pronunciation information for the word to be added.

# cainteoir::tts::dictionary::lookup
{: .doc }

Get the pronunciation entry associated with the word.

@aWord
: The word to lookup.

If `aWord` does not exist in the dictionary, the returned entry type is
[no_match](^^cainteoir::tts::dictionary::no_match).

@return
: The pronunciation entry associated with the word.

# cainteoir::tts::dictionary::size
{: .doc }

Get the number of entries in the dictionary.

@return
: The number of entries in the dictionary.

# cainteoir::tts::dictionary::empty
{: .doc }

Is the dictionary true  If the dictionary is empty.

@return
: `true` if the dictionary is empty, `false` otherwise.

# cainteoir::tts::dictionary::begin
{: .doc }

Get an iterator to the first entry in the dictionary.

@return
: An iterator to the first entry in the dictionary.

# cainteoir::tts::dictionary::end
{: .doc }

Get an iterator to one past the last entry in the dictionary.

@return
: An iterator to one past the last entry in the dictionary.

# cainteoir::tts::dictionary::pronounce
{: .doc }

Get the pronunciation for the word in the dictionary.

@aWord
: The word to get the pronunciation for.

@aPronunciationRules
: The letter-to-phoneme rules to use if the word cannot be pronounced.

@aPhonemes
: The list of phonemes that make up the pronunciation of the word.

@return
: `true` if the word was pronounced, `false` otherwise.

# cainteoir::tts::dictionary_reader
{: .doc }

Parse a pronunciation dictionary file.

# cainteoir::tts::dictionary_reader::~dictionary_reader
{: .doc }

Clean up the dictionary reader object.

# cainteoir::tts::dictionary_reader::read
{: .doc }

Read the next entry in the dictionary file.

@return
: `true` if the next entry was read from the file, `false` otherwise.

# cainteoir::tts::dictionary_reader::word
{: .doc }

The word of the current entry.

# cainteoir::tts::dictionary_reader::entry
{: .doc }

The pronunciation information for the current entry.

# cainteoir::tts::dictionary_formatter
{: .doc }

Format dictionary entries in a given file format.

# cainteoir::tts::dictionary_formatter::~dictionary_formatter
{: .doc }

Clean up the dictionary formatter object.

# cainteoir::tts::dictionary_formatter::write_phoneme_entry
{: .doc }

Write a phoneme entry in the given phonemeset.

@word
: The word of the entry being written.

@writer
: The phonemeset to transcribe the phonemes in.

@phonemes
: The pronunciation of `word` being written.

@line_separator
: The character(s) to use at the end of each line.

# cainteoir::tts::dictionary_formatter::write_say_as_entry
{: .doc }

Write a say as entry.

@word
: The word of the entry being written.

@say_as
: The word to pronounce `word` as.

@line_separator
: The character(s) to use at the end of each line.

# cainteoir::tts::createCainteoirDictionaryReader
{: .doc }

Read entries from a Cainteoir Dictionary file.

@aDictionaryPath
: The location of the dictionary to read.

@return
: A [dictionary_reader](^^cainteoir::tts::dictionary_reader) object to read
  the entries in the dictionary file.

# cainteoir::tts::createCainteoirDictionaryFormatter
{: .doc }

Create a formatter to output Cainteoir Dictionary formatted dictionary entries.
@out The file to write the dictionary to.

@return The formatter to write dictionary entries to.

# cainteoir::tts::createEspeakDictionaryFormatter
{: .doc }

Create a formatter to output eSpeak formatted dictionary entries.

@out
: The file to write the dictionary to.

@return
: The formatter to write dictionary entries to.

# cainteoir::tts::createDictionaryEntryFormatter
{: .doc }

Create a formatter to output dictionary entries to.

@out
: The file to write the dictionary to.

This outputs entries in the form:

	"word" => /phonemes/ [phonemeset]
	"word" => "say-as"@Script [say-as]

@return The formatter to write dictionary entries to.

# cainteoir::tts::formatDictionary
{: .doc }

Serialize the entries in the dictionary.

@dict
: The dictionary containing the entries to serialize.

@formatter
: The format to serialize the entries in.

@writer
: The phonemeset to transcribe pronunciation entries as.

@resolve_say_as_entries
: Should say-as entries be mapped to their associated phoneme entry?

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
