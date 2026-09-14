#!/usr/bin/env python3
"""Generate the mechanical faces and aliases a slice's link wall asks for.

WHY THIS EXISTS. When a seat lane's link (or port/tools/closure.py) names its
unresolved externals, most of the wall is not judgment, it is transcription.
The MG1 pathfinder enumerated its wall as "30 ordinary /alternatename faces,
2 argument-landing faces, 7 pointer-to-member symbols"; the S1 spine wrote 24
faces in one round and 12 aliases in another; hal/actor_class_faces.cpp is a
whole file of the same shape written by hand. Every one of those rows is a
pure function of two mangled names, and lanes have been spending their
attention writing them one at a time.

WHAT IT EMITS, sorted from an unresolved-external list plus a universe of
available definitions (a map, a targets file, or both):

  ALIAS    a decorated DATA name over a hosted C symbol:
           #pragma comment(linker, "/alternatename:?d@@3PAHA=_d").
           Data only, plus __cdecl free functions onto a same-named C
           definition. NEVER a method: an alias cannot change a calling
           convention, the ov007 lane's law.
  FACE     a __thiscall method whose body is a matched C-name TU: the
           shadow-struct forwarder of hal/actor_class_faces.cpp, declared
           from the CALLER's mangle so the symbol reproduces exactly, in a
           file that includes NOTHING (that file's header says why).
  RFACE    the reverse: an unresolved Itanium C name whose body is a matched
           MSVC method (the TrackInDeathTable shape). Zero-argument methods
           only; a reverse face with arguments re-lands them and is written
           by hand against the cxxname_bridge precedent.

WHAT IT REFUSES, because a generator that guesses saves nothing (the law:
every generator carries its verifier, and judgment rows go to a human):

  * anything pointer-to-member typed (P8/Q8 in the TYPE TEXT, under any
    wrapping: P8 direct, PAP8 as an array) -- the gate-16 wall; a host
    copy, never a face. The boundary is the consumer-spelling law
    (port/mg_fanout_costs.txt section 4): a pair whose consumer spells it
    as two ints is safe as an alias, and a consumer naming a member-pointer
    type needs a host copy WHETHER IT IS CALLED OR ONLY COPIED, because the
    four-byte MSVC member pointer shifts every field after it;
  * struct-typed globals in ANY spelling -- @@3U by value AND @@3PAU as a
    pointer or array. The mangle cannot show the struct's members, and a
    struct wrapper hides a member-pointer table: @@3PAUEntry@@A slipped the
    first version's two guards and was the twenty-five-entry dScMgCurling_c
    state table (mg_fanout_costs.txt s10 finding 1). Where the tool cannot
    see the consumer it refuses with the reason rather than aliasing;
  * data spellings outside the scalar subset -- ALIAS is now a whitelist
    (scalars and pointer/array-of-scalar chains), not "anything that is not
    a struct". A spelling this tool has not proven safe is a refusal, not
    an alias. NAMED BEHAVIOR CHANGE: W4 enum-spelled globals, which the
    pre-hardening tool ALIASED, now refuse through this whitelist. An enum
    cannot hide a member pointer, so the alias would be safe in principle,
    but no wall row of that shape has ever appeared and the whitelist does
    not claim spellings it has never seen; if one arrives, the refusal
    reason says where to rule it;
  * namespace-scope free functions, both directions (the @@Y-after-a-
    qualifier spelling). Memory is a NAMESPACE, and the first version read
    ?Allocate@Memory@@YAPAXIH@Z as a method of class Memory and refused
    the Itanium side as "reverse face with 2 args" -- right refusal, wrong
    reason (s10 finding 4). Detected now and refused with the true one:
    when both sides are __cdecl with the same arguments a plain hand
    /alternatename is the whole fix (hal/scene_mg_faces.cpp section 2d is
    the ruling). Auto-emitting that alias was considered and DECLINED: it
    would need a cross-mangling parameter-type comparison, and the failure
    mode of getting one row wrong is an alias that links and misreads the
    stack. One row has ever needed it;
  * arity disagreements between the two mangles -- ModelBase::ApplyOpacity
    takes 2 on the caller's side and 1 in the ROM body, and the hand face
    that drops the argument was a RULING, not plumbing;
  * ambiguous joins (two candidates for one class::method), float/double
    signatures, struct-by-value returns (SRET is trap territory), templates,
    statics, non-thiscall conventions.

THE VERIFIER (--verify) IS MANDATORY, not an extra: no generated file is
wired into a slice without a PASS from it, and the fan-out briefs carry
that as a rule. It compiles the emitted file and reads the object back
with dumpbin: the defined-external set must equal the requested rows
EXACTLY and the undefined set must be a subset of the target universe.
That is the same check A1 ran on WallSign's Render by hand, and it is
load-bearing: a class-typed parameter spelled as a struct silently
produces a wrong-mangling face (MSVC mangles class V where struct is U,
proven in review), and nothing but the surface check catches it. What no
verifier checks is method_faces.cpp's failure mode 1 (a forward to a
plausible SIBLING): the generator's exact-name join refuses lookalike
siblings instead of choosing between them, and every emitted row still
gets the per-face checklist at review -- target, arity, receiver.

    python port/tools/facegen.py --unresolved wall.txt --map build/port/walk_window.map --out faces_gen.cpp --verify
    python port/tools/facegen.py --unresolved wall.txt --targets have.txt --out faces_gen.cpp --verify
    python port/tools/facegen.py --selftest
"""

import argparse
import os
import pathlib
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import closure  # noqa: E402

SCALARS = {"int", "unsigned int", "short", "unsigned short", "char",
           "unsigned char", "signed char", "long", "unsigned long", "bool"}
REFUSED_SCALARS = {"float", "double", "__int64", "unsigned __int64"}

# (?!Y) is finding 4's fix: after a single qualifier, Y is "function", not a
# member access specifier, so ?Allocate@Memory@@YA... is a free function in
# NAMESPACE Memory and never was a method of a class Memory. MSVC_NSFREE
# catches exactly what MSVC_METHOD now excludes. Deeper nestings
# (?f@Inner@Outer@@...) match neither and fall to UNKNOWN, a safe refusal.
MSVC_METHOD = re.compile(r"^\?(\w+)@(\w+)@@(?!Y)[A-Z]")
# A STRUCTOR IS A MEMBER TOO, and MSVC_METHOD cannot see one: ??0Cls@@QAE@XZ
# and ??1Cls@@UAE@XZ have no method-name component for it to match, so the
# forward direction refused every one of them as "not a class-member mangle"
# -- 34 rows of the sync wall, including ??0fBase_c@@QAE@XZ. parse_msvc_sig
# has always reported the structor kind; only the gate was short.
MSVC_STRUCTOR = re.compile(r"^\?\?([01])((?:\w+@)+)@[A-Z]")
MSVC_NSFREE = re.compile(r"^\?(\w+)@(\w+)@@Y([A-Z])")
MSVC_DATA = re.compile(r"^\?(\w+)@(?:(\w+)@)?@3(.+)$")
MSVC_FREE = re.compile(r"^\?(\w+)@@Y([A-Z])")
ITANIUM = re.compile(r"^_?(_ZNK?\d.*)$")

# The ALIAS whitelist for data spellings: scalars, and pointer/array-of-
# scalar chains ([PQ][AB] prefixes), with a trailing cv/storage letter.
# These are the spellings that CANNOT hide a member pointer -- the two-ints
# law's mechanical form. Everything else refuses: U/T structs in any
# wrapping (the members are invisible in the mangle), W4 enums (safe in
# principle, never seen on a wall, so not claimed), and anything this
# expression has no opinion on.
SAFE_DATA_TYPE = re.compile(r"^(?:[PQ][AB])*(?:[CDEFGHIJKMNX]|_[JKN])[A-D]$")


# ---------------------------------------------------------------------------
# THE ADDRESS BINDING (lane FACES1, the main -> port sync's last link wall).
#
# THE RULE THAT MAKES A FACE HONEST: a face for flat name F binds F to exactly
# the member the ROM name names, never a sibling. Three things have to agree
# before a row is generated, and the row is REFUSED with the reason when any of
# them does not:
#
#   1. F IS A ROM NAME AT A KNOWN ADDRESS. F must appear in config/**/symbols.txt
#      with an addr:. A flat name with no ROM address is not a ROM function and
#      no face can be derived for it.
#   2. ONE ADDRESS PER TARGET. Every ROM symbol that would join the same MSVC
#      definition is gathered, and their addresses must be ONE address. This is
#      the mechanical form of method_faces.cpp's failure mode 1: ApproachLinear
#      and ApproachLinear2 are two ROM bodies 140 lines apart, and a face that
#      picked the wrong one shipped (signs spinning forever, 156 src TUs). The
#      pair that makes this bite here is the destructor: _ZN1XD1Ev at one
#      address and _ZN1XD2Ev at another BOTH demangle to X::~X, and MSVC has one
#      ??1X@@UAE@XZ. Two addresses, one target, so both refuse.
#   3. CLASS AND METHOD AGREE LITERALLY. undname is asked what the decorated
#      definition really says, and its class and method text must equal the
#      Itanium name's, component for component; its parameter count must equal
#      the Itanium parameter count; its constness must match.
#
# The structor kinds are keyed APART, because they are different ROM functions:
#   D1, D2  -> "~"        the complete/base object destructor, no deallocation.
#                         MSVC's ??1Cls@@ is exactly this.
#   D0      -> "~delete"  the DELETING destructor. MSVC's is ??_GCls@@UAEPAXI@Z,
#                         the scalar deleting destructor, which cannot be named
#                         from C++ source at all -- and ??1 is its non-deleting
#                         sibling, so binding D0 to ??1 would drop the free.
#                         Always refused; the D0 half is the flat .c TU's job
#                         (hal/dtor_faces_cpp.cpp's "WHAT IS NOT DONE" note).
#   C1, C2, C3 -> "ctor"  MSVC has one ??0Cls@@ per signature; when the ROM has
#                         C1 and C2 at different addresses rule 2 refuses both.
STRUCTOR_KEY = {"C1": "ctor", "C2": "ctor", "C3": "ctor",
                "D0": "~delete", "D1": "~", "D2": "~"}

# MSVC access/virtualness letters, member functions, near (A) code model.
# The pair is what the shadow declaration has to reproduce: get either half
# wrong and the emitted reference names a symbol nothing defines.
MSVC_ACCESS = {
    "A": ("private", False),   "B": ("private", False),
    "E": ("private", True),    "F": ("private", True),
    "I": ("protected", False), "J": ("protected", False),
    "M": ("protected", True),  "N": ("protected", True),
    "Q": ("public", False),    "R": ("public", False),
    "U": ("public", True),     "V": ("public", True),
}


def decorated_access(sym):
    """The (access, is_virtual) pair a decorated member name declares.

    ?meth@Cls@@<letter>... and ??1Cls@@<letter>..., where <letter> is the
    access/virtualness code. Returns None for anything that is not a member
    function mangle.
    """
    m = re.match(r"^\?\?[0-9A-Z_]+(?:\w+@)+@([A-Z])[A-Z]", sym)
    if not m:
        m = re.match(r"^\?\w+(?:@\w+)+@@([A-Z])[A-Z]", sym)
    if not m:
        return None
    return MSVC_ACCESS.get(m.group(1))


def itanium_parse_ext(name):
    """The extended demangler subset: what a sync face may be derived from.

    Covers, beyond itanium_parse's two-component non-structor shape:
      * ONE to FOUR name components, so a nested class or a class inside a
        namespace (dScEntry_c::graphCallback_c::GraphCallback2,
        dPa_c::level_c::callback_c::SpawnParticles) parses instead of
        falling to UNKNOWN;
      * structors, keyed by STRUCTOR_KEY above;
      * method names that begin with an underscore (Heap::_Deallocate),
        which itanium_parse rejected on meth[0].isalpha().

    Returns a dict, or None when the name is outside the subset (operators,
    _ZTV/_ZTI, _ZSt names, template blocks in the NAME components). Arity
    comes from itanium_arity and is -1 when that refuses.
    """
    m = re.match(r"^_ZN(K?)(\d.*)$", name)
    if not m:
        return None
    is_const = m.group(1) == "K"
    comps, rest = [], m.group(2)
    while rest and rest[0].isdigit():
        n = re.match(r"(\d+)", rest)
        length = int(n.group(1))
        body = rest[n.end():n.end() + length]
        if len(body) != length:
            return None
        comps.append(body)
        rest = rest[n.end() + length:]
    if not comps or len(comps) > 4:
        return None
    st = re.match(r"^([CD][0-3])E", rest)
    if st:
        tag = st.group(1)
        key = STRUCTOR_KEY.get(tag)
        if key is None:
            return None
        return {"comps": comps, "cls": list(comps), "meth": key,
                "structor": tag, "const": is_const,
                "arity": itanium_arity(rest[st.end():])}
    if not rest.startswith("E") or len(comps) < 2:
        return None
    meth = comps[-1]
    if not (meth[0].isalpha() or meth[0] == "_"):
        return None
    return {"comps": comps, "cls": comps[:-1], "meth": meth,
            "structor": None, "const": is_const,
            "arity": itanium_arity(rest[1:])}


def msvc_prefix(rec):
    """The decorated-name prefix the Itanium record's target must carry.

    MSVC writes a qualified name INNERMOST FIRST, so Outer::Inner::meth is
    ?meth@Inner@Outer@@ -- the reverse of the Itanium component order.
    """
    qual = "".join(c + "@" for c in reversed(rec["cls"]))
    if rec["meth"] == "ctor":
        return "??0" + qual + "@"
    if rec["meth"] == "~":
        return "??1" + qual + "@"
    if rec["structor"]:
        return None                     # "~delete": ??_G, not nameable
    return "?" + rec["meth"] + "@" + qual + "@"


def cpp_qualified(rec):
    """Outer::Inner, the C++ spelling of the record's class."""
    return "::".join(rec["cls"])


_ROM_INDEX_CACHE = {}


def rom_index(root):
    """{ROM symbol: [(address, module)]} from every config/**/symbols.txt.

    This is the ADDRESS half of the binding rule. Reading every symbols.txt
    under config/ (arm9, itcm, dtcm and every overlay) is what lets rule 2
    see a D1/D2 pair that lives in one overlay and a C1/C2 pair that lives
    in another.
    """
    root = os.path.abspath(root)
    if root in _ROM_INDEX_CACHE:
        return _ROM_INDEX_CACHE[root]
    idx = {}
    cfg = os.path.join(root, "config")
    if not os.path.isdir(cfg):
        sys.exit("no config/ under %s: the address binding cannot be checked, "
                 "and a face derived without it is a guess" % root)
    pat = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-fA-F]+)")
    for dirpath, _dirs, files in os.walk(cfg):
        if "symbols.txt" not in files:
            continue
        mod = os.path.relpath(dirpath, cfg).replace("\\", "/")
        with open(os.path.join(dirpath, "symbols.txt"), encoding="utf-8",
                  errors="replace") as fh:
            for ln in fh:
                m = pat.match(ln)
                if m:
                    idx.setdefault(m.group(1), []).append(
                        (int(m.group(3), 16), mod))
    if not idx:
        sys.exit("config/ under %s holds no symbols.txt rows" % root)
    _ROM_INDEX_CACHE[root] = idx
    return idx


_JOIN_CACHE = {}


def rom_join_addresses(root):
    """{(cls tuple, meth key): {address: [ROM names]}} over the whole ROM.

    Rule 2's table. Built from the ROM index, not from the wall, because a
    sibling that is ALREADY DEFINED in this link never appears on the wall
    and would otherwise be invisible to the check.
    """
    root = os.path.abspath(root)
    if root in _JOIN_CACHE:
        return _JOIN_CACHE[root]
    table = {}
    for name, entries in rom_index(root).items():
        rec = itanium_parse_ext(name)
        if rec is None:
            continue
        key = (tuple(rec["cls"]), rec["meth"])
        slot = table.setdefault(key, {})
        for addr, _mod in entries:
            slot.setdefault(addr, []).append(name)
    _JOIN_CACHE[root] = table
    return table


# ---------------------------------------------------------------------------
# THE D0 RULE (lane FACES2).
#
# MSVC's deleting destructor is ??_GCls@@UAEPAXI@Z, which C++ source cannot
# name, so the address binding refuses every _ZN<Cls>D0Ev outright (rule 2
# above) and the sync wall kept 356 of them. They are not judgment rows: lane
# FACES1 measured all 356 against the ROM's own relocations and every one is
# its D1 PLUS EXACTLY ONE CALL, to Memory::Deallocate(void *, Heap *) or to
# Memory::operator_delete2(void *). So the D0 face is the D1 body followed by
# that one call, and the only thing in it that is not a name is Deallocate's
# second argument: a heap pointer each D0 body reads out of its OWN LITERAL
# POOL. That pool word is RELOCATED, so config/**/relocs.txt says which symbol
# it points at, per class. Getting it wrong is heap corruption, which is why it
# is derived from the reloc row and never defaulted.
#
# WHAT IS CHECKED WHERE, and why the split.
#   * At DERIVATION (a lane, once, with capstone and extracted/): the D0 body's
#     whole instruction stream is diffed against its D1's, with pc-relative
#     loads and branches resolved through the relocations, and the diff must be
#     ONE contiguous insertion that makes exactly one call. A class whose D0
#     differs from its D1 by anything else is REFUSED with the diff pasted.
#     That check needs the ROM image, which is gitignored, so it cannot be the
#     build's check.
#   * At BUILD (this tool, every time): the same claim re-proved from
#     config/**/relocs.txt alone, which IS checked in -- the D0's call
#     relocations must be the D1's plus exactly one to the ledger's
#     deallocator, and its load relocations the D1's plus, for Deallocate,
#     exactly one, to the ledger's heap word. A ledger row whose ROM
#     relocations stop saying that fails the build.

_SPAN_CACHE = {}


def rom_spans(root):
    """{ROM symbol: [(address, module, size)]} for every config record.

    rom_index drops the size because the address binding never needed it; the
    D0 rule does, because its proof is over a body's own address RANGE.
    """
    root = os.path.abspath(root)
    if root in _SPAN_CACHE:
        return _SPAN_CACHE[root]
    idx = {}
    cfg = os.path.join(root, "config")
    pat = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-fA-F]+)")
    for dirpath, _dirs, files in os.walk(cfg):
        if "symbols.txt" not in files:
            continue
        mod = os.path.relpath(dirpath, cfg).replace("\\", "/")
        with open(os.path.join(dirpath, "symbols.txt"), encoding="utf-8",
                  errors="replace") as fh:
            for ln in fh:
                m = pat.match(ln)
                if not m:
                    continue
                sm = re.search(r"size=(0x[0-9a-fA-F]+)", m.group(2))
                idx.setdefault(m.group(1), []).append(
                    (int(m.group(3), 16), mod,
                     int(sm.group(1), 16) if sm else None))
    _SPAN_CACHE[root] = idx
    return idx


_RELOC_CACHE = {}


def rom_relocs(root):
    """{module: sorted [(from, kind, to)]} from every config/**/relocs.txt."""
    root = os.path.abspath(root)
    if root in _RELOC_CACHE:
        return _RELOC_CACHE[root]
    out = {}
    cfg = os.path.join(root, "config")
    pat = re.compile(
        r"^from:(0x[0-9a-fA-F]+)\s+kind:(\S+)\s+to:(0x[0-9a-fA-F]+)")
    for dirpath, _dirs, files in os.walk(cfg):
        if "relocs.txt" not in files:
            continue
        mod = os.path.relpath(dirpath, cfg).replace("\\", "/")
        rows = []
        with open(os.path.join(dirpath, "relocs.txt"), encoding="utf-8",
                  errors="replace") as fh:
            for ln in fh:
                m = pat.match(ln)
                if m:
                    rows.append((int(m.group(1), 16), m.group(2),
                                 int(m.group(3), 16)))
        out[mod] = sorted(rows)
    _RELOC_CACHE[root] = out
    return out


def _span_of(root, name):
    """((address, module, size), None) for a uniquely placed, sized symbol."""
    ents = rom_spans(root).get(name)
    if not ents:
        return None, "no ROM record for %s in config/**/symbols.txt" % name
    if len(ents) > 1:
        return None, ("%s is listed at %d ROM addresses (%s), so its body has "
                      "no single span" % (name, len(ents),
                                          ", ".join("0x%08x" % a
                                                    for a, _m, _s in ents)))
    addr, mod, size = ents[0]
    if size is None:
        return None, "the config record for %s carries no size" % name
    return (addr, mod, size), None


def _reloc_slice(root, mod, addr, size, kinds):
    """The relocation targets of one body, sorted, for the named kinds."""
    out = []
    for frm, kind, to in rom_relocs(root).get(mod, ()):
        if addr <= frm < addr + size and any(k in kind for k in kinds):
            out.append(to)
    return sorted(out)


def _multiset_diff(a, b):
    """(a minus b, b minus a) as multisets, order preserved."""
    extra, missing = list(a), list(b)
    for t in b:
        if t in extra:
            extra.remove(t)
    for t in a:
        if t in missing:
            missing.remove(t)
    return extra, missing


def d0_reloc_proof(root, d0name, d1name, dealloc_addr, heap_addr):
    """Re-prove 'D0 is D1 plus one deallocation' from the checked-in relocs.

    Returns (info, None) or (None, reason). This is the BUILD-time half of the
    D0 rule: no ROM image and no disassembler, only config/**/relocs.txt.
    """
    s0, why = _span_of(root, d0name)
    if s0 is None:
        return None, why
    s1, why = _span_of(root, d1name)
    if s1 is None:
        return None, why
    a0, m0, z0 = s0
    a1, m1, z1 = s1
    if m0 != m1:
        return None, ("%s is in %s and %s in %s, so the pair is not one body "
                      "plus a call" % (d0name, m0, d1name, m1))
    extra, missing = _multiset_diff(
        _reloc_slice(root, m0, a0, z0, ("call",)),
        _reloc_slice(root, m1, a1, z1, ("call",)))
    if missing:
        return None, ("%s makes calls %s does not (%s), so the D0 is not the "
                      "D1 plus a call"
                      % (d1name, d0name,
                         ", ".join("0x%08x" % t for t in missing)))
    if extra != [dealloc_addr]:
        return None, ("the calls %s makes beyond %s are %s, not the one "
                      "deallocation at 0x%08x"
                      % (d0name, d1name,
                         ", ".join("0x%08x" % t for t in extra) or "none",
                         dealloc_addr))
    lextra, lmissing = _multiset_diff(
        _reloc_slice(root, m0, a0, z0, ("load",)),
        _reloc_slice(root, m1, a1, z1, ("load",)))
    if lmissing:
        return None, ("%s pools words %s does not (%s)"
                      % (d1name, d0name,
                         ", ".join("0x%08x" % t for t in lmissing)))
    if heap_addr is None:
        if lextra:
            return None, ("operator_delete2 takes no heap, but %s pools %s "
                          "beyond its D1"
                          % (d0name, ", ".join("0x%08x" % t for t in lextra)))
    elif lextra != [heap_addr]:
        return None, ("the words %s pools beyond %s are %s, not the one heap "
                      "pointer at 0x%08x"
                      % (d0name, d1name,
                         ", ".join("0x%08x" % t for t in lextra) or "none",
                         heap_addr))
    return {"addr": a0, "module": m0, "size": z0, "d1_addr": a1}, None


# ---------------------------------------------------------------------------
# THE STRUCTOR TWIN RULE (lane FACES3).
#
# Rule 2 refuses a flat name whose (class, method) key joins more than one ROM
# address, because one decorated definition cannot be two different functions.
# For an ordinary overload that is exactly right and it stays. For a STRUCTOR
# PAIR it is a question about the ROM rather than about names: the Itanium ABI
# emits D1 (complete object) and D2 (base object) as two symbols, and C1/C2
# likewise, and for a class with no virtual bases the two are the SAME CODE
# emitted twice. MSVC emits ONE destructor and one constructor. So the pair can
# be bound to that one definition exactly when the two ROM bodies are PROVED to
# be one body emitted twice, and it keeps refusing when they are not.
#
# That refusal is also the net for config/**/symbols.txt carrying a pair the
# wrong way round (the Actor/ActorBase D1/D2 swap this tree has seen): an
# identical pair cannot be swapped into a wrong answer, and a pair that is not
# identical never binds.
#
# WHAT IS CHECKED WHERE, the same split the D0 rule uses.
#   * At DERIVATION (a lane, once, with extracted/): the two bodies' BYTES are
#     compared over the ROM image with every relocated word masked out, and the
#     only difference allowed beyond that is an ARM B/BL word whose resolved
#     target is the same in both -- a call that did not leave the delink unit
#     encodes a different displacement from a different address and is the same
#     call. Lane FACES3 ran that over the 28 pairs the sync wall asks for: 27
#     are byte-identical bodies, and the one that is not (dCapEnemy_c, whose D1
#     is in arm9 and whose D2 is in ov002) refuses on the first clause below.
#   * At BUILD (this tool, every time): the same claim re-proved from the
#     checked-in config alone -- one module, equal sizes, and relocations equal
#     as body-relative (offset, kind, target) triples, so the two bodies
#     reference the same things at the same places. A pair that stops saying
#     that fails the build.


def _reloc_triples(root, mod, addr, size):
    """A body's relocations as sorted body-relative (offset, kind, target)."""
    out = []
    for frm, kind, to in rom_relocs(root).get(mod, ()):
        if addr <= frm < addr + size:
            out.append((frm - addr, kind, to))
    return sorted(out)


def structor_twin_proof(root, name_a, name_b):
    """(True, None) when two ROM structor bodies are one body emitted twice."""
    sa, why = _span_of(root, name_a)
    if sa is None:
        return False, why
    sb, why = _span_of(root, name_b)
    if sb is None:
        return False, why
    a_addr, a_mod, a_size = sa
    b_addr, b_mod, b_size = sb
    if a_mod != b_mod:
        return False, ("%s is in %s and %s in %s, so they are not one body "
                       "emitted twice" % (name_a, a_mod, name_b, b_mod))
    if a_size != b_size:
        return False, ("%s is 0x%x bytes and %s is 0x%x, so they are not one "
                       "body emitted twice" % (name_a, a_size, name_b, b_size))
    ra = _reloc_triples(root, a_mod, a_addr, a_size)
    rb = _reloc_triples(root, b_mod, b_addr, b_size)
    if ra != rb:
        return False, ("%s and %s relocate differently (%s against %s), so "
                       "they are not one body emitted twice"
                       % (name_a, name_b,
                          ["+0x%x %s 0x%08x" % t for t in ra],
                          ["+0x%x %s 0x%08x" % t for t in rb]))
    return True, None


def structor_twin_slot(root, rec, slot):
    """(True, None) when a rule-2 slot is a provable C1/C2 or D1/D2 pair.

    `slot` is rom_join_addresses' {address: [ROM names]} for one key. The pair
    has to be exactly the two structor spellings of ONE class -- the same name
    components, the same structor letter, the digits 1 and 2 -- before the ROM
    proof is asked at all, so an overload or a third address never reaches it.
    """
    if rec["meth"] not in ("ctor", "~"):
        return False, "the joined name is not a constructor or a destructor"
    if len(slot) != 2:
        return False, ("%d ROM addresses join it, and only a pair of structor "
                       "siblings can be one body emitted twice" % len(slot))
    names = sorted(n for a in slot for n in slot[a])
    if len(names) != 2:
        return False, ("the two addresses carry %d ROM names (%s)"
                       % (len(names), ", ".join(names)))
    letter = "C" if rec["meth"] == "ctor" else "D"
    stem = "_ZN" + "".join("%d%s" % (len(c), c) for c in rec["cls"])
    want = ["%s%s1Ev" % (stem, letter), "%s%s2Ev" % (stem, letter)]
    if names != want:
        return False, ("the pair is %s, not the %s1/%s2 siblings of one class"
                       % (", ".join(names), letter, letter))
    return structor_twin_proof(root, names[0], names[1])


def member_ptr_typed(sym):
    """True when an MSVC mangle names a pointer-to-member TYPE anywhere in
    its type text -- P8/Q8 direct, or under any wrapping such as PAP8 (an
    array of member pointers). Only the text after the first '@@' is
    scanned, so an identifier that happens to contain 'P8' cannot
    false-positive; the first version scanned the whole symbol.
    R8/S8 (the cv-qualified member-pointer spellings) are NOT tested for
    because none has ever appeared on a wall; if one arrives it falls to
    the struct or whitelist refusals below, which is still a refusal."""
    parts = sym.split("@@", 1)
    return len(parts) == 2 and ("P8" in parts[1] or "Q8" in parts[1])


# ---------------------------------------------------------------------------
# name parsing

def itanium_parse(name):
    """_ZN8dActor_c9UpdatePosEP5dCc_c -> (cls, meth, const, arity).

    Returns None for anything outside the two-component non-structor shape:
    nested namespaces, ctors/dtors and operators are not face material.
    Arity counting handles the subset the repo's mangles use -- builtins,
    P/R/K prefixes, <len>Name types with one template block, S substitutions.
    Anything else returns arity -1 and the row is refused rather than
    miscounted.
    """
    m = re.match(r"^_ZN(K?)(\d.*)$", name)
    if not m:
        return None
    is_const = m.group(1) == "K"
    comps, rest = [], m.group(2)
    while rest and rest[0].isdigit():
        n = re.match(r"(\d+)", rest)
        length = int(n.group(1))
        body = rest[n.end():n.end() + length]
        if len(body) != length:
            return None
        comps.append(body)
        rest = rest[n.end() + length:]
    if not rest.startswith("E") or len(comps) != 2:
        return None
    cls, meth = comps
    if re.match(r"^[CD][0-3]$", meth) or not meth[0].isalpha():
        return None
    return cls, meth, is_const, itanium_arity(rest[1:])


def itanium_arity(params):
    if params == "v":
        return 0
    i, n = 0, 0
    while i < len(params):
        c = params[i]
        if c in "PRK":
            i += 1
            continue
        if c.isdigit():
            m = re.match(r"(\d+)", params[i:])
            length = int(m.group(1))
            i += m.end() + length
            if i < len(params) and params[i] == "I":
                depth, i = 1, i + 1
                while i < len(params) and depth:
                    if params[i] == "I":
                        depth += 1
                    elif params[i] == "E":
                        depth -= 1
                    i += 1
            n += 1
            continue
        if c == "S":
            j = params.find("_", i)
            if j < 0:
                return -1
            i = j + 1
            n += 1
            continue
        if c == "N":
            # A nested-name type, N<components>E: Particle::System spelled as
            # N8Particle6SystemE. One argument, however many components.
            # Added by lane FACES1 so a parameter whose TYPE is a nested class
            # counts instead of poisoning the whole row's arity with -1.
            depth, i = 1, i + 1
            while i < len(params) and depth:
                if params[i] == "E":
                    depth -= 1
                i += 1
            if depth:
                return -1
            n += 1
            continue
        if c in "abcdefghijlmnostuvwxyz":
            i += 1
            n += 1
            continue
        return -1
    return n


_UNDNAME_EXE = []


def find_undname():
    """The absolute path of undname.exe, or None.

    WHY NOT JUST CALL VCVARS. undname needs nothing but its own directory,
    and calling vcvars32.bat to put it on PATH costs TWO MINUTES on this box
    (vswhere is missing here, so the batch takes its slow path). The sync
    ledger asks about more than a thousand symbols on every regeneration, and
    two minutes of batch file per build is two minutes nobody gets back.
    Measured: 127s for ONE undname call through vcvars, under a second for
    1385 through the direct path. The vcvars route stays as the fallback.
    """
    if _UNDNAME_EXE:
        return _UNDNAME_EXE[0]
    found = None
    vcvars = closure.find_vcvars()
    if vcvars:
        vcroot = os.path.dirname(os.path.dirname(os.path.dirname(
            os.path.abspath(vcvars))))          # ...\VC
        tools = os.path.join(vcroot, "Tools", "MSVC")
        if os.path.isdir(tools):
            for ver in sorted(os.listdir(tools), reverse=True):
                for host in ("Hostx86", "Hostx64"):
                    cand = os.path.join(tools, ver, "bin", host, "x86",
                                        "undname.exe")
                    if os.path.exists(cand):
                        found = cand
                        break
                if found:
                    break
    _UNDNAME_EXE.append(found)
    return found


def undname_batch(symbols):
    """{decorated: undecorated} via the toolchain's own undname.exe."""
    if not symbols:
        return {}
    exe = find_undname()
    if exe:
        return _undname_direct(exe, symbols)
    vcvars = closure.find_vcvars()
    if vcvars is None:
        return {}
    with tempfile.TemporaryDirectory() as td:
        runner = pathlib.Path(td) / "und.cmd"
        outp = pathlib.Path(td) / "und.txt"
        lines = ["@echo off", 'call "%s" >nul' % vcvars]
        # undname takes MANY symbols per invocation, and the sync ledger asks
        # about more than a thousand. One process per symbol measured over two
        # minutes for 1385 rows, on every build; chunked onto one command line
        # (well under cmd's 8191-character limit) it is a handful of spawns.
        chunk, width = [], 0
        for sym in symbols:
            piece = ' "%s"' % sym
            if chunk and width + len(piece) > 6000:
                lines.append('undname%s >> "%s"' % ("".join(chunk), outp))
                chunk, width = [], 0
            chunk.append(piece)
            width += len(piece)
        if chunk:
            lines.append('undname%s >> "%s"' % ("".join(chunk), outp))
        runner.write_text("\r\n".join(lines) + "\r\n")
        subprocess.run(["cmd", "/c", str(runner)], capture_output=True)
        text = outp.read_text(errors="replace") if outp.exists() else ""
    return _undname_parse(text)


def _undname_parse(text):
    result, key = {}, None
    for line in text.splitlines():
        m = re.match(r'Undecoration of :- "(.+)"', line)
        if m:
            key = m.group(1)
        m = re.match(r'is :- "(.+)"', line)
        if m and key:
            result[key] = m.group(1)
            key = None
    return result


def _undname_direct(exe, symbols):
    """undname.exe called straight, many symbols per invocation.

    The command line is capped well under Windows' 32k so a long ledger runs
    in a handful of spawns instead of one per symbol.
    """
    out = {}
    chunk, width = [], 0
    for sym in list(symbols) + [None]:
        piece = None if sym is None else sym
        if piece is None or (chunk and width + len(piece) + 3 > 24000):
            res = subprocess.run([exe] + chunk, capture_output=True)
            out.update(_undname_parse(
                res.stdout.decode("latin-1", errors="replace")))
            chunk, width = [], 0
        if piece is not None:
            chunk.append(piece)
            width += len(piece) + 3
    return out


# The access specifier is CAPTURED, not skipped. MSVC puts it in the mangle
# (Q public / A private / I protected non-virtual, U public / E private /
# M protected virtual), so ?InitResources@X@@QAEHXZ and ?InitResources@X@@UAEHXZ
# are DIFFERENT SYMBOLS. A shadow declaration that gets either half wrong emits
# a reference to a name nothing defines, and the row comes back as a fresh
# unresolved external with no hint of why. See faces_sync.txt's header.
SIG = re.compile(
    r"^(public|protected|private): "
    r"(virtual )?(.*?)__thiscall ([\w:]+)::(~?\w+)\((.*)\)(const )?\s*$")


def parse_param(text):
    """One undname param -> (kind, spelling, classname).

    kind: 'scalar' | 'ptr' | 'ref'; classname set for struct/class types.
    None means the row must be refused (the type is beyond the mechanical
    subset -- float, function pointer, template, by-value struct).
    """
    t = text.strip()
    if "<" in t or "(" in t:
        return None
    is_const = False
    # A QUALIFIED class name (Particle::System, dPa_c::level_c::callback_c) is
    # a class like any other at the ABI: the face takes a pointer either way.
    # What it needs beyond a plain name is a nested DECLARATION rather than a
    # bare `struct X;`, which emit_sync does from the same spelling.
    # A CLASS-SPELLED TYPE IS NOT A STRUCT-SPELLED ONE. MSVC mangles a class
    # as V and a struct as U, and every shadow this tool writes is a struct,
    # so a `class X *` parameter comes out as PAUX@@ against the definition's
    # PAVX@@ -- a face that names a symbol nothing defines. Nothing but
    # --verify's surface check ever caught it, and only after a compile. It is
    # a refusal here instead, with its reason, until a lane rules on giving
    # shadows a keyword and on what to do when two rows disagree about one
    # class. The old regexes accepted class and struct alike and dropped the
    # word.
    m = re.match(r"^(?:const )?(struct|class) ([\w:]+) (const )?([*&])$", t)
    if m:
        if m.group(1) == "class":
            return None
        kind = "ptr" if m.group(4) == "*" else "ref"
        return (kind, m.group(2),
                bool(m.group(3)) or t.startswith("const "))
    if t in SCALARS:
        return ("scalar", t, False)
    if t in ("void *", "void const *", "const void *"):
        return ("ptr", "void", "const" in t)
    # A REFERENCE TO A SCALAR (short &, signed char &) is a pointer at the
    # ABI, so the flat caller pushes an address and the face passes *(T *)a.
    # It is spelled as a 'ref' whose name is the scalar, which is what tells
    # the emitter not to forward-declare a struct called `short`.
    m = re.match(r"^(const )?(.+) &$", t)
    if m and m.group(2).strip() in SCALARS:
        return ("ref", m.group(2).strip(), bool(m.group(1)))
    return None


def parse_msvc_sig(und):
    """undname text -> dict or a refusal string."""
    m = SIG.match(und)
    if m is None:
        if "static" in und:
            return "static method"
        if "__thiscall" not in und:
            return "not __thiscall"
        return "unparsed signature"
    access, virtual, ret, cls, meth, params, constness = m.groups()
    ret = ret.strip()
    structor = None
    if meth.startswith("~"):
        structor = "dtor"
    elif cls.split("::")[-1] == meth:
        structor = "ctor"
    if structor and not ret:
        ret = "void"
    if ret in REFUSED_SCALARS or "<" in ret:
        return "refused return type %r" % ret
    if ret not in SCALARS and ret != "void":
        pm = parse_param(ret + " *")  # 'struct X *' shape check via reuse
        if ret.endswith("*") or ret.endswith("&"):
            rp = parse_param("struct " + ret if not
                             ret.startswith(("struct", "class")) else ret)
            if rp is None:
                return "refused return type %r" % ret
        else:
            return "struct-by-value return (SRET territory)"
    plist = []
    if params.strip() not in ("void", ""):
        for p in params.split(","):
            parsed = parse_param(p)
            if parsed is None:
                if re.match(r"^\s*(?:const )?class\b", p):
                    return ("refused param %r: a class-spelled type mangles V "
                            "where this file's struct shadows mangle U, so the "
                            "face would name a symbol nothing defines"
                            % p.strip())
                return "refused param %r" % p.strip()
            if parsed[0] == "scalar" and parsed[1] in REFUSED_SCALARS:
                return "refused param %r" % p.strip()
            plist.append(parsed)
    return {"virtual": bool(virtual), "access": access, "ret": ret,
            "cls": cls, "meth": meth, "params": plist,
            "const": bool(constness), "structor": structor}


# ---------------------------------------------------------------------------
# classification

def build_universe(targets):
    """targets (raw map/list spellings) -> lookup structures.

    One leading underscore is the x86 C decoration and is stripped for the
    JOIN KEY ONLY; raw spellings are kept for reporting. Nothing here strips
    more than one underscore (linkage.py's old lstrip("_") bug is the
    counterexample this comment exists for).

    Namespace-scope free functions (the @@Y-after-a-qualifier spelling) get
    their own table so the Itanium side of such a pair can be refused with
    the TRUE reason instead of "no MSVC method in the universe" -- the
    definition is there, it just is not a method (finding 4).
    """
    cnames, itanium_by_cm, msvc_by_cm, nsfree_by_cm = {}, {}, {}, {}
    for raw in targets:
        if raw.startswith("?"):
            m = MSVC_METHOD.match(raw)
            if m:
                msvc_by_cm.setdefault((m.group(2), m.group(1)),
                                      []).append(raw)
            else:
                m = MSVC_NSFREE.match(raw)
                if m:
                    nsfree_by_cm.setdefault((m.group(2), m.group(1)),
                                            []).append(raw)
            continue
        ident = raw[1:] if raw.startswith("_") else raw
        cnames[ident] = raw
        if ident.startswith("_ZN"):
            parsed = itanium_parse(ident)
            if parsed:
                cls, meth, is_const, arity = parsed
                itanium_by_cm.setdefault((cls, meth), []).append(
                    (ident, is_const, arity))
    return cnames, itanium_by_cm, msvc_by_cm, nsfree_by_cm


def classify(unresolved, universe):
    cnames, itanium_by_cm, msvc_by_cm, nsfree_by_cm = universe
    rows = {"ALIAS": [], "ALIAS_FN": [], "FACE": [], "RFACE": [],
            "WALL": [], "REFUSED": [], "MISSING": [], "UNKNOWN": []}
    need_undname = []
    for raw in unresolved:
        sym = raw
        if sym.startswith("?"):
            if member_ptr_typed(sym):
                rows["WALL"].append((raw, "pointer-to-member typed (P8/Q8 "
                                     "in the type text): the consumer names "
                                     "a member-pointer type, so it needs a "
                                     "host copy whether the pair is called "
                                     "or only copied -- the consumer-"
                                     "spelling law, mg_fanout_costs.txt s4; "
                                     "the gate-16 wall"))
                continue
            m = MSVC_DATA.match(sym)
            if m and m.group(2) is None:
                t = m.group(3)
                if re.match(r"^(?:[PQ][AB])*[UT]", t):
                    rows["REFUSED"].append((raw, "struct-typed global (%s): "
                                            "the mangle cannot show the "
                                            "struct's members, and a struct "
                                            "wrapper hides a member-pointer "
                                            "table -- @@3PAUEntry@@A was the "
                                            "dScMgCurling_c state table "
                                            "(s10 finding 1); rule the "
                                            "consumer's source by hand" % t))
                elif not SAFE_DATA_TYPE.match(t):
                    rows["REFUSED"].append((raw, "data spelling %r outside "
                                            "the scalar subset this tool "
                                            "has proven safe (the two-ints "
                                            "law); rule it by hand" % t))
                elif m.group(1) in cnames:
                    rows["ALIAS"].append((raw, m.group(1)))
                else:
                    rows["MISSING"].append((raw, "no C definition named %s"
                                            % m.group(1)))
                continue
            m = MSVC_FREE.match(sym)
            if m:
                if m.group(2) != "A":
                    rows["REFUSED"].append((raw, "non-cdecl free function: "
                                            "an alias cannot change a "
                                            "calling convention"))
                elif m.group(1) in cnames:
                    rows["ALIAS_FN"].append((raw, m.group(1)))
                else:
                    rows["MISSING"].append((raw, "no C definition named %s"
                                            % m.group(1)))
                continue
            m = MSVC_NSFREE.match(sym)
            if m:
                rows["REFUSED"].append((raw, "free function at NAMESPACE "
                                        "scope (%s::%s, the @@Y spelling): "
                                        "not a method and not face "
                                        "material, and ALIAS_FN joins "
                                        "global-scope names only -- a "
                                        "namespace member joined onto a "
                                        "bare C name could land on a "
                                        "different function; rule it by "
                                        "hand" % (m.group(2), m.group(1))))
                continue
            if MSVC_METHOD.match(sym):
                need_undname.append(raw)
                continue
            rows["UNKNOWN"].append((raw, "unclassified MSVC mangle"))
            continue
        ident = sym[1:] if sym.startswith("_") else sym
        if ident.startswith("_ZN"):
            parsed = itanium_parse(ident)
            if not parsed:
                rows["UNKNOWN"].append((raw, "Itanium shape outside the "
                                        "two-component subset"))
                continue
            cls, meth, is_const, arity = parsed
            cands = msvc_by_cm.get((cls, meth), [])
            if not cands:
                nsf = nsfree_by_cm.get((cls, meth), [])
                if nsf:
                    # Finding 4's original shape: the first version read
                    # this as a class method and refused the row as
                    # "reverse face with N args" -- right refusal, wrong
                    # reason, and a wrong reason in a refusal sends a lane
                    # to hand-write the wrong thing.
                    rows["REFUSED"].append((raw, "the MSVC definition %s "
                                            "is a namespace-scope __cdecl "
                                            "free function, not a method: "
                                            "no face and no argument "
                                            "re-landing is needed; when "
                                            "both sides are __cdecl with "
                                            "the same arguments a plain "
                                            "hand /alternatename is the "
                                            "whole fix -- the "
                                            "Memory::Allocate ruling, "
                                            "hal/scene_mg_faces.cpp 2d"
                                            % nsf[0]))
                else:
                    rows["MISSING"].append((raw, "no MSVC method %s::%s in "
                                            "the universe" % (cls, meth)))
            elif len(cands) > 1:
                rows["REFUSED"].append((raw, "ambiguous: %d MSVC candidates"
                                        % len(cands)))
            elif arity != 0:
                rows["REFUSED"].append((raw, "reverse face with %d args: "
                                        "hand-write it against the "
                                        "cxxname_bridge precedent" % arity))
            else:
                rows["RFACE"].append((raw, ident, cands[0]))
            continue
        rows["UNKNOWN"].append((raw, "plain C name: a face cannot supply "
                                "it, host or mount it"))

    und = undname_batch(need_undname + [c for r in rows["RFACE"]
                                        for c in [r[2]]])
    for raw in need_undname:
        sig = und.get(raw)
        if sig is None:
            rows["REFUSED"].append((raw, "undname produced nothing"))
            continue
        parsed = parse_msvc_sig(sig)
        if isinstance(parsed, str):
            rows["REFUSED"].append((raw, parsed + " [" + sig + "]"))
            continue
        cands = itanium_by_cm.get((parsed["cls"], parsed["meth"]), [])
        if not cands:
            rows["MISSING"].append((raw, "no Itanium body for %s::%s"
                                    % (parsed["cls"], parsed["meth"])))
            continue
        if len(cands) > 1:
            rows["REFUSED"].append((raw, "ambiguous: %d Itanium candidates"
                                    % len(cands)))
            continue
        ident, t_const, t_arity = cands[0]
        if t_arity < 0:
            rows["REFUSED"].append((raw, "target arity unparseable: %s"
                                    % ident))
            continue
        if t_arity != len(parsed["params"]):
            rows["REFUSED"].append((raw, "ARITY MISMATCH: caller %d vs "
                                    "body %d (%s) -- a ruling, not "
                                    "plumbing" % (len(parsed["params"]),
                                                  t_arity, ident)))
            continue
        rows["FACE"].append((raw, parsed, ident))

    # resolve the RFACE signatures now that undname has run
    resolved_rfaces = []
    for raw, ident, cand in rows["RFACE"]:
        sig = und.get(cand)
        parsed = parse_msvc_sig(sig) if sig else "undname produced nothing"
        if isinstance(parsed, str):
            rows["REFUSED"].append((raw, parsed))
            continue
        resolved_rfaces.append((raw, ident, cand, parsed))
    rows["RFACE"] = resolved_rfaces
    return rows


# ---------------------------------------------------------------------------
# emission

def c_type(kind, name, is_const):
    if kind == "scalar":
        return name
    return ("const void *" if is_const else "void *")


def cpp_type(kind, name, is_const):
    if kind == "scalar":
        return name
    base = ("const %s" % name) if is_const else name
    return base + (" *" if kind == "ptr" else " &")


def emit(rows, out):
    lines = [
        "// GENERATED by port/tools/facegen.py -- the mechanical rows only.",
        "// Every face here passed the exact-name join and the arity gate;",
        "// what no generator checks is method_faces.cpp's checklist item",
        "// (a), the plausible-sibling trap, so review each row's target",
        "// against its caller before wiring. This file includes NOTHING,",
        "// for hal/actor_class_faces.cpp's reasons.",
        ""]
    fwd, externs, structs, bodies, pragmas = set(), [], {}, [], []

    for raw, ident in rows["ALIAS"]:
        pragmas.append('#pragma comment(linker, "/alternatename:%s=_%s")'
                       % (raw, ident))
    for raw, ident in rows["ALIAS_FN"]:
        pragmas.append('#pragma comment(linker, "/alternatename:%s=_%s")'
                       % (raw, ident))

    for raw, sig, ident in rows["FACE"]:
        cls, meth = sig["cls"], sig["meth"]
        for kind, name, k in sig["params"]:
            if kind != "scalar" and name != "void":
                fwd.add(name)
        cparams = ["void *self"] + [c_type(*p) for p in sig["params"]]
        cret = sig["ret"] if sig["ret"] in SCALARS or sig["ret"] == "void" \
            else "void *"
        externs.append("%s %s(%s);" % (cret, ident, ", ".join(cparams)))
        argn = ["a%d" % i for i in range(len(sig["params"]))]
        mparams = ", ".join("%s %s" % (cpp_type(*p), n)
                            for p, n in zip(sig["params"], argn))
        structs.setdefault(cls, []).append(
            "    %s %s(%s)%s;" % (sig["ret"], meth, mparams,
                                  " const" if sig["const"] else ""))
        callargs = ["(void *)this" if sig["const"] else "this"]
        for p, n in zip(sig["params"], argn):
            kind = p[0]
            callargs.append("&" + n if kind == "ref" else n)
        retkw = "return " if sig["ret"] != "void" else ""
        bodies.append("%s %s::%s(%s)%s\n{ %s%s(%s); }"
                      % (sig["ret"], cls, meth, mparams,
                         " const" if sig["const"] else "",
                         retkw, ident, ", ".join(callargs)))

    for raw, ident, cand, sig in rows["RFACE"]:
        cls, meth = sig["cls"], sig["meth"]
        structs.setdefault(cls, []).append(
            "    %s %s()%s;  /* declared, DEFINED BY THE MATCHED TU */"
            % (sig["ret"], meth, " const" if sig["const"] else ""))
        cast = "(const %s *)" % cls if sig["const"] else "(%s *)" % cls
        selft = "const void *" if sig["const"] else "void *"
        retkw = "return " if sig["ret"] != "void" else ""
        bodies.append('extern "C" %s %s(%s self)\n{ %s(%sself)->%s::%s(); }'
                      % (sig["ret"], ident, selft, retkw, cast, cls, meth))

    for name in sorted(fwd):
        lines.append("struct %s;" % name)
    if externs:
        lines += ["", 'extern "C" {'] + externs + ["}"]
    for cls in sorted(structs):
        lines += ["", "struct %s {" % cls] + structs[cls] + ["};"]
    lines += [""] + bodies
    if pragmas:
        lines += [""] + pragmas
    text = "\n".join(lines) + "\n"
    pathlib.Path(out).write_text(text)
    return text


def verify(rows, genfile, scratch):
    """Compile the emitted file; its symbol surface must equal the request.

    genfile is resolved to an ABSOLUTE path first, and that one line is
    finding 2's fix: compile_batch resolves a relative source against its
    root argument, which this function sets to the output's own parent, so
    a relative --out carrying a directory component (build/faces.cpp)
    used to become <parent>/<parent>/<file>, a path that does not exist.
    cl was handed a missing file and the tool reported a FALSE 'DID NOT
    COMPILE' on a correct one -- and under the standing rule that no
    generated file is wired without a PASS, a false fail costs a lane the
    whole hand-written alternative.
    """
    genfile = pathlib.Path(genfile).resolve()
    results = closure.compile_batch(genfile.parent, [str(genfile)], scratch)
    syms = results[str(genfile)]
    if syms is None:
        return False, "generated file DID NOT COMPILE (see %s)" % scratch
    und, dfn = closure.read_syms(syms)
    want_def = set(r[0] for r in rows["FACE"])
    want_def |= set("_" + r[1] for r in rows["RFACE"])
    want_und = set("_" + r[2] for r in rows["FACE"]) | \
        set(r[2] for r in rows["RFACE"])  # RFACE r[2] = decorated target
    problems = []
    if dfn != want_def:
        problems.append("defined set mismatch: extra=%s missing=%s"
                        % (sorted(dfn - want_def),
                           sorted(want_def - dfn)))
    if not und <= want_und:
        problems.append("unexpected undefineds: %s" % sorted(und - want_und))
    return (not problems), "; ".join(problems) or "surface exact"


def report(rows):
    for bucket in ("FACE", "RFACE", "ALIAS", "ALIAS_FN", "WALL", "REFUSED",
                   "MISSING", "UNKNOWN"):
        items = rows[bucket]
        if not items:
            continue
        print("=== %s (%d) ===" % (bucket, len(items)))
        for it in items:
            if bucket in ("FACE", "RFACE"):
                print("    %s  ->  %s" % (it[0], it[2] if bucket == "FACE"
                                          else it[1]))
            else:
                print("    %s  --  %s" % (it[0], it[1]))


# ---------------------------------------------------------------------------

MSVC_QUAL = re.compile(r"^\?\?([01])((?:\w+@)+)@[A-Z]")
MSVC_MEM = re.compile(r"^\?(\w+)@((?:\w+@)+)@[A-Z]")


def comps_head(cls):
    """The innermost class name of a `Outer::Inner` spelling."""
    return cls.split("::")[-1]


def msvc_member_index(defined):
    """{(class components outermost-first, method key): [decorated names]}.

    The same key shape itanium_parse_ext produces, so the two sides join on
    equal keys rather than on a string prefix. MSVC writes a qualified name
    innermost first, so the components are reversed back here.
    """
    idx = {}
    for raw in defined:
        if not raw.startswith("?"):
            continue
        m = MSVC_QUAL.match(raw)
        if m:
            meth = "ctor" if m.group(1) == "0" else "~"
            quals = m.group(2).rstrip("@").split("@")
        else:
            m = MSVC_MEM.match(raw)
            if not m:
                continue
            meth = m.group(1)
            quals = m.group(2).rstrip("@").split("@")
        idx.setdefault((tuple(reversed(quals)), meth), []).append(raw)
    return idx


def derive_rows(flat_names, defined, root, und=None):
    """The ADDRESS BINDING, applied to a list of flat ROM names.

    Returns (rows, refusals). Each row is a dict carrying the flat name, the
    one decorated definition it binds to, the ROM address that proves the
    binding, the Itanium record and the undname signature. Every refusal
    carries the reason, in the form a reader can act on.
    """
    rom = rom_index(root)
    joins = rom_join_addresses(root)
    midx = msvc_member_index(defined)
    staged, refusals = [], []
    for raw in flat_names:
        ident = raw[1:] if raw.startswith("_") else raw
        rec = itanium_parse_ext(ident)
        if rec is None:
            refusals.append((raw, "Itanium shape outside the derivable subset "
                             "(operator, _ZTV/_ZTI, _ZSt, a template block in "
                             "a name component, or more than four components)"))
            continue
        entries = rom.get(ident)
        if not entries:
            refusals.append((raw, "rule 1: no ROM address for %s in "
                             "config/**/symbols.txt, so it is not a ROM "
                             "function and no face can be derived" % ident))
            continue
        addrs = sorted(set(a for a, _m in entries))
        if len(addrs) > 1:
            refusals.append((raw, "rule 1: %s is listed at %d different ROM "
                             "addresses (%s)"
                             % (ident, len(addrs),
                                ", ".join("0x%08x" % a for a in addrs))))
            continue
        if rec["meth"] == "~delete":
            refusals.append((raw, "rule 2: D0 is the DELETING destructor. "
                             "MSVC's is ??_G%s@@UAEPAXI@Z, the scalar "
                             "deleting destructor, which C++ source cannot "
                             "name; ??1 is its non-deleting sibling and "
                             "binding D0 to it would drop the free. The D0 "
                             "half is a flat .c TU's job"
                             % rec["cls"][-1]))
            continue
        key = (tuple(rec["cls"]), rec["meth"])
        slot = joins.get(key, {})
        if len(slot) > 1:
            twin, twhy = structor_twin_slot(root, rec, slot)
            if not twin:
                parts = []
                for a in sorted(slot):
                    parts.append("0x%08x %s" % (a, "/".join(sorted(slot[a]))))
                refusals.append((raw, "rule 2: %d ROM addresses join %s::%s, "
                                 "so one decorated definition cannot be all "
                                 "of them -- the plausible-sibling trap (%s); "
                                 "and the structor twin rule does not save it: "
                                 "%s"
                                 % (len(slot), cpp_qualified(rec),
                                    rec["meth"], "; ".join(parts), twhy)))
                continue
        cands = midx.get(key, [])
        if not cands:
            refusals.append((raw, "no MSVC definition of %s::%s in this link"
                             % (cpp_qualified(rec), rec["meth"])))
            continue
        if len(cands) > 1:
            refusals.append((raw, "ambiguous: %d MSVC definitions of %s::%s "
                             "(%s)" % (len(cands), cpp_qualified(rec),
                                       rec["meth"], ", ".join(sorted(cands)))))
            continue
        acc = decorated_access(cands[0])
        if acc is None:
            refusals.append((raw, "the access/virtualness letter of %s is not "
                             "a member-function code" % cands[0]))
            continue
        staged.append((raw, ident, rec, cands[0], addrs[0], acc))

    if und is None:
        und = undname_batch([t[3] for t in staged])
    rows = []
    for raw, ident, rec, cand, addr, acc in staged:
        text = und.get(cand)
        if not text:
            refusals.append((raw, "undname produced nothing for %s" % cand))
            continue
        sig = parse_msvc_sig(text)
        if isinstance(sig, str):
            refusals.append((raw, "%s [%s]" % (sig, text)))
            continue
        # rule 3, literally: class and method text, parameter count, constness.
        want_cls = cpp_qualified(rec)
        if sig["cls"] != want_cls:
            refusals.append((raw, "rule 3: %s undecorates to class %s, not %s"
                             % (cand, sig["cls"], want_cls)))
            continue
        if rec["meth"] == "~":
            want_meth = "~" + rec["cls"][-1]
        elif rec["meth"] == "ctor":
            want_meth = rec["cls"][-1]
        else:
            want_meth = rec["meth"]
        if sig["meth"] != want_meth:
            refusals.append((raw, "rule 3: %s undecorates to method %s, not %s"
                             % (cand, sig["meth"], want_meth)))
            continue
        if rec["arity"] < 0:
            refusals.append((raw, "rule 3: the Itanium parameter list of %s "
                             "is outside the counted subset, so the arity "
                             "cannot be compared" % ident))
            continue
        if rec["arity"] != len(sig["params"]):
            refusals.append((raw, "rule 3: ARITY MISMATCH, the ROM name takes "
                             "%d and %s takes %d -- a ruling, not plumbing"
                             % (rec["arity"], cand, len(sig["params"]))))
            continue
        if rec["const"] != sig["const"]:
            refusals.append((raw, "rule 3: constness disagrees, ROM name %s "
                             "and %s %s"
                             % ("const" if rec["const"] else "non-const", cand,
                                "const" if sig["const"] else "non-const")))
            continue
        if (sig["access"], sig["virtual"]) != acc:
            refusals.append((raw, "rule 3: the mangle's access letter says "
                             "%s/%s and undname says %s/%s"
                             % (acc[0], "virtual" if acc[1] else "non-virtual",
                                sig["access"],
                                "virtual" if sig["virtual"] else
                                "non-virtual")))
            continue
        rows.append({"flat": raw, "ident": ident, "rec": rec, "target": cand,
                     "addr": addr, "sig": sig})
    return rows, refusals


def derive_forward_rows(decorated, defined, root, und=None):
    """The address binding in the OTHER direction: a decorated CALLER name.

    A caller TU declares a shadow class and calls a member of it, so MSVC
    emits a __thiscall reference to ?meth@Cls@@... that nothing defines --
    while the port carries the body under the ROM's flat C name. The face is
    the mirror of the reverse one: a real __thiscall DEFINITION whose body
    calls the flat name with the receiver as its first stack argument.

    The same three rules apply, in the same order, and the ROM address is
    still what binds: undname says which class and method the decoration
    names, exactly ONE ROM address may join that class and method, and the
    flat name at that address must be defined in this link.
    """
    joins = rom_join_addresses(root)
    rom = rom_index(root)
    if und is None:
        und = undname_batch([d for d in decorated if d.startswith("?")])
    rows, refusals = [], []
    for raw in decorated:
        if not (MSVC_METHOD.match(raw) or MSVC_STRUCTOR.match(raw)):
            refusals.append((raw, "not a class-member mangle"))
            continue
        text = und.get(raw)
        if not text:
            refusals.append((raw, "undname produced nothing"))
            continue
        sig = parse_msvc_sig(text)
        if isinstance(sig, str):
            refusals.append((raw, "%s [%s]" % (sig, text)))
            continue
        if sig["structor"]:
            # A FORWARD FACE FOR A STRUCTOR DRAGS THE WHOLE CLASS WITH IT, and
            # this file must not own any of it. Defining Cls::~Cls() or
            # Cls::Cls() over a shadow that declares one virtual member makes
            # MSVC emit that class's VFTABLE (??_7Cls@@6B@) and its scalar
            # deleting destructor (??_GCls@@UAEPAXI@Z) from this object, and
            # reference the vector one (??_ECls@@), operator delete and
            # type_info besides. Measured, not feared: widening the gate to
            # structors put eight such definitions and six such references on
            # the generated file's surface in one build. ??_7 would duplicate
            # or displace whatever else emits that class's table, and ??_G is
            # the deleting destructor the whole D-row mechanism exists because
            # nothing can name -- satisfying it here with a host body is a
            # ruling, not plumbing. So the gate above accepts a structor and
            # this refuses it with the true reason, which is better than the
            # old "not a class-member mangle": that was never why.
            refusals.append((raw, "a forward face cannot DEFINE a structor: "
                             "MSVC emits the class's vftable ??_7%s@@6B@ and "
                             "its deleting destructor ??_G%s@@ from the same "
                             "object, and this file must not own either. The "
                             "reverse direction (a face for the ROM's own "
                             "flat name) has no such cost"
                             % (comps_head(sig["cls"]), comps_head(sig["cls"]))))
            continue
        comps = sig["cls"].split("::")
        if sig["structor"] == "dtor":
            meth = "~"
        elif sig["structor"] == "ctor":
            meth = "ctor"
        else:
            meth = sig["meth"]
        key = (tuple(comps), meth)
        slot = joins.get(key, {})
        if not slot:
            refusals.append((raw, "no ROM name for %s::%s in "
                             "config/**/symbols.txt" % (sig["cls"], meth)))
            continue
        if len(slot) > 1:
            parts = ["0x%08x %s" % (a, "/".join(sorted(slot[a])))
                     for a in sorted(slot)]
            refusals.append((raw, "rule 2: %d ROM addresses join %s::%s -- "
                             "the plausible-sibling trap (%s)"
                             % (len(slot), sig["cls"], meth,
                                "; ".join(parts))))
            continue
        addr = list(slot)[0]
        cands = [n for n in slot[addr]
                 if ("_" + n) in defined or n in defined]
        if not cands:
            refusals.append((raw, "the ROM body at 0x%08x (%s) is not "
                             "defined in this link"
                             % (addr, "/".join(sorted(slot[addr])))))
            continue
        if len(cands) > 1:
            refusals.append((raw, "%d defined flat spellings at 0x%08x (%s)"
                             % (len(cands), addr, "/".join(sorted(cands)))))
            continue
        ident = cands[0]
        rec = itanium_parse_ext(ident)
        if rec is None:
            refusals.append((raw, "the ROM name %s is outside the "
                             "derivable subset" % ident))
            continue
        if rec["arity"] < 0:
            refusals.append((raw, "rule 3: the parameter list of %s is "
                             "outside the counted subset" % ident))
            continue
        if rec["arity"] != len(sig["params"]):
            refusals.append((raw, "rule 3: ARITY MISMATCH, the caller spells "
                             "%d and the ROM body %s takes %d -- a ruling, "
                             "not plumbing"
                             % (len(sig["params"]), ident, rec["arity"])))
            continue
        if rec["const"] != sig["const"]:
            refusals.append((raw, "rule 3: constness disagrees between %s "
                             "and %s" % (raw, ident)))
            continue
        acc = decorated_access(raw)
        if acc is None or (sig["access"], sig["virtual"]) != acc:
            refusals.append((raw, "the access/virtualness letter does not "
                             "agree with undname"))
            continue
        rows.append({"flat": "_" + ident, "ident": ident, "rec": rec,
                     "target": raw, "addr": addr, "sig": sig,
                     "forward": True})

    # TWO CALLER SPELLINGS OF ONE MEMBER. Player::GetHealth is asked for as
    # ?GetHealth@Player@@QAEEXZ by one caller and ?GetHealth@Player@@QAEHXZ by
    # another: same class, same method, same parameter list, different return
    # width. Both are honest (eax carries either) and both are unresolved, but
    # ONE SHADOW CLASS CANNOT DECLARE BOTH -- C++ has no overload on return
    # type. The first spelling by name is emitted and the rest are refused
    # with the reason, so the leftovers go to a hand file rather than
    # disappearing quietly.
    seen, kept = {}, []
    for r in sorted(rows, key=lambda x: x["target"]):
        sig = r["sig"]
        k = (tuple(sig["cls"].split("::")), sig["meth"],
             tuple(c_type(*p) for p in sig["params"]), sig["const"])
        if k in seen:
            refusals.append((r["target"], "a second caller spelling of "
                             "%s::%s with the same parameters and a "
                             "different return type; %s is emitted and one "
                             "shadow class cannot declare both, so this one "
                             "needs a hand file"
                             % (sig["cls"], sig["meth"], seen[k])))
            continue
        seen[k] = r["target"]
        kept.append(r)
    return kept, refusals


# The two deallocations a ROM D0 body is allowed to add to its D1, and how the
# emitted face spells each. Memory is a NAMESPACE, so Deallocate's definition in
# this link is the C++ one (?Deallocate@Memory@@YAXPAXPAVHeap@@@Z) and the face
# names the method; operator_delete2 is carried under its flat C name by
# hal/cxxname_bridge.cpp and the face declares that. Neither is guessed: the
# ledger row names the call and d0_reloc_proof checks the ROM agrees.
DEALLOCATORS = {
    "_ZN6Memory10DeallocateEPvP4Heap": {
        "heap": True,
        "decl": ['extern "C" void _ZN6Memory10DeallocateEPvP4Heap('
                 "void *ptr, void *heap);"],
        "undefined": "__ZN6Memory10DeallocateEPvP4Heap",
    },
    "_ZN6Memory16operator_delete2EPv": {
        "heap": False,
        "decl": ['extern "C" void _ZN6Memory16operator_delete2EPv(void *p);'],
        "undefined": "__ZN6Memory16operator_delete2EPv",
    },
}


def derive_d0_rows(want, defined, root, und=None):
    """The D0 RULE, applied to ledger rows of direction D.

    `want` is [(flat D0 name, callee, D0 address, "D", deallocator, heap)].
    The callee is either the flat ROM D1 name (when this link defines it) or
    the decorated MSVC destructor of the same class; the heap column is the
    ROM symbol the D0's pooled word relocates to, or "-" for the
    operator_delete2 shape, which takes none.

    Returns (rows, refusals). Nothing here is derived from a name alone: every
    row is re-proved against config/**/relocs.txt by d0_reloc_proof.
    """
    rom = rom_index(root)
    rows, refusals = [], []
    staged = []
    for flat, callee, addr, _d, dealloc, heap in want:
        ident = flat[1:] if flat.startswith("_") else flat
        rec = itanium_parse_ext(ident)
        if rec is None or rec["meth"] != "~delete":
            refusals.append((flat, "a D row must name a D0 destructor and %s "
                             "is not one" % ident))
            continue
        ents = rom.get(ident)
        if not ents:
            refusals.append((flat, "rule 1: no ROM address for %s" % ident))
            continue
        addrs = sorted(set(a for a, _m in ents))
        if len(addrs) != 1:
            refusals.append((flat, "rule 1: %s is listed at %d ROM addresses"
                             % (ident, len(addrs))))
            continue
        if addrs[0] != addr:
            refusals.append((flat, "LEDGER DISAGREES: the row names ROM "
                             "0x%08x and config says 0x%08x"
                             % (addr, addrs[0])))
            continue
        spec = DEALLOCATORS.get(dealloc)
        if spec is None:
            refusals.append((flat, "%s is not one of the two deallocations a "
                             "D0 body may add (%s)"
                             % (dealloc, ", ".join(sorted(DEALLOCATORS)))))
            continue
        dents = rom.get(dealloc)
        if not dents or len(set(a for a, _m in dents)) != 1:
            refusals.append((flat, "no single ROM address for %s" % dealloc))
            continue
        dealloc_addr = dents[0][0]
        heap_addr = None
        if spec["heap"]:
            if heap == "-":
                refusals.append((flat, "%s takes a heap pointer and the row "
                                 "names none" % dealloc))
                continue
            hents = rom.get(heap)
            if not hents or len(set(a for a, _m in hents)) != 1:
                refusals.append((flat, "no single ROM address for the heap "
                                 "word %s" % heap))
                continue
            heap_addr = hents[0][0]
        elif heap != "-":
            refusals.append((flat, "%s takes no heap pointer, so the row's "
                             "heap column must be '-', not %s"
                             % (dealloc, heap)))
            continue
        d1name = ident[:-4] + "D1Ev"
        info, why = d0_reloc_proof(root, ident, d1name, dealloc_addr,
                                   heap_addr)
        if info is None:
            refusals.append((flat, "the ROM relocations no longer say this D0 "
                             "is its D1 plus one deallocation: %s" % why))
            continue
        if callee.startswith("?"):
            staged.append((flat, ident, rec, callee, addr, dealloc, heap,
                           d1name))
            continue
        if callee != d1name:
            refusals.append((flat, "a flat callee must be the class's own D1, "
                             "%s, not %s" % (d1name, callee)))
            continue
        if ("_" + callee) not in defined and callee not in defined:
            refusals.append((flat, "the D1 body %s is not defined in this "
                             "link, so the D0 face would call nothing"
                             % callee))
            continue
        rows.append({"flat": flat, "ident": ident, "rec": rec,
                     "target": callee, "addr": addr, "sig": None,
                     "d0": True, "dealloc": dealloc, "heap": heap,
                     "flat_callee": True})

    if staged:
        if und is None:
            und = undname_batch([t[3] for t in staged])
        for flat, ident, rec, callee, addr, dealloc, heap, d1name in staged:
            if callee not in defined:
                refusals.append((flat, "the destructor %s is not defined in "
                                 "this link, so the D0 face would call "
                                 "nothing" % callee))
                continue
            text = und.get(callee)
            if not text:
                refusals.append((flat, "undname produced nothing for %s"
                                 % callee))
                continue
            sig = parse_msvc_sig(text)
            if isinstance(sig, str):
                refusals.append((flat, "%s [%s]" % (sig, text)))
                continue
            want_cls = cpp_qualified(rec)
            if sig["cls"] != want_cls:
                refusals.append((flat, "rule 3: %s undecorates to class %s, "
                                 "not %s" % (callee, sig["cls"], want_cls)))
                continue
            if sig["meth"] != "~" + rec["cls"][-1]:
                refusals.append((flat, "rule 3: %s undecorates to method %s, "
                                 "not the destructor of %s"
                                 % (callee, sig["meth"], want_cls)))
                continue
            if sig["params"]:
                refusals.append((flat, "rule 3: %s takes %d parameters and a "
                                 "destructor takes none"
                                 % (callee, len(sig["params"]))))
                continue
            acc = decorated_access(callee)
            if acc is None or (sig["access"], sig["virtual"]) != acc:
                refusals.append((flat, "the access/virtualness letter of %s "
                                 "does not agree with undname" % callee))
                continue
            rows.append({"flat": flat, "ident": ident, "rec": rec,
                         "target": callee, "addr": addr, "sig": sig,
                         "d0": True, "dealloc": dealloc, "heap": heap,
                         "flat_callee": False})
    return rows, refusals


def refuse_face_cycles(rows):
    """Take out any pair of faces that would call each other forever.

    THE TRAP, found while re-deriving against a link that already carried the
    generated file. A reverse face DEFINES the flat ROM name and CALLS the
    decorated member; a forward face DEFINES the decorated member and CALLS the
    flat ROM name. Derive both for the same function and each one's body is the
    other: ?Behavior@BowserPuzzleManager@@QAEHXZ calls
    __ZN19BowserPuzzleManager8BehaviorEv calls
    ?Behavior@BowserPuzzleManager@@QAEHXZ, forever, until the stack runs out.

    It cannot happen from one wall, because a name the link already resolves is
    not on the wall. It happens the moment a lane re-derives with the generated
    object in the universe: the reverse face's own definition of the flat name
    reads as "the ROM body is defined in this link", and the forward face binds
    straight onto it. The link is clean, --verify is clean, and the fault is a
    stack overflow the first time the function is called.

    So the two directions are cross-checked here rather than trusted. The
    REVERSE row is the one kept: it is the ROM's own name, which is what the
    ROM's callers ask for.
    """
    rev = set(r["flat"] for r in rows if not r.get("forward"))
    revtgt = set(r["target"] for r in rows if not r.get("forward")
                 and not r.get("d0"))
    kept, refusals = [], []
    dropped_fwd = set()
    for r in rows:
        if r.get("forward") and (r["flat"] in rev or r["target"] in revtgt):
            refusals.append((r["target"], "FACE CYCLE: a reverse face in this "
                             "same set defines %s and calls %s, so a forward "
                             "face here would make the two call each other "
                             "until the stack runs out. The reverse row is "
                             "kept" % (r["flat"], r["target"])))
            dropped_fwd.add(r["target"])
            continue
        kept.append(r)
    # A D0 face runs the class's destructor. When the only definition of that
    # destructor was a forward face this guard just dropped, the D0 face would
    # call nothing, so it goes with it rather than becoming a dangling row.
    kept2 = []
    for r in kept:
        if (r.get("d0") and not r.get("flat_callee")
                and r["target"] in dropped_fwd):
            refusals.append((r["flat"], "the D0 face runs %s and the face "
                             "cycle guard just dropped the only definition "
                             "of it" % r["target"]))
            continue
        kept2.append(r)
    return kept2, refusals


# ---------------------------------------------------------------------------
# the sync-face emitter

def _fwd_from(text):
    """The class name a 'struct X *' / 'class X &' spelling names, or None.

    The name may be QUALIFIED (Particle::SysTracker::Contents::Entry), which
    is what a nested return type undecorates to.
    """
    m = re.match(r"^(?:const )?(?:struct|class) ([\w:]+) (?:const )?[*&]$",
                 text.strip())
    return m.group(1) if m else None


def _shadow_comps(r):
    """The class components the shadow declaration must carry.

    For a reverse face that is the ROM name's own qualification; for a
    forward face it is the CALLER's, read off the decoration through undname,
    because the shadow has to reproduce the caller's mangle exactly.
    """
    if r.get("forward"):
        return r["sig"]["cls"].split("::")
    return r["rec"]["cls"]


def _face_signature(r):
    """(return type, name, [(C parameter type, argument name)]) for one face.

    The return type is the target's own C++ spelling (so a pointer return
    keeps its type and the forward declaration above covers it); the
    parameters are the C-side ones the ROM's caller pushes, receiver first.
    One helper, used by the definition, by the forward prototype and by the
    friend declaration, so the three cannot drift apart.
    """
    rec, sig = r["rec"], r["sig"]
    # "~delete" is the D0 face: it runs the destructor and then hands the
    # storage back, and like every destructor face it returns nothing.
    ret = "void" if rec["meth"] in ("~", "~delete", "ctor") else sig["ret"]
    cparams = [("const void *" if sig["const"] else "void *", "self")]
    for i, pr in enumerate(sig["params"]):
        cparams.append((c_type(*pr), "a%d" % i))
    return ret, r["ident"], cparams


def _spell_params(cparams, with_names):
    out = []
    for ctype, name in cparams:
        if not with_names:
            out.append(ctype.rstrip())
        elif ctype.endswith("*"):
            out.append("%s%s" % (ctype, name))
        else:
            out.append("%s %s" % (ctype, name))
    return ", ".join(out)


def _member_decl(rec, sig, comps=None):
    """The shadow declaration: the target's REAL access and virtualness.

    Both halves are in the decoration, so both have to be reproduced or the
    emitted reference names a symbol nothing defines.
    """
    cls = (comps or rec["cls"])[-1]
    virt = "virtual " if sig["virtual"] else ""
    params = ", ".join(cpp_type(*p) for p in sig["params"])
    if rec["meth"] in ("~", "~delete"):
        body = "%s~%s()" % (virt, cls)
    elif rec["meth"] == "ctor":
        body = "%s(%s)" % (cls, params)
    else:
        body = "%s%s %s(%s)%s" % (virt, sig["ret"], sig["meth"], params,
                                  " const" if sig["const"] else "")
    return "%s: %s;" % (sig["access"], body)


def _node(tree, comps):
    """Walk/create the shadow-tree node for a list of class components."""
    cur, node = tree, None
    for c in comps:
        node = cur.setdefault(c, {"kids": {}, "decls": [], "fwds": set()})
        node.setdefault("fwds", set())
        cur = node["kids"]
    return node


def _walk(tree, prefix=()):
    """[(path tuple, node)] over the whole shadow tree."""
    out = []
    for name in sorted(tree):
        path = prefix + (name,)
        out.append((path, tree[name]))
        out += _walk(tree[name]["kids"], path)
    return out


QUAL_USE = re.compile(r"\b(\w+(?:::\w+)+)\b")


def _render_struct(path, node):
    """One class's definition, written OUT OF LINE.

    THE SHADOWS ARE FLATTENED, not nested, and the reason is a parameter type
    that names another tree's nested class. dPa_c::level_c::callback_c's
    members take a Particle::System &, and Particle::SysTracker::Contents'
    members take a dPa_c::level_c::callback_c *: each needs the OTHER tree's
    nested name declared before its own body is read, and a class cannot be
    reopened to add that later. Written out of line the two orders are
    separable -- `struct Particle { struct System; ... };` declares the name,
    `struct dPa_c::level_c::callback_c { ... };` uses it, and
    `struct Particle::SysTracker::Contents { ... };` comes after both. The
    mangling is identical either way; what changes is only what is declared by
    the time a body is read.
    """
    out = ["struct %s {" % "::".join(path)]
    # nested declarations first, so a member of this same shadow may use one
    for f in sorted(node.get("fwds", ())):
        out.append("    struct %s;" % f)
    for kid in sorted(node["kids"]):
        out.append("    struct %s;" % kid)
    for d in node["decls"]:
        out.append("    %s" % d)
    out.append("};")
    return out


def _render_tree(tree):
    """Every shadow class, out of line, in an order that always compiles.

    The order obeys two edges. A class is declared by its ENCLOSING class, so
    a parent is written before its children. And a class whose member
    declarations name a qualified type needs that type's enclosing class
    written first, which is the edge that makes the flattening worth anything.
    Ties break alphabetically so the generated file is stable.
    """
    nodes = dict(_walk(tree))
    deps = {p: set() for p in nodes}
    for path, node in nodes.items():
        if len(path) > 1:
            deps[path].add(path[:-1])
        for d in node["decls"]:
            for use in QUAL_USE.findall(d):
                owner = tuple(use.split("::"))[:-1]
                if owner in nodes and owner != path:
                    deps[path].add(owner)
    out, done = [], set()
    ready = sorted(nodes)
    while len(done) < len(nodes):
        progressed = False
        for path in ready:
            if path in done or deps[path] - done:
                continue
            out += _render_struct(path, nodes[path]) + [""]
            done.add(path)
            progressed = True
        if not progressed:
            stuck = sorted("::".join(p) for p in nodes if p not in done)
            sys.exit("facegen: the shadow classes %s depend on each other's "
                     "nested names in a cycle, so no order of definitions "
                     "declares every name before it is used. Refuse the rows "
                     "that cross, do not reorder by hand." % ", ".join(stuck))
    return out


def emit_sync(rows, out, header_note=""):
    """One generated source for the derived sync faces."""
    lines = [
        "// GENERATED by port/tools/facegen.py --sync. DO NOT EDIT.",
        "// The input is the checked-in ledger port/faces_sync.txt; every row",
        "// below was re-derived from it at build time and every row that",
        "// failed the address binding was REFUSED rather than guessed.",
        "// This file includes NOTHING, for hal/actor_class_faces.cpp's",
        "// reasons: the shadow declarations exist to reproduce a mangle, not",
        "// to describe a type, and a real header would fight them.",
        "//",
        "// Each face is  extern \"C\" RET _ZN...(void *self, args...)  whose",
        "// body is the QUALIFIED call ((Cls *)self)->Cls::meth(args), so it",
        "// dispatches directly to the member the ROM name names and never",
        "// through a vtable slot back into itself.",
    ]
    if header_note:
        lines += ["//", "// " + header_note]
    lines.append("")

    # forward declarations for every class named in a parameter or return
    fwd = set()
    shadow_names = set()
    for r in rows:
        if r.get("d0") and r.get("flat_callee"):
            continue            # a flat-callee D0 face declares no class
        shadow_names.add(_shadow_comps(r)[0])
    for r in rows:
        sig = r["sig"]
        if sig is None:
            continue
        for kind, name, _c in sig["params"]:
            # a 'ref' to a SCALAR carries the scalar's own spelling, and
            # `struct short;` is not a declaration of anything
            if kind != "scalar" and name != "void" and name not in SCALARS:
                fwd.add(name)
        n = _fwd_from(sig["ret"])
        if n:
            fwd.add(n)
    # A QUALIFIED name cannot be forward-declared from the outside, so it is
    # handed to the shadow tree instead and its enclosing class declares it.
    nested_fwd = sorted(n for n in fwd if "::" in n)
    fwd = set(n for n in fwd if "::" not in n)
    # EVERY class name gets a forward declaration first, shadows included.
    # The shadow definitions are emitted in name order and one of them can
    # name another as a parameter type (ExpandingHeap's constructor takes an
    # ExpandingHeapAllocator *), so without this the later one is an unknown
    # identifier at the point of use.
    for name in sorted(fwd | shadow_names | set(n.split("::")[0]
                                                for n in nested_fwd)):
        lines.append("struct %s;" % name)
    lines.append("")
    if any(r["rec"]["meth"] == "ctor" for r in rows):
        lines += [
            "// A CONSTRUCTOR FACE IS A PLACEMENT NEW, because C++ has no other",
            "// way to spell 'call this constructor on memory the caller owns'.",
            "// The placement form of operator new is declared here rather than",
            "// included, for this file's include-nothing rule: it is inline and",
            "// returns its argument, so it emits no symbol of its own and the",
            "// new-expression compiles to one call of ??0Cls@@ with the",
            "// receiver in ecx, which is exactly what the ROM's C1/C2 is. No",
            "// vptr is stored at the call site; the ROM's own constructor body",
            "// does that, the way it does on the DS.",
            "inline void *operator new(unsigned int, void *p) { return p; }",
            "",
        ]

    # the shadow classes
    tree = {}
    prototypes = []
    for name in nested_fwd:
        comps = name.split("::")
        parent = _node(tree, comps[:-1])
        if comps[-1] not in parent["kids"]:
            parent["fwds"].add(comps[-1])
    for r in rows:
        if r.get("d0") and r.get("flat_callee"):
            continue
        rec = r["rec"]
        node = _node(tree, _shadow_comps(r))
        d = _member_decl(rec, r["sig"], _shadow_comps(r))
        if d not in node["decls"]:
            node["decls"].append(d)
        if r["sig"]["access"] != "public":
            # A NON-PUBLIC TARGET still has to be called, and the access is
            # half of the decorated name (A private, I protected), so the
            # shadow cannot simply declare it public to get at it. The face
            # is made a FRIEND of the shadow instead: the declaration refers
            # to the extern "C" prototype emitted above the structs, the
            # call is legal, and the emitted reference is still the private
            # mangle the definition carries.
            ret, name, cparams = _face_signature(r)
            fr = "friend %s ::%s(%s);" % (ret, name,
                                          _spell_params(cparams, False))
            if fr not in node["decls"]:
                node["decls"].append(fr)
            prototypes.append('extern "C" %s %s(%s);'
                              % (ret, name, _spell_params(cparams, True)))
    if prototypes:
        lines += [
            "// Prototypes for the faces whose target is NOT public. Each is",
            "// befriended by its shadow below, which is how a private or",
            "// protected member is called without changing the access -- and",
            "// the access is half of the decorated name.",
        ] + sorted(set(prototypes)) + [""]
    lines += _render_tree(tree)

    # the flat bodies a FORWARD face calls, declared extern "C"
    fwd_rows = [r for r in rows if r.get("forward")]
    if fwd_rows:
        lines += [
            "// The ROM bodies the FORWARD faces call. A forward face is the",
            "// mirror of a reverse one: the caller's TU emitted a __thiscall",
            "// reference to a member of its own shadow class, and the port",
            "// carries that body under the ROM's flat C name, so the face is",
            "// a real __thiscall DEFINITION whose body passes the receiver",
            "// on as the first stack argument.",
            'extern "C" {']
        for r in sorted(fwd_rows, key=lambda x: x["ident"]):
            ret, name, cparams = _face_signature(r)
            lines.append("%s %s(%s);" % (ret, name,
                                         _spell_params(cparams, True)))
        lines += ["}", ""]

    # what a D0 face needs beyond its D1: the one deallocation, and the heap
    # pointer word the ROM body reads out of its own literal pool.
    d0_rows = [r for r in rows if r.get("d0")]
    if d0_rows:
        lines += [
            "// THE D0 FACES. MSVC's deleting destructor is ??_GCls@@UAEPAXI@Z,",
            "// which C++ source cannot name, so a D0 cannot be bound to a",
            "// definition the way every other row here is. What it can be is",
            "// WRITTEN: the ROM's own D0 body is its D1 plus exactly one call,",
            "// proved per class against config/**/relocs.txt, so each face",
            "// below runs the destructor and then makes that one call. The",
            "// heap pointer is Deallocate's second argument, read out of the",
            "// D0's own literal pool on the DS; the pooled word is relocated,",
            "// so the ROM says per class which symbol it is and no default is",
            "// ever used -- getting it wrong is heap corruption.",
            "//",
            "// BOTH CALLS ARE MADE BY THE ROM'S OWN FLAT NAME, which is what",
            "// the D0's relocation names and what the port already resolves:",
            "// operator_delete2 is defined under it in hal/cxxname_bridge.cpp",
            "// and Deallocate reaches its C++ definition through the",
            "// /alternatename in hal/cxx_aliases.cpp. Spelling Deallocate as",
            "// a C++ method here instead would need a `class Heap` in a file",
            "// whose shadows already declare `struct Heap`, and MSVC mangles",
            "// the two differently (PAVHeap vs PAUHeap), so the face would",
            "// name a symbol nothing defines.",
        ]
        decls = []
        for r in d0_rows:
            decls += DEALLOCATORS[r["dealloc"]]["decl"]
        heaps = sorted(set(r["heap"] for r in d0_rows if r["heap"] != "-"))
        for h in heaps:
            decls.append('extern "C" void *%s;' % h)
        seen_d = []
        for d in decls:
            if d not in seen_d:
                seen_d.append(d)
        lines += seen_d
        flatcallees = sorted(set(r["target"] for r in d0_rows
                                 if r.get("flat_callee")))
        if flatcallees:
            lines += ["// The D1 bodies the D0 faces run, under the ROM's own "
                      "flat name.", 'extern "C" {']
            lines += ["void %s(void *self);" % n for n in flatcallees]
            lines += ["}"]
        lines.append("")

    # the faces
    for r in sorted(rows, key=lambda x: x["flat"]):
        rec, sig = r["rec"], r["sig"]
        if r.get("d0"):
            spec = DEALLOCATORS[r["dealloc"]]
            if spec["heap"]:
                free = ("_ZN6Memory10DeallocateEPvP4Heap(self, %s);"
                        % r["heap"])
            else:
                free = "_ZN6Memory16operator_delete2EPv(self);"
            if r.get("flat_callee"):
                run = "%s(self);" % r["target"]
            else:
                qual = cpp_qualified(rec)
                run = "((%s *)self)->%s::~%s();" % (qual, qual,
                                                    rec["cls"][-1])
            lines.append("/* ROM 0x%08x %s = %s + %s */"
                         % (r["addr"], r["ident"],
                            r["target"], r["dealloc"]))
            lines.append('extern "C" void %s(void *self)' % r["ident"])
            lines.append("{ %s %s }" % (run, free))
            lines.append("")
            continue
        if r.get("forward"):
            argn = ["a%d" % i for i in range(len(sig["params"]))]
            mparams = ", ".join(
                "%s %s" % (cpp_type(*pr), n)
                for pr, n in zip(sig["params"], argn))
            callargs = ["(void *)this" if sig["const"] else "this"]
            for pr, n in zip(sig["params"], argn):
                callargs.append("&" + n if pr[0] == "ref" else n)
            retkw = "return " if sig["ret"] != "void" else ""
            lines.append("/* ROM 0x%08x %s -> the caller's %s */"
                         % (r["addr"], r["ident"], r["target"]))
            # A STRUCTOR IS DECLARED WITHOUT A RETURN TYPE, and undname reports
            # one for it anyway (`void` for a destructor, the class for a
            # constructor). Writing it out is C2577/C2533 and the whole
            # generated file stops compiling, so the two are spelled apart.
            if sig["structor"]:
                retkw = ""
                lines.append("%s::%s(%s)"
                             % (cpp_qualified(rec) if len(rec["cls"]) > 1
                                else rec["cls"][0], sig["meth"], mparams))
            else:
                lines.append("%s %s::%s(%s)%s"
                             % (sig["ret"], cpp_qualified(rec) if
                                len(rec["cls"]) > 1 else rec["cls"][0],
                                sig["meth"], mparams,
                                " const" if sig["const"] else ""))
            lines.append("{ %s%s(%s); }"
                         % (retkw, r["ident"], ", ".join(callargs)))
            lines.append("")
            continue
        qual = cpp_qualified(rec)
        cls = rec["cls"][-1]
        argn = ["a%d" % i for i in range(len(sig["params"]))]
        _ret, _name, _cp = _face_signature(r)
        cparams = [_spell_params(_cp, True)]
        callargs = []
        for pr, n in zip(sig["params"], argn):
            kind, tname, isc = pr
            if kind == "scalar":
                callargs.append(n)
            elif kind == "ref":
                callargs.append("*(%s%s *)%s"
                                % ("const " if isc else "", tname, n))
            else:
                callargs.append("(%s%s *)%s"
                                % ("const " if isc else "", tname, n))
        cast = "(const %s *)" % qual if sig["const"] else "(%s *)" % qual
        lines.append("/* ROM 0x%08x -> %s */" % (r["addr"], r["target"]))
        if rec["meth"] == "~":
            lines.append('extern "C" void %s(void *self)' % r["ident"])
            lines.append("{ ((%s *)self)->%s::~%s(); }" % (qual, qual, cls))
        elif rec["meth"] == "ctor":
            lines.append('extern "C" void %s(%s)'
                         % (r["ident"], cparams[0]))
            lines.append("{ new ((void *)self) %s(%s); }"
                         % (qual, ", ".join(callargs)))
        else:
            ret = sig["ret"]
            retkw = "return " if ret != "void" else ""
            lines.append('extern "C" %s %s(%s)'
                         % (ret, r["ident"], cparams[0]))
            lines.append("{ %s(%sself)->%s::%s(%s); }"
                         % (retkw, cast, qual, sig["meth"],
                            ", ".join(callargs)))
        lines.append("")

    text = "\n".join(lines) + "\n"
    pathlib.Path(out).parent.mkdir(parents=True, exist_ok=True)
    pathlib.Path(out).write_text(text)
    return text


def verify_sync(rows, genfile, scratch):
    """Compile the emitted file; its symbol surface must equal the request."""
    genfile = pathlib.Path(genfile).resolve()
    results = closure.compile_batch(genfile.parent, [str(genfile)], scratch)
    syms = results[str(genfile)]
    if syms is None:
        return False, "generated file DID NOT COMPILE (see %s)" % scratch
    und, dfn = closure.read_syms(syms)
    want_def = set(r["target"] if r.get("forward") else "_" + r["ident"]
                   for r in rows)
    if any(r["rec"]["meth"] == "ctor" and not r.get("forward")
           for r in rows):
        # The inline placement operator new the constructor faces need comes
        # back as a COMDAT of its own. Three other objects in this link
        # already emit it (fader_wipes, sdat, editor_channel), COMDATs fold,
        # and it is the standard body, so it is an expected member of the
        # surface rather than a stray.
        want_def.add("??2@YAPAXIPAX@Z")
    want_und = set("_" + r["ident"] if r.get("forward") else r["target"]
                   for r in rows)
    for r in rows:
        if not r.get("d0"):
            continue
        # A D0 face refers to its D1 (flat or decorated), to the one
        # deallocation the ROM body makes, and to the pooled heap word.
        if r.get("flat_callee"):
            want_und.add("_" + r["target"])
        want_und.add(DEALLOCATORS[r["dealloc"]]["undefined"])
        if r["heap"] != "-":
            want_und.add("_" + r["heap"])
    problems = []
    if dfn != want_def:
        problems.append("defined set mismatch: extra=%s missing=%s"
                        % (sorted(dfn - want_def)[:8],
                           sorted(want_def - dfn)[:8]))
    if not und <= want_und:
        problems.append("unexpected undefineds: %s"
                        % sorted(und - want_und)[:8])
    return (not problems), "; ".join(problems) or "surface exact"


SYNC_ROW = re.compile(r"^(\S+)\s+(\S+)\s+(0x[0-9a-fA-F]+)\s+"
                      r"(?:([RF])|D\s+(\S+)\s+(\S+))\s*$")


def read_sync_ledger(path):
    """port/faces_sync.txt -> [(flat, other, address, direction, *extra)].

    The direction says which of the two names the FACE DEFINES:
      R  reverse -- the face defines the flat ROM name and calls the member;
      F  forward -- the face defines the decorated member the caller asked
         for and calls the flat ROM body;
      D  deleting destructor -- the face defines the flat ROM D0 name, runs
         the class's D1 (the second column: the flat D1 when this link
         defines it, else the decorated destructor) and then makes the one
         deallocation the ROM's own D0 body makes. A D row carries two more
         columns, the deallocation and the heap word its pooled literal
         relocates to ("-" for the operator_delete2 shape, which takes none),
         because Deallocate's heap argument is per class and a default would
         be heap corruption.

    Comment and blank lines are skipped. A malformed row is fatal: this file
    is the tool's whole universe in --sync mode, and a row the reader drops
    is a face that silently stops being generated.
    """
    rows = []
    for num, ln in enumerate(pathlib.Path(path).read_text(
            errors="replace").splitlines(), 1):
        t = ln.strip()
        if not t or t.startswith("#"):
            continue
        m = SYNC_ROW.match(t)
        if not m:
            sys.exit("%s:%d: malformed ledger row %r (want "
                     "'<flat> <other> <0xaddress> <R|F>' or "
                     "'<flat D0> <D1> <0xaddress> D <deallocation> <heap>')"
                     % (path, num, t))
        if m.group(4):
            rows.append((m.group(1), m.group(2), int(m.group(3), 16),
                         m.group(4)))
        else:
            rows.append((m.group(1), m.group(2), int(m.group(3), 16),
                         "D", m.group(5), m.group(6)))
    return rows


def write_ledger(rows, path):
    """Write a derived row set out as a faces_sync.txt ledger body.

    Header text is not written here: port/faces_sync.txt carries a hand
    header that explains the contract, and a regeneration must not silently
    drop it. This writes the rows only, for a lane to paste under it.
    """
    lines = []
    cur = None
    for r in sorted(rows, key=lambda x: (_shadow_comps(x)[0], x["flat"],
                                         x["target"])):
        top = _shadow_comps(r)[0]
        if top != cur:
            lines.append("")
            cur = top
        if r.get("d0"):
            lines.append("%-72s %-64s 0x%08x D %s %s"
                         % (r["flat"], r["target"], r["addr"],
                            r["dealloc"], r["heap"]))
        else:
            lines.append("%-72s %-64s 0x%08x %s"
                         % (r["flat"], r["target"], r["addr"],
                            "F" if r.get("forward") else "R"))
    pathlib.Path(path).write_text("\n".join(lines) + "\n")
    return len(rows)


def run_sync(ledger, root, out, defined=None, strict=True):
    """Generate the sync faces named by the checked-in ledger.

    THE TOOL REFUSES TO RUN ON A NAME THAT IS NOT IN THE LEDGER: the ledger
    IS the request in this mode, so a face can only appear in the output by
    appearing in a reviewed, checked-in row first.
    """
    want = read_sync_ledger(ledger)
    rev = [w for w in want if w[3] == "R"]
    fwd = [w for w in want if w[3] == "F"]
    dzero = [w for w in want if w[3] == "D"]
    if defined is None:
        # The ledger's own columns are the universe: each row names both the
        # definition it binds to and the ROM body, and the build has no link
        # map to read yet. A D row's second column is the destructor its face
        # runs, which is in this link by the same argument.
        defined = (set(w[1] for w in rev) | set(w[0] for w in fwd)
                   | set(w[1] for w in dzero)
                   | set("_" + w[1] for w in dzero))
    rows, refusals = derive_rows([w[0] for w in rev], defined, root)
    frows, frefusals = derive_forward_rows([w[1] for w in fwd], defined, root)
    refusals += frefusals
    drows, drefusals = derive_d0_rows(dzero, defined, root)
    refusals += drefusals
    bound = {}
    for r in rows:
        bound[("R", r["flat"])] = r
    for r in frows:
        bound[("F", r["target"])] = r
    for r in drows:
        bound[("D", r["flat"])] = r

    def _key(w):
        return (w[3], w[1] if w[3] == "F" else w[0])

    for w in want:
        direction, flat, dec, addr = w[3], w[0], w[1], w[2]
        k = _key(w)
        r = bound.get(k)
        if r is None:
            continue
        want_other = flat if direction == "F" else dec
        got_other = r["flat"] if direction == "F" else r["target"]
        if got_other != want_other:
            refusals.append((k[1], "LEDGER DISAGREES: the row names %s and "
                             "the derivation binds %s"
                             % (want_other, got_other)))
            bound.pop(k)
            continue
        if r["addr"] != addr:
            refusals.append((k[1], "LEDGER DISAGREES: the row names ROM "
                             "0x%08x and config says 0x%08x"
                             % (addr, r["addr"])))
            bound.pop(k)
    rows = [bound[_key(w)] for w in want if _key(w) in bound]
    rows, cyc = refuse_face_cycles(rows)
    refusals += cyc
    emit_sync(rows, out, "%d faces from %d ledger rows; %d REFUSED"
              % (len(rows), len(want), len(want) - len(rows)))
    for flat, why in refusals:
        print("REFUSED %s -- %s" % (flat, why))
    print("faces %d of %d ledger rows -> %s" % (len(rows), len(want), out))
    if strict and len(rows) != len(want):
        sys.exit("%d ledger rows did not survive the address binding; a "
                 "checked-in row that stops deriving is a face that silently "
                 "left the link" % (len(want) - len(rows)))
    return rows, refusals


def read_list(path):
    """A symbol per line; closure.py output is accepted as-is.

    If the text carries closure.py section headers, ONLY the UNRESOLVED
    section is ingested; everything else in that report is context, not a
    request. The first version of this gate was computed and never applied,
    and fed a real closure report it ingested 44 rows including all the
    duplicate-definition candidates and a header line -- generating a face
    for a symbol the image already defines, a manufactured LNK2005 that
    --verify cannot catch because the request itself was poisoned. A plain
    list (no headers anywhere) ingests every non-comment line.
    """
    lines = pathlib.Path(path).read_text(errors="replace").splitlines()
    has_headers = any(line.startswith("=== ") for line in lines)
    out, in_section = [], False
    for line in lines:
        if line.startswith("==="):
            in_section = line.startswith("=== UNRESOLVED")
            continue
        s = line.strip()
        if not s or s.startswith("#"):
            continue
        if in_section or not has_headers:
            out.append(s)
    return out


def selftest():
    """The classifier and emitter against a curated request set.

    Ten forward faces and the TrackInDeathTable reverse face are drawn from
    hal/actor_class_faces.cpp (TouchesWater, that file's other reverse
    face, is exercised by the full reconstruction proof, not here); the
    ApplyOpacity arity refusal is that file's judgment row; the two ov004
    globals are the MG1 wall's, NOT that file's -- one pointer-to-member
    spelling that must be refused as the WALL and one PAHA spelling that
    must alias. Expected: ten FACE, one RFACE, one ALIAS, one WALL, the
    ARITY MISMATCH refusal, and a compiled symbol surface that equals the
    request exactly. A second case feeds read_list a real mixed closure.py
    report and expects ONLY the UNRESOLVED rows back.

    THE HARDENING ARMS, one per defect real use found (mg_fanout_costs.txt
    s10), each fed the exact symbol that slipped or misfired:

      1. ?data_ov006_02141950@@3PAUEntry@@A -- the dScMgCurling_c state
         table, a PMF table behind a struct wrapper, WITH its C name in
         the universe so the old alias temptation is live. Must be
         REFUSED, never ALIAS. Its by-value sibling @@3UPair@@A must
         refuse too (that one always did; the arm keeps it that way).
      2. verify() run twice more, on a relative --out WITH a directory
         component and on the bare-relative shape, both from a changed
         CWD. Both must PASS; the first used to be a false DID NOT
         COMPILE.
      3. a zero-byte map through closure.map_defined, the exact call
         --map makes: must refuse loudly, not return an empty universe.
      4. Memory::Allocate both ways -- the Itanium spelling against the
         @@Y definition must refuse WITH THE NAMESPACE REASON (the old
         reason was "reverse face with 2 args", a method misread), and
         the @@Y spelling as an unresolved row must refuse as a
         namespace-scope free function rather than dying in undname.
    """
    unresolved = [
        "?UpdatePos@dActor_c@@QAEXPAUCylinderClsn@@@Z",
        "?UpdatePosWithHorzSpeedAndAng@dActor_c@@QAEXXZ",
        "?ReflectAngle@dActor_c@@QAEFHHF@Z",
        "?DistToCPlayer@dActor_c@@QAEHXZ",
        "?UpdateWMClsn@dEnemyBase_c@@QAEXAAUWithMeshClsn@@I@Z",
        "?Init@dCcAc_c@@QAEXPAUActor@@HHII@Z",
        "?SetFile@dBgW_KcMbg@@QAEXPAUKCL_File@@ABUMatrix4x3@@HFAAUCLPS_Block@@@Z",
        "?Init@dBgCh_Actr@@QAEXPAUActor@@HHPAUVector3_16@@H@Z",
        "?IsOnWall@dBgCh_Actr@@QBEHXZ",
        "?JustHitGround@dBgCh_Actr@@QBEHXZ",
        "?ApplyOpacity@ModelBase@@QAEXIH@Z",
        "?data_ov004_020beb98@@3PAP8C@@AEXXZA",
        "?data_ov004_020beb68@@3PAHA",
        "__ZN8dActor_c17TrackInDeathTableEv",
        "?data_ov006_02141950@@3PAUEntry@@A",
        "?data_ov004_020bc904@@3UPair@@A",
        "__ZN6Memory8AllocateEji",
        "?Allocate@Memory@@YAPAXIH@Z",
        "?dataP8pad@@3PAHA",
        "?data_ov004_020bff00@@3PAP6AXXZA",
    ]
    targets = [
        "__ZN8dActor_c9UpdatePosEP5dCc_c",
        "__ZN8dActor_c28UpdatePosWithHorzSpeedAndAngEv",
        "__ZN8dActor_c12ReflectAngleE5Fix12IiES1_s",
        "__ZN8dActor_c13DistToCPlayerEv",
        "__ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj",
        "__ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj",
        "__ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block",
        "__ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_",
        "__ZNK10dBgCh_Actr8IsOnWallEv",
        "__ZNK10dBgCh_Actr13JustHitGroundEv",
        "__ZN9ModelBase12ApplyOpacityEj",
        "_data_ov004_020beb68",
        "?TrackInDeathTable@dActor_c@@QAEXXZ",
        "_data_ov006_02141950",
        "_data_ov004_020bc904",
        "?Allocate@Memory@@YAPAXIH@Z",
        "_dataP8pad",
        "_data_ov004_020bff00",
    ]
    rows = classify(unresolved, build_universe(targets))
    ok = True

    def expect(cond, what):
        nonlocal ok
        if not cond:
            print("FAIL:", what)
            ok = False

    expect(len(rows["FACE"]) == 10, "10 faces, got %d" % len(rows["FACE"]))
    expect(len(rows["RFACE"]) == 1 and
           rows["RFACE"][0][1] == "_ZN8dActor_c17TrackInDeathTableEv",
           "TrackInDeathTable reverse face")
    # dataP8pad is the WALL scan's name-exclusion arm: 'P8' in the NAME
    # portion of a scalar-spelled global must still alias -- the scan
    # covers the type text only.
    expect(set(r[1] for r in rows["ALIAS"]) ==
           {"data_ov004_020beb68", "dataP8pad"},
           "the two PAHA aliases (incl. P8-in-the-name): %s" % rows["ALIAS"])
    expect(len(rows["WALL"]) == 1 and "beb98" in rows["WALL"][0][0],
           "the P8 wall row and only it")
    expect(any("ARITY MISMATCH" in r[1] and "ApplyOpacity" in r[0]
               for r in rows["REFUSED"]), "ApplyOpacity arity refusal")
    expect(not rows["MISSING"] and not rows["UNKNOWN"],
           "no MISSING/UNKNOWN: %s %s" % (rows["MISSING"], rows["UNKNOWN"]))

    # arm 1: the struct-wrapped PMF table. Its C name IS in the universe,
    # so an ALIAS classification is one regex away; the len==1 assert on
    # ALIAS above catches that too, but this arm names the row.
    refused = {r[0]: r[1] for r in rows["REFUSED"]}
    expect("?data_ov006_02141950@@3PAUEntry@@A" in refused and
           "struct" in refused.get("?data_ov006_02141950@@3PAUEntry@@A", ""),
           "PAUEntry struct-wrapper refusal: %s" % rows["REFUSED"])
    expect("?data_ov004_020bc904@@3UPair@@A" in refused,
           "UPair by-value refusal held")
    # The whitelist's own arm: a function-pointer array (PAP6) is neither
    # struct-spelled nor scalar, its C name is in the universe, and its
    # words are DS code addresses -- it must refuse via the whitelist, not
    # fall through to ALIAS the way the old anything-not-a-struct rule did.
    expect("scalar subset" in
           refused.get("?data_ov004_020bff00@@3PAP6AXXZA", ""),
           "PAP6 function-pointer array refused by the whitelist: %s"
           % refused.get("?data_ov004_020bff00@@3PAP6AXXZA"))

    # arm 4: the namespace pair, both directions, and the REASON is the
    # assertion -- the old classifier refused the first row too, with a
    # method-shaped reason that sent the reader to the wrong precedent.
    r_it = refused.get("__ZN6Memory8AllocateEji", "")
    expect("namespace-scope" in r_it and "reverse face" not in r_it,
           "Memory::Allocate Itanium-side reason: %r" % r_it)
    r_ms = refused.get("?Allocate@Memory@@YAPAXIH@Z", "")
    expect("NAMESPACE" in r_ms,
           "Memory::Allocate MSVC-side reason: %r" % r_ms)

    with tempfile.TemporaryDirectory() as td:
        gen = pathlib.Path(td) / "faces_gen.cpp"
        emit(rows, gen)
        good, msg = verify(rows, gen, pathlib.Path(td) / "probe")
        expect(good, "verify: " + msg)
        if good:
            print("verify:", msg)

        # arm 2: the same request through a RELATIVE --out with a
        # directory component, from a changed CWD -- the shape finding 2
        # false-failed -- plus the bare-relative shape that always worked.
        oldcwd = os.getcwd()
        try:
            os.chdir(td)
            os.makedirs("sub", exist_ok=True)
            relout = os.path.join("sub", "faces_rel.cpp")
            emit(rows, relout)
            good, msg = verify(rows, relout, "probe_rel")
            expect(good, "verify on relative --out with a dir component: "
                   + msg)
            emit(rows, "faces_bare.cpp")
            good, msg = verify(rows, "faces_bare.cpp", "probe_bare")
            expect(good, "verify on bare relative --out: " + msg)
        finally:
            os.chdir(oldcwd)

        # arm 3: facegen's --map path is closure.map_defined, this exact
        # call; a zero-byte map (what a failed link leaves) must refuse
        # loudly rather than come back as an empty universe.
        zmap = pathlib.Path(td) / "zero.map"
        zmap.write_text("")
        try:
            closure.map_defined(zmap)
            expect(False, "zero-byte map ACCEPTED by the --map path")
        except SystemExit as e:
            expect("ZERO BYTES" in str(e),
                   "zero-map refusal message: %s" % e)

        # read_list against a real mixed closure.py report: only the
        # UNRESOLVED rows may come back. The first shipped version ingested
        # every section of exactly this shape and manufactured an LNK2005.
        mixed = pathlib.Path(td) / "closure_out.txt"
        mixed.write_text(
            "map defines 32535 symbols\n"
            "\n"
            "=== COMPILE FAILURES (1) ===\n"
            "   src/_ZN14dScMgCurling_c8BehaviorEv.cpp\n"
            "        error C2440: cannot convert\n"
            "\n"
            "=== UNRESOLVED after this slice: 2 "
            "(an estimate; confirm with a link) ===\n"
            "    ?UpdatePos@dActor_c@@QAEXPAUCylinderClsn@@@Z\n"
            "    _data_02075720\n"
            "\n"
            "=== DUP-DEF CANDIDATES, NEED A REAL LINK ===\n"
            "  already in the image from its own TU: 1\n"
            "    _Enable3dEngines  --  Enable3dEngines.c.obj compiles it "
            "already\n"
            "  FOREIGN-object rows, the LNK2005 candidates: 1\n"
            "    _LoadArchive  --  image copy is from scene_boot.cpp.obj\n")
        got = read_list(mixed)
        expect(got == ["?UpdatePos@dActor_c@@QAEXPAUCylinderClsn@@@Z",
                       "_data_02075720"],
               "read_list on mixed closure output: %s" % got)

        # and a plain list, no headers, still ingests every row
        plain = pathlib.Path(td) / "plain.txt"
        plain.write_text("# a comment\n?A@B@@QAEXXZ\n_c_name\n")
        got = read_list(plain)
        expect(got == ["?A@B@@QAEXXZ", "_c_name"],
               "read_list on a plain list: %s" % got)
    print("selftest %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(
        description="generate mechanical faces/aliases; see the docstring")
    ap.add_argument("--unresolved", help="symbol list or closure.py output")
    ap.add_argument("--targets", action="append", default=[],
                    help="available-definition list (repeatable)")
    ap.add_argument("--map", dest="mapfile",
                    help="walk_window.map to use as the universe")
    ap.add_argument("--out", default="faces_gen.cpp")
    ap.add_argument("--verify", action="store_true",
                    help="compile the output and check its symbol surface")
    ap.add_argument("--selftest", action="store_true")
    ap.add_argument("--sync", metavar="LEDGER",
                    help="generate the sync faces named by a checked-in "
                         "ledger (port/faces_sync.txt); every row is "
                         "re-derived against the ROM addresses and a row "
                         "that stops deriving fails")
    ap.add_argument("--derive", action="store_true",
                    help="with --unresolved and a universe: WRITE a candidate "
                         "ledger to --out instead of a source file")
    ap.add_argument("--root", default=None,
                    help="repo root holding config/**/symbols.txt (default: "
                         "the parent of the port/ directory this tool is in)")
    ap.add_argument("--lenient", action="store_true",
                    help="--sync only: report refusals without failing")
    args = ap.parse_args()

    if args.selftest:
        sys.exit(selftest())

    root = args.root or os.path.dirname(os.path.dirname(
        os.path.dirname(os.path.abspath(__file__))))

    if args.sync:
        rows, _ref = run_sync(args.sync, root, args.out,
                              strict=not args.lenient)
        if args.verify:
            with tempfile.TemporaryDirectory() as td:
                good, msg = verify_sync(rows, args.out, td)
                print("verify:", ("PASS " if good else "FAIL ") + msg)
                sys.exit(0 if good else 1)
        sys.exit(0)
    if not args.unresolved:
        sys.exit("pass --unresolved (or --selftest)")

    targets = []
    for t in args.targets:
        targets += read_list(t)
    if args.mapfile:
        targets += sorted(closure.map_defined(args.mapfile))
    if not targets:
        sys.exit("empty universe: pass --targets and/or --map")

    if args.derive:
        flats = [x for x in read_list(args.unresolved)
                 if not x.startswith("?")]
        drows, refusals = derive_rows(flats, set(targets), root)
        write_ledger(drows, args.out)
        for flat, why in refusals:
            print("REFUSED %s -- %s" % (flat, why))
        print("\nledger %s: %d rows derived, %d refused"
              % (args.out, len(drows), len(refusals)))
        sys.exit(0)

    rows = classify(read_list(args.unresolved), build_universe(targets))
    report(rows)
    if rows["FACE"] or rows["RFACE"] or rows["ALIAS"] or rows["ALIAS_FN"]:
        emit(rows, args.out)
        print("\nwrote %s" % args.out)
        if args.verify:
            with tempfile.TemporaryDirectory() as td:
                good, msg = verify(rows, args.out, td)
                print("verify:", ("PASS " if good else "FAIL ") + msg)
                sys.exit(0 if good else 1)


if __name__ == "__main__":
    main()
