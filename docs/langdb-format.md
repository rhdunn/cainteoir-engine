# Language Database Format

- [Data Types](#data-types)
- [Structure](#structure)
- [Header](#header)
- [Letter-to-Phoneme Rules](#letter-to-phoneme-rules)
  - [Pattern](#pattern)
  - [Default Context Match](#default-context-match)
  - [Right Context Match](#right-context-match)
  - [Left Context Match](#left-context-match)
  - [Phoneme Look Ahead](#phoneme-look-ahead)
- [Dictionary](#dictionary)
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
| pstr   | A `u32` containing the offset from the start of the file to a `str`. |

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
| phonemeset     | pstr   | 12     |
| END OF HEADER  |        | 16     |

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
| phonemes       | pstr   |  4     |
| END OF ENTRY   |        |  8     |

The `pattern` field defines how this letter-to-phoneme entry matches a string
from the current position within that string.

The `phonenes` field is the phonemes to use if the pattern is matched.

### Pattern

The letter-to-phoneme rule pattern describes how the rule should be matched.

The rule pattern is a sequence of characters with the following meaning:

| Character    | Description                                         |
|--------------|-----------------------------------------------------|
| `\x0`        | The end of the rule pattern.                        |
| `[a-z]`      | Match the specified character in the given context. |
| `[\80-\xFF]` | Match the specified character in the given context. |
| `(`          | Switch to the right context.                        |
| `)`          | Switch to the left context.                         |
| `{ccc}`      | Match a phoneme feature, where `c` is `[a-z0-9]`.   |

If the end of the rule pattern is reached, the default context location is
where the current match ends.

If any of the pattern characters fail to match, there is no match and the last
match position is preserved.

### Default Context Match

The default context state starts at the location where the last match ended, or
the start of the string if no rules have been checked for the string.

A character match moves the default context to the right.

A phoneme feature match is not supported.

### Right Context Match

The right context state starts at the default context location.

A character match moves the right context to the right.

A phoneme feature triggers a phoneme look ahead pass. If the phonemes match,
the right context is moved to the default context from the matching rule of
the look ahead phoneme.

### Left Context Match

The left context state starts at the location just before where the last match
ended.

A character match in this state moves the left context to the left.

A phoneme feature match is not supported.

### Phoneme Look Ahead

A new scan process is triggered from the current context location as if a match
occurred at that point. This does not update any state-based information from
the current scan.

If the scan matches a rule with an empty phoneme sequence, that rule is ignored
and the scan continues. This is to support elision (phoneme deletion) rules,
while matching assimilation rules depending on the elided phonemes (e.g. `ng`
rules in English).

The phonemes from a match are checked in order such that the phoneme contains
the phoneme features in the order they are listed in the pattern. That is, the
pattern string must contain at least the same number of phoneme feature
patterns from the current position as there are phonemes, and each phoneme
must contain the feature at the same offset (e.g. the second phoneme must
contain the second phoneme feature pattern).

If there is a match, the rule continues from after the last matching phoneme
feature.

## Dictionary

This is the representation of dictionary entries in an exception dictionary
associated with the letter-to-phoneme rules.

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[3]  |  0     |
| num-entries    | u16    |  3     |
| END OF SECTION |        |  5     |

The `magic` field identifies the section as a dictionary section. This is the
string "DIC".

The `num-entries` field is the number of entries there are in this table.

After the section block, `num-entries` entry blocks are written out in order.
An associated String Table section occurs after the last entry, with the `pstr`
strings from all the entry blocks included.

Each entry block has the form:

| Field          | Type   | Offset |
|----------------|--------|--------|
| word           | pstr   |  0     |
| phonemes       | pstr   |  4     |
| END OF ENTRY   |        |  8     |

The `word` field is the word in the dictionary that has the specified
pronunciation.

The `phonenes` field is the pronunciation of the given word.

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
| next-section   | u32    |  3     |
| END OF SECTION |        |  7     |

The `magic` field identifies the section as a data table. This is the string
"STR".

The `next-section` field is the offset to the next data block.

## Magic Values

This is the list of 3-letter magic values used to identify the different
section and table types. This list is non-normative and is useful when
creating a new section type to avoid collisions in the magic values.

| Magic | Usage                        |
|-------|------------------------------|
| DIC   | Dictionary                   |
| L2P   | Letter To Phoneme Rules      |
| STR   | String Table                 |

Copyright (C) 2014 Reece H. Dunn
