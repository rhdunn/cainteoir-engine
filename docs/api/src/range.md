# cainteoir::overlap_status {: .doc }

The overlapping nature of two ranges.

# cainteoir::no_overlap {: .doc }

The two ranges do not overlap.

# cainteoir::overlap_inner {: .doc }

The second range (other) is completely contained within the first range (this).

# cainteoir::overlap_outer {: .doc }

The first range (this) is completely contained within the second range (other).

# cainteoir::overlap_at_start {: .doc }

The second range (other) overlaps the first range (this) at the start of the first range.

# cainteoir::overlap_at_end {: .doc }

The second range (other) overlaps the first range (this) at the end of the first range.

# cainteoir::range {: .doc }

A sequence of items between two iterators or integral values.

This can be used to define a range of two iterators or pointers, for example:

	int data = { 1, 2, 3, 4, 5 };
	cainteoir::range<int *> r{ std::begin(data), std::end(data) };
	for (const auto &item : r)
		std::cout << item << std::endl;

It can also be used to define an integral range, for example:

	for (const auto &item : cainteoir::range<int>(1, 5))
		std::cout << item << std::endl;

Both these examples display the same output.

# cainteoir::range::first {: .doc }

The first item in the range.

# cainteoir::range::last {: .doc }

One past the last item in the range.

# cainteoir::range::iterator {: .doc }

The iterator type for this range object.

# cainteoir::range::const_reverse_iterator {: .doc }

An iterator for iterating backwards through the range.

# cainteoir::range::size_type {: .doc }

The type used to represent sizes.

# cainteoir::range::range {: .doc }

Create a new range object.

@f
: The first item in the range.

@l
: One past the last item in the range.

# cainteoir::range::~range {: .doc }

Clean up the range object.

# cainteoir::range::begin {: .doc }

Get the beginning of the range.

@return
: An iterator at the beginning of the range.

# cainteoir::range::end {: .doc }

Get the end of the range.

@return
: An iterator at one past the end of the range.

# cainteoir::range::rbegin {: .doc }

Get the end of the range, iterating backwards.

@return
: A reverse iterator at the end of the range.

# cainteoir::range::rend {: .doc }

Get the beginning of the range, iterating backwards.

@return
: A reverse iterator at the start of the range.

# cainteoir::range::size {: .doc }

Get the number of items in the range.

@return
: The number of items in the range.

# cainteoir::range::empty {: .doc }

Is the range empty?

@return
: `true` if the range is empty, `false` otherwise.

# cainteoir::range::contains {: .doc }

Does the range contain pos?

@pos
: The iterator (for iterator ranges) or integral value (for integral type
  ranges) to check is located within the range.

@return `true` if `pos` is located within the range, `false` otherwise.

# cainteoir::range::contains {: .doc }

Does the range contain another range, either partially or completely?

@other
: The range to check is contained within this range.

@return
: The overlapping nature of the two ranges (see [overlap_status](^^cainteoir::overlap_status)
  for a description of the return values).

# cainteoir::reverse {: .doc }

Traverse the container in reverse order.

@c
: The container to traverse in reverse order.

This is useful in conjunction with the C++11 range-based for loop, for example:

	std::vector<int> v = { 1, 2, 3, 4 };
	for (auto &item : cainteoir::reverse(v))
		std::cout << item << std::endl;

@return
: A range object for traversing the container in reverse order.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
