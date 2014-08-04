# cainteoir::utf8::write {: .doc }

Write a UTF-8 character to an output buffer.

@out
: The output buffer to write the UTF-8 character to.

@c
: The UTF-32 unicode code-point.

This function does not null-terminate out and does not check that there is
enough space in out to write all the UTF-8 characters (up-to 4 characters).

@return
: A pointer to the new position within `out` after writing the UTF-8 character.

# cainteoir::utf8::read {: .doc }

Read a UTF-8 character from an input buffer.

@in
: The input buffer to read the UTF-8 character from.

@c
: The UTF-32 unicode code-point.

@return
: A pointer to the new position within `in` after reading the UTF-8 character.

# cainteoir::utf8::next {: .doc }

Get the next UTF-8 character in the sequence.

@c
: The current position in the UTF-8 string.

@return
: A pointer to the next UTF-8 character.

# cainteoir::utf8::prev {: .doc }

Get the previous UTF-8 character in the sequence.

@c
: The current position in the UTF-8 string.

@return
: A pointer to the previous UTF-8 character.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
