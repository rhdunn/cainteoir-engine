" Vim syntax file
" Language:	Phoneme Transcription
" Filenames:    *.phon
" Maintainer:	Reece H. Dunn <msclrhd@gmail.com>
" Last Change:	2012 Aug 29

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword	phonMetadata			rights license reference name alias description
syn keyword	phonPlaceOfArticulation		blb lbd dnt alv pla rfx pal vel uvl lbv phr glt
syn keyword	phonMannerOfArticulation	stp nas trl flp frc afr apr lat ejc imp clk
syn keyword	phonVoicing			vls vcd
syn keyword	phonVowelBackness		fnt cnt bck
syn keyword	phonVowelHeight			hgh smh umd mid lmd sml low
syn keyword	phonVowelRoundness		unr rnd
syn keyword	phonVowelFeature		vwl lqd gld
syn keyword	phonDiacritic			lng syl st1 st2 asp mrm
syn keyword	phonDiacritic			nzd vzd rzd pzd fzd lzd
syn keyword	phonDiacritic			sil syb

syn region	phonComment		start="#" end="$" keepend contains=@Spell
syn region	phonTranscription	start="/" end="/"
syn region	phonLink		start="\[" end="\]"

" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link phonComment			Comment
hi def link phonLink			Float
hi def link phonMetadata		Statement
hi def link phonTranscription		String
hi def link phonPlaceOfArticulation	phonConsonantFeature
hi def link phonMannerOfArticulation	phonConsonantFeature
hi def link phonVoicing			phonConsonantFeature
hi def link phonConsonantFeature	phonFeature
hi def link phonVowelBackness		phonVowelFeature
hi def link phonVowelHeight		phonVowelFeature
hi def link phonVowelRoundness		phonVowelFeature
hi def link phonVowelFeature		phonFeature
hi def link phonDiacritic		phonFeature
hi def link phonFeature			Statement

let b:current_syntax = "phon"
" vim: ts=8
