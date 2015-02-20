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
	test = harness.TestSuite('media-stream', sys.argv)
	test.run({ 'name': 'ePub3', 'groups': [
		{'name': 'media overlay', 'type': 'media-stream', 'tests': [
			{'test': 'epub3/media-overlay/single-file/xhtml-before-smil.epub', 'result': 'epub3/media-overlay/single-file/xhtml-before-smil.csv'},
			{'test': 'epub3/media-overlay/single-file/xhtml-after-smil.epub', 'result': 'epub3/media-overlay/single-file/xhtml-before-smil.csv'},
			{'test': 'epub3/media-overlay/single-file/text-but-no-audio.epub', 'result': 'epub3/media-overlay/single-file/text-but-no-audio.csv'},
			{'test': 'epub3/media-overlay/multiple-files/multiple.epub', 'result': 'epub3/media-overlay/multiple-files/multiple.csv'},
			{'test': 'epub3/media-overlay/multiple-files/spanning.epub', 'result': 'epub3/media-overlay/multiple-files/multiple.csv'},
			{'test': 'epub3/media-overlay/multiple-files/spanning-different-id.epub', 'result': 'epub3/media-overlay/multiple-files/multiple.csv'},
			{'test': 'epub3/media-overlay/single-file/comma-and-quotes.epub', 'result': 'epub3/media-overlay/single-file/comma-and-quotes.csv'},
		]},
	]})
	test.summary()
