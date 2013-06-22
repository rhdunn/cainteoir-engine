#!/usr/bin/python

# Copyright (C) 2010-2013 Reece H. Dunn
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

diff_program = None

def replace_strings(string, replacements):
	for (key, value) in replacements.items():
		string = string.replace('@%s@' % key, value)
	return string

def write(s):
	sys.stdout.write(s)

class TestSuite:
	def __init__(self, name, args):
		self.passed = 0
		self.failed = 0
		self.name = name
		if len(args) == 2:
			self.run_only = args[1]
		else:
			self.run_only = None

	def check_command(self, filename, expect, command, test_expect, replacements, sort=False):
		tmpfile = '/tmp/metadata.txt'

		if sort:
			os.system('XDG_DATA_DIRS=%s:/usr/local/share/:/usr/share/ CAINTEOIR_DATA_DIR=%s %s "%s" | sort > %s' % (
				os.path.join(sys.path[0], '../data'),
				os.path.join(sys.path[0], '../data'),
				command,
				filename,
				tmpfile))
		else:
			os.system('XDG_DATA_DIRS=%s:/usr/local/share/:/usr/share/ CAINTEOIR_DATA_DIR=%s %s "%s" 2>&1 | tee > %s' % (
				os.path.join(sys.path[0], '../data'),
				os.path.join(sys.path[0], '../data'),
				command,
				filename,
				tmpfile))

		with open(expect, 'r') as f:
			expected = [ repr(replace_strings(x.replace('<DATETIME>', date.today().strftime('%Y')), replacements)) for x in f.read().split('\n') if not x == '' ]

		with open(tmpfile, 'r') as f:
			got = [ repr(x.replace('<%s' % filename, '<').replace('[%s' % filename, '[')) for x in f.read().split('\n') if not x == '' ]

		if test_expect == 'expect-pass':
			ret = expected == got
		else:
			ret = expected != got

		if ret:
			self.passed = self.passed + 1
			write('passed [%s]\n' % test_expect)
		else:
			self.failed = self.failed + 1
			write('failed [%s]\n' % test_expect)

		if not ret or test_expect == 'expect-fail':
			if diff_program:
				with open('/tmp/expected', 'w') as f:
					f.write('\n'.join(expected))

				with open('/tmp/got', 'w') as f:
					f.write('\n'.join(got))

				os.system('%s /tmp/expected /tmp/got' % diff_program)
			else:
				write('    %s\n' % ('>'*75))
				for line in difflib.unified_diff(expected, got, fromfile='expected', tofile='got'):
					write('    | %s\n' % line.replace('\n', ''))
				write('    %s\n' % ('<'*75))

	def check_metadata(self, filename, expect, formattype, displayas=None, test_expect='expect-pass', replacements={}):
		write('... checking %s as %s metadata ... ' % ((displayas or filename), formattype))
		self.check_command(filename=filename, expect=expect, test_expect=test_expect,
			command='%s --%s' % (os.path.join(sys.path[0], '../src/apps/metadata/metadata'), formattype), replacements=replacements)

	def check_events(self, filename, expect, displayas=None, test_expect='expect-pass', replacements={}):
		write('... checking %s as text/speech events ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command=os.path.join(sys.path[0], 'events'), test_expect=test_expect, replacements=replacements)

	def check_xmlreader(self, filename, expect, displayas=None, test_expect='expect-pass', replacements={}):
		write('... checking %s as xmlreader tags ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command=os.path.join(sys.path[0], 'xmlreader'), test_expect=test_expect, replacements=replacements)

	def check_styles(self, filename, expect, displayas=None, test_expect='expect-pass', replacements={}):
		write('... checking %s as styles tags ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command=os.path.join(sys.path[0], 'styles'), test_expect=test_expect, replacements=replacements)

	def check_parsetext(self, filename, expect, parsetype, displayas=None, test_expect='expect-pass', replacements={}):
		if 'locale' in replacements.keys() and 'scale' in replacements.keys():
			write('... checking %s as %s tags in %s (%s scale) ... ' % (displayas or filename, parsetype, replacements['locale'], replacements['scale']))
			self.check_command(filename=filename, expect=expect, command='%s --%s --locale %s --%s-scale' % (os.path.join(sys.path[0], 'parsetext'), parsetype, replacements['locale'], replacements['scale']), test_expect=test_expect, replacements=replacements)
		else:
			write('... checking %s as %s tags ... ' % (displayas or filename, parsetype))
			self.check_command(filename=filename, expect=expect, command='%s --%s' % (os.path.join(sys.path[0], 'parsetext'), parsetype), test_expect=test_expect, replacements=replacements)

	def check_dictionary(self, filename, expect, displayas=None, test_expect='expect-pass', replacements={}):
		write('... checking %s as dictionary entries ... ' % (displayas or filename))
		self.check_command(filename=filename, expect=expect, command='%s --list' % os.path.join(sys.path[0], '../src/apps/dictionary/dictionary'), test_expect=test_expect, replacements=replacements, sort=True)

	def check_phonemeset(self, filename, expect, displayas=None, test_expect='expect-pass', replacements={}, data={}):
		srcphon = data['from']
		dstphon = data['to']
		write('... checking %s from %s to %s ... ' % (displayas or filename, srcphon, dstphon))
		self.check_command(filename=filename, expect=expect, command='%s --separate --no-pauses %s %s' % (os.path.join(sys.path[0], '../src/apps/phoneme-converter/phoneme-converter'), srcphon, dstphon), test_expect=test_expect, replacements=replacements)

	def run(self, data):
		if self.run_only and data['name'] != self.run_only:
			return

		for group in data['groups']:
			write('\n')
			write('testing %s :: %s ...\n' % (data['name'], group['name']))
			if group['type'] in ['ntriple', 'turtle', 'vorbis']:
				check = lambda got, exp, expect, displayas, replacements, data: self.check_metadata(got, exp, group['type'], test_expect=expect, displayas=displayas, replacements=replacements)
			elif group['type'] == 'events':
				check = lambda got, exp, expect, displayas, replacements, data: self.check_events(got, exp, test_expect=expect, displayas=displayas, replacements=replacements)
			elif group['type'] == 'xmlreader':
				check = lambda got, exp, expect, displayas, replacements, data: self.check_xmlreader(got, exp, test_expect=expect, displayas=displayas, replacements=replacements)
			elif group['type'] == 'styles':
				check = lambda got, exp, expect, displayas, replacements, data: self.check_styles(got, exp, test_expect=expect, displayas=displayas, replacements=replacements)
			elif group['type'] in ['parsetext', 'contextanalysis', 'wordstream']:
				check = lambda got, exp, expect, displayas, replacements, data: self.check_parsetext(got, exp, group['type'], test_expect=expect, displayas=displayas, replacements=replacements)
			elif group['type'] == 'dictionary':
				check = lambda got, exp, expect, displayas, replacements, data: self.check_dictionary(got, exp, test_expect=expect, displayas=displayas, replacements=replacements)
			elif group['type'] == 'phonemeset':
				check = lambda got, exp, expect, displayas, replacements, data: self.check_phonemeset(got, exp, test_expect=expect, displayas=displayas, replacements=replacements, data=data)

			for test in group['tests']:
				expect = 'pass'
				if 'expect' in test.keys():
					expect = test['expect']

				got = os.path.join(sys.path[0], test['test'])
				exp = os.path.join(sys.path[0], test['result'])

				replacements = {}
				if 'replace' in data.keys():
					for replacement in data['replace']:
						if replacement in test.keys():
							replacements[replacement] = test[replacement]
						else:
							replacements[replacement] = data[replacement]
				if 'replace' in group.keys():
					for replacement in group['replace']:
						if replacement in test.keys():
							replacements[replacement] = test[replacement]
						else:
							replacements[replacement] = data[replacement]

				if 'archive' in data.keys():
					archive = '/tmp/test.zip'
					zf = zipfile.ZipFile(archive, mode='w', compression=zipfile.ZIP_STORED)
					for location, filename in data['archive']:
						if location == 'mimetype' or location.endswith('/'):
							zfi = zipfile.ZipInfo(location)
							if filename:
								zf.writestr(zfi, filename)
							else:
								zfi.external_attr = 16
								zf.writestr(zfi, '')
						else:
							if filename.startswith('@'):
								filename = test[ filename.replace('@', '') ]
							zf.write(os.path.join(sys.path[0], filename), location, compress_type=zipfile.ZIP_DEFLATED)
					zf.close()
					check(archive, exp, expect='expect-%s' % expect, displayas=test['test'], replacements=replacements, data=test)
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
					check(filename, exp, expect='expect-%s' % expect, displayas=test['test'], replacements=replacements, data=test)
				else:
					check(got, exp, expect='expect-%s' % expect, displayas=got, replacements=replacements, data=test)

	def summary(self):
		write('\n')
		write('========== summary of the %s test results ==========\n' % self.name)
		write('  %s passed\n' % str(self.passed).rjust(4))
		write('  %s failed\n' % str(self.failed).rjust(4))
		write('  %s total\n'  % str(self.passed + self.failed).rjust(4))
		write('\n')
		if self.failed != 0:
			raise Exception('Some of the tests failed.')
