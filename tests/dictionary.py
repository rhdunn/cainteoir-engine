#!/usr/bin/python

# Copyright (C) 2011 Reece H. Dunn
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

def test_dictionary(dictionary):
	data = []
	with open(os.path.join(sys.path[0], dictionary)) as f:
		for line in f:
			if line != '\n' and not line.startswith('#'):
				word, pronunciation = line.split()
				data.append({ 'word': word, 'pronunciation': pronunciation })

	with open('/tmp/words.lst', 'w') as f:
		for item in data:
			f.write('%s\n\n' % item['word'])

	os.system('espeak -v en -xq -f /tmp/words.lst > /tmp/pronunciation.lst')

	with open('/tmp/pronunciation.lst') as f:
		espeak = f.read().split()

	passed = 0
	failed = 0

	for i in range(0, len(data)):
		word = data[i]['word']
		expected = data[i]['pronunciation']
		actual = '/%s/' % espeak[i]

		if expected == actual:
			print '%s %s ... pass' % (word, expected)
			passed = passed + 1
		else:
			print '%s %s got: %s ... fail' % (word, expected, actual)
			failed = failed + 1

	print '%d passed %d failed %d total' % (passed, failed, passed + failed)

if __name__ == '__main__':
	test_dictionary('../data/dictionary/names.dict')
