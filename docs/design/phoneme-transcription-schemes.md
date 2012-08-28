# Phoneme Transcription Schemes

A phonetic transcription scheme is a way of writing down phonemes to represent the different sounds of one or more languages or accents. They are typically used either to describe how to pronounce the words of a language or to transcribe someone speaking.

## Arpabet

Arpabet is used for transcribing General American English. It has several variants used in different text-to-speech programs and dictionaries:

  1.  [Arpabet](http://en.wikipedia.org/wiki/Arpabet) -- used in the CMU dictionary
  2.  [festival](http://www.cstr.ed.ac.uk/projects/festival/) -- festival dictionary phonemes

### Cepstral

The Cepstral text-to-speech program uses the CMU dictionary and a variation of the Arpabet phoneme set for its General American English voices. The other languages support different phoneme sets specific for that language that appear to be derived from Arpabet as well:

  1.  [cepstral-de](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_german.html) -- Cepstral German voices
  2.  [cepstral-en_UK](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_uk_eng.html) -- Cepstral British English voices
  3.  [cepstral-en_US](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_us_eng.html) -- Cepstral American English voices
  4.  [cepstral-es_LA](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_amer_span.html) -- Cepstral Americas Spanish voices
  5.  [cepstral-fr_CA](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_can_fr.html) -- Cepstral Canadian French voices
  6.  [cepstral-it](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_italian.html) -- Cepstral Iialian voices

## SAMPA

The SAMPA-based transcription schemes vary between the different languages, and consist of:

  1.  [sampa-ar](http://www.phon.ucl.ac.uk/home/sampa/arabic.htm)
  2.  [sampa-bg](http://www.phon.ucl.ac.uk/home/sampa/bulgar.htm)
  3.  [sampa-cs](http://www.phon.ucl.ac.uk/home/sampa/czech-uni.htm)
  4.  [sampa-da](http://www.phon.ucl.ac.uk/home/sampa/danish.htm)
  5.  [sampa-de](http://www.phon.ucl.ac.uk/home/sampa/german.htm)
  6.  [sampa-el](http://www.phon.ucl.ac.uk/home/sampa/greek.htm)
  7.  [sampa-en](http://www.phon.ucl.ac.uk/home/sampa/english.htm)
  8.  [sampa-en_US](http://www.phon.ucl.ac.uk/home/sampa/american.htm)
  9.  [sampa-es](http://www.phon.ucl.ac.uk/home/sampa/spanish.htm)
  10. [sampa-et](http://www.phon.ucl.ac.uk/home/sampa/estonian.htm)
  11. [sampa-fr](http://www.phon.ucl.ac.uk/home/sampa/french.htm)
  12. [sampa-he](http://www.phon.ucl.ac.uk/home/sampa/hebrew.htm)
  13. [sampa-hr](http://www.phon.ucl.ac.uk/home/sampa/croatian.htm)
  14. [sampa-hu](http://www.phon.ucl.ac.uk/home/sampa/hungaria.htm)
  15. [sampa-it](http://www.phon.ucl.ac.uk/home/sampa/italian.htm)
  16. [sampa-nl](http://www.phon.ucl.ac.uk/home/sampa/dutch.htm)
  17. [sampa-no](http://www.phon.ucl.ac.uk/home/sampa/norweg.htm)
  18. [sampa-pl](http://www.phon.ucl.ac.uk/home/sampa/polish.htm)
  19. [sampa-pt](http://www.phon.ucl.ac.uk/home/sampa/portug.htm)
  20. [sampa-ro](http://www.phon.ucl.ac.uk/home/sampa/romanian.htm)
  21. [sampa-ru](http://www.phon.ucl.ac.uk/home/sampa/russian.htm)
  22. [sampa-sl](http://www.phon.ucl.ac.uk/home/sampa/slovenian.htm)
  23. [sampa-sv](http://www.phon.ucl.ac.uk/home/sampa/swedish.htm)
  24. [sampa-th](http://www.phon.ucl.ac.uk/home/sampa/thai.htm)
  25. [sampa-tr](http://www.phon.ucl.ac.uk/home/sampa/turkish.htm)
  26. [sampa-yue](http://www.phon.ucl.ac.uk/home/sampa/cantonese.htm)

Various prosodic elements are supported through the [SAMPROSA](http://www.phon.ucl.ac.uk/home/sampa/samprosa.htm) transcription scheme that can be used in addition to the language transcription scheme.

### MBROLA

The [MBROLA](http://tcts.fpms.ac.be/synthesis/) phoneme sets are
different for each voice. The voice describes the phonemes supported
in a text file with the same name as the voice (e.g. ca1.txt) which
are derivatives of the corresponding SAMPA phoneme set for that
language.

MBROLA transcriptions also contain prosody information. They consist
of the following on each line:

    phoneme length position frequency position frequency ...

where the (position, frequency) pairs are optional and form the frequency
contour shape, with position being the percentage through the phoneme.

## International Phonetic Alphabet

The [International Phonetic Alphabet](http://en.wikipedia.org/wiki/International_Phonetic_Alphabet) (IPA) provides a representation of all the different pronouncible sounds that humans can make that are used in languages from all over the world. There are different transcription schemes based on IPA:

  1.  [Unicode](http://www.unicode.org) -- using the correct Unicode symbols for the IPA
  2.  [Kirshenbaum](http://en.wikipedia.org/wiki/Kirshenbaum) -- ASCII transcription of IPA
  3.  [X-SAMPA](http://en.wikipedia.org/wiki/X-SAMPA) -- alternate ASCII transcription of IPA
  4.  [CXS](http://en.wikipedia.org/wiki/Conlang_XSAMPA) -- conlang extensions to X-SAMPA

NOTE: The IPA symbols are used differently by different transcribers depending on the language and phonetic qualities the transcriber wants to record.

### eSpeak

The [eSpeak](http://espeak.sourceforge.net/phonemes.html) phoneme set is derived from Kirshenbaum but instead of having a single phoneme set for all languages it uses a different phoneme set for each language, tailoring the voice to that language.

## Language Phonetic Alphabets

Some of the worlds languages have alphabets that are phonetic -- they describe how the words are to be pronounced implicitly. These alphabets include:

  1.  [Hiragana](http://en.wikipedia.org/wiki/Hiragana) -- Japanese (for Japanese language words)
  2.  [Katakana](http://en.wikipedia.org/wiki/Katakana) -- Japanese (for foreign language words)
  3.  [Cherokee](http://en.wikipedia.org/wiki/Cherokee_alphabet)
  4.  [Hangul](http://en.wikipedia.org/wiki/Hangul) -- Korean

For these alphabets, the alphabet characters can either be:

  1.  pronounced directly;
  2.  mapped to the alphabet of the speaker (e.g. using [Hepburn romanji](http://en.wikipedia.org/wiki/Hepburn_romanization) for Japanese Hiragana/Katakana);
  3.  mapped to the IPA or one of its variants (for a more precise pronunciation).

Languages that have other alphabets in addition to the phonetic ones (e.g. Japanese has Kanji, which are imported Chinese characters) can map those alphabets to their phonetic alphabet (e.g. mapping Kanji to Hiragana). This allows the pronunciation of those characters to be kept as accurate as possible for as long as possible.
