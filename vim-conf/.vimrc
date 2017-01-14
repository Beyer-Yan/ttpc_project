set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
Plugin 'tpope/vim-fugitive'
" plugin from http://vim-scripts.org/vim/scripts.html
Plugin 'L9'
" Git plugin not hosted on GitHub
" Plugin 'git://git.wincent.com/command-t.git'
" git repos on your local machine (i.e. when working on your own plugin)
" Plugin 'file:///home/gmarik/path/to/plugin'
" The sparkup vim script is in a subdirectory of this repo called vim.
" Pass the path to set the runtimepath properly.
" Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}
" Install L9 and avoid a Naming conflict if you've already installed a
" different version somewhere else.
" Plugin 'ascenator/L9', {'name': 'newL9'}
Plugin 'molokai'
Plugin 'godlygeek/tabular'
Plugin 'plasticboy/vim-markdown'
Plugin 'scrooloose/nerdtree'
Plugin 'vim-scripts/taglist.vim'
Plugin 'wesleyche/srcexpl'
"Plugin 'vim-scripts/armasm'
" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on

"-----------
"vim config
"-----------
	set nu "line number
	set ai "auto indent
	set ts=4 "tab size
	syntax on

"-------------------
"ctags database path
"-------------------
	set tags=/home/barrelfish4jetsontk1/tags

"--------------------
"cscope database path
"--------------------
	set csprg=/usr/bin/cscope "cscope位置
	set csto=0 "cscope DB search fisrt
	set cst "cscope DB tag DB search
	set nocsverb "verbose off

	"cs add /home/cx/barrelfish4jetsontk1/cscope.out
	"cs add /home/cx/barrelfish-5f7db2c/cscope.out
	set csverb

"--------------------
"Tap list config
"--------------------
	"filetype on "vim filetype on
	nmap <F7> :TlistToggle <CR> "F7 key = Tag List Toggling
	let Tlist_Ctags_Cmd = "/usr/local/bin/ctags"
	let Tlist_Inc_Winwidth = 0
	let Tlist_Exit_OnlyWindow = 0
	let Tlist_Auto_Open = 0
	let Tlist_Use_Right_Window = 1

"----------------------
"Source Explorer config
"----------------------
	nmap <F8> :SrcExplToggle <CR>
	nmap <C-H> <C-W>h
	nmap <C-J> <C-W>j
	nmap <C-K> <C-W>k
	nmap <C-L> <C-W>l

	let g:SrcExpl_winHeight = 8
	let g:SrcExpl_refreshTime = 100
	let g:SrcExpl_jumpKey = "<ENTER>"
	let g:SrcExpl_gobackKey = "<SPACE>"
	let g:SrcExpl_isUpdateTags = 0
	let g:SrcExpl_pluginList = [
		\ "__Tag_List__",
	    \ "_NERD_tree_",
	    \ "Source_Explorer"
	\]

	let g:SrcExpl_searchLocalDef = 1
	let g:SrcExpl_updateTagsCmd = "ctags --sort=foldcase -R ."
	let g:SrcExpl_updateTagsKey = "<F12>"

"------------------------
"NERD Tree config
"------------------------
	let NERDTreeWinPos = "left"
	nmap <F9> :NERDTreeToggle<CR>

"----------
"vim theme
"----------
	set background=dark
	colorscheme molokai 
"---------------
"markdown config
"---------------
	let g:vim_markdown_frontmatter=1
	let g:vim_markdown_math=1
