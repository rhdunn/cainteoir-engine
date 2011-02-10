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
import harness

test = harness.TestSuite('XMLReader')

def test_dir(basedir):
	rootdir = os.path.join(sys.path[0], basedir)
	testcases = sorted(os.listdir(rootdir))

	print 'testing XML/HTML parsing for files in directory %s:' % basedir
	for filename in testcases:
		if filename.endswith('.xml'):
			srcfile = os.path.join(rootdir, filename)
			tagsfile = os.path.join(rootdir, filename.replace('.xml', '.tags'))
			test.check_xmlreader(srcfile, tagsfile)

if __name__ == '__main__':
	test_dir('xmlparser/comments')
	test_dir('xmlparser/cdata-section')
	test_dir('xmlparser/element')
	test_dir('xmlparser/empty-element')
	test_dir('xmlparser/text')
	test_dir('xmlparser/html-entity')
	test.summary()
