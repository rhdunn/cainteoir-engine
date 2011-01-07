#!/usr/bin/python

# Copyright (C) 2010 Reece H. Dunn
#
# This file is part of cainteoir-engine.
#
# cainteoir-engine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cainteoir-engine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.

import sys
import os
import zipfile
import harness

test = harness.TestSuite()

def check_metadata(epubfiles, expect, metadata):
	epubfile = '/tmp/test.epub'
	tmpfile = '/tmp/metadata.n3'

	zf = zipfile.ZipFile(epubfile, mode='w', compression=zipfile.ZIP_STORED)
	zf.writestr('mimetype', 'application/epub+zip')
	for location, filename in epubfiles:
		zf.write(os.path.join(sys.path[0], filename), location, compress_type=zipfile.ZIP_DEFLATED)
	zf.close()

	test.check_metadata(epubfile, expect, 'turtle', displayas='%s in %s' % (metadata, epubfile))

def test_dir(basedir):
	rootdir = os.path.join(sys.path[0], basedir)
	testcases = sorted(os.listdir(rootdir))

	for filename in testcases:
		if filename.endswith('.opf'):
			opffile = os.path.join(rootdir, filename)
			n3file = os.path.join(rootdir, filename.replace('.opf', '.n3'))
			check_metadata(expect=n3file, metadata=opffile, epubfiles=[
					('META-INF/container.xml', 'ocf/simple.ocf'),
					('OEBPS/content.opf', opffile),
					('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
					('OEBPS/test.html', 'xhtml1/simple.xhtml')
				])

if __name__ == '__main__':
	test_dir('opf/metadata')
	test_dir('opf/dc-metadata')
	test_dir('epub/metadata')
	test.summary()
