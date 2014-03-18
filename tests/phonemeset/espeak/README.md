# eSpeak Phonemeset Tests

These tests are designed to test the different eSpeak language phonemesets as
supported by Cainteoir Text-to-Speech, covering the different phonemes that
each language supports.

The files are as follows:

| Extension    | Description |
|--------------|-------------|
| `phoneme`    | The phonemes for the eSpeak language found in `dict_phonemes`. |
| `espeak-x`   | The output of `espeak -x` on the `phoneme` file. |
| `espeak-ipa` | The output of `espeak --ipa` on the `phoneme` file. |
| `ipa`        | The phonemes in `phoneme` converted to `ipa` via Cainteoir TTS. |
| `espeak`     | The phonemes in `ipa` converted to eSpeak via Cainteoir TTS. |

The relationship of these files is:

	                           .............
	                           : espeak -x :
	                         .-------------------> LANG.espeak-x
	                         |
	dict_phonemes ---> DICT.phoneme
	       ..............|   |
	       : espeak/LANG |   .-------------------> LANG.espeak-ipa
	       :      to     |     : espeak --ipa :
	       :     ipa     |     ................
	       ..............|       ...............
	                     v       : ipa to      :
	                   LANG.ipa -----------------> LANG.espeak
	                             : espeak/LANG :
	                             ...............

The `DICT.phoneme`, `LANG.espeak-x` and `LANG.espeak-ipa` files are generated
using the `tools/espeak-phonemes` script.
