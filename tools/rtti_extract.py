#!/usr/bin/env python3
"""Pass 3 of the hierarchy programme: read the class graph out of the ROM's RTTI.

`evidence_hierarchy.py` (pass 2) reconstructs inheritance from ctor/dtor vptr chains
and ranks the result by source authority, because it is deliberately source+config
only and had no ground truth.  There is ground truth.  Most of this game was compiled
with RTTI on, so the ROM stores each polymorphic class's original name and a pointer
to its base's record.  Following those pointers is not inference; this pass emits
edges, not evidence.

Output: build/rtti.json -- every record, its name, its vtable, its base edges.

How a record is read
--------------------
    vtable V     ->  typeinfo ptr at V-4
    typeinfo T   ->  [0] type_info vtable   (which kind of record this is)
                     [4] name pointer       (length-prefixed, Itanium style)
                     [8] base / base count  (kind-dependent)

    0x0209a774  __class_type_info      8 bytes, no base -- a root
    0x0209a764  __si_class_type_info   12 bytes, one base at [8]
    0x0209a754  __vmi_class_type_info  [8]=flags [12]=count then {base, off<<8|flags}

Those three addresses are in arm9 .data, which is always resident, so a record in any
overlay points at the same three.

Four traps, each of which produced a wrong answer during development
--------------------------------------------------------------------
scan .text        `type_info`'s own constructors carry the three vtable addresses as
                  pool literals.  Scanning the whole image yields 7 arm9 hits that are
                  code, not records.  Sections come from delinks.txt; only rodata/data
                  are scanned.

name filter       An early draft filtered names by first character (`\\d`, `N`, `K`) and
                  so dropped `St9type_info` -- the `St` substitution for `std::`.  That
                  orphaned the one edge pointing at it, which then looked like "the ABI
                  root is unresolvable, expected".  It was not expected; it was the
                  filter.  Accept every shape and let the parser reject.

bare addresses    Overlays share address space.  Two collisions exist in this ROM:
                  0x02114868 is daObjWanwanShutter_c (ov014) *and* daObjBk_Fall_Block_c
                  (ov015); 0x021123fc is daObjTtWater_c (ov033) *and* daObjKm3_Kuruma_c
                  (ov047).  Record identity is (module, addr).  Never the addr alone.

cross-overlay     183 of 413 edges leave their overlay -- every ov006 dScMg*_c scene
edges             derives from dScMgBase_c in ov004, and so on.  A resolver that only
                  looks in the owning overlay and then arm9 cannot resolve them.  The
                  order is: own module, then arm9, then whatever residency leaves
                  (tools/overlay_residency.py), then give up and say so.

Recall
------
Section 2 of notes/plan-gen-header.md: a pass that silently matches nothing reports a
clean, confident, wrong answer.  Every population is counted, including rejects and
why.  `--report` prints all of it before any finding.

Pure stdlib apart from the yaml the repo already vendors for overlays.yaml.
Deterministic: every collection is sorted before it is emitted.
"""

from __future__ import annotations

import argparse
import json
import os
import pathlib
import re
import struct
import sys
from collections import Counter, defaultdict

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

TI_KIND = {
    0x0209A774: "class",   # __class_type_info    -- root, no base
    0x0209A764: "si",      # __si_class_type_info -- exactly one base
    0x0209A754: "vmi",     # __vmi_class_type_info -- multiple/virtual bases
}

ARM9_BASE = 0x02004000
ARM9_BIN = REPO / "extracted" / "arm9_dec.bin"
OVERLAY_DIR = REPO / "extracted" / "dsd" / "arm9_overlays"
OVERLAY_YAML = OVERLAY_DIR / "overlays.yaml"

# delinks.txt section table: leading indented lines before the first `path:` line
SECTION_RE = re.compile(
    r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\s+kind:(\S+)")
SYM_RE = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-fA-F]+)")

# Sections a record can legally live in.  bss has no file bytes; code is the trap.
DATA_KINDS = {"rodata", "data"}

MAX_NAME = 96          # longest legitimate mangled class name seen is 34
MAX_VMI_BASES = 16     # a count above this is a misread, not a class


# --------------------------------------------------------------------------
# name parsing
# --------------------------------------------------------------------------

_LEN = re.compile(r"(\d+)")


def _split_len(s):
    """`10ChainChomp` -> ['ChainChomp'];  `5dPa_c7level_c` -> both parts."""
    parts, i = [], 0
    while i < len(s):
        m = _LEN.match(s, i)
        if not m:
            return None
        n = int(m.group(1))
        i = m.end()
        if n == 0 or i + n > len(s):
            return None
        parts.append(s[i:i + n])
        i += n
    return parts or None


def demangle(raw):
    """Itanium type_info name -> qualified C++ name, or None if it is not one.

    Three shapes occur in this ROM:
        8dFader_c                  plain
        N8dGraph_c10callback_cE    nested
        St9type_info               `St` == the std:: substitution
    """
    if not raw or len(raw) > MAX_NAME or not raw.isprintable():
        return None
    if raw.startswith("N") and raw.endswith("E"):
        parts = _split_len(raw[1:-1])
    elif raw.startswith("St"):
        parts = _split_len(raw[2:])
        if parts:
            parts = ["std"] + parts
    else:
        parts = _split_len(raw)
    if not parts:
        return None
    if not all(re.fullmatch(r"[A-Za-z_]\w*", p) for p in parts):
        return None
    return "::".join(parts)


# --------------------------------------------------------------------------
# modules
# --------------------------------------------------------------------------

class Module:
    """One loadable image: its bytes, where it loads, and its data sections."""

    __slots__ = ("name", "base", "data", "sections", "cfgdir")

    def __init__(self, name, base, data, sections, cfgdir):
        self.name = name
        self.base = base
        self.data = data
        self.sections = sections      # [(start, end, kind)] absolute VAs
        self.cfgdir = cfgdir

    def contains(self, va):
        return self.base <= va < self.base + len(self.data)

    def in_data_section(self, va):
        for start, end, kind in self.sections:
            if start <= va < end:
                return kind in DATA_KINDS
        return False

    def word(self, va):
        return struct.unpack_from("<I", self.data, va - self.base)[0]

    def cstr(self, va):
        o = va - self.base
        if not (0 <= o < len(self.data)):
            return None
        e = self.data.find(b"\0", o)
        if e < 0 or e - o > MAX_NAME:
            return None
        return self.data[o:e].decode("ascii", "replace")


def load_sections(delinks_path):
    out = []
    if not delinks_path.is_file():
        return out
    for line in delinks_path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = SECTION_RE.match(line)
        if m:
            out.append((int(m.group(2), 16), int(m.group(3), 16), m.group(4)))
        elif line and not line[0].isspace():
            break          # first file entry; the section table is over
    return out


def load_modules(stats):
    mods = []
    arm9_cfg = REPO / "config" / "arm9"
    mods.append(Module("arm9", ARM9_BASE, ARM9_BIN.read_bytes(),
                       load_sections(arm9_cfg / "delinks.txt"), arm9_cfg))
    import yaml
    cfg = yaml.safe_load(OVERLAY_YAML.read_text(encoding="utf-8"))
    for ov in cfg["overlays"]:
        name = "ov%03d" % ov["id"]
        binp = OVERLAY_DIR / ov["file_name"]
        if not binp.is_file():
            stats["overlay_images_missing"] += 1
            continue
        cfgdir = arm9_cfg / "overlays" / name
        mods.append(Module(name, ov["base_address"], binp.read_bytes(),
                           load_sections(cfgdir / "delinks.txt"), cfgdir))
    stats["modules_loaded"] = len(mods)
    stats["modules_without_a_section_table"] = sum(1 for m in mods if not m.sections)
    return mods


# --------------------------------------------------------------------------
# scan
# --------------------------------------------------------------------------

class Record:
    __slots__ = ("module", "va", "kind", "raw", "name", "name_va", "bases", "vtable",
                 "vtable_module")

    def __init__(self, module, va, kind, raw, name, name_va):
        self.module = module
        self.va = va
        self.kind = kind
        self.raw = raw
        self.name = name
        self.name_va = name_va
        self.bases = []            # [(base_va, offset, flags)]
        self.vtable = None
        self.vtable_module = None   # NOT always self.module -- see attach_vtables

    @property
    def key(self):
        return "%s:0x%08x" % (self.module, self.va)


def scan_records(mods, stats, rejects):
    """Every type_info record in every module, keyed by (module, va)."""
    recs = {}
    for m in mods:
        for off in range(0, len(m.data) - 8, 4):
            w = struct.unpack_from("<I", m.data, off)[0]
            kind = TI_KIND.get(w)
            if kind is None:
                continue
            va = m.base + off
            stats["candidate_words"] += 1
            if not m.in_data_section(va):
                stats["rejected_not_in_a_data_section"] += 1
                rejects.append((m.name, va, "not in a data section (pool literal)"))
                continue
            name_va = m.word(va + 4)
            owner = m if m.contains(name_va) else None
            if owner is None:
                for other in mods:
                    if other.name == "arm9" and other.contains(name_va):
                        owner = other
                        break
            if owner is None:
                stats["rejected_name_pointer_out_of_range"] += 1
                rejects.append((m.name, va, "name pointer 0x%08x out of range" % name_va))
                continue
            raw = owner.cstr(name_va)
            name = demangle(raw) if raw else None
            if name is None:
                stats["rejected_name_not_a_mangled_class_name"] += 1
                rejects.append((m.name, va, "name %r does not parse" % (raw,)))
                continue
            recs[(m.name, va)] = Record(m.name, va, kind, raw, name, name_va)
            stats["records_" + kind] += 1
    stats["records_total"] = len(recs)
    stats["records_nested_name"] = sum(1 for r in recs.values() if "::" in r.name)
    return recs


def read_bases(recs, mods, stats, rejects):
    """Fill each record's raw base pointers.  Resolution happens separately."""
    bym = {m.name: m for m in mods}
    for r in sorted(recs.values(), key=lambda x: (x.module, x.va)):
        m = bym[r.module]
        if r.kind == "class":
            continue
        if r.kind == "si":
            r.bases.append((m.word(r.va + 8), 0, None))
            continue
        n = m.word(r.va + 12)
        if n == 0 or n > MAX_VMI_BASES:
            stats["vmi_records_with_an_implausible_base_count"] += 1
            rejects.append((r.module, r.va, "vmi base count %d" % n))
            continue
        for i in range(n):
            bva, packed = struct.unpack_from("<II", m.data, r.va - m.base + 16 + 8 * i)
            r.bases.append((bva, packed >> 8, packed & 0xFF))
    stats["base_pointers_read"] = sum(len(r.bases) for r in recs.values())


# --------------------------------------------------------------------------
# resolution
# --------------------------------------------------------------------------

def resolve(recs, mods, stats, unresolved):
    """(module, base_va) for every base pointer, or a logged failure.

    Order: the referring module itself, then arm9, then whatever residency leaves.
    Never the bare address -- two addresses in this ROM host two different classes.
    """
    try:
        import overlay_residency as RES
    except Exception as exc:                                    # noqa: BLE001
        RES = None
        stats["residency_unavailable"] = 1
        unresolved.append(("-", 0, "overlay_residency unimportable: %s" % exc))

    by_va = defaultdict(list)
    for (mod, va) in recs:
        by_va[va].append(mod)
    stats["addresses_hosting_more_than_one_record"] = sum(
        1 for v in by_va.values() if len(v) > 1)

    edges = []
    for r in sorted(recs.values(), key=lambda x: (x.module, x.va)):
        for bva, off, flags in r.bases:
            cands = sorted(by_va.get(bva, []))
            if not cands:
                stats["edges_unresolved_no_record_at_target"] += 1
                unresolved.append((r.module, r.va,
                                   "%s -> 0x%08x: no record there" % (r.name, bva)))
                continue
            if r.module in cands:
                pick, how = r.module, "own_module"
            elif "arm9" in cands:
                pick, how = "arm9", "arm9"
            elif len(cands) == 1:
                pick, how = cands[0], "unique"
            else:
                left = ([c for c in cands if RES.possible(c, r.module)]
                        if RES else cands)
                if len(left) == 1:
                    pick, how = left[0], "residency"
                else:
                    stats["edges_unresolved_ambiguous"] += 1
                    unresolved.append(
                        (r.module, r.va, "%s -> 0x%08x: %d candidates survive residency (%s)"
                         % (r.name, bva, len(left), ",".join(left))))
                    continue
            base = recs[(pick, bva)]
            edges.append({
                "derived": r.name, "derived_key": r.key,
                "base": base.name, "base_key": base.key,
                "offset": off, "flags": flags, "resolved_by": how,
            })
            stats["edges_resolved"] += 1
            stats["edges_by_" + how] += 1
            if pick != r.module and pick != "arm9":
                stats["edges_crossing_into_another_overlay"] += 1
            elif pick == "arm9" and r.module != "arm9":
                stats["edges_into_arm9"] += 1
    return edges


def attach_vtables(recs, mods, stats):
    """A vtable's typeinfo pointer sits at V-4; record the reverse link.

    Primary vtables have offset-to-top 0 at V-8.  Secondary (multiple-inheritance)
    tables have a negative one and are counted, not attached.

    A vtable does NOT always sit in the same module as the typeinfo it points at.
    Four classes -- daDsnBase_c (ov025), daObjFallBlock_c (ov015), daObjMaruta_c
    (ov022) and daOts_c (ov027) -- have their vtable in a *different*, non-
    overlapping overlay (ov091, ov098, ov080, ov064).  Looking only in the record's
    own module and arm9 left all four with no vtable at all, which then read as
    "this class has no methods" downstream -- the opposite of the truth.

    Cross-module candidates are settled the same way every other cross-overlay
    question in this tree is: only one module hosts a record at the address, and
    tools/overlay_residency.py must allow it to be resident alongside the module
    holding the vtable.  Same-module and arm9 keep priority so no attribution that
    was already right can move.
    """
    try:
        import overlay_residency as RES
    except Exception:                                           # noqa: BLE001
        RES = None

    byk = {(r.module, r.va): r for r in recs.values()}
    by_addr = defaultdict(list)
    for r in recs.values():
        by_addr[r.va].append(r)

    for m in mods:
        for off in range(8, len(m.data) - 4, 4):
            va = m.base + off
            if not m.in_data_section(va):
                continue
            p = struct.unpack_from("<I", m.data, off)[0]
            if p not in by_addr:
                continue
            top = struct.unpack_from("<i", m.data, off - 4)[0]
            if top != 0:
                if -0x10000 < top < 0:
                    stats["vtables_secondary"] += 1
                continue
            if (m.name, p) in byk:
                r, how = byk[(m.name, p)], "own_module"
            elif ("arm9", p) in byk:
                r, how = byk[("arm9", p)], "arm9"
            else:
                left = [x for x in by_addr[p]
                        if RES is None or RES.possible(x.module, m.name)]
                if len(left) != 1:
                    stats["vtables_cross_module_ambiguous"] += 1
                    continue
                r, how = left[0], "cross_module"
            if r.vtable is None:
                r.vtable = va + 4
                r.vtable_module = m.name
                stats["vtables_primary"] += 1
                stats["vtables_by_" + how] += 1
    stats["records_without_a_vtable"] = sum(1 for r in recs.values() if r.vtable is None)


# --------------------------------------------------------------------------
# main
# --------------------------------------------------------------------------

def build():
    stats = Counter()
    rejects, unresolved = [], []
    mods = load_modules(stats)
    recs = scan_records(mods, stats, rejects)
    read_bases(recs, mods, stats, rejects)
    edges = resolve(recs, mods, stats, unresolved)
    attach_vtables(recs, mods, stats)

    # graph shape
    parents = defaultdict(list)
    for e in edges:
        parents[e["derived_key"]].append(e["base_key"])
    adj = defaultdict(set)
    for e in edges:
        adj[e["derived_key"]].add(e["base_key"])
        adj[e["base_key"]].add(e["derived_key"])
    seen, comps = set(), []
    for k in sorted(r.key for r in recs.values()):
        if k in seen:
            continue
        stack, comp = [k], set()
        while stack:
            x = stack.pop()
            if x in comp:
                continue
            comp.add(x)
            stack.extend(adj[x] - comp)
        seen |= comp
        comps.append(sorted(comp))
    comps.sort(key=lambda c: (-len(c), c[0]))
    stats["components"] = len(comps)
    stats["roots"] = sum(1 for r in recs.values() if not parents.get(r.key))
    stats["classes_with_multiple_bases"] = sum(1 for v in parents.values() if len(v) > 1)

    out = {
        "meta": {
            "source": "extracted/arm9_dec.bin + extracted/dsd/arm9_overlays/*.bin",
            "type_info_vtables": {hex(k): v for k, v in sorted(TI_KIND.items())},
            "stats": dict(sorted(stats.items())),
        },
        "records": {
            r.key: {
                "module": r.module, "addr": "0x%08x" % r.va, "kind": r.kind,
                "mangled": r.raw, "name": r.name,
                "name_addr": "0x%08x" % r.name_va,
                "vtable": ("0x%08x" % r.vtable) if r.vtable else None,
                "vtable_module": r.vtable_module,
            } for r in sorted(recs.values(), key=lambda x: (x.module, x.va))
        },
        "edges": sorted(edges, key=lambda e: (e["derived_key"], e["base_key"])),
        "components": [{"size": len(c), "members": c} for c in comps],
        "rejected": [{"module": m, "addr": "0x%08x" % a, "why": w}
                     for m, a, w in sorted(rejects)],
        "unresolved": [{"module": m, "addr": "0x%08x" % a, "why": w}
                       for m, a, w in sorted(unresolved)],
    }
    return out


GATE = {"records_total": 429, "edges_resolved": 413}


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--out", default=str(REPO / "build" / "rtti.json"))
    ap.add_argument("--report", action="store_true", help="print populations first")
    ap.add_argument("--check", action="store_true",
                    help="fail unless the frozen census reproduces")
    a = ap.parse_args()

    out = build()
    st = out["meta"]["stats"]

    if a.report:
        print("== populations ==")
        for k in sorted(st):
            print("  %-48s %d" % (k, st[k]))
        print("\n== rejected (%d) ==" % len(out["rejected"]))
        for r in out["rejected"][:20]:
            print("  %s %s  %s" % (r["module"], r["addr"], r["why"]))
        print("\n== unresolved (%d) ==" % len(out["unresolved"]))
        for r in out["unresolved"]:
            print("  %s %s  %s" % (r["module"], r["addr"], r["why"]))
        print("\n== components ==")
        for c in out["components"][:8]:
            root = [m for m in c["members"]
                    if not any(e["derived_key"] == m for e in out["edges"])]
            print("  %4d  %s" % (c["size"], ", ".join(
                out["records"][m]["name"] for m in root[:4]) or "(cycle)"))

    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    with open(a.out, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(out, fh, indent=1, sort_keys=False)
        fh.write("\n")
    print("wrote %s: %d records, %d edges, %d unresolved, %d components"
          % (a.out, st.get("records_total", 0), st.get("edges_resolved", 0),
             len(out["unresolved"]), st.get("components", 0)))

    if a.check:
        bad = [(k, v, st.get(k)) for k, v in GATE.items() if st.get(k) != v]
        if bad or out["unresolved"]:
            for k, want, got in bad:
                print("GATE FAIL %s: want %d got %s" % (k, want, got))
            for u in out["unresolved"]:
                print("GATE FAIL unresolved: %s %s %s" % (u["module"], u["addr"], u["why"]))
            return 1
        print("GATE OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
