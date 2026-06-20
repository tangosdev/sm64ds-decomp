#!/bin/bash
# decomp-permuter compile wrapper for our mwccarm toolchain (native Windows, no wine).
# The permuter invokes this as:  mwccarm_compile.sh <C_FILE> -o <O_FILE>
# It compiles with the SAME canonical compiler+flags as tools/match.py so the
# permuter's candidates are scored against the exact build we match to.
set -e
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
CC="$REPO/tools/mwccarm/1.2/sp2p3/mwccarm.exe"
export LM_LICENSE_FILE="$REPO/tools/mwccarm/license.dat"
C_FILE="$1"   # $2 is "-o", $3 is the output object
O_FILE="$3"
# A source whose first line is "//cpp" is C++ (virtual/PMF dispatch etc.); compile it
# with the C++ frontend, matching the oracle's //cpp path (tools/swarm.py CPP_FLAGS).
LANG="c99"
if head -1 "$C_FILE" | grep -q '^//cpp'; then LANG="c++"; fi
FLAGS="-O4,p -enum int -lang $LANG -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
"$CC" $FLAGS -c -o "$O_FILE" "$C_FILE"
