#!/bin/bash
# Linux port build into a PRIVATE short WSL-native path.
set -e
SRC=/mnt/c/tmp/sm64ds-linux/port
# Windows-fs build dir so artifacts persist across WSL instance restarts.
B=/mnt/c/tmp/sm64ds-linux/b
mkdir -p "$B"
cmake -S "$SRC" -B "$B" -G Ninja -DCMAKE_BUILD_TYPE=Release "$@" 2>&1 | tail -25
echo "=== BUILD walk_window ==="
ninja -C "$B" walk_window 2>&1 | tail -40
echo "=== EXIT ninja: $? ==="
ls -la "$B/walk_window" 2>/dev/null && echo BUILT_OK || echo NOT_BUILT
