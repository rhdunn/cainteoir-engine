# Phoneme Transcription Schemes

Phonemes can be transcribed to text in various different ways, but they are
performing the same task: expressing the phonemes that are spoken in a
written form.

All phonemes ultimately derive from the International Phonetic Alphabet (IPA)
which characterizes each phoneme by its fundamental characteristics, such as
the manner of articulation for a consonant. This constructs an IPA chart of
the different possible phonemes expressed as IPA symbols.

Phoneme sets can be categorized into three different schemes:

  1.  [IPA](http://en.wikipedia.org/wiki/International_Phonetic_Alphabet) -- representing all the IPA symbols;
  2.  [SAMPA](http://www.phon.ucl.ac.uk/home/sampa) -- representing language-specific symbols in ASCII;
  3.  [ARPABET](http://en.wikipedia.org/wiki/Arpabet)

Phoneme sets that describe IPA transcriptions specify how IPA symbols are
expressed. They attempt to reproduce the IPA phoneme set as completely as
possible. Phoneme transcriptions of this type are:

  1.  Unicode -- using the correct Unicode symbols for the IPA;
  2.  [Kirshenbaum](http://en.wikipedia.org/wiki/Kirshenbaum) -- ASCII transcription of IPA;
  3.  [X-SAMPA](http://en.wikipedia.org/wiki/X-SAMPA) -- alternate ASCII transcription of IPA;
  4.  [CXS](http://en.wikipedia.org/wiki/Conlang_XSAMPA) -- conlang extensions to X-SAMPA;

The language-specific phoneme sets can be grouped into several general
classifications which specify the root transcription scheme on which they
are based:

  1.  [ARPABET](http://en.wikipedia.org/wiki/Arpabet)
  2.  [SAMPA](http://www.phon.ucl.ac.uk/home/sampa)
  3.  [eSpeak](http://espeak.sourceforge.net/phonemes.html) -- a Kirshenbaum derivative

NOTE: The eSpeak voices are configured to use a different phoneme set for
each language, so are classified as being language-specific even though a
language-independent IPA transcription should be possible.

The ARPABET-based schemes consist of:

  1.  [ARPABET](http://en.wikipedia.org/wiki/Arpabet) -- used in the CMU dictionary
  2.  [cepstral-de](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_german.html) -- Cepstral German voices
  3.  [cepstral-en_UK](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_uk_eng.html) -- Cepstral British English voices
  4.  [cepstral-en_US](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_us_eng.html) -- Cepstral American English voices
  5.  [cepstral-es_LA](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_amer_span.html) -- Cepstral Americas Spanish voices
  6.  [cepstral-fr_CA](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_can_fr.html) -- Cepstral Canadian French voices
  7.  [cepstral-it](http://www.cepstral.com/support/faq/customization/lexicon/lexicon_italian.html) -- Cepstral Iialian voices

The SAMPA-based schemes consist of:

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

The [MBROLA](http://tcts.fpms.ac.be/synthesis/) phoneme scheme is
based on X-SAMPA. Each voice describes the phonemes supported in a
text file with the same name as the voice (e.g. ca1.txt).

MBROLA transcriptions also contain prosody information. They consist
of the following on each line:

    phoneme length position frequency position frequency ...

where the (position, frequency) pairs are optional and form the frequency
contour shape, with position being the percentage through the phoneme.

## Japanese

Japanese is interesting as it comes with its own phonetic system. This
system has several transcriptions:

  1.  [hiragana](http://en.wikipedia.org/wiki/Hiragana)
  2.  [katakana](http://en.wikipedia.org/wiki/Katakana)
  3.  [romaji](http://en.wikipedia.org/wiki/Hepburn_romanization)

Here, the Chinese characters used in Japanese can be transcribed
phonetically using Hiragana.

Likewise, Katakana can be mapped onto the equivalent Hiragana syllables.

There are then three ways the Hiragana phonemes can be pronounced:

  1.  directly from the hiragana (for native Japanese voices);
  2.  via the IPA equivalents;
  3.  via Westernized variants of the IPA (equivalent to their romaji form).
