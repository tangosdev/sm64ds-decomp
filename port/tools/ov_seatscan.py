#!/usr/bin/env python3
"""ov_seatscan -- which overlay actor classes the port has never seated, and
what seating each would buy in LINKED translation units.

Everything below is read off the ROM and the delink config, never off a name:

  extracted/arm9_dec.bin        the arm9 image, for ACTOR_SPAWN_TABLE
                                (0x02090864, 391 SpawnInfo*) -- the same table
                                tools/actor_names.py reads.
  extracted/overlays/*.bin      the RAW overlay images (dsd's arm9_overlays
                                exports are rebuilt and go stale; only
                                extracted/overlays is config-aligned).
  config/arm9/**/delinks.txt    section spans + address -> src file blocks.
  config/arm9/**/relocs.txt     the load relocations that name a vtable and
                                every slot's target.
  config/arm9/**/symbols.txt    exact-address names, and the kind: field the
                                overlay disambiguation below leans on.
  port/hal/actor_classes.inc    which ids the registry already carries.

OVERLAY DISAMBIGUATION, which is the whole difficulty.  Object overlays are
overlaid: ov010, ov016, ov029, ov043, ov064 ... all cover roughly
0x02111000..0x0211c000, so an address alone does not name a module.  A
SpawnInfo pointer is attributed to overlay N only when, reading N's own
image, the record's +0 word lands exactly on a `kind:function` symbol of that
same overlay, its two priority halfwords are in range, and -- the free
cross-check the registry itself uses -- the +4 halfword equals the actor id
the table row came from.  Ids that stay ambiguous are printed as such rather
than guessed.

THE VTABLE is the last same-module data address the factory loads whose word
at +4 is Actor::BeforeInitResources (0x02011268), the slot-1 invariant
port/tools/vtspan.py documents.  A factory loads several data addresses (its
SpawnInfo, tuning tables, model-name strings); only the vtable passes.

    python port/tools/ov_seatscan.py <root>              unregistered classes
    python port/tools/ov_seatscan.py <root> --all        registered too
    python port/tools/ov_seatscan.py <root> --id 105     one class, slot by slot
    python port/tools/ov_seatscan.py <root> --vtable ov064:0x0211b768
"""
import argparse
import pathlib
import re
import struct

TABLE = 0x02090864
N_ACTORS = 391
STUB_MARK = "recovered from vtable slot identity"
ACTOR_BINIT = 0x02011268    # Actor::BeforeInitResources, slot 1 of every table
ARM9_BASE = 0x02004000


def load_sections(p):
    out = []
    for ln in p.read_text().splitlines():
        m = re.match(r"\s+(\.\w+)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+) "
                     r"kind:(\w+)", ln)
        if m:
            out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16),
                        m.group(4)))
    return out


def load_delink_files(p):
    out = []
    cur = None
    for ln in p.read_text().splitlines():
        m = re.match(r"(\S+):\s*$", ln)
        if m and "/" in m.group(1):
            cur = m.group(1)
            continue
        m = re.match(r"\s+\.(\w+)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+)", ln)
        if m and cur:
            out.append((int(m.group(2), 16), int(m.group(3), 16), cur,
                        m.group(1)))
    out.sort()
    return out


def load_syms(p):
    by_addr = {}
    kind = {}
    for ln in p.read_text().splitlines():
        m = re.match(r"(\S+) kind:(\S+) addr:0x([0-9a-f]+)", ln)
        if m:
            a = int(m.group(3), 16)
            by_addr.setdefault(a, []).append(m.group(1))
            kind.setdefault(a, m.group(2).split("(")[0])
    return by_addr, kind


def load_relocs(p):
    out = {}
    for ln in p.read_text().splitlines():
        m = re.match(r"from:0x([0-9a-f]+) kind:(\S+) to:0x([0-9a-f]+) "
                     r"module:(\S+)", ln)
        if m:
            out[int(m.group(1), 16)] = (m.group(2), int(m.group(3), 16),
                                        m.group(4))
    return out


class Module(object):
    def __init__(self, name, cfgdir, image):
        self.name = name
        self.sections = load_sections(cfgdir / "delinks.txt")
        self.files = load_delink_files(cfgdir / "delinks.txt")
        self.syms, self.kind = load_syms(cfgdir / "symbols.txt")
        self.relocs = load_relocs(cfgdir / "relocs.txt")
        self.base = min(s[1] for s in self.sections)
        self.end = max(s[2] for s in self.sections)
        self.image = image.read_bytes() if image and image.exists() else b""
        self.code = [(s[1], s[2]) for s in self.sections if s[3] == "code"]

    def has(self, a):
        return self.base <= a < self.end

    def in_image(self, a):
        return 0 <= a - self.base < len(self.image)

    def is_code(self, a):
        return any(lo <= a < hi for lo, hi in self.code)

    def is_func(self, a):
        return self.kind.get(a) == "function"

    def word(self, a):
        off = a - self.base
        if off < 0 or off + 4 > len(self.image):
            return None
        return struct.unpack_from("<I", self.image, off)[0]

    def half(self, a):
        off = a - self.base
        if off < 0 or off + 2 > len(self.image):
            return None
        return struct.unpack_from("<H", self.image, off)[0]

    def srcfile(self, a):
        for lo, hi, f, sec in self.files:
            if lo <= a < hi:
                return f
        return None

    def fspan(self, a):
        for lo, hi, f, sec in self.files:
            if lo <= a < hi:
                return lo, hi
        return None

    def name_at(self, a):
        n = self.syms.get(a)
        return n[0] if n else None


def build_modules(root):
    mods = {}
    cfg = root / "config/arm9"
    mods["main"] = Module("main", cfg, root / "extracted/arm9_dec.bin")
    for d in sorted((cfg / "overlays").iterdir()):
        if not d.is_dir():
            continue
        ovid = int(d.name[2:])
        img = root / ("extracted/overlays/overlay_%04d.bin" % ovid)
        mods[d.name] = Module(d.name, d, img)
    return mods


def registered_ids(root):
    txt = (root / "port/hal/actor_classes.inc").read_text()
    body = txt.split("#define PORT_ACTOR_CLASS_ROWS", 1)
    if len(body) < 2:
        return set()
    ids = set()
    for m in re.finditer(r"\{\s*(0x[0-9a-fA-F]+|\d+)\s*,\s*\"", body[1]):
        ids.add(int(m.group(1), 0))
    return ids


def slice_srcs(root):
    out = set()
    for f in sorted((root / "port").glob("slice_*.txt")):
        for ln in f.read_text().splitlines():
            ln = ln.strip()
            if ln and not ln.startswith("#"):
                out.add(ln)
    return out


def is_vtable(mod, v):
    r = mod.relocs.get(v + 4)
    return r is not None and r[0] == "load" and r[1] == ACTOR_BINIT


def vtable_of(mod, spawn):
    """The class table the factory installs. Same-module data loads inside the
    factory's own delink span, filtered by the slot-1 invariant."""
    span = mod.fspan(spawn) or (spawn, spawn + 0x200)
    cands = []
    for frm in mod.relocs:
        kind, to, module = mod.relocs[frm]
        if kind != "load" or not (span[0] <= frm < span[1]):
            continue
        if mod.has(to) and not mod.is_code(to):
            cands.append(to)
    real = [v for v in sorted(set(cands)) if is_vtable(mod, v)]
    return (real[-1] if real else None), span


def resolve_actor(mods, aid, si):
    """Which overlay owns this SpawnInfo. Returns list of (module, spawnfn)."""
    hits = []
    for k in mods:
        if k == "main":
            continue
        m = mods[k]
        if not m.has(si) or not m.in_image(si):
            continue
        fn = m.word(si)
        if not fn or fn & 3:
            continue
        if not (m.is_func(fn) or mods["main"].is_func(fn)):
            continue
        rec = m.half(si + 4)
        if rec != aid:
            continue
        hits.append((m, fn))
    return hits


def scan(root, mods):
    """actor id -> (module, spawninfo, spawnfn, vtable) for overlay classes."""
    arm9 = mods["main"]
    out = {}
    amb = {}
    for aid in range(N_ACTORS):
        si = arm9.word(TABLE + 4 * aid)
        if not si:
            continue
        if arm9.has(si):
            continue                      # arm9-resident class, not ours
        hits = resolve_actor(mods, aid, si)
        if len(hits) != 1:
            amb[aid] = [(m.name, fn) for m, fn in hits]
            continue
        m, fn = hits[0]
        vt = vtable_of(m, fn)[0] if m.has(fn) else None
        out[aid] = (m.name, si, fn, vt)
    return out, amb


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--id", type=lambda s: int(s, 0), default=None)
    ap.add_argument("--vtable", default=None, help="ovNNN:0xADDR")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--ambiguous", action="store_true")
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    mods = build_modules(root)
    reg = registered_ids(root)
    sl = slice_srcs(root)
    stubcache = {}

    def is_stub(f):
        if f not in stubcache:
            p = root / f
            stubcache[f] = (p.exists()
                            and STUB_MARK in p.read_text(errors="replace"))
        return stubcache[f]

    if a.vtable:
        mn, ad = a.vtable.split(":")
        dump_vtable(root, mods, mods[mn], int(ad, 0), sl, is_stub)
        return

    tbl, amb = scan(root, mods)
    if a.ambiguous:
        for aid in sorted(amb):
            print(aid, amb[aid])
        return

    print("id   mod    SpawnInfo   Spawn       vtable      reg own slice stub")
    for aid in sorted(tbl):
        mn, si, fn, vt = tbl[aid]
        if a.id is not None and aid != a.id:
            continue
        if a.id is None and not a.all and aid in reg:
            continue
        m = mods[mn]
        own = insl = stub = 0
        rows = []
        if vt:
            for i in range(48):
                r = m.relocs.get(vt + 4 * i)
                if r is None or r[0] != "load":
                    break
                tgt = r[1]
                if not m.has(tgt):
                    rows.append((i, tgt, None, "", False))
                    continue
                own += 1
                f = m.srcfile(tgt)
                st = "STUB" if (f and is_stub(f)) else ""
                if st:
                    stub += 1
                lk = bool(f) and f in sl
                if lk:
                    insl += 1
                rows.append((i, tgt, f, st, lk))
        print("%-4d %-6s 0x%08x  0x%08x  %s %-3s %-3d %-5d %d"
              % (aid, mn, si, fn, ("0x%08x" % vt) if vt else "    ?     ",
                 "R" if aid in reg else "-", own, insl, stub))
        if a.id is not None:
            for i, tgt, f, st, lk in rows:
                print("   slot %-2d 0x%08x %-6s %-4s %-46s %s"
                      % (i, tgt, "SLICE" if lk else "", st, f or "-",
                         m.name_at(tgt) or mods["main"].name_at(tgt) or ""))


def dump_vtable(root, mods, m, vt, sl, is_stub):
    print("vtable %s 0x%08x  %s" % (m.name, vt, m.name_at(vt) or ""))
    for i in range(48):
        r = m.relocs.get(vt + 4 * i)
        if r is None or r[0] != "load":
            break
        tgt = r[1]
        own = m.has(tgt)
        f = m.srcfile(tgt) if own else None
        st = "STUB" if (f and is_stub(f)) else ""
        print("   slot %-2d 0x%08x %-4s %-6s %-46s %s"
              % (i, tgt, "own" if own else "", st,
                 (f or "-") if own else "(arm9)",
                 m.name_at(tgt) or mods["main"].name_at(tgt) or ""))


if __name__ == "__main__":
    main()
