#<cldoc:cainteoir::buffer>

Represents a data or string buffer.

This class does not manage the lifetime of the data held within it, however
derived classes may manage the lifetime.

#<cldoc:cainteoir::buffer::buffer>

Create a new data buffer from a range.
@f The start of the buffer.
@l The end of the buffer.

#<cldoc:cainteoir::buffer::buffer>

Create a new data buffer from a string.
@f The null-terminated string to create the buffer from.

#<cldoc:cainteoir::buffer::~buffer>

Clean up the buffer.

#<cldoc:cainteoir::buffer::match_type>

Pointer to a string comparison function.
@s1 The first string to compare.
@s2 The second string to compare.
@n  The number of characters to compare.

#<cldoc:cainteoir::buffer::match_case>

Perform case-sensitive string comparison.

#<cldoc:cainteoir::buffer::ignore_case>

Perform case-insensitive string comparison.

#<cldoc:cainteoir::buffer::startswith>

Does the buffer start with the specified string?
@str   The string to check for.
@match The comparison method to use (case sensitive or insensitive).

@return `true` if the buffer starts with the specified string, `false` otherwise.

#<cldoc:cainteoir::buffer::compare>

Compare the buffer with a string.
@str   The string to compare with.
@match The comparison method to use (case sensitive or insensitive).

@return `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

#<cldoc:cainteoir::buffer::compare>

Compare the buffer with a string.
@str   The string to compare with.
@match The comparison method to use (case sensitive or insensitive).

@return `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

#<cldoc:cainteoir::buffer::comparei>

Compare the buffer with a string, ignoring case differences.
@str The string to compare with.

@return `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

#<cldoc:cainteoir::buffer::comparei>

Compare the buffer with a string, ignoring case differences.
@str The string to compare with.

@return `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

#<cldoc:cainteoir::buffer::str>

Convert the buffer to a C++ string.

@return The buffer as a C++ string.

#<cldoc:cainteoir::data_buffer>

A buffer allocated in memory of a given size.

#<cldoc:cainteoir::data_buffer::data_buffer>

Create a memory buffer of the requested size.
@aSize The size of the buffer to create.

#<cldoc:cainteoir::data_buffer::~data_buffer>

Clean up the buffer.

#<cldoc:cainteoir::memory_file>

Create a file that is stored in memory.

This class is used to create a short-term use `FILE` object to write data to
and get a cainteoir::buffer with the contents of that object. For example:

	cainteoir::memory_file out;
	fprintf(out, "Hello World!");
	std::shared_ptr<cainteoir::buffer> text = out.buffer(); // contains "Hello World!"

#<cldoc:cainteoir::memory_file::memory_file>

Open a new file in memory.

The `open_memstream` POSIX function is used to create the memory file if the
system supports it, otherwise a temporary file is created.

#<cldoc:cainteoir::memory_file::~memory_file>

Clean up the memory file.

#<cldoc:cainteoir::memory_file::operator FILE *>

Return a pointer to the `FILE` object.

#<cldoc:cainteoir::memory_file::buffer>

Create a buffer with the contents of the memory file.

This closes the `FILE` object, so a new <memory_file> needs to be created after
this call.

#<cldoc:cainteoir::rope>

Manage a list of buffers.

This class is for situations where many buffers are created and concatenated
together. It only concatenates the buffers when requested, performing the
operation in one pass.

#<cldoc:cainteoir::rope::rope>

Create an empty rope object.

#<cldoc:cainteoir::rope::size>

Get the number of bytes in the rope buffers.

@return The number of bytes in the rope buffers.

#<cldoc:cainteoir::rope::empty>

Is the rope empty?

@return `true` if the rope does not contain any buffer objects, `false` otherwise.

#<cldoc:cainteoir::rope::clear>

Empty the rope.

#<cldoc:cainteoir::rope::operator=>

Set the rope to the content of the buffer.
@item The buffer content to set the rope to.

@return This rope object (for method chaining).

#<cldoc:cainteoir::rope::operator+=>

Append the content of the buffer to the rope.
@item The buffer content to append.

@return This rope object (for method chaining).

#<cldoc:cainteoir::rope::buffer>

Get a buffer to the entire rope content.

@return The entire rope content.

#<cldoc:cainteoir::rope::content>

Get the content of the rope.

This differs from the buffer method in that if the rope only contains whitespace
then an empty buffer here is returned.

@return The entire rope content.

#<cloc:cainteoir::rope::normalize>

Get the rope buffer in its whitespace-normalized form.

@return The rope buffer in its whitespace-normalized form.

#<cldoc:cainteoir::rope::str>

Get the buffer as a C++ string.

@return The buffer as a C++ string.

#<cldoc:cainteoir::whitespace>

Specifies how whitespace should be processed.

#<cldoc:cainteoir::whitespace::preserve>

The whitespace is preserved.

#<cldoc:cainteoir::whitespace::collapse>

Consecutive whitespace is collapsed.

If there is any whitespace, consecutive whitespace is removed such that a
single whitespace character remains.

For trim options, this indicates that the whitespace on the left/right should
be removed.

#<cldoc:cainteoir::make_buffer>

Create a data buffer with the content of the string.
@aString The content of the buffer.

@return A new data buffer with the content of `aString`.

#<cldoc:cainteoir::make_buffer>

Create a data buffer with the content of the string.
@aString The content of the buffer.
@aLength The length of the string (or number of bytes of the string to copy).

@return A new data buffer with the content of `aString`.

#<cldoc:cainteoir::make_file_buffer>

Create a buffer from a file.
@path The path of the file to read the data from.

This function creates a memory mapped view of the file for optimal reading
of file contents.

@return A buffer containing the content of the specified file.

#<cldoc:cainteoir::make_file_buffer>

Create a buffer from a file.
@f The file to read the data from.

This function reads the data from the file in chunks. It is useful for
files that are buffered (stdin, pipes, sockets, etc.). For regular
files, use the version of this function that takes a path as that reads
the content in a single memory-mapped block.

@return A buffer containing the content of the specified file.

#<cldoc:cainteoir::make_file_buffer>

Create a buffer from a file descriptor.
@fd The file descriptor to read the data from.

This function reads the data from the file in chunks. It is useful for
files that are buffered (stdin, pipes, sockets, etc.). For regular
files, use the version of this function that takes a path as that reads
the content in a single memory-mapped block.

@return A buffer containing the content of the specified file descriptor.

#<cldoc:cainteoir::normalize>

Create a whitespace-normalized buffer.
@aBuffer The buffer containing the text to normalize.

This trims whitespace from the start and end of the buffer, as well as
consecutive whitespace characters within the buffer. Any whitespace
character is replaced by an ASCII space character.

@return A new buffer with the whitespace normalized.

#<cldoc:cainteoir::normalize>

Create a whitespace-normalized buffer.
@aBuffer     The buffer containing the text to normalize.
@aWhitespace <whitespace::preserve> to keep or <whitespace::collapse> to remove consecutive non-newline whitespace characters
@aNewlines   <whitespace::preserve> to keep or <whitespace::collapse> to remove consecutive newline characters
@aTrimLeft   <whitespace::preserve> to keep or <whitespace::collapse> to remove whitespace at the start
@aTrimRight  <whitespace::preserve> to keep or <whitespace::collapse> to remove whitespace at the end

The `aWhitespace` and `aNewlines` options configure how consecutive whitespace
is collapsed, supporting the CSS `normal`, `pre` and `pre-line` whitespace
modes. If `aTrimLeft` or `aTrimRight` are <whitespace::preserve>, the whitespace
rules for `aWhitespace` and `aNewlines` apply.

@return A new buffer with the whitespace normalized.

#<cldoc:cainteoir::decoder_ptr>

Pointer to a decoding/decompression algorithm.
@data The data buffer to be decoded/decompressed.
@size The size of the decoded/decompressed data buffer.

@return The new data buffer.

#<cldoc:cainteoir::copy>

Copy the data in buffer to a memory buffer.
@data The data buffer to be decoded/decompressed.
@size The size of the decoded/decompressed data buffer.

@return The new data buffer.

#<cldoc:cainteoir::inflate_zlib>

Inflate a zlib compressed data buffer.
@data The data buffer to be decoded/decompressed.
@size The size of the decoded/decompressed data buffer.

@return The uncompressed data buffer.

#<cldoc:cainteoir::inflate_gzip>

Inflate a gzip compressed data buffer.
@data The data buffer to be decoded/decompressed.
@size The size of the decoded/decompressed data buffer.

@return The uncompressed data buffer.

#<cldoc:cainteoir::decode_quoted_printable>

Decode a quoted printable encoded data buffer.
@data The data buffer to be decoded/decompressed.
@size The size of the decoded/decompressed data buffer.

@return The decoded data buffer.

#<cldoc:cainteoir::decode_base64>

Decode a base64 encoded data buffer.
@data The data buffer to be decoded/decompressed.
@size The size of the decoded/decompressed data buffer.

@return The decoded data buffer.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
