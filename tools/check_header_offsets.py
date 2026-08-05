"""Confirm every field in a struct header lands on the offset its comment claims.

The generated headers encode layout twice: once as a sequence of declarations and
pads, and once as a `/* 0x0a4 */` comment per field. Retyping a field silently
breaks the agreement between them unless the following pad is shrunk to match, and
nothing else in the build would notice -- a header is not compiled on its own, and
the byte gate cannot see a field no source file happens to read.

This walks the declarations, applies natural alignment, and compares. Used as the
first gate on any header edit; see notes/plan-scalar-markers.md 4.

    python tools/check_header_offsets.py include/Enemy.h include/Camera.h
    python tools/check_header_offsets.py $(git diff --name-only include/)
"""
import re, sys, pathlib
SZ = {"u8":1,"s8":1,"char":1,"u16":2,"s16":2,"short":2,"u32":4,"s32":4,"int":4,
      "Fix12i":4,"float":4,"u64":8,"s64":8}
DECL = re.compile(r"^\s*(\w+)\s+(\w+)\s*(?:\[\s*(0x[0-9a-fA-F]+|\d+)\s*\])?\s*;(?:\s*/\*\s*(0x[0-9a-fA-F]+))?")
for path in sys.argv[1:]:
    txt = pathlib.Path(path).read_text(errors="replace")
    off, bad, n = 0, 0, 0
    started = False
    for line in txt.splitlines():
        if re.match(r"^\s*struct \w+ \{", line):
            started = True; continue
        if not started: continue
        if re.match(r"^\s*(#|\}|/\* methods)", line): break
        m = DECL.match(line)
        if not m: continue
        typ, name, arr, decl = m.groups()
        if typ not in SZ: continue
        w = SZ[typ]
        if off % w:                       # compiler would insert padding here
            off += w - (off % w)
        if decl is not None:
            n += 1
            if int(decl, 16) != off:
                print(f"  MISMATCH {path} {name}: comment {decl}, computed 0x{off:03x}")
                bad += 1
        off += w * (int(arr, 0) if arr else 1)
    print(f"{path}: {n} commented fields, {bad} mismatched, struct spans 0x{off:x}")
