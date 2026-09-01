#!/bin/sh
# VS16: N windows, one loopback session, one arena.
#
# The VS4P four-window script, generalised. Quiet and muted per the standing
# rule (minimized, never activated, volume 0). The port base is derived from
# this shell's PID so two lanes never collide, is stepped by 20 rather than 10
# because a wide session's slot span is sixteen ports wide, and stays clear of
# 51765 (the owner's desk pair) and the 58434-58733 band.
#
# THE ONE THING THIS SCRIPT DOES THAT four.sh DID NOT is export
# SM64DS_VS_PLAYERS. That is not a convenience: it is the number the carrier
# reads to decide which of the two wires the session speaks
# (port/hal/comms_loopback.cpp, THE TWO WIRES), and every peer must read the
# same one or they refuse each other -- which is the design working, but it
# makes a run that forgets it look like a bug.
#
#   $1  how many windows (2..16)
#   $2  frames of selftest per CHILD (default 900)
#   $3  VS map index 0..3 (default 0 = level 51)
ROOT="C:/tmp/vs16"
EXE="$ROOT/build/port/walk_window.exe"
N="${1:-4}"
FRAMES="${2:-900}"
VSMAP="${3:-0}"
OUT="C:/tmp/vs16-out/rung$N"

if [ "$N" -lt 2 ] || [ "$N" -gt 16 ]; then
  echo "N must be 2..16"; exit 2
fi

# PID-derived base in 42000..49900, stepped by 20 so slot k = base+k is clear
# for sixteen slots and two neighbouring bases cannot interleave.
BASE=$(( 42000 + ($$ % 390) * 20 ))

# THE PARENT OUTLIVES EVERYONE, and this was learned the hard way at the
# five-window rung. four.sh gave every window the same budget and slept a whole
# second between children -- fine at four windows, and at five and up it
# MANUFACTURES A FALSE DESYNC: the parent spends its own budget WAITING for the
# last joiner, so by the time that window's world boots the parent is at the end
# of its run and about to send BYE. The symptom is a last window whose round
# counter is pinned and whose world hash diverges, which reads exactly like a
# real desync and is not one.
#
# So the stagger is short (enough for the parent to bind before the first JOIN,
# no more) and the parent's budget is the children's plus the whole join window.
PBUDGET=$(( FRAMES + 60 * (N + 8) ))

echo "windows $N  ports $BASE..$((BASE + N - 1))  child frames $FRAMES  parent $PBUDGET  map $VSMAP  out $OUT"

rm -rf "$OUT"
k=0
while [ "$k" -lt "$N" ]; do mkdir -p "$OUT/p$k/tmp"; k=$((k + 1)); done

export SM64DS_ASSET_ROOT="C:\\tmp\\vs16"
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

# One held direction per window so every player walks a different way and a
# capture can tell them apart. Sixteen windows and four directions, so the
# directions repeat -- fine for telling bodies apart on screen, and NOT what
# the desync tracker reads (it hashes world state, not input).
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
  : # no stagger: every window boots and joins together (see the note above)
  k=$((k + 1))
done
wait
echo "all $N exited"
