" Vim syntax file
" Language:	Dictionary Entries
" Filenames:    *.dict
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2013 Feb 23

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match	dictSpecialEntry	"_[0-9]x"		contained
syn match	dictSpecialEntry	"_10\^[0-9]*"		contained
syn match	dictSpecialEntry	"_andDD"		contained

syn match	dictDirective		"\.[a-z]*"		contained

syn match	dictEntry		"^[^\t]*"		contains=dictSpecialEntry,dictDirective
syn region	dictDefinition		start='\t' end='$'

"syn region	dictDirectiveContent	start='\t' end='$'

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link dictDirective		PreProc
hi def link dictSpecialEntry		Statement
hi def link dictEntry			Constant

let b:current_syntax = "dict"
" vim: ts=8
