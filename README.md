# img
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/6f3e5af1-8005-4999-9c4e-5bceee1d35cb" />


A minimal, lightweight image viewer for X11 written in C99. Supports PNG, JPEG, and WebP formats.

## Motivation

I wanted something smaller than `feh` and `nsxiv`. Most image viewers are bloated with features you don't need. `img` is a no-frills image viewer that does one thing well: display images fast with minimal overhead.

## Features

- Fast, minimal image viewer
- Support for PNG, JPEG, and WebP formats
- X11-based display
- Simple and clean C99 codebase
- Modular backend architecture for easy format extension

## Requirements

- X11 development libraries (`libX11-dev`)
- Image format libraries (optional, but at least one recommended):
  - `libpng-dev` for PNG support
  - `libjpeg-dev` for JPEG support
  - `libwebp-dev` for WebP support

### Prebuilt Static Binary

From version `1.1` onward, a statically linked binary (`img-static-linux-x86_64`) is attached to each [release](https://github.com/KrzysztofMarciniak/img/releases), no dependencies required, just download and go to:

## [Releases](https://github.com/KrzysztofMarciniak/img/releases)

Download img-static-linux-x86_64

```bash
chmod +x img-static-linux-x86_64
./img-static-linux-x86_64 image.png
```
## Building

Build options are controlled by `config.mk`, which is included by the `Makefile`. By default, `config.mk` enables all three image formats:

```make
WITH_PNG = 1
WITH_JPEG = 1
WITH_WEBP = 1
```

So running the plain build command already builds with PNG, JPEG, and WebP support:

```
make
```

### Disabling a format

To build without a given format, override the corresponding variable on the command line, e.g. to build without WebP:

```
make WITH_WEBP=0
```

Or edit `config.mk` directly to change the defaults.

### Static build

```
make static
```

### Clean build artifacts

```
make clean
```

## Usage
```bash
./img image_file.png
./img image_file.jpg
./img image_file.webp
./img image1.png image2.jpg image3.webp
./img ~/Pictures/
```
Pass one or more image files as arguments, or a directory to load every image inside it. The viewer displays the first image and lets you navigate, zoom, and pan from there.

### Keybindings
| Key | Action |
|-----|--------|
| `h` / `l` / `j` / `k` | Pan left / right / up / down |
| `Shift+h` / `Shift+l` | Previous / next image |
| `+` / `-` | Zoom in / out |
| `0` | Reset zoom |
| `q` / `Esc` | Quit |

## Comparison with Other Viewers

`img` is significantly more lightweight than popular X11 image viewers:

| Viewer | Binary Size | Text Section | Total Size | Comparison |
|--------|-------------|--------------|------------|------------|
| **img** | **~16 KB** | **15,207 bytes** | **16,463 bytes** | - |
| nsxiv | ~76 KB | 75,791 bytes | 88,983 bytes | 5.4 larger |
| feh | ~169 KB | 173,180 bytes | 184,885 bytes | **11.2 larger** |

## Project Structure

- `main.c` - Entry point and event loop
- `backend/` - Image format decoders
  - `backend.h/c` - Backend interface
  - `png/` - PNG decoder
  - `jpeg/` - JPEG decoder
  - `webp/` - WebP decoder
- `x11/` - X11 windowing and rendering
- `files/` - File management utilities
- `config.mk` - Build configuration

## About the Code

`img` is deliberately small: the whole program is built from four cooperating pieces, wired together in `main.c`.

**Entry point (`main.c`)**

The `main` function is a straight line with no hidden state:

1. Parse arguments — at least one file path is required.
2. `files_collect()` (from `files/files.c`) gathers the given paths into a `FileList`.
3. `img_load()` (from `backend/backend.c`) decodes the first file into an in-memory `Image`.
4. `x11_init()` opens an X11 window sized to the image.
5. `x11_draw()` blits the decoded image to the window.
6. A tiny event loop calls `x11_wait_event()`
7. On exit, `img_free()`, `x11_destroy()`, and `files_free()` release everything that was allocated.

There's no image cache, no multi-file navigation, and no configuration file to parse.

**Backend interface (`backend/`)**

`backend.h`/`backend.c` define a small dispatch layer: `img_load()` looks at the file and hands off to the matching decoder. Each format lives in its own subdirectory (`png/`, `jpeg/`, `webp/`) as an isolated backend that implements the same interface. This is what the `Makefile` conditionally compiles in:

**X11 layer (`x11/`)**

Handles window creation, image blitting, and the event loop primitives (`x11_init`, `x11_draw`, `x11_wait_event`, `x11_destroy`). This is the only part of the codebase that talks to Xlib directly, keeping windowing concerns out of `main.c` and the decoders.

**File handling (`files/`)**

`files_collect`/`files_free` turn the raw `argv` into a `FileList` structure and manage its lifetime. Currently minimal since `img` only displays the first file passed, but structured so multi-file support (e.g. next/previous) could be added without touching the backend or X11 layers.

## License

GNU General Public License v3.0 - See LICENSE file for details

## Author

Krzysztof Marciniak
