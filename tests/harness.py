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
import zipfile

from datetime import date

class TestSuite:
	def __init__(self, name):
		self.passed = 0
		self.failed = 0
		self.name = name

	def check_command(self, filename, expect, command, test_expect):
		tmpfile = '/tmp/metadata.txt'

		os.system('CAINTEOIR_DATADIR=%s %s "%s" > %s' % (
			os.path.join(sys.path[0], '../data'),
			command,
			filename,
			tmpfile))

		with open(expect, 'r') as f:
			expected = [ repr(x.replace('<DATETIME>', date.today().strftime('%Y'))) for x in f.read().split('\n') if not x == '' ]

		with open(tmpfile, 'r') as f:
			got = [ repr(x.replace('<%s' % filename, '<')) for x in f.read().split('\n') if not x == '' ]

		if test_expect == 'expect-pass':
			ret = expected == got
		else:
			ret = expected != got

		if ret:
			self.passed = self.passed + 1
			print 'passed [%s]' % test_expect
		else:
			self.failed = self.failed + 1
			print 'failed [%s]' % test_expect

		if not ret or test_expect == 'expect-fail':
			print '    %s' % ('>'*75)
			for line in difflib.unified_diff(expected, got, fromfile='expected', tofile='got'):
				print '    | %s' % line.replace('\n', '')
			print '    %s' % ('<'*75)

	def check_metadata(self, filename, expect, formattype, displayas=None, test_expect='expect-pass'):
		sys.stdout.write('... checking %s as %s metadata ... ' % ((displayas or filename), formattype))
		self.check_command(filename=filename, expect=expect, test_expect=test_expect,
			command='%s --%s' % (os.path.join(sys.path[0], '../src/apps/metadata/metadata'), formattype))

	def check_events(self, filename, expect, displayas=None, test_expect='expect-pass'):
		sys.stdout.write('... checking %s as text/speech events ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command=os.path.join(sys.path[0], 'events'), test_expect=test_expect)

	def check_xmlreader(self, filename, expect, displayas=None, test_expect='expect-pass'):
		sys.stdout.write('... checking %s as xmlreader tags ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command=os.path.join(sys.path[0], 'xmlreader'), test_expect=test_expect)

	def run(self, data):
		for group in data['groups']:
			print 'testing %s :: %s ...' % (data['name'], group['name'])
			if group['type'] in ['ntriple', 'turtle', 'vorbis']:
				check = lambda got, exp, expect, displayas: self.check_metadata(got, exp, group['type'], test_expect=expect, displayas=displayas)
			elif group['type'] == 'events':
				check = lambda got, exp, expect, displayas: self.check_events(got, exp, test_expect=expect, displayas=displayas)
			elif group['type'] == 'xmlreader':
				check = lambda got, exp, expect, displayas: self.check_xmlreader(got, exp, test_expect=expect, displayas=displayas)

			for test in group['tests']:
				expect = 'pass'
				if 'expect' in test.keys():
					expect = test['expect']

				got = os.path.join(sys.path[0], test['test'])
				exp = os.path.join(sys.path[0], test['result'])

				if 'archive' in data.keys():
					archive = '/tmp/test.zip'
					zf = zipfile.ZipFile(archive, mode='w', compression=zipfile.ZIP_STORED)
					for location, filename in data['archive']:
						if location == 'mimetype':
							zf.writestr('mimetype', filename)
						else:
							if filename.startswith('@'):
								filename = test[ filename.replace('@', '') ]
							zf.write(os.path.join(sys.path[0], filename), location, compress_type=zipfile.ZIP_DEFLATED)
					zf.close()
					check(archive, exp, expect='expect-%s' % expect, displayas=test['test'])
				elif 'compress' in group.keys():
					if group['compress'] == 'gzip':
						filename = '/tmp/test.gz'
						os.system('gzip -c %s > %s' % (got, filename))
					elif group['compress'] == 'bzip2':
						filename = '/tmp/test.bz2'
						os.system('bzip2 -c %s > %s' % (got, filename))
					elif group['compress'] == 'lzma':
						filename = '/tmp/test.lzma'
						os.system('lzma -c %s > %s' % (got, filename))
					check(filename, exp, expect='expect-%s' % expect, displayas=test['test'])
				else:
					check(got, exp, expect='expect-%s' % expect, displayas=got)

	def summary(self):
		print
		print '========== summary of the %s test results ==========' % self.name
		print '  %s passed' % str(self.passed).rjust(4)
		print '  %s failed' % str(self.failed).rjust(4)
		print '  %s total'  % str(self.passed + self.failed).rjust(4)
		print
		if self.failed != 0:
			raise Exception('Some of the tests failed.')
