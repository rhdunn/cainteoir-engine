" We take care to preserve the user's fileencodings and fileformats,
" because those settings are global (not buffer local), yet we want
" to override them for loading phon files, which are defined to be UTF-8.
let s:current_fileformats = ''
let s:current_fileencodings = ''

" define fileencodings to open as utf-8 encoding even if it's ascii.
function! s:ltphonfiletype_pre()
  let s:current_fileformats = &g:fileformats
  let s:current_fileencodings = &g:fileencodings
  set fileencodings=utf-8 fileformats=unix
  setlocal filetype=ltphon
endfunction

" restore fileencodings as others
function! s:ltphonfiletype_post()
  let &g:fileformats = s:current_fileformats
  let &g:fileencodings = s:current_fileencodings
endfunction

au BufNewFile *.ltphon setlocal filetype=ltphon fileencoding=utf-8 fileformat=unix
au BufRead *.ltphon call s:ltphonfiletype_pre()
au BufReadPost *.ltphon call s:ltphonfiletype_post()
