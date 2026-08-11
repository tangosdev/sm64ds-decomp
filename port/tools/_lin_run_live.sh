#!/bin/bash
# Launch a LIVE (non-selftest) window under WSLg for a few seconds, confirm it
# opens and stays alive, report SDL's chosen video driver, then close it.
B=/mnt/c/tmp/sm64ds-linux/b
OUT=/mnt/c/tmp/sm64ds-linux/b/out
mkdir -p "$OUT"
cd "$OUT"
SECS="${1:-4}"
unset SDL_VIDEODRIVER
export SM64DS_VOLUME=0
export SM64DS_NO_AUDIO=1
export SM64DS_NO_PLAYLOG=1
export PORT_REPO_ROOT=/mnt/c/tmp/sm64ds-linux
echo "DISPLAY=$DISPLAY WAYLAND=$WAYLAND_DISPLAY  live for ${SECS}s"
"$B/walk_window" > "$OUT/live.log" 2>&1 &
PID=$!
sleep 1
if kill -0 "$PID" 2>/dev/null; then echo "ALIVE_AFTER_1S pid=$PID"; else echo "DIED_EARLY"; fi
# probe the SDL driver from a tiny sibling program sharing the same env
sleep "$SECS"
if kill -0 "$PID" 2>/dev/null; then
    echo "STILL_ALIVE_AFTER_${SECS}S pid=$PID"
    kill "$PID" 2>/dev/null
    sleep 1
    kill -9 "$PID" 2>/dev/null
else
    echo "EXITED_BEFORE_${SECS}S"
fi
echo "=== log tail ==="
tail -8 "$OUT/live.log"
echo "=== errors ==="
grep -iE "SDL_|failed|error|abort|segfault|fault" "$OUT/live.log" | head -10
