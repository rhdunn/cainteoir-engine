# Voice Database Format

- [Data Types](#data-types)
- [Structure](#structure)
- [Header](#header)
- [Index](#index)
  - [Diphone Index](#diphone-index)
  - [LPC Residual Index](#lpc-residual-index)
  - [LPC Residual Sizes](#lpc-residual-sizes)
- [Data](#data)
  - [Residual Excited LPC](#residual-excited-lpc)
- [LPC Data](#lpc-data)
- [Residual Data](#residual-data)

-----

The voice database format (`*.vdb`) is an on-disk file format used by Cainteoir
Text-to-Speech to store the information needed for a synthetic voice to produce
audio when given phoneme and prosodic information. This format acts as a container
for different voice formats in a form suitable for reading directly from the file.

## Data Types

| T[]  | A variable-length array of type `T` |
| T[n] | A fixed-length array of type `T` with `n` elements |
| u8   | An 8-bit unsigned integer |
| u16  | A 16-bit unsigned integer |
| u32  | A 32-bit unsigned integer |
| i8   | An 8-bit signed integer |
| i16  | A 16-bit signed integer |
| i32  | A 32-bit signed integer |
| str  | A variable-length UTF-8 string terminated by a NULL (`0`) character |

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

| magic          | u8[7] |
| endianness     | u16   |
| header version | u8    |
| major version  | u8    |
| minor version  | u8    |
| patch version  | u8    |
| sample rate    | u16   |
| name           | str   |
| company        | str   |
| locale         | str   |
| phonemeset     | str   |
| gender         | u8    |
| age            | u8    |

The `magic` field identifies the file as a voice database file. This is the
string "VOICEDB".

The `endianness` field contains the value `0x3031`. It is used to identify
whether the file is in little endian (`10`) or big endian (`01`) order.

The `header version` field determines the version of this header section and
thus which header fields are present. This is currently `0`, but will change
in future versions of the file format.

The `major version`, `minor version` and `patch version` fields are used by the
voice to identify the version of the voice itself.

The `sample rate` field is the sample rate of the audio produced by the voice.

The `name` field specifies the name of the voice.

The `company` field specifies the name of the company or institution that provided
the voice data.

The `locale` field specifies the BCP 47 locale (language) of the voice. This is
the language and accent of the voice. Other accents and languages may be supported
as well to varying degrees.

The `phonemeset` field specifies the name of the phonemeset used to transcribe the
phonemes (or diphones) the voice database supports.

The `gender` field specifies the ggener of the voice, using 'F' for a female voice
and `M` for a male voice.

The `age` field specifies the approximate age of the voice, with `0` denoting that
the age is not specified.

## Index

| magic      | u8[3] |
| index type | u8    |
| entries    | u16   |
| data       | u8[]  |

The `magic` field identifies the section as an index section. This is the string
"IDX".

The `index type` field identifies the type of the index. This determines the
structure of the `data` field. This can be:

| `0` | Diphone Index |
| `1` | LPC Residual Index |
| `2` | LPC Residual Sizes |

The `entries` field specifies how many entries are in this index section.

The `data` field specifies the contents of the index. This consists of `entries`
items whose format is determined by `index type`.

### Diphone Index

| name  | str |
| start | u16 |
| mid   | u8  |
| end   | u8  |

The `name` field specifies the name of the diphone in the form `phoneme1-phoneme2`.

The `start` field specifies the start frame of the diphone entry.

The `mid` field specifies the midpoint of the diphone entry relative to `start`. The
actual value is therefore `start + mid`.

The `end` field specifies the end of the diphone entry relative to `mid`. The
actual value is therefore `start + mid + end`.

### LPC Residual Index

| offset | u32 |

The `offset` field specifies the byte offset of the start of the residual at the
specified index.

### LPC Residual Sizes

| size | u8 |

The `size` field specifies the uncompressed size of the residual at the specified
index.

## Data

| magic     | u8[3] |
| data type | u8    |
| data      | u8[]  |

The `magic` field identifies the section as a data section. This is the string
"DAT".

The `data type` field identifies the type of the audio data. This determines the
structure of the `data` field. This can be:

| `0` | Residual Excited LPC (ulaw) |
| `1` | Residual Excited LPC (G.721) |

The `data` field specifies the contents of the remaining part of the data fiels
and the sections that follow it.

### Residual Excited LPC

| frames             | u16 |
| order              | u8  |
| coefficient min    | i32 |
| coefficient range  | i32 |

The `frames` field specifies the number of frames of audio are stored in the LPC
and Residual sections.

The `order` field specifies the LPC order. This is how many LPC values are in
each frame.

The `coefficient min` field specifies the minimum LPC coefficient.

The `coefficient range` field specifies the range of LPC coefficients.

After the data section, the following sections appear in any order:

	Index : Residual Index
	LPC Data
	Residual Data

If the residual data is compressed (such as when using G.721), the following
section is present:

	Index : Residual Sizes

## LPC Data

| magic     | u8[3] |
| data      | u16[] |

The `magic` field identifies the section as an LPC data section. This is the string
"LPC".

The `data` field specifies the LPC data. This is a set of LPC frames. With `dat`
being the data section, there are `dat.frames` frames of LPC data, each of which
contains `dat.order` items.

## Residual Data

| magic     | u8[3] |
| data      | u8[]  |

The `magic` field identifies the section as a residual data section. This is the string
"RES".

The `data` field specifies the residual data. The start of the residual for a given frame
is located in the Residual Index section.

For `ulaw`, the residual data is not compressed, so there is no Residual Sizes section.

For `G.721`, the residual data is compressed and the Residual Sizes section specifies the
uncompressed size of the residual data in each field.

Copyright (C) 2014 Reece H. Dunn
