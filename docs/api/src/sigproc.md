# cainteoir::audio_data {: .doc }

Stores audio sample data.

# cainteoir::audio_data::samples {: .doc }

The samples for the audio.

# cainteoir::audio_data::info {: .doc }

Information about the associated audio samples.

# cainteoir::read_s16_samples {: .doc }

Extract the S16 audio samples from an audio data buffer.

@aData
: The audio data buffer to read.

@aStart
: The point in the audio object to start reading from.

@aEnd
: The point in the audio object to end reading at.

@aChannel
: The index of the audio channel to extract the samples for.

@aFrequency
: The frequency to extract the samples in.

@return
: The samples associated with the audio data buffer.

# cainteoir::cosine_window {: .doc }

Create a first-order cosine window function.

@aAlpha
: The value for the alpha parameter of the function.

@aBeta
: The value for the beta parameter of the function.

@aWindowSize
: The number of samples to use in the window.

This calculates the values of a generalized hamming window (first order cosine
window) of the form:

	w(n) = alpha - beta * cos((2 * pi * n) / (N - 1))

where `N` is the window size.

@return
: The values of the window function at each point in the window.

# cainteoir::window {: .doc }

Create a window function by name.

@aName
: The name of the window function

@aWindowSize
: The number of samples to use in the window.

The `aName` parameter specifies the window function to create, which can be:

| Name     | Description |
|----------|-------------|
| hamming  | A Hamming window. |
| hamming0 | A zero-phase Hamming window. |
| hann     | A Hann/Hanning window. |
| hann0    | A zero-phase Hann/Hanning window. |
| hanning  | A Hann/Hanning window. |
| hanning0 | A zero-phase Hann/Hanning window. |

@return
: The values of the window function at each point in the window.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
