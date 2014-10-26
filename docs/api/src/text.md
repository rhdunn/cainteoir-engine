# cainteoir::tts::event_type {: .doc }

The matched context for the [text_event](^^cainteoir::tts::text_event) object.

# cainteoir::tts::error {: .doc }

The unicode codepoints are invalid or not recognised.

# cainteoir::tts::word_uppercase {: .doc }

A sequence of upper-case letters.

# cainteoir::tts::word_lowercase {: .doc }

A sequence of lower-case letters.

# cainteoir::tts::word_capitalized {: .doc }

An upper-case or title-case letter followed by a sequence of lower-case letters.

# cainteoir::tts::word_mixedcase {: .doc }

A sequence of lower-case and upper-case letters.

# cainteoir::tts::word_script {: .doc }

A sequence of CJK or similar script characters.

# cainteoir::tts::number {: .doc }

A sequence of number characters.

# cainteoir::tts::ordinal_number {: .doc }

An ordinal number such as 1st, 2nd or 3rd.

# cainteoir::tts::punctuation {: .doc }

A generic punctuation character.

# cainteoir::tts::comma {: .doc }

A comma punctuation character.

# cainteoir::tts::semicolon {: .doc }

A semicolon punctuation character.

# cainteoir::tts::colon {: .doc }

A colon punctuation character.

# cainteoir::tts::ellipsis {: .doc }

An ellipsis punctuation character.

# cainteoir::tts::full_stop {: .doc }

A full stop (period) punctuation character.

# cainteoir::tts::double_stop {: .doc }

A sequence of two full stop punctuation characters.

# cainteoir::tts::exclamation {: .doc }

An exclamation mark punctuation character.

# cainteoir::tts::question {: .doc }

A question mark punctuation character.

# cainteoir::tts::symbol {: .doc }

A generic symbol character.

# cainteoir::tts::paragraph {: .doc }

A separator between two paragraphs.

# cainteoir::tts::en_dash {: .doc }

An en-dash punctuation character.

# cainteoir::tts::em_dash {: .doc }

An em-dash punctuation character.

# cainteoir::tts::text_event {: .doc }

Information about a text block match.

# cainteoir::tts::text_event::text {: .doc }

The Unicode codepoints associated with the event.

# cainteoir::tts::text_event::phonemes {: .doc }

The sequence of phonemes associated with the event.

# cainteoir::tts::text_event::type {: .doc }

The classification of the event.

# cainteoir::tts::text_event::range {: .doc }

The start and end character of the event.

The start and end values are the number of Unicode codepoints from the start of
the document to the relevant position.

# cainteoir::tts::text_event::codepoint {: .doc }

The Unicode codepoint of the first character in the event.

# cainteoir::tts::text_event::text_event {: .doc }

Create a text event object.

@aText
: The content of the event.

@aType
: The classification of the event.

@aRange
: The start and end Unicode codepoint of the event.

@aCodePoint
: The first Unicode codepoint in the event.

# cainteoir::tts::text_event::text_event {: .doc }

Create a text event object.

@aType
: The classification of the event.

@aRange
: The start and end Unicode codepoint of the event.

@aCodePoint
: The first Unicode codepoint in the event.

# cainteoir::tts::text_event::text_event {: .doc }

Create an empty text event object.

# cainteoir::tts::text_reader {: .doc }

Parse text from a document into a sequence of text events.

# cainteoir::tts::text_reader::event {: .doc }

Get the current text event.

@return
: The current text event.

# cainteoir::tts::text_reader::read {: .doc }

Get the next text event.

@return
: `true` if the next event was read, `false` if there are no more text events.

# cainteoir::tts::create_text_reader {: .doc }

Create a text reader object.

@aReader
: The document to generate text events from.

@return
: The new text reader object.

# cainteoir::tts::context_analysis {: .doc }

Create a context analysis object.

@return
: The context analysis object.

# cainteoir::tts::number_scale {: .doc }

How to pronounce large numbers.

# cainteoir::tts::short_scale {: .doc }

The short number scale (`10^6` = million, `10^9` = billion, `10^12` = trillion, etc).

# cainteoir::tts::long_scale {: .doc }

The long number scale (`10^6` = million, `10^9` = milliard, `10^12` = billion, etc).

# cainteoir::tts::numbers_to_words {: .doc }

Create a number to word converter.

@aLocale
: The language used to convert numbers, etc. into words.

@aScale
: The number scale system to use.

@return
: The number to word converter.

# cainteoir::tts::words_to_phonemes {: .doc }

Create a phoneme stream object.

@aRules
: The letter to phoneme rules to use.

@aExceptionDictionary
: Pronunciation of words that are mispronounced by `aRules`.

@return
: The phoneme stream object.

# cainteoir::tts::generate_phonemes {: .doc }

Write the phonemes to the specified file.

@reader
: The source of the phonetic transcription.

@out
: The file to write the transcription to.

@phonemeset
: The phonemeset to transcribe phonemes as.

@stress
: The placement of stress marks in the phonetic transcription.

@open
: The characters to use to indicate the start of a phoneme sequence.

@close
: The characters to use to indicate the end of a phoneme sequence.

@phrase
: The characters used to separate phrases (full stops, commas, etc.).

This is used to write out the pronunciation from the [text_reader](^^cainteoir::tts::text_reader)
object in the given `phonemeset` transcription, while preserving the punctuation.
The punctuation is preserved to allow other TTS engines like eSpeak to produce the
correct pauses and other prosodic elements.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
