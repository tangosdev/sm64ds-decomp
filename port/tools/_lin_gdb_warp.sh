#!/bin/bash
B=/mnt/c/tmp/sm64ds-linux/b
OUT=/mnt/c/tmp/sm64ds-linux/b/out
cd "$OUT"
LVL="${1:-5}"
export SDL_VIDEODRIVER=dummy
export SM64DS_WINDOW_SELFTEST=400
export SM64DS_WARP_SEQ="${LVL}@60"
export SM64DS_VOLUME=0
export SM64DS_NO_AUDIO=1
export SM64DS_NO_PLAYLOG=1
export PORT_REPO_ROOT=/mnt/c/tmp/sm64ds-linux
gdb -q -batch -ex run -ex "bt 20" -ex "info registers pc" "$B/walk_window" 2>&1 \
  | grep -vE "^\[|^ *\+ |texture transform|probe:|^level |selftest:" | tail -30
