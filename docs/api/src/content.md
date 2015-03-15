# cainteoir::css::length
{: .doc }

Represents an absolute CSS [length](http://www.w3.org/TR/css3-values/#absolute-lengths).

# cainteoir::css::length::type
{: .doc }

The units in which the length is specified.

# cainteoir::css::length::inherit
{: .doc }

The length value is inherited from the parent object.

# cainteoir::css::length::millimeters
{: .doc }

The `mm` unit type.

# cainteoir::css::length::centimeters
{: .doc }

The `cm` unit type.

# cainteoir::css::length::inches
{: .doc }

The `in` unit type.

# cainteoir::css::length::points
{: .doc }

The `pt` unit type.

# cainteoir::css::length::picas
{: .doc }

The `pc` unit type.

# cainteoir::css::length::pixels
{: .doc }

The `px` unit type.

# cainteoir::css::length::length
{: .doc }

Create a default valued length object.

# cainteoir::css::length::length
{: .doc }

Create a length object.

@aValue
: The size of the length.

@aUnits
: The units the length is specified in.

# cainteoir::css::length::as
{: .doc }

Convert the length to the specified units.

@aUnits
: The units to convert to.

@return
: A new length object in the specified units.

# cainteoir::css::length::units
{: .doc }

Gets the units the length is specified in.

@return
: The units the length is specified in.

# cainteoir::css::length::value
{: .doc }

Gets the scalar value of the length.

@return
: The scalar value of the length.

# cainteoir::css::time
{: .doc }

Represents a CSS [time](http://www.w3.org/TR/css3-values/#time).

# cainteoir::css::time::type
{: .doc }

The units in which the time is specified.

# cainteoir::css::time::inherit
{: .doc }

The time value is inherited from the parent object.

# cainteoir::css::time::seconds
{: .doc }

The `s` unit type.

# cainteoir::css::time::milliseconds
{: .doc }

The `ms` unit type.

# cainteoir::css::time::time
{: .doc }

Create a default valued time object.

# cainteoir::css::time::time
{: .doc }

Create a time object.

@aValue
: The duration of the time.

@aUnits
: The units the time is specified in.

# cainteoir::css::time::as
{: .doc }

Convert the time to the specified units.

@aUnits
: The units to convert to.

@return
: A new time object in the specified units.

# cainteoir::css::time::units
{: .doc }

Gets the units the time is specified in.

@return
: The units the time is specified in.

# cainteoir::css::time::value
{: .doc }

Gets the scalar value of the time.

@return
: The scalar value of the time.

# cainteoir::css::parse_smil_time
{: .doc }

Parse a time string in the SMIL time format.

@aValue
: The SMIL time string.

@return
: The parsed time object.

# cainteoir::css::frequency
{: .doc }

Represents a CSS [frequency](http://www.w3.org/TR/css3-values/#frequency).

# cainteoir::css::frequency::type
{: .doc }

The units in which the frequency is specified.

# cainteoir::css::frequency::inherit
{: .doc }

The frequency value is inherited from the parent object.

# cainteoir::css::frequency::hertz
{: .doc }

The `Hz` unit type.

# cainteoir::css::frequency::kilohertz
{: .doc }

The `kHz` unit type.

# cainteoir::css::frequency::frequency
{: .doc }

Create a default valued frequency object.

# cainteoir::css::frequency::frequency
{: .doc }

Create a frequency object.

@aValue
: The frequency.

@aUnits
: The units the frequency is specified in.

# cainteoir::css::frequency::as
{: .doc }

Convert the frequency to the specified units.

@aUnits
: The units to convert to.

@return
: A new frequency object in the specified units.

# cainteoir::css::frequency::units
{: .doc }

Gets the units the frequency is specified in.

@return
: The units the frequency is specified in.

# cainteoir::css::frequency::value
{: .doc }

Gets the scalar value of the frequency.

@return
: The scalar value of the frequency.

# cainteoir::css::parse_frequency
{: .doc }

Parse a frequency value or musical note.

A frequency value is either a `#` or `#.#` formatted number. If no suffix, or
`Hz` is used, the frequency is in hertz; if the `kHz` suffix is used, the
frequency is in kilohertz.

A musical note is the key followed by the octave. The key is one of `A`,
`A#`/`Bb`, `B`, `C`, `C#`/`Db`, `D`, `D#`/`Eb`, `E`, `F`, `F#`/`Gb`, `G` or
`G#`/`Ab` with `#` denoting a sharp and `b` a flat. The octave is a number from
`0` to `8`, with middle C being `C4`. The notes are tuned to:

	A4 = 440Hz

The musical notes are specified to two decimal places, so middle C is `261.63`
instead of `261.625565`.

@aValue
: The frequency to be parsed.

@return
: The parsed frequency object.

# cainteoir::css::display
{: .doc }

Represents the CSS [display](http://www.w3.org/TR/CSS21/visuren.html#display-prop) property.

# cainteoir::css::display::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::display::block
{: .doc }

The object is a block of text.

A block of text is a sequence of text wrapped within a bounding box that has a
specified margin, border and padding applied to it according to the CSS box
model.

# cainteoir::css::display::inlined
{: .doc }

The object is an inline text span.

An inlined text object flows within the parent block that contains it.

# cainteoir::css::display::list_item
{: .doc }

The object is an item in a list.

A list item is a block that contains a gutter with the list item counter value.

# cainteoir::css::display::table
{: .doc }

The object is a table.

# cainteoir::css::display::table_row
{: .doc }

The object is a row within a table.

# cainteoir::css::display::table_cell
{: .doc }

The object is a cell within a table row.

# cainteoir::css::display::none
{: .doc }

The object is not visible in the document.

# cainteoir::css::vertical_align
{: .doc }

Represents the CSS [vertical-align](http://www.w3.org/TR/CSS21/visudet.html#propdef-vertical-align) property.

# cainteoir::css::vertical_align::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::vertical_align::baseline
{: .doc }

The baseline of the text is aligned with the baseline of the parent.

# cainteoir::css::vertical_align::sub
{: .doc }

The baseline of the text is aligned below the baseline of the parent to the
subscript position.

# cainteoir::css::vertical_align::super
{: .doc }

The baseline of the text is aligned below the baseline of the parent to the
superscript position.

# cainteoir::css::text_align
{: .doc }

Represents the CSS [text-align](http://www.w3.org/TR/css3-text/#text-align) property.

# cainteoir::css::text_align::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::text_align::left
{: .doc }

Align the text to the left edge.

# cainteoir::css::text_align::right
{: .doc }

Align the text to the right edge.

# cainteoir::css::text_align::center
{: .doc }

Center the text horizontally.

# cainteoir::css::text_align::justify
{: .doc }

Extend the spacing between words such that the left and right edges line up.

# cainteoir::css::text_decoration
{: .doc }

Represents the CSS [text-decoration-line](http://www.w3.org/TR/css-text-decor-3/#text-decoration-line) property.

# cainteoir::css::text_decoration::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::text_decoration::none
{: .doc }

There is no associated decoration of the text.

# cainteoir::css::text_decoration::underline
{: .doc }

Each row of text contains a line below it, closer than a border line.

# cainteoir::css::text_decoration::line_through
{: .doc }

Each row of text contains a line along the middle.

# cainteoir::css::whitespace
{: .doc }

Represents the CSS [white-space](http://www.w3.org/TR/css3-text/#white-space) property.

# cainteoir::css::whitespace::normal
{: .doc }

Collapse whitespace, wrap long lines.

# cainteoir::css::whitespace::preserved
{: .doc }

Preserve whitespace, don't wrap long lines.

# cainteoir::css::whitespace::preserved_wrap
{: .doc }

Preserve whitespace, wrap long lines.

# cainteoir::css::whitespace::preserved_line
{: .doc }

Preserve line breaks, collapse whitespace, wrap long lines.

# cainteoir::css::whitespace::nowrap
{: .doc }

Collapse whitespace, don't wrap long lines.

# cainteoir::css::media_synchronisation
{: .doc }

Represents a SMIL media synchronisation mode.

# cainteoir::css::media_synchronisation::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::media_synchronisation::sequential
{: .doc }

The child items occur sequentially over time.

# cainteoir::css::media_synchronisation::parallel
{: .doc }

The child items occur at the same time.

This is used to synchronise different media types, such as aligning sentences
in the transcription of an audio file.

# cainteoir::css::role
{: .doc }

This represents the HTML/WAI-ARIA role attribute.

# cainteoir::css::role::none
{: .doc }

No role is associated with this object.

# cainteoir::css::role::heading
{: .doc }

Represents a heading for a part, chapter or section.

# cainteoir::css::role::paragraph
{: .doc }

Represents a paragraph in a document.

# cainteoir::css::role::sentence
{: .doc }

Represents a sentence in a document.

# cainteoir::css::font_style
{: .doc }

Represents the CSS [font-style](http://www.w3.org/TR/css3-fonts/#font-style-prop) property.

# cainteoir::css::font_style::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::font_style::normal
{: .doc }

The font is rendered in its natural orientation.

# cainteoir::css::font_style::italic
{: .doc }

The font is rendered with a slant, preferring an italic face over an oblique one.

# cainteoir::css::font_style::oblique
{: .doc }

The font is rendered with a slant, preferring an oblique face over an italic one.

# cainteoir::css::font_variant_caps
{: .doc }

Represents the CSS [font-variant-caps](http://www.w3.org/TR/css3-fonts/#font-variant-caps-prop) property.

# cainteoir::css::font_variant_caps::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::font_variant_caps::normal
{: .doc }

Do not use any special capitalization features.

# cainteoir::css::font_variant_caps::small_caps
{: .doc }

Render lower case letters in capitals at the lower letter height.

# cainteoir::css::font_weight
{: .doc }

Represents the CSS [font-weight](http://www.w3.org/TR/css3-fonts/#font-style-prop) property.

With the exception of `inherit`, `bolder` and `lighter`, these named weights
have their associated weight values from `100`-`900`.

# cainteoir::css::font_weight::inherit
{: .doc }

The value is inherited from the parent object.

# cainteoir::css::font_weight::bolder
{: .doc }

The font weight is higer (bolder) than the parent.

# cainteoir::css::font_weight::lighter
{: .doc }

The font weight is lower (lighter) than the parent.

# cainteoir::css::font_weight::thin
{: .doc }

100

# cainteoir::css::font_weight::ultra_light
{: .doc }

200

# cainteoir::css::font_weight::light
{: .doc }

300

# cainteoir::css::font_weight::normal
{: .doc }

400

# cainteoir::css::font_weight::medium
{: .doc }

500

# cainteoir::css::font_weight::semi_bold
{: .doc }

600

# cainteoir::css::font_weight::bold
{: .doc }

700

# cainteoir::css::font_weight::ultra_bold
{: .doc }

800

# cainteoir::css::font_weight::heavy
{: .doc }

900

# cainteoir::css::box
{: .doc }

Represents a box (rectangle) in the CSS [box model](http://www.w3.org/TR/CSS21/box.html).

# cainteoir::css::box::left
{: .doc }

The left edge.

# cainteoir::css::box::top
{: .doc }

The top edge.

# cainteoir::css::box::right
{: .doc }

The right edge.

# cainteoir::css::box::bottom
{: .doc }

The bottom edge.

# cainteoir::css::counter_system
{: .doc }

The algorithm used to construct the counter's representation.

# cainteoir::css::counter_system::cyclic
{: .doc }

Repeatedly cycles through the specified symbols.

# cainteoir::css::counter_system::fixed
{: .doc }

Runs through the symbols once, then falls back.

# cainteoir::css::counter_system::symbolic
{: .doc }

As cyclic, but doubles, triples, etc. the symbols on each pass.

# cainteoir::css::counter_system::alphabetic
{: .doc }

As with numeric, but using an alphabet instead of numeric digits.

# cainteoir::css::counter_system::numeric
{: .doc }

Interprets the symbols as digits to a place value numbering system.

# cainteoir::css::counter_system::additive
{: .doc }

Uses a combination of symbols where their sum is the counter value.

# cainteoir::css::counter_style
{: .doc }

Represents a `@counter-style` CSS rule.

# cainteoir::css::counter_style::value_t
{: .doc }

The type used for counter values.

# cainteoir::css::counter_style::range_t
{: .doc }

The type used to specify counter ranges.

# cainteoir::css::counter_style::system
{: .doc }

The algorithm to use to format the counter value.

# cainteoir::css::counter_style::initial_symbol_value
{: .doc }

The value of the first symbol (for the fixed system).

# cainteoir::css::counter_style::symbols
{: .doc }

The set of symbols to use for non-additive systems.

# cainteoir::css::counter_style::additive_symbols
{: .doc }

The set of symbols to use for the additive system.

# cainteoir::css::counter_style::negative_prefix
{: .doc }

The string to use to denote negative numbers (before the number).

# cainteoir::css::counter_style::negative_suffix
{: .doc }

The string to use to denote negative numbers (after the number).

# cainteoir::css::counter_style::prefix
{: .doc }

The string to use at the start of the formatted counter value.

# cainteoir::css::counter_style::suffix
{: .doc }

The string to use at the end of the formatted counter value.

# cainteoir::css::counter_style::range
{: .doc }

Defines the range over which the counter style is defined.

# cainteoir::css::counter_style::fallback
{: .doc }

The counter to use when this counter cannot represent a value.

# cainteoir::css::counter_style::infinite
{: .doc }

The value for the `infinite infinite` range.

# cainteoir::css::counter_style::get_auto_range
{: .doc }

The range to use for auto ranges based on the counter system.

@system
: The counter system to get the auto range for.

@return
: The range to use for the auto range.

# cainteoir::css::counter_style::counter_style
{: .doc }

Construct a counter style with default values.

# cainteoir::css::counter_style::marker
{: .doc }

Format a count value using the counter style.

@count
: The numeric value of the counter to format.

@return
: The formatted value.

# cainteoir::css::styles
{: .doc }

Represents a CSS style definition.

# cainteoir::css::styles::name
{: .doc }

The name of the style.

# cainteoir::css::styles::display
{: .doc }

The CSS [display](http://www.w3.org/TR/CSS21/visuren.html#display-prop) property value.

# cainteoir::css::styles::media_synchronisation
{: .doc }

The SMIL media synchronisation mode.

# cainteoir::css::styles::vertical_align
{: .doc }

The CSS [vertical-align](http://www.w3.org/TR/CSS21/visudet.html#propdef-vertical-align) property value.

# cainteoir::css::styles::text_align
{: .doc }

The CSS [text-align](http://www.w3.org/TR/css3-text/#text-align) property value.

# cainteoir::css::styles::text_decoration
{: .doc }

The CSS [text-decoration-line](http://www.w3.org/TR/css-text-decor-3/#text-decoration-line) property value.

# cainteoir::css::styles::whitespace
{: .doc }

The CSS [white-space](http://www.w3.org/TR/css3-text/#white-space) property value.

# cainteoir::css::styles::font_style
{: .doc }

The CSS [font-style](http://www.w3.org/TR/css3-fonts/#font-style-prop) property value.

# cainteoir::css::styles::font_variant_caps
{: .doc }

The CSS [font-variant-caps](http://www.w3.org/TR/css3-fonts/#font-variant-caps-prop) property value.

# cainteoir::css::styles::font_weight
{: .doc }

The CSS [font-weight](http://www.w3.org/TR/css3-fonts/#font-style-prop) property value.

# cainteoir::css::styles::list_style_type
{: .doc }

The CSS [list-style-type](http://www.w3.org/TR/css3-lists/#propdef-list-style-type) property value, referencing a counter style.

# cainteoir::css::styles::font_family
{: .doc }

The CSS [font-family](http://www.w3.org/TR/css3-fonts/#font-family-prop) property value.

# cainteoir::css::styles::font_size
{: .doc }

The CSS [font-size](http://www.w3.org/TR/css3-fonts/#font-size-prop) property value.

# cainteoir::css::styles::margin
{: .doc }

The CSS [margin](http://www.w3.org/TR/CSS21/box.html#margin-properties) property value.

# cainteoir::css::styles::role
{: .doc }

The HTML or WAI-ARIA role property.

# cainteoir::css::styles::aria_level
{: .doc }

The WAI-ARIA [aria-level](http://www.w3.org/TR/wai-aria/states_and_properties#aria-level) property.

This is used in conjunction with the `heading` role to specify the heading level.

# cainteoir::css::styles::styles
{: .doc }

Create a style object with default values.

@aName
: The name of the style.

# cainteoir::css::styles::styles
{: .doc }

Create a style object with default values.

@aName
: The name of the style.

@aDisplay
: The CSS [display](http://www.w3.org/TR/CSS21/visuren.html#display-prop) property value.

@aMediaSynchronisation
: The SMIL media synchronisation mode.

@aVerticalAlign
: The CSS [vertical-align](http://www.w3.org/TR/CSS21/visudet.html#propdef-vertical-align) property value.

@aTextAlign
: The CSS [text-align](http://www.w3.org/TR/css3-text/#text-align) property value.

@aTextDecoration
: The CSS [text-decoration-line](http://www.w3.org/TR/css-text-decor-3/#text-decoration-line) property value.

@aWhiteSpace
: The CSS [white-space](http://www.w3.org/TR/css3-text/#white-space) property value.

@aFontStyle
: The CSS [font-style](http://www.w3.org/TR/css3-fonts/#font-style-prop) property value.

@aFontVariantCaps
: The CSS [font-variant-caps](http://www.w3.org/TR/css3-fonts/#font-variant-caps-prop) property value.

@aFontWeight
: The CSS [font-weight](http://www.w3.org/TR/css3-fonts/#font-style-prop) property value.

@aListStyleType
: The CSS [list-style-type](http://www.w3.org/TR/css3-lists/#propdef-list-style-type) property value, referencing a counter style.

@aFontFamily
: The CSS [font-family](http://www.w3.org/TR/css3-fonts/#font-family-prop) property value.

@aFontSize
: The CSS [font-size](http://www.w3.org/TR/css3-fonts/#font-size-prop) property value.

@aMargin
: The CSS [margin](http://www.w3.org/TR/CSS21/box.html#margin-properties) property value.

@aRole
: The HTML or WAI-ARIA role property.

@aAriaLevel
: The WAI-ARIA [aria-level](http://www.w3.org/TR/wai-aria/states_and_properties#aria-level) property.

# cainteoir::css::reader
{: .doc }

A reader-style CSS tokenizer.

# cainteoir::css::reader::token_type
{: .doc }

Specifies which token is being referenced.

# cainteoir::css::reader::identifier
{: .doc }

A CSS [ident-token](http://www.w3.org/TR/css3-syntax/#ident-token-diagram).

# cainteoir::css::reader::at_keyword
{: .doc }

A CSS [at-keyword-token](http://www.w3.org/TR/css3-syntax/#at-keyword-token-diagram).

# cainteoir::css::reader::string
{: .doc }

A CSS [string-token](http://www.w3.org/TR/css3-syntax/#string-token-diagram).

# cainteoir::css::reader::integer
{: .doc }

A CSS [number-token](http://www.w3.org/TR/css3-syntax/#number-token-diagram) that is an integer value.

# cainteoir::css::reader::open_block
{: .doc }

A CSS [{-token](http://www.w3.org/TR/css3-syntax/#tokendef-open-curly); the `{` character.

# cainteoir::css::reader::close_block
{: .doc }

A CSS [}-token](http://www.w3.org/TR/css3-syntax/#tokendef-close-curly); the `}` character.

# cainteoir::css::reader::colon
{: .doc }

A CSS [colon-token](http://www.w3.org/TR/css3-syntax/#typedef-colon-token); the `:` character.

# cainteoir::css::reader::semicolon
{: .doc }

A CSS [semicolon-token](http://www.w3.org/TR/css3-syntax/#typedef-semicolon-token); the `;` character.

# cainteoir::css::reader::comma
{: .doc }

A CSS [comma-token](http://www.w3.org/TR/css3-syntax/#typedef-comma-token); the `,` character.

# cainteoir::css::reader::error
{: .doc }

An unknown or invalid character or character sequence.

# cainteoir::css::reader::value
{: .doc }

The string representation of the currently read token value.

# cainteoir::css::reader::type
{: .doc }

The type of the currently read text value.

# cainteoir::css::reader::read
{: .doc }

Read the next token in the data buffer.

# cainteoir::css::reader::reader
{: .doc }

Create a new CSS tokenizer.

@aData
: The data buffer containing the CSS text to be tokenized.

# cainteoir::css::style_manager
{: .doc }

Access the styles read from one or more CSS files.

# cainteoir::css::style_manager::get_counter_style
{: .doc }

Get the named [@counter-style](http://dev.w3.org/csswg/css-counter-styles/#the-counter-style-rule) rule.

@aName
: The name of the counter style.

@return
: The associated counter style, or `nullptr` if the counter style does not exist.

# cainteoir::css::style_manager::create_counter_style
{: .doc }

Create a [@counter-style](http://dev.w3.org/csswg/css-counter-styles/#the-counter-style-rule) rule.

@aName
: The name of the counter style.

If a counter style with this name already exists, that style rule is replaced
by the new object.

@return
: The new counter style object.

# cainteoir::css::style_manager::counter_styles
{: .doc }

Get all [@counter-style](http://dev.w3.org/csswg/css-counter-styles/#the-counter-style-rule) rules.

@return
: The [@counter-style](http://dev.w3.org/csswg/css-counter-styles/#the-counter-style-rule) rules in this style manager.

# cainteoir::css::style_manager::parse
{: .doc }

Add the CSS rules from the file to this style manager.

This supports the following rules:

 * [@counter-style](http://dev.w3.org/csswg/css-counter-styles/#the-counter-style-rule)

@css_file
: The name of the CSS file to parse.

# cainteoir::css::style_manager::parse
{: .doc }

Add the CSS rules from the file to this style manager.

This supports the following rules:

 * [@counter-style](http://dev.w3.org/csswg/css-counter-styles/#the-counter-style-rule)

@style
: The contents of the CSS file to parse.

# cainteoir::unknown
{: .doc }

The default styles for an unknown tag.

# cainteoir::sequential
{: .doc }

The default styles for a SMIL `seq` tag.

# cainteoir::parallel
{: .doc }

The default styles for a SMIL `par` tag.

# cainteoir::hidden
{: .doc }

The default styles for a hidden tag.

# cainteoir::block
{: .doc }

The default styles for a block displayed tag.

# cainteoir::paragraph
{: .doc }

The default styles for a block displayed paragraph tag with a bottom margin.

# cainteoir::heading0
{: .doc }

The default styles for a level 0 heading tag.

# cainteoir::heading1
{: .doc }

The default styles for a level 1 heading tag.

# cainteoir::heading2
{: .doc }

The default styles for a level 2 heading tag.

# cainteoir::heading3
{: .doc }

The default styles for a level 3 heading tag.

# cainteoir::heading4
{: .doc }

The default styles for a level 4 heading tag.

# cainteoir::heading5
{: .doc }

The default styles for a level 5 heading tag.

# cainteoir::heading6
{: .doc }

The default styles for a level 6 heading tag.

# cainteoir::span
{: .doc }

The default styles for an inline displayed tag.

# cainteoir::sentence
{: .doc }

The default styles for a sentence tag.

# cainteoir::superscript
{: .doc }

The default styles for a superscript tag.

# cainteoir::subscript
{: .doc }

The default styles for a subscript tag.

# cainteoir::emphasized
{: .doc }

The default styles for an emphasized (italic) inline displayed tag.

# cainteoir::emphasized_block
{: .doc }

The default styles for an emphasized (italic) block displayed tag.

# cainteoir::strong
{: .doc }

The default styles for a strong (bold) inline displayed tag.

# cainteoir::reduced
{: .doc }

The default styles for an inline displayed tag with reduced emphasis.

# cainteoir::underlined
{: .doc }

The default styles for an inline displayed tag with an underline.

# cainteoir::monospace
{: .doc }

The default styles for an inline displayed tag in a monospace font.

# cainteoir::monospace_block
{: .doc }

The default styles for a block displayed tag in a monospace font.

# cainteoir::bullet_list
{: .doc }

The default styles for an unordered list.

# cainteoir::number_list
{: .doc }

The default styles for an ordered list.

# cainteoir::list_item
{: .doc }

The default styles for an item in an ordered or unordered list.

# cainteoir::table
{: .doc }

The default styles for a table.

# cainteoir::table_row
{: .doc }

The default styles for a table row within a table.

# cainteoir::table_cell
{: .doc }

The default styles for a table cell within a table row.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
