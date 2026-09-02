#!/bin/sh
# THE WIDE SESSION TO AN AGREED STOP ROUND, then every pairing through dhdiff.
#
#   sh port/tools/vs16_stop_sweep.sh N STOP_ROUND [FRAMES] [MAP]
#
# vs16_ladder.sh gives every window its own FRAME budget, so the windows tear
# down at different rounds and the last one to four hashed frames of whichever
# window stops first can carry state no other window computed
# (status/LAGDELAY.md, P5). This runs the same ladder with
# SM64DS_COMMS_STOP_ROUND set, so every window ends its selftest at the same
# completed round, and then diffs ALL pairings of the untrimmed logs. FRAMES is
# the budget behind the stop round and must be larger than it; the default is
# STOP_ROUND + 300.
#
# Every knob a lane wants (SM64DS_COMMS_INPUT_DELAY, SM64DS_COMMS_DELAY_MS,
# VS16_ROOT, VS16_OUT, VS16_ASSET_ROOT) is inherited from the environment, the
# way vs16_ladder.sh already inherits it.
N="${1:?windows}"
STOP="${2:?stop round}"
FRAMES="${3:-$((STOP + 300))}"
VSMAP="${4:-0}"
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT="${VS16_ROOT:-$(cd "$HERE/../.." && pwd)}"
OUT="${VS16_OUT:-$ROOT/runs/vsdec/out/VSSTOP}"
export VS16_ROOT="$ROOT"
export VS16_OUT="$OUT"
export VS16_ASSET_ROOT="${VS16_ASSET_ROOT:-$ROOT}"
export SM64DS_COMMS_STOP_ROUND="$STOP"

echo "stop sweep: $N windows, agreed stop round $STOP, frame budget $FRAMES behind it, map $VSMAP"
echo "  input delay ${SM64DS_COMMS_INPUT_DELAY:-(mode default)}  induced one-way ${SM64DS_COMMS_DELAY_MS:-0} ms"
sh "$HERE/vs16_ladder.sh" "$N" "$FRAMES" "$VSMAP" || exit 1

D="$OUT/rung$N"
echo "stopped at:"
k=0
while [ "$k" -lt "$N" ]; do
  echo "  p$k  $(grep -o 'SM64DS_COMMS_STOP_ROUND: completed round [0-9]* (agreed stop [0-9]*) at frame [0-9]*' "$D/p$k/run.log" | head -1)"
  echo "      $(grep -o 'closed after [0-9]* rounds; indelay=[0-9]* starved=[0-9]*' "$D/p$k/run.log" | head -1)"
  k=$((k + 1))
done

echo "pairwise dhdiff over $N windows in $D (untrimmed logs)"
i=0
fail=0
pairs=0
while [ "$i" -lt "$N" ]; do
  j=$((i + 1))
  while [ "$j" -lt "$N" ]; do
    out=$(python "$HERE/dhdiff.py" "$D/p$i/run.log" "$D/p$j/run.log" 2>&1)
    pairs=$((pairs + 1))
    if echo "$out" | grep -q "NO DIVERGENCE"; then
      echo "p$i vs p$j  NO DIVERGENCE  ($(echo "$out" | grep -o 'common frames: [0-9]*'))"
    else
      echo "p$i vs p$j  DIVERGED or unreadable:"
      echo "$out" | head -8 | sed 's/^/    /'
      fail=$((fail + 1))
    fi
    j=$((j + 1))
  done
  i=$((i + 1))
done
echo "stop sweep: $pairs pairings, fail=$fail"
[ "$fail" -eq 0 ] && echo "stop sweep: NO DIVERGENCE across all $N windows to round $STOP"
exit $fail
