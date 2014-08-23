# Voice Database Format

- [Data Types](#data-types)
- [Header](#header)
- [String Table](#string-table)
- [Magic Values](#magic-values)

-----

The voice database format (`*.vdb`) is an on-disk file format used by Cainteoir
Text-to-Speech to store the information needed for a synthetic voice to produce
audio when given phoneme and prosodic information. This format acts as a container
for different voice formats in a form suitable for reading directly from the file.

## Data Types

| u8     | An 8-bit unsigned integer |
| u16    | A 16-bit unsigned integer |
| u32    | A 32-bit unsigned integer |
| f8:8   | A fixed point number (8-bit integral part, 8-bit fraction part) |
| str    | A variable-length UTF-8 string terminated by a NULL (`0`) character |
| pstr   | A `u16` containing the offset from the start of the file to a `str`. |

## Structure

The voice database file has the following general structure:

	Header
	Index
	Data

The `Header` section specifies the information about the voice and identifies
the file as a voice database file.

The `Index` section specifies how to access the phonemes or diphones to be spoken.

The `Data` section contains the information needed to generate the audio for the
specified phonemes or diphones.

## Header

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[7]  |  0     |
| endianness     | u16    |  7     |
| rdfns          | pstr   |  9     |
| id             | pstr   | 11     |
| name           | pstr   | 13     |
| synthesizer    | pstr   | 15     |
| author         | pstr   | 17     |
| phonemeset     | pstr   | 19     |
| gender         | u8     | 21     |
| volume-scale   | f8:8   | 22     |
| frequency      | u16    | 24     |
| channels       | u8     | 26     |
| sample-format  | pstr   | 27     |
| END OF HEADER  |        | 29     |

The `magic` field identifies the file as a voice database file. This is the
string "VOICEDB".

The `endianness` field contains the value `0x3031`. It is used to identify
whether the file is in little endian (`10`) or big endian (`01`) order.

The `rdfns` field contains the URI used as the RDF namespace for generated RDF
metadata. It is also the URI associated with information about the synthesizer.

The `id` field is the programatic identifier used to reference the voice. Any
RDF metadata associated with the voice will be associated with `rdfns:id`.

The `name` field is the name of the voice that will be displayed to the user.

The `synthesizer` field is the name of the synthesizer this voice uses. This is
used to select the synthesis algorithm, as well as being displayed to the user.

The `author` field is the name of the authors/companies that created the voice.

The `phonemeset` field is the name of the phonemeset used to transcribe
phonemes or diphones supported by the voice.

The `gender` field is the gender of the voice, with `M` used for male voices
and `F` used for female voices.

The `volume-scale` field is the value used to scale the audio to 0.5 on a scale
of 0.0 to 1.0. This represents 100% volume.

## String Table

A string table is a data table that does not contain a `num-elements` field.
Each entry is a `str` value that is referenced by a `pstr` field in the
previous section.

This is designed to make it easy to traverse over the variable-length string
data.

It has the form:

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[3]  |  0     |
| next-section   | u16    |  3     |
| END OF SECTION |        |  5     |

The `magic` field identifies the section as a data table. See the sub-sections
for the actual value.

The `next-section` field is the offset to the next data block.

## Magic Values

This is the list of 3-letter magic values used to identify the different
section and table types. This list is non-normative and is useful when
creating a new section type to avoid collisions in the magic values.

| Magic | Usage                        |
|-------|------------------------------|
| STR   | String Table                 |

Copyright (C) 2014 Reece H. Dunn
