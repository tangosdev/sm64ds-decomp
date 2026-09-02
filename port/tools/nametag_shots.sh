#!/bin/sh
# NAME TAGS: the pictures.
#
# A SHORT four-window session on purpose. nametag_proof.sh runs long enough to
# say something about lockstep, and by the end of it the four players have walked
# a long way apart in four directions -- which is the right shape for a
# divergence verdict and the wrong one for a photograph. This run gives each
# window a small budget so the selftest BMP is written while the other bodies
# are still in frame, and turns SM64DS_TAG_PROBE on so the log says, per tag,
# which slot it drew, where it landed and how far away it was.
#
# Quiet and muted per the standing rule: minimized, never activated, volume 0.
#
#   $1  frames per window before it writes its BMP (default 240)
#   $2  VS map index 0..3 (default 0)
ROOT="C:/tmp/nametag"
EXE="$ROOT/build/port/walk_window.exe"
N=4
FRAMES="${1:-240}"
VSMAP="${2:-0}"
OUT="C:/tmp/nametag-out/shots"

BASE=$(( 43000 + ($$ % 200) * 20 ))
PBUDGET=$(( FRAMES + 60 ))

echo "shots: $N windows  ports $BASE..$((BASE + N - 1))  frames $FRAMES  out $OUT"

rm -rf "$OUT"
k=0
while [ "$k" -lt "$N" ]; do mkdir -p "$OUT/p$k/tmp"; k=$((k + 1)); done

export SM64DS_ASSET_ROOT="C:\\tmp\\nametag"
export SM64DS_VS_MAP="$VSMAP"
export SM64DS_VS_PLAYERS="$N"
export SM64DS_COMMS_PORT="$BASE"
export SM64DS_COMMS_REPORT=1
export SM64DS_VS_PROBE=1
export SM64DS_NO_FOCUS=1
export SM64DS_MINIMIZED=1
export SM64DS_VOLUME=0
export SM64DS_NO_DIALOG=1
export SM64DS_NAME_TAGS=1
export SM64DS_TAG_PROBE=1
export SM64DS_VS_NAMES="Tango,Opie,Hermit,Vessel"

# Only p0 walks. The other three stand where the arena put them, so p0's own
# camera keeps them in frame for the whole run and the picture is of tags over
# bodies rather than of an empty arena.
cd "$OUT/p0"
TEMP="$OUT/p0/tmp" TMP="$OUT/p0/tmp" SM64DS_INSTANCE=p0 \
  SM64DS_WINDOW_SELFTEST="$PBUDGET" \
  SM64DS_COMMS_ROLE=parent "$EXE" > run.log 2>&1 &
sleep 1
k=1
while [ "$k" -lt "$N" ]; do
  cd "$OUT/p$k"
  TEMP="$OUT/p$k/tmp" TMP="$OUT/p$k/tmp" SM64DS_INSTANCE="p$k" \
    SM64DS_WINDOW_SELFTEST="$FRAMES" \
    SM64DS_COMMS_ROLE=child SM64DS_COMMS_SLOT="$k" "$EXE" > run.log 2>&1 &
  k=$((k + 1))
done
wait
echo "all $N exited"

mkdir -p "$ROOT/status_shots"
k=0
while [ "$k" -lt "$N" ]; do
  if [ -f "$OUT/p$k/walk_window_selftest.bmp" ]; then
    cp "$OUT/p$k/walk_window_selftest.bmp" "$ROOT/status_shots/nametag_p$k.bmp"
    echo "kept status_shots/nametag_p$k.bmp"
  else
    echo "p$k wrote no BMP"
  fi
  k=$((k + 1))
done
echo "--- tag probe ---"
grep -h "^\[tag\]" "$OUT"/p*/run.log | head -30
