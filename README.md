# dvim

dvim is a terminal text editor inspired by the functionality of vim and neovim. dvim is designed to target iTerm2, but can be run on any Linux/macOS terminal.

## Installation

dvim will automatically detect if iTerm2 is being used at runtime.

To render symbols properly, install your favorite font from [NerdFonts](https://www.nerdfonts.com/).

```
make
```

If you don't have NerdFonts installed, compile as:

```
make MODE=-DNONF
```

Alternatively, for ASCII-only mode, compile as:

```
make MODE=-DASCIIONLY
```

## Modifications and Contributing

dvim started off as a passion project because my friend's Neovim setup looked better than my Vim setup. It's by no means anywhere close to being as good as Vim/Neovim/Emacs/etc., but if you'd like to fork dvim, feel free to! Similarly, if you would like to contribute towards dvim with a pull request, you're welcome to do so.

## System Design

dvim is split up into two key parts: dcurses (ncurses with iTerm2 image support) and dvim (the editor logic).

### dcurses

dcurses is the 

### dvim

## Acknowledgements

Much of the functionality of dvim is inspired by vim, neovim, and ncurses. No source code has been directly taken from these projects, but the functionality of these tools has significantly shaped the development of nvim.
