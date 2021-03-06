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
		{'name': 'ePub 2.0 Dublin Core', 'type': 'vorbis', 'tests': [
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
		{'name': 'ePub 3.0', 'type': 'vorbis', 'tests': [
			{'test': 'opf/metadata30/contributor-role.opf', 'result': 'opf/metadata30/contributor-role.txt'},
			{'test': 'opf/metadata30/contributor-file-as.opf', 'result': 'opf/metadata30/contributor-file-as.txt'},
			{'test': 'opf/metadata30/contributor-alternate-script.opf', 'result': 'opf/metadata30/contributor-alternate-script.txt'},
			{'test': 'opf/metadata30/creator-role.opf', 'result': 'opf/metadata30/creator-role.txt'},
			{'test': 'opf/metadata30/creator-file-as.opf', 'result': 'opf/metadata30/creator-file-as.txt'},
			{'test': 'opf/metadata30/creator-alternate-script.opf', 'result': 'opf/metadata30/creator-alternate-script.txt'},
			{'test': 'opf/metadata30/meta-auth.opf', 'result': 'opf/metadata30/meta-auth.txt'},
			{'test': 'opf/metadata30/identifier.opf', 'result': 'opf/metadata30/identifier.txt'},
			{'test': 'opf/metadata30/publication-type.opf', 'result': 'opf/metadata30/publication-type.txt'},
			{'test': 'opf/metadata30/source-identifier.opf', 'result': 'opf/metadata30/source-identifier.txt'},
			{'test': 'opf/metadata30/title-title-type.opf', 'result': 'opf/metadata30/title-title-type.txt'},
			{'test': 'opf/metadata30/title-display-seq.opf', 'result': 'opf/metadata30/title-display-seq.txt'},
			{'test': 'opf/metadata30/title-file-as.opf', 'result': 'opf/metadata30/title-file-as.txt'},
			{'test': 'opf/metadata30/title-group-position.opf', 'result': 'opf/metadata30/title-group-position.txt'},
		]},
	]})
	test.summary()
