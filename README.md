# dvim

dvim is a terminal text editor inspired by the functionality of vim and neovim. 
dvim is designed to target iTerm2, but can be run on any Linux/macOS terminal.

## Installation

dvim will automatically detect if iTerm2 is being used at runtime.

To render symbols properly, install your favorite font from 
[NerdFonts](https://www.nerdfonts.com/).

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

dvim started off as a passion project because my friend's Neovim setup looked 
better than my Vim setup. It's by no means anywhere close to being as good as 
Vim/Neovim/Emacs/etc., but if you'd like to fork dvim, feel free to! Similarly, 
if you would like to contribute towards dvim with a pull request, you're welcome 
to do so.

## System Design

dvim is split up into two key parts: dcurses (ncurses with iTerm2 image support)
and dvim (the editor logic).

### dcurses

dcurses is a ncurses-inspired library to handle all TUI rendering. 
It has two main classes: `Window` and `WindowManager`.

#### `Window.hpp`: Window class

`Window` is an abstraction for a single TUI window. It contains the logic for 
displaying both text and images in windows. To optimize rendering, `Window` also 
contains a cache for when only text is being rendered. This design, inspired by
ncurses, means that only changes to the screen will be displayed, rather than
clearing the entire window and re-rendering the entire window.

#### `WindowManager.hpp`: Window manager class

`WindowManager` is an abstraction for the main controller of the entire dcurses
framework. `WindowManager` keeps track of all the windows visible in the TUI at
the current time, giving the ability to add and remove windows as well as render
them in the correct z-order.

### dvim

dvim contains the main editor logic. The overall design of dvim was inspired by
Vim's design. The overall editor utilizes a four-state state machine, going
between `PREVIEW`, `EDITOR`, `PREVIEWCOMMAND`, and `EDITORCOMMAND` states.

#### Common Windows

In all modes, a file tree and usage hint panel will be displayed. The file tree
(implemented in [FileTree.hpp](src/dvim/FileTree.hpp) and 
[FileTreeView.hpp](src/dvim/FileTreeView.hpp)) displays all files starting in
the directory that dvim was invoked from. Subdirectories can be expanded or
collapsed to show or hide their trees.

Additionally, a usage hint panel is shown. This updates in different modes to
show the various actions that the user can perform. This was inspired by
Spacemacs, which provides a similar panel that improves usability significantly.

#### `PREVIEW` mode
In `PREVIEW` mode, a preview window is displayed. In this window, the contents
of the currently selected file are shown. If the selected file is a text file,
the text contents will be displayed. If the selected file is an image, the image
will be displayed (when running in iTerm2), or a placeholder message will be
shown (for non-iTerm2 terminals).

From here, the user can enter `EDITOR` mode by selecting a file in the file tree
using `j`, `k`, and `SPACE` to navigate, then pressing `ENTER` to open the file.

#### `EDITOR` mode
In `EDITOR` mode, the user is able to edit the selected file.

## Acknowledgements

Much of the functionality of dvim is inspired by vim, neovim, and ncurses. No 
source code has been directly taken from these projects, but the functionality 
of these tools has significantly shaped the development of nvim.
