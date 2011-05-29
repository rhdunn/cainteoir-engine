#!/usr/bin/python

# Copyright (C) 2010-2011 Reece H. Dunn
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
	test = harness.TestSuite('VorbisComments', sys.argv)
	test.run({'name': 'Vorbis Comments', 'groups': [
		{'name': 'Dublin Core', 'type': 'vorbis', 'tests': [
			{'test': 'opf/vorbis-comments/simple.opf', 'result': 'opf/vorbis-comments/simple.txt'},
			{'test': 'opf/vorbis-comments/creator-only.opf', 'result': 'opf/vorbis-comments/creator-only.txt'},
			{'test': 'opf/vorbis-comments/creator-file-as.opf', 'result': 'opf/vorbis-comments/creator-file-as.txt'},
			{'test': 'opf/vorbis-comments/creator-role-aut.opf', 'result': 'opf/vorbis-comments/creator-role-aut.txt'},
			{'test': 'opf/vorbis-comments/creator-roles.opf', 'result': 'opf/vorbis-comments/creator-roles.txt'},
			{'test': 'opf/vorbis-comments/date-only-yyyy.opf', 'result': 'opf/vorbis-comments/date-only-yyyy.txt'},
			{'test': 'opf/vorbis-comments/date-only-yyyy-mm.opf', 'result': 'opf/vorbis-comments/date-only-yyyy-mm.txt'},
			{'test': 'opf/vorbis-comments/date-only-yyyy-mm-dd.opf', 'result': 'opf/vorbis-comments/date-only-yyyy-mm-dd.txt'},
			{'test': 'opf/vorbis-comments/date-created-yyyy.opf', 'result': 'opf/vorbis-comments/date-created-yyyy.txt'},
			{'test': 'opf/vorbis-comments/date-created-yyyy-mm.opf', 'result': 'opf/vorbis-comments/date-created-yyyy-mm.txt'},
			{'test': 'opf/vorbis-comments/date-created-yyyy-mm-dd.opf', 'result': 'opf/vorbis-comments/date-created-yyyy-mm-dd.txt'},
			{'test': 'opf/vorbis-comments/date-published-yyyy.opf', 'result': 'opf/vorbis-comments/date-published-yyyy.txt'},
			{'test': 'opf/vorbis-comments/date-published-yyyy-mm.opf', 'result': 'opf/vorbis-comments/date-published-yyyy-mm.txt'},
			{'test': 'opf/vorbis-comments/date-published-yyyy-mm-dd.opf', 'result': 'opf/vorbis-comments/date-published-yyyy-mm-dd.txt'},
			{'test': 'opf/vorbis-comments/description-only.opf', 'result': 'opf/vorbis-comments/description-only.txt'},
			{'test': 'opf/vorbis-comments/title-only.opf', 'result': 'opf/vorbis-comments/title-only.txt'},
		]},
	]})
	test.summary()
