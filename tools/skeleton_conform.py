#!/usr/bin/env python3
"""Compile a C++ class skeleton and byte-compare its RTTI output against the ROM.

## Why this exists, and why it is not the same question as matching a function

`notes/runbook-type-reconstruction.md` 2 establishes that field types are mostly
invisible: a retype that nothing reads emits identical bytes, so the ROM cannot
adjudicate it.  71 retypes were A/B'd over 119 translation units and 117/117 came out
byte-identical under `s32/s16` and `u32/u16` alike.  The ROM is a weak oracle there.

The **skeleton** is the opposite case.  Inheritance and virtual dispatch are not
inferred from code -- the compiler emits them as DATA, and that data is in the ROM:

    _ZTS<C>   the name string                     -- literal bytes, no relocations
    _ZTI<C>   typeinfo: kind, name ptr, base ptr  -- for vmi, base count and
                                                     offset_flags are literals too
    _ZTV<C>   offset-to-top, typeinfo ptr, then one word per virtual in declaration
              order, null for a pure virtual

None of that depends on `sizeof` for a single-inheritance class, so a shadow struct
with nothing but padding emits the same table as the real one.  That makes the
skeleton independently checkable, and this tool is the check.

## What is compared, and what is deliberately wildcarded

A word holding a relocation cannot be compared: in a `.o` it is an addend against an
undefined symbol, in the ROM it is a resolved address.  `tools/match.py` has the same
problem for code and solves it the same way -- reloc slots are wildcards.  So:

| word                          | compared |
|-------------------------------|----------|
| `_ZTS` string bytes           | EXACT -- the strongest single check here |
| vtable offset-to-top          | EXACT |
| vtable length / slot count    | EXACT |
| a pure-virtual (null) slot    | EXACT -- and see the note below |
| vmi base count, offset_flags  | EXACT |
| any pointer word              | wildcard (reloc) |

That is not a weakened test.  Slot COUNT, slot ORDER and WHICH slots are pure are
precisely the facts a C++ declaration encodes, and they are all literals.  Whether the
pointer words hold the right functions is already answered, by a different join:
`tools/rtti_reference.py` walks every slot out to a symbol and then to
`build/eligible-names.txt`, and reports 9351/9351 named, 8868 enrolled.

**The open question this tool answers first.**  `notes/rtti-reconciliation.md` 8 records
that the ROM stores a plain zero in a pure-virtual slot.  Whether mwccarm emits a zero
or a pointer to `__cxa_pure_virtual` for `= 0` was never established, and it decides
whether abstract classes conform out of the box.  Run `--cls Fader`: it has 10 slots
with 2-9 null, so it answers the question by itself.

## What conformance does NOT claim

Nothing about fields.  A conforming skeleton says the inheritance edge, the virtual
count, the ordering and the pure-ness are right.  It says nothing about what the object
holds between its vptr and its end, and `notes/rtti-reconciliation.md` 6 is the standing
warning against reading more into a structural result than it carries.

It also does not land anything.  `eligible.py` rejects an object with a `.data` section
outright (85 files today), so a conforming skeleton still cannot be enrolled --
`notes/dtor-variant-audit.md` 7 has the two independent causes.  This tool is a
verifier, not a migration: it tells you the declaration is right, which is exactly the
thing that was previously unknowable.

Usage
-----
    python tools/skeleton_conform.py --cls Fader          # one class, verbose
    python tools/skeleton_conform.py --cls Fader --keep   # leave the probe .cpp behind
    python tools/skeleton_conform.py --report             # every reachable class
    python tools/skeleton_conform.py --report --limit 40
"""

from __future__ import annotations

import argparse
import collections
import io
import json
import pathlib
import struct
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

RTTI = REPO / "build" / "rtti.json"
VTABLES = REPO / "build" / "rtti_vtables.json"
OUT = REPO / "build" / "skeleton_conform.json"

try:
    from elftools.elf.elffile import ELFFile
except ImportError:                                        # pragma: no cover
    sys.exit("pyelftools not installed. Run: pip install pyelftools")

import match as M                                          # compile_c, MW
import modules as MOD                                      # module registry / ROM bytes
import rombuild as RB                                      # the pinned version and flags
import rtti_reference as RR                                # suspect_tail: the shared defect test


# --------------------------------------------------------------------------
# ROM side
# --------------------------------------------------------------------------

_bincache: dict = {}
_modcache: dict | None = None


def mod_for(name):
    """dsd's module name -> the registry entry, with `arm9` aliased to `main`.

    tools/reverify_corpus.py has this same alias and the same reason: matched.jsonl and
    build/rtti.json both say `arm9` while modules.py calls the image `main`.
    """
    global _modcache
    if _modcache is None:
        _modcache = {m["name"]: m for m in MOD.modules()}
        if "main" in _modcache:
            _modcache["arm9"] = _modcache["main"]
    return _modcache.get(name)


def rom_bytes(modname, addr, size):
    m = mod_for(modname)
    if m is None:
        return None
    if modname not in _bincache:
        _bincache[modname] = m["bin"].read_bytes()
    off = addr - m["base"]
    if off < 0 or off + size > len(_bincache[modname]):
        return None
    return _bincache[modname][off:off + size]


def rom_cstring(modname, addr, cap=256):
    b = rom_bytes(modname, addr, cap)
    if b is None:
        return None
    end = b.find(b"\0")
    return b[:end] if end >= 0 else None


# --------------------------------------------------------------------------
# the probe
# --------------------------------------------------------------------------

PROBE_HEAD = """\
/* GENERATED by tools/skeleton_conform.py -- a probe, never committed.
 *
 * The struct bodies are deliberately EMPTY. For a single-inheritance class none of
 * _ZTV / _ZTI / _ZTS depends on sizeof, so padding would change nothing that is being
 * compared here, and inventing fields would imply knowledge this tool does not have.
 */
"""


def decl_name(cls):
    """`mHeap::Heap_t` -> ('Heap_t', ['mHeap']) -- leaf name and enclosing scopes."""
    parts = cls.split("::")
    return parts[-1], parts[:-1]


def probe_source(chain, slots_by_class):
    """C++ for the ancestor chain, most-base first, then the key-function definition.

    Every virtual is declared in slot order and named `sN`, NOT after the ROM symbol.
    Naming them correctly would make the vtable words comparable, but it also makes the
    probe depend on a method-name recovery this tool does not have and cannot fake -- a
    slot under a `func_*` placeholder has no method name anywhere. Slot words are
    wildcarded instead, and what is compared stays honest: count, order, pure-ness.
    """
    out = [PROBE_HEAD]
    prev = None
    for cls in chain:
        n_total = slots_by_class[cls]["count"]
        pure = slots_by_class[cls]["pure"]
        n_inherited = slots_by_class[cls]["inherited"]
        leafname, outers = decl_name(cls)
        for o in outers:
            out.append("struct %s {" % o)
        base = " : public %s" % prev if prev else ""
        out.append("struct %s%s {" % (leafname, base))
        for i in range(n_inherited, n_total):
            # `= 0` only where the ROM has a null word. This was the open question for
            # abstract classes and the tool answered it: mwccarm emits a plain zero, the
            # same word the ROM stores, with no relocation and no __cxa_pure_virtual.
            suffix = " = 0" if i in pure else ""
            out.append("    virtual void s%d()%s;" % (i, suffix))
        # Redeclare every INHERITED slot this class overrides. Two reasons, and the
        # second was found the hard way. First, a class that adds no new virtual has
        # nothing to define as a key function, so without this it is unprobeable at all.
        # Second, where the base declares a slot pure and the derived class overrides it,
        # not redeclaring leaves the slot pure and the probe emits a literal zero against
        # a real pointer in the ROM -- dMgTrmpln2Mario_c and dMgTrmpln3DMario_c failed
        # exactly that way. An override takes its base's slot and adds none, so
        # redeclaring is free; include/Actor.h states the same property.
        for i in slots_by_class[cls].get("overrides", ()):
            if i < n_inherited:
                out.append("    virtual void s%d();" % i)
        out.append("};")
        for _ in outers:
            out.append("};")
        prev = cls
    leaf = chain[-1]
    info = slots_by_class[leaf]
    first_own = min((i for i in range(info["inherited"], info["count"])
                     if i not in info["pure"]), default=None)
    if first_own is None:
        first_own = info.get("key_override")
    if first_own is None:
        return None, ("no definable key function -- every own slot is pure and the class "
                      "overrides nothing")
    out.append("")
    out.append("void %s::s%d() {}" % (leaf, first_own))
    out.append("")
    return "\n".join(out), None


# --------------------------------------------------------------------------
# object side
# --------------------------------------------------------------------------

def sections_of(obj: bytes):
    """symbol name -> (bytes, {reloc offsets relative to the symbol}) for data symbols."""
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    out = {}
    if symtab is None:
        return out
    for sym in symtab.iter_symbols():
        if not sym.name or sym["st_shndx"] in ("SHN_UNDEF", "SHN_ABS"):
            continue
        if sym["st_info"]["type"] != "STT_OBJECT":
            continue
        sec = elf.get_section(sym["st_shndx"])
        start, size = sym["st_value"], sym["st_size"]
        if not size:
            continue
        data = sec.data()[start:start + size]
        relocs = set()
        # By sh_info, never by name: mwccarm names every emitted section the same, so a
        # name lookup returns some other symbol's relocations. match.py:156 records the
        # same trap for .text and it bites identically here.
        rel = next((s for s in elf.iter_sections()
                    if s.header["sh_type"] in ("SHT_REL", "SHT_RELA")
                    and s.header["sh_info"] == sym["st_shndx"]), None)
        if rel is not None:
            for r in rel.iter_relocations():
                o = r["r_offset"] - start
                if 0 <= o < size:
                    relocs.add(o & ~3)
        out[sym.name] = (data, relocs)
    return out


def compare_words(rom: bytes, obj: bytes, relocs: set, label: str):
    """Word compare with reloc wildcards. Returns (ok, [detail lines])."""
    lines = []
    if len(rom) != len(obj):
        return False, ["%s: SIZE differs -- rom 0x%x, probe 0x%x"
                       % (label, len(rom), len(obj))]
    ok = True
    for i in range(0, len(rom), 4):
        rw = struct.unpack_from("<I", rom, i)[0]
        ow = struct.unpack_from("<I", obj, i)[0]
        if i in relocs:
            lines.append("  +0x%03x  %08x  %08x  reloc (wildcard)" % (i, rw, ow))
            continue
        same = rw == ow
        ok &= same
        lines.append("  +0x%03x  %08x  %08x  %s"
                     % (i, rw, ow, "OK" if same else "<<< MISMATCH"))
    return ok, lines


# --------------------------------------------------------------------------
# driver
# --------------------------------------------------------------------------

def load():
    for p in (RTTI, VTABLES):
        if not p.is_file():
            sys.exit("missing %s -- run tools/rtti_extract.py and tools/rtti_vtables.py"
                     % p.name)
    rtti = json.loads(RTTI.read_text(encoding="utf-8"))
    vt = json.loads(VTABLES.read_text(encoding="utf-8"))
    recs = {}
    for r in rtti["records"].values():
        recs[r["name"]] = r
    parent = {}
    for ed in rtti["edges"]:
        if ed.get("offset", 0) == 0 or ed["derived"] not in parent:
            parent[ed["derived"]] = ed["base"]
    return rtti, vt, recs, parent


def ancestry(name, parent):
    """Most-base-first chain ending at `name`; None if it is not a simple chain."""
    chain, seen = [], set()
    n = name
    while n:
        if n in seen:
            return None
        seen.add(n)
        chain.append(n)
        n = parent.get(n)
    chain.reverse()
    return chain


def plan_class(name, vt, recs, parent):
    """Everything the probe and the comparison need, or (None, reason)."""
    rec = recs.get(name)
    if rec is None:
        return None, "no typeinfo record"
    if rec.get("kind") == "vmi":
        # Multiple inheritance needs base OFFSETS to be right, which needs sizeof, which
        # needs fields. Refusing is the honest answer; six classes are affected.
        return None, "multiple inheritance -- needs field layout, out of scope"
    chain = ancestry(name, parent)
    if chain is None:
        return None, "inheritance chain is cyclic"
    if any("::" in c for c in chain):
        # A nested class mangles under its enclosing scope, so the probe would have to
        # reopen that scope once per chain member -- and reopening `struct mHeap` to add
        # a second nested class is not legal C++. Emitting a flat `mHeap_Heap_t` instead
        # would produce _ZTS13mHeap_Heap_t and fail the name comparison for a reason that
        # says nothing about the skeleton. Refused rather than faked; 32 records have
        # nested names.
        return None, "nested class name -- probe scoping not implemented"
    slots_by_class = {}
    for cls in chain:
        v = vt.get(cls)
        if v is None or not v.get("slots"):
            return None, "no vtable slots for %s" % cls
        slots = v["slots"]
        # A suspect tail is a REFUSAL, not something to trim. Truncating it was tried and
        # is not defensible: the two orientations disagree about which slot is the last
        # real one -- daPeach_c's tail reads {0, fn} so the cut lands after a pointer,
        # daObjSwdoor_c's reads {fn, 0} so the same rule cuts one slot late and its
        # children then appear to have fewer slots than their base. Nothing in the ROM
        # settles it (dsd delinked no symbol at the boundary), so the honest output is
        # "this class's slot count is unreliable" rather than a plausible guess.
        # notes/rtti-reconciliation.md 8 records three such guesses, each wrong.
        if RR.suspect_tail(slots, v.get("parent_slots")) is not None:
            return None, "slot count unreliable (suspect tail in %s)" % cls
        pure = {i for i, s in enumerate(slots) if not s}
        p = parent.get(cls)
        inherited = len(vt[p]["slots"]) if p and vt.get(p, {}).get("slots") else 0
        if inherited > len(slots):
            return None, "fewer slots than its base (%s: %d < %d)" % (
                cls, len(slots), inherited)
        own = sorted(o["slot"] for o in (v.get("own") or [])
                     if o["slot"] < len(slots) and o["slot"] not in pure)
        slots_by_class[cls] = {"count": len(slots), "pure": pure,
                               "inherited": inherited, "overrides": own,
                               "key_override": next((i for i in own if i < inherited),
                                                    None)}
    return {"chain": chain, "slots": slots_by_class, "rec": rec,
            "vt": vt[name]}, None


def check(name, plan, version, flags, verbose=False, keep=False):
    """Compile the probe and compare _ZTS / _ZTV against the ROM."""
    res = {"class": name, "status": None, "detail": [], "checks": {}}
    src, why = probe_source(plan["chain"], plan["slots"])
    if src is None:
        res["status"] = "skipped"
        res["detail"].append(why)
        return res

    tmp = REPO / "build" / ("probe_%s.cpp" % name.replace("::", "_"))
    tmp.parent.mkdir(parents=True, exist_ok=True)
    tmp.write_text(src, encoding="utf-8", newline="\n")
    try:
        obj = M.compile_c(tmp, version, flags)
    finally:
        if not keep:
            tmp.unlink(missing_ok=True)
    if obj is None:
        res["status"] = "compile_failed"
        return res

    syms = sections_of(obj)
    mang = plan["rec"].get("mangled")
    rec = plan["rec"]
    vmod = plan["vt"].get("vtable_module") or rec["module"]
    slot0 = int(plan["vt"]["vtable"], 16)
    nslots = len(plan["vt"]["slots"])

    # ---- _ZTS: the name string. No relocations, so this is an exact test. ----
    want = rom_cstring(rec["module"], int(rec["name_addr"], 16))
    got = None
    for cand in ("_ZTS" + (mang or ""), "@%s" % name):
        if cand in syms:
            got = syms[cand][0].split(b"\0")[0]
            break
    if want is not None and got is not None:
        ok = want == got
        res["checks"]["name_string"] = "exact" if ok else "MISMATCH"
        res["detail"].append("  _ZTS  rom %r  probe %r  %s"
                             % (want, got, "OK" if ok else "<<< MISMATCH"))
    else:
        res["checks"]["name_string"] = "unavailable"
        res["detail"].append("  _ZTS  not emitted by the probe (or unreadable in ROM)")

    # ---- _ZTV: offset-to-top, typeinfo ptr, then the slots. ----
    vt_sym = next((s for s in syms if s.startswith("_ZTV")), None)
    rom_vt = rom_bytes(vmod, slot0 - 8, 8 + 4 * nslots)
    if vt_sym and rom_vt is not None:
        data, relocs = syms[vt_sym]
        ok, lines = compare_words(rom_vt, data, relocs, "_ZTV")
        res["checks"]["vtable"] = "conform" if ok else "MISMATCH"
        res["checks"]["vtable_symbol"] = vt_sym
        pure_idx = sorted(plan["slots"][name]["pure"])
        res["checks"]["pure_slots"] = pure_idx
        if verbose or not ok:
            res["detail"].append("  _ZTV %s  (%d slots, %d pure)"
                                 % (vt_sym, nslots, len(pure_idx)))
            res["detail"].extend(lines)
    else:
        res["checks"]["vtable"] = "unavailable"
        res["detail"].append("  _ZTV  %s"
                             % ("probe emitted none" if not vt_sym
                                else "ROM bytes unreadable"))

    vals = [v for k, v in res["checks"].items()
            if k in ("name_string", "vtable")]
    if "MISMATCH" in vals:
        res["status"] = "mismatch"
    elif "unavailable" in vals:
        res["status"] = "incomplete"
    else:
        res["status"] = "conform"
    return res


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--cls", help="one class, verbose")
    ap.add_argument("--report", action="store_true", help="every reachable class")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--keep", action="store_true", help="leave probe sources in build/")
    ap.add_argument("--version", default=RB.VERSION,
                    help="mwccarm build (default: the ROM build's pin, %s)" % RB.VERSION)
    ap.add_argument("-o", "--out", default=str(OUT))
    a = ap.parse_args()
    if not a.cls and not a.report:
        ap.error("pass --cls <Name> or --report")

    rtti, vt, recs, parent = load()
    # The build's own flags, not match.DEFAULT_FLAGS. build_pin.py exists because using
    # the sweep's flags to bless an edit the build then rejects is a real failure this
    # tree has already had; the same reasoning applies to a data-section comparison.
    flags = RB.CFLAGS + " -lang c++"

    targets = [a.cls] if a.cls else sorted(recs)
    if a.limit:
        targets = targets[:a.limit]

    rows, skipped = [], collections.Counter()
    for name in targets:
        plan, why = plan_class(name, vt, recs, parent)
        if plan is None:
            # Bucket on the REASON only. Keying on a string carrying the class name gave
            # every class its own bucket with count 1 and a 120-line unreadable tally.
            skipped[why.split(" --")[0].split(" (")[0]] += 1
            if a.cls:
                print("%s: SKIPPED -- %s" % (name, why))
            continue
        r = check(name, plan, a.version, flags, verbose=bool(a.cls), keep=a.keep)
        rows.append(r)
        if a.cls:
            print("%s  [%s]  chain: %s" % (name, r["status"], " -> ".join(plan["chain"])))
            for line in r["detail"]:
                print(line)
        else:
            print("%-34s %s" % (name, r["status"]))

    if a.report or len(rows) > 1:
        tally = collections.Counter(r["status"] for r in rows)
        print("\nchecked %d: %s" % (len(rows), ", ".join(
            "%s=%d" % kv for kv in sorted(tally.items()))))
        if skipped:
            print("skipped %d: %s" % (sum(skipped.values()), ", ".join(
                "%s=%d" % kv for kv in sorted(skipped.items()))))
        p = pathlib.Path(a.out)
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(json.dumps({"rows": rows, "skipped": dict(skipped),
                                 "version": a.version, "flags": flags},
                                indent=1), encoding="utf-8", newline="\n")
        print("wrote %s" % a.out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
