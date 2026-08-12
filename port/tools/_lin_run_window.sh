#!/bin/bash
# Try a REAL SDL2 window under WSLg. Uses the selftest frame cap so it opens,
# renders N frames to the actual window, dumps the final framebuffer, exits.
# This exercises SDL_CreateWindow/Renderer/Texture + RenderPresent for real.
B=/mnt/c/tmp/sm64ds-linux/b
OUT=/mnt/c/tmp/sm64ds-linux/b/out
mkdir -p "$OUT"
cd "$OUT"
FRAMES="${1:-300}"
unset SDL_VIDEODRIVER            # let SDL pick wayland/x11 under WSLg
export SM64DS_WINDOW_SELFTEST="$FRAMES"
export SM64DS_VOLUME=0
export SM64DS_NO_AUDIO=1
export SM64DS_FAULTS_FATAL=1
export SM64DS_NO_PLAYLOG=1
export PORT_REPO_ROOT=/mnt/c/tmp/sm64ds-linux
echo "DISPLAY=$DISPLAY WAYLAND_DISPLAY=$WAYLAND_DISPLAY"
"$B/walk_window" > "$OUT/window_${FRAMES}.log" 2>&1
echo "EXIT=$?"
echo "=== SDL driver / window lines ==="
grep -iE "SDL|driver|window|renderer|texture|wayland|x11|failed" "$OUT/window_${FRAMES}.log" | head -15
echo "=== tail ==="
tail -5 "$OUT/window_${FRAMES}.log"
echo "=== bmp dumped? ==="
ls -la "$OUT"/*.bmp 2>/dev/null | tail -3
