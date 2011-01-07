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

class TestSuite:
	def __init__(self):
		self.passed = 0
		self.failed = 0

	def check_metadata(self, filename, expect, formattype, displayas=None):
		tmpfile = '/tmp/metadata.txt'

		sys.stdout.write('... checking %s as %s metadata ... ' % ((displayas or filename), formattype))

		os.system('CAINTEOIR_DATADIR=%s %s --%s "%s" > %s' % (
			os.path.join(sys.path[0], '../data'),
			os.path.join(sys.path[0], '../src/apps/metadata/metadata'),
			formattype,
			filename,
			tmpfile))

		with open(expect, 'r') as f:
			expected = [ unicode(x.replace('<DATETIME>', date.today().strftime('%Y'))) for x in f.read().split('\n') if not x == '' ]

		with open(tmpfile, 'r') as f:
			got = [ unicode(x.replace('<%s' % filename, '<')) for x in f.read().split('\n') if not x == '' ]

		if expected == got:
			self.passed = self.passed + 1
			print 'passed'
		else:
			self.failed = self.failed + 1
			print 'failed'

	def summary(self):
		print '========== test summary =========='
		print '%d passed, %d failed, %d total' % (self.passed, self.failed, (self.passed + self.failed))
		if self.failed != 0:
			raise Exception('Some of the tests failed.')
