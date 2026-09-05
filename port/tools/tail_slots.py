#!/usr/bin/env python3
"""What the ROM puts in a hosted vtable's slots, and which of those words the
port's link is still missing.

WHY. Run link100 lane TAIL's work list is "a slot the port fills with something
other than the ROM's own word". Deciding one of those by hand means opening
config/<module>/relocs.txt, finding the entry whose `from:` is base + 4*i,
resolving its `to:` against config/<module>/symbols.txt, and then asking
walk_window.map whether the TU of that name is in the binary. Three files and
an address arithmetic per slot, thirty-odd slots per table: exactly the shape
that gets transcribed wrong once and then quoted forever.

WHAT IT PRINTS, one line per slot:

    slot  from-address  to-address  symbol                     L/U/X

  L  the symbol's matched TU is in walk_window.map      (nothing to do)
  U  a matched TU of that name exists in src/ and is NOT in the map
     (the candidate: the ROM says this word belongs here and the port's
     link does not carry it)
  X  no matched TU of that name exists at all           (host code's job)

and a tail summary counting the U rows, which is the only number a seat
proposal should ever quote for a table.

THE MODULE MATTERS AND IS NOT GUESSED. Overlays share address space, so
0x02112408 answers both _ZTV12SwitchPillar in ov047 and _ZTV14daObjC0Water_c
in ov012 -- the caveat hal/w2_dtor_heads.cpp closes its sweep with. This tool
therefore takes the module explicitly and resolves both ends inside it, with
arm9 as the only fallback for a `to:` whose module column says main.

    python port/tools/tail_slots.py --root C:/tmp/l1-tail \
           --module ov002 --vtable 0x021093e0 --width 31
    python port/tools/tail_slots.py --root . --selftest
"""
import argparse
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import linkage  # noqa: E402


def load_symbols(path):
    """addr -> name, for exact-address lookups."""
    out = {}
    if not os.path.exists(path):
        return out
    for line in open(path, errors="replace"):
        m = re.match(r"(\S+) kind:(\w+)(?:\(([^)]*)\))? addr:0x([0-9a-f]+)", line)
        if not m:
            continue
        out.setdefault(int(m.group(4), 16), m.group(1))
    return out


def load_relocs(path):
    """from-addr -> (to-addr, to-module)."""
    out = {}
    if not os.path.exists(path):
        return out
    for line in open(path, errors="replace"):
        m = re.match(r"from:0x([0-9a-f]+) kind:(\w+) to:0x([0-9a-f]+) module:(\S+)",
                     line)
        if not m:
            continue
        mod = m.group(4)
        if mod.startswith("overlay"):
            digits = re.sub(r"\D", "", mod)
            mod = "ov%03d" % int(digits or 0)
        out[int(m.group(1), 16)] = (int(m.group(3), 16), mod)
    return out


def module_dir(root, module):
    if module in ("main", "arm9"):
        return os.path.join(root, "config", "arm9")
    return os.path.join(root, "config", "arm9", "overlays", module)


def linked_stems(root):
    """Stems of matched TUs that are in walk_window.map."""
    mapfile = os.path.join(root, "build", "port", "walk_window.map")
    matched = linkage.matched_index(root)
    linked = set()
    if os.path.exists(mapfile):
        for _sym, obj in linkage.map_symbols(mapfile):
            stem = os.path.splitext(os.path.splitext(obj)[0])[0]
            if stem in matched:
                linked.add(stem)
    return matched, linked, mapfile


def report(root, module, base, width):
    matched, linked, mapfile = linked_stems(root)
    md = module_dir(root, module)
    relocs = load_relocs(os.path.join(md, "relocs.txt"))
    syms = {module: load_symbols(os.path.join(md, "symbols.txt"))}
    if module not in ("main", "arm9"):
        syms["main"] = load_symbols(
            os.path.join(root, "config", "arm9", "symbols.txt"))
    print("root   %s" % os.path.abspath(root))
    print("map    %s%s" % (mapfile, "" if os.path.exists(mapfile) else "  (MISSING)"))
    print("table  %s 0x%08x, %d slots" % (module, base, width))
    print()
    print("slot  from        to          state symbol")
    want = []
    for i in range(width):
        frm = base + 4 * i
        ent = relocs.get(frm)
        if ent is None:
            print("%4d  0x%08x  --          -     (no relocation: literal word)" % (i, frm))
            continue
        to, tomod = ent
        name = syms.get(tomod, {}).get(to) or syms.get("main", {}).get(to)
        if name is None:
            state, name = "?", "(no exact symbol at 0x%08x in %s)" % (to, tomod)
        elif name in linked:
            state = "L"
        elif name in matched:
            state = "U"
            want.append((i, name))
        else:
            state = "X"
        print("%4d  0x%08x  0x%08x  %-5s %s" % (i, frm, to, state, name))
    print()
    print("WANT (matched TU exists, not in the map): %d" % len(want))
    for i, n in want:
        print("   slot %-3d %s   %s" % (i, n, matched[n]))
    return want


def selftest():
    """The parsers, on fixtures -- no tree, no map, no toolchain."""
    import tempfile
    ok = True

    def check(what, got, exp):
        nonlocal ok
        if got != exp:
            ok = False
            print("FAIL %s: %r != %r" % (what, got, exp))

    with tempfile.TemporaryDirectory() as td:
        s = os.path.join(td, "symbols.txt")
        open(s, "w").write(
            "_ZN5ThingD1Ev kind:function(arm,size=0x20) addr:0x0211abcd\n"
            "_ZTV5Thing kind:data(any) addr:0x02110000\n"
            "junk line with no addr\n")
        check("symbols", load_symbols(s),
              {0x0211abcd: "_ZN5ThingD1Ev", 0x02110000: "_ZTV5Thing"})
        r = os.path.join(td, "relocs.txt")
        open(r, "w").write(
            "from:0x02110040 kind:load to:0x0211abcd module:overlays(2)\n"
            "from:0x02110044 kind:load to:0x02043b2c module:main\n"
            "noise\n")
        check("relocs", load_relocs(r),
              {0x02110040: (0x0211abcd, "ov002"),
               0x02110044: (0x02043b2c, "main")})
        # a module column that already reads ovNNN is left alone
        open(r, "w").write("from:0x1 kind:load to:0x2 module:ov007\n")
        check("relocs ovNNN", load_relocs(r), {0x1: (0x2, "ov007")})
        # the multi-overlay module spelling picks the FIRST number, which is
        # what dsd writes for a shared address; the caller passes the module in
        # explicitly for exactly that reason.
        open(r, "w").write("from:0x1 kind:load to:0x2 module:overlays(2,4)\n")
        check("relocs multi", load_relocs(r), {0x1: (0x2, "ov024")})
    check("module_dir arm9", module_dir("R", "main").replace("\\", "/"),
          "R/config/arm9")
    check("module_dir ov", module_dir("R", "ov081").replace("\\", "/"),
          "R/config/arm9/overlays/ov081")
    print("tail_slots selftest %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".")
    ap.add_argument("--module", default="main")
    ap.add_argument("--vtable", type=lambda x: int(x, 0))
    ap.add_argument("--width", type=int, default=32)
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if a.vtable is None:
        ap.error("--vtable is required")
    report(a.root, a.module, a.vtable, a.width)
    return 0


if __name__ == "__main__":
    sys.exit(main())
