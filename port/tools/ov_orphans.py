#!/usr/bin/env python3
"""ov_orphans -- every Actor-derived vtable in the overlays that NO actor id's
factory installs, and whether the port hosts it at all.

An overlay carries more class tables than the spawn table has ids: shared
bases a derived ctor overwrites, helper objects a registered actor builds,
and classes reached only from another class's code.  A table like that has
the same slot-1 signature as any Actor vtable (Actor::BeforeInitResources,
0x02011268) and the same consequence when it is left unfilled in the port --
a dispatch through a null host word -- but it never shows up in a spawn-table
sweep, so a seat lane working from ids alone cannot see it.

For each such table: which functions load it (that is the class's own ctor /
destructors / factory, and it is how the table is named), how many of its
slots are the class's own bodies, how many of those bodies have a matched src
TU, how many are guessed stubs, and whether any file under port/ mentions the
table by either of its config names.

    python port/tools/ov_orphans.py <root>
    python port/tools/ov_orphans.py <root> --module ov064
"""
import argparse
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import ov_seatscan as S  # noqa: E402
import ov_rank as R  # noqa: E402


def port_mentions(root, names):
    hay = {}
    for p in sorted((root / "port").rglob("*")):
        if p.is_dir() or p.suffix.lower() not in (".cpp", ".c", ".h", ".txt",
                                                  ".inc"):
            continue
        try:
            hay[p] = p.read_text(errors="replace")
        except Exception:
            pass

    def find(n):
        return [str(p) for p in hay if n in hay[p]]
    return find


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--module", default=None)
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    mods = S.build_modules(root)
    tbl, _ = S.scan(root, mods)
    sl = S.slice_srcs(root)
    find = port_mentions(root, None)
    stub = {}

    def is_stub(f):
        if f not in stub:
            p = root / f
            stub[f] = (p.exists()
                       and S.STUB_MARK in p.read_text(errors="replace"))
        return stub[f]

    owned = set()
    for aid in tbl:
        mn, si, fn, _v = tbl[aid]
        v, route = R.resolve_vtable(mods[mn], fn)
        if v:
            owned.add((mn, v))

    print("module  vtable      own src  slice stub  hosted  loaders")
    for mn in sorted(mods):
        if mn == "main" or (a.module and mn != a.module):
            continue
        m = mods[mn]
        # every same-module data address whose +4 word is Actor's slot 1
        for frm in sorted(m.relocs):
            kind, to, module = m.relocs[frm]
            if kind != "load" or to != S.ACTOR_BINIT:
                continue
            v = frm - 4
            if m.is_code(v) or (mn, v) in owned:
                continue
            own = src = insl = nstub = 0
            # slot 0 can be a NULL word (an abstract InitResources) and holes
            # are legal, so the walk is bounded by Actor's own 31 rather than
            # stopped by the first missing reloc.
            for i in range(31):
                r = m.relocs.get(v + 4 * i)
                if r is None or r[0] != "load":
                    continue
                t = r[1]
                if not m.has(t):
                    continue
                own += 1
                f = m.srcfile(t)
                if f and (root / f).exists():
                    src += 1
                    if is_stub(f):
                        nstub += 1
                    elif f in sl:
                        insl += 1
            names = [n for n in (m.name_at(v) or "",) if n]
            names.append("data_%s_%08x" % (mn, v))
            hosted = []
            for n in set(names):
                hosted += find(n)
            ldr = []
            for f2 in m.relocs:
                k2, t2, _m2 = m.relocs[f2]
                if t2 == v and k2 == "load" and m.is_code(f2):
                    s = m.srcfile(f2)
                    ldr.append(s.split("/")[-1] if s else "0x%08x" % f2)
            print("%-7s 0x%08x %-3d %-4d %-5d %-5d %-7s %s"
                  % (mn, v, own, src, insl, nstub,
                     "yes" if hosted else "NO", " ".join(sorted(set(ldr))[:4])))


if __name__ == "__main__":
    main()
