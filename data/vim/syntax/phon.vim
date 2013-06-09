" Vim syntax file
" Language:	Phoneme Transcription
" Filenames:    *.phon
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2013 May 24

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword	phonFeature		afr alv apr asp
syn keyword	phonFeature		bck blb
syn keyword	phonFeature		clk cnt ctl
syn keyword	phonFeature		dnt
syn keyword	phonFeature		ejc epg
syn keyword	phonFeature		flp fnt frc fzd
syn keyword	phonFeature		glt
syn keyword	phonFeature		hgh
syn keyword	phonFeature		imp
syn keyword	phonFeature		lat lbd lbv lmd lng low lzd
syn keyword	phonFeature		mid mrm
syn keyword	phonFeature		nas nzd
syn keyword	phonFeature		orl
syn keyword	phonFeature		pal phr pla pzd
syn keyword	phonFeature		rfx rnd rzd
syn keyword	phonFeature		smh sml stp syl
syn keyword	phonFeature		trl
syn keyword	phonFeature		umd unr unx uvl
syn keyword	phonFeature		vcd vel vls vwl vzd

syn region	phonComment		start="#" end="$" keepend contains=@Spell
syn region	phonTranscription	start="/" end="/" contains=phonFeature

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link phonComment			Comment
hi def link phonTranscription		String
hi def link phonFeature			Statement

let b:current_syntax = "phon"
" vim: ts=8
