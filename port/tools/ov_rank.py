#!/usr/bin/env python3
"""ov_rank -- rank the overlay actor classes the port has never seated by what
seating each would actually buy, measured as a CLOSURE over the run's residue
graph rather than as a vtable slot count.

The residue file (run link100's residue.tsv shape) lists every matched TU that
compiles into the port and does NOT reach walk_window.exe, with the symbols
that refer to it and whether each referrer is itself linked (L), unlinked (U)
or source-less (X).  A class's gain is not its own slot bodies: seating the
class references those bodies, they reference their callees, and the callees
theirs, so the number is the transitive closure over the U edges.

Class identity is by ADDRESS throughout.  The vtable is resolved three ways,
in order, and the route used is printed:

  inline    the last same-module data load inside the factory's own delink
            span whose word at +4 is Actor::BeforeInitResources -- the slot-1
            invariant port/tools/vtspan.py documents.
  ctor      the same test over the spans of the functions the factory CALLS
            (one level), for the factories that allocate and delegate.
  loaders   module-wide: every address passing the slot-1 test, attributed to
            the id whose factory or whose factory's callees load it.

    python port/tools/ov_rank.py <root> <residue.tsv>
    python port/tools/ov_rank.py <root> <residue.tsv> --id 348
"""
import argparse
import collections
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import ov_seatscan as S  # noqa: E402


def callees(mod, span):
    out = []
    for frm in mod.relocs:
        kind, to, module = mod.relocs[frm]
        if kind.endswith("call") and span[0] <= frm < span[1] and mod.has(to):
            out.append(to)
    return sorted(set(out))


def resolve_vtable(mod, spawn):
    vt, span = S.vtable_of(mod, spawn)
    if vt:
        return vt, "inline"
    for c in callees(mod, span):
        cs = mod.fspan(c)
        if not cs:
            continue
        v2, _ = S.vtable_of(mod, c)
        if v2:
            return v2, "ctor"
    return None, "-"


def load_residue(path):
    rows = {}
    for ln in open(path):
        if ln.startswith("#"):
            continue
        p = ln.rstrip("\n").split("\t")
        while len(p) < 7:
            p.append("")
        rows[p[0]] = dict(tu=p[0], src=p[1], mod=p[2], addr=p[3], cat=p[4],
                          refs=p[5], flags=p[6].strip())
    return rows


def build_graph(rows):
    """referrer symbol -> unlinked TUs that name it."""
    children = collections.defaultdict(set)
    for tu, r in rows.items():
        for x in r["refs"].split(","):
            x = x.strip()
            if not x:
                continue
            sym = x.rsplit(":", 1)[0]
            children[sym].add(tu)
    return children


def closure(children, seeds):
    seen = set()
    st = list(seeds)
    while st:
        x = st.pop()
        if x in seen:
            continue
        seen.add(x)
        st.extend(children.get(x, ()))
    return seen


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("residue")
    ap.add_argument("--id", type=lambda s: int(s, 0), default=None)
    ap.add_argument("--all", action="store_true")
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    mods = S.build_modules(root)
    reg = S.registered_ids(root)
    reg.add(191)          # PLAYER, its row is outside the macro
    reg.add(0x14c)        # CAMERA, likewise
    sl = S.slice_srcs(root)
    rows = load_residue(a.residue)
    children = build_graph(rows)
    tbl, amb = S.scan(root, mods)
    stubcache = {}

    def is_stub(f):
        if f not in stubcache:
            p = root / f
            stubcache[f] = (p.exists()
                            and S.STUB_MARK in p.read_text(errors="replace"))
        return stubcache[f]

    out = []
    for aid in sorted(tbl):
        if aid in reg and not a.all:
            continue
        if a.id is not None and aid != a.id:
            continue
        mn, si, fn, _ = tbl[aid]
        m = mods[mn]
        vt, route = resolve_vtable(m, fn)
        seeds, slotrows = [], []
        width = 0
        if vt:
            for i in range(48):
                r = m.relocs.get(vt + 4 * i)
                if r is None or r[0] != "load":
                    break
                width = i + 1
                tgt = r[1]
                if not m.has(tgt):
                    slotrows.append((i, tgt, "(arm9)", "", False))
                    continue
                f = m.srcfile(tgt)
                st = "STUB" if (f and is_stub(f)) else ""
                nm = (m.name_at(tgt) or "")
                stem = f.split("/")[-1].rsplit(".", 1)[0] if f else None
                if stem and stem in rows and not st:
                    seeds.append(stem)
                slotrows.append((i, tgt, f or "-", st, stem in rows if stem else False))
        # the factory itself, and the vtable data symbol's own name
        for extra in (m.srcfile(fn),):
            if extra:
                stem = extra.split("/")[-1].rsplit(".", 1)[0]
                if stem in rows:
                    seeds.append(stem)
        cl = closure(children, seeds)
        nstub = sum(1 for t in cl if "STUB" in rows[t]["flags"])
        nnm = sum(1 for t in cl if "NONMATCHING" in rows[t]["flags"])
        out.append((len(cl) - nstub - nnm, aid, mn, vt, route, width,
                    len(cl), nstub, nnm, slotrows, sorted(cl)))

    out.sort(reverse=True)
    print("gain id   mod    vtable      route  w   clos stub nm")
    for gain, aid, mn, vt, route, width, ncl, nstub, nnm, slotrows, cl in out:
        print("%-4d %-4d %-6s %s %-6s %-3d %-4d %-4d %d"
              % (gain, aid, mn, ("0x%08x" % vt) if vt else "     ?    ",
                 route, width, ncl, nstub, nnm))
        if a.id is not None:
            m = mods[mn]
            for i, tgt, f, st, inres in slotrows:
                print("   slot %-2d 0x%08x %-4s %-9s %s"
                      % (i, tgt, st, "RESIDUE" if inres else "", f))
            print("   closure:")
            for t in cl:
                print("      %-38s %-8s %s"
                      % (t, rows[t]["flags"], rows[t]["src"]))
    print("sum of gains (overlapping):", sum(o[0] for o in out))


if __name__ == "__main__":
    main()
