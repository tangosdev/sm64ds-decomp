#!/usr/bin/env python3
"""ov_resjoin -- join a residue list of unlinked matched TUs to the ACTOR
CLASS whose vtable references them, so a seat lane works from ids rather than
from anonymous data symbols.

Input is a TSV whose rows are `tu  src-path  module  addr  cat  referrers
flags` (run link100's residue.tsv shape). For every row whose referrer is a
same-module DATA symbol, the referrer is looked up in the table of vtables
this ROM's factories install (ov_seatscan's derivation: the last same-module
data load inside the factory's own delink span), and the row is attributed to
that actor id. Rows whose referrer is not any factory's vtable are reported
separately with the function that loads them, which is what distinguishes a
class vtable from a pointer-to-member state table.

    python port/tools/ov_resjoin.py <root> <residue.tsv>
"""
import argparse
import collections
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import ov_seatscan as S  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("residue")
    ap.add_argument("--modules", default="")
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    mods = S.build_modules(root)
    arm9 = mods["main"]
    reg = S.registered_ids(root)

    # vtable -> actor ids that install it
    vt2id = collections.defaultdict(list)
    id2 = {}
    for aid in range(S.N_ACTORS):
        si = arm9.word(S.TABLE + 4 * aid)
        if not si:
            continue
        m = S.module_of(mods, si)
        if m is None or m.name == "main":
            continue
        spawn = m.word(si)
        if not spawn:
            continue
        sm = S.module_of(mods, spawn)
        if sm is None or sm.name != m.name:
            continue
        vt, span = S.vtable_of(mods, sm, spawn)
        id2[aid] = (m.name, si, spawn, vt)
        if vt:
            vt2id[(m.name, vt)].append(aid)

    rows = []
    for ln in open(a.residue):
        if ln.startswith("#"):
            continue
        p = ln.rstrip("\n").split("\t")
        while len(p) < 7:
            p.append("")
        rows.append(p)

    byid = collections.defaultdict(list)
    orphan = collections.defaultdict(list)
    for tu, src, mod, addr, cat, refs, flags in rows:
        if mod not in mods:
            continue
        for r in refs.split(","):
            r = r.strip()
            if not r.startswith("data_" + mod + "_"):
                continue
            ra = int(r.split("_")[-1].split(":")[0], 16)
            k = (mod, ra)
            if k in vt2id:
                for aid in vt2id[k]:
                    byid[aid].append((tu, src, flags.strip()))
            else:
                orphan[k].append((tu, src, flags.strip()))

    print("=== residue attributed to an actor class vtable ===")
    print("id   mod    vtable      reg  rows clean  stub")
    for aid in sorted(byid, key=lambda i: -len(byid[i])):
        mn, si, spawn, vt = id2[aid]
        rr = byid[aid]
        clean = sum(1 for x in rr if not x[2])
        stub = sum(1 for x in rr if "STUB" in x[2])
        print("%-4d %-6s 0x%08x  %-3s  %-4d %-5d %d"
              % (aid, mn, vt, "R" if aid in reg else "-", len(rr), clean, stub))
    print()
    print("=== residue whose referrer is NOT any factory vtable ===")
    print("module  data        rows clean stub  loaded-by")
    for (mn, ra) in sorted(orphan, key=lambda k: -len(orphan[k])):
        m = mods[mn]
        rr = orphan[(mn, ra)]
        clean = sum(1 for x in rr if not x[2])
        stub = sum(1 for x in rr if "STUB" in x[2])
        ldr = []
        for frm in m.relocs:
            kind, to, module = m.relocs[frm]
            if to == ra and kind == "load" and m.is_code(frm):
                f = m.srcfile(frm)
                ldr.append(f or ("0x%08x" % frm))
        ldr = sorted(set(ldr))[:3]
        print("%-7s 0x%08x  %-4d %-5d %-4d %s"
              % (mn, ra, len(rr), clean, stub, " ".join(ldr)))


if __name__ == "__main__":
    main()
