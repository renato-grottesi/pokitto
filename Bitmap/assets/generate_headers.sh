#!/bin/bash

# Compile the converter.
gcc png_pal_rgb_2_header.c -lpng -o png2h

# Split the atlases into images.
# Exclude the background chunk to limit the palette to the minumum size.
convert kenney_16x16_tiles.png -crop 16x16 -define png:exclude-chunk=bKGD tile_%03d.png
convert kenney_256x32_backgrounds.png -crop 256x32 -define png:exclude-chunk=bKGD bg_%03d.png

# Generate the headers.
/bin/ls tile_* | while read f; do ./png2h $f; done | sed 's/\.png//g' > ../tiles.h
/bin/ls bg_00* | while read f; do ./png2h $f; done | sed 's/\.png//g' > ../backgrounds.h

rm -fr tile_*.png bg_*.png png2h
