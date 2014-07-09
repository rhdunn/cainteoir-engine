#<cldoc:cainteoir::encoding>

Manage the conversion of text in different character encodings to UTF-8.

#<cldoc:cainteoir::encoding::encoding>

Initialize the encoder with the specified Windows codepage.
@aCodepage The Windows codepage to use.

#<cldoc:cainteoir::encoding::encoding>

Initialize the encoder with the specified character encoding.
@aEncoding The character encoding to use.

#<cldoc:cainteoir::encoding::set_encoding>

Set the character encoding to the specified Windows codepage.
@aCodepage The Windows codepage to change to.

@return `true` if the encoding was changed, `false` otherwise.

#<cldoc:cainteoir::encoding::set_encoding>

Set the character encoding.
@aEncoding The character encoding to change to.

@return `true` if the encoding was changed, `false` otherwise.

#<cldoc:cainteoir::encoding::lookup>

Lookup the single-byte character.
@c The character to lookup.

@return The UTF-8 representation of <c>.

#<cldoc:cainteoir::encoding::decode>

Convert the data buffer to UTF-8.
@data The character buffer to convert.

@return The UTF-8 representation of data.

#<cldoc:cainteoir::encoding::decode>

Convert the data buffer to UTF-8.
@data    The character buffer to convert.
@decoded The rope to add the UTF-8 representation of data to.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
