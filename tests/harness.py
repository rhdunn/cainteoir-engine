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
	def __init__(self, name):
		self.passed = 0
		self.failed = 0
		self.name = name

	def check_command(self, filename, expect, command):
		tmpfile = '/tmp/metadata.txt'

		os.system('CAINTEOIR_DATADIR=%s %s "%s" > %s' % (
			os.path.join(sys.path[0], '../data'),
			command,
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
			print '    %s' % ('>'*75)
			for line in difflib.unified_diff(expected, got, fromfile='expected', tofile='got'):
				print '    | %s' % line.replace('\n', '')
			print '    %s' % ('<'*75)

	def check_metadata(self, filename, expect, formattype, displayas=None):
		sys.stdout.write('... checking %s as %s metadata ... ' % ((displayas or filename), formattype))
		self.check_command(filename=filename, expect=expect,
			command='%s --%s' % (os.path.join(sys.path[0], '../src/apps/metadata/metadata'), formattype))

	def check_events(self, filename, expect, displayas=None):
		sys.stdout.write('... checking %s as text/speech events ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command=os.path.join(sys.path[0], 'events'))

	def check_xmlreader(self, filename, expect, displayas=None):
		sys.stdout.write('... checking %s as xmlreader tags ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command=os.path.join(sys.path[0], 'xmlreader'))

	def summary(self):
		print
		print '========== summary of the %s test results ==========' % self.name
		print '  %s passed' % str(self.passed).rjust(4)
		print '  %s failed' % str(self.failed).rjust(4)
		print '  %s total'  % str(self.passed + self.failed).rjust(4)
		print
		if self.failed != 0:
			raise Exception('Some of the tests failed.')
