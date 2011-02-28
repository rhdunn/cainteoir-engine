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

	def run_tests(self, name, testtype, tests=[]):
		print 'testing %s:' % name
		if testtype == 'events':
			for doc, events, expect in tests:
				self.check_events(os.path.join(sys.path[0], doc), os.path.join(sys.path[0], events), test_expect=expect)
		elif testtype == 'xmlreader':
			for doc, events, expect in tests:
				self.check_xmlreader(os.path.join(sys.path[0], doc), os.path.join(sys.path[0], events), test_expect=expect)
		else:
			for doc, nt, expect in tests:
				self.check_metadata(os.path.join(sys.path[0], doc), os.path.join(sys.path[0], nt), testtype, test_expect=expect)

	def run_epub_tests(self, name, testtype, tests=[]):
		print 'testing %s:' % name
		for doc, out, expect in tests:
			epubfile = '/tmp/test.epub'
			epubfiles=[
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', doc),
				('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
				('OEBPS/test.html', 'html/semantics/simple.xhtml')
			]

			zf = zipfile.ZipFile(epubfile, mode='w', compression=zipfile.ZIP_STORED)
			zf.writestr('mimetype', 'application/epub+zip')
			for location, filename in epubfiles:
				zf.write(os.path.join(sys.path[0], filename), location, compress_type=zipfile.ZIP_DEFLATED)
			zf.close()

			if testtype == 'events':
				self.check_events(epubfile, os.path.join(sys.path[0], out), test_expect=expect, displayas='epub://%s' % doc)
			else:
				self.check_metadata(epubfile, os.path.join(sys.path[0], out), testtype, test_expect=expect, displayas='epub://%s' % doc)

	def summary(self):
		print
		print '========== summary of the %s test results ==========' % self.name
		print '  %s passed' % str(self.passed).rjust(4)
		print '  %s failed' % str(self.failed).rjust(4)
		print '  %s total'  % str(self.passed + self.failed).rjust(4)
		print
		if self.failed != 0:
			raise Exception('Some of the tests failed.')
