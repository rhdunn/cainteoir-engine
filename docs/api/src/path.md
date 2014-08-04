# cainteoir::path {: .doc }

Represents a file-system path.

# cainteoir::path::path {: .doc }

Create an empty path object.

# cainteoir::path::path {: .doc }

Create a path object.

@aPath
: The location to set the path object to.

# cainteoir::path::path {: .doc }

Create a path object.

@aPath
: The location to set the path object to.

# cainteoir::path::operator/= {: .doc }

Append the path to the end of this path.

@aPath
: The path to join at the end of this path.

@return
: This path object.

# cainteoir::path::operator/= {: .doc }

Append the path to the end of this path.

@aPath
: The path to join at the end of this path.

@return
: This path object.

# cainteoir::path::operator const char * {: .doc }

Get a C-style string representation of the path.

@return
: A C-style string representation of the path.

# cainteoir::path::str {: .doc }

Get a string representation of the path.

@return
: A string representation of the path.

# cainteoir::path::size {: .doc }

Get the number of characters in the path.

@return
: The number of characters in the path.

# cainteoir::path::empty {: .doc }

Is the path empty?

@return
: `true` if the path is empty, `false` otherwise.

# cainteoir::path::parent {: .doc }

Get the parent node of the path.

If the path is to a file, this returns the directory the file is located in.

If the path is to a directory, this returns the parent directory.

@return
: The parent node of the path.

# cainteoir::path::zip_file {: .doc }

Get the path of the zip container.

@return
: `zip_file` for paths of the form `zip_file!/zip_path`.

# cainteoir::path::zip_path {: .doc }

Get the path of the file in a zip container.

@return
: `zip_path` for paths of the form `zip_file!/zip_path`.

# cainteoir::operator/ {: .doc }

Join two paths together.

@a
: The path to be placed on the left-hand side.

@b
: The path to be placed on the right-hand side.

@return
: The joined path of the form `a/b`.

# cainteoir::operator/ {: .doc }

Join two paths together.

@a
: The path to be placed on the left-hand side.

@b
: The path to be placed on the right-hand side.

@return
: The joined path of the form `a/b`.

# cainteoir::get_data_path {: .doc }

Get the path to the Cainteoir Engine data files.

@return
: The path to the Cainteoir Engine data files.

# cainteoir::create_temp_file {: .doc }

Create a temporary file object.

@mode
: The mode of the file to create.

@return
: A file object to a new file created in temporary storage.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2014 Reece H. Dunn
