#!/usr/bin/env python3
"""Cross-check langmode_audit's layout_free set against the ROM's own data.

`layout_free` means: no `this`, no vtable, no struct -- so migrating the function
to a real namespace function cannot shift anyone's offsets. langmode_audit.py
decides it from the mangled name and the presence of a header, and deliberately
never reads the ROM: plan phase 0 scoped it to run on a bare clone with no
compiler and no extracted ROM, and that property is worth keeping.

This tool is the other half. It needs the ROM and answers the one question the
static test cannot: IS THE FUNCTION'S ADDRESS STORED SOMEWHERE AS DATA? A
function whose address sits in a vtable is not layout-free, and no amount of
name analysis will tell you.

WHAT COUNTS AS A HIT, AND WHY THE DISTINCTION MATTERS. An earlier, ad-hoc version
of this check called every address-taken function a false positive and reported
27 of 84. That was wrong by about half, and the error is worth naming so it is
not repeated: an address that appears INSIDE another function's body is that
function's LITERAL POOL -- how ARM materializes a constant -- and being pointed
at from a pool does not give a function a `this` or a layout. Only a hit OUTSIDE
every known function is data.

So each hit is classified:

    pool    inside some sized function in config/**/symbols.txt  -> harmless
    data    outside every one of them                            -> report it

`data` still covers two different things and the tool does not pretend
otherwise. Consecutive slots per class is a vtable, which is disqualifying.
A contiguous run of single entries is a dispatch table, and a static function may
legitimately live in one -- the sixteen LVL_Overlay loaders sit in exactly such a
table at ov002:0x0210cbb8 and are genuinely free functions. Read the addresses
before concluding.

AND SCAN THE RIGHT MODULE. The first version read only arm9_dec.bin while
candidates live in overlays too, so it silently missed every overlay table. Each
candidate is scanned in the module it belongs to.

This is a REPORT, not a gate: distinguishing the two `data' cases needs someone
to read the addresses, so it exits 0 even when it finds something. --strict makes
it exit non-zero.

Usage:
    python tools/check_layout_free.py            # report
    python tools/check_layout_free.py --verbose  # list every hit address too
    python tools/check_layout_free.py --strict   # non-zero exit on any data hit
"""
import argparse
import bisect
import collections
import pathlib
import re
import struct
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parents[1]
ARM9_BASE = 0x02004000          # arm9_dec.bin loads here, NOT at the RAM base


def modules():
    """{name: (bytes, load address)} for arm9 and every extracted overlay.

    The overlay list is dsd/arm9_overlays/overlays.yaml, NOT a key in
    dsd/config.yaml -- config.yaml only holds a path to it under `arm9_overlays'.
    A first draft of this tool read `config.yaml["overlays"]', got None, and
    scanned arm9 alone; the fourteen ov002 candidates then fell through every
    bucket and were reported by nothing. tools/evidence_rom.py already had this
    right, and this follows it.
    """
    import yaml
    ext = REPO / "extracted"
    out = {"arm9": ((ext / "arm9_dec.bin").read_bytes(), ARM9_BASE)}
    ovl = yaml.safe_load((ext / "dsd/arm9_overlays/overlays.yaml").read_text())
    for o in ovl["overlays"]:
        p = ext / "overlays" / f"overlay_{o['id']:04d}.bin"
        if p.exists():
            base = o["base_address"]
            base = int(base, 16) if isinstance(base, str) else base
            out[f"ov{o['id']:03d}"] = (p.read_bytes(), base)
    return out


def symbols():
    """(functions per module, {symbol: (module, addr, size)})."""
    spans = collections.defaultdict(list)
    syms = {}
    for sf in REPO.rglob("config/**/symbols.txt"):
        parts = sf.parts
        mod = "arm9"
        if "overlays" in parts:
            mod = parts[parts.index("overlays") + 1]
        elif "itcm" in parts:
            mod = "itcm"
        elif "dtcm" in parts:
            mod = "dtcm"
        for line in sf.read_text(errors="replace").splitlines():
            m = re.match(r"\s*(\S+)\s+kind:function\(arm,size=(0x[0-9a-fA-F]+)\)"
                         r"\s+addr:(0x[0-9a-fA-F]+)", line)
            if m:
                addr, size = int(m.group(3), 16), int(m.group(2), 16)
                spans[mod].append((addr, addr + size))
                syms[m.group(1)] = (mod, addr, size)
    for mod in spans:
        spans[mod].sort()
    return spans, syms


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--verbose", action="store_true", help="list every hit address")
    ap.add_argument("--strict", action="store_true",
                    help="exit non-zero if anything is stored as data (see below: "
                         "a dispatch-table entry is legitimate, so this is off by "
                         "default and the tool is a report, not a gate)")
    args = ap.parse_args()

    listing = subprocess.run([sys.executable, str(REPO / "tools/langmode_audit.py"),
                              "--list", "layout-free"],
                             capture_output=True, text=True, cwd=REPO).stdout.split()
    stems = [pathlib.Path(x).stem for x in listing]
    if not stems:
        print("no layout-free candidates")
        return 0

    spans, syms = symbols()
    mods = modules()

    def inside_a_function(mod, addr):
        lst = spans.get(mod, [])
        i = bisect.bisect_right(lst, (addr, 1 << 62)) - 1
        return i >= 0 and lst[i][0] <= addr < lst[i][1]

    # group the candidates by module so each blob is scanned once
    by_mod = collections.defaultdict(dict)
    unknown = []
    for s in stems:
        if s not in syms:
            unknown.append(s)
            continue
        mod, addr, _ = syms[s]
        by_mod[mod][addr] = s

    hits = collections.defaultdict(lambda: {"pool": [], "data": []})
    for mod, wanted in by_mod.items():
        if mod not in mods:
            for s in wanted.values():
                hits[s]["missing"] = True
            continue
        blob, base = mods[mod]
        for off in range(0, len(blob) - 4, 4):
            w = struct.unpack_from("<I", blob, off)[0]
            s = wanted.get(w)
            if s is not None:
                at = base + off
                hits[s]["pool" if inside_a_function(mod, at) else "data"].append(at)

    missing = sorted(s for s in hits if hits[s].get("missing"))
    in_data = sorted(s for s in hits if hits[s]["data"])
    pool_only = sorted(s for s in hits if not hits[s]["data"] and hits[s]["pool"])
    clean = sorted(s for s in stems if s not in hits and s not in unknown)
    # THE BUCKETS MUST TOTAL. A candidate that matches none of them is a candidate
    # nothing reported, which is the failure this whole tool exists to prevent.
    total = len(clean) + len(pool_only) + len(in_data) + len(unknown) + len(missing)
    assert total == len(stems), (
        f"accounting lost {len(stems) - total} candidate(s): "
        f"{len(clean)} clean + {len(pool_only)} pool + {len(in_data)} data + "
        f"{len(unknown)} no-symbol + {len(missing)} no-module != {len(stems)}")

    print(f"layout-free candidates      {len(stems):5d}")
    print(f"  address never stored      {len(clean):5d}   consistent with layout-free")
    print(f"  in a literal pool only    {len(pool_only):5d}   harmless -- a pool is not a table")
    print(f"  STORED AS DATA            {len(in_data):5d}   <- vtable or dispatch table; look")
    if unknown:
        print(f"  no symbol found           {len(unknown):5d}")
    if missing:
        print(f"  MODULE NOT EXTRACTED      {len(missing):5d}   <- not checked, not clean")
        for s in missing[:8]:
            print(f"      {syms[s][0]}  {s}")

    if in_data:
        print("\nstored as data -- read the addresses before concluding. Consecutive")
        print("slots repeating per class is a vtable (disqualifying); a contiguous run")
        print("of single entries is a dispatch table, which a static function may")
        print("legitimately be listed in.\n")
        for s in in_data:
            where = ", ".join(hex(a) for a in hits[s]["data"][:4])
            more = "" if len(hits[s]["data"]) <= 4 else f" (+{len(hits[s]['data']) - 4})"
            print(f"  {s}\n      {where}{more}")

    if args.verbose and pool_only:
        print("\nliteral-pool references (informational):")
        for s in pool_only:
            print(f"  {s}  {', '.join(hex(a) for a in hits[s]['pool'][:4])}")

    # A REPORT, NOT A GATE, and deliberately so. Telling a vtable from a dispatch
    # table needs a human to read the addresses -- the sixteen LVL_Overlay loaders
    # are a legitimate dispatch table -- so exiting non-zero by default would cry
    # wolf on the one case the docstring spends a paragraph explaining. --strict is
    # there for anyone who wants to wire it up anyway.
    return 1 if (args.strict and in_data) else 0


if __name__ == "__main__":
    sys.exit(main())
