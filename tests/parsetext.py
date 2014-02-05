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
			{'test': 'parse-text/latin/hyphens.txt', 'result': 'parse-text/latin/hyphens.pt'},
			{'test': 'parse-text/latin/shy-hyphens.txt', 'result': 'parse-text/latin/shy-hyphens.pt'},
			{'test': 'parse-text/latin/quotes.txt', 'result': 'parse-text/latin/quotes.pt'},
			{'test': 'parse-text/latin/symbols.txt', 'result': 'parse-text/latin/symbols.pt'},
			{'test': 'parse-text/latin/paragraph.txt', 'result': 'parse-text/latin/paragraph.pt'},
			{'test': 'parse-text/latin/match-limit.txt', 'result': 'parse-text/latin/match-limit.pt'},
			{'test': 'parse-text/latin/no-newline-at-end.txt', 'result': 'parse-text/latin/no-newline-at-end.pt'},
		]},
		{'name': 'Japanese', 'type': 'parsetext', 'tests': [
			{'test': 'parse-text/japanese/words.txt', 'result': 'parse-text/japanese/words.pt'},
			{'test': 'parse-text/japanese/mixed-script.txt', 'result': 'parse-text/japanese/mixed-script.pt'},
		]},
		{'name': 'HTML', 'type': 'parsetext', 'tests': [
			{'test': 'parse-text/html/block-first-empty.html', 'result': 'parse-text/html/block-first-empty.pt'},
			{'test': 'parse-text/html/block-nested.html', 'result': 'parse-text/html/block-nested.pt'},
			{'test': 'parse-text/html/paragraph.html', 'result': 'parse-text/html/paragraph.pt'},
			{'test': 'parse-text/html/paragraph-singleword.html', 'result': 'parse-text/html/paragraph-singleword.pt'},
			{'test': 'parse-text/html/msword.html', 'result': 'parse-text/html/msword.pt'},
			{'test': 'parse-text/html/split-word.html', 'result': 'parse-text/html/split-word.pt'},
			{'test': 'parse-text/html/ordinal.html', 'result': 'parse-text/html/ordinal.pt'},
		]},
	]})
	test.run({'name': 'Context Analysis', 'groups': [
		{'name': 'Latin', 'type': 'contextanalysis', 'tests': [
			{'test': 'parse-text/latin/words.txt', 'result': 'parse-text/latin/words.ca'},
			{'test': 'parse-text/latin/numbers.txt', 'result': 'parse-text/latin/numbers.pt'},
			{'test': 'parse-text/latin/punctuation.txt', 'result': 'parse-text/latin/punctuation.ca'},
			{'test': 'parse-text/latin/hyphens.txt', 'result': 'parse-text/latin/hyphens.ca'},
			{'test': 'parse-text/latin/shy-hyphens.txt', 'result': 'parse-text/latin/shy-hyphens.ca'},
			{'test': 'parse-text/latin/quotes.txt', 'result': 'parse-text/latin/quotes.ca'},
			{'test': 'parse-text/latin/symbols.txt', 'result': 'parse-text/latin/symbols.ca'},
			{'test': 'parse-text/latin/paragraph.txt', 'result': 'parse-text/latin/paragraph.ca'},
			{'test': 'parse-text/latin/match-limit.txt', 'result': 'parse-text/latin/match-limit.pt'},
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/numbers.ca'},
			{'test': 'parse-text/ordinal-numbers/numbers.txt', 'result': 'parse-text/ordinal-numbers/numbers.ca'},
			{'test': 'parse-text/latin/no-newline-at-end.txt', 'result': 'parse-text/latin/no-newline-at-end.pt'},
		]},
		{'name': 'Japanese', 'type': 'contextanalysis', 'tests': [
			{'test': 'parse-text/japanese/words.txt', 'result': 'parse-text/japanese/words.pt'},
			{'test': 'parse-text/japanese/mixed-script.txt', 'result': 'parse-text/japanese/mixed-script.ca'},
		]},
		{'name': 'HTML', 'type': 'contextanalysis', 'tests': [
			{'test': 'parse-text/html/block-first-empty.html', 'result': 'parse-text/html/block-first-empty.ca'},
			{'test': 'parse-text/html/block-nested.html', 'result': 'parse-text/html/block-nested.pt'},
			{'test': 'parse-text/html/paragraph.html', 'result': 'parse-text/html/paragraph.ca'},
			{'test': 'parse-text/html/paragraph-singleword.html', 'result': 'parse-text/html/paragraph-singleword.pt'},
			{'test': 'parse-text/html/msword.html', 'result': 'parse-text/html/msword.ca'},
			{'test': 'parse-text/html/split-word.html', 'result': 'parse-text/html/split-word.pt'},
			{'test': 'parse-text/html/ordinal.html', 'result': 'parse-text/html/ordinal.ca'},
		]},
	]})
	test.run({'name': 'Word Stream', 'groups': [
		{'name': 'Latin', 'type': 'wordstream', 'tests': [
			{'test': 'parse-text/latin/words.txt', 'result': 'parse-text/latin/words.ws'},
			{'test': 'parse-text/latin/punctuation.txt', 'result': 'parse-text/latin/punctuation.ws'},
			{'test': 'parse-text/latin/hyphens.txt', 'result': 'parse-text/latin/hyphens.ws'},
			{'test': 'parse-text/latin/shy-hyphens.txt', 'result': 'parse-text/latin/shy-hyphens.ws'},
			{'test': 'parse-text/latin/quotes.txt', 'result': 'parse-text/latin/quotes.ws'},
			{'test': 'parse-text/latin/symbols.txt', 'result': 'parse-text/latin/symbols.ws'},
			{'test': 'parse-text/latin/paragraph.txt', 'result': 'parse-text/latin/paragraph.ws'},
			{'test': 'parse-text/latin/match-limit.txt', 'result': 'parse-text/latin/match-limit.pt'},
			{'test': 'parse-text/latin/no-newline-at-end.txt', 'result': 'parse-text/latin/no-newline-at-end.pt'},
		]},
		{'name': 'Japanese', 'type': 'wordstream', 'tests': [
			{'test': 'parse-text/japanese/words.txt', 'result': 'parse-text/japanese/words.pt'},
			{'test': 'parse-text/japanese/mixed-script.txt', 'result': 'parse-text/japanese/mixed-script.ws'},
		]},
		{'name': 'HTML', 'type': 'wordstream', 'tests': [
			{'test': 'parse-text/html/block-first-empty.html', 'result': 'parse-text/html/block-first-empty.ws'},
			{'test': 'parse-text/html/block-nested.html', 'result': 'parse-text/html/block-nested.ws'},
			{'test': 'parse-text/html/paragraph.html', 'result': 'parse-text/html/paragraph.ws'},
			{'test': 'parse-text/html/paragraph-singleword.html', 'result': 'parse-text/html/paragraph-singleword.ws'},
			{'test': 'parse-text/html/msword.html', 'result': 'parse-text/html/msword.ws'},
			{'test': 'parse-text/html/split-word.html', 'result': 'parse-text/html/split-word.ws'},
			{'test': 'parse-text/html/ordinal.html', 'result': 'parse-text/html/ordinal.ws'},
		]},
		{'name': 'Cardinal Numbers', 'type': 'wordstream', 'tests': [
			# af -- Afrikaans
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/af.ws', 'locale': 'af', 'scale': 'short'},
			# cs -- Czech
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/cs.ws', 'locale': 'cs', 'scale': 'short', 'expect': 'fail'},
			# de -- German
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/de.ws', 'locale': 'de', 'scale': 'short', 'expect': 'fail'},
			# en -- English
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/en-GB-x-shtscale.ws', 'locale': 'en-GB', 'scale': 'short'},
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/en-GB-x-lngscale.ws', 'locale': 'en-GB', 'scale': 'long'},
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/en-x-shtscale.ws', 'locale': 'en-US', 'scale': 'short'},
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/en-x-shtscale.ws', 'locale': 'en', 'scale': 'short'},
			# ga -- Irish Gaelic
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/ga.ws', 'locale': 'ga', 'scale': 'short', 'expect': 'fail'},
			# hr -- Croatian
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/hr.ws', 'locale': 'hr', 'scale': 'short'},
			# nl -- Dutch/Flemish
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/nl.ws', 'locale': 'nl', 'scale': 'short'},
			# sl -- Slovenian
			{'test': 'parse-text/cardinal-numbers/numbers.txt', 'result': 'parse-text/cardinal-numbers/sl.ws', 'locale': 'sl', 'scale': 'short'},
		]},
		{'name': 'Ordinal Numbers', 'type': 'wordstream', 'tests': [
			# en -- English
			{'test': 'parse-text/ordinal-numbers/numbers.txt', 'result': 'parse-text/ordinal-numbers/en-GB-x-shtscale.ws', 'locale': 'en-GB', 'scale': 'short'},
			{'test': 'parse-text/ordinal-numbers/numbers.txt', 'result': 'parse-text/ordinal-numbers/en-x-shtscale.ws', 'locale': 'en', 'scale': 'short'},
		]},
	]})
	test.summary()
