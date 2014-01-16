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

def map_line(line, replacements):
	for src, dst in replacements:
		line = line.replace(src, dst)
	return line

class Command:
	def __init__(self, command, collator='tee'):
		self.command = 'XDG_DATA_DIRS=%s:/usr/local/share/:/usr/share/ CAINTEOIR_DATA_DIR=%s %s' % (
			os.path.join(sys.path[0], '../data'),
			os.path.join(sys.path[0], '../data'),
			os.path.join(sys.path[0], command))
		self.collator = collator

	def replacements(self, filename):
		return []

	def run(self, args, filename, data):
		tmpfile = '/tmp/testrun'
		if filename:
			os.system('%s %s %s 2>&1 | %s > %s' % (self.command, ' '.join(args), filename, self.collator, tmpfile))
		else:
			os.system('%s %s 2>&1 | %s > %s' % (self.command, ' '.join(args), self.collator, tmpfile))
		replaced = self.replacements(filename)
		with open(tmpfile, 'r') as f:
			output = [ repr(map_line(x, replaced)) for x in f.read().split('\n') if not x == '' ]
		return output

class MetadataCommand(Command):
	def __init__(self, test_type):
		Command.__init__(self, '../src/apps/metadata --%s' % test_type)

	def replacements(self, filename):
		# NOTE: The %EMPTY% marker is to differentiate actual empty URIs from URIs
		# containing just the filename.
		return [
			('<>', '<%EMPTY%>'),
			('<#', '<%EMPTY%#'),
			('<%s' % filename, '<')
		]

class EventsCommand(Command):
	def __init__(self):
		Command.__init__(self, 'events')

	def replacements(self, filename):
		# NOTE: The %EMPTY% marker is to differentiate actual empty URIs from URIs
		# containing just the filename.
		ret = [
			('[]', '[%EMPTY%]'),
			('[#', '[%EMPTY%#'),
			('[%s' % filename, '[')
		]
		if filename:
			ret.append((os.path.dirname(filename), '%CWD%'))
		return ret

class DictionaryCommand(Command):
	def __init__(self):
		Command.__init__(self, '../src/apps/dictionary', collator='sort')

	def run(self, args, filename, data):
		if filename:
			return Command.run(self, args, '--dictionary %s' % filename, data)
		return Command.run(self, args, filename, data)

class ParseTextCommand(Command):
	def __init__(self, test_type):
		Command.__init__(self, 'parsetext --%s' % test_type)

	def run(self, args, filename, data):
		params = [x for x in args]
		if 'locale' in data:
			params.extend(['--locale', data['locale']])
		if 'scale' in data:
			params.append('--%s-scale' % data['scale'])
		return Command.run(self, params, filename, data)

class PhonemeSetCommand(Command):
	def __init__(self):
		Command.__init__(self, '../src/apps/phoneme-converter')

	def run(self, args, filename, data):
		params = [x for x in args]
		params.extend(['--separate', '--no-pauses', data['from'], data['to']])
		return Command.run(self, params, filename, data)

def create_command(test_type):
	if test_type in ['ntriple', 'turtle', 'vorbis']:
		return MetadataCommand(test_type)
	if test_type == 'dictionary':
		return DictionaryCommand()
	if test_type == 'phonemeset':
		return PhonemeSetCommand()
	if test_type == 'events':
		return EventsCommand()
	if test_type in ['styles', 'xmlreader']:
		return Command(test_type)
	if test_type == 'htmlreader':
		return Command('xmlreader --html')
	if test_type in ['parsetext', 'contextanalysis', 'wordstream']:
		return ParseTextCommand(test_type)
	raise Exception('Unsupported command "%s"' % test_type)

class TestSuite:
	def __init__(self, name, args):
		self.passed = 0
		self.failed = 0
		self.name = name
		if len(args) == 2:
			self.run_only = args[1]
		else:
			self.run_only = None

	def check_(self, filename, expect, command, args, test_expect, replacements, data, displayas):
		if displayas:
			write('... checking %s [%s] ... ' % (displayas, command))
		else:
			write('... checking %s [%s] ... ' % (' '.join(args), command))
		tmpfile = '/tmp/metadata.txt'

		cmd = create_command(command)
		got = cmd.run(args, filename, data)

		with open(expect, 'r') as f:
			expected = [ repr(replace_strings(x.replace('<DATETIME>', date.today().strftime('%Y')), replacements)) for x in f.read().split('\n') if not x == '' ]

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

	def check(self, filename, expect, command, args, test_expect, replacements, data, displayas):
		if type(command).__name__ == 'list':
			for c in command:
				self.check_(filename, expect, c, args, test_expect, replacements, data, displayas)
		else:
			self.check_(filename, expect, command, args, test_expect, replacements, data, displayas)

	def run(self, data):
		if self.run_only and data['name'] != self.run_only:
			return

		for group in data['groups']:
			write('\n')
			write('testing %s :: %s ...\n' % (data['name'], group['name']))

			group_args = []
			if 'args' in group:
				group_args = group['args']

			for test in group['tests']:
				expect = 'pass'
				if 'expect' in test.keys():
					expect = test['expect']

				if 'args' in test:
					args = [x for x in group_args]
					args.extend([x.replace('${PWD}', sys.path[0]) for x in test['args']])
				else:
					args = group_args

				if 'test' in test:
					got = os.path.join(sys.path[0], test['test'])
				else:
					got = None
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
					self.check(archive, exp, group['type'], args, 'expect-%s' % expect, replacements, test, test['test'])
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
					self.check(filename, exp, group['type'], args, 'expect-%s' % expect, replacements, test, test['test'])
				else:
					self.check(got, exp, group['type'], args, 'expect-%s' % expect, replacements, test, got)

	def summary(self):
		write('\n')
		write('========== summary of the %s test results ==========\n' % self.name)
		write('  %s passed\n' % str(self.passed).rjust(4))
		write('  %s failed\n' % str(self.failed).rjust(4))
		write('  %s total\n'  % str(self.passed + self.failed).rjust(4))
		write('\n')
		if self.failed != 0:
			raise Exception('Some of the tests failed.')
