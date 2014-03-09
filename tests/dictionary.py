#!/usr/bin/python

# Copyright (C) 2013 Reece H. Dunn
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
	test = harness.TestSuite('dictionary', sys.argv)
	test.run({'name': 'Dictionary', 'groups': [
		{'name': 'Cainteoir', 'type': 'dictionary', 'args': ['--list'], 'tests': [
			{'test': '../data/locale/en/cardinal.dict', 'result': 'dict/cainteoir/en-cardinal.lst'}, # say-as (word => word)
			{'test': '../data/locale/en-GB/cardinal.dict', 'result': 'dict/cainteoir/en-GB-cardinal.lst'}, # .import directive
			{'test': 'dict/cainteoir/ipa-phonemes.dict', 'result': 'dict/cainteoir/ipa-phonemes.lst'},
			{'test': 'dict/cainteoir/comments.dict', 'result': 'dict/cainteoir/comments.lst'},
			{'test': 'dict/cainteoir/duplicate-entry.dict', 'result': 'dict/cainteoir/ipa-phonemes.lst'},
		]},
		{'name': 'Resolve Say As', 'type': 'dictionary', 'args': ['--resolve-say-as'], 'tests': [
			{'test': 'dict/cainteoir/ipa-phonemes.dict', 'result': 'dict/cainteoir/ipa-phonemes.lst'},
			{'test': 'dict/cainteoir/say-as.dict', 'result': 'dict/cainteoir/say-as.lst'},
			{'test': 'dict/cainteoir/say-as-nested.dict', 'result': 'dict/cainteoir/say-as-nested.lst'},
			{'test': 'dict/cainteoir/say-as-infinite-recursion.dict', 'result': 'dict/cainteoir/say-as-infinite-recursion.lst'},
		]},
		{'name': 'Command Line', 'type': 'dictionary', 'tests': [
			# --dictionary
			{'result': 'dict/test/dictionary.stats', 'args': ['-d', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary.stats', 'args': ['--dictionary', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary.stats', 'args': ['--dictionary=${PWD}/dict/test/dictionary.dict']},
			# --list
			{'result': 'dict/test/dictionary.lst', 'args': ['-L', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary.lst', 'args': ['--list', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			# --resolve-say-as
			{'result': 'dict/test/dictionary-resolved.lst', 'args': ['-R', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary-resolved.lst', 'args': ['--resolve-say-as', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			# --as-dictionary cainteoir
			{'result': 'dict/test/dictionary.cainteoir', 'args': ['-D', 'cainteoir', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary.cainteoir', 'args': ['--as-dictionary', 'cainteoir', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary.cainteoir', 'args': ['--as-dictionary=cainteoir', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			# --as-dictionary espeak
			{'result': 'dict/test/dictionary.espeak', 'args': ['-D', 'espeak', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary.espeak', 'args': ['--as-dictionary', 'espeak', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			{'result': 'dict/test/dictionary.espeak', 'args': ['--as-dictionary=espeak', '--dictionary', '${PWD}/dict/test/dictionary.dict']},
			# --phonemeset espeak/en
			{'result': 'dict/test/dictionary-espeak-en.lst', 'args': ['--list', '-P', 'espeak/en', '--dictionary', '${PWD}/dict/test/dictionary.dict'], 'expect': 'fail'},
			{'result': 'dict/test/dictionary-espeak-en.lst', 'args': ['--list', '--phonemeset', 'espeak/en', '--dictionary', '${PWD}/dict/test/dictionary.dict'], 'expect': 'fail'},
			{'result': 'dict/test/dictionary-espeak-en.lst', 'args': ['--list', '--phonemeset=espeak/en', '--dictionary', '${PWD}/dict/test/dictionary.dict'], 'expect': 'fail'},
		]},
	]})
	test.summary()
