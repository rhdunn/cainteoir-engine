# cainteoir::tts::event_type
{: .doc }

The matched context for the [text_event](^^cainteoir::tts::text_event) object.

# cainteoir::tts::error
{: .doc }

The unicode codepoints are invalid or not recognised.

# cainteoir::tts::word_uppercase
{: .doc }

A sequence of upper-case letters.

# cainteoir::tts::word_lowercase
{: .doc }

A sequence of lower-case letters.

# cainteoir::tts::word_capitalized
{: .doc }

An upper-case or title-case letter followed by a sequence of lower-case letters.

# cainteoir::tts::word_mixedcase
{: .doc }

A sequence of lower-case and upper-case letters.

# cainteoir::tts::word_script
{: .doc }

A sequence of CJK or similar script characters.

# cainteoir::tts::number
{: .doc }

A sequence of number characters.

# cainteoir::tts::ordinal_number
{: .doc }

An ordinal number such as 1st, 2nd or 3rd.

# cainteoir::tts::punctuation
{: .doc }

A generic punctuation character.

# cainteoir::tts::symbol
{: .doc }

A generic symbol character.

# cainteoir::tts::paragraph
{: .doc }

A separator between two paragraphs.

# cainteoir::tts::en_dash
{: .doc }

An en-dash punctuation character.

# cainteoir::tts::text_reader
{: .doc }

Parse text from a document into a sequence of text events.

# cainteoir::tts::text_reader::event
{: .doc }

Get the current text event.

@return
: The current text event.

# cainteoir::tts::text_reader::read
{: .doc }

Get the next text event.

@return
: `true` if the next event was read, `false` if there are no more text events.

# cainteoir::tts::create_text_reader
{: .doc }

Create a text reader object.

@aReader
: The document to generate text events from.

@return
: The new text reader object.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
