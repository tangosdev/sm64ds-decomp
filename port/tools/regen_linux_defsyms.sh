#!/usr/bin/env bash
# Regenerate port/linux_defsyms.txt -- the GCC --defsym flags that stand in for
# the port's MSVC /alternatename pragmas on Linux.
#
# Run from a configured Linux build dir (the objects must already be compiled;
# do `ninja walk_window` once and let the LINK fail on undefineds, then run this
# and re-link). It scans the walk_window objects for the truly-undefined symbols,
# pairs each with its resolved definition via the pragmas, and writes the flags.
#
#   cd port/build && ninja walk_window        # compiles; link may report undefs
#   ../tools/regen_linux_defsyms.sh           # rewrites ../linux_defsyms.txt
#   ninja walk_window                         # links clean
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
PORT="$(cd "$HERE/.." && pwd)"
BUILD="${1:-$PORT/build}"
OUTDIR="$(mktemp -d)"
trap 'rm -rf "$OUTDIR"' EXIT

# 1) all /alternatename pragmas
grep -rhoP 'alternatename:\K[^"]+' "$PORT"/hal/*.cpp "$PORT"/unmatched/*.cpp \
    2>/dev/null | sort -u > "$OUTDIR/altnames.txt"

# 2) undefined + defined symbols across the walk_window objects
python3 - "$BUILD" "$OUTDIR" <<'PY'
import subprocess, os, sys
build, out = sys.argv[1], sys.argv[2]
objs = []
for root, _, files in os.walk(os.path.join(build, "CMakeFiles/walk_window.dir")):
    for f in files:
        if f.endswith(".o"):
            objs.append(os.path.join(root, f))
undef, defined = set(), set()
for o in objs:
    for line in subprocess.run(["nm", o], capture_output=True, text=True).stdout.splitlines():
        p = line.split()
        if len(p) == 2 and p[0] == "U":
            undef.add(p[1])
        elif len(p) >= 3 and p[1] in "TtWwVvDdBbRr":
            defined.add(p[2])
real = sorted(undef - defined)
dem = subprocess.run(["c++filt"], input="\n".join(real), capture_output=True, text=True).stdout.splitlines()
open(os.path.join(out, "undef_map.tsv"), "w").write("\n".join(f"{m}\t{d}" for m, d in zip(real, dem)))
open(os.path.join(out, "defined_syms.txt"), "w").write("\n".join(sorted(defined)))
PY

# 3) generate the flags
python3 "$HERE/gcc_aliases.py" \
    --altnames "$OUTDIR/altnames.txt" \
    --undef "$OUTDIR/undef_map.tsv" \
    --defined "$OUTDIR/defined_syms.txt" \
    --out "$PORT/linux_defsyms.txt"
echo "wrote $PORT/linux_defsyms.txt ($(wc -l < "$PORT/linux_defsyms.txt") flags)"
