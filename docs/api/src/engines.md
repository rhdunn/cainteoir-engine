# cainteoir::tts::speech
{: .doc }

Manage a speech synthesis session.

# cainteoir::tts::speech::~speech
{: .doc }

Clean up the speech object.

# cainteoir::tts::speech::is_speaking
{: .doc }

Is the session currently speaking?

@return
: `true` if the session is currently speaking, `false` otherwise.

# cainteoir::tts::speech::stop
{: .doc }

Immediately stop the session from speaking.

# cainteoir::tts::speech::wait
{: .doc }

Wait until the session has finished speaking.

# cainteoir::tts::speech::elapsedTime
{: .doc }

Get the time elapsed since starting this session.

@return
: The time elapsed since starting this session.

# cainteoir::tts::speech::totalTime
{: .doc }

Get the (estimated) total time for this session.

@return
: The (estimated) total time for this session.

# cainteoir::tts::speech::completed
{: .doc }

Get the percentage of this session completed.

@return
: The percentage of this session completed.

# cainteoir::tts::speech::position
{: .doc }

Get the current speaking position in the document for this session.

@return
: The current speaking position in the document for this session.

# cainteoir::tts::speech::error_message
{: .doc }

Get the error message associated with this session.

If there is no error message associated with this session, the return value
is empty.

@return
: The error message associated with this session.

# cainteoir::tts::speech::context
{: .doc }

Get the toc entry the currently reading text is located in.

This can be used by a text-to-speech UI to provide feedback of which section
of the document is currently being read.

@return
: The toc entry the currently reading text is located in.

# cainteoir::tts::parameter::type
{: .doc }

Identifies the parameter.

# cainteoir::tts::parameter::rate
{: .doc }

How fast the voice speaks.

# cainteoir::tts::parameter::volume
{: .doc }

The volume of the voice audio.

# cainteoir::tts::parameter::pitch
{: .doc }

The base pitch to render the voice at.

# cainteoir::tts::parameter::pitch_range
{: .doc }

How varied the pitch is due to prosody variations.

# cainteoir::tts::parameter::word_gap
{: .doc }

How long to pause between each word.

# cainteoir::tts::parameter::number_of_parameters
{: .doc }

The number of parameters in the parameter type enumeration.

# cainteoir::tts::parameter
{: .doc }

A configurable option of a text-to-speech engine or voice.

# cainteoir::tts::parameter::~parameter
{: .doc }

Clean up the parameter object.

# cainteoir::tts::parameter::name
{: .doc }

Get the name of the parameter.

@return
: The name of the parameter.

# cainteoir::tts::parameter::units
{: .doc }

Get the units the parameter is stored in.

@return
: The units the parameter is stored in.

# cainteoir::tts::parameter::minimum
{: .doc }

Get the minimum value for the parameter.

@return
: The minimum value for the parameter.

# cainteoir::tts::parameter::maximum
{: .doc }

Get the maximum value for the parameter.

@return
: The maximum value for the parameter.

# cainteoir::tts::parameter::default_value
{: .doc }

Get the default value for the parameter.

@return
: The default value for the parameter.

# cainteoir::tts::parameter::value
{: .doc }

Get the current value for the parameter.

@return
: The current value for the parameter.

# cainteoir::tts::parameter::set_value
{: .doc }

Set the current value for the parameter.

@value
: The new value of the parameter.

@return
: `true` if the parameter value was changed, `false` otherwise.

# cainteoir::tts::media_overlays_mode
{: .doc }

The behaviour of the Text-to-Speech engine in the presence of ePub 3 media overlays.

# cainteoir::tts::media_overlays_mode::tts_only
{: .doc }

Use the active Text-to-Speech voice to read all the document text.

# cainteoir::tts::media_overlays_mode::media_overlays_only
{: .doc }

Only use the media overlays audio; don't read any text not annotated with
media overlays.

# cainteoir::tts::media_overlays_mode::tts_and_media_overlays
{: .doc }

Use media overlays when the text is annotated with them, otherwise use the
active Text-to-Speech voice.

# cainteoir::tts::engines
{: .doc }

Manages the available text-to-speech engines.

# cainteoir::tts::engines::engines
{: .doc }

Create a new text-to-speech engine manager object.

@metadata
: The RDF graph to add engine and voice metadata to.

# cainteoir::tts::engines::~engines
{: .doc }

Clean up the engines.

# cainteoir::tts::engines::select_voice
{: .doc }

Change the voice used to read the documents.

@aMetadata
: The RDF graph containing the engine and voice metadata.

@aVoice
: The voice to use to read the documents.

@return
: `true` if the voice was changed, `false` otherwise.

# cainteoir::tts::engines::voice
{: .doc }

Get the currently active voice.

@return
: The currently active voice.

# cainteoir::tts::engines::speak
{: .doc }

Speak the specified document.

@out
: The audio output device (for reading) or file (for recording).

@aListing
: The table of contents for the document being read.

@aRange
: The start and end points of the document to read.

@aMediaOverlays
: The behaviour to use when ePub 3 media overlays are encountered.

@return
: The object associated with this speech session.

# cainteoir::tts::engines::pronunciation
{: .doc }

Create an object to convert text to phonemes.

This uses the text-to-speech engines text to phoneme logic if available to
pronounce text or words. For example:

	cainteoir::rdf::graph metadata;
	cainteoir::tts::engines tts(metadata);

	auto ipa = cainteoir::tts::createPhonemeWriter("ipa");
	ipa->reset(stdout);

	auto pronounce = tts.pronunciation();
	pronounce->reset(std::make_shared<cainteoir::buffer>("Hello world!"));
	while (pronounce->read())
		ipa->write(*pronounce);

@return
: An object that can convert text to phonemes.

# cainteoir::tts::engines::parameter
{: .doc }

Get the specified parameter associated with the active engine.

@aType
: The parameter to get.

@return
: The specified parameter.

# cainteoir::tts::get_voice_uri
{: .doc }

Get the RDF statement containing information about the TTS voice.

@aMetadata
: The RDF graph containing the TTS metadata.

@predicate
: The RDF predicate to check.

@value
: The content of the RDF object to match against.

@return
: The RDF statement containing information about the TTS voice, or `nullptr` if
  no matching voice was found.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
