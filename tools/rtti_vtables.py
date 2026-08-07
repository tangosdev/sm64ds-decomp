#!/usr/bin/env python3
"""Pass 5: read each class's vtable, and say which slots are its own code.

notes/rtti-reconciliation.md section 6 ends on a wall: RTTI proves 10 intermediate
classes exist and proves their edges, but nothing in include/ carries a field that
belongs to them, so they cannot be given a header.  Populating them needs matched
functions that touch them.

This pass finds those functions.  A class's vtable is the one place the ROM lists its
methods, and a slot where a class differs from its base is that class's own override.
So diffing a vtable against its base's vtable yields exactly the set of functions that
belong to the class and to no ancestor -- the code whose field accesses would give the
class a layout.

Reading a vtable
----------------
    V-8   offset-to-top   (0 for a primary table, negative for a secondary one)
    V-4   typeinfo ptr
    V+0.. slots

Where the table ends is the only subtle part, and two rules that look reasonable are
both wrong:

  * "stop at the next typeinfo record" -- vtables are not adjacent to records; this
    gave fBase_c 521 slots against a documented 18.
  * "stop at the first word that is not a code pointer" -- a pure-virtual slot is a
    zero, so this stopped at slot 0 for every abstract class.  All six intermediates
    with vtables are abstract, so this silently reported that none of them had any
    methods at all, which is the exact opposite of the truth.

The rule that holds is the one notes/model-rtti-names.md already states: a zero
followed by a pointer to a typeinfo record is not an empty slot, it is the
offset-to-top word of the *next* vtable.  So: a slot may be code or may be null; the
table ends at a null whose successor is a record pointer, or at any other non-code
word.

Validated against the documented counts: fBase_c (= the tree's ActorBase) reads 18
slots, matching the 18-slot virtual order in notes/cpp-naming-guide.md, and Animation
reads 2, matching notes/model-rtti-names.md.

Usage
-----
    python tools/rtti_vtables.py --report            # every class, slot counts
    python tools/rtti_vtables.py --own <ClassName>   # slots this class overrides
    python tools/rtti_vtables.py --intermediates     # the 10 recovered classes
"""

from __future__ import annotations

import argparse
import collections
import json
import os
import pathlib
import re
import struct
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
RTTI = REPO / "build" / "rtti.json"
ARM9 = REPO / "extracted" / "arm9_dec.bin"
OVDIR = REPO / "extracted" / "dsd" / "arm9_overlays"
SEC = re.compile(r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\s+kind:(\S+)")
SYM = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-fA-F]+)")
MAX_SLOTS = 96


def load_sections(path):
    out = []
    if not path.is_file():
        return out
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = SEC.match(line)
        if m:
            out.append((int(m.group(2), 16), int(m.group(3), 16), m.group(4)))
        elif line and not line[0].isspace():
            break
    return out


class Mod:
    __slots__ = ("name", "base", "data", "sections")

    def __init__(self, name, base, data, sections):
        self.name, self.base, self.data, self.sections = name, base, data, sections

    def has(self, va):
        return self.base <= va < self.base + len(self.data)

    def word(self, va):
        return struct.unpack_from("<I", self.data, va - self.base)[0]

    def is_code(self, va):
        return any(s <= va < e and k == "code" for s, e, k in self.sections)


def load_modules():
    import yaml
    cfg9 = REPO / "config" / "arm9"
    mods = {"arm9": Mod("arm9", 0x02004000, ARM9.read_bytes(),
                        load_sections(cfg9 / "delinks.txt"))}
    cfg = yaml.safe_load((OVDIR / "overlays.yaml").read_text(encoding="utf-8"))
    for ov in cfg["overlays"]:
        p = OVDIR / ov["file_name"]
        if not p.is_file():
            continue
        n = "ov%03d" % ov["id"]
        mods[n] = Mod(n, ov["base_address"], p.read_bytes(),
                      load_sections(cfg9 / "overlays" / n / "delinks.txt"))
    return mods


def load_symbol_names():
    out = collections.defaultdict(dict)
    for p in (REPO / "config").rglob("symbols.txt"):
        mod = p.parent.name
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines():
            m = SYM.match(line.strip())
            if m:
                out[mod].setdefault(int(m.group(3), 16), m.group(1))
    return out


def read_slots(mods, mod_name, vt, record_addrs):
    """Slots of the vtable at `vt`.  None entries are pure-virtual."""
    m = mods[mod_name]
    a9 = mods["arm9"]
    out = []
    for i in range(MAX_SLOTS):
        a = vt + 4 * i
        if not m.has(a):
            break
        w = m.word(a)
        if w == 0:
            # a zero whose successor points at a typeinfo record is the
            # offset-to-top of the NEXT vtable, not a slot of this one
            nxt = m.word(a + 4) if m.has(a + 4) else 0
            if nxt in record_addrs:
                break
            out.append(None)
            continue
        if m.is_code(w) or a9.is_code(w):
            out.append(w)
            continue
        break
    while out and out[-1] is None:
        out.pop()                    # trailing nulls belong to no slot we can prove
    return out


def build():
    data = json.loads(RTTI.read_text(encoding="utf-8"))
    mods = load_modules()
    syms = load_symbol_names()
    recs = data["records"]
    parent = {}
    for e in data["edges"]:
        if e.get("offset", 0) == 0 or e["derived"] not in parent:
            parent[e["derived"]] = e["base"]
    by_name = {v["name"]: v for v in recs.values()}
    rec_addrs = collections.defaultdict(set)
    for v in recs.values():
        rec_addrs[v["module"]].add(int(v["addr"], 16))

    tables = {}
    for v in recs.values():
        if not v["vtable"]:
            continue
        # The vtable is not always in the record's module: four classes keep
        # theirs in a different, non-overlapping overlay (see attach_vtables in
        # rtti_extract.py).  Reading from the record's module returned zero slots
        # for all four, which read as "this class has no methods".
        vmod = v.get("vtable_module") or v["module"]
        tables[v["name"]] = {
            "module": v["module"],
            "vtable_module": vmod,
            "vtable": v["vtable"],
            "slots": read_slots(mods, vmod, int(v["vtable"], 16),
                                rec_addrs[vmod]),
        }

    def name_of(mod, addr):
        return syms.get(mod, {}).get(addr) or syms.get("arm9", {}).get(addr)

    for cls, t in tables.items():
        p = parent.get(cls)
        pt = tables.get(p)
        own = []
        for i, s in enumerate(t["slots"]):
            if s is None:
                continue
            inherited = pt and i < len(pt["slots"]) and pt["slots"][i] == s
            if not inherited:
                own.append({"slot": i, "addr": "0x%08x" % s,
                            "symbol": name_of(t["vtable_module"], s)})
        t["parent"] = p
        t["parent_slots"] = len(pt["slots"]) if pt else None
        t["own"] = own
    return tables, parent, by_name


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--report", action="store_true")
    ap.add_argument("--own", metavar="CLASS")
    ap.add_argument("--intermediates", action="store_true")
    ap.add_argument("--fields", action="store_true",
                    help="recover each intermediate's own fields from its own methods")
    ap.add_argument("--emit-headers", action="store_true",
                    help="write include/<Class>.h for every intermediate with fields")
    ap.add_argument("--out", default=str(REPO / "build" / "rtti_vtables.json"))
    a = ap.parse_args()

    tables, parent, by_name = build()
    pathlib.Path(a.out).write_text(json.dumps(tables, indent=1) + "\n",
                                   encoding="utf-8", newline="\n")

    # self-check against the two counts the notes document
    checks = [("fBase_c", 18, "notes/cpp-naming-guide.md 18-slot virtual order"),
              ("dExtFrameCtrl_c", 2, "notes/model-rtti-names.md Animation")]
    bad = 0
    for cls, want, why in checks:
        got = len(tables.get(cls, {}).get("slots", []))
        if got != want:
            print("SELF-CHECK FAIL %s: %d slots, expected %d (%s)" % (cls, got, want, why))
            bad += 1
    if bad:
        return 1

    if a.report:
        print("%-24s %-8s %-10s %5s %5s %5s" % ("class", "module", "vtable",
                                                "slots", "base", "own"))
        for cls in sorted(tables):
            t = tables[cls]
            print("%-24s %-8s %-10s %5d %5s %5d"
                  % (cls, t["module"], t["vtable"], len(t["slots"]),
                     t["parent_slots"] if t["parent_slots"] is not None else "-",
                     len(t["own"])))

    if a.own:
        t = tables.get(a.own)
        if not t:
            print("no vtable for %s" % a.own)
            return 1
        print("%s : %s   %d slots (base %s)"
              % (a.own, t["parent"], len(t["slots"]), t["parent_slots"]))
        for o in t["own"]:
            print("  slot %-3d %s  %s" % (o["slot"], o["addr"], o["symbol"] or "(unnamed)"))

    if a.intermediates:
        rc = json.loads((REPO / "build" / "rtti_reconcile.json").read_text(encoding="utf-8"))
        inter = sorted({r["rom_bases"][0] for r in rc["rows"]
                        if r["verdict"] == "flattened"})
        print("%-22s %5s %5s %5s  own slots (function -> is it matched in src/?)"
              % ("intermediate", "slots", "base", "own"))
        srcdir = REPO / "src"
        have = {p.stem for p in srcdir.rglob("*") if p.suffix in (".c", ".cpp")}
        tot = collections.Counter()
        for cls in inter:
            t = tables.get(cls)
            if not t:
                print("%-22s   -- no vtable --" % cls)
                tot["no_vtable"] += 1
                continue
            names = []
            for o in t["own"]:
                s = o["symbol"] or "?"
                names.append("%s%s" % (s, "*" if s in have else ""))
                tot["own_total"] += 1
                tot["own_matched" if s in have else "own_unmatched"] += 1
            print("%-22s %5d %5s %5d  %s"
                  % (cls, len(t["slots"]), t["parent_slots"], len(t["own"]),
                     ", ".join(names) or "(none)"))
        print("\n  * = a src/ file with that name already exists (already matched)")
        for k in sorted(tot):
            print("  %-16s %d" % (k, tot[k]))

    if a.fields:
        print(recover_fields(tables))

    if a.emit_headers:
        print(recover_fields(tables))       # refresh the evidence file first
        print()
        print(emit_headers(tables))
    return 0


WIDTH_TYPE = {1: "u8", 2: "s16", 4: "s32"}


def emit_headers(tables):
    """One header per intermediate that has recovered fields.

    Deliberately FLAT, in the style of the 241 bannered headers, not an inheriting
    C++ struct.  A derived struct cannot place a field at an absolute offset --
    you would have to know sizeof(base) to compute the leading padding, and this
    pass does not know it.  Padding from zero states only what was observed and
    leaves the base relationship to the comment, which is where the evidence for
    it lives anyway.

    Every field cites the matched function it was read from.  Nothing here is
    inferred from a name or a neighbour.
    """
    data = json.loads((REPO / "build" / "rtti_intermediate_fields.json")
                      .read_text(encoding="utf-8"))
    rtti = json.loads(RTTI.read_text(encoding="utf-8"))
    by_name = {v["name"]: v for v in rtti["records"].values()}
    parent = {}
    for e in rtti["edges"]:
        if e.get("offset", 0) == 0 or e["derived"] not in parent:
            parent[e["derived"]] = e["base"]
    kids = collections.defaultdict(list)
    for e in rtti["edges"]:
        kids[e["base"]].append(e["derived"])

    written = []
    for cls, info in sorted(data.items()):
        fields = info.get("fields") or {}
        if not fields:
            continue
        rec = by_name[cls]
        t = tables[cls]
        offs = sorted(int(k, 16) for k in fields)
        guard = re.sub(r"\W", "_", cls).upper() + "_H"

        lines = []
        lines.append("#ifndef %s" % guard)
        lines.append("#define %s" % guard)
        lines.append("")
        lines.append('#include "types.h"')
        lines.append("")
        lines.append("/* %s -- an intermediate class the ROM's RTTI names and the tree" % cls)
        lines.append(" * did not.  Base: %s (the tree calls it Platform)." % parent.get(cls, "?"))
        lines.append(" *")
        vmod = t.get("vtable_module") or rec["module"]
        lines.append(" * typeinfo %s (%s), vtable %s (%s), %d slots (base has %s)."
                     % (rec["addr"], rec["module"], rec["vtable"], vmod,
                        len(t["slots"]), t["parent_slots"]))
        if vmod != rec["module"]:
            lines.append(" * Note the vtable is in a DIFFERENT overlay from the typeinfo.")
        nulls = [i for i, s in enumerate(t["slots"]) if s is None]
        lines.append(" * Abstract -- pure-virtual (null) slots: %s"
                     % (", ".join(str(i) for i in nulls) if nulls else "none"))
        lines.append(" * Own overrides at slots: %s"
                     % ", ".join(str(o["slot"]) for o in t["own"]))
        lines.append(" * %d known descendant(s): %s"
                     % (len(kids.get(cls, [])), ", ".join(sorted(kids.get(cls, []))[:6])))
        lines.append(" *")
        lines.append(" * The fields below are the offsets this class's OWN vtable overrides")
        lines.append(" * touch that no named ancestor declares.  A method of a class can")
        lines.append(" * reach its own members and its ancestors', never a descendant's, so")
        lines.append(" * an offset seen here and owned by no ancestor belongs to this class.")
        lines.append(" * Read from these byte-matched functions:")
        for m in info["methods"]:
            lines.append(" *   %s" % m)
        lines.append(" *")
        lines.append(" * The space below 0x%x is left as padding.  It is NOT all the base's:"
                     % offs[0])
        lines.append(" * this class's own subobjects live in there too -- daObjDorifu_c's")
        lines.append(" * destructor destroys a Model[5] at 0x320 and a MeshCollider[5] at")
        lines.append(" * 0x4b0, both its own.  Padding means unobserved, not inherited.")
        lines.append(" * The struct is flat like the rest of the generated corpus rather than")
        lines.append(" * inheriting because this pass knows offsets, not sizeof(base).")
        lines.append(" * Regenerate: python tools/rtti_vtables.py --emit-headers */")
        lines.append("")
        lines.append("struct %s {" % cls)
        cur = 0
        for o in offs:
            w = sorted(fields["0x%x" % o]["widths"], key=lambda x: -int(x))
            width = int(w[0])
            if o > cur:
                lines.append("    u8  pad_%03x[0x%x];" % (cur, o - cur))
            lines.append("    %-3s unk_%03x;%s/* 0x%03x */"
                         % (WIDTH_TYPE.get(width, "u8"), o, " " * 12, o))
            cur = o + width
        lines.append("};")
        lines.append("")
        lines.append("#endif")
        text = "\n".join(lines) + "\n"
        p = REPO / "include" / ("%s.h" % cls)
        p.write_text(text, encoding="utf-8", newline="\n")
        written.append("%s  (%d field(s), first at 0x%x)" % (p.name, len(offs), offs[0]))
    return "wrote:\n  " + "\n  ".join(written)


def recover_fields(tables):
    """Each intermediate's own fields, read out of its own already-matched methods.

    Sound because of what a method can reach: a method of class C can touch C's
    members and its ancestors', never a descendant's.  So a `this`-relative offset
    seen in one of C's own overrides, which no named ancestor declares, belongs to
    C.  Verified that no own-slot function is listed by a non-descendant class, so
    identical-code folding is not silently reattributing anything.

    Offset 0 is dropped: `this[0] = vtable` is the vptr store, not a field.
    """
    sys.path.insert(0, str(REPO / "tools"))
    import evidence_hierarchy as EH

    rc = json.loads((REPO / "build" / "rtti_reconcile.json").read_text(encoding="utf-8"))
    inter = sorted({r["rom_bases"][0] for r in rc["rows"] if r["verdict"] == "flattened"})
    idx = {p.stem: p for p in (REPO / "src").rglob("*") if p.suffix in (".c", ".cpp")}

    fld = re.compile(r"^\s*(\S+)\s+(\w+)\s*(\[[^\]]*\])?\s*;\s*/\*\s*(0x[0-9a-fA-F]+)")
    owned = {}
    for n in ("Platform", "Actor", "ActorBase", "ActorDerived", "Enemy"):
        p = REPO / "include" / (n + ".h")
        if not p.is_file():
            continue
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines():
            m = fld.match(line)
            if m and not m.group(2).startswith("pad"):
                owned.setdefault(int(m.group(4), 16), n)

    out, lines = {}, []
    for cls in inter:
        t = tables.get(cls)
        if not t:
            lines.append("%-20s -- no vtable, no methods to read --" % cls)
            continue
        acc = collections.defaultdict(collections.Counter)
        fns = []
        for o in t["own"]:
            p = idx.get(o["symbol"] or "")
            if not p:
                continue
            fns.append(o["symbol"])
            for off, w, _ty, _ln in EH.scan_raw_offsets(
                    p.read_text(encoding="utf-8", errors="replace")):
                if off:
                    acc[off][w] += 1
        novel = {o: v for o, v in acc.items() if o not in owned}
        out[cls] = {"methods": fns,
                    "fields": {"0x%x" % o: {"widths": dict(v)} for o, v in sorted(novel.items())}}
        lines.append("%-20s %2d method(s), %2d own field(s)" % (cls, len(fns), len(novel)))
        for o in sorted(novel):
            w = novel[o]
            lines.append("      0x%-6x width %s%s" % (
                o, "/".join("%d(x%d)" % (k, c) for k, c in sorted(w.items())),
                "   <-- INCONSISTENT" if len(w) > 1 else ""))
    (REPO / "build" / "rtti_intermediate_fields.json").write_text(
        json.dumps(out, indent=1) + "\n", encoding="utf-8", newline="\n")
    return "\n".join(lines)


if __name__ == "__main__":
    sys.exit(main())
