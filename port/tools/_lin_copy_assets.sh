#!/bin/bash
set -e
SRC=/mnt/c/Users/bmanu/Documents/sm64ds-decomp/build/assets
DST=/mnt/c/tmp/sm64ds-linux/build/assets
mkdir -p "$DST"
cp "$SRC/files.tsv" "$DST/files.tsv"
cp "$SRC/handles.tsv" "$DST/handles.tsv"
echo COPY_OK
ls -la "$DST"
echo "extracted/dsd/files present: $(ls /mnt/c/tmp/sm64ds-linux/extracted/dsd/files 2>/dev/null | wc -l) entries"
