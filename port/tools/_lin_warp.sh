#!/bin/bash
# Boot castle grounds, warp to a course id at frame 60, run to N frames.
B=/mnt/c/tmp/sm64ds-linux/b
OUT=/mnt/c/tmp/sm64ds-linux/b/out
mkdir -p "$OUT"; cd "$OUT"
LVL="${1:-1}"
FRAMES="${2:-400}"
export SDL_VIDEODRIVER=dummy
export SM64DS_WINDOW_SELFTEST="$FRAMES"
export SM64DS_WARP_SEQ="${LVL}@60"
export SM64DS_VOLUME=0
export SM64DS_NO_AUDIO=1
export SM64DS_NO_PLAYLOG=1
export PORT_REPO_ROOT=/mnt/c/tmp/sm64ds-linux
LOG="$OUT/warp_${LVL}_f${FRAMES}.log"
"$B/walk_window" > "$LOG" 2>&1
echo "EXIT=$?  WARP->$LVL @60  FRAMES=$FRAMES"
grep -iE "warpseq|level [0-9]|course|stage actor|tris|FATAL|Segmentation|abort|recursion" "$LOG" | grep -viE "texture|animate" | head -8
echo "last frame reached: $(grep -oE 'f[0-9]+' "$LOG" | tail -1)"
echo "selftest line: $(grep -E 'selftest: [0-9]' "$LOG" | tail -1)"
