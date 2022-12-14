# dvim

dvim is a terminal text editor inspired by the functionality of vim and neovim. dvim is designed to target iterm2, but can be run on any Linux/macOS terminal.

## Installation

dvim will automatically detect if iterm2 is being used at runtime.

To render symbols properly, install your favorite font from [NerdFonts](https://www.nerdfonts.com/).

```
make
```

Alternatively, for ASCII-only mode, compile as:

```
make MODE=-DASCIIONLY
```

## Acknowledgements

Much of the functionality of dvim is inspired by vim, neovim, and ncurses. No source code has been directly taken from these projects, but the functionality of these tools has significantly shaped the development of nvim.
