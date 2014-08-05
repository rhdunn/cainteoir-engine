# cainteoir::tts::get_voice_uri {: .doc }

# cainteoir::language::tag {: .doc }

A BCP 47/[RFC 5646](http://www.ietf.org/rfc/rfc5646.txt) language tag.

# cainteoir::language::tag::tag {: .doc }

Create a language tag object.

@l
: The primary language code.
@e
: The extended language code.

@s
: The writing script code.

@r
: The region code.

@v
: The variant code.

@p
: The private-use code.

# cainteoir::language::tag::lang {: .doc }

ISO 639 language code (primary language).

# cainteoir::language::tag::extlang {: .doc }

ISO 639 language code (extended language).

# cainteoir::language::tag::script {: .doc }

ISO 15924 script code.

# cainteoir::language::tag::region {: .doc }

ISO 3166 or UN M.49 region code.

# cainteoir::language::tag::variant {: .doc }

IANA variant subtag.

# cainteoir::language::tag::private_use {: .doc }

An application-specific custom subtag.

# cainteoir::language::make_lang {: .doc }

Extract language tag information from a BCP 47 language id.

@code
: The language identifier, e.g. "es-MX".

@return
: The extracted language, script and country codes.

# cainteoir::language::operator== {: .doc }

Compare two language tags for equality.

@a
: The first language tag to compare.

@b
: The second language tag to compare.

@return
: `true` if the language tags match, `false` otherwise.

# cainteoir::language::operator< {: .doc }

Compare two language tags for ordering.

@a
: The first language tag to compare.

@b
: The second language tag to compare.

@return
: `true` if `a` compares less than `b`.

# cainteoir::languages {: .doc }

Helper for localizing language tags.

# cainteoir::languages::language {: .doc }

Get the translated ISO 639 language code.

@id
: The localized tag to localize.

@return
: The localized name.

# cainteoir::languages::script {: .doc }

Get the localized ISO 15924 script code.

@id
: The language tag to localize.

@return
: The localized name.

# cainteoir::languages::region {: .doc }

Get the localized ISO 3166 region code.

@id
: The language tag to localize.

@return
: The localized name.

# cainteoir::languages::operator() {: .doc }

Get the localized name of the language.

@langid
: The language tag to localize.

@return
: The localized name.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
