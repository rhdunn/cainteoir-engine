#<cldoc:cainteoir::tts::dictionary::entry_type>

The type of the associated dictionary entry.

#<cldoc:cainteoir::tts::dictionary::no_match>

There is no matching entry for the word in the dictionary.

#<cldoc:cainteoir::tts::dictionary::say_as>

The entry is pronounced as another entry in the dictionary.

#<cldoc:cainteoir::tts::dictionary::phonemes>

The entry is pronounced by the associated sequence of phonemes.

#<cldoc:cainteoir::tts::dictionary::entry>

Provides information about a dictionary entry.

#<cldoc:cainteoir::tts::dictionary::entry::type>

The type of the dictionary entry.

#<cldoc:cainteoir::tts::dictionary::entry::text>

The word to lookup for a <cainteoir::tts::dictionary::say_as> entry.

#<cldoc:cainteoir::tts::dictionary::entry::phonemes>

The phonemes for a <cainteoir::tts::dictionary::phonemes> entry.

#<cldoc:cainteoir::tts::dictionary::entry::entry>

Create a <cainteoir::tts::dictionary::no_match> entry.

#<cldoc:cainteoir::tts::dictionary::entry::entry>

Create a <cainteoir::tts::dictionary::say_as> entry.
@aSayAs The text of the word to pronounce this entry as.

#<cainteoir::tts::dictionary::entry::entry>

Create a cainteoir::tts::dictionary::phonemes entry.
@aPhonemes   The pronunciation of the entry as a phonetic transciption in the given phonemeset.
@aPhonemeSet The phonemeset used to read `aPhonemes`.

#<cldoc:cainteoir::tts::dictionary::key_hash>

Generate the hash of a <cainteoir::tts::dictionary::key_type> object.

#<cldoc:cainteoir::tts::dictionary::key_hash::operator()>

Generate the hash of a <cainteoir::tts::dictionary::key_type> object.
@a The <cainteoir::tts::dictionary::key_type> object to hash.

@return The hash of the <cainteoir::tts::dictionary::key_type> object.

#<cldoc:cainteoir::tts::dictionary::key_equals>

Compare two <cainteoir::tts::dictionary::key_type> objects for equality.

#<cldoc:cainteoir::tts::dictionary::key_equals::operator()>

Compare two <cainteoir::tts::dictionary::key_type> objects for equality.
@a The first key value to compare.
@b The second key value to compare.

@return `true` if the key values are equal, `false` otherwise.

#<cldoc:cainteoir::tts::dictionary>

Manages a pronunciation dictionary.

#<cldoc:cainteoir::tts::dictionary::key_type>

The index type of entries in the dictionary.

#<cldoc:cainteoir::tts::dictionary::storage_type>

The type used to store the entries in the dictionary.

#<cldoc:cainteoir::tts::dictionary::const_iterator>

Iterate over entries in the dictionary.

#<cldoc:cainteoir::tts::dictionary::add_entry>

Add or update a dictionary entry.
@aWord  The word to add to the dictionary.
@aEntry The pronunciation information for the word to be added.

#<cldoc:cainteoir::tts::dictionary::lookup>

Get the pronunciation entry associated with the word.
@aWord The word to lookup.

If `aWord` does not exist in the dictionary, the returned entry has the type of
<cainteoir::tts::dictionary::no_match>.

@return The pronunciation entry associated with the word.

#<cldoc:cainteoir::tts::dictionary::size>

Get the number of entries in the dictionary.

@return The number of entries in the dictionary.

#<cldoc:cainteoir::tts::dictionary::empty>

Is the dictionary true  If the dictionary is empty.

@return `true` if the dictionary is empty, `false` otherwise.

#<cldoc:cainteoir::tts::dictionary::begin>

Get an iterator to the first entry in the dictionary.

@return An iterator to the first entry in the dictionary.

#<cldoc:cainteoir::tts::dictionary::end>

Get an iterator to one past the last entry in the dictionary.

@return An iterator to one past the last entry in the dictionary.

#<cldoc:cainteoir::tts::dictionary::pronounce>

Get the pronunciation for the word in the dictionary.
@aWord               The word to get the pronunciation for.
@aPronunciationRules The letter-to-phoneme rules to use if the word cannot be pronounced.
@aPhonemes           The list of phonemes that make up the pronunciation of the word.

@return `true` if the word was pronounced, `false` otherwise.

#<cldoc:cainteoir::tts::dictionary_reader>

Parse a pronunciation dictionary file.

#<cldoc:cainteoir::tts::dictionary_reader::~dictionary_reader>

Clean up the dictionary reader object.

#<cldoc:cainteoir::tts::dictionary_reader::read>

Read the next entry in the dictionary file.

@return `true` if the next entry was read from the file, `false` otherwise.

#<cldoc:cainteoir::tts::dictionary_reader::word>

The word of the current entry.

#<cldoc:cainteoir::tts::dictionary_reader::entry>

The pronunciation information for the current entry.

#<cldoc:cainteoir::tts::dictionary_formatter>

Format dictionary entries in a given file format.

#<cldoc:cainteoir::tts::dictionary_formatter::~dictionary_formatter>

Clean up the dictionary formatter object.

#<cldoc:cainteoir::tts::dictionary_formatter::write_phoneme_entry>

Write a phoneme entry in the given phonemeset.
@word           The word of the entry being written.
@writer         The phonemeset to transcribe the phonemes in.
@phonemes       The pronunciation of `word` being written.
@line_separator The character(s) to use at the end of each line.

#<cldoc:cainteoir::tts::dictionary_formatter::write_say_as_entry>

Write a say as entry.
@word           The word of the entry being written.
@say_as         The word to pronounce `word` as.
@line_separator The character(s) to use at the end of each line.

#<cldoc:cainteoir::tts::createCainteoirDictionaryReader>

Read entries from a Cainteoir Dictionary file.
@aDictionaryPath The location of the dictionary to read.

@return A <cainteoir::tts::dictionary_reader> object to read the entries in the dictionary file.

#<cldoc:cainteoir::tts::createCainteoirDictionaryFormatter>

Create a formatter to output Cainteoir Dictionary formatted dictionary entries.
@out The file to write the dictionary to.

@return The formatter to write dictionary entries to.

#<cldoc:cainteoir::tts::createEspeakDictionaryFormatter>

Create a formatter to output eSpeak formatted dictionary entries.
@out The file to write the dictionary to.

@return The formatter to write dictionary entries to.

#<cldoc:cainteoir::tts::createDictionaryEntryFormatter>

Create a formatter to output dictionary entries to.
@out The file to write the dictionary to.

This outputs entries in the form:

	"word" => /phonemes/ [phonemeset]
	"word" => "say-as"@Script [say-as]

@return The formatter to write dictionary entries to.

#<cldoc:cainteoir::tts::formatDictionary>

Serialize the entries in the dictionary.
@dict                   The dictionary containing the entries to serialize.
@formatter              The format to serialize the entries in.
@writer                 The phonemeset to transcribe pronunciation entries as.
@resolve_say_as_entries Should say-as entries be mapped to their associated phoneme entry?

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
