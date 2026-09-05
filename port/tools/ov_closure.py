#!/usr/bin/env python3
"""ov_closure -- what ELSE a seat drags into the link, off the ROM's own
reference graph rather than off a link.

Given a slice file (or a list of src/ paths), walk the ROM call/load
relocations out of each TU's delink span and report every reachable symbol
that (a) has a matched src TU in this tree and (b) is not already listed by
any port/slice_*.txt.  Those are the TUs a seat will PULL IN once the linker
follows the same edges -- the difference between "my class has seven bodies"
and "my class costs the link forty".

A ROM edge is followed only when the target resolves inside a module the
source TU can actually reach: same overlay, or arm9.  Cross-overlay edges are
reported separately, because object overlays share one load window and an
address alone does not name a module (the ov039/ov041/ov044 spellings in
Cloud::InitResources are exactly that hazard).

    python port/tools/ov_closure.py <root> port/slice_gate217.txt
    python port/tools/ov_closure.py <root> --tu src/Cloud_Spawn.c
"""
import argparse
import collections
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import ov_seatscan as S  # noqa: E402


def spans_by_file(mods):
    """src path -> list of (module, lo, hi)."""
    out = collections.defaultdict(list)
    for mn in mods:
        m = mods[mn]
        for lo, hi, f, sec in m.files:
            if sec == "text":
                out[f].append((mn, lo, hi))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("slice", nargs="?")
    ap.add_argument("--tu", action="append", default=[])
    ap.add_argument("--depth", type=int, default=6)
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    mods = S.build_modules(root)
    sl = S.slice_srcs(root)
    spans = spans_by_file(mods)

    seeds = list(a.tu)
    if a.slice:
        for ln in (root / a.slice).read_text().splitlines():
            ln = ln.strip()
            if ln and not ln.startswith("#"):
                seeds.append(ln)
    if not seeds:
        sys.exit("no seeds")

    seen = set(seeds)
    frontier = list(seeds)
    pulled, cross, nosrc = {}, {}, set()
    for _ in range(a.depth):
        nxt = []
        for f in frontier:
            for mn, lo, hi in spans.get(f, []):
                m = mods[mn]
                for addr in range(lo, hi, 4):
                    r = m.relocs.get(addr)
                    if r is None:
                        continue
                    kind, tgt, tmod = r
                    home = m if m.has(tgt) else mods["main"]
                    if not home.has(tgt):
                        continue
                    if home is mods["main"] and not mods["main"].has(tgt):
                        continue
                    tf = home.srcfile(tgt)
                    if not tf:
                        nosrc.add("%s 0x%08x" % (home.name, tgt))
                        continue
                    if not (root / tf).exists():
                        nosrc.add(tf)
                        continue
                    if tf in seen:
                        continue
                    seen.add(tf)
                    if tf in sl:
                        continue          # already compiled into the image
                    # a same-window overlay target reached from another
                    # overlay's TU is only a guess at which module owns it
                    if m.name != "main" and home.name != "main" \
                            and home.name != m.name:
                        cross[tf] = (m.name, home.name, tgt)
                        continue
                    pulled[tf] = (m.name, home.name, tgt, f)
                    nxt.append(tf)
        frontier = nxt
        if not frontier:
            break

    print("seeds: %d" % len(seeds))
    print("NOT IN ANY SLICE, reachable from the seeds by ROM edges: %d"
          % len(pulled))
    for f in sorted(pulled):
        mn, hm, tgt, src = pulled[f]
        print("   %-52s 0x%08x %-6s <- %s" % (f, tgt, hm, src.split('/')[-1]))
    if cross:
        print("CROSS-WINDOW targets (module ambiguous, not followed): %d"
              % len(cross))
        for f in sorted(cross):
            print("   %s" % f)


if __name__ == "__main__":
    main()
