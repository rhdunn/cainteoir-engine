#<cldoc:cainteoir::tts::createPronunciationRules>

Create a letter to phoneme converter.
@aRuleSetPath The path of the letter to phoneme rules to use.

@return A letter to phoneme converter object.

#<cldoc:cainteoir::tts::event_type>

The matched context for the <text_event> object.

#<cldoc:cainteoir::tts::event_type::error>

The unicode codepoints are invalid or not recognised.

#<cldoc:cainteoir::tts::event_type::word_uppercase>

A sequence of upper-case letters.

#<cldoc:cainteoir::tts::event_type::word_lowercase>

A sequence of lower-case letters.

#<cldoc:cainteoir::tts::event_type::word_capitalized>

An upper-case or title-case letter followed by a sequence of lower-case letters.

#<cldoc:cainteoir::tts::event_type::word_mixedcase>

A sequence of lower-case and upper-case letters.

#<cldoc:cainteoir::tts::event_type::word_script>

A sequence of CJK or similar script characters.

#<cldoc:cainteoir::tts::event_type::number>

A sequence of number characters.

#<cldoc:cainteoir::tts::event_type::ordinal_number>

An ordinal number such as 1st, 2nd or 3rd.

#<cldoc:cainteoir::tts::event_type::punctuation>

A generic punctuation character.

#<cldoc:cainteoir::tts::event_type::comma>

A comma punctuation character.

#<cldoc:cainteoir::tts::event_type::semicolon>

A semicolon punctuation character.

#<cldoc:cainteoir::tts::event_type::colon>

A colon punctuation character.

#<cldoc:cainteoir::tts::event_type::ellipsis>

An ellipsis punctuation character.

#<cldoc:cainteoir::tts::event_type::full_stop>

A full stop (period) punctuation character.

#<cldoc:cainteoir::tts::event_type::double_stop>

A sequence of two full stop punctuation characters.

#<cldoc:cainteoir::tts::event_type::exclamation>

An exclamation mark punctuation character.

#<cldoc:cainteoir::tts::event_type::question>

A question mark punctuation character.

#<cldoc:cainteoir::tts::event_type::symbol>

A generic symbol character.

#<cldoc:cainteoir::tts::event_type::paragraph>

A separator between two paragraphs.

#<cldoc:cainteoir::tts::event_type::en_dash>

An en-dash punctuation character.

#<cldoc:cainteoir::tts::event_type::em_dash>

An em-dash punctuation character.

#<cldoc:cainteoir::tts::text_event>

Information about a text block match.

#<cldoc:cainteoir::tts::text_event::text>

The Unicode codepoints associated with the event.

#<cldoc:cainteoir::tts::text_event::phonemes>

The sequence of phonemes associated with the event.

#<cldoc:cainteoir::tts::text_event::type>

The classification of the event.

#<cldoc:cainteoir::tts::text_event::range>

The start and end character of the event.

The start and end values are the number of Unicode codepoints from the start of
the document to the relevant position.

#<cldoc:cainteoir::tts::text_event::codepoint>

The Unicode codepoint of the first character in the event.

#<cldoc:cainteoir::tts::text_event::text_event>

Create a text event object.
@aText      The content of the event.
@aType      The classification of the event.
@aRange     The start and end Unicode codepoint of the event.
@aCodePoint The first Unicode codepoint in the event.

#<cldoc:cainteoir::tts::text_event::text_event>

Create a text event object.
@aType      The classification of the event.
@aRange     The start and end Unicode codepoint of the event.
@aCodePoint The first Unicode codepoint in the event.

#<cldoc:cainteoir::tts::text_event::text_event>

Create an empty text event object.

#<cldoc:cainteoir::tts::text_reader>

Parse text from a document into a sequence of text events.

#<cldoc:cainteoir::tts::text_reader::event>

Get the current text event.

@return The current text event.

#<cldoc:cainteoir::tts::text_reader::read>

Get the next text event.

@return `true` if the next event was read, `false` if there are no more text events.

#<cldoc:cainteoir::tts::create_text_reader>

Create a text reader object.
@aReader The document to generate text events from.

@return The new text reader object.

#<cldoc:cainteoir::tts::context_analysis>

Create a context analysis object.

@return The context analysis object.

#<cldoc:cainteoir::tts::number_scale>

How to pronounce large numbers.

#<cldoc:cainteoir::tts::number_scale::short_scale>

The short number scale (`10^6` = million, `10^9` = billion, `10^12` = trillion, etc).

#<cldoc:cainteoir::tts::number_scale::long_scale>

The long number scale (`10^6` = million, `10^9` = milliard, `10^12` = billion, etc).

#<cldoc:cainteoir::tts::numbers_to_words>

Create a number to word converter.
@aLocale The language used to convert numbers, etc. into words.
@aScale  The number scale system to use.

@return The number to word converter.

#<cldoc:cainteoir::tts::words_to_phonemes>

Create a phoneme stream object.
@aRules               The letter to phoneme rules to use.
@aExceptionDictionary Pronunciation of words that are mispronounced by `aRules`.

@return The phoneme stream object.

#<cldoc:cainteoir::tts::generate_phonemes>

Write the phonemes to the specified file.
@reader     The source of the phonetic transcription.
@out        The file to write the transcription to.
@phonemeset The phonemeset to transcribe phonemes as.
@stress     The placement of stress marks in the phonetic transcription.
@open       The characters to use to indicate the start of a phoneme sequence.
@close      The characters to use to indicate the end of a phoneme sequence.
@phrase     The characters used to separate phrases (full stops, commas, etc.).

This is used to write out the pronunciation from the <phoneme_stream> object,
while preserving the punctuation. The punctuation is preserved to allow other
TTS engines like eSpeak to produce the correct pauses and other prosodic elements.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
