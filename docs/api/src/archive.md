#<cldoc:cainteoir::archive>

An interface for extracting documents from an archive file format.

#<cldoc:cainteoir::archive::~archive>

Clean up the archive object.

#<cldoc:cainteoir::archive::location>

Get the location of the specified file (for use in RDF metadata).
@aFilename The file in the archive to get the location of.
@aRef      The reference section of the uri to use (for document ids).

@return The uri associated with the file.

#<cldoc:cainteoir::archive::read>

Get the content of the specified file.
@aFilename The file in the archive to get the data for.

@return The content of the file.

#<cldoc:cainteoir::archive::files>

Get the list of files contained in the archive.

@return The filenames of all the files in the archive.

#<cldoc:cainteoir::create_zip_archive>

Create an archive for extracting files in a ZIP file.
@aData    A buffer containing the zip file contents.
@aSubject The uri to identify the zip file (its location on disk).

@return An archive object to access the zip file contents.

# License

This API documentation is licensed under the CC BY-SA 2.0 UK License.

Copyright (C) 2012-2014 Reece H. Dunn
