# Language Database Format

- [Data Types](#data-types)
- [Structure](#structure)
- [Header](#header)
- [String Table](#string-table)
- [Magic Values](#magic-values)

-----

The language database format (`*.ldb`) is an on-disk file format used by Cainteoir
Text-to-Speech to store the information needed to convert text to phonemes.

## Data Types

| u8     | An 8-bit unsigned integer |
| u16    | A 16-bit unsigned integer |
| u32    | A 32-bit unsigned integer |
| u64    | A 64-bit unsigned integer |
| f8:8   | A fixed point number (8-bit integral part, 8-bit fraction part) |
| f16:16 | A fixed point number (16-bit integral part, 16-bit fraction part) |
| str    | A variable-length UTF-8 string terminated by a NULL (`0`) character |
| pstr   | A `u16` containing the offset from the start of the file to a `str`. |

## Structure

The voice database file has the following general structure:

	Header
	Section
	...
	Section

A `String Table` section follows any section (including the `Header` section)
that contains `pstr` values.

## Header

The header section identifies the file as a LangDB file and provides
information about the language.

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[6]  |  0     |
| endianness     | u16    |  6     |
| locale         | pstr   |  8     |
| END OF HEADER  |        | 10     |

The `magic` field identifies the file as a voice database file. This is the
string "LANGDB".

The `endianness` field contains the value `0x3031`. It is used to identify
whether the file is in little endian (`10`) or big endian (`01`) order.

The `locale` field is the name of the BCP 47 language code for the language
supported by this file. This does not cover any accents supported within the
file.

## String Table

A string table is a data table that does not contain a `num-elements` field.
Instead, it contains an offset to the start of the next section. Each entry
is a `str` value that is referenced by a `pstr` field in the previous section.

This is designed to make it easy to traverse over the variable-length string
data.

It has the form:

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[3]  |  0     |
| next-section   | u16    |  3     |
| END OF SECTION |        |  5     |

The `magic` field identifies the section as a data table. This is the string
"STR".

The `next-section` field is the offset to the next data block.

## Magic Values

This is the list of 3-letter magic values used to identify the different
section and table types. This list is non-normative and is useful when
creating a new section type to avoid collisions in the magic values.

| Magic | Usage                        |
|-------|------------------------------|
| STR   | String Table                 |

Copyright (C) 2014 Reece H. Dunn
