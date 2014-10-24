# cainteoir::complex {: .doc }

Stores a `float`-based complex number in cartesian coordinate form.

This is used by the fourier-based signal processing logic to represent complex
numbers. Although this is equivalent to `std::complex<float>`, the standard
type is not used because there is no guaranteed memory layout and you cannot
set the real and imaginary parts directly. This makes it difficult to work
with and prevents using vectorized assembly instructions.

# cainteoir::complex::re {: .doc }

The real part of the complex number.

# cainteoir::complex::im {: .doc }

The imaginary part of the complex number.

# cainteoir::abs {: .doc }

Calculate `|x|` for each value in the array.

This is used to evaluate the power spectrum (spectrogram) and power cepstrum
of a signal. It is defined as:

	|x| = sqrt(re(x)^2 + im(x)^2)

@aData
: The values to apply `|x|` to.

# cainteoir::abs2 {: .doc }

Calculate `|x|^2` for each value in the array.

This is used to evaluate the energy spectrum and energy cepstrum of a signal.
It is defined as:

	|x|^2 = sqrt(re(x)^2 + im(x)^2)^2
	      = re(x)^2 + im(x)^2

@aData
: The values to apply `|x|^2` to.

# cainteoir::logr {: .doc }

Calculate `log` on the real part of each value in the array.

This assumes that the complex number only has a real component (e.g. after
performing `abs` or `abs2` on the data).

@aData
: The values to apply `log` to.

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

# cainteoir::window_enumerator {: .doc }

Enumerate over audio samples applying a window function to them.

# cainteoir::window_enumerator::~window_enumerator {: .doc }

Cleanup the enumerator object.

# cainteoir::window_enumerator::next {: .doc }

Get the sample data for the next window.

@return
: `true` if a window was retrieved, `false` otherwise.

# cainteoir::window_enumerator::data {: .doc }

The data associated with the current window in complex form.

# cainteoir::create_s16_window_enumerator {: .doc }

Create a window enumerator over S16 samples.

@aData
: The S16 audio samples to enumerate over.

@aWindow
: The window function to apply to the audio samples.

@aStepSize
: The number of samples each window is offset by.

@return
: An object to enumerate the windows in the audio data.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
