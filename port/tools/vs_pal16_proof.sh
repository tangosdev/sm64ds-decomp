#!/bin/sh
# PAL16: N windows in one loopback VS session, each seat wearing its OWN
# palette row, with the colours checked from inside the running game.
#
# WHY A SEPARATE RIG AND NOT A FLAG ON vs16_ladder.sh. The ladder proves the
# SIMULATION agrees (world hashes, per-slot movement). This proves the thing
# the ladder is blind to by construction: what colour each seat is painted.
# Sixteen perfectly-agreeing peers can all draw the same green Yoshi, and the
# ladder would call that a clean run -- it did, for the whole of 0.3.2.
#
# So this rig arms SM64DS_VS_COLOR_PROBE, which reads the palette back out of
# the mapped TEXPLTT window AFTER each player's draw (hal/player_bridges.cpp),
# and vs_pal16_check.py turns those lines into a verdict. It keeps
# SM64DS_VS_STATE_HASH on as well, so the same run also answers "and nothing
# desynced", which is the other half of the claim.
#
# Quiet and muted per the standing rule: minimized, never activated, volume 0.
#
#   $1  how many windows (2..16), default 7
#   $2  frames of selftest per CHILD, default 900
#   $3  VS map index 0..3, default 0 (level 51)
#
# The colour string is SM64DS_VS_COLORS in the sixteen-field shape. SLOT 5 IS
# 000000:000000 on purpose -- that is the owner's own report, "a player who
# picked black came out brown", and black is also the pick that used to be
# hardest to see going wrong, because a wrong ROW is a plausible-looking Yoshi
# rather than an obviously broken one.
ROOT="${PAL16_ROOT:-C:/tmp/pal16}"
EXE="$ROOT/build/port/walk_window.exe"
N="${1:-7}"
FRAMES="${2:-900}"
VSMAP="${3:-0}"
OUT="${PAL16_OUT:-C:/tmp/pal16-out}/pal$N"

if [ "$N" -lt 2 ] || [ "$N" -gt 16 ]; then
  echo "N must be 2..16"; exit 2
fi
if [ ! -f "$EXE" ]; then
  echo "no walk_window.exe at $EXE"; exit 2
fi

BASE=$(( 42000 + ($$ % 390) * 20 ))
PBUDGET=$(( FRAMES + 60 * (N + 8) ))

# Sixteen fields, exactly fifteen commas, every field bbbbbb:ssssss. Twelve of
# the sixteen are picks and four are left empty so the run also exercises the
# defaulted path in the same session: seats 3, 7, 11 and 15 wear the built-in
# row for their slot rather than a pick.
COLORS="ff0000:0000ff,00ff00:ff00ff,0000ff:ffff00,,ff8800:004488,000000:000000,00ffff:880000,,ff00ff:00ff88,8800ff:ffcc00,ffffff:cc0000,,00ff88:660066,ff99cc:330066,999999:0044cc,"

echo "windows $N  ports $BASE..$((BASE + N - 1))  child frames $FRAMES  parent $PBUDGET  map $VSMAP"
echo "out $OUT"
echo "colors $COLORS"

rm -rf "$OUT"
k=0
while [ "$k" -lt "$N" ]; do mkdir -p "$OUT/p$k/tmp"; k=$((k + 1)); done

export SM64DS_ASSET_ROOT="$ROOT"
export SM64DS_VS_MAP="$VSMAP"
export SM64DS_VS_PLAYERS="$N"
export SM64DS_VS_COLORS="$COLORS"
export SM64DS_COMMS_PORT="$BASE"
export SM64DS_COMMS_REPORT=1
export SM64DS_VS_PROBE=1
export SM64DS_VS_COLOR_PROBE=1
export SM64DS_VS_STATE_HASH=2
export SM64DS_NO_FOCUS=1
export SM64DS_MINIMIZED=1
export SM64DS_VOLUME=0
export SM64DS_NO_DIALOG=1

key_for() {
  case $(( $1 % 4 )) in
    0) echo 0x0040 ;;   # up
    1) echo 0x0080 ;;   # down
    2) echo 0x0020 ;;   # left
    *) echo 0x0010 ;;   # right
  esac
}

cd "$OUT/p0"
TEMP="$OUT/p0/tmp" TMP="$OUT/p0/tmp" SM64DS_INSTANCE=p0 \
  SM64DS_WINDOW_SELFTEST="$PBUDGET" \
  SM64DS_COMMS_INJECT="key=$(key_for 0)" \
  SM64DS_COMMS_ROLE=parent "$EXE" > run.log 2>&1 &
echo "parent pid $! budget $PBUDGET" > "$OUT/pids.txt"
sleep 1
k=1
while [ "$k" -lt "$N" ]; do
  cd "$OUT/p$k"
  TEMP="$OUT/p$k/tmp" TMP="$OUT/p$k/tmp" SM64DS_INSTANCE="p$k" \
    SM64DS_WINDOW_SELFTEST="$FRAMES" \
    SM64DS_COMMS_INJECT="key=$(key_for $k)" \
    SM64DS_COMMS_ROLE=child SM64DS_COMMS_SLOT="$k" "$EXE" > run.log 2>&1 &
  echo "child$k pid $!" >> "$OUT/pids.txt"
  k=$((k + 1))
done
wait
echo "all $N exited"
ls "$OUT"/p*/walk_window_selftest.bmp 2>/dev/null
