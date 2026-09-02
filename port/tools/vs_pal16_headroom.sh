#!/bin/sh
# vs_pal16_headroom.sh -- what the sixteen-row palette actually costs.
#
# Measure how much palette VRAM a VS arena boot actually uses, so the 384 extra
# bytes the sixteen-row palette costs can be stated against real headroom
# rather than against the 96K budget in the abstract.
#
# Large palettes FALL from data_020a4bd8 = 0x18000 and small ones RISE from
# data_020a4bcc = 0. SM64DS_TEX_LOG prints paloff per distinct bind, which is
# the material's base*16 -- the palette's own VRAM byte offset. The LOWEST
# paloff seen is how far the falling arena descended.
OUT="${1:-C:/tmp/pal16-out/headroom}"
ROOT="${PAL16_ROOT:-C:/tmp/pal16}"
FRAMES="${2:-400}"
mkdir -p "$OUT/tmp"
cd "$OUT"
TEMP="$OUT/tmp" TMP="$OUT/tmp" \
SM64DS_ASSET_ROOT="$ROOT" \
SM64DS_VS_MAP=0 \
SM64DS_TEX_LOG=1 \
SM64DS_NO_FOCUS=1 SM64DS_MINIMIZED=1 SM64DS_VOLUME=0 SM64DS_NO_DIALOG=1 \
SM64DS_WINDOW_SELFTEST="$FRAMES" \
"$ROOT/build/port/walk_window.exe" > texlog.txt 2>&1
echo "rc=$?  lines=$(wc -l < texlog.txt)"
python - "$OUT/texlog.txt" <<'PY'
import re, sys
offs = sorted(set(int(m, 16) for m in
      re.findall(r"paloff=([0-9a-f]+)", open(sys.argv[1], errors="replace").read())))
if not offs:
    print("no [texbind] paloff lines; is SM64DS_TEX_LOG reaching a bind?")
    raise SystemExit(1)
# THE TWO ARENAS ARE READ APART BY THE GAP, not by a threshold typed here.
# Small palettes rise from 0 and large ones fall from 0x18000, so the offsets
# come out as two clusters with one wide gap between them; the widest gap IS
# the free space, and that is the number this probe exists to print.
gaps = [(offs[i+1] - offs[i], i) for i in range(len(offs) - 1)]
width, at = max(gaps) if gaps else (0, -1)
rising_top = offs[at] if at >= 0 else offs[-1]
falling_floor = offs[at+1] if at >= 0 else 0x18000
print("distinct palette offsets: %d, from 0x%05x to 0x%05x" %
      (len(offs), offs[0], offs[-1]))
print("rising arena (small palettes, up from 0)   tops out at 0x%05x" % rising_top)
print("falling arena (large palettes, down from 0x18000) floors at 0x%05x"
      % falling_floor)
print("FREE SPACE BETWEEN THEM: 0x%05x bytes (%d)" % (width, width))
print("the sixteen-row palette costs 0x180 (384) more than the ROM's four,")
print("which is %.2f%% of that gap" % (100.0 * 384 / width if width else 0))
PY
