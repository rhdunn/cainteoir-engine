# Voice Database Format

- [Data Types](#data-types)
- [Header](#header)

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

The `magic` field identifies the file as a voice database file. This is the
string "VOICEDB".

The `endianness` field contains the value `0x3031`. It is used to identify
whether the file is in little endian (`10`) or big endian (`01`) order.

Copyright (C) 2014 Reece H. Dunn
