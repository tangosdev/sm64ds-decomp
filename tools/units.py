#!/usr/bin/env python3
"""The unit worklist: classes and translation units by state, not functions.

At 100% matched, a function worklist has nothing left to hand out. The work that
moves is class conversion and TU promotion, so the unit a writer (or tangOS Console)
asks for is a class or a TU. This answers "what are the next N units, and what state
is each in", read-only, from what the tree already records:

    build/tu_map.json          TU candidates          (tools/tu_map.py; tubuild's ids)
    config/tu_manifest.d/      TU state               (shadow, text-verified,
                                                       link-verified, promoted, ...)
    config/**/delinks.txt      who builds each function (srcpath's enrolment table)
    build/rtti.json            ROM class names        (tools/rtti_extract.py; optional)
    notes/data/tu-promotion-queue.tsv  route/blockers (tools/classqueue.py; optional)

It reuses tubuild's candidate ids and disqualifier scan, srcpath's ownership index
and classqueue's queue reader instead of re-deriving any of them, and it writes
nothing.

Two kinds of unit:

  --kind tu     one row per tu_map candidate, plus each manifest entry no candidate
                covers (arm9 entries are hand-assembled because tu_map cannot segment
                arm9 main). State is read by ADDRESS against every manifest entry, not
                by id: `tubuild list` matches ids only, so a TU promoted under a
                different id (ov020/daBook_c+daBookGen_c for tu_map's
                ov020/daBookGen_c+daBook_c) reads as untouched there.
  --kind class  one row per class name the tree's symbols carry (the class_of of a
                mangled method or a `<Class>_classInit` factory), with the RTTI name
                when the ROM proves one. State says how many of its functions are
                already built from a promoted TU and how many are one-function files.

States, best first: promoted, partial (some functions licensed by a promoted entry,
the rest not), link-verified / text-verified / shadow (a manifest entry exists and is
not promoted yet), none (nothing yet). `residue-lifecycle` marks a class whose only
unpromoted functions are constructors, destructors and thunks -- the shape the
key-function and MI walls leave behind (arm9/ModelAnim, arm9/Actor).

    python tools/units.py                              # next 20 units, table
    python tools/units.py --kind class --next 10       # next 10 classes to convert
    python tools/units.py --state none,shadow --json   # machine rows
    python tools/units.py --module ov002 --kind tu --all
"""
import argparse
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import srcpath as SP        # noqa: E402
import tu_manifest as TUM   # noqa: E402

RTTI = REPO / "build" / "rtti.json"
CFG = REPO / "config" / "arm9"
STATE_ORDER = ["none", "shadow", "text-verified", "link-verified", "data-verified",
               "partial", "residue-lifecycle", "promoted"]
_FUNC_RE = re.compile(r"^(\S+)\s+kind:function\([^,]+,size=0x([0-9a-fA-F]+)\)"
                      r"\s+addr:0x([0-9a-fA-F]+)")
_STRUCTOR_RE = re.compile(r"(C[123]|D[012])Ev$")


def label(module):
    """tu_map says `main`, the manifest and config say `arm9`."""
    return "arm9" if module == "main" else module


def symbols_path(module):
    module = label(module)
    if module == "arm9":
        return CFG / "symbols.txt"
    if module in ("itcm", "dtcm"):
        return CFG / module / "symbols.txt"
    return CFG / "overlays" / module / "symbols.txt"


_funcs = {}


def functions(module):
    """{name: (addr, size)} for a module's kind:function rows."""
    module = label(module)
    if module not in _funcs:
        out = {}
        p = symbols_path(module)
        if p.is_file():
            for line in p.read_text(encoding="utf-8", errors="ignore").splitlines():
                m = _FUNC_RE.match(line.strip())
                if m:
                    out[m.group(1)] = (int(m.group(3), 16), int(m.group(2), 16))
        _funcs[module] = out
    return _funcs[module]


def all_modules():
    mods = ["arm9", "itcm"]
    mods += sorted(p.name for p in (CFG / "overlays").iterdir() if p.is_dir())
    return mods


def manifest_index():
    """{(module, addr): (entry id, status)} for every function a manifest entry licenses."""
    idx = {}
    entries = TUM.load()["entries"]
    for e in entries:
        for f in e.get("functions", []):
            idx[(label(e["module"]), int(f["address"], 16))] = (e["id"], e.get("status"))
    return idx, entries


def owner(symbol, ownership):
    """'tu' (a file owning several functions), 'legacy' (one-function file) or 'none'."""
    p = SP.path_for(symbol)
    if p is None:
        return "none", None
    rel = p.relative_to(REPO).as_posix()
    return ("tu" if len(ownership.get(rel) or ()) > 1 else "legacy"), rel


def rtti_names():
    if not RTTI.is_file():
        return {}, False
    recs = json.loads(RTTI.read_text(encoding="utf-8"))["records"].values()
    return {r["name"]: r for r in recs}, True


def queue_rows():
    try:
        import classqueue as CQ
        _path, rows = CQ.rows()
    except (ImportError, SystemExit, OSError):
        return {}
    return {r.get("class_name"): r for r in rows if r.get("class_name")}


def state_of(statuses):
    """Fold the manifest statuses of a unit's functions into one state."""
    present = [s for s in statuses if s]
    if not present:
        return "none"
    if all(s == "promoted" for s in statuses):
        return "promoted"
    if any(s == "promoted" for s in present):
        return "partial"
    for s in ("data-verified", "link-verified", "text-verified", "shadow"):
        if s in present:
            return s
    return present[0]


# ===================================================================== TU rows

def tu_rows(module_filter=None):
    import tubuild as TB
    tu_data, note = TB.load_tu_map()
    midx, entries = manifest_index()
    ownership = SP.source_ownership_index()
    rtti, _have = rtti_names()
    covered = set()
    rows = []
    for mod, _rec, unit, tid in TB.iter_candidate_units(tu_data, module_filter):
        mod_l = label(mod)
        fidx = functions(mod_l)
        funcs = [(n, *fidx[n]) for n in unit.get("functions", []) if n in fidx]
        funcs.sort(key=lambda r: r[1])
        lic = [midx.get((mod_l, a)) for _n, a, _s in funcs]
        covered.update((mod_l, a) for _n, a, _s in funcs)
        own = collections.Counter(owner(n, ownership)[0] for n, _a, _s in funcs)
        dq = TB.function_disqualifiers(mod_l, [(i, n, a, s) for i, (n, a, s) in enumerate(funcs)])
        classes = unit.get("classes") or []
        rows.append({
            "kind": "tu", "id": tid, "module": mod_l,
            "range": [unit["start"], unit["end"]], "functions": len(funcs),
            "classes": classes, "rtti_classes": [c for c in classes if c in rtti],
            "state": state_of([s for _i, s in (x or (None, None) for x in lic)]),
            "manifest_ids": sorted({x[0] for x in lic if x}),
            "owners": dict(own),
            "blockers": {"pragma_files": len(dq["pragmas"]), "dtor_variants": len(dq["dtor_variants"]),
                         "unbuilt": len(dq["unmatched"])},
        })
    # Manifest entries tu_map has no candidate for (arm9's are hand-assembled), and
    # every unpromoted entry whose id is not a candidate id: inside an
    # under-segmented unit such as arm9 main its state would otherwise be folded
    # into the giant unit's `partial` and never surface.
    ids = {r["id"] for r in rows}
    if module_filter is None or label(module_filter) in {label(e["module"]) for e in entries}:
        for e in entries:
            m = label(e["module"])
            if module_filter and m != label(module_filter):
                continue
            if e["id"] in ids:
                continue
            addrs = [(m, int(f["address"], 16)) for f in e.get("functions", [])]
            if addrs and all(a in covered for a in addrs) and e.get("status") == "promoted":
                continue
            own = collections.Counter(owner(f["symbol"], ownership)[0] for f in e.get("functions", []))
            rows.append({
                "kind": "tu", "id": e["id"], "module": m,
                "range": [s["start"] for s in e.get("sections", [])[:1]]
                         + [s["end"] for s in e.get("sections", [])[:1]],
                "functions": len(e.get("functions", [])), "classes": [], "rtti_classes": [],
                "state": e.get("status") or "none", "manifest_ids": [e["id"]],
                "owners": dict(own), "blockers": {}, "hand_assembled": True,
            })
    return rows, note


# ===================================================================== class rows

def class_rows(module_filter=None):
    midx, _entries = manifest_index()
    ownership = SP.source_ownership_index()
    rtti, have_rtti = rtti_names()
    queue = queue_rows()
    tree_to_rom = {}
    if have_rtti:
        for name, r in rtti.items():
            tree_to_rom[name] = name
    # A coined tree name maps to its ROM name through the vtable symbol at the
    # RTTI record's vtable address.
    vt_names = {}
    for name, r in rtti.items():
        if r.get("vtable"):
            vt_names[(label(r.get("vtable_module") or r["module"]), int(r["vtable"], 16))] = name
    for mod in all_modules():
        p = symbols_path(mod)
        if not p.is_file():
            continue
        for line in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = re.match(r"^_ZTV(\d+)(\S+)\s+kind:data\S*\s+addr:0x([0-9a-fA-F]+)", line.strip())
            if m:
                rom = vt_names.get((mod, int(m.group(3), 16)))
                tree = m.group(2)[: int(m.group(1))]
                if rom:
                    tree_to_rom.setdefault(tree, rom)

    by_class = collections.defaultdict(list)
    for mod in all_modules():
        if module_filter and mod != label(module_filter):
            continue
        for name, (addr, size) in functions(mod).items():
            m = re.match(r"(.+)_classInit$", name)
            cls = m.group(1) if m else SP.class_of(name)
            if cls:
                by_class[cls].append((mod, name, addr, size))
    rows = []
    for cls, funcs in by_class.items():
        statuses, structor_left = [], 0
        own = collections.Counter()
        for mod, name, addr, _size in funcs:
            lic = midx.get((mod, addr))
            statuses.append(lic[1] if lic else None)
            kind, _path = owner(name, ownership)
            own[kind] += 1
            if not lic or lic[1] != "promoted":
                if _STRUCTOR_RE.search(name) or name.startswith("_ZThn"):
                    structor_left += 1
        state = state_of(statuses)
        not_done = sum(1 for s in statuses if s != "promoted")
        if state in ("partial", "none") and not_done and structor_left == not_done \
                and any(s == "promoted" for s in statuses):
            state = "residue-lifecycle"
        q = queue.get(cls) or queue.get(tree_to_rom.get(cls) or "") or {}
        rows.append({
            "kind": "class", "id": cls, "rom_name": tree_to_rom.get(cls),
            "modules": sorted({m for m, *_ in funcs}), "functions": len(funcs),
            "state": state, "unpromoted": not_done, "owners": dict(own),
            "queue": {k: q.get(k) for k in ("blockers", "promotion_route", "already_promoted")} if q else None,
        })
    return rows


# ===================================================================== main

def order_key(row):
    st = row["state"]
    return (STATE_ORDER.index(st) if st in STATE_ORDER else len(STATE_ORDER),
            -(row.get("rom_name") is not None or bool(row.get("rtti_classes"))),
            row.get("unpromoted", row["functions"]), row["id"])


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--kind", choices=("tu", "class"), default="tu")
    ap.add_argument("--state", help="comma-separated states to keep (default: everything "
                                    "not promoted)")
    ap.add_argument("--module", help="one module (arm9, itcm, ov002, ...)")
    ap.add_argument("--next", type=int, default=20, metavar="N", help="first N rows (default 20)")
    ap.add_argument("--all", action="store_true", help="every row, no --next limit")
    ap.add_argument("--json", action="store_true", help="JSON: {meta, units: [...]}")
    a = ap.parse_args(argv)

    note = None
    if a.kind == "tu":
        rows, note = tu_rows(a.module)
    else:
        rows = class_rows(a.module)
    counts = collections.Counter(r["state"] for r in rows)
    keep = set(a.state.split(",")) if a.state else set(STATE_ORDER) - {"promoted"}
    rows = sorted((r for r in rows if r["state"] in keep), key=order_key)
    total = len(rows)
    if not a.all:
        rows = rows[: a.next]

    if a.json:
        print(json.dumps({"meta": {"kind": a.kind, "states": dict(counts), "matching": total,
                                   "shown": len(rows), "note": note},
                          "units": rows}, indent=1))
        return 0
    if note:
        print(f"NOTE: {note}\n")
    print("states: " + ", ".join(f"{k} {counts[k]}" for k in STATE_ORDER if counts.get(k))
          + f"   ({total} matching, {len(rows)} shown)")
    if a.kind == "tu":
        print(f"{'id':48} {'state':15} {'fn':>4}  owners              rtti")
        for r in rows:
            own = " ".join(f"{k}:{v}" for k, v in sorted(r["owners"].items()))
            print(f"{r['id'][:48]:48} {r['state']:15} {r['functions']:4d}  {own:19} "
                  f"{','.join(r['rtti_classes'])[:40]}")
    else:
        print(f"{'class':28} {'rom name':24} {'state':17} {'fn':>4} {'left':>4}  owners")
        for r in rows:
            own = " ".join(f"{k}:{v}" for k, v in sorted(r["owners"].items()))
            print(f"{r['id'][:28]:28} {(r['rom_name'] or '-')[:24]:24} {r['state']:17} "
                  f"{r['functions']:4d} {r['unpromoted']:4d}  {own}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
