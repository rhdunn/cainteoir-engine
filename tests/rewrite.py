#!/usr/bin/python

# Copyright (C) 2015 Reece H. Dunn
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

import harness
import sys

if __name__ == '__main__':
	test = harness.TestSuite('rewrite', sys.argv)
	test.run({'name': 'Rewrite', 'groups': [
		{'name': 'Pinyin', 'type': 'rewrite', 'tests': [
			{'test': 'letter2phoneme/cantonese.txt', 'result': 'letter2phoneme/cantonese.rewrite', 'ruleset': 'letter2phoneme/cantonese.ldb'},
			{'test': 'letter2phoneme/mandarin.txt', 'result': 'letter2phoneme/mandarin.rewrite', 'ruleset': 'letter2phoneme/mandarin.ldb'},
		]},
	]})
	test.summary()
