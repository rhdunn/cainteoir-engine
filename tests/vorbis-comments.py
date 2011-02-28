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

import harness

if __name__ == '__main__':
	test = harness.TestSuite('VorbisComments')
	test.run_tests(name='Vorbis Comments', testtype='vorbis', tests=[
		('opf/vorbis-comments/simple.opf', 'opf/vorbis-comments/simple.txt', 'expect-pass'),
		('opf/vorbis-comments/creator-only.opf', 'opf/vorbis-comments/creator-only.txt', 'expect-pass'),
		('opf/vorbis-comments/creator-file-as.opf', 'opf/vorbis-comments/creator-file-as.txt', 'expect-pass'),
		('opf/vorbis-comments/creator-role-aut.opf', 'opf/vorbis-comments/creator-role-aut.txt', 'expect-pass'),
		('opf/vorbis-comments/creator-roles.opf', 'opf/vorbis-comments/creator-roles.txt', 'expect-pass'),
		('opf/vorbis-comments/date-only-yyyy.opf', 'opf/vorbis-comments/date-only-yyyy.txt', 'expect-pass'),
		('opf/vorbis-comments/date-only-yyyy-mm.opf', 'opf/vorbis-comments/date-only-yyyy-mm.txt', 'expect-pass'),
		('opf/vorbis-comments/date-only-yyyy-mm-dd.opf', 'opf/vorbis-comments/date-only-yyyy-mm-dd.txt', 'expect-pass'),
		('opf/vorbis-comments/date-created-yyyy.opf', 'opf/vorbis-comments/date-created-yyyy.txt', 'expect-pass'),
		('opf/vorbis-comments/date-created-yyyy-mm.opf', 'opf/vorbis-comments/date-created-yyyy-mm.txt', 'expect-pass'),
		('opf/vorbis-comments/date-created-yyyy-mm-dd.opf', 'opf/vorbis-comments/date-created-yyyy-mm-dd.txt', 'expect-pass'),
		('opf/vorbis-comments/date-published-yyyy.opf', 'opf/vorbis-comments/date-published-yyyy.txt', 'expect-pass'),
		('opf/vorbis-comments/date-published-yyyy-mm.opf', 'opf/vorbis-comments/date-published-yyyy-mm.txt', 'expect-pass'),
		('opf/vorbis-comments/date-published-yyyy-mm-dd.opf', 'opf/vorbis-comments/date-published-yyyy-mm-dd.txt', 'expect-pass'),
		('opf/vorbis-comments/description-only.opf', 'opf/vorbis-comments/description-only.txt', 'expect-pass'),
		('opf/vorbis-comments/title-only.opf', 'opf/vorbis-comments/title-only.txt', 'expect-pass'),
	])
	test.summary()
