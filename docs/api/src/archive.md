# cainteoir::archive {: .doc }

An interface for extracting documents from an archive file format.

# cainteoir::archive::~archive {: .doc }

Clean up the archive object.

# cainteoir::archive::location {: .doc }

Get the location of the specified file (for use in RDF metadata).

@aFilename
: The file in the archive to get the location of.

@aRef
: The reference section of the uri to use (for document ids).

@return
: The uri associated with the file.

# cainteoir::archive::read {: .doc }

Get the content of the specified file.

@aFilename
: The file in the archive to get the data for.

@return
: The content of the file.

# cainteoir::archive::files {: .doc }

Get the list of files contained in the archive.

@return
: The filenames of all the files in the archive.

# cainteoir::create_zip_archive {: .doc }

Create an archive for extracting files in a ZIP file.

@aData
: A buffer containing the zip file contents.

@aSubject
: The uri to identify the zip file (its location on disk).

@return
: An archive object to access the zip file contents.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
