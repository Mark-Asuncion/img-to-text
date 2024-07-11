# Image to ASCII Image

# Usage
```
Usage:
toascii [options] [file]
help:
-h, --help\tprints this message
options:
-o, --output <path>   Save Output Image to <path>. Default is current directory
```

# Dependencies
- ImageMagick (should include Magick++-config) >= 7.1

# Building
run make
```
make
```

# Development
if using clangd LSP run
```
make compile_flags.txt
```
