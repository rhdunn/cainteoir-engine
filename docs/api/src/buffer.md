# cainteoir::buffer {: .doc }

Represents a data or string buffer.

This class does not manage the lifetime of the data held within it, however
derived classes may manage the lifetime.

# cainteoir::buffer::buffer {: .doc }

Create a new data buffer from a range.

@f
: The start of the buffer.

@l
: The end of the buffer.

# cainteoir::buffer::buffer {: .doc }

Create a new data buffer from a string.

@f
: The null-terminated string to create the buffer from.

# cainteoir::buffer::~buffer {: .doc }

Clean up the buffer.

# cainteoir::buffer::match_type {: .doc }

Pointer to a string comparison function.

@s1
: The first string to compare.

@s2
: The second string to compare.

@n
: The number of characters to compare.

# cainteoir::buffer::match_case {: .doc }

Perform case-sensitive string comparison.

# cainteoir::buffer::ignore_case {: .doc }

Perform case-insensitive string comparison.

# cainteoir::buffer::startswith {: .doc }

Does the buffer start with the specified string?

@str
: The string to check for.

@match
: The comparison method to use (case sensitive or insensitive).

@return
: `true` if the buffer starts with the specified string, `false` otherwise.

# cainteoir::buffer::compare {: .doc }

Compare the buffer with a string.

@str
: The string to compare with.

@match
: The comparison method to use (case sensitive or insensitive).

@return
: `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

# cainteoir::buffer::compare {: .doc }

Compare the buffer with a string.

@str
: The string to compare with.

@match
: The comparison method to use (case sensitive or insensitive).

@return
: `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

# cainteoir::buffer::comparei {: .doc }

Compare the buffer with a string, ignoring case differences.

@str
: The string to compare with.

@return
: `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

# cainteoir::buffer::comparei {: .doc }

Compare the buffer with a string, ignoring case differences.

@str
: The string to compare with.

@return
: `0` if the strings match, `<0` if str is less than the buffer or `>0` if greater.

# cainteoir::buffer::str {: .doc }

Convert the buffer to a C++ string.

@return
: The buffer as a C++ string.

# cainteoir::data_buffer {: .doc }

A buffer allocated in memory of a given size.

# cainteoir::data_buffer::data_buffer {: .doc }

Create a memory buffer of the requested size.

@aSize
: The size of the buffer to create.

# cainteoir::data_buffer::~data_buffer {: .doc }

Clean up the buffer.

# cainteoir::memory_file {: .doc }

Create a file that is stored in memory.

This class is used to create a short-term use `FILE` object to write data to
and get a cainteoir::buffer with the contents of that object. For example:

	cainteoir::memory_file out;
	fprintf(out, "Hello World!");
	std::shared_ptr<cainteoir::buffer> text = out.buffer(); // contains "Hello World!"

# cainteoir::memory_file::memory_file {: .doc }

Open a new file in memory.

The `open_memstream` POSIX function is used to create the memory file if the
system supports it, otherwise a temporary file is created.

# cainteoir::memory_file::~memory_file {: .doc }

Clean up the memory file.

# cainteoir::memory_file::operator FILE * {: .doc }

Return a pointer to the `FILE` object.

# cainteoir::memory_file::buffer {: .doc }

Create a buffer with the contents of the memory file.

This closes the `FILE` object, so a new `memory_file` needs to be created after
this call.

# cainteoir::rope {: .doc }

Manage a list of buffers.

This class is for situations where many buffers are created and concatenated
together. It only concatenates the buffers when requested, performing the
operation in one pass.

# cainteoir::rope::rope {: .doc }

Create an empty rope object.

# cainteoir::rope::size {: .doc }

Get the number of bytes in the rope buffers.

@return
: The number of bytes in the rope buffers.

# cainteoir::rope::empty {: .doc }

Is the rope empty?

@return
: `true` if the rope does not contain any buffer objects, `false` otherwise.

# cainteoir::rope::clear {: .doc }

Empty the rope.

# cainteoir::rope::operator= {: .doc }

Set the rope to the content of the buffer.

@item
: The buffer content to set the rope to.

@return
: This rope object (for method chaining).

# cainteoir::rope::operator+= {: .doc }

Append the content of the buffer to the rope.

@item
: The buffer content to append.

@return
: This rope object (for method chaining).

# cainteoir::rope::buffer {: .doc }

Get a buffer to the entire rope content.

@return
: The entire rope content.

# cainteoir::rope::content {: .doc }

Get the content of the rope.

This differs from the buffer method in that if the rope only contains whitespace
then an empty buffer here is returned.

@return
: The entire rope content.

# cainteoir::rope::normalize {: .doc }

Get the rope buffer in its whitespace-normalized form.

@return
: The rope buffer in its whitespace-normalized form.

# cainteoir::rope::str {: .doc }

Get the buffer as a C++ string.

@return
: The buffer as a C++ string.

# cainteoir::whitespace {: .doc }

Specifies how whitespace should be processed.

# cainteoir::whitespace::preserve {: .doc }

The whitespace is preserved.

# cainteoir::whitespace::collapse {: .doc }

Consecutive whitespace is collapsed.

If there is any whitespace, consecutive whitespace is removed such that a
single whitespace character remains.

For trim options, this indicates that the whitespace on the left/right should
be removed.

# cainteoir::make_buffer {: .doc }

Create a data buffer with the content of the string.

@aString
: The content of the buffer.

@return
: A new data buffer with the content of `aString`.

# cainteoir::make_buffer {: .doc }

Create a data buffer with the content of the string.

@aString
: The content of the buffer.

@aLength
: The length of the string (or number of bytes of the string to copy).

@return
: A new data buffer with the content of `aString`.

# cainteoir::make_file_buffer {: .doc }

Create a buffer from a file.

@path
: The path of the file to read the data from.

This function creates a memory mapped view of the file for optimal reading
of file contents.

@return
: A buffer containing the content of the specified file.

# cainteoir::make_file_buffer {: .doc }

Create a buffer from a file.

@f
: The file to read the data from.

This function reads the data from the file in chunks. It is useful for
files that are buffered (stdin, pipes, sockets, etc.). For regular
files, use the version of this function that takes a path as that reads
the content in a single memory-mapped block.

@return
: A buffer containing the content of the specified file.

# cainteoir::make_file_buffer {: .doc }

Create a buffer from a file descriptor.

@fd
: The file descriptor to read the data from.

This function reads the data from the file in chunks. It is useful for
files that are buffered (stdin, pipes, sockets, etc.). For regular
files, use the version of this function that takes a path as that reads
the content in a single memory-mapped block.

@return
: A buffer containing the content of the specified file descriptor.

# cainteoir::normalize {: .doc }

Create a whitespace-normalized buffer.

@aBuffer
: The buffer containing the text to normalize.

This trims whitespace from the start and end of the buffer, as well as
consecutive whitespace characters within the buffer. Any whitespace
character is replaced by an ASCII space character.

@return
: A new buffer with the whitespace normalized.

# cainteoir::normalize {: .doc }

Create a whitespace-normalized buffer.

@aBuffer
: The buffer containing the text to normalize.

@aWhitespace
: [preserve](^^cainteoir::whitespace::preserve) to keep or
  [collapse](^^cainteoir::whitespace::collapse) to remove consecutive
  non-newline whitespace characters

@aNewlines
: [preserve](^^cainteoir::whitespace::preserve) to keep or
  [collapse](^^cainteoir::whitespace::collapse) to remove consecutive
  newline characters

@aTrimLeft
: [preserve](^^cainteoir::whitespace::preserve) to keep or
  [collapse](^^cainteoir::whitespace::collapse) to remove whitespace
  at the start

@aTrimRight
: [preserve](^^cainteoir::whitespace::preserve) to keep or
  [collapse](^^cainteoir::whitespace::collapse) to remove whitespace
  at the end

The `aWhitespace` and `aNewlines` options configure how consecutive whitespace
is collapsed, supporting the CSS `normal`, `pre` and `pre-line` whitespace
modes. If `aTrimLeft` or `aTrimRight` are [preserve](^^cainteoir::whitespace::preserve),
the whitespace rules for `aWhitespace` and `aNewlines` apply.

@return
: A new buffer with the whitespace normalized.

# cainteoir::decoder_ptr {: .doc }

Pointer to a decoding/decompression algorithm.

@data
: The data buffer to be decoded/decompressed.

@size
: The size of the decoded/decompressed data buffer.

@return
: The new data buffer.

# cainteoir::copy {: .doc }

Copy the data in buffer to a memory buffer.

@data
: The data buffer to be decoded/decompressed.

@size
: The size of the decoded/decompressed data buffer.

@return
: The new data buffer.

# cainteoir::inflate_zlib {: .doc }

Inflate a zlib compressed data buffer.

@data
: The data buffer to be decoded/decompressed.

@size
: The size of the decoded/decompressed data buffer.

@return
: The uncompressed data buffer.

# cainteoir::inflate_gzip {: .doc }

Inflate a gzip compressed data buffer.

@data
: The data buffer to be decoded/decompressed.

@size
: The size of the decoded/decompressed data buffer.

@return
: The uncompressed data buffer.

# cainteoir::decode_quoted_printable {: .doc }

Decode a quoted printable encoded data buffer.

@data
: The data buffer to be decoded/decompressed.

@size
: The size of the decoded/decompressed data buffer.

@return
: The decoded data buffer.

# cainteoir::decode_base64 {: .doc }

Decode a base64 encoded data buffer.

@data
: The data buffer to be decoded/decompressed.

@size
: The size of the decoded/decompressed data buffer.

@return
: The decoded data buffer.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
