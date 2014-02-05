" Vim syntax file
" Language:	Letter-to-Phoneme Rules
" Filenames:    *.ltphon
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2013 Feb 25

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match	dictDirective		"\.[a-z]*"		contained

syn match	dictEntry		"^[^\t]*"		contains=dictDirective
syn region	dictDefinition		start='\t' end='$'

syn region	dictComment		start='^#' end='$'

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link dictDirective		PreProc
hi def link dictEntry			Constant
hi def link dictComment			Comment

let b:current_syntax = "ltphon"
" vim: ts=8
