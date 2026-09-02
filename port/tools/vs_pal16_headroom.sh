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
offs = [int(m, 16) for m in
        re.findall(r"paloff=([0-9a-f]+)", open(sys.argv[1], errors="replace").read())]
if not offs:
    print("no [texbind] paloff lines; is SM64DS_TEX_LOG reaching a bind?")
    raise SystemExit(1)
lo, hi = min(offs), max(offs)
print("distinct palette offsets seen: %d" % len(set(offs)))
print("lowest  paloff 0x%05x  (the falling arena's floor this run)" % lo)
print("highest paloff 0x%05x" % hi)
print("ceiling        0x18000")
print("headroom below the falling arena: 0x%05x bytes (%d)" % (lo, lo))
print("the sixteen-row palette costs 384 (0x180) more than the ROM's four")
PY
