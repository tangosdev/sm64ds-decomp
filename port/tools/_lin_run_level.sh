#!/bin/bash
# Boot a specific level headless for N frames and report survival.
B=/mnt/c/tmp/sm64ds-linux/b
OUT=/mnt/c/tmp/sm64ds-linux/b/out
mkdir -p "$OUT"; cd "$OUT"
LVL="${1:-1}"
FRAMES="${2:-300}"
export SDL_VIDEODRIVER=dummy
export SM64DS_WINDOW_SELFTEST="$FRAMES"
export SM64DS_LEVEL="$LVL"
export SM64DS_VOLUME=0
export SM64DS_NO_AUDIO=1
export SM64DS_NO_PLAYLOG=1
export PORT_REPO_ROOT=/mnt/c/tmp/sm64ds-linux
LOG="$OUT/lvl${LVL}_f${FRAMES}.log"
"$B/walk_window" > "$LOG" 2>&1
echo "EXIT=$?  LEVEL=$LVL FRAMES=$FRAMES"
grep -iE "level |course|stage|selftest: [0-9]|FATAL|Segmentation|abort" "$LOG" | grep -viE "texture|animate" | head -6
echo "last frame: $(grep -oE 'f[0-9]+' "$LOG" | tail -1)"
