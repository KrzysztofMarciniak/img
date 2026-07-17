# img

A minimal, lightweight image viewer for X11 written in C99. Supports PNG, JPEG, and WebP formats.

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
```

Pass one or more image files as arguments. The viewer will load and display the first image.

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
