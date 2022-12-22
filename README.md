# dvim

[![Compile Check](https://github.com/danlliu/dvim/actions/workflows/compilecheck.yml/badge.svg)](https://github.com/danlliu/dvim/actions/workflows/compilecheck.yml)

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
In `EDITOR` mode, the user is able to edit the selected file. Within `EDITOR` 
mode, the user can switch between four different modes: `NORMAL`, `INSERT`, 
`COMMAND`, and `VISUAL` mode. Additional modes are specified when displaying
windows, such as `REG SHOW` to show all register contents.

##### Editor Design

The editor ([Editor.hpp](src/dvim/Editor.hpp)) consists of two key parts: the
text contents and the registers.

The text contents represent the contents of the active file. In code, it is
represented as a linked list of linked lists of characters
(`std::list<std::list<char>>`), to support fast arbitrary insertion of both
characters and lines. The current cursor position is tracked by a pair of
iterators (line and column), as well as two integer values which track the row
and column indexes. The iterators are used by the code to handle editing, while
the integer values are primarily used for display purposes.

The registers represent areas which can be used to save strings of copied text.
There are ten registers total (named `0` through `9`), with one register being
marked as "active" at any time. The "active" register is the register that will
be used for copying and pasting. (Note that this is a key difference from Vim,
where using a different register has to be specified before each operation)

##### `NORMAL` mode
`NORMAL` mode is used to navigate the cursor across the file, giving the ability
to move the cursor up (`k`), down (`j`), left (`h`), and right (`l`). The full
list of navigation commands is:

- `h`: move the cursor left.
- `j`: move the cursor down.
- `k`: move the cursor up.
- `l`: move the cursor right.
- `w`: move the cursor to the beginning of the next word
- `e`: move the cursor to the end of the next word
- `b`: move the cursor to the beginning of the previous word
- `^`: move the cursor to the first character of the line.
- `$`: move the cursor to the last character of the line.

To switch to `INSERT` mode, the following commands can be used:

- `i`: enter `INSERT` mode at the current cursor location.
- `a`: enter `INSERT` mode at the position _after_ the current cursor location
(append to the current location).
- `o`: insert a new line on the line _after_ the current cursor location, and
enter `INSERT` mode on that new line.
- `O`: insert a new line on the line _before_ the current cursor location, and
enter `INSERT` mode on that new line.

Additionally, some editing can be performed directly from `NORMAL` mode. The
following commands can be used:

- `x`: delete the character at the current cursor location. Saves the character
into the active register.

To switch to `COMMAND` mode, the following command can be used:
- `:`: enters `COMMAND` mode.

To switch to `VISUAL` mode, the following command can be used:
- `v`: enters `VISUAL` mode at the current cursor location.

##### `INSERT` mode
`INSERT` mode is used to edit the file directly. The following commands can be
used in `INSERT` mode:

- `ESC`: exit `INSERT` mode, switching back to `NORMAL` mode.

##### `COMMAND` mode
`COMMAND` mode is used to issue commands to dvim. The following commands are
available:

- `ESC`: exit `COMMAND` mode, switching back to `NORMAL` mode.
- `w`: write to the file, saving it.
- `q`: quit the editor for the current file.
- `reg show`: show the contents of all registers. The active register is marked
with a `*`.
- `reg select <x>`: select the provided register as the active register. `<x>`
is an integer between 0 and 9.

To submit a command, press `ENTER`; to cancel, press `ESC`.

Single-letter commands can be placed sequentially; ex. `wq` saves the file and
exits the editor.

##### `VISUAL` mode
`VISUAL` mode is used to select areas of text for modification. In `VISUAL`
mode, the following commands are available:

- `ESC`: exit `VISUAL` mode, switching back to `NORMAL` mode.
- `h`: move the cursor left.
- `j`: move the cursor down.
- `k`: move the cursor up.
- `l`: move the cursor right.
- `y`: copies the current selection into the active register, and switches back
to `NORMAL` mode.

## Acknowledgements

Much of the functionality of dvim is inspired by vim, neovim, and ncurses. No 
source code has been directly taken from these projects, but the functionality 
of these tools has significantly shaped the development of nvim.
