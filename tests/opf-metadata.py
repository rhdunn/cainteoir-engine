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
from xml.dom import minidom

import harness as test

def check_metadata(filename, expect):
	tmpfile = '/tmp/metadata.n3'

	print '... checking %s' % filename
	os.system('CAINTEOIR_DATADIR=%s %s --turtle "%s" > %s' % (os.path.join(sys.path[0], '../data'), os.path.join(sys.path[0], '../src/apps/metadata/metadata'), filename, tmpfile))

	with open(expect, 'r') as f:
		expected = [ unicode(x) for x in f.read().split('\n') if not x == '' ]

	with open(tmpfile, 'r') as f:
		got = [ unicode(x.replace('<%s>' % filename, '<>')) for x in f.read().split('\n') if not x == '' ]

	test.equal(got, expected, 'opf.format(%s)' % filename)

def test_dir(basedir):
	rootdir = os.path.join(sys.path[0], basedir)
	testcases = sorted(os.listdir(rootdir))

	for filename in testcases:
		if filename.endswith('.opf'):
			opffile = os.path.join(rootdir, filename)
			n3file = os.path.join(rootdir, filename.replace('.opf', '.n3'))
			check_metadata(opffile, n3file)

if __name__ == '__main__':
	test_dir('opf/metadata')
	test_dir('opf/dc-metadata')
