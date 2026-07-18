# img

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

### Basic build (X11 only, no image formats)
```bash
make
```

### With PNG support
```bash
make WITH_PNG=1
```

### With JPEG support
```bash
make WITH_JPEG=1
```

### With WebP support
```bash
make WITH_WEBP=1
```

### With all format support
```bash
make WITH_PNG=1 WITH_JPEG=1 WITH_WEBP=1
```

### Clean build artifacts
```bash
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

## License

GNU General Public License v3.0 - See LICENSE file for details

## Author

Krzysztof Marciniak
