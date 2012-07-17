# Phoneme Morphology

Phoneme morphology is where various phonemes transform to other phonemes as
part of natural speech. This can be for a number of reasons:

  1.  modification of end/beginning of word phonemes to make them flow
      together, e.g.:

         "said john" : /s'Ed dZ'0n/ -> /s'E-dZ'0n/

  2.  modification of a phoneme as a result of surrounding phonemes, e.g.:

         "houses" : /hoUs@s/ -> /hoUz@z/

  3.  modification of a phoneme as a result of syllabic stress placement,
      e.g.:

         "demote" : /dIm'oUt/ -> /d@m'oUt/

  4.  modification of a phoneme due to the dialect of the speaker, e.g.:

         "cot" : /c'0t/ -> /c'A:t/

  5.  modification of a phoneme due to speech impediments, lisps or a
      blockage of the nasal cavity, e.g.

         "silly" : /s'Ili/ -> /s'Iwi/

Encoding these variations at the letter-to-phoneme phase would be very
complex as this would lead to a large number of letter-to-phoneme rules
that would not necessarily be 100% accurate (e.g. when to use /s/ or /z/
is difficult to write letter-to-phoneme rules for, but /z/ is used if
the preceeding phoneme is voiced or a vowel).

It is not yet clear how phoneme morphology works in practice. In order
to do this methodically, word transcriptions need a morphology neutral
transcription along with transcriptions for different cases (e.g. stress
placement).
