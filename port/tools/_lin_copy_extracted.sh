#!/bin/bash
set -e
SRC=/mnt/c/Users/bmanu/Documents/sm64ds-decomp/extracted
DST=/mnt/c/tmp/sm64ds-linux/extracted
rm -rf "$DST"
cp -r "$SRC" "$DST"
echo COPY_OK
ls "$DST"
ls -la "$DST/arm9_dec.bin"
echo "overlays: $(ls "$DST/overlays" | wc -l)"
