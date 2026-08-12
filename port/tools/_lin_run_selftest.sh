#!/bin/bash
# Headless selftest reproduction. Keeps ALL artifacts under our own out/ dir.
B=/mnt/c/tmp/sm64ds-linux/b
OUT=/mnt/c/tmp/sm64ds-linux/b/out
mkdir -p "$OUT"
cd "$OUT"
FRAMES="${1:-300}"
export SDL_VIDEODRIVER=dummy
export SM64DS_WINDOW_SELFTEST="$FRAMES"
export SM64DS_VOLUME=0
export SM64DS_FAULTS_FATAL=1
export SM64DS_NO_PLAYLOG=1
export PORT_REPO_ROOT=/mnt/c/tmp/sm64ds-linux
"$B/walk_window" > "$OUT/selftest_${FRAMES}.log" 2>&1
echo "EXIT=$?"
echo "=== last 20 lines ==="
tail -20 "$OUT/selftest_${FRAMES}.log"
echo "=== frame markers ==="
grep -oE "f[0-9]+ |frame [0-9]+" "$OUT/selftest_${FRAMES}.log" | tail -3
