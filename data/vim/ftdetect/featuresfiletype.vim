" We take care to preserve the user's fileencodings and fileformats,
" because those settings are global (not buffer local), yet we want
" to override them for loading phon files, which are defined to be UTF-8.
let s:current_fileformats = ''
let s:current_fileencodings = ''

" define fileencodings to open as utf-8 encoding even if it's ascii.
function! s:features_filetype_pre()
  let s:current_fileformats = &g:fileformats
  let s:current_fileencodings = &g:fileencodings
  set fileencodings=utf-8 fileformats=unix
  setlocal filetype=phon
endfunction

" restore fileencodings as others
function! s:features_filetype_post()
  let &g:fileformats = s:current_fileformats
  let &g:fileencodings = s:current_fileencodings
endfunction

au BufNewFile *.features setlocal filetype=phon fileencoding=utf-8 fileformat=unix
au BufRead *.features call s:features_filetype_pre()
au BufReadPost *.features call s:features_filetype_post()
