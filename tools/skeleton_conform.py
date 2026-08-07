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
    """The NUL-terminated string at `addr`, clamped to the end of the module.

    A fixed-size read fails at the end of a module: dScDSMT_c's name string is 0x54
    bytes from the end of ov007, so asking for 256 overran and rom_bytes returned None.
    That surfaced as "the probe emitted no _ZTS" for 57 classes when the probe had
    emitted it perfectly well and the ROM side was the broken half.
    """
    m = mod_for(modname)
    if m is None:
        return None
    if modname not in _bincache:
        _bincache[modname] = m["bin"].read_bytes()
    off = addr - m["base"]
    if off < 0 or off >= len(_bincache[modname]):
        return None
    b = _bincache[modname][off:off + min(cap, len(_bincache[modname]) - off)]
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


# Itanium __vmi_class_type_info, after the two words every type_info has:
#     +08  flags          (u32)
#     +0c  base_count     (u32)
#     +10  base[i]._ZTI   (ptr)
#     +14  base[i].offset_flags  (u32)  -- offset = >> 8, access bits = & 0xff
VMI_HDR = 16
VMI_BASE = 8


def read_vmi(rec, byaddr):
    """(flags, [(base_name_or_addr, offset, access_flags)]) from the ROM record.

    Read out of the binary rather than off build/rtti.json's edge list, because the
    ORDER matters here and an edge list is a set of pairs. The record stores bases in
    DECLARATION order, which is what a probe has to reproduce -- and declaration order
    is not offset order: daDemo_c::anmModel_c declares param_c (offset 100) first and
    its offset-0 base second, which happens when the first-declared base is not the one
    the compiler makes primary.
    """
    m, a = rec["module"], int(rec["addr"], 16)
    head = rom_bytes(m, a, VMI_HDR)
    if head is None:
        return None, None
    flags, n = struct.unpack_from("<II", head, 8)
    if not 0 < n <= 16:
        return None, None
    body = rom_bytes(m, a + VMI_HDR, VMI_BASE * n)
    if body is None:
        return None, None
    out = []
    for i in range(n):
        p, of = struct.unpack_from("<II", body, VMI_BASE * i)
        out.append((byaddr.get((m, p)) or byaddr.get(("arm9", p)) or "0x%08x" % p,
                    of >> 8, of & 0xFF))
    return flags, out


def base_sizes(bases):
    """Required sizeof for each base subobject, from the gaps between offsets.

    This is the payload. The ROM stores where each base subobject STARTS; consecutive
    starts therefore state how much room the earlier one occupies, and under Itanium a
    non-virtual base is placed at the next available offset. So dBgCh_SphCrr's
    (dBgCh @0, dBgPi @16, dM3dGSph @56) says sizeof(dBgCh) == 16 and the dBgPi
    subobject spans 40 bytes -- layout facts, in a tree where layout is the thing the
    compiler erased.

    The LAST base is unconstrained: nothing follows it in the record, so its size is
    reported as None rather than invented.
    """
    order = sorted(range(len(bases)), key=lambda i: bases[i][1])
    size = [None] * len(bases)
    for k in range(len(order) - 1):
        i, j = order[k], order[k + 1]
        size[i] = bases[j][1] - bases[i][1]
    return size


def probe_vmi_source(name, plan):
    """C++ for a multiple-inheritance class: flat bases at pinned sizes, then the leaf."""
    out = [PROBE_HEAD]
    leafname, outers = decl_name(name)
    for s in outers:
        out.append("namespace %s {" % s)
    for b in plan["bases"]:
        bleaf, bouter = decl_name(b["name"])
        if bouter:
            return None, "vmi base %s is nested" % b["name"]
        out.append("struct %s {" % bleaf)
        for i in range(b["slots"]):
            out.append("    virtual void s%d()%s;" % (i, " = 0" if i in b["pure"] else ""))
        if b["size"] is not None and b["size"] > 4:
            # vptr is 4; pad out so the NEXT base lands on the offset the ROM records.
            # `unsigned char`, not u8: the probe includes nothing, deliberately, so that
            # what it proves depends on no header in the tree.
            out.append("    unsigned char pad[0x%x];" % (b["size"] - 4))
        out.append("};")
    bl = ", ".join("public %s" % decl_name(b["name"])[0] for b in plan["bases"])
    out.append("struct %s : %s {" % (leafname, bl))
    # Slots the derived class adds beyond its PRIMARY base (the offset-0 one). Under
    # Itanium the primary base's table is extended in place, so those are the only new
    # entries in the primary vtable.
    prim = next((b for b in plan["bases"] if b["offset"] == 0), None)
    inherited = prim["slots"] if prim else 0
    total = len(plan["vt"]["slots"])
    pure = {i for i, s in enumerate(plan["vt"]["slots"]) if not s}
    for i in range(inherited, total):
        out.append("    virtual void s%d()%s;" % (i, " = 0" if i in pure else ""))
    key = next((i for i in range(inherited, total) if i not in pure), None)
    if key is None:
        # No new concrete virtual: override one of the primary base's instead, which
        # takes its slot rather than adding one.
        key = next((i for i in range(inherited) if i not in (prim or {}).get("pure", ())),
                   None)
        if key is None:
            return None, "no definable key function"
        out.append("    virtual void s%d();" % key)
    out.append("};")
    for _ in outers:
        out.append("}")
    out.append("")
    out.append("void %s::s%d() {}" % (name, key))
    out.append("")
    return "\n".join(out), None


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
    cur: list = []           # scopes currently open
    opened = set()
    for cls in chain:
        leafname, outers = decl_name(cls)
        # Enter and leave enclosing scopes as the chain moves through them. Emitting a
        # fresh `struct Outer { ... }` per chain member was the first attempt and is not
        # legal C++ -- reopening `struct dPa_c` to add a second nested class is an error,
        # and dPa_c::level_c has eleven of them.
        k = 0
        while k < len(cur) and k < len(outers) and cur[k] == outers[k]:
            k += 1
        for _ in range(len(cur) - k):
            out.append("}")
            cur.pop()
        for s in outers[k:]:
            key = tuple(cur + [s])
            if key in opened:
                return None, ("scope %s is re-entered along the chain"
                              % "::".join(key))
            opened.add(key)
            # `namespace`, not `struct`: Itanium mangles a nested name as N...E either
            # way (the ROM stores N5dPa_c7level_c10callback_cE), so this is free -- and
            # it is REQUIRED for std, whose St substitution only applies to the
            # namespace. std::type_info is St9type_info in the ROM, not N3stdE...
            out.append("namespace %s {" % s)
            cur.append(s)
        n_total = slots_by_class[cls]["count"]
        pure = slots_by_class[cls]["pure"]
        n_inherited = slots_by_class[cls]["inherited"]
        # The base is named in full. A qualified name resolves from inside an enclosing
        # scope as well as from global, so one spelling works everywhere.
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
        prev = cls
    # Close whatever scopes are still open, then define the key function at global
    # scope: a member of a nested class is definable from outside with its full
    # qualification, and that keeps one spelling for every case.
    for _ in range(len(cur)):
        out.append("}")
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


def plan_vmi(name, rec, vt, recs):
    """Plan for a multiple-inheritance class.  Bases are modelled FLAT, on purpose.

    The probe declares each base as its own struct with the base's own vtable slot count
    and padded to the size the record implies -- it does NOT reproduce the base's own
    ancestry. That is sound for what is being compared: the derived record cites each
    base only by an _ZTI POINTER (a relocation, wildcarded) plus an offset, and the
    offset depends on the base's SIZE, not on how the base got that size. Reproducing
    each base's chain as well would test the bases, which they already are on their own.

    The size choice is not circular in the way it first looks. Taking the offsets from
    the ROM and checking the offsets come back is nearly vacuous; what is not vacuous is
    that the compiler has to AGREE the layout is expressible -- declaration order, base
    alignment and the primary-base choice all get a vote, and any of them can refuse.
    The output that matters is the restatement: "dBgPi starts at 16" becomes
    "sizeof(dBgCh) == 16", which is the form a matcher can use.
    """
    byaddr = {}
    for r in recs.values():
        byaddr[(r["module"], int(r["addr"], 16))] = r["name"]
    flags, bases = read_vmi(rec, byaddr)
    if bases is None:
        return None, "vmi record unreadable"
    pre = None
    if any(b[0].startswith("0x") for b in bases):
        # A base pointer no record in this module claims -- a cross-module base. Naming
        # it would be a guess and the probe needs a real class to derive from.
        pre = "a vmi base does not resolve to a known record"
    order = [b[1] for b in bases]
    if order != sorted(order):
        # Declaration order drives layout, so a probe can only be written when the two
        # agree. daDemo_c's two model classes declare their offset-100 / offset-80 base
        # first and the primary second, which is what happens when the first-declared
        # base is not the one the compiler makes primary. Expressing that needs the
        # empty-base and primary-base rules modelled, which this pass does not do.
        pre = pre or "vmi declaration order is not offset order"
    sizes = base_sizes(bases)
    # The base offsets are the payload and they do NOT need a probe -- they are literals
    # in the record. So build the table first and record separately whether a probe can
    # be written for it. dBgCh_Lin is exactly this case: dM3dGLin has no readable vtable
    # so nothing can be compiled, but "dBgCh @0, dBgPi @16, dM3dGLin @56" is a fact
    # about layout either way, and it corroborates dBgCh_SphCrr's identical first two.
    binfo, blocked = [], pre
    for (bn, off, bf), sz in zip(bases, sizes):
        bt = vt.get(bn)
        nslots = len(bt["slots"]) if bt and bt.get("slots") else None
        if nslots is None:
            blocked = blocked or "vmi base %s has no vtable" % bn
        if sz is not None and sz < 4:
            blocked = blocked or ("vmi base %s needs %d bytes but a vptr is 4" % (bn, sz))
        binfo.append({"name": bn, "offset": off, "flags": bf, "size": sz,
                      "slots": nslots,
                      "pure": {i for i, s in enumerate(bt["slots"]) if not s}
                      if bt and bt.get("slots") else set()})
    t = vt.get(name)
    if t is None or not t.get("slots"):
        blocked = blocked or "no vtable slots for %s" % name
    elif RR.suspect_tail(t["slots"], t.get("parent_slots")) is not None:
        blocked = blocked or "slot count unreliable (suspect tail in %s)" % name
    return {"vmi": True, "rec": rec, "vt": t, "bases": binfo, "flags": flags,
            "chain": [name], "blocked": blocked}, None


def plan_class(name, vt, recs, parent):
    """Everything the probe and the comparison need, or (None, reason)."""
    rec = recs.get(name)
    if rec is None:
        return None, "no typeinfo record"
    if rec.get("kind") == "vmi":
        return plan_vmi(name, rec, vt, recs)
    chain = ancestry(name, parent)
    if chain is None:
        return None, "inheritance chain is cyclic"
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
    if plan.get("vmi") and plan.get("blocked"):
        # Report the layout facts anyway; only the compile half is unavailable.
        res["status"] = "bases_only"
        res["checks"]["bases"] = [
            {"name": b["name"], "offset": b["offset"], "sizeof": b["size"]}
            for b in plan["bases"]]
        res["detail"].append("  no probe: %s" % plan["blocked"])
        for b in plan["bases"]:
            res["detail"].append(
                "  base %-20s offset %3d   %s"
                % (b["name"], b["offset"],
                   ("sizeof == %d" % b["size"]) if b["size"] is not None
                   else "sizeof unconstrained (last base)"))
        return res
    if plan.get("vmi"):
        src, why = probe_vmi_source(name, plan)
    else:
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

    # ---- _ZTI: for a vmi record the flags, base count and every base's
    # offset_flags are LITERALS, so they compare exactly. Only the _ZTI and _ZTS
    # pointers relocate. This is the whole multiple-inheritance test: base count,
    # base order, each base's subobject offset and its access bits.
    if plan.get("vmi"):
        # Exact name, never a prefix/suffix scan: the object also defines every
        # ancestor's _ZTI, and a scan can return one of those instead of the class's own.
        ti_sym = ("_ZTI" + mang) if mang and ("_ZTI" + mang) in syms else None
        rom_ti = rom_bytes(rec["module"], int(rec["addr"], 16),
                           VMI_HDR + VMI_BASE * len(plan["bases"]))
        if ti_sym and rom_ti is not None:
            data, relocs = syms[ti_sym]
            ok, lines = compare_words(rom_ti[:len(data)], data, relocs, "_ZTI")
            res["checks"]["typeinfo"] = "conform" if ok else "MISMATCH"
            if verbose or not ok:
                res["detail"].append("  _ZTI %s  (%d bases)" % (ti_sym, len(plan["bases"])))
                res["detail"].extend(lines)
        else:
            res["checks"]["typeinfo"] = "unavailable"
            res["detail"].append("  _ZTI  %s"
                                 % ("probe emitted none" if not ti_sym
                                    else "ROM bytes unreadable"))
        res["checks"]["bases"] = [
            {"name": b["name"], "offset": b["offset"], "sizeof": b["size"]}
            for b in plan["bases"]]
        for b in plan["bases"]:
            res["detail"].append(
                "  base %-20s offset %3d   %s"
                % (b["name"], b["offset"],
                   ("sizeof == %d" % b["size"]) if b["size"] is not None
                   else "sizeof unconstrained (last base)"))

    # ---- _ZTS: the name string. No relocations, so this is an exact test. ----
    want = rom_cstring(rec["module"], int(rec["name_addr"], 16))
    got = None
    if mang and ("_ZTS" + mang) in syms:
        got = syms["_ZTS" + mang][0].split(b"\0")[0]
    if want is not None and got is not None:
        ok = want == got
        res["checks"]["name_string"] = "exact" if ok else "MISMATCH"
        res["detail"].append("  _ZTS  rom %r  probe %r  %s"
                             % (want, got, "OK" if ok else "<<< MISMATCH"))
    else:
        res["checks"]["name_string"] = "unavailable"
        # Say WHICH side is missing. Collapsing the two hid a bug in this tool's own ROM
        # reader behind a claim about the compiler for 57 classes.
        res["detail"].append("  _ZTS  %s"
                             % ("ROM string unreadable at %s" % rec.get("name_addr")
                                if want is None else "probe emitted no _ZTS%s" % mang))

    # ---- _ZTV: offset-to-top, typeinfo ptr, then the slots. ----
    vt_sym = ("_ZTV" + mang) if mang and ("_ZTV" + mang) in syms else None
    rom_vt = rom_bytes(vmod, slot0 - 8, 8 + 4 * nslots)
    if vt_sym and rom_vt is not None:
        data, relocs = syms[vt_sym]
        extra = 0
        if plan.get("vmi") and len(data) > len(rom_vt):
            # A multiple-inheritance class emits a vtable GROUP under one _ZTV symbol:
            # the primary table, then one secondary table per non-primary base, each
            # with its own negative offset-to-top and its thunks. The ROM's _ZTV here
            # names only the primary -- rtti_extract counts the secondaries separately,
            # at their own addresses -- so compare the primary and say what was left.
            extra = len(data) - len(rom_vt)
            data = data[:len(rom_vt)]
            relocs = {r for r in relocs if r < len(rom_vt)}
        ok, lines = compare_words(rom_vt, data, relocs, "_ZTV")
        if extra:
            res["checks"]["vtable_secondary_bytes"] = extra
            lines.append("  (+%d bytes of secondary vtables in the probe, not compared: "
                         "the ROM records those at their own addresses)" % extra)
        res["checks"]["vtable"] = "conform" if ok else "MISMATCH"
        res["checks"]["vtable_symbol"] = vt_sym
        pure_idx = sorted(plan["slots"][name]["pure"]) if not plan.get("vmi") else \
            sorted(i for i, s in enumerate(plan["vt"]["slots"]) if not s)
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
            if k in ("name_string", "vtable", "typeinfo")]
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

        # The multiple-inheritance payload, collected in one place. These are the only
        # sizeof statements the ROM makes directly: everywhere else layout has to be
        # inferred from what matched code happens to touch.
        mi = [r for r in rows if r["checks"].get("bases")]
        if mi:
            print("\nbase subobject offsets (the ROM's only direct sizeof statements):")
            sizes = collections.defaultdict(set)
            for r in mi:
                print("  %-24s %s" % (r["class"], "  ".join(
                    "%s@%d" % (b["name"], b["offset"]) for b in r["checks"]["bases"])))
                for b in r["checks"]["bases"]:
                    if b["sizeof"] is not None:
                        sizes[b["name"]].add(b["sizeof"])
            print("\n  implied sizeof:")
            for n in sorted(sizes):
                v = sorted(sizes[n])
                # Two records naming the same size is corroboration; disagreement would
                # mean one of them is not what this pass thinks it is, so say which.
                print("    %-24s %s%s" % (n, ", ".join(str(x) for x in v),
                                          "   <- CONFLICT" if len(v) > 1 else ""))
        p = pathlib.Path(a.out)
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(json.dumps({"rows": rows, "skipped": dict(skipped),
                                 "version": a.version, "flags": flags},
                                indent=1), encoding="utf-8", newline="\n")
        print("wrote %s" % a.out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
