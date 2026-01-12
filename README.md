# Terminal Emulator

A toy terminal emulator written in C with OpenGL, featuring ANSI escape sequences, UTF-8 support, and Nerd Font rendering.
Learn more about it [here](https://keagana.me/blog1)

![License](https://img.shields.io/badge/license-MIT-blue)
![Language](https://img.shields.io/badge/language-C-brightgreen)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey)

## Features

- **ANSI Escape Sequences** - Full color support and cursor positioning
- **UTF-8 Character Rendering** - Proper multi-byte Unicode handling
- **Nerd Font Support** - Beautiful icon rendering for modern prompts
- **OpenGL Rendering** - Hardware-accelerated text display with FreeType
- **PTY Shell Integration** - Real interactive bash shell
- **Cursor Blinking** - Visual cursor feedback
- **Local Input Echo** - See what you type before sending to shell

## About This Project

This is my first project in C, so there are likely errors everywhere! I wanted to learn C by building something complex and challenging. Through this project, I learned a lot about:

- Shell integration and PTY communication
- Terminal escape sequence parsing
- UTF-8 text encoding/decoding
- OpenGL rendering pipelines
- Font rasterization with FreeType

OpenGL alone was its own beast - I probably spent the first 3-4 days just making a window pop up! C is a pretty hard language, but pointers are fun.

I did use AI for some portions when I got stuck. As I learn more C, I hope to rely on it less.

## Building

*The fonts and shell path are hard-coded*
*If you want to use something other than bash it will need be changed in main.c on line 120*

### Prerequisites

- **CMake** (3.10+)
- **Make**
- **C Compiler** (GCC/Clang)
- **OpenGL** development libraries
- **FreeType** (included in `external/`)
- **GLFW** (included in `external/`)
- **Glad** - clone into `external/`

### Compilation Steps

1. **Generate build files:**
   ```bash
   # On Linux/macOS
   ./scripts/buildCmake.sh
   ```

2. **Build the project:**
   ```bash
   cd build
   make -j
   ```

3. **Run the terminal:**
   ```bash
   ./mag-terminal
   ```

## Platform Support

| Platform | Status |
|----------|--------|
| Linux    | ✅ Tested on Linux Mint |
| macOS    | ✅ Tested |
| Windows  | ❌ Not yet (PTY library limitation) |

## Font Configuration

The terminal looks for Nerd Fonts in the following locations:

- **Linux:** `~/.local/share/fonts/SpaceMonoNerdFontMono-Regular.ttf`
- **macOS:** `/System/Library/Fonts/Menlo.ttc`

For best results with Oh My Posh and modern prompts, install a Nerd Font from [nerdfonts.com](https://www.nerdfonts.com/).

## Known Issues

- Some Nerd Font glyphs may display as `?` if not present in the font file
- Scrollback is not yet implemented
- Window resizing not fully supported

## Acknowledgments

- **[FreeType Project](https://www.freetype.org/)** - Font rendering library
- **[GLFW](https://www.glfw.org/)** - OpenGL window management
- **[Glad](https://glad.dav1d.de/)** - OpenGL loader

## License

This project is open source. Feel free to learn from it, modify it, or laugh at my C mistakes.

---

*"If it's not broken, don't fix it, right?"*


