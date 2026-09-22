#!/usr/bin/env python
"""Group every crash the boot sweep kept by WHERE IT FAULTED, not by level id.

Clustering by level number is what makes a sweep look like fifty bugs. The
faulting address is what says how many there actually are, and the map turns
that address into a name a lane can act on without re-deriving it.

The module base in crash.txt is the runtime one; walk_window.map is written
against the preferred load address, so every address is rebased before it is
looked up. Getting that wrong resolves every row to the wrong name, or to none,
and the whole table then reads as a build that crashed in functions this
repository does not contain.
"""

import os
import re
import sys

SWEEP = sys.argv[1]
MAPF = sys.argv[2]

MAP_ROW = re.compile(
    r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s+")
PREF = re.compile(r"Preferred load address is ([0-9a-fA-F]+)")

syms = []
pref = 0x400000
with open(MAPF, encoding="utf-8", errors="replace") as f:
    for line in f:
        m = PREF.search(line)
        if m:
            pref = int(m.group(1), 16)
            continue
        m = MAP_ROW.match(line)
        if m:
            syms.append((int(m.group(2), 16), m.group(1)))
syms.sort()
addrs = [a for a, _n in syms]

import bisect  # noqa: E402


def name_of(va):
    i = bisect.bisect_right(addrs, va) - 1
    if i < 0:
        return "?", 0
    return syms[i][1], va - addrs[i]


rows = []
for d in sorted(os.listdir(SWEEP)):
    p = os.path.join(SWEEP, d, "crash.txt")
    if not os.path.isfile(p):
        continue
    txt = open(p, encoding="utf-8", errors="replace").read()

    def field(k):
        m = re.search(r"^%s\s+(\S+)" % k, txt, re.M)
        return m.group(1) if m else ""

    code = field("code")
    base = int(field("module") or "400000", 16)
    eip = int(field("eip") or "0", 16)
    acc = re.search(r"^access\s+(\S+) at (\S+)", txt, re.M)
    what = acc.group(2) if acc else ""

    if eip < 0x10000:
        nm, off, kind = "(no code: jumped to 0x%x)" % eip, 0, "NULLCALL"
    elif 0xC0000000 <= eip <= 0xFFFFFFFF or (eip >> 24) in (0xCC, 0xCD, 0xDD):
        nm, off, kind = "(garbage pointer 0x%08x)" % eip, 0, "GARBAGE"
    else:
        nm, off = name_of(eip - base + pref)
        kind = "INCODE"
    rows.append((d, code, kind, "%s+0x%x" % (nm, off) if kind == "INCODE" else nm,
                 what))

groups = {}
for d, code, kind, nm, what in rows:
    groups.setdefault((code, kind, nm), []).append(d)

print("%d crash reports, %d distinct fault sites\n" % (len(rows), len(groups)))
for (code, kind, nm), ds in sorted(groups.items(), key=lambda kv: -len(kv[1])):
    lv = sorted(x for x in ds if x.startswith("level"))
    sc = sorted(x for x in ds if x.startswith("scene"))
    print("== %d rows  code %s  %s" % (len(ds), code, kind))
    print("   at %s" % nm)
    if lv:
        print("   levels: %s" % " ".join(x.replace("level", "") for x in lv))
    if sc:
        print("   scenes: %s" % " ".join(x.replace("scene", "") for x in sc))
    print()
