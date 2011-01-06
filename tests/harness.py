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

import os
import sys
import difflib

from datetime import date

def ok(cond, message):
	if (not cond):
		raise Exception(message)

def check_equal(a, b, message):
	if (not a == b):
		sys.stderr.write('%s\n' % message)
		sys.stderr.write('\texpected -- %s\n' % b)
		sys.stderr.write('\tactual   -- %s\n' % a)
		return False
	return True

def equal(a, b, message):
	are_equal = True
	if not check_equal(type(a).__name__, type(b).__name__, '%s -- type' % message):
		are_equal = False
	if isinstance(a, list) and isinstance(b, list):
		if not a == b:
			print '%s -- lists are not equal:' % message
			print '-'*40
			for line in difflib.unified_diff(a, b, fromfile='expected', tofile='got'):
				print line.replace('\n', '')
			print '-'*40
			are_equal = False
	else:
		if not check_equal(a, b, '%s -- value' % message):
			are_equal = False
	ok(are_equal, '%s : values are not equal' % message)

def check_metadata(filename, expect, formattype, displayas=None):
	tmpfile = '/tmp/metadata.txt'

	print '... checking %s' % (displayas or filename)
	print '         ==> %s' % expect
	print '          as %s metadata' % formattype
	os.system('CAINTEOIR_DATADIR=%s %s --%s "%s" > %s' % (os.path.join(sys.path[0], '../data'), os.path.join(sys.path[0], '../src/apps/metadata/metadata'), formattype, filename, tmpfile))

	with open(expect, 'r') as f:
		expected = [ unicode(x.replace('<DATETIME>', date.today().strftime('%Y'))) for x in f.read().split('\n') if not x == '' ]

	with open(tmpfile, 'r') as f:
		got = [ unicode(x.replace('<%s' % filename, '<')) for x in f.read().split('\n') if not x == '' ]

	equal(got, expected, 'metadata for %s' % filename)
