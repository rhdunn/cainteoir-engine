# Languages, Voices, Accents and Dialects

A language such as English or Spanish defines:

  1.  a set of phonemes that are used by the language;
  2.  a set of letter-to-phoneme rules;
  3.  an exception dictionary containing words not covered by the letter-to-phoneme rules;
  4.  a set of rules for identifying context-based pronunciation (for words like 'read' and 'Polish').

Some languages -- specifically those that share a common ancestry -- can be
defined in terms of their base language, allowing the languages to share
pronunciation rules. For example, Afrikaans is a variant of Middle Dutch,
from which Modern Dutch is also derived.

A dialect is a variant of a language which varies only in the words used to
describe certain things (e.g. "a wee bit" in Scottish, instead of "a small bit").

The words of each dialect should be supported by the language. Optionally,
the dialect can map the words in the text to use the dialect variants to
force the text to be spoken in the specified dialect.

An accent is either a general accent (such as Canadian French) or a regional
accent (such as Scouse (Liverpudlian) English) of a specific language. The
accent defines:

  1.  a set of phoneme-to-phoneme rules (e.g. /Vk/ => /Vx/ for Scouse English);
  2.  a set of letter-to-phoneme rules;
  3.  an exception dictionary.

The letter-to-phoneme rules and exception dictionary extend and take priority
over the base language.

A voice consists of:

  1.  an engine used to synthesize the audio data from a phoneme sequence;
  2.  a set of phonemes that are synthesized by the voice;
  3.  audio data used by the voice synthesizer.

## Mapping Languages To Voices

The phoneme set defined by a language/accent provides the set of phonemes
required to pronounce this language/accent. Note that the language and
accent must be treated as a single unit here, as an accent may add phonemes
to the base language and no-one speaks a language without an accent!

A voice provides a set of synthesized phonemes, which is typically restricted
to one language/accent (e.g. Latin American Spanish), so on its own does not
map to many supported language/accent pairs.

Linguistically, certain phonemes are related to each other and can be used
as a replacement for one another. For example, consider the phonemes /r/,
/ɹ/, /ʁ/ and /ʀ/. In English only /ɹ/ is supported, in Spanish /r/ and /ɹ/
are supported and in German /ʁ/ and /ʀ/ are supported. Looking at the IPA
phoneme table, there are two transforms that can be done on this phoneme
group:

  1.  trilled <=> approximant (/r/ <=> /ɹ/ and /ʀ/ <=> /ʁ/) and;
  2.  alveolar <=> uvular (/ɹ/ <=> /ʁ/ and /r/ <=> /ʀ/).

Other phonemes are typically approximated by speakers using the phonemes that
speaker can articulate. For example, /ɲ/ from Spanish is typically pronounced
as /nj/ by English speakers.

These equivalence rules can be applied to the voice's phoneme set to provide
the set of phonemes supported by the voice using a mapping table to map the
supported to synthesized phonemes.

The eSpeak engine provides a per-voice mapping for the MBROLA voices, which
allows the German voices to speak English. This approach is avoided as the
mappings are duplicated between the voices and the phoneme set of the language
can change which then breaks the voice!

If the phonemes supported by a voice contain all the phonemes required by a
language/accent pair then that voice can speak in that language/accent.

## eSpeak Voices

The eSpeak voices are actually a single voice ("Jonathon"), but voice and
language are intertwined so each 'voice' implements the subset of the IPA
phonemes specific to that language.

Ideally, the eSpeak voice should be provided as a single voice data file
that exposes all supported IPA phonemes irrespective of any target
language. It should also avoid phoneme morphology to remain accurate to
the provided transcription (with exceptions such as aspiration of plosives
at the start of a word, which tends to get ommitted in phonetic
transcriptions).

## Visualization / UI

It should be possible to visualize the general phoneme mapping rules by
showing the phonemes in an IPA table layout and showing coloured arrows
between the phonemes that are mapped to each other.

It should be possible to visualize equivalent phonemes of a given voice
by showing the phonemes in an IPA table layout, with the synthesized
phonemes in one colour and the mapped phonemes in another with arrows
showing the derivation.

The UI providing the voice selection to the user should only show voices
that are capable of reading the current document (with unspecified
language defaulting to English and allowing the user to change that in
the document information UI). The accent should be selected independently
of the voice.
