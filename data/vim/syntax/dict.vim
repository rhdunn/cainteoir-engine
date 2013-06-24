" Vim syntax file
" Language:	Dictionary Entries
" Filenames:    *.dict
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2013 Jun 24

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match	dictSpecialEntry	"_[0-9]x"		contained
syn match	dictSpecialEntry	"_10\^[0-9]*"		contained
syn match	dictSpecialEntry	"_andDD"		contained
syn match	dictSpecialEntry	"_DandDD"		contained

syn match	dictDirective		"\.[a-z]*"		contained

syn match	dictEntry		"^[^\t]*"		contains=dictSpecialEntry,dictDirective
syn region	dictComment		start='#'  end='$'

syn match	dictDefinition		"\t[^#]*$"
syn region	dictPronunciation	start="/" end="/"

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link dictDirective		PreProc
hi def link dictSpecialEntry		Statement
hi def link dictDefinition		String
hi def link dictComment			Comment

let b:current_syntax = "dict"
" vim: ts=8
