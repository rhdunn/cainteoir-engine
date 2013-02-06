#!/usr/bin/python

# Copyright (C) 2012 Reece H. Dunn
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
	test = harness.TestSuite('parsetext', sys.argv)
	test.run({'name': 'Parse Text', 'groups': [
		{'name': 'Latin', 'type': 'parsetext', 'tests': [
			{'test': 'parse-text/latin/words.txt', 'result': 'parse-text/latin/words.pt'},
			{'test': 'parse-text/latin/numbers.txt', 'result': 'parse-text/latin/numbers.pt'},
			{'test': 'parse-text/latin/punctuation.txt', 'result': 'parse-text/latin/punctuation.pt'},
			{'test': 'parse-text/latin/quotes.txt', 'result': 'parse-text/latin/quotes.pt'},
			{'test': 'parse-text/latin/symbols.txt', 'result': 'parse-text/latin/symbols.pt'},
			{'test': 'parse-text/latin/match-limit.txt', 'result': 'parse-text/latin/match-limit.pt'},
		]},
		{'name': 'Japanese', 'type': 'parsetext', 'tests': [
			{'test': 'parse-text/japanese/words.txt', 'result': 'parse-text/japanese/words.pt'},
		]},
		{'name': 'HTML', 'type': 'parsetext', 'tests': [
			{'test': 'parse-text/html/paragraph.html', 'result': 'parse-text/html/paragraph.pt'},
		]},
	]})
	test.summary()
