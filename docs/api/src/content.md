# cainteoir::css::length {: .doc }

Represents a CSS length.

# cainteoir::css::length::type {: .doc }

The units in which the length is specified.

# cainteoir::css::length::inherit {: .doc }

The length value is inherited from the parent object.

# cainteoir::css::length::millimeters {: .doc }

The `mm` unit type.

# cainteoir::css::length::centimeters {: .doc }

The `cm` unit type.

# cainteoir::css::length::inches {: .doc }

The `in` unit type.

# cainteoir::css::length::points {: .doc }

The `pt` unit type.

# cainteoir::css::length::picas {: .doc }

The `pc` unit type.

# cainteoir::css::length::pixels {: .doc }

The `px` unit type.

# cainteoir::css::length::length {: .doc }

Create a default valued length object.

# cainteoir::css::length::length {: .doc }

Create a length object.

@aValue
: The size of the length.

@aUnits
: The units the length is specified in.

# cainteoir::css::length::as {: .doc }

Convert the length to the specified units.

@aUnits
: The units to convert to.

@return
: A new length object in the specified units.

# cainteoir::css::length::units {: .doc }

Gets the units the length is specified in.

@return
: The units the length is specified in.

# cainteoir::css::length::value {: .doc }

Gets the scalar value of the length.

@return
: The scalar value of the length.

# cainteoir::css::box {: .doc }

Represents a CSS rectangle.

# cainteoir::css::box::left {: .doc }

The left edge.

# cainteoir::css::box::top {: .doc }

The top edge.

# cainteoir::css::box::right {: .doc }

The right edge.

# cainteoir::css::box::bottom {: .doc }

The bottom edge.

# cainteoir::css::counter_system {: .doc }

The algorithm used to construct the counter's representation.

# cainteoir::css::counter_system::cyclic {: .doc }

Repeatedly cycles through the specified symbols.

# cainteoir::css::counter_system::fixed {: .doc }

Runs through the symbols once, then falls back.

# cainteoir::css::counter_system::symbolic {: .doc }

As cyclic, but doubles, triples, etc. the symbols on each pass.

# cainteoir::css::counter_system::alphabetic {: .doc }

As with numeric, but using an alphabet instead of numeric digits.

# cainteoir::css::counter_system::numeric {: .doc }

Interprets the symbols as digits to a place value numbering system.

# cainteoir::css::counter_system::additive {: .doc }

Uses a combination of symbols where their sum is the counter value.

# cainteoir::css::counter_style {: .doc }

Represents a `@counter-style` CSS rule.

# cainteoir::css::counter_style::value_t {: .doc }

The type used for counter values.

# cainteoir::css::counter_style::range_t {: .doc }

The type used to specify counter ranges.

# cainteoir::css::counter_style::system {: .doc }

The algorithm to use to format the counter value.

# cainteoir::css::counter_style::initial_symbol_value {: .doc }

The value of the first symbol (for the fixed system).

# cainteoir::css::counter_style::symbols {: .doc }

The set of symbols to use for non-additive systems.

# cainteoir::css::counter_style::additive_symbols {: .doc }

The set of symbols to use for the additive system.

# cainteoir::css::counter_style::negative_prefix {: .doc }

The string to use to denote negative numbers (before the number).

# cainteoir::css::counter_style::negative_suffix {: .doc }

The string to use to denote negative numbers (after the number).

# cainteoir::css::counter_style::prefix {: .doc }

The string to use at the start of the formatted counter value.

# cainteoir::css::counter_style::suffix {: .doc }

The string to use at the end of the formatted counter value.

# cainteoir::css::counter_style::range {: .doc }

Defines the range over which the counter style is defined.

# cainteoir::css::counter_style::fallback {: .doc }

The counter to use when this counter cannot represent a value.

# cainteoir::css::counter_style::infinite {: .doc }

The value for the `infinite infinite` range.

# cainteoir::css::counter_style::get_auto_range {: .doc }

The range to use for auto ranges based on the counter system.

@system
: The counter system to get the auto range for.

@return
: The range to use for the auto range.

# cainteoir::css::counter_style::counter_style {: .doc }

Construct a counter style with default values.

# cainteoir::css::counter_style::marker {: .doc }

Format a count value using the counter style.

@count
: The numeric value of the counter to format.

@return
: The formatted value.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
