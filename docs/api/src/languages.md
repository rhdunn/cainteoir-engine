#<cldoc:cainteoir::tts::get_voice_uri>

#<cldoc:cainteoir::language::tag>

A BCP 47/[RFC 5646](http://www.ietf.org/rfc/rfc5646.txt) language tag.

#<cldoc:cainteoir::language::tag::tag>

Create a language tag object.
@l The primary language code.
@e The extended language code.
@s The writing script code.
@r The region code.
@v The variant code.
@p The private-use code.

#<cldoc:cainteoir::language::tag::lang>

ISO 639 language code (primary language).

#<cldoc:cainteoir::language::tag::extlang>

ISO 639 language code (extended language).

#<cldoc:cainteoir::language::tag::script>

ISO 15924 script code.

#<cldoc:cainteoir::language::tag::region>

ISO 3166 or UN M.49 region code.

#<cldoc:cainteoir::language::tag::variant>

IANA variant subtag.

#<cldoc:cainteoir::language::tag::private_use>

An application-specific custom subtag.

#<cldoc:cainteoir::language::make_lang>

Extract language tag information from a BCP 47 language id.
@code The language identifier, e.g. "es-MX".

@return The extracted language, script and country codes.

#<cldoc:cainteoir::language::operator==>

Compare two language tags for equality.
@a The fist language tag to compare.
@b The second language tag to compare.

@return `true` if the language tags match, `false` otherwise.

#<cldoc:cainteoir::language::operator<>

Compare two language tags for ordering.
@a The fist language tag to compare.
@b The second language tag to compare.

@return `true` if <a> compares less than <b>.

#<cldoc:cainteoir::languages>

Helper for localizing language tags.

#<cldoc:cainteoir::languages::language>

Get the translated ISO 639 language code.
@id The localized tag to localize.

@return The localized name.

#<cldoc:cainteoir::languages::script>

Get the localized ISO 15924 script code.
@id The language tag to localize.

@return The localized name.

#<cldoc:cainteoir::languages::region>

Get the localized ISO 3166 region code.
@id The language tag to localize.

@return The localized name.

#<cldoc:cainteoir::languages::operator()>

Get the localized name of the language.
@langid The language tag to localize.

@return The localized name.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
