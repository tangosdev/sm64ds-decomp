#!/usr/bin/env python3
"""Scaffold a C++ class and its translation unit from what the ROM proves.

The C++-first path: a class is recovered from the cartridge's own RTTI, its vtable
and its factory before any method body is written, so the first source a writer
touches is a real class in the right TU instead of a one-function C file. This
tool joins the existing evidence passes and writes nothing it cannot cite:

    build/rtti.json          (tools/rtti_extract.py)  ROM class name, record kind,
                                                      base edges and offsets
    the ROM vtable itself    (read here, see below)   slot order, own overrides,
                                                      new virtuals
    build/opnew_sizes.json   (tools/opnew_sizes.py)   instance size from the
                                                      factory's operator new literal
    build/tu_map.json        (tools/tu_map.py)        the recovered TU boundary
    config/**/symbols.txt    (via srcpath / tubuild)  the tree's names, current owners
    config/tu_manifest.d/                             what is already promoted

What it emits, for one class:

  * a header: the RTTI base, the destructor pair as the inline one-line
    `virtual ~X() {}` form (notes/cpp-class-form.md), overrides and new virtuals in
    vtable slot order with the slot and ROM address beside each, the size assertion
    when the factory proves it, and one `unk_` block for the bytes nothing proves,
    marked for a reviewer. Return types come from a base header declaration of the
    same method when one exists; anything else is marked UNPROVEN.
  * a TU skeleton for src_tu/: one `// @symbol` stub per licensed member, written
    ROM-descending because mwccarm emits .text in reverse source order under the
    default deferred codegen (every promoted TU in the tree is written that way;
    tubuild verify reports the emitted order).
  * a manifest entry (status `shadow`) shaped like tubuild create's, so
    `tubuild verify` and `tu_promote --dry-run` take it unchanged.

Dry run by default: it prints the evidence join and the files it would write.
`--write` writes them. An existing include/ header is never overwritten (a shared
header is a claim every consumer agrees on, AGENTS.md); the scaffold header then goes
to build/scaffold/<class>/ for a reviewer to diff. `--out DIR` sends every file
there instead, which is how a scaffold is compared with a hand-made promotion.

    python tools/class_scaffold.py dExtAnm2Model_c                 # dry run
    python tools/class_scaffold.py ModelAnim2 --exclude-lifecycle  # tree name works too
    python tools/class_scaffold.py daBar_c --out build/scaffold/daBar_c --write
    python tools/class_scaffold.py daBar_c --json                  # evidence only

WHY IT READS THE VTABLE ITSELF. build/rtti_vtables.json is read out of
extracted/dsd/arm9_overlays, so it is only as good as that extraction. Built from a
stale shared extraction it put 364 of 426 tables wrong (daBar_c: 21 slots from
0x02010ecc, where the ROM's 0x84-byte table at 0x02108480 holds 31 from InitResources
0x020b067c, as the ov002/daBar_c manifest records). Rebuilt from a fresh one it agrees
with this reader on 411 of 426. Reading through build_pin.target_bytes -- the bytes
every byte gate compares against -- takes the extraction out of the question. The
primary base's slot count is the floor for a derived table; see read_vtable for
where a table ends.

WHAT IT DOES NOT DO. It does not name fields, guess return types, reconstruct a
body, or decide a lifecycle form. Those are the human (or AI writer) parts. It says
which lifecycle shape the class needs and cites the promoted precedent, because the
compiler's destructor and key-function behaviour is what decides whether the TU can
own the vtable (notes/cpp-class-form.md, notes/tu-promotion-conventions.md).
"""
import argparse
import json
import pathlib
import re
import struct
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import build_pin as BP     # noqa: E402
import demangle as DM      # noqa: E402
import srcpath as SP       # noqa: E402
import tu_manifest as TUM  # noqa: E402
import tu_names as TN      # noqa: E402

BUILD = REPO / "build"
RTTI = BUILD / "rtti.json"
TU_MAP = BUILD / "tu_map.json"
OPNEW = BUILD / "opnew_sizes.json"
CFG = REPO / "config" / "arm9"
INCLUDE = REPO / "include"
SRC_TU = REPO / "src_tu"

_SYM_RE = re.compile(r"^(\S+)\s+kind:(\w+)(?:\(([^)]*)\))?\s+addr:0x([0-9a-fA-F]+)")
_STRUCTOR_RE = re.compile(r"(C[123]|D[012])Ev$")
_MAX_SLOTS = 128


# ============================================================================ loading

def symbols_path(module):
    if module in ("arm9", "main"):
        return CFG / "symbols.txt"
    if module in ("itcm", "dtcm"):
        return CFG / module / "symbols.txt"
    return CFG / "overlays" / module / "symbols.txt"


_sym_cache = {}


def module_symbols(module):
    """[(name, kind, addr, size)] for one module, ROM order. size is 0 for data."""
    module = "arm9" if module == "main" else module
    if module not in _sym_cache:
        rows = []
        path = symbols_path(module)
        if path.is_file():
            for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
                m = _SYM_RE.match(line.strip())
                if not m:
                    continue
                size = 0
                sm = re.search(r"size=0x([0-9a-fA-F]+)", m.group(3) or "")
                if sm:
                    size = int(sm.group(1), 16)
                rows.append((m.group(1), m.group(2), int(m.group(4), 16), size))
        rows.sort(key=lambda r: r[2])
        _sym_cache[module] = rows
    return _sym_cache[module]


def functions_by_addr(module):
    return {a: (n, s) for n, k, a, s in module_symbols(module) if k == "function"}


def data_at(module, addr):
    return [n for n, k, a, _s in module_symbols(module) if k != "function" and a == addr]


def need(path, how):
    if not path.is_file():
        raise SystemExit(f"missing {path.relative_to(REPO).as_posix()} -- run {how}")
    return json.loads(path.read_text(encoding="utf-8"))


def tree_name_from_ztv(sym):
    m = re.match(r"_ZTV(\d+)(.+)$", sym or "")
    if not m:
        return None
    n = int(m.group(1))
    return m.group(2)[:n] if len(m.group(2)) >= n else None


class Evidence:
    """Every RTTI class, its vtable symbol and its tree name, keyed both ways."""

    def __init__(self):
        rtti = need(RTTI, "python tools/rtti_extract.py")
        self.records = {}
        for key, rec in rtti["records"].items():
            rec = dict(rec, key=key)
            vt_mod = rec.get("vtable_module") or rec["module"]
            rec["ztv"] = None
            rec["tree"] = None
            if rec.get("vtable"):
                for sym in data_at(vt_mod, int(rec["vtable"], 16)):
                    if sym.startswith("_ZTV"):
                        rec["ztv"] = sym
                        rec["tree"] = tree_name_from_ztv(sym)
            self.records.setdefault(rec["name"], rec)
        self.bases = {}
        for e in rtti["edges"]:
            self.bases.setdefault(e["derived"], []).append(e)
        self.by_tree = {}
        for rec in self.records.values():
            if rec["tree"]:
                self.by_tree.setdefault(rec["tree"], rec)
        self.opnew = json.loads(OPNEW.read_text(encoding="utf-8"))["classes"] \
            if OPNEW.is_file() else None

    def find(self, name):
        rec = self.records.get(name) or self.by_tree.get(name)
        if rec is None:
            raise SystemExit(f"{name!r}: no RTTI record and no _ZTV naming it. The scaffolder "
                             "only starts from a class the ROM proves; for an RTTI-less class "
                             "use the vptr-chain evidence (tools/evidence_hierarchy.py) by hand.")
        return rec

    def tree_of(self, rtti_name):
        rec = self.records.get(rtti_name)
        return (rec and rec["tree"]) or rtti_name

    def size_of(self, rec):
        if not self.opnew:
            return None
        for key in (rec["name"], rec["tree"]):
            row = self.opnew.get(key) if key else None
            if row and not row.get("ambiguous"):
                return row
        for row in self.opnew.values():
            if rec["name"] in (row.get("rtti_name") or []) and not row.get("ambiguous"):
                return row
        return None


# ============================================================================ vtable

def read_vtable(rec, floor=0):
    """[(slot, addr, symbol_or_None, module)] read from the ROM bytes.

    A slot is a function address (the class's module, arm9, itcm, or a unique hit
    in another overlay) or a zero (pure virtual). ``floor`` is the primary base's
    slot count: a derived table has at least that many slots, so the first
    ``floor`` words are taken as slots whatever labels the delinker put among them
    (a coined `BridgePenguin_SpawnInfo` sits inside daPgMthr_c's table). Past the
    floor the table ends at a named symbol, at a non-code word, at the offset-to-top
    zero before a typeinfo pointer, or where (function, 0) pointer-to-member pairs
    start (state tables follow daObjWaterfall_c's vtable)."""
    if not rec.get("vtable"):
        return []
    mod = rec.get("vtable_module") or rec["module"]
    start = int(rec["vtable"], 16)
    later = [a for n, _k, a, _s in module_symbols(mod)
             if a >= start + 4 * max(floor, 1) and not n.startswith("data_")]
    limit = min(later) if later else start + 4 * _MAX_SLOTS
    nwords = max(floor, min(_MAX_SLOTS, (limit - start) // 4))
    raw = BP.target_bytes("arm9" if mod in ("main",) else mod, start, nwords * 4) or b""
    words = struct.unpack(f"<{len(raw) // 4}I", raw[: len(raw) // 4 * 4])
    tables = [(m, functions_by_addr(m)) for m in dict.fromkeys((rec["module"], mod, "arm9", "itcm"))]

    def lookup(w):  # (module, symbol) or None; ("?", None) when ambiguous
        hit = next(((m, t[w][0]) for m, t in tables if w in t), None)
        if hit is None:
            # A slot can point into another overlay loaded beside this one
            # (dMgPsOpt_c::TouchIcon_c's table in ov004 points into ov001).
            hits = [(m, functions_by_addr(m)[w][0]) for m in _all_modules()
                    if w in functions_by_addr(m)]
            if len(hits) == 1:
                hit = hits[0]
            elif hits:
                # Several overlays define a function here: a slot, but which
                # module it binds to is a load-group question.
                hit = ("?", None)
        return hit

    slots = []
    for i in range(min(floor, len(words))):
        w = words[i]
        hit = lookup(w) if w else None
        slots.append((i, w, hit[1] if hit else None, hit[0] if hit else None))
    i = len(slots)
    while i < len(words):
        w = words[i]
        if w == 0:
            # A zero is a pure-virtual slot, unless the zero run ends at a pointer
            # to a typeinfo record: then its last zero is the NEXT table's
            # offset-to-top (notes/model-rtti-names.md).
            j = i
            while j < len(words) and words[j] == 0:
                j += 1
            nxt = words[j] if j < len(words) else None
            if nxt is not None and lookup(nxt) is not None and i > 0 and floor == 0:
                slots.extend((k, 0, None, None) for k in range(i, j))
                i = j
                continue
            if nxt is not None and nxt in _typeinfo_addrs():
                slots.extend((k, 0, None, None) for k in range(i, j - 1))
            break
        hit = lookup(w)
        if hit is None:
            break
        if floor and i + 2 < len(words) and words[i + 1] == 0 and lookup(words[i + 2]):
            break  # (function, 0) pairs: a pointer-to-member table, not slots
        slots.append((i, w, hit[1], hit[0]))
        i += 1
    return slots


_vt_memo = {}


def vtable_for(ev, name, _depth=0):
    """read_vtable with the primary base's slot count as the floor, memoized."""
    if name in _vt_memo:
        return _vt_memo[name]
    rec = ev.records[name]
    prim = next((e for e in ev.bases.get(name, []) if e["offset"] == 0), None)
    floor = 0
    if prim and prim["base"] in ev.records and _depth < 16:
        floor = len(vtable_for(ev, prim["base"], _depth + 1))
    _vt_memo[name] = read_vtable(rec, floor)
    return _vt_memo[name]


_typeinfo_cache = None


def _typeinfo_addrs():
    global _typeinfo_cache
    if _typeinfo_cache is None:
        recs = json.loads(RTTI.read_text(encoding="utf-8"))["records"].values() \
            if RTTI.is_file() else []
        _typeinfo_cache = {int(r["addr"], 16) for r in recs}
    return _typeinfo_cache


def _all_modules():
    mods = ["arm9", "itcm"]
    ovs = CFG / "overlays"
    if ovs.is_dir():
        mods += sorted(p.name for p in ovs.iterdir() if p.is_dir())
    return mods


def classify_slots(ev, rec):
    """Each slot as inherited / override / new against the primary base's table."""
    slots = vtable_for(ev, rec["name"])
    prim = next((e for e in ev.bases.get(rec["name"], []) if e["offset"] == 0), None)
    base_slots = vtable_for(ev, prim["base"]) if prim and prim["base"] in ev.records else []
    base_addr = {i: a for i, a, _s, _m in base_slots}
    out = []
    for i, a, sym, mod in slots:
        if i >= len(base_slots):
            kind = "new"
        elif base_addr.get(i) == a:
            kind = "inherited"
        else:
            kind = "override"
        out.append({"slot": i, "addr": a, "symbol": sym, "module": mod, "kind": kind})
    return out, len(base_slots)


# ============================================================================ members

def class_of(sym):
    m = re.match(r"(.+)_classInit$", sym)
    if m:
        return m.group(1)
    return SP.class_of(sym)


def owner_kind(sym):
    """('legacy'|'tu'|'none', repo-relative path or None) for a symbol's current source."""
    p = SP.path_for(sym)
    if p is None:
        return "none", None
    rel = p.relative_to(REPO).as_posix()
    owned = SP.source_ownership_index().get(rel) or []
    return ("tu" if len(owned) > 1 else "legacy"), rel


_manifest_idx = None


def promoted_owner(module, addr):
    """(manifest id, status) of the entry licensing this function, or None."""
    global _manifest_idx
    if _manifest_idx is None:
        _manifest_idx = {}
        for e in TUM.load()["entries"]:
            for f in e.get("functions", []):
                _manifest_idx[(e.get("module"), int(f["address"], 16))] = (e["id"], e.get("status"))
    return _manifest_idx.get(("arm9" if module == "main" else module, addr))


def tu_unit(module, addrs):
    """The tu_map unit holding these addresses, plus the module's segmentation flag."""
    if not TU_MAP.is_file():
        return None, None
    data = json.loads(TU_MAP.read_text(encoding="utf-8"))
    label = "main" if module == "arm9" else module
    rec = data.get("modules", {}).get(label)
    if not rec:
        return None, None
    under = any(u["module"] == label for u in data["meta"].get("under_segmented", []))
    for u in rec["units"]:
        lo, hi = int(u["start"], 16), int(u["end"], 16)
        if any(lo <= a < hi for a in addrs):
            return u, under
    return None, under


def member_run(ev, rec, slots, exclude_lifecycle):
    """The functions this class's TU licenses, ROM order, plus the boundary evidence."""
    module = rec["module"]
    names = {rec["name"], rec["tree"]} - {None}
    funcs = [(a, n, s) for n, k, a, s in module_symbols(module) if k == "function"]
    mine = [(a, n, s) for a, n, s in funcs if class_of(n) in names]
    own_addrs = {s["addr"] for s in slots if s["kind"] != "inherited"
                 and s["module"] in (module, "main" if module == "arm9" else module)}
    mine_addrs = {a for a, _n, _s in mine} | own_addrs
    evidence, warnings = [], []
    unit, under = tu_unit(module, sorted(mine_addrs))
    classes = set(unit.get("classes") or []) if unit else set()
    if unit and not under and classes <= names | {None}:
        lo, hi = int(unit["start"], 16), int(unit["end"], 16)
        evidence.append(f"contiguous linker run 0x{lo:08x}..0x{hi:08x} from build/tu_map.json "
                        f"(tools/tu_map.py), class label(s): {', '.join(sorted(classes)) or 'none'}")
        if not under and unit and not classes:
            pass
    else:
        if not mine_addrs:
            raise SystemExit(f"{rec['name']}: no function in {module} carries the class name "
                             "or sits in its vtable")
        lo = min(mine_addrs)
        last = max(mine_addrs)
        hi = next(a + s for a, n, s in funcs if a == last)
        others = sorted(classes - names)
        why = ("module is under-segmented in build/tu_map.json" if under
               else f"tu_map unit also carries {len(others)} other class(es), e.g. "
                    f"{', '.join(others[:4])}" if unit else "no tu_map unit")
        evidence.append(f"class run 0x{lo:08x}..0x{hi:08x}: first to last function named for "
                        f"the class or in its vtable ({why}); a TU boundary a reviewer must "
                        "confirm against the neighbours")
    # tu_map cuts a factory into a unit of its own (ov002 daBar_c_classInit, ov003
    # dScTitle_c_classInit); a neighbour named for the class belongs to the run.
    addrs = [a for a, _n, _s in funcs]
    i = addrs.index(lo) if lo in addrs else None
    while i is not None and i > 0 and class_of(funcs[i - 1][1]) in names:
        i -= 1
        lo = funcs[i][0]
        evidence.append(f"extended down over {funcs[i][1]}, named for the class")
    j = next((k for k, a in enumerate(addrs) if a >= hi), None)
    while j is not None and j < len(funcs) and funcs[j][0] == hi and class_of(funcs[j][1]) in names:
        evidence.append(f"extended up over {funcs[j][1]}, named for the class")
        hi = funcs[j][0] + funcs[j][2]
        j += 1
    run = [(a, n, s) for a, n, s in funcs if lo <= a < hi]
    foreign = [n for a, n, s in run if class_of(n) not in names | {None}
               and not n.startswith("func_")]
    if foreign:
        warnings.append(f"run holds functions named for other classes: {foreign[:6]}"
                        f"{' ...' if len(foreign) > 6 else ''}")
    structors = [n for _a, n, _s in run if _STRUCTOR_RE.search(n) or n.startswith("_ZThn")]
    if exclude_lifecycle and structors:
        kept = [r for r in run if r[1] not in structors]
        idx = [i for i, r in enumerate(run) if r[1] not in structors]
        if idx and idx != list(range(idx[0], idx[-1] + 1)):
            raise SystemExit(f"{rec['name']}: lifecycle members sit inside the method run, so "
                             "excluding them leaves two .text spans; tu_promote needs one")
        evidence.append(f"lifecycle excluded ({', '.join(structors)}): they stay enrolled "
                        "shards, the split arm9/ModelAnim and arm9/BlendModelAnim recorded")
        run = kept
    return run, evidence, warnings, structors


# ============================================================================ header

def header_for(tree):
    p = INCLUDE / f"{tree}.h"
    return p if p.is_file() else None


def header_size(tree):
    p = header_for(tree)
    if p is None:
        return None
    m = re.search(r"sizeof\(\s*(?:struct\s+|class\s+)?" + re.escape(tree)
                  + r"\s*\)\s*==\s*(0x[0-9a-fA-F]+|\d+)",
                  p.read_text(encoding="utf-8", errors="ignore"))
    return int(m.group(1), 0) if m else None


def declared_signature(tree_chain, method):
    """'ret name(args)' text from the nearest header in the chain declaring ``method``."""
    for tree in tree_chain:
        p = header_for(tree)
        if p is None:
            continue
        text = p.read_text(encoding="utf-8", errors="ignore")
        m = re.search(r"^\s*(?:virtual\s+)?([A-Za-z_][\w:<>\s\*&]*?[\s\*&])" + re.escape(method)
                      + r"\s*\(([^;{]*)\)\s*(?:const\s*)?[;{]", text, re.MULTILINE)
        if m:
            ret = " ".join(m.group(1).split())
            if ret in ("virtual",):
                continue
            return ret, " ".join(m.group(2).split()), tree
    return None


def _count_params(text):
    depth = n = 0
    for ch in text:
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth -= 1
        elif ch == "," and depth == 0:
            n += 1
    return n + 1


def base_chain(ev, rec):
    chain, cur = [], rec["name"]
    seen = set()
    while cur and cur not in seen:
        seen.add(cur)
        prim = next((e for e in ev.bases.get(cur, []) if e["offset"] == 0), None)
        if not prim:
            break
        chain.append(ev.tree_of(prim["base"]))
        cur = prim["base"]
    return chain


def member_decl(sym, tree, chain):
    d = DM.demangle(sym)
    if not d:
        return None
    sig = declared_signature([tree] + chain, d["method"])
    args = ", ".join(d["args"])
    if sig:
        declared = sig[1]
        n_decl = 0 if declared in ("", "void") else _count_params(declared)
        if n_decl == len(d["args"]):
            args = declared
        return f"{sig[0]} {d['method']}({args})", f"declaration from include/{sig[2]}.h"
    return f"void /* RETURN TYPE UNPROVEN */ {d['method']}({args})", None


def render_header(ev, rec, slots, base_n, size_row, run):
    tree = rec["tree"] or rec["name"]
    bases = ev.bases.get(rec["name"], [])
    chain = base_chain(ev, rec)
    base_list = ", ".join(f"{ev.tree_of(b['base'])}" for b in bases) or ""
    guard = re.sub(r"\W", "_", tree).upper() + "_H"
    lines = [f"#ifndef {guard}", f"#define {guard}", ""]
    for b in bases:
        lines.append(f'#include "{ev.tree_of(b["base"])}.h"')
    lines.append("")
    lines.append("/* SCAFFOLD (tools/class_scaffold.py). What the ROM proves:")
    lines.append(f" *   RTTI   {rec['kind']} record _ZTI{rec['mangled']} at {rec['module']}:{rec['addr']}, "
                 f"name _ZTS at {rec['name_addr']}")
    if rec["name"] != tree:
        lines.append(f" *   ROM name {rec['name']}; the tree's name {tree} is coined")
    for b in bases:
        lines.append(f" *   base   {b['base']} (tree {ev.tree_of(b['base'])}) at +0x{b['offset']:x}")
    lines.append(f" *   vtable {rec.get('ztv') or '?'} at {rec.get('vtable')}: {len(slots)} slot(s), "
                 f"{base_n} from the primary base")
    if size_row:
        lines.append(f" *   size   {size_row['size_hex']} from operator new in "
                     f"{', '.join(size_row['factories'])}")
    lines.append(" * Everything else below is marked: a reviewer names it or proves it. */")
    lines.append("")
    lines.append(f"struct {tree}" + (f" : {base_list}" if base_list else "") + " {")
    base_size = header_size(chain[0]) if chain else None
    if size_row and base_size is not None and size_row["size"] > base_size:
        off, n = base_size, size_row["size"] - base_size
        lines.append(f"    u8 unk_{off:03x}[0x{n:x}];   /* 0x{off:03x}..0x{size_row['size']:03x} "
                     "UNPROVEN: no field evidence joined yet */")
        lines.append("")
    elif size_row and base_size is None and chain:
        lines.append(f"    /* fields: {chain[0]} carries no size assertion, so the unk_ span "
                     "cannot be placed */")
        lines.append("")
    dtor = [s for s in slots if s["symbol"] and re.search(r"D[01]Ev$", s["symbol"])
            and s["kind"] != "inherited"]
    if dtor:
        where = ", ".join(f"slot {s['slot']} {s['symbol'][-4:-2]} 0x{s['addr']:08x}" for s in dtor)
        lines.append(f"    virtual ~{tree}() {{}}   /* {where}; inline + `new {tree}` "
                     "emits D1 then D0 (notes/cpp-class-form.md) */")
    for s in slots:
        if s["kind"] == "inherited" or not s["symbol"] or re.search(r"D[012]Ev$", s["symbol"]):
            continue
        decl = member_decl(s["symbol"], tree, chain) if s["symbol"].startswith("_Z") else None
        if decl:
            lines.append(f"    virtual {decl[0]};   /* slot {s['slot']} {s['kind']} 0x{s['addr']:08x} */")
        else:
            lines.append(f"    /* slot {s['slot']} {s['kind']} 0x{s['addr']:08x} {s['symbol']}: "
                         "UNNAMED virtual, name it before declaring */")
    slot_syms = {s["symbol"] for s in slots}
    for _a, n, _s in run:
        if n in slot_syms or not n.startswith("_ZN") or _STRUCTOR_RE.search(n):
            continue
        decl = member_decl(n, tree, chain)
        if decl:
            lines.append(f"    {decl[0]};")
    lines.append("};")
    if size_row:
        lines += ["", "#ifndef SM64DS_PLATFORM_PC",
                  f"typedef char {tree}_size_must_be_{size_row['size_hex']}[",
                  f"    sizeof({tree}) == {size_row['size_hex']} ? 1 : -1];", "#endif"]
    lines += ["", f"#endif /* {guard} */", ""]
    return "\n".join(lines)


# ============================================================================ TU

def render_tu(ev, rec, run, evidence, slots):
    tree = rec["tree"] or rec["name"]
    chain = base_chain(ev, rec)
    lines = ["//cpp", "/* SCAFFOLD (tools/class_scaffold.py) -- not a reconstruction yet.",
             f"   Class {tree} (ROM RTTI {rec['name']}), module {rec['module']}."]
    for e in evidence:
        lines.append(f"   {e}")
    lines += ["   Written ROM-descending: mwccarm emits .text in reverse source order under the",
              "   default deferred codegen, so this file's LAST member is the TU's first function.",
              "   Every hand-written member keeps its unique `// @symbol` line",
              "   (notes/tu-promotion-conventions.md section 6). */",
              f'#include "{tree}.h"', ""]
    slot_of = {s["symbol"]: s for s in slots if s["symbol"]}
    for a, n, s in reversed(run):
        kind, path = owner_kind(n)
        cite = f"current source: {path} ({kind})" if path else "no source in src/"
        slot = slot_of.get(n)
        tag = f", vtable slot {slot['slot']}" if slot else ""
        if _STRUCTOR_RE.search(n) or n.startswith("_ZThn"):
            lines.append(f"/* {n} 0x{a:08x} size 0x{s:x}{tag}: compiler-emitted lifecycle "
                         f"variant, no source text ({cite}) */")
            lines.append("")
            continue
        lines.append(f"// @symbol {n}")
        if n.endswith("_classInit"):
            lines.append(f"/* 0x{a:08x} size 0x{s:x}; factory proven by the actor spawn table */")
            lines.append(f'extern "C" {tree} *{n}()')
            lines.append("{")
            lines.append(f"    return new {tree}();")
        elif n.startswith("_ZN"):
            decl = member_decl(n, tree, chain)
            d = DM.demangle(n)
            sig = decl[0] if decl else f"void {d['method']}()"
            ret, rest = sig.split(f" {d['method']}(", 1) if f" {d['method']}(" in sig else ("void", ")")
            lines.append(f"/* 0x{a:08x} size 0x{s:x}{tag}; {cite} */")
            lines.append(f"{ret} {tree}::{d['method']}({rest}")
            lines.append("{")
            lines.append("    /* TODO reconstruct */")
        else:
            lines.append(f"/* 0x{a:08x} size 0x{s:x}{tag}; address-named, linkage and "
                         f"parameters unproven; {cite} */")
            lines.append(f'extern "C" void {n}(/* TODO */)')
            lines.append("{")
            lines.append("    /* TODO reconstruct */")
        lines.append("}")
        lines.append("")
    return "\n".join(lines)


# ============================================================================ plan

def destination(ev, rec):
    """(promoted_source, reason) per AGENTS.md's bucket table and file-name rule."""
    tree = rec["tree"] or rec["name"]
    stem = TN.candidate_stem(rec["name"]) if tree == rec["name"] else tree
    why_stem = ("NSMBW snake_case of the ROM name (tools/tu_names.candidate_stem)"
                if tree == rec["name"] else
                f"the tree's coined class name {tree} is kept until the class is renamed "
                f"to its ROM name {rec['name']} (AGENTS.md file-name rule)")
    name = rec["name"]
    bucket = None
    if re.match(r"daPly_c$", name):
        bucket = "src/game/player"
    elif re.match(r"(dScMg|dMg|cMg)", name):
        bucket = "src/minigames"
    elif re.match(r"da[A-Z0-9]", name):
        bucket = "src/game/actors"
    elif re.match(r"dSc[A-Z]", name):
        bucket = "src/game/stages"
    why_dir = "AGENTS.md bucket table by class prefix" if bucket else None
    if bucket is None:
        for base in base_chain(ev, rec):
            hits = [n for n, k, _a, _s in module_symbols("arm9") + module_symbols(rec["module"])
                    if k == "function" and SP.class_of(n) == base]
            for n in hits:
                kind, path = owner_kind(n)
                if kind == "tu" and path and path.startswith("src/"):
                    bucket = str(pathlib.PurePosixPath(path).parent)
                    why_dir = f"directory of the base class {base}'s promoted TU ({path})"
                    break
            if bucket:
                break
    if bucket is None:
        return None, f"stem {stem}.cpp ({why_stem}); directory UNDECIDED -- pick one per " \
                     "notes/src-domain-buckets.md"
    return f"{bucket}/{stem}.cpp", f"{why_dir}; {why_stem}"


def build_plan(name, exclude_lifecycle=False, out=None):
    ev = Evidence()
    rec = ev.find(name)
    slots, base_n = classify_slots(ev, rec)
    run, evidence, warnings, structors = member_run(ev, rec, slots, exclude_lifecycle)
    tree = rec["tree"] or rec["name"]
    size_row = ev.size_of(rec)
    dest, why_dest = destination(ev, rec)
    module = "arm9" if rec["module"] == "main" else rec["module"]
    rows = []
    for o, (a, n, s) in enumerate(run):
        kind, path = owner_kind(n)
        rows.append({"symbol": n, "address": f"0x{a:08x}", "size": f"0x{s:08x}",
                     "legacy_source": path, "ordinal": o, "owner": kind,
                     "promoted_in": (promoted_owner(module, a) or (None,))[0]})
    tu_id = f"{module}/{tree}"
    src_rel = f"src_tu/{module}/{tree}.cpp"
    chain_kinds = [ev.records[b]["kind"] for b in _rtti_chain(ev, rec) if b in ev.records]
    lifecycle = lifecycle_note(rec, slots, run, structors, "vmi" in chain_kinds)
    entry = {
        "id": tu_id, "module": module, "source": src_rel, "promoted_source": dest,
        "status": "shadow",
        # A tu_map unit carries tu_map's own boundary judgement; a class run is only
        # "first to last function named for the class" until a reviewer reads the
        # neighbours, so it starts low.
        "boundary_confidence": "medium" if evidence[0].startswith("contiguous linker run")
        else "low",
        "boundary_evidence": evidence + [
            f"ROM class identity: _ZTI{rec['mangled']} at {rec['addr']}, _ZTS at {rec['name_addr']}"
            + (f", vtable {rec['ztv']} at {rec['vtable']}" if rec.get("ztv") else "")],
        "sections": [{"name": ".text", "start": rows[0]["address"],
                      "end": f"0x{run[-1][0] + run[-1][2]:08x}"}] if rows else [],
        "functions": [{k: r[k] for k in ("symbol", "address", "size", "legacy_source", "ordinal")}
                      for r in rows],
        "data": [], "bss": [],
        "notes": ["Generated by tools/class_scaffold.py from RTTI, the ROM vtable, tu_map and "
                  "srcpath; boundary, lifecycle form and every compiler-only output still need "
                  "tubuild verify and a reviewer.", lifecycle] + [f"scaffold warning: {w}" for w in warnings],
    }
    already = sorted({r["promoted_in"] for r in rows if r["promoted_in"]})
    plan = {
        "class": rec["name"], "tree_name": tree, "module": module, "record": {
            k: rec.get(k) for k in ("kind", "addr", "name_addr", "vtable", "vtable_module", "ztv", "mangled")},
        "bases": [{"base": b["base"], "tree": ev.tree_of(b["base"]), "offset": b["offset"]}
                  for b in ev.bases.get(rec["name"], [])],
        "size": size_row and {"size": size_row["size_hex"], "factories": size_row["factories"]},
        "header_size": (lambda hs: hs and f"0x{hs:x}")(header_size(tree)),
        "vtable": [{**s, "addr": f"0x{s['addr']:08x}"} for s in slots], "base_slots": base_n,
        "members": rows, "lifecycle": lifecycle, "warnings": warnings,
        "already_in_manifest": already, "destination": dest, "destination_reason": why_dest,
        "manifest_entry": entry,
        "files": {},
    }
    out_dir = pathlib.Path(out) if out else None
    hdr_existing = header_for(tree)
    hdr_text = render_header(ev, rec, slots, base_n, size_row, run)
    tu_text = render_tu(ev, rec, run, evidence, slots)
    if out_dir:
        plan["files"] = {str(out_dir / f"{tree}.h"): hdr_text, str(out_dir / f"{tree}.cpp"): tu_text,
                         str(out_dir / f"{tree}.json"): json.dumps(entry, indent=2) + "\n"}
    else:
        hdr_path = (BUILD / "scaffold" / tree / f"{tree}.h") if hdr_existing else (INCLUDE / f"{tree}.h")
        plan["files"] = {str(hdr_path): hdr_text, str(REPO / src_rel): tu_text}
        plan["manifest_path"] = str(TUM.entry_path(tu_id))
    plan["existing_header"] = hdr_existing and hdr_existing.relative_to(REPO).as_posix()
    return plan, entry


def _rtti_chain(ev, rec):
    out, cur = [], rec["name"]
    while True:
        prim = next((e for e in ev.bases.get(cur, []) if e["offset"] == 0), None)
        if not prim or prim["base"] in out:
            return out
        out.append(prim["base"])
        cur = prim["base"]


def lifecycle_note(rec, slots, run, structors, base_mi=False):
    own_virtual = [s for s in slots if s["kind"] != "inherited" and s["symbol"]
                   and not re.search(r"D[012]Ev$", s["symbol"])]
    in_run = {n for _a, n, _s in run}
    key = next((s["symbol"] for s in own_virtual if s["symbol"] in in_run), None)
    has_d = any(s["symbol"] and re.search(r"D[01]Ev$", s["symbol"]) and s["kind"] != "inherited"
                for s in slots)
    mi = rec["kind"] == "vmi" or base_mi or any(n.startswith("_ZThn") for n in structors)
    if key:
        text = (f"key function candidate {key}: an out-of-line definition of it in this TU emits "
                f"{rec.get('ztv') or '_ZTV'} here, so the vtable group is this TU's output to license")
    elif has_d:
        text = ("no own non-destructor virtual in the run: the destructor is the key function. Inline "
                "`virtual ~X() {}` plus a `new X` in the TU emits D1 then D0 and the vtable; without "
                "an instantiation the TU emits neither, which is the method-run-only shape")
    else:
        text = "the class overrides nothing: no vtable is emitted by this TU"
    if mi:
        text += ("; the class is multiply inherited (itself or a base), so its destructors carry "
                 "_ZThn thunks; the promoted MI precedent "
                 "(arm9/ModelAnim) keeps the lifecycle row as shards because the secondary table's "
                 "preamble cannot be licensed")
    return text


# ============================================================================ main

def write_files(plan, entry, out):
    written = []
    for path, text in plan["files"].items():
        p = pathlib.Path(path)
        p.parent.mkdir(parents=True, exist_ok=True)
        if p.exists() and not out:
            raise SystemExit(f"{p.relative_to(REPO).as_posix()} exists; refusing to overwrite")
        p.write_text(text, encoding="utf-8", newline="\n")
        written.append(p)
    if not out:
        # One file, not TUM.save(): save() re-serializes every entry, and ten tracked
        # entries on main are not in its canonical form (missing final newline, key
        # order), so a whole-manifest save turns a one-entry scaffold into a
        # ten-file diff. tubuild verify's save has the same side effect.
        if any(e.get("id") == entry["id"] for e in TUM.load()["entries"]):
            raise SystemExit(f"manifest entry {entry['id']} exists; refusing to overwrite")
        path = pathlib.Path(plan["manifest_path"])
        path.parent.mkdir(parents=True, exist_ok=True)
        TUM._dump(path, entry)
        written.append(path)
    return written


def print_plan(plan):
    p = plan
    print(f"class      {p['class']}  (tree name {p['tree_name']}, module {p['module']})")
    r = p["record"]
    print(f"RTTI       {r['kind']} _ZTI{r['mangled']} at {r['addr']}, vtable {r['ztv'] or '?'} at {r['vtable']}")
    for b in p["bases"]:
        print(f"base       {b['base']} (tree {b['tree']}) at +0x{b['offset']:x}")
    print(f"size       {p['size']['size'] + ' from ' + ', '.join(p['size']['factories']) if p['size'] else 'not ROM-proven (no operator new site)'}"
          + (f"; include/{p['tree_name']}.h asserts {p['header_size']}" if p.get('header_size') else ""))
    print(f"vtable     {len(p['vtable'])} slot(s), {p['base_slots']} from the primary base")
    for s in p["vtable"]:
        if s["kind"] != "inherited":
            print(f"  slot {s['slot']:2d} {s['kind']:9} {s['addr']}  {s['symbol']}")
    print(f"members    {len(p['members'])} function(s) licensed")
    for m in p["members"]:
        tag = f"  [promoted in {m['promoted_in']}]" if m["promoted_in"] else ""
        print(f"  {m['address']} {m['size']}  {m['symbol']:52} {m['owner']:6} {m['legacy_source'] or '-'}{tag}")
    print(f"lifecycle  {p['lifecycle']}")
    for w in p["warnings"]:
        print(f"WARNING    {w}")
    if p["already_in_manifest"]:
        print(f"NOTE       already licensed by manifest entry {', '.join(p['already_in_manifest'])}: "
              "the scaffold is for comparison, not for a new entry")
    print(f"dest       {p['destination'] or '-'}  ({p['destination_reason']})")
    if p["existing_header"]:
        print(f"header     {p['existing_header']} exists and is kept; the scaffold header goes beside it for a diff")
    print("files      " + "\n           ".join(
        pathlib.Path(f).relative_to(REPO).as_posix() if pathlib.Path(f).is_relative_to(REPO) else f
        for f in p["files"]))
    if p.get("manifest_path"):
        print(f"manifest   {pathlib.Path(p['manifest_path']).relative_to(REPO).as_posix()}")


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cls", help="RTTI class name (daBar_c) or the tree's name for it (ModelAnim2)")
    ap.add_argument("--write", action="store_true", help="write the files (default: dry run)")
    ap.add_argument("--out", help="write every file into this directory instead of the tree")
    ap.add_argument("--exclude-lifecycle", action="store_true",
                    help="license the method run only; structors and thunks stay shards")
    ap.add_argument("--json", action="store_true", help="print the evidence join as JSON")
    ap.add_argument("--show", action="store_true", help="print the generated files")
    a = ap.parse_args(argv)
    plan, entry = build_plan(a.cls, a.exclude_lifecycle, a.out)
    if a.json:
        print(json.dumps({k: v for k, v in plan.items() if k != "files"}, indent=2))
    else:
        print_plan(plan)
    if a.show:
        for path, text in plan["files"].items():
            print(f"\n===== {path}\n{text}")
    if a.write:
        if plan["already_in_manifest"] and not a.out:
            raise SystemExit("refusing to write into the tree: the members are already licensed "
                             "(use --out DIR to compare)")
        for p in write_files(plan, entry, a.out):
            print(f"wrote {p}")
    elif not a.json:
        print("\n(dry run: nothing written; pass --write)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
