/* Archive/File Collection API Documentation.
 *
 * Copyright (C) 2012 Reece H. Dunn
 *
 * This file is part of cainteoir-engine.
 *
 * cainteoir-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cainteoir-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @struct cainteoir::archive
  * @brief  An interface for extracting documents from an archive file format.
  */

/** @fn    cainteoir::archive::~archive()
  * @brief Clean up the archive object.
  */

/** @fn    const rdf::uri cainteoir::archive::location(const std::string &aFilename, const std::string &aRef) const
  * @brief Get the location of the specified file (for use in RDF metadata).
  *
  * @param[in] aFilename The file in the archive to get the location of.
  * @param[in] aRef      The reference section of the uri to use (for document ids).
  *
  * @return The uri associated with the file.
  */

/** @fn    std::shared_ptr<buffer> cainteoir::archive::read(const char *aFilename) const
  * @brief Get the content of the specified file.
  *
  * @param[in] aFilename The file in the archive to get the data for.
  *
  * @return The content of the file.
  */

/** @fn    const std::list<std::string> &cainteoir::archive::files() const
  * @brief Get the list of files contained in the archive.
  *
  * @return The filenames of all the files in the archive.
  */
