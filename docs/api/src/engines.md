#<cldoc:cainteoir::tts::speech>

Manage a speech synthesis session.

#<cldoc:cainteoir::tts::speech::~speech>

Clean up the speech object.

#<cldoc:cainteoir::tts::speech::is_speaking>

Is the session currently speaking?

@return `true` if the session is currently speaking, `false` otherwise.

#<cldoc:cainteoir::tts::speech::stop>

Immediately stop the session from speaking.

#<cldoc:cainteoir::tts::speech::wait>

Wait until the session has finished speaking.

#<cldoc:cainteoir::tts::speech::elapsedTime>

Get the time elapsed since starting this session.

@return The time elapsed since starting this session.

#<cldoc:cainteoir::tts::speech::totalTime>

Get the (estimated) total time for this session.

@return The (estimated) total time for this session.

#<cldoc:cainteoir::tts::speech::completed>

Get the percentage of this session completed.

@return The percentage of this session completed.

#<cldoc:cainteoir::tts::speech::position>

Get the current speaking position in the document for this session.

@return The current speaking position in the document for this session.

#<cldoc:cainteoir::tts::speech::error_message>

Get the error message associated with this session.

If there is no error message associated with this session, the return value
is empty.

@return The error message associated with this session.

#<cldoc:cainteoir::tts::speech::context>

Get the toc entry the currently reading text is located in.

This can be used by a text-to-speech UI to provide feedback of which section
of the document is currently being read.

@return The toc entry the currently reading text is located in.

#<cldoc:cainteoir::tts::parameter::type>

Identifies the parameter.

#<cldoc:cainteoir::tts::parameter::rate>

How fast the voice speaks.

#<cldoc:cainteoir::tts::parameter::volume>

The volume of the voice audio.

#<cldoc:cainteoir::tts::parameter::pitch>

The base pitch to render the voice at.

#<cldoc:cainteoir::tts::parameter::pitch_range>

How varied the pitch is due to prosody variations.

#<cldoc:cainteoir::tts::parameter::word_gap>

How long to pause between each word.

#<cldoc:cainteoir::tts::parameter::number_of_parameters>

The number of parameters in the parameter type enumeration.

#<cldoc:cainteoir::tts::parameter>

A configurable option of a text-to-speech engine or voice.

#<cldoc:cainteoir::tts::parameter::~parameter>

Clean up the parameter object.

#<cldoc:cainteoir::tts::parameter::name>

Get the name of the parameter.

@return The name of the parameter.

#<cldoc:cainteoir::tts::parameter::units>

Get the units the parameter is stored in.

@return The units the parameter is stored in.

#<cldoc:cainteoir::tts::parameter::minimum>

Get the minimum value for the parameter.

@return The minimum value for the parameter.

#<cldoc:cainteoir::tts::parameter::maximum>

Get the maximum value for the parameter.

@return The maximum value for the parameter.

#<cldoc:cainteoir::tts::parameter::default_value>

Get the default value for the parameter.

@return The default value for the parameter.

#<cldoc:cainteoir::tts::parameter::value>

Get the current value for the parameter.

@return The current value for the parameter.

#<cldoc:cainteoir::tts::parameter::set_value>

Set the current value for the parameter.
@value The new value of the parameter.

@return `true` if the parameter value was changed, `false` otherwise.

#<cldoc:cainteoir::tts::media_overlays_mode>

The behaviour of the Text-to-Speech engine in the presence of ePub 3 media overlays.

#<cldoc:cainteoir::tts::media_overlays_mode::tts_only>

Use the active Text-to-Speech voice to read all the document text.

#<cldoc:cainteoir::tts::media_overlays_mode::media_overlays_only>

Only use the media overlays audio; don't read any text not annotated with media overlays.

#<cldoc:cainteoir::tts::media_overlays_mode::tts_and_media_overlays>

Use media overlays when the text is annotated with them, otherwise use the active Text-to-Speech voice.

#<cldoc:cainteoir::tts::engines>

Manages the available text-to-speech engines.

#<cldoc:cainteoir::tts::engines::engines>

Create a new text-to-speech engine manager object.
@metadata The RDF graph to add engine and voice metadata to.

#<cldoc:cainteoir::tts::engines::~engines>

Clean up the engines.

#<cldoc:tts::engines::select_voice>

Change the voice used to read the documents.
@aMetadata The RDF graph containing the engine and voice metadata.
@aVoice    The voice to use to read the documents.

@return `true` if the voice was changed, `false` otherwise.

#<cldoc:cainteoir::tts::engines::voice>

Get the currently active voice.

@return The currently active voice.

#<cldoc:cainteoir::tts::engines::speak>

Speak the specified document.
@out            The audio output device (for reading) or file (for recording).
@aListing       The table of contents for the document being read.
@aRange         The start and end points of the document to read.
@aMediaOverlays The behaviour to use when ePub 3 media overlays are encountered.

@return The object associated with this speech session.

#<cldoc:cainteoir::tts::engines::pronunciation>

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

@return An object that can convert text to phonemes.

#<cldoc:cainteoir::tts::engines::parameter>

Get the specified parameter associated with the active engine.
@aType The parameter to get.

@return The specified parameter.

#<cldoc:cainteoir::tts::get_voice_uri>

Get the RDF statement containing information about the TTS voice.
@aMetadata The RDF graph containing the TTS metadata.
@predicate The RDF predicate to check.
@value     The content of the RDF object to match against.

@return The RDF statement containing information about the TTS voice, or
        `nullptr` if no matching voice was found.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
