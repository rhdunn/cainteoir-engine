/* Cainteoir Engine documentation.
 *
 * Copyright (C) 2013 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @mainpage
  *
  * The Cainteoir Text-to-Speech Engine API provides support for
  * @ref document_reader "reading documents" in a variety of formats, controlling
  * @ref tts "Text-to-Speech engines" and feeding the generated audio to an
  * @ref audio "audio file or device".
  */

/** @defgroup archives Archives
  * @brief    Support for reading the content of archive files.
  */

/** @defgroup audio Audio
  * @brief    Manage writing audio data to an audio device or file.
  */

/** @defgroup buffers Buffers
  * @brief    Magage data or string buffers.
  */

/** @defgroup encoding Character Encoding
  * @brief    Helpers for working with UTF-8 and other character encodings.
  */

/** @defgroup css CSS
  * @brief    Cascading Style Sheet implementation.
  *
  * This provides an implementation of the W3C Cascading Style Sheet specifications
  * that is focused on providing this information to the text-to-speech engines
  * (for controlling pronunciation and prosody) and rendering engines to display
  * the document content in a graphical interface.
  *
  * Where the Level 3 and Level 4 CSS specifications have been broken out into
  * separate standards, the API is grouped into a separate module corresponding
  * to the broken out specification. These modules are:
  *
  *   1.  @ref css_units "CSS Values and Units";
  *   2.  @ref css_counter_styles "CSS Counter Styles".
  */

/** @defgroup css_counter_styles CSS Counter Styles
  * @brief    Implementation of the CSS Counter Styles Level 3 specification.
  * @see      http://www.w3.org/TR/css-counter-styles-3/
  *
  * This is the C++ API to CSS counter styles as defined in the CSS Counter
  * Styles Level 3 specification. The CSS parsing of \@counter-style rules
  * is handled in the cainteoir::css::style_manager::parse method.
  */

/** @defgroup css_units CSS Values and Units
  * @brief    Implementation of the CSS Values and Units Level 3 specification.
  * @see      http://www.w3.org/TR/css3-values/
  *
  * This is the C++ API to the various unit types for CSS values.
  */

/** @defgroup decoders Decoding and Decompression
  * @brief    Decode encoded data and decompress compressed data.
  */

/** @defgroup document_reader Document Reader
  * @brief    Extract information from a supported document format in a reader-style API.
  */

/** @defgroup languages Languages
  * @brief    BCP 47 / RFC 5646 language, region and variant tag processing.
  */

/** @defgroup mime MIME
  * @brief    MIME type handling.
  * @see      http://standards.freedesktop.org/shared-mime-info-spec/0.20/
  */

/** @defgroup ranges Ranges
  * @brief    C++11 range-based for loop helpers.
  */

/** @defgroup rdf RDF Model
  * @brief    A C++ representation of the RDF data model.
  */

/** @defgroup rdf_query RDF Query
  * @brief    Perform operations on an RDF triple store.
  */

/** @defgroup rdf_selectors RDF Selectors
  * @brief    Selectors used to match RDF triples.
  */

/** @defgroup tts Text-to-Speech
  * @brief    High-level interface to Text-to-Speech engines and voices.
  */

/** @defgroup xml XML
  * @brief    XML document processing.
  */
