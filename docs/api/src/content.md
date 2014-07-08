#<cldoc:cainteoir::css::length>

Represents a CSS length.

#<cldoc:cainteoir::css::length::type>

The units in which the length is specified.

#<cldoc:cainteoir::css::length::inherit>

The length value is inherited from the parent object.

#<cldoc:cainteoir::css::length::millimeters>

The `mm` unit type.

#<cldoc:cainteoir::css::length::centimeters>

The `cm` unit type.

#<cldoc:cainteoir::css::length::inches>

The `in` unit type.

#<cldoc:cainteoir::css::length::points>

The `pt` unit type.

#<cldoc:cainteoir::css::length::picas>

The `pc` unit type.

#<cldoc:cainteoir::css::length::pixels>

The `px` unit type.

#<cldoc:cainteoir::css::length::length>

Create a default valued length object.

#<cldoc:cainteoir::css::length::length>

Create a length object.
@aValue The size of the length.
@aUnits The units the length is specified in.

#<cldoc:cainteoir::css::length::as>

Convert the length to the specified units.
@aUnits The units to convert to.

@return A new length object in the specified units.

#<cldoc:cainteoir::css::length::units>

Gets the units the length is specified in.

@return The units the length is specified in.

#<cldoc:cainteoir::css::length::value>

Gets the scalar value of the length.

@return The scalar value of the length.

#<cldoc:cainteoir::css::box>

Represents a CSS rectangle.

#<cldoc:cainteoir::css::box::left>

The left edge.

#<cldoc:cainteoir::css::box::top>

The top edge.

#<cldoc:cainteoir::css::box::right>

The right edge.

#<cldoc:cainteoir::css::box::bottom>

The bottom edge.

#<cldoc:cainteoir::css::counter_system>

The algorithm used to construct the counter's representation.

#<cldoc:cainteoir::css::counter_system::cyclic>

Repeatedly cycles through the specified symbols.

#<cldoc:cainteoir::css::counter_system::fixed>

Runs through the symbols once, then falls back.

#<cldoc:cainteoir::css::counter_system::symbolic>

As cyclic, but doubles, triples, etc. the symbols on each pass.

#<cldoc:cainteoir::css::counter_system::alphabetic>

As with numeric, but using an alphabet instead of numeric digits.

#<cldoc:cainteoir::css::counter_system::numeric>

Interprets the symbols as digits to a place value numbering system.

#<cldoc:cainteoir::css::counter_system::additive>

Uses a combination of symbols where their sum is the counter value.

#<cldoc:cainteoir::css::counter_style>

Represents a `@counter-style` CSS rule.

#<cldoc:cainteoir::css::counter_style::value_t>

The type used for counter values.

#<cldoc:cainteoir::css::counter_style::range_t>

The type used to specify counter ranges.

#<cldoc:cainteoir::css::counter_style::system>

The algorithm to use to format the counter value.

#<cldoc:cainteoir::css::counter_style::initial_symbol_value>

The value of the first symbol (for the fixed system).

#<cldoc:cainteoir::css::counter_style::symbols>

The set of symbols to use for non-additive systems.

#<cldoc:cainteoir::css::counter_style::additive_symbols>

The set of symbols to use for the additive system.

#<cldoc:cainteoir::css::counter_style::negative_prefix>

The string to use to denote negative numbers (before the number).

#<cldoc:cainteoir::css::counter_style::negative_suffix>

The string to use to denote negative numbers (after the number).

#<cldoc:cainteoir::css::counter_style::prefix>

The string to use at the start of the formatted counter value.

#<cldoc:cainteoir::css::counter_style::suffix>

The string to use at the end of the formatted counter value.

#<cldoc:cainteoir::css::counter_style::range>

Defines the range over which the counter style is defined.

#<cldoc:cainteoir::css::counter_style::fallback>

The counter to use when this counter cannot represent a value.

#<cldoc:cainteoir::css::counter_style::infinite>

The value for the `infinite infinite` range.

#<cldoc:cainteoir::css::counter_style::get_auto_range>

The range to use for auto ranges based on the counter system.
@system The counter system to get the auto range for.

@return The range to use for the auto range.

#<cldoc:cainteoir::css::counter_style::counter_style>

Construct a counter style with default values.

#<cldoc:cainteoir::css::counter_style::marker>

Format a count value using the counter style.
@count The numeric value of the counter to format.

@return The formatted value.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
