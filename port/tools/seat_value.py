#!/usr/bin/env python3
"""seat_value -- what seating an actor class would actually buy, measured
against the ROM's own reference graph and the CURRENT walk_window.map.

ov_rank.py answers the same question against a run's residue.tsv snapshot.
This one does not need a snapshot: "unlinked" is read straight out of the map
(a matched TU whose object never reached the image), and the reference graph
is config/**/relocs.txt -- the cartridge's own edges, so a file only counts
when the ROM reaches it, never because the port calls it from somewhere the
ROM does not.

    roots      the class's factory, plus every vtable slot target that lives
               in the class's OWN module (an arm9 target is an inherited base
               body and belongs to whoever already links it).
    closure    from each root's delink span, follow every call/load reloc into
               a code address, add that address's src file, repeat.  Only
               files with a matched TU in src/ that the map does NOT carry
               are counted -- a file another chain already links is worth 0.

    python port/tools/seat_value.py <root> --id 223
    python port/tools/seat_value.py <root> --unregistered
    python port/tools/seat_value.py <root> --all            every table row
"""
import argparse
import collections
import os
import pathlib
import re
import struct
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import ov_seatscan as S  # noqa: E402

STUB_MARK = S.STUB_MARK


def map_stems(mapfile):
    """object stems present in the MSVC /MAP (linkage.py's own reading)."""
    stems = set()
    pat = re.compile(
        r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+[0-9a-fA-F]{8}"
        r"\s+(?:[fi]\s+)*(\S+\.obj)\s*$")
    with open(mapfile, errors="replace") as f:
        for line in f:
            m = pat.match(line)
            if m:
                obj = m.group(2)
                stem = os.path.splitext(os.path.splitext(
                    os.path.basename(obj))[0])[0]
                stems.add(stem)
    return stems


def matched_stems(root):
    out = {}
    for dirpath, _d, files in os.walk(os.path.join(str(root), "src")):
        for fn in files:
            if fn.endswith((".c", ".cpp")):
                stem = os.path.splitext(fn)[0]
                rel = os.path.relpath(os.path.join(dirpath, fn), str(root))
                out.setdefault(stem, rel.replace("\\", "/"))
    return out


def stem_of(srcfile):
    return os.path.splitext(os.path.basename(srcfile))[0]


class World(object):
    def __init__(self, root):
        self.root = pathlib.Path(root).resolve()
        self.mods = S.build_modules(self.root)
        self.linked = map_stems(str(self.root / "build/port/walk_window.map"))
        self.matched = matched_stems(self.root)
        self.stubcache = {}
        self.rulings = self.load_rulings()

    def load_rulings(self):
        """port/tools/inferred_stub_adjudicated.txt -- stem -> ruling line."""
        p = self.root / "port/tools/inferred_stub_adjudicated.txt"
        out = {}
        if not p.exists():
            return out
        for ln in p.read_text(errors="replace").splitlines():
            m = re.search(r"(func_\w+|_Z\w+)", ln)
            if m:
                out.setdefault(m.group(1), ln.strip())
        return out

    def is_stub(self, f):
        if f not in self.stubcache:
            p = self.root / f
            self.stubcache[f] = (p.exists() and STUB_MARK
                                 in p.read_text(errors="replace"))
        return self.stubcache[f]

    def is_linked(self, f):
        return stem_of(f) in self.linked

    def has_src(self, f):
        return stem_of(f) in self.matched

    def closure(self, mod, roots, budget=4000):
        """unlinked matched src files the ROM reaches from these addresses."""
        seen_span = set()
        files = set()
        work = list(roots)
        steps = 0
        while work and steps < budget:
            a = work.pop()
            steps += 1
            m = mod if mod.has(a) else self.mods["main"]
            if not m.has(a):
                continue
            span = m.fspan(a)
            if not span or span in seen_span:
                continue
            seen_span.add(span)
            f = m.srcfile(a)
            if f and self.has_src(f) and not self.is_linked(f):
                files.add(f)
            elif f and self.is_linked(f):
                continue      # an already-linked TU's callees are already in
            for frm, (kind, to, module) in m.relocs.items():
                if not (span[0] <= frm < span[1]):
                    continue
                if kind.endswith("call") or kind == "load":
                    if m.has(to) and m.is_code(to):
                        work.append(to)
                    elif self.mods["main"].has(to) \
                            and self.mods["main"].is_code(to):
                        work.append(to)
        return files


def vtable_span(mod, vt):
    """slot count from the reloc run AND from the next named symbol, so a
    table that runs into the next data object is reported as both."""
    n = 0
    while mod.relocs.get(vt + 4 * n) and mod.relocs[vt + 4 * n][0] == "load":
        n += 1
        if n > 64:
            break
    nxt = None
    for a in sorted(mod.syms):
        if a > vt:
            nxt = a
            break
    by_sym = ((nxt - vt) // 4) if nxt else None
    return n, by_sym, nxt


def rtti_name(mod, vt):
    """the Itanium typeinfo the ROM stores at V-4, and its name string."""
    r = mod.relocs.get(vt - 4)
    if not r or r[0] != "load":
        return None, None
    ti = r[1]
    nm = mod.relocs.get(ti)          # typeinfo +0 is the name pointer
    if not nm or not mod.has(nm[1]):
        return ti, None
    off = nm[1] - mod.base
    end = mod.image.find(b"\0", off)
    return ti, mod.image[off:end].decode("ascii", "replace")


def report(w, aid, mn, si, fn, vt, reg, verbose):
    mod = w.mods[mn]
    n_reloc, n_sym, nxt = (vtable_span(mod, vt) if vt else (0, None, None))
    ti, tn = rtti_name(mod, vt) if vt else (None, None)
    own_files, own_unlinked, stubs, ruled = set(), set(), set(), set()
    rows = []
    if vt:
        for i in range(n_reloc):
            r = mod.relocs.get(vt + 4 * i)
            tgt = r[1]
            if not mod.has(tgt) or not mod.is_code(tgt):
                rows.append((i, tgt, "base" if not mod.has(tgt) else "data",
                             None, "", ""))
                continue
            f = mod.srcfile(tgt)
            own_files.add(f)
            st = "STUB" if (f and w.is_stub(f)) else ""
            if st:
                stubs.add(f)
                if stem_of(f) in w.rulings:
                    ruled.add(f)
            lk = "LINKED" if (f and w.is_linked(f)) else (
                "unlinked" if (f and w.has_src(f)) else "NOSRC")
            if lk == "unlinked":
                own_unlinked.add(f)
            rows.append((i, tgt, "own", f, st, lk))
    roots = [fn] + [r[1] for i, r in
                    ((i, mod.relocs.get(vt + 4 * i)) for i in range(n_reloc))
                    if r and mod.has(r[1]) and mod.is_code(r[1])] if vt else [fn]
    gain = w.closure(mod, roots)
    print("id %-4d %-6s SpawnInfo 0x%08x  factory 0x%08x%s  vtable %s"
          % (aid, mn, si, fn,
             " [factory %s]" % ("LINKED" if mod.srcfile(fn)
                                and w.is_linked(mod.srcfile(fn))
                                else "unlinked/nosrc"),
             ("0x%08x" % vt) if vt else "unresolved"))
    if vt:
        print("   slots: %d by reloc run, %s by next symbol (%s); RTTI %s %s"
              % (n_reloc,
                 n_sym if n_sym is not None else "-",
                 ("0x%08x" % nxt) if nxt else "-",
                 ("0x%08x" % ti) if ti else "-", tn or ""))
    print("   own bodies %d, unlinked %d, guess-marked %d (ruled %d)"
          % (len(own_files), len(own_unlinked), len(stubs), len(ruled)))
    print("   FILES UNLOCKED (ROM closure over unlinked matched TUs): %d"
          % len(gain))
    if verbose:
        for i, tgt, k, f, st, lk in rows:
            print("     slot %-2d 0x%08x %-5s %-8s %-9s %s"
                  % (i, tgt, k, st, lk, f or ""))
        for f in sorted(gain):
            print("     + %s" % f)
    return len(gain), len(own_unlinked), len(stubs), len(ruled), tn


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--id", type=lambda s: int(s, 0), default=None)
    ap.add_argument("--unregistered", action="store_true")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--tsv", action="store_true")
    a = ap.parse_args()
    w = World(a.root)
    reg = S.registered_ids(w.root)
    reg |= {191, 332}
    tbl, amb = S.scan(w.root, w.mods)

    ids = sorted(tbl)
    if a.id is not None:
        ids = [a.id]
    elif a.unregistered:
        ids = [i for i in ids if i not in reg]

    if a.tsv:
        print("id\tmodule\tvtable\tfiles_unlocked\town_unlinked\tguess\truled\trtti")
    for aid in ids:
        if aid not in tbl:
            print("id %d is not an overlay spawn-table row" % aid)
            continue
        mn, si, fn, vt = tbl[aid]
        if a.tsv:
            mod = w.mods[mn]
            n_reloc = vtable_span(mod, vt)[0] if vt else 0
            roots = [fn] + ([r[1] for i, r in
                             ((i, mod.relocs.get(vt + 4 * i))
                              for i in range(n_reloc))
                             if r and mod.has(r[1]) and mod.is_code(r[1])]
                            if vt else [])
            gain = w.closure(mod, roots)
            ti, tn = rtti_name(mod, vt) if vt else (None, None)
            print("%d\t%s\t%s\t%d\t-\t-\t-\t%s"
                  % (aid, mn, ("0x%08x" % vt) if vt else "-", len(gain),
                     tn or ""))
        else:
            report(w, aid, mn, si, fn, vt, aid in reg,
                   a.id is not None)


if __name__ == "__main__":
    main()
