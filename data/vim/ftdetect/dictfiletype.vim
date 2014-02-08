" Cached versions of the global values to preserve user settings.
let s:current_fileformats   = ''
let s:current_fileencodings = ''

function! s:filetype_pre()
  let s:current_fileformats   = &g:fileformats
  let s:current_fileencodings = &g:fileencodings
  set fileencodings=utf-8 fileformats=unix
  setlocal filetype=dict
endfunction

function! s:filetype_post()
  let &g:fileformats   = s:current_fileformats
  let &g:fileencodings = s:current_fileencodings
endfunction

au BufNewFile  *.dict setlocal filetype=dict fileencoding=utf-8 fileformat=unix
au BufRead     *.dict call s:filetype_pre()
au BufReadPost *.dict call s:filetype_post()
