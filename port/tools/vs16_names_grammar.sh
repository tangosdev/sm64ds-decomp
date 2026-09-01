#!/bin/sh
# VS16: does the GAME actually read a sixteen-field name and colour string?
#
# One window, solo, VS arena, with the two variables set to their WIDE shape.
# This proves the readers in hal/star_flow.cpp and hal/fs_mods.cpp accept the
# sixteen-field grammar end to end, which is a different question from whether
# sixteen people can play -- and it is answerable without the wall.
#
# It also runs the NARROW shape and the two malformed counts either side of
# each legal one, because a grammar that accepts everything proves nothing.
ROOT="C:/tmp/vs16"
EXE="$ROOT/build/port/walk_window.exe"
OUT="C:/tmp/vs16-out/names16"
rm -rf "$OUT"; mkdir -p "$OUT/tmp"
cd "$OUT"

export SM64DS_ASSET_ROOT="C:\\tmp\\vs16"
export SM64DS_VS_MAP=0
export SM64DS_NO_FOCUS=1
export SM64DS_MINIMIZED=1
export SM64DS_VOLUME=0
export SM64DS_NO_DIALOG=1
export SM64DS_WINDOW_SELFTEST=120
export TEMP="$OUT/tmp"
export TMP="$OUT/tmp"

N16="tango,opie,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15"
N4="tango,opie,,"
N8="a,b,c,d,e,f,g,h"
C16="8a2be2:ffd700,ff0000:00ff00,,,,,,,,,,,,,,"
C4="8a2be2:ffd700,ff0000:00ff00,,"
C8="8a2be2:ffd700,,,,,,,"

run() {
  echo "---- $1"
  SM64DS_INSTANCE="$1" SM64DS_VS_NAMES="$2" SM64DS_VS_COLORS="$3" \
    "$EXE" 2>&1 | grep -E "SM64DS_VS_(NAMES|COLORS)" | head -6
}

run narrow  "$N4"  "$C4"
run wide    "$N16" "$C16"
run eight   "$N8"  "$C8"
echo "done"
