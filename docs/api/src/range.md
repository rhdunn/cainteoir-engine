#<cldoc:cainteoir::overlap_status>

The overlapping nature of two ranges,

#<cldoc:cainteoir::no_overlap>

The two ranges do not overlap.

#<cldoc:cainteoir::overlap_inner>

The second range (other) is completely contained within the first range (this).

#<cldoc:cainteoir::overlap_outer>

The first range (this) is completely contained within the second range (other).

#<cldoc:cainteoir::overlap_at_start>

The second range (other) overlaps the first range (this) at the start of the first range.

#<cldoc:cainteoir::overlap_at_end>

The second range (other) overlaps the first range (this) at the end of the first range.

#<cldoc:cainteoir::range>

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

#<cldoc:cainteoir::range::first>

The first item in the range.

#<cldoc:cainteoir::range::last>

One past the last item in the range.

#<cldoc:cainteoir::range::iterator>

The iterator type for this range object.

#<cldoc:cainteoir::range::const_reverse_iterator>

An iterator for iterating backwards through the range.

#<cldoc:cainteoir::range::size_type>

The type used to represent sizes.

#<cldoc:cainteoir::range::range>

Create a new range object.
@f The first item in the range.
@l One past the last item in the range.

#<cldoc:cainteoir::range::~range>

Clean up the range object.

#<cldoc:cainteoir::range::begin>

Get the beginning of the range.

@return An iterator at the beginning of the range.

#<cldoc:cainteoir::range::end>

Get the end of the range.

@return An iterator at one past the end of the range.

#<cldoc:cainteoir::range::rbegin>

Get the end of the range, iterating backwards.

@return A reverse iterator at the end of the range.

#<cldoc:cainteoir::range::rend>

Get the beginning of the range, iterating backwards.

@return A reverse iterator at the start of the range.

#<cldoc:cainteoir::range::size>

Get the number of items in the range.

@return The number of items in the range.

#<cldoc:cainteoir::range::empty>

Is the range empty?

@return `true` if the range is empty, `false` otherwise.

#<cldoc:cainteoir::range::contains>

Does the range contain pos?
@pos The iterator (for iterator ranges) or integral value (for integral type
     ranges) to check is located within the range.

@return `true` if <pos> is located within the range, `false` otherwise.

#<cldoc:cainteoir::range::contains>

Does the range contain another range, either partially or completely?
@other The range to check is contained within this range.

@return The overlapping nature of the two ranges (see <cainteoir::overlap_status>
        for a description of the return values).

#<cldoc:cainteoir::reverse>

Traverse the container in reverse order.
@c The container to traverse in reverse order.

This is useful in conjunction with the C++11 range-based for loop, for example:

	std::vector<int> v = { 1, 2, 3, 4 };
	for (auto &item : cainteoir::reverse(v))
		std::cout << item << std::endl;

@return A range object for traversing the container in reverse order.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
