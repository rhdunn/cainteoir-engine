# Language Database Format

- [Data Types](#data-types)
- [Structure](#structure)
- [Header](#header)
- [Letter-to-Phoneme Rules](#letter-to-phoneme-rules)
  - [Pattern](#pattern)
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
| phonemeset     | pstr   | 10     |
| END OF HEADER  |        | 12     |

The `magic` field identifies the file as a voice database file. This is the
string "LANGDB".

The `endianness` field contains the value `0x3031`. It is used to identify
whether the file is in little endian (`10`) or big endian (`01`) order.

The `locale` field is the name of the BCP 47 language code for the language
supported by this file. This does not cover any accents supported within the
file.

The `phonemeset` field identifies the format in which the phonemes are
transcribed.

## Letter-to-Phoneme Rules

This is the representation of the letter-to-phoneme rule group, which holds a
collection of related rules.

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[3]  |  0     |
| num-entries    | u16    |  3     |
| group          | u8     |  5     |
| END OF SECTION |        |  6     |

The `magic` field identifies the section as a letter-to-phoneme rules group.
This is the string "L2P".

The `num-entries` field is the number of entries there are in this table.

The `group` field is the initial character of the context match for the rules
within this set of rules.

After the section block, `num-entries` entry blocks are written out in order.
An associated String Table section occurs after the last entry, with the `pstr`
strings from all the entry blocks included.

Each entry block has the form:

| Field          | Type   | Offset |
|----------------|--------|--------|
| pattern        | pstr   |  0     |
| phonemes       | pstr   |  2     |
| END OF ENTRY   |        |  4     |

The `pattern` field defines how this letter-to-phoneme entry matches a string
from the current position within that string.

The `phonenes` field is the phonemes to use if the pattern is matched.

### Pattern

The letter-to-phoneme rule pattern describes how the rule should be matched.

The `context` string occurs first and is any character that is not a control
character (i.e. is `0x20` or above). This includes characters above `0x7F` to
support UTF-8 characters, allowing the matching algorithm to operate at the
byte level.

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
| L2P   | Letter To Phoneme Rules      |
| STR   | String Table                 |

Copyright (C) 2014 Reece H. Dunn
