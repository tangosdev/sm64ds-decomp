#!/bin/bash
# Build the Linux port into a NATIVE ext4 build dir, then copy the finished
# binary out to /mnt/c so it survives a WSL restart.
#
# Why not /mnt/c for the build dir: the 9p filesystem stalls badly on the link
# step. Why not /tmp: WSL clears it when the VM shuts down on idle, and losing
# the build dir costs a full ~4400-target reconfigure and rebuild -- which is
# most of the cost of a merge-verification cycle. /root persists across those
# shutdowns, so an incremental rebuild stays incremental.
set -e
SRC=/mnt/c/tmp/sm64ds-linux/port
NB=/root/lbuild                    # native, persistent across WSL idle shutdown
OUT=/mnt/c/tmp/sm64ds-linux/b      # persistent copy target
mkdir -p "$NB" "$OUT"
if [ ! -f "$NB/build.ninja" ]; then
    echo "=== configuring (native) ==="
    cmake -S "$SRC" -B "$NB" -G Ninja -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -3
fi
# -k 0 so one pass reports EVERY error. Stopping at the first failing TU turns a
# batch of platform gaps into one rebuild each, which is how the last merge got
# expensive.
echo "=== building walk_window (native ext4, -k 0) ==="
/usr/bin/time -v ninja -C "$NB" -k 0 walk_window > /tmp/blog.txt 2>&1
echo "NINJA_EXIT=$?"
grep -E "error:" /tmp/blog.txt | sed "s|$SRC/||" | sort -u | head -40
# Undefined symbols usually mean linux_defsyms.txt is stale after new
# /alternatename pragmas landed: run tools/regen_linux_defsyms.sh "$NB", then
# re-run this. See that script's header.
grep -oE "undefined reference to \`[^']*'" /tmp/blog.txt | sort -u | head -20
grep -E "Elapsed \(wall" /tmp/blog.txt || true
if [ -x "$NB/walk_window" ]; then
    cp "$NB/walk_window" "$OUT/walk_window"
    echo "COPIED to $OUT/walk_window"
    ls -la "$OUT/walk_window"
    file "$OUT/walk_window"
else
    echo "BUILD_FAILED: no native binary"
fi
