# Voice Database Format

- [Data Types](#data-types)
- [Structure](#structure)
  - [MBROLA](#mbrola)
- [Header](#header)
- [Pitch Data](#pitch-data)
- [Data Table](#data-table)
  - [Duration Table](#duration-table)
  - [Phoneme Unit Table](#phoneme-unit-table)
  - [Phoneme Table](#phoneme-table)
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

The specific layout is dependent on the synthesizer type.

### MBROLA

These files describe how Cainteoir Text-to-Speech phonemes are mapped to the
MBROLA PHO file format and the voice-specific phonemes. This is then sent to
the `mbrola` program to synthesize using the MBROLA voice database file.

The `id` in the Header section is the name of the MBROLA voice database file
to use.

A Duration Table section is used to specify the duration model to be used.

The Phoneme Table and Phoneme Unit Table sections map Cainteoir Text-to-Speech
phonemes to the voice-specific MBROLA phonemes.

The Pitch Data section specifies the mean and standard deviation of the voice's
pitch.

## Header

The header section identifies the file as a VoiceDB file and provides
information about the voice. This information is used to construct the RDF
metadata associated with the voice.

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[7]  |  0     |
| endianness     | u16    |  7     |
| rdfns          | pstr   |  9     |
| id             | pstr   | 13     |
| name           | pstr   | 17     |
| synthesizer    | pstr   | 21     |
| author         | pstr   | 25     |
| locale         | pstr   | 29     |
| gender         | u8     | 33     |
| volume-scale   | f8:8   | 34     |
| frequency      | u16    | 36     |
| channels       | u8     | 38     |
| sample-format  | pstr   | 39     |
| END OF HEADER  |        | 43     |

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

The `locale` field is the name of the BCP 47 language code for the default
language and accent supported by the voice.

The `gender` field is the gender of the voice, with `M` used for male voices
and `F` used for female voices.

The `volume-scale` field is the value used to scale the audio to 0.5 on a scale
of 0.0 to 1.0. This represents 100% volume.

## Pitch Data

This provides information about the fundamental frequency (F0) of the voice,
used to determine the pitch envelopes of the different phonemes. It describes
the default speaking pitch of the voice.

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[3]  |  0     |
| baseline       | f16:16 |  3     |
| step           | f16:16 |  7     |
| sdev           | f16:16 | 11     |
| END OF HEADER  |        | 15     |

The `magic` field identifies the file as a voice database file. This is the
string "PTC".

The `baseline` field is the average low pitch of the voice in Hertz. That is,
this represents the lowest pitch the voice is comfortable speaking.

The `step` field is the frequency gap between each tone levels.

The `sdev` field is the standard deviation of the pitch around each tone level
in Hertz.

This model is used as it can be adapted to the different prosody algorighms.

To calculate the average pitch for the IPA tone levels, you can use the following:
*	*Top Tone* = `baseline + 4*step`
*	*High Tone* = `baseline + 3*step`
*	*Mid Tone* = `baseline + 2*step`
*	*Low Tone* = `baseline + step`
*	*Bottom Tone* = `baseline`

To use the `INTSINT` prosody model, `T` is the *Top Tone*, `M` is the *Mid Tone*
and `B` is the *Bottom Tone*.

The `baseline`, `step` and `sdev` values can be derived from a minimum (`min`)
and maximum (`max`) pitch value, such that:
*	`sdev = (max - min)/ 20`
*	`baseline = min + 2*sdev`
*	`step = 4 *sdev`

This is because there are 5 tone levels, each with 4 `sdev` sections, with 2
`sdev` sections either side of the mean value of each tone level.

## Data Table

This is the generic structure of a tabular section. That is, a fixed sized
array of entities of the same type.

| Field          | Type   | Offset |
|----------------|--------|--------|
| magic          | u8[3]  |  0     |
| num-entries    | u16    |  3     |
| END OF SECTION |        |  5     |

The `magic` field identifies the section as a data table. See the sub-sections
for the actual value.

The `num-entries` field is the number of entries there are in this table.

After the section block, `num-entries` entry blocks are written out in order.
An associated String Table section occurs after the last entry, with the `pstr`
strings from all the entry blocks included.

### Duration Table

This is the binary representation of a (mean, standard-deviation) duration
model.

A duration table has the "DUR" magic string, and each entry has the form:

| Field          | Type   | Offset |
|----------------|--------|--------|
| phoneme1       | u64    |  0     |
| phoneme2       | u64    |  8     |
| duration-mean  | u8     | 16     |
| duration-stdev | u8     | 17     |
| END OF ENTRY   |        | 18     |

The `phoneme1` field is the value of the phoneme as represented by the
`cainteoir::ipa::phoneme` class.

The `phoneme2` field is the value of the phoneme as represented by the
`cainteoir::ipa::phoneme` class. This is for affricates and diphthongs.

The`duration-mean` field is the average duration of the phoneme in milliseconds.

The`duration-stdev` field is the standard deviation for the duration of the phoneme
in milliseconds.

### Phoneme Unit Table

This is the information about how a given unit should be constructed when
synthesizing the associated phoneme.

A phoneme unit table has the "PUT" magic string, and each entry has the form:

| Field          | Type   | Offset |
|----------------|--------|--------|
| name           | pstr   |  0     |
| phoneme-start  | u8     |  4     |
| unit-start     | u8     |  5     |
| unit-end       | u8     |  6     |
| END OF ENTRY   |        |  7     |

The `name` field is the name of the unit as used to select the unit in the
phone or diphone synthesis.

The `phoneme-start` field is the percentage offset of the phoneme at which this
unit is generated. The end of the phoneme is the `phoneme-start` of the next
unit in the sequence, or `100%` if this is the last unit.

The `unit-start` field is the percentage offset at which this unit starts. This
allows for partial units to be synthesized.

The `unit-end` field is the percentage offset at which this unit ends. This
allows for partial units to be synthesized.

### Phoneme Table

This is the information for how to map phonemes to units in unit-based phoneme
synthesis.

A phoneme table has the "PHO" magic string, and each entry has the form:

| Field          | Type   | Offset |
|----------------|--------|--------|
| phoneme1       | u64    |  0     |
| phoneme2       | u64    |  8     |
| first-unit     | u16    | 16     |
| num-units      | u8     | 18     |
| END OF ENTRY   |        | 19     |

The `phoneme1` field is the value of the phoneme as represented by the
`cainteoir::ipa::phoneme` class.

The `phoneme2` field is the value of the phoneme as represented by the
`cainteoir::ipa::phoneme` class. This is for affricates and diphthongs.

The `first-unit` field is the index of the first unit in the Phoneme Unit
Table.

The`num-units` field is the number of sound units this phoneme is composed of.

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
| DUR   | Duration Table               |
| PHO   | Phoneme Table                |
| PTC   | Pitch Data                   |
| PUT   | Phoneme Unit Table           |
| STR   | String Table                 |

Copyright (C) 2014 Reece H. Dunn
