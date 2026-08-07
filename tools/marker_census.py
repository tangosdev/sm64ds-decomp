"""How many object markers are left, and how many are already decidable.

This exists because a worklist reported itself complete while a third more work sat
in the tree with the programme's own gold-standard evidence attached.

`tools/gen_header.py` files a `u8` marker as **confirmed** when a pass observed the
declared 1-byte width at that offset -- which is true, and which quietly removes it
from the marker bucket. So the markers with the BEST evidence fell off the worklist
precisely because the evidence was good. `Goomba.mModelAnim @0x370` is the case that
proves it: `src/_ZN6GoombaD1Ev.c` D1-calls `_ZN9ModelAnimD1Ev((char *)t + 0x370)` --
the very relocation notes/plan-gen-header.md 3 uses as its worked example -- and the
field is still a bare `u8`.

This counts the tree directly rather than a report's bucket, and asks one question
per marker: does the owning class's own destructor name a type at this exact offset?
That is the same D1 evidence the typing programme acts on, so a nonzero
`decidable now` is work that could be done today.

    python tools/marker_census.py
    python tools/marker_census.py --list      # every decidable one, with its evidence
"""
import argparse
import collections
import json
import pathlib
import re
import sys

BANNER = "AUTO-GENERATED from matched-function evidence"
MARKER = re.compile(r"^\s*u8\s+(\w+)\s*;\s*/\*\s*(0x[0-9a-fA-F]+)")
PAD = re.compile(r"^\s*u8\s+pad_[0-9a-fA-F]+\s*\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]")
SKIPPABLE = re.compile(r"^\s*($|/\*|\*|//)")
# _ZN9ModelAnimD1Ev((char *)t + 0xd4)  /  _ZN5ModelD1Ev(c + 0x138)
DTOR_CALL = re.compile(r"(_ZN\w+?D[012]Ev)\s*\(\s*[^;]*?\+\s*(0x[0-9a-fA-F]+)\s*\)")
SIZE_ASSERT = re.compile(r"(\w+)_size_must_be_(0x[0-9a-fA-F]+)")
MANGLED_CLASS = re.compile(r"^_ZN(\d+)")


def class_of_dtor(sym):
    m = re.match(r"^_ZN(\d+)([A-Za-z_]\w*)", sym)
    if not m:
        return None
    n, rest = int(m.group(1)), m.group(2)
    return rest[:n] if len(rest) >= n else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("-o", "--out", default=None)
    args = ap.parse_args()
    root = pathlib.Path(args.root).resolve()

    sizes = {}
    for h in (root / "include").glob("*.h"):
        for m in SIZE_ASSERT.finditer(h.read_text(errors="replace")):
            sizes[m.group(1)] = int(m.group(2), 16)

    # every destructor's (class, offset) -> member type, from the source itself
    named = collections.defaultdict(dict)
    for p in (root / "src").rglob("*.c*"):
        cls = class_of_dtor(p.stem)
        if not cls or not re.search(r"D[012]Ev$", p.stem):
            continue
        for m in DTOR_CALL.finditer(p.read_text(errors="replace")):
            t = class_of_dtor(m.group(1))
            if t and t != cls:
                named[cls][int(m.group(2), 16)] = (t, p.name)

    total, decidable, rows = 0, [], []
    for h in sorted((root / "include").glob("*.h")):
        txt = h.read_text(errors="replace")
        if BANNER not in txt:
            continue
        lines = txt.splitlines()
        for i, line in enumerate(lines):
            m = MARKER.match(line)
            if not m or m.group(1).startswith("pad_"):
                continue
            j = i + 1
            while j < len(lines) and SKIPPABLE.match(lines[j]):
                j += 1
            pm = PAD.match(lines[j]) if j < len(lines) else None
            span = 1 + int(pm.group(1), 0) if pm else None
            if span is not None and span <= 4:
                continue                      # scalar marker, a different problem
            total += 1
            off = int(m.group(2), 16)
            hit = named.get(h.stem, {}).get(off)
            if hit and hit[0] in sizes:
                decidable.append((h.stem, m.group(1), m.group(2), span, hit[0],
                                  sizes[hit[0]], hit[1]))
        rows.append(h.stem)

    print(f"bannered headers                    : {len(rows)}")
    print(f"bare u8 object markers (span > 4)   : {total}")
    print(f"  DECIDABLE NOW -- own dtor D-calls a")
    print(f"  sized type at that exact offset   : {len(decidable)}")
    print()
    if decidable:
        by = collections.Counter(d[4] for d in decidable)
        print("proposed types:", dict(by))
        print("\nThese are not 'remaining unknowns'. They carry the same evidence the")
        print("typing programme already acts on, and could be done today.")
    if args.list:
        print()
        for cls, fld, off, span, typ, sz, ev in decidable:
            print(f"  {cls}.{fld} @{off} span={span}  -> {typ} (0x{sz:x})   [{ev}]")
    if args.out:
        (root / args.out).write_text(json.dumps(
            {"markers_remaining": total, "decidable": [
                {"cls": c, "field": f, "offset": o, "span": s,
                 "type": t, "size": z, "evidence": e}
                for c, f, o, s, t, z, e in decidable]}, indent=1), encoding="utf-8")
        print(f"\nwrote {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
