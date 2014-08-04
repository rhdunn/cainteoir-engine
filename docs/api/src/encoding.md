# cainteoir::encoding {: .doc }

Manage the conversion of text in different character encodings to UTF-8.

# cainteoir::encoding::encoding {: .doc }

Initialize the encoder with the specified Windows codepage.

@aCodepage
: The Windows codepage to use.

# cainteoir::encoding::encoding {: .doc }

Initialize the encoder with the specified character encoding.

@aEncoding
: The character encoding to use.

# cainteoir::encoding::set_encoding {: .doc }

Set the character encoding to the specified Windows codepage.

@aCodepage
: The Windows codepage to change to.

@return
: `true` if the encoding was changed, `false` otherwise.

# cainteoir::encoding::set_encoding {: .doc }

Set the character encoding.

@aEncoding
: The character encoding to change to.

@return
: `true` if the encoding was changed, `false` otherwise.

# cainteoir::encoding::lookup {: .doc }

Lookup the single-byte character.

@c
: The character to lookup.

@return
: The UTF-8 representation of `c`.

# cainteoir::encoding::decode {: .doc }

Convert the data buffer to UTF-8.

@data
: The character buffer to convert.

@return
: The UTF-8 representation of data.

# cainteoir::encoding::decode {: .doc }

Convert the data buffer to UTF-8.

@data
: The character buffer to convert.

@decoded
: The rope to add the UTF-8 representation of data to.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
