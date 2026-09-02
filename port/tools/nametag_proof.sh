#!/bin/sh
# NAME TAGS: a wide muted session that draws them, and the divergence verdict.
#
# port/tools/vs16_ladder.sh with three things changed and nothing else:
#   - the tree is this lane's worktree rather than C:/tmp/vs16
#   - SM64DS_VS_NAMES carries real nicknames, so a tag has something to say
#     and the run also exercises the sixteen-field grammar
#   - every window keeps its walk_window_selftest.bmp, which is written into
#     its own $OUT/pN directory because the ladder cd's there first
#
# Quiet and muted per the standing rule: minimized, never activated, volume 0.
# The port base is derived from this shell's PID so two lanes never collide.
#
#   $1  how many windows (2..16)
#   $2  frames of selftest per CHILD (default 900)
#   $3  VS map index 0..3 (default 0 = level 51)
ROOT="C:/tmp/nametag"
EXE="$ROOT/build/port/walk_window.exe"
N="${1:-4}"
FRAMES="${2:-900}"
VSMAP="${3:-0}"
OUT="C:/tmp/nametag-out/rung$N"

if [ "$N" -lt 2 ] || [ "$N" -gt 16 ]; then
  echo "N must be 2..16"; exit 2
fi

BASE=$(( 42000 + ($$ % 390) * 20 ))
PBUDGET=$(( FRAMES + 60 * (N + 8) ))

echo "windows $N  ports $BASE..$((BASE + N - 1))  child frames $FRAMES  parent $PBUDGET  map $VSMAP  out $OUT"

rm -rf "$OUT"
k=0
while [ "$k" -lt "$N" ]; do mkdir -p "$OUT/p$k/tmp"; k=$((k + 1)); done

export SM64DS_ASSET_ROOT="C:\\tmp\\nametag"
export SM64DS_VS_MAP="$VSMAP"
export SM64DS_VS_PLAYERS="$N"
export SM64DS_COMMS_PORT="$BASE"
export SM64DS_COMMS_REPORT=1
export SM64DS_VS_PROBE=1
export SM64DS_VS_STATE_HASH=2
export SM64DS_NO_FOCUS=1
export SM64DS_MINIMIZED=1
export SM64DS_VOLUME=0
export SM64DS_NO_DIALOG=1
export SM64DS_NAME_TAGS=1

# The nicknames. Sixteen fields when the session is wide, four when it is not:
# the grammar hal/star_flow.cpp reads settles the shape by comma count, so the
# string has to match the session or it is dropped wholesale and every tag falls
# back to PLAYER n+1 (which would still draw, and would prove less).
if [ "$N" -gt 4 ]; then
  export SM64DS_VS_NAMES="Tango,Opie,Hermit,Vessel,Kappa,Luma,Bob,Ella,Gus,Hana,Iggy,Jo,Kit,Lux,Moe,Nyx"
else
  export SM64DS_VS_NAMES="Tango,Opie,Hermit,Vessel"
fi

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

# The verdict: every pairing among the live slots, through the tool the VS16
# lanes use. A single DIVERGE line here is the whole proof failing.
echo "--- dhdiff ---"
bad=0
i=0
while [ "$i" -lt "$N" ]; do
  j=$((i + 1))
  while [ "$j" -lt "$N" ]; do
    out=$(python "$ROOT/port/tools/dhdiff.py" "$OUT/p$i/run.log" "$OUT/p$j/run.log" 2>&1)
    rc=$?
    echo "p$i vs p$j: rc=$rc  $(echo "$out" | head -1)"
    [ "$rc" -eq 0 ] || bad=$((bad + 1))
    j=$((j + 1))
  done
  i=$((i + 1))
done
echo "pairings with a problem: $bad"
grep -h "^\[tag\]" "$OUT"/p*/run.log | head -8
exit $bad
