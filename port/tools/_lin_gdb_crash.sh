#!/bin/bash
# Run under gdb to the frame-318 crash and dump the backtrace.
B=/mnt/c/tmp/sm64ds-linux/b
OUT=/mnt/c/tmp/sm64ds-linux/b/out
cd "$OUT"
export SDL_VIDEODRIVER=dummy
export SM64DS_WINDOW_SELFTEST="${1:-330}"
export SM64DS_VOLUME=0
export SM64DS_NO_AUDIO=1
export SM64DS_NO_PLAYLOG=1
export PORT_REPO_ROOT=/mnt/c/tmp/sm64ds-linux
which gdb >/dev/null 2>&1 || { echo "gdb: MISSING"; exit 3; }
gdb -q -batch \
    -ex "run" \
    -ex "bt 30" \
    -ex "info registers pc" \
    "$B/walk_window" 2>&1 | grep -vE "^\[f[0-9]|^\[cam\]|^\[w\]|^\[st\]|^\[fx\]|^ *\+ |texture transform" | tail -50
