#!/usr/bin/env python3
"""Generate the address-switch include and the dispatching host copy's
skeleton for an mwcc pointer-to-member state machine, from the ROM's own
table data.

WHY THIS EXISTS. port/mg_fanout_costs.txt section 4 is the wall: mwcc's
member pointer is eight bytes {code, adjustment} and MSVC's is four, the
code word is a raw DS address, and the ARM Itanium dispatch sequence has
no MSVC equivalent. So every dispatching TU needs a HOST COPY with the
pointer-to-member site replaced by a switch from DS code address to a
real call, and that switch is the single most expensive hand artifact in
the minigame fan-out. Two were built by hand:

    port/unmatched/MgCurling_StateDispatch.cpp   25 cases, dScMgCurling_c
    port/hal/player_states.inc                  197 rows, Player
                                     (its generator did not survive)

Twenty-nine more minigames need one each. This tool derives them.

THE ONE MECHANISM BEHIND BOTH PRECEDENTS, which is the finding that makes
a single tool possible. An overlay's .init constructor assigns .data pair
sources into .bss dispatch storage, one statement per slot:

    data_ov006_02141950.p0 = data_ov006_0213c21c;   the mg shape, an array
    data_ov002_0211013c.lo = data_ov002_02109e0c;   the player shape, a
                                                    State object

Either way the destination is zero-initialised .bss carrying NO
relocations, so nothing is reachable by walking relocs, and the source is
an eight-byte pair whose first word is the DS code address. Read the
constructor for WHICH pair fills WHICH slot and the raw overlay image for
WHAT THE PAIR SAYS, and the state table falls out. Both shapes parse with
one parser because the constructor declares its own structs and the field
ORDER in that declaration is the slot order.

DO NOT SWEEP AN ADDRESS RANGE FOR PAIRS. mg_fanout_costs section 4 names
the counterexample: 0x0213c1e4..0x0213c2bc spans twenty-eight slots of
which three are not pairs at all (one is a SpawnInfo record). The
constructor is the only thing that knows which words are pairs.

WHAT IT READS, all of it produced by something that is not this tool:

  src/__sinit_ovNNN_XXXXXXXX.c   the overlay constructor: struct decls,
                     destination externs, and one assignment per slot.
  extracted/overlays/overlay_00NN.bin   the RAW image. dsd's rebuilt
                     arm9_overlays exports are twice-proven stale; only
                     extracted/overlays is config-aligned, so that is the
                     only thing this tool will open. Based and
                     span-checked off delinks, the vtablerows rule.
  config .../delinks.txt   section spans, and the address->src-file
                     blocks the body join runs on.
  config .../symbols.txt   exact-address symbol names.
  port/ovNNN_syms.txt   the mount list ovdata.py generates host storage
                     from. A destination table absent from it has no
                     storage in the port, and emitting a switch over it
                     would produce an unresolved external at link.
  src/<dispatching TU>   only to read the PMF typedef, for arity.

WHAT IT REFUSES, and every refusal names its remedy. The philosophy is
port/tools/inferred_stub_guard's: a generator that guesses is worse than
no generator, because its output looks checked.

  * a stale or wrong overlay image (the delinks span check)
  * a destination table that is NOT MOUNTED in port/ovNNN_syms.txt
  * a destination that is not in .bss, or a pair source not in .data
  * a NONZERO ADJUSTMENT word. Zero means a plain call; nonzero means the
    virtual-dispatch or multiple-inheritance form, and an address switch
    cannot express it. All 25 curling pairs and all 216 player pairs read
    zero, so this arm has never fired on the real tree -- it is here so
    that the first class where it DOES fire stops rather than emits a
    wrong call.
  * a state address with NO DELINK BLOCK. This is the one that earns the
    tool. 0x020e1854 HAS a symbol in ov006/symbols.txt
    (func_ov006_020e1854) and has no delink block and no src file, so a
    name-shaped check on symbols.txt emits a call to it and the link
    fails. The delinks join is what catches it. Such a slot is emitted as
    a REPORTING case, never as a call, and never given an invented
    symbol -- that guess is exactly what inferred_stub_guard exists to
    refuse. It is listed under REFUSALS in the report.
  * an arity it cannot derive (see below).

WHAT IT DOES NOT DO:

  * IT DOES NOT COVER THE FRAMEWORK HALF OF THE WALL, and a fan-out lane
    reading the ONE MECHANISM claim above as full coverage of
    mg_fanout_costs section 4 will be wrong in the most expensive
    direction. src/_ZN10dMgState_c8SetStateEi.cpp is dScMgBase_c's state setter,
    the TU section 4 says unblocks the framework half of all thirty
    minigames, and it is a DIFFERENT MECHANISM: it builds its twenty-entry
    member-pointer table IN-FUNCTION out of twenty individually named
    .data PMF globals (`extern PMF data_ov004_020bc974;` and nineteen
    more). There is no constructor, no .data pair block and no .bss
    destination, so nothing this tool reads is present and it will not
    produce a table for it. That TU stays hand work, or wants a second
    mode reading named PMF globals rather than a constructor. The same
    holds for the other ov004 dispatchers in section 4's list.
  * it does not adjudicate. A hand file and this tool disagreeing is a
    CONTRADICTION TO INVESTIGATE; --reconstruct prints both sides and
    exits nonzero, and a human rules with the ROM open.
  * it does not write src/ or port/. Output goes to stdout or a path
    that RESOLVES inside this tree's build/, and --out refuses anything
    else. Lanes commit what they verify.
  * --reconstruct's ROM-name test is only as wide as the symbol tables
    loaded for the run, which are arm9 plus the ONE overlay the
    constructor lives in. A hand row calling a ROM symbol that belongs to
    a THIRD overlay would not be found among those names and would be
    binned as a host-side ruling rather than examined. No row in either
    precedent does that, so the narrowness is latent rather than active,
    but a class whose states span overlays needs the loaded set widened
    before its reconstruction means anything.
  * it does not derive the CALL SHAPE rulings a hand file may carry: a
    method that defines at C linkage in src, a forwarding bridge standing
    in for a state whose source needs a shadow class, a (void) body in an
    argument slot. It emits the default shape from the symbol and the
    arity; a lane edits from there. --reconstruct compares TARGET
    SYMBOLS, not text, for exactly this reason.

ARITY. Each dispatching TU declares its own member-pointer typedef and
the table's extern names it:

    typedef void (C::*PMF)(int);   extern PMF data_ov006_021418c0[];
    typedef void (C::*PMF0)();     extern PMF0 data_ov006_021418b0[];
    struct Entry { PMF pmf[1]; };  extern Entry data_ov006_02141950[];

so arity is DERIVED, not asked for, by finding the TU that declares the
table and reading its typedef through any struct wrapper. Derived
arities reproduce the hand header's column for all seven curling tables.
When no TU declares a table -- the player's State objects are dispatched
by hal_call_state_fn rather than by a typed extern -- derivation refuses
and names --arity as the remedy.

THE TWO TEST MODES COVER DIFFERENT THINGS AND BOTH ARE WIRED into
build-port.cmd, the vtablerows precedent for the same reason. --selftest
exercises the parsing, the pair read, the slot ordering, the arity
derivation and every refusal on fabricated fixtures, and cannot see the
REAL-TREE WIRING: the extracted/config paths, the mount lists, the
precedent file locations. --reconstruct is the net for that half. A
broken path constant passes --selftest and fails the reconstruction.
They join a pre-configure block that runs SIX --selftest invocations
across EIGHT tools in TEN steps; the two here are the fifth and sixth
--selftest and the eighth and ninth step.

WHAT IS AND IS NOT REPRODUCIBLE FROM THIS ARTIFACT, stated because a
green selftest invites more confidence than it earns. Reproducible: the
sixteen refusal arms and the assertions around them, on every run, plus
the two reconstructions against the real tree. NOT reproducible: the
mutation battery this file was developed against. No mutation harness is
committed, so "29/29 mutants caught" is a claim about a run that a reader
cannot repeat here. Review's own independent mutations of this tool are
the measurement that stands on its own; treat the battery number as
development evidence, not as a property of the checked-in code.

    python port/tools/stategen.py --sinit src/__sinit_ov006_021304ac.c
    python port/tools/stategen.py --sinit ... --emit-switch
    python port/tools/stategen.py --sinit ... --emit-host
    python port/tools/stategen.py --sinit src/__sinit_ov002_021019d0.c \
                                  --arity 0 --class Player --emit-switch
    python port/tools/stategen.py --reconstruct
    python port/tools/stategen.py --selftest
"""

import argparse
import bisect
import os
import pathlib
import re
import struct
import sys
import tempfile

SECTION = re.compile(
    r"^\s+\.(\w+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)\s+"
    r"kind:(\w+)")
SRC_BLOCK = re.compile(r"^(src/\S+):\s*$")
SRC_RANGE = re.compile(
    r"\.(?:text|init)\s+start:0x([0-9a-fA-F]+) end:0x([0-9a-fA-F]+)")
SYMBOL = re.compile(r"^(\S+)\s+kind:\S+\s+addr:0x([0-9a-fA-F]+)")

DATASYM = r"data_(?:ov(\d{3})_)?([0-9a-f]{8})"
# THE CONSTRUCTOR WRITES A SLOT IN TWO SHAPES and a parser that knows only
# one silently emits a short switch. Both are here, and anything else that
# writes a destination is a refusal rather than a skip.
#
# 1. the whole-pair assignment:      DST.FIELD = SRC;
#    RHS must be a bare data symbol, which is what drops the scalar writes
#    (`.tail.a = tb ? ta : ta;`) without needing a second rule.
ASSIGN = re.compile(
    r"^\s*(" + DATASYM + r")\.([A-Za-z_]\w*)\s*=\s*(" + DATASYM + r")\s*;\s*$")
# 2. the WORD-INDEX pair copy, two consecutive statements:
#       ((int *)&DST)[2k]   = ((int *)&SRC)[0];
#       ((int *)&DST)[2k+1] = ((int *)&SRC)[1];
#    This is the decomp's rendering of a pair copy the compiler emitted as
#    two word stores. It is not cosmetic: the player constructor fills the
#    NoControl kind table and the LevelEnter step table this way ONLY, and
#    those two tables are 22 of player_states.inc's rows. It also reaches
#    PAST the declared struct -- data_ov002_0211073c is declared P6 (three
#    pairs) and written at word index 14 -- so the struct declaration
#    bounds the named-field shape and nothing else.
WORDIDX = re.compile(
    r"^\s*\(\s*\(\s*int\s*\*\s*\)\s*&\s*(" + DATASYM + r")\s*\)\s*"
    r"\[\s*(\d+)\s*\]\s*=\s*\(\s*\(\s*int\s*\*\s*\)\s*&\s*(" + DATASYM +
    r")\s*\)\s*\[\s*(\d+)\s*\]\s*;\s*$")
# struct decls in the constructor. Two shapes only: the PAIR (two scalar
# fields) and a DESTINATION (fields of the pair type).
STRUCT = re.compile(r"^\s*struct\s+(\w+)\s*\{(.*?)\}\s*;", re.S | re.M)
# the same declaration spelled as an anonymous typedef. Seven of ov006's
# thirty-one constructors use this form and nothing else, so a parser that
# reads only `struct NAME {` refuses seven fan-out lanes for a spelling.
STRUCT_TD = re.compile(
    r"^\s*typedef\s+struct\s*(?:\w+\s*)?\{(.*?)\}\s*(\w+)\s*;", re.S | re.M)
DEST_DECL = re.compile(
    r"^\s*extern\s+struct\s+(\w+)\s+(" + DATASYM + r")\s*;\s*$", re.M)
# the destination declared as an ARRAY of the pair type, with or without
# an extent, and written by index rather than by field name. This is the
# third slot-assignment shape and the commonest one in ov006.
DEST_ARR = re.compile(
    r"^\s*extern\s+(?:struct\s+)?(\w+)\s+(" + DATASYM +
    r")\s*\[\s*(\d*)\s*\]\s*;\s*$", re.M)
# 3. the array-index slot assignment:  DST[i] = SRC;
ASSIGN_IDX = re.compile(
    r"^\s*(" + DATASYM + r")\s*\[\s*(\d+)\s*\]\s*=\s*(" + DATASYM +
    r")\s*;\s*$")
# the destination declared through an ANONYMOUS TYPEDEF whose one field is an
# ARRAY of the pair type, and written field-then-index. Run link60 lane MGB.
#
#     typedef struct { Pair p[4]; } Dst4_21423c0;
#     extern Dst4_21423c0 data_ov006_021423c0;
#     ...
#     data_ov006_021423c0.p[0] = data_ov006_0213d248;
#
# This is a FOURTH slot-assignment spelling and it is one of the five the STG
# lane's report listed as "refuse on a spelling not yet taught". Nothing above
# reaches it: DEST_DECL wants a literal `struct` keyword the typedef removed,
# ASSIGN wants `.field =` with no subscript, and ASSIGN_IDX wants the subscript
# on the SYMBOL rather than on a field. The old failure was not even a refusal
# -- every line fell through the unknown-write branch, whose dest_arr/dest_type
# tests both missed, so the constructor reported "no pair assignments" and read
# as though it built no state table at all.
DEST_TD_DECL = re.compile(
    r"^\s*extern\s+(\w+)\s+(" + DATASYM + r")\s*;\s*$", re.M)
ASSIGN_FIELD_IDX = re.compile(
    r"^\s*(" + DATASYM + r")\.([A-Za-z_]\w*)\s*\[\s*(\d+)\s*\]\s*=\s*(" +
    DATASYM + r")\s*;\s*$")
# a field spelled `name[N]` inside such a typedef. The extent is required: a
# slot index has to be BOUNDED by something the constructor itself declares,
# or an out-of-range write reads as a new slot instead of as a parse error.
ARR_FIELD = re.compile(r"^(\w+)\[(\d+)\]$")

PMF_TYPEDEF = re.compile(
    r"typedef\s+\w[\w\s*]*\(\s*\w+\s*::\s*\*\s*(\w+)\s*\)\s*\(([^)]*)\)\s*;")
PMF_STRUCT = re.compile(r"struct\s+(\w+)\s*\{\s*(\w+)\s+\w+\s*(?:\[\d*\])?\s*;\s*\}")

OUT_ALLOWED = "build"


def die(msg):
    sys.exit("stategen: " + msg)


class Overlay:
    """A raw overlay image, based and span-checked from its delinks file.

    Same rule as port/tools/vtablerows.py, and for the same reason: the
    base is the FIRST in-file section's start and the image must be
    exactly last-in-file-section-end minus base bytes long. Anything else
    is a stale or wrong image and the answer is a refusal, not a read.
    """

    def __init__(self, name, image_path, delinks_path):
        self.name = name
        sections = []
        for line in pathlib.Path(delinks_path).read_text(
                encoding="utf-8-sig", errors="replace").splitlines():
            if SRC_BLOCK.match(line):
                break
            m = SECTION.match(line)
            if m:
                sections.append((m.group(1), int(m.group(2), 16),
                                 int(m.group(3), 16), m.group(4)))
        if not sections:
            die("no section lines in %s" % delinks_path)
        infile = [s for s in sections if s[3] != "bss"]
        self.base = infile[0][1]
        want = infile[-1][2] - self.base
        self.spans = {s[0]: (s[1], s[2]) for s in sections}
        self.image = pathlib.Path(image_path).read_bytes()
        if len(self.image) != want:
            die("%s is %d bytes but %s says %d (0x%08x..0x%08x) -- a stale "
                "or wrong image, refusing to read it. Re-copy extracted/ "
                "from a live worktree (the decomp-worktree setup step)"
                % (image_path, len(self.image), delinks_path, want,
                   self.base, infile[-1][2]))

    def word(self, addr):
        off = addr - self.base
        if not 0 <= off <= len(self.image) - 4:
            die("address 0x%08x is outside %s (0x%08x..0x%08x)"
                % (addr, self.name, self.base, self.base + len(self.image)))
        return struct.unpack_from("<I", self.image, off)[0]

    def in_span(self, kind, addr, size=1):
        s = self.spans.get(kind)
        return s is not None and s[0] <= addr and addr + size <= s[1]


class Sources:
    """The delinks-keyed address->src join.

    A state address with no interval here has no body in the decomp,
    whatever symbols.txt says about it. That distinction is the whole
    point: func_ov006_020e1854 has a symbol and no block.
    """

    def __init__(self, root, delinks_paths):
        self.root = pathlib.Path(root)
        ivs = []
        for dp in delinks_paths:
            p = pathlib.Path(dp)
            if not p.is_file():
                continue
            cur = None
            for line in p.read_text(encoding="utf-8-sig",
                                    errors="replace").splitlines():
                m = SRC_BLOCK.match(line)
                if m:
                    cur = m.group(1)
                    continue
                m = SRC_RANGE.search(line)
                if m and cur:
                    ivs.append((int(m.group(1), 16), int(m.group(2), 16), cur))
        self.ivs = sorted(ivs)
        self.starts = [a for a, _, _ in self.ivs]

    def owner(self, addr):
        i = bisect.bisect_right(self.starts, addr) - 1
        if i >= 0 and self.ivs[i][0] <= addr < self.ivs[i][1]:
            return self.ivs[i][2]
        return None


def load_symbols(paths):
    out = {}
    for sp in paths:
        p = pathlib.Path(sp)
        if not p.is_file():
            continue
        for line in p.read_text(encoding="utf-8-sig",
                                errors="replace").splitlines():
            m = SYMBOL.match(line)
            if m:
                out.setdefault(int(m.group(2), 16), m.group(1))
    return out


# ---------------------------------------------------------------------------
# the constructor parse


def parse_structs(text):
    """{name: [field, ...]} for destination structs, plus the pair name.

    The PAIR struct is the one whose fields are scalars; a DESTINATION is
    any struct whose fields are all of the pair type. Field ORDER is the
    slot order and the slot's DS offset is 8 bytes per field, which is
    why this is read rather than guessed from names (p0..p10 does not
    sort lexicographically, and the player's lo/hi/tail never would).
    """
    pair = None
    dests = {}
    raw = {}
    decls = [(m.group(1), m.group(2)) for m in STRUCT.finditer(text)]
    decls += [(m.group(2), m.group(1)) for m in STRUCT_TD.finditer(text)]
    for name, body in decls:
        fields = []
        for decl in body.split(";"):
            decl = decl.strip()
            if not decl:
                continue
            dm = re.match(r"(?:struct\s+)?(\w+)\s+(.+)$", decl)
            if not dm:
                continue
            ftype = dm.group(1)
            for nm in dm.group(2).split(","):
                nm = nm.strip().lstrip("*").strip()
                if nm:
                    fields.append((ftype, nm))
        raw[name] = fields
    # A pair field is a SCALAR. `struct DstA { int x[3]; int y[3]; };` has
    # two int-typed fields and is not a {code, adjustment} pair, and
    # counting it as one is how __sinit_ov006_02131fa4 picked the wrong
    # struct and then reported the misdirecting "no pair assignments".
    cands = [name for name, fields in raw.items()
             if len(fields) == 2
             and all(f[0] in ("int", "unsigned", "long") and "[" not in f[1]
                     for f in fields)]
    if len(cands) > 1:
        die("this constructor declares %d candidate pair structs (%s) and "
            "the {code, adjustment} pair must be exactly one of them. "
            "Refusing rather than picking: taking the last declaration is "
            "how a coordinate pair wins over the real one and the slot "
            "offsets come out wrong. Rule which is the pair by hand"
            % (len(cands), ", ".join(sorted(cands))))
    if cands:
        pair = cands[0]
    if pair is None:
        # Distinguish the two reasons, because they mean opposite things
        # to a fan-out lane: one constructor declares no struct at all
        # (eleven of ov006's thirty-one), and another declares structs but
        # none this parser can read as the pair.
        if not raw:
            die("this constructor declares no structs, so there is no pair "
                "type here to key slot offsets off and nothing to "
                "generate. WHAT THIS PROVES IS NARROW: that no struct is "
                "declared, not that the constructor builds no state table. "
                "A table copied through some other spelling would look the "
                "same from here. The review scanned all fifteen "
                "constructors in this bucket and found none hiding a "
                "table, which is a measurement of today's tree and not a "
                "property of the test. Check you passed the right __sinit")
        die("no two-scalar pair struct among the %d struct(s) declared "
            "here (%s) -- this parser keys slot offsets off the {code, "
            "adjustment} pair type and will not guess which of these is "
            "it. Teach it the spelling rather than picking one"
            % (len(raw), ", ".join(sorted(raw))))
    for name, fields in raw.items():
        if name != pair and fields and all(f[0] == pair for f in fields):
            dests[name] = [f[1] for f in fields]
    return pair, dests


def parse_sinit(path):
    """(overlay_num, {dest_addr: [(slot_index, slot_name, src_addr)]})."""
    text = pathlib.Path(path).read_text(encoding="utf-8-sig",
                                        errors="replace")
    pair, dests = parse_structs(text)

    dest_type = {}
    for m in DEST_DECL.finditer(text):
        sname, sym, ovn, addr = m.group(1), m.group(2), m.group(3), m.group(4)
        if sname in dests:
            dest_type[sym] = (sname, ovn, int(addr, 16))
    # arrays OF THE PAIR TYPE only. The discriminator matters: these
    # constructors also copy plain `int[]` permutation tables between two
    # data symbols with the same statement shape, and reading one of those
    # as a state table would invent slots out of index values.
    dest_arr = {}
    for m in DEST_ARR.finditer(text):
        ename, sym, ovn, addr = m.group(1), m.group(2), m.group(3), m.group(4)
        if ename == pair:
            dest_arr[sym] = (ename, ovn, int(addr, 16))

    # the typedef'd single-array-field destination. EXACTLY ONE field, and it
    # must be `pair name[N]`. Two array fields would need a cumulative slot
    # offset the tool would be inventing, so that refuses by name below rather
    # than getting a guessed base.
    dest_tdarr = {}
    tdarr_multi = {}
    for sname, fields in dests.items():
        arrs = [ARR_FIELD.match(f) for f in fields]
        if not all(arrs):
            continue
        if len(arrs) != 1:
            tdarr_multi[sname] = fields
            continue
        dest_tdarr[sname] = (arrs[0].group(1), int(arrs[0].group(2)))
    tdarr = {}
    for m in DEST_TD_DECL.finditer(text):
        tname, sym, ovn, addr = m.group(1), m.group(2), m.group(3), m.group(4)
        if tname in tdarr_multi:
            die("%s declares %s as %s, whose fields are %s -- more than one "
                "array of the pair type in one destination struct. The slot "
                "index of the second array depends on the extent of the "
                "first, and this tool will not compute a base it was not "
                "told. Split the declaration or rule the slots by hand"
                % (path, sym, tname, ", ".join(tdarr_multi[tname])))
        if tname in dest_tdarr:
            fname, extent = dest_tdarr[tname]
            tdarr[sym] = (tname, fname, extent, int(addr, 16))

    fn = re.search(r"^void\s+(__sinit_\w+)\s*\(\s*void\s*\)\s*\{", text, re.M)
    if not fn:
        die("%s has no `void __sinit_...(void) {` body -- not an overlay "
            "constructor" % path)
    body = text[fn.end():]
    # bound the body at its own closing brace. Without this the "rest of
    # the file" is read as constructor statements, which is harmless today
    # only because these files end at the constructor.
    end = re.search(r"^\}", body, re.M)
    if not end:
        die("%s has no closing brace for its constructor body" % path)
    body = body[:end.start()]

    ovnums = set()
    out = {}
    n_assign = 0
    seen = {}          # (dst_addr, slot) -> (src_addr, line)
    pending = None     # the even half of a word-index pair copy

    def record(dsym, dov, dst_addr, slot, slotname, ssym, sov, saddr, line,
               sname):
        nonlocal n_assign
        if dov:
            ovnums.add(int(dov))
        if sov:
            ovnums.add(int(sov))
        key = (dst_addr, slot)
        if key in seen and seen[key][0] != saddr:
            die("%s writes slot %d of %s twice, from %s and from %s. Two "
                "different sources for one slot is not something this tool "
                "will resolve by taking the last one; adjudicate against "
                "the disassembly of the constructor"
                % (path, slot, dsym, seen[key][1], ssym))
        if key in seen:
            return
        seen[key] = (saddr, ssym)
        n_assign += 1
        out.setdefault(dst_addr, {"sym": dsym, "struct": sname, "slots": []})
        out[dst_addr]["slots"].append((slot, slotname, saddr, ssym))

    for line in body.splitlines():
        m = ASSIGN.match(line)
        if m:
            pending = None
            dsym, dov, daddr, field, ssym, sov, saddr = (
                m.group(1), m.group(2), m.group(3), m.group(4),
                m.group(5), m.group(6), m.group(7))
            if dsym not in dest_type:
                continue
            sname, _, dst_addr = dest_type[dsym]
            fields = dests[sname]
            if field not in fields:
                die("%s assigns .%s of %s but struct %s has no such field "
                    "(%s) -- the constructor's own declaration is the slot "
                    "order, so this is a parse the tool will not paper over"
                    % (path, field, dsym, sname, ", ".join(fields)))
            record(dsym, dov, dst_addr, fields.index(field), field, ssym,
                   sov, int(saddr, 16), line, sname)
            continue

        m = ASSIGN_IDX.match(line)
        if m:
            pending = None
            dsym, dov, daddr, idx, ssym, sov, saddr = (
                m.group(1), m.group(2), m.group(3), int(m.group(4)),
                m.group(5), m.group(6), m.group(7))
            if dsym not in dest_arr:
                continue
            ename, _, dst_addr = dest_arr[dsym]
            record(dsym, dov, dst_addr, idx, "[%d]" % idx, ssym, sov,
                   int(saddr, 16), line, ename)
            continue

        m = ASSIGN_FIELD_IDX.match(line)
        if m:
            pending = None
            dsym, dov, daddr, field, idx, ssym, sov, saddr = (
                m.group(1), m.group(2), m.group(3), m.group(4),
                int(m.group(5)), m.group(6), m.group(7), m.group(8))
            if dsym not in tdarr:
                continue
            tname, fname, extent, dst_addr = tdarr[dsym]
            if field != fname:
                die("%s assigns .%s[%d] of %s but %s's only field is %s "
                    "-- the constructor's own declaration is the slot "
                    "order and this is a parse the tool will not paper "
                    "over" % (path, field, idx, dsym, tname, fname))
            if not 0 <= idx < extent:
                die("%s writes .%s[%d] of %s, whose declared extent is %d. "
                    "An out-of-range index is either a wrong declaration or "
                    "a write past the table, and reading it as slot %d "
                    "invents a state" % (path, field, idx, dsym, extent, idx))
            record(dsym, dov, dst_addr, idx, "%s[%d]" % (field, idx), ssym,
                   sov, int(saddr, 16), line, tname)
            continue

        m = WORDIDX.match(line)
        if m:
            dsym, dov, daddr, di, ssym, sov, saddr, si = (
                m.group(1), m.group(2), m.group(3), int(m.group(4)),
                m.group(5), m.group(6), m.group(7), int(m.group(8)))
            if di % 2 == 0 and si == 0:
                pending = (dsym, dov, daddr, di, ssym, sov, saddr, line)
                continue
            if pending is None or si != 1 or di != pending[3] + 1 \
                    or dsym != pending[0] or ssym != pending[4]:
                die("%s copies a word at\n    %s\nthat is not the second "
                    "half of a {code, adjustment} pair copy. This parser "
                    "accepts the word-index shape ONLY as an even/odd pair "
                    "from indices [0] and [1] of one source; a lone or "
                    "crossed word store means the constructor is doing "
                    "something this tool must not read as a state table"
                    % (path, line.strip()))
            sname = dest_type[dsym][0] if dsym in dest_type else "(word-index)"
            dst_addr = int(pending[2], 16)
            slot = pending[3] // 2
            record(dsym, pending[1], dst_addr, slot, "[%d]" % slot,
                   pending[4], pending[5], int(pending[6], 16), line, sname)
            pending = None
            continue

        # anything else that WRITES a destination is a shape this parser
        # does not know, and a silent skip there is a short switch. The
        # scalar field write (`.tail.a = tb ? ta : ta;`) is the one known
        # non-slot write and is allowed through by name.
        if "=" in line:
            lhs = line.split("=", 1)[0]
            for dm in re.finditer(DATASYM, lhs):
                sym = dm.group(0)
                if sym in dest_arr:
                    die("%s writes the pair array %s in a shape this "
                        "parser does not know:\n    %s\nRefusing rather "
                        "than skipping it: a dropped slot is a switch that "
                        "is short by one state and looks complete"
                        % (path, sym, line.strip()))
                if sym in tdarr:
                    die("%s writes the typedef'd pair table %s in a shape "
                        "this parser does not know:\n    %s\nRefusing "
                        "rather than skipping it, for the reason above: "
                        "this destination IS a state table, so a line the "
                        "parser cannot read is a slot it would drop"
                        % (path, sym, line.strip()))
                if sym not in dest_type:
                    continue
                sname = dest_type[sym][0]
                if re.match(r"^\s*" + re.escape(sym) + r"\.(" +
                            "|".join(re.escape(f) for f in dests[sname]) +
                            r")\.\w+\s*$", lhs):
                    break
                die("%s writes destination %s in a shape this parser does "
                    "not know:\n    %s\nRefusing rather than skipping it: "
                    "a dropped slot is a switch that is short by one state "
                    "and looks complete. Teach the parser this shape or "
                    "rule the line out by hand" % (path, sym, line.strip()))
            pending = None
            continue
        pending = None

    if pending is not None:
        die("%s ends with\n    %s\nwhich is the first half of a word-index "
            "pair copy and has no second half. Refusing rather than "
            "dropping the slot" % (path, pending[7].strip()))
    if not n_assign:
        die("%s has no pair assignments -- either it is not a "
            "pair-copying constructor or its destinations are not "
            "declared `extern struct T data_...;`" % path)
    if len(ovnums) != 1:
        die("the constructor spans overlays %s; this tool reads one "
            "overlay image at a time, so split the run or pass an "
            "explicit --overlay" % sorted(ovnums))
    for d in out.values():
        d["slots"].sort(key=lambda s: s[0])
    return ovnums.pop(), out


# ---------------------------------------------------------------------------
# arity


def parse_arity(root, tables):
    """{dest_addr: arity} derived from the dispatching TUs' PMF typedefs.

    Returns (arities, unresolved) -- unresolved is [(addr, sym, why)] and
    the caller decides whether to refuse or take --arity.
    """
    root = pathlib.Path(root)
    srcdir = root / "src"
    want = {d["sym"]: a for a, d in tables.items()}
    found = {}
    unresolved = []
    if srcdir.is_dir():
        for p in sorted(srcdir.iterdir()):
            if p.suffix not in (".c", ".cpp"):
                continue
            try:
                text = p.read_text(encoding="utf-8-sig", errors="replace")
            except OSError:
                continue
            if not any(s in text for s in want):
                continue
            typedefs = {m.group(1): m.group(2)
                        for m in PMF_TYPEDEF.finditer(text)}
            if not typedefs:
                continue
            wrappers = {m.group(1): m.group(2)
                        for m in PMF_STRUCT.finditer(text)}
            for sym, addr in want.items():
                dm = re.search(
                    r"extern\s+(?:\"C\"\s+)?(?:struct\s+)?(\w+)\s+" +
                    re.escape(sym) + r"\s*\[", text)
                if not dm:
                    continue
                tname = dm.group(1)
                if tname in wrappers:
                    tname = wrappers[tname]
                if tname not in typedefs:
                    unresolved.append(
                        (addr, sym, "declared in %s as `%s` which is "
                         "neither a member-pointer typedef nor a struct "
                         "wrapping one" % (p.name, dm.group(1))))
                    continue
                params = typedefs[tname].strip()
                arity = 0 if params in ("", "void") else \
                    params.count(",") + 1
                prev = found.get(addr)
                if prev is not None and prev[0] != arity:
                    die("%s is dispatched at arity %d in %s and arity %d "
                        "in %s -- two TUs disagree about the ROM's own "
                        "table. Adjudicate against the disassembly before "
                        "emitting anything"
                        % (sym, prev[0], prev[1], arity, p.name))
                found[addr] = (arity, p.name)
    for sym, addr in want.items():
        if addr not in found and not any(u[0] == addr for u in unresolved):
            unresolved.append((addr, sym, "no src TU declares it as a "
                                          "typed member-pointer array"))
    return {a: v[0] for a, v in found.items()}, unresolved


# ---------------------------------------------------------------------------
# demangling, only the shape these symbols use


def demangle_method(sym):
    """_ZN6Player12St_Wait_MainEv -> ('Player', 'St_Wait_Main'), else None."""
    if not sym.startswith("_ZN"):
        return None
    i, parts = 3, []
    while i < len(sym) and sym[i].isdigit():
        j = i
        while j < len(sym) and sym[j].isdigit():
            j += 1
        n = int(sym[i:j])
        if j + n > len(sym):
            return None
        parts.append(sym[j:j + n])
        i = j + n
    if len(parts) != 2 or i >= len(sym) or sym[i] != "E":
        return None
    return parts[0], parts[1]


def mangle_method(cls, meth):
    return "_ZN%d%s%d%sEv" % (len(cls), cls, len(meth), meth)


# ---------------------------------------------------------------------------
# the model


class Row(object):
    def __init__(self, addr, sym, owner, table, slot, slotname, src_pair):
        self.addr = addr
        self.sym = sym
        self.owner = owner
        self.table = table
        self.slot = slot
        self.slotname = slotname
        self.src_pair = src_pair

    @property
    def hole(self):
        return self.owner is None


def src_file_for(root, symbol):
    """`src/<symbol>.c` or `.cpp` if the tree has one, else None.

    A NAME LOOKUP AND NOTHING MORE. It exists so a delinks-join miss is not
    reported as "undecompiled": the two questions are independent, and
    conflating them is what made this tool's hole message wrong on
    _ZN14dScMgMemory2_c14RoundShowCardsEv. Nothing is inferred about the body from its
    existence -- the banner is what says matched vs NONMATCHING, and the
    message tells the reader to go read it.
    """
    for ext in (".c", ".cpp"):
        rel = "src/%s%s" % (symbol, ext)
        if (pathlib.Path(root) / rel).is_file():
            return rel
    return None


def build(root, sinit, overlay_num=None, arity_override=None):
    root = pathlib.Path(root)
    ovn, tables = parse_sinit(root / sinit if not os.path.isabs(str(sinit))
                              else sinit)
    if overlay_num is not None:
        ovn = overlay_num
    ovdir = "config/arm9/overlays/ov%03d" % ovn
    img = root / ("extracted/overlays/overlay_%04d.bin" % ovn)
    dl = root / ovdir / "delinks.txt"
    for need in (img, dl):
        if not need.is_file():
            die("%s is missing -- this tree has no extracted ROM images "
                "for ov%03d; copy extracted/ from a live worktree (the "
                "decomp-worktree setup step) before generating anything"
                % (need, ovn))
    ov = Overlay("ov%03d" % ovn, img, dl)
    sources = Sources(root, [root / "config/arm9/delinks.txt", dl])
    syms = load_symbols([root / "config/arm9/symbols.txt",
                         root / ovdir / "symbols.txt"])

    mountlist = root / ("port/ov%03d_syms.txt" % ovn)
    mounted = set()
    if mountlist.is_file():
        # A row may pin its size (name:0x18) or its address (name@0xADDR):
        # ovdata.py's --from-list reads both as the NAME with a pin, so the
        # name is what is mounted. data_ov002_021100f4:0x18 is a Player
        # State pinned to its real 0x18 because a false label had cut it.
        mounted = {w.split(":", 1)[0].split("@", 1)[0]
                   for w in mountlist.read_text(errors="replace").split()}

    refusals = []
    rows = []
    for daddr in sorted(tables):
        d = tables[daddr]
        if not ov.in_span("bss", daddr):
            die("destination %s (0x%08x) is not in ov%03d .bss -- a "
                "dispatch table the constructor fills is zero-initialised "
                "storage; this address is not, so the parse is wrong"
                % (d["sym"], daddr, ovn))
        if mountlist.is_file() and d["sym"] not in mounted:
            die("destination %s is NOT MOUNTED: it is absent from %s, the "
                "--from-list ovdata.py generates host storage from. A "
                "switch over an unmounted table links to nothing. Remedy: "
                "add %s to that list and rebuild, then re-run"
                % (d["sym"], mountlist, d["sym"]))
        for slot, slotname, saddr, ssym in d["slots"]:
            if not ov.in_span("data", saddr, 8):
                die("pair source %s (0x%08x) is not inside ov%03d .data -- "
                    "refusing to read eight bytes from outside the section "
                    "the constructor copies from" % (ssym, saddr, ovn))
            code, adj = ov.word(saddr), ov.word(saddr + 4)
            if code == 0:
                die("pair %s (slot %s of %s) has a ZERO code word. That is "
                    "a null member pointer, an unfilled slot or a source "
                    "address read from the wrong place, and none of the "
                    "three is a state to dispatch. Without this check the "
                    "slot becomes `case 0x00000000` routed to the no-body "
                    "reporter, which is safe but tells a lane nothing "
                    "about which of the three it is. No pair in either "
                    "precedent reads zero, so this arm is defensive. "
                    "Remedy: confirm the source address against the "
                    "constructor's disassembly before generating"
                    % (ssym, slotname, d["sym"]))
            if adj != 0:
                die("pair %s (slot %s of %s) has adjustment 0x%08x, not "
                    "zero. A nonzero adjustment is the virtual or "
                    "multiple-inheritance member pointer: `this` is moved "
                    "and the LSB selects a vtable lookup, and an address "
                    "switch cannot express either. Remedy: this class "
                    "needs the ov085/ov100 port_*_states_seat shape, not "
                    "this one" % (ssym, slotname, d["sym"], adj))
            rows.append(Row(code, syms.get(code), sources.owner(code),
                            d["sym"], slot, slotname, ssym))

    for r in rows:
        if r.hole:
            named = (" (symbols.txt DOES name it %s, which is why a "
                     "name-shaped check would have emitted a call)" % r.sym
                     if r.sym else "")
            # WHAT A HOLE ACTUALLY PROVES, corrected by run link60 lane MGB
            # after review. This test is the DELINKS JOIN and nothing else,
            # and the message used to report its result as "HAS NO DECOMPILED
            # BODY ... and no src file". For the first three holes the tool
            # ever hit that happened to be true, so nothing contradicted it.
            # It is FALSE for _ZN14dScMgMemory2_c14RoundShowCardsEv: no delink block, and a
            # decompiled src/actors/dScMgMemory2_c.cpp bannered NONMATCHING.
            # NONMATCHING TUs are routinely sliced and built, so calling that
            # address unreachable over-costs the class that owns it -- which
            # is exactly the error this lane then wrote into
            # mg_fanout_costs.txt. Say which of the two things is true.
            src = src_file_for(root, r.sym) if r.sym else None
            if src:
                refusals.append(
                    "0x%08x (slot %d/%s of %s) IS NOT IN THE DELINKS JOIN: "
                    "no block in %s/delinks.txt covers it, so this tool "
                    "cannot emit a call for it. IT IS NOT UNDECOMPILED "
                    "THOUGH -- %s exists%s. Read that file's banner before "
                    "costing this state: an unbannered TU is matched and a "
                    "NONMATCHING one is still decompiled, and BOTH are "
                    "routinely sliced and built, so this is a boundary of "
                    "THIS TOOL rather than a floor for the port. Remedy: "
                    "slice the TU and hand-write this case, or delink the "
                    "block and re-run"
                    % (r.addr, r.slot, r.slotname, r.table, ovdir, src,
                       named))
            else:
                refusals.append(
                    "0x%08x (slot %d/%s of %s) HAS NO DECOMPILED BODY: no "
                    "delink block in %s/delinks.txt and no src file%s. "
                    "Emitted as a reporting case, never as a call, and "
                    "given no invented symbol. Remedy: decompile it, or "
                    "leave the report in place and the state stays "
                    "unreachable by design"
                    % (r.addr, r.slot, r.slotname, r.table, ovdir, named))

    by_table = {}
    for r in rows:
        by_table.setdefault(r.table, []).append(r)

    if arity_override is not None:
        arities = {}
        for daddr, d in tables.items():
            arities[d["sym"]] = arity_override
        unresolved = []
    else:
        by_addr, unresolved = parse_arity(root, tables)
        arities = {tables[a]["sym"]: v for a, v in by_addr.items()}
        if unresolved:
            die("cannot derive the dispatch arity for %d table(s):\n%s\n"
                "Arity comes from the dispatching TU's member-pointer "
                "typedef (`typedef void (C::*PMF)(int);` plus `extern PMF "
                "data_...[];`). Remedy: pass --arity N when every table "
                "shares one arity -- the State-object shape that "
                "port/hal/player_states.inc uses is --arity 0, because "
                "those objects are dispatched through hal_call_state_fn "
                "rather than a typed extern"
                % (len(unresolved),
                   "\n".join("    %s (0x%08x): %s" % (s, a, w)
                             for a, s, w in unresolved)))

    return {"ov": ovn, "ovdir": ovdir, "tables": tables, "rows": rows,
            "by_table": by_table, "arities": arities, "refusals": refusals,
            "romnames": set(syms.values())}


# ---------------------------------------------------------------------------
# emitters


def emit_report(m):
    out = []
    out.append("ov%03d: %d table(s), %d slot(s), %d distinct state address(es)"
               % (m["ov"], len(m["tables"]), len(m["rows"]),
                  len({r.addr for r in m["rows"]})))
    out.append("")
    out.append("  table                n  arity  state addresses (slot order)")
    out.append("  -------------------  -  -----  ----------------------------")
    for t in sorted(m["by_table"]):
        rs = m["by_table"][t]
        out.append("  %-19s %2d  %5s  %s"
                   % (t, len(rs), m["arities"].get(t, "?"),
                      " ".join("%08x" % r.addr for r in rs)))
    out.append("")
    for t in sorted(m["by_table"]):
        for r in m["by_table"][t]:
            out.append("    %-19s slot %-2d %-6s 0x%08x  %-38s %s"
                       % (t, r.slot, r.slotname, r.addr,
                          r.sym or "(no symbol)",
                          r.owner or "NO DELINK BLOCK"))
    if m["refusals"]:
        out.append("")
        out.append("REFUSALS (%d):" % len(m["refusals"]))
        for x in m["refusals"]:
            out.append("  * " + x)
    else:
        out.append("")
        out.append("REFUSALS: none")
    return "\n".join(out) + "\n"


def call_text(r, arity, cls, selfvar):
    """The default call shape for one row. Rulings are the lane's."""
    if r.hole:
        return None
    dm = demangle_method(r.sym) if r.sym else None
    if dm and cls and dm[0] == cls:
        return "return ((%s *)%s)->%s::%s();" % (cls, selfvar, dm[0], dm[1])
    # a C-named body under a --class run is the player include's shape:
    # the object is passed as a plain pointer, cast at the call site.
    first = "(char *)%s" % selfvar if cls else "c"
    args = [first] if arity == 0 else [first, "a"]
    return "%s(%s); return 1;" % (r.sym, ", ".join(args))


def emit_switch(m, cls=None, selfvar="self", holefn=None):
    """The address-switch include, the player_states.inc shape."""
    ovn = m["ov"]
    hole = holefn or "port_state_no_body_ov%03d" % ovn
    groups = {}
    for t, rs in m["by_table"].items():
        groups.setdefault(m["arities"].get(t, 0), []).extend(rs)
    out = []
    out.append("/* GENERATED by port/tools/stategen.py from the ov%03d "
               "constructor's pair" % ovn)
    out.append(" * tables and the raw overlay image: DS state-fn address -> "
               "host call.")
    out.append(" * Call shapes are the default ones; a ruling (a C-linkage "
               "body, a")
    out.append(" * forwarding bridge, a (void) body in an argument slot) is "
               "the lane's")
    out.append(" * to make and this file is a starting point, not an "
               "artifact to commit")
    out.append(" * unread. */")
    for arity in sorted(groups):
        rs = sorted(groups[arity], key=lambda r: r.addr)
        if len(groups) > 1:
            out.append("")
            out.append("/* ---- arity %d ---- */" % arity)
        seen = set()
        for r in rs:
            if r.addr in seen:
                continue
            seen.add(r.addr)
            body = call_text(r, arity, cls, selfvar)
            if body is None:
                out.append("    /* NO DECOMPILED BODY: slot %d/%s of %s. "
                           "Reported, never called. */"
                           % (r.slot, r.slotname, r.table))
                out.append("    case 0x%08x: %s(0x%08x); return 1;"
                           % (r.addr, hole, r.addr))
            else:
                out.append("    case 0x%08x: %s" % (r.addr, body))
    return "\n".join(out) + "\n"


def emit_host(m, cls=None):
    """The skeleton of the dispatching host copy."""
    ovn = m["ov"]
    holes = [r for r in m["rows"] if r.hole]
    body_arity = {}
    for t, rs in m["by_table"].items():
        for r in rs:
            if r.hole or not r.sym:
                continue
            if cls and demangle_method(r.sym) \
                    and demangle_method(r.sym)[0] == cls:
                continue
            body_arity.setdefault(r.sym, set()).add(m["arities"].get(t, 0))
    bodies = sorted(body_arity)
    methods = sorted({r.sym for r in m["rows"]
                      if not r.hole and r.sym and cls
                      and demangle_method(r.sym)
                      and demangle_method(r.sym)[0] == cls})
    out = []
    out.append("// PORT_HOST_ABI. SKELETON generated by "
               "port/tools/stategen.py.")
    out.append("// The mwcc POINTER-TO-MEMBER WALL for ov%03d: the "
               "dispatching TUs' PMF" % ovn)
    out.append("// sites become the address switch below. Read "
               "port/mg_fanout_costs.txt")
    out.append("// section 4 first. THE HOST COPIES ARE NOT GENERATED -- "
               "each one is a")
    out.append("// transcription of its matched src with the PMF site "
               "replaced, and this")
    out.append("// tool refuses to guess a body it has not read.")
    out.append("")
    out.append("#include <cstdio>")
    out.append("")
    out.append("struct MgPmf { unsigned code; int adj; };")
    out.append("")
    out.append("extern \"C\" {")
    out.append("")
    out.append("/* the %d state bodies with a delink block, in address "
               "order */" % len(bodies))
    out.append("/* Signatures are the ones the SLOT implies. Check each "
               "against its src:")
    out.append("   a ride-through body sitting in an argument slot takes "
               "(void) on the DS,")
    out.append("   because r0 and r1 already hold self and the index, and "
               "the faithful host")
    out.append("   form is to call it as it is declared. */")
    for s in bodies:
        ars = sorted(body_arity[s])
        if len(ars) > 1:
            out.append("/* %s is dispatched at arities %s -- rule on it "
                       "before declaring it */"
                       % (s, ", ".join(str(a) for a in ars)))
            continue
        sig = "char *c" if ars[0] == 0 else "char *c, int a"
        out.append("void %s(%s);" % (s, sig))
    if methods:
        out.append("")
        out.append("/* %d rows resolve to %s methods and are called as "
                   "methods */" % (len(methods), cls))
        for s in methods:
            dm = demangle_method(s)
            out.append("/*   %s -> %s::%s */" % (s, dm[0], dm[1]))
    out.append("")
    out.append("/* the %d mount tables, re-typed to the ROM's eight-byte "
               "pair. The mount" % len(m["by_table"]))
    out.append("   defines the storage; the constructor fills it at scene "
               "load. */")
    for t in sorted(m["by_table"]):
        out.append("extern MgPmf %s[];" % t)
    out.append("")
    out.append("}  /* extern \"C\" */")
    out.append("")
    if holes:
        out.append("/* The decomp hole%s, reported at the dispatch site "
                   "rather than stubbed. */" % ("s" if len(holes) > 1 else ""))
        out.append("static void port_state_no_body_ov%03d(unsigned addr)"
                   % ovn)
        out.append("{")
        out.append("    std::fprintf(stderr, \"  [state] ov%03d STATE "
                   "0x%%08x HAS NO DECOMPILED \"" % ovn)
        out.append("                 \"BODY: no delink block and no src "
                   "file. The state was not \"")
        out.append("                 \"entered.\\n\", addr);")
        out.append("    std::fflush(stderr);")
        out.append("}")
        out.append("")
    out.append("// ---- the class's address switch "
               "------------------------------------------")
    out.append("")
    for arity in sorted({m["arities"].get(t, 0) for t in m["by_table"]}):
        sig = "void *self" if arity == 0 else "void *self, int a"
        out.append("extern \"C\" int port_state_try_ov%03d_%d(%s)"
                   % (ovn, arity, sig))
        out.append("{")
        out.append("    switch (code_from_the_pair_at_the_call_site) {")
        out.append("    /* paste the --emit-switch arity-%d block here */"
                   % arity)
        out.append("    default: return 0;")
        out.append("    }")
        out.append("}")
        out.append("")
    out.append("// ---- the host copies -------------------------------------"
               "------------------")
    out.append("//")
    out.append("// One per dispatching TU. stategen does NOT write these: a "
               "host copy is the")
    out.append("// matched src transcribed with `(c->*table[j])(...)` "
               "replaced by a call")
    out.append("// through the switch above, and everything else verbatim. "
               "Copy the src,")
    out.append("// replace the site, and say in a comment what you changed "
               "and why.")
    return "\n".join(out) + "\n"


# ---------------------------------------------------------------------------
# reconstruction of the two hand artifacts


HAND_MG = "port/unmatched/MgCurling_StateDispatch.cpp"
HAND_MG_SINIT = "src/__sinit_ov006_021304ac.c"
HAND_PL = "port/hal/player_states.inc"
HAND_PL_SINIT = "src/__sinit_ov002_021019d0.c"

MG_CASE = re.compile(r"case\s+0x([0-9a-fA-F]{8})u:\s*(\w+)\s*\(")
PL_CASE = re.compile(r"^\s*case\s+0x([0-9a-fA-F]{8}):\s*(.+?)\s*$", re.M)
PL_METHOD = re.compile(r"->\s*(\w+)\s*::\s*(\w+)\s*\(")
PL_CALL = re.compile(r"(?:return\s+)?([A-Za-z_]\w*)\s*\(")


def parse_hand_mg(path):
    """{addr: target-name} out of the hand switch."""
    text = pathlib.Path(path).read_text(errors="replace")
    out = {}
    for m in MG_CASE.finditer(text):
        out.setdefault(int(m.group(1), 16), m.group(2))
    return out


def parse_hand_player(path):
    """{addr: target-symbol} out of the hand include."""
    text = pathlib.Path(path).read_text(errors="replace")
    out = {}
    n = 0
    for m in PL_CASE.finditer(text):
        addr, body = int(m.group(1), 16), m.group(2)
        n += 1
        mm = PL_METHOD.search(body)
        if mm:
            out[addr] = mangle_method(mm.group(1), mm.group(2))
            continue
        cm = PL_CALL.search(body)
        out[addr] = cm.group(1) if cm else "(unparsed)"
    return out, n


def cmd_reconstruct(root):
    root = pathlib.Path(root)
    bad = 0

    # ---- (a) the 25-case curling switch -----------------------------------
    print("=" * 70)
    print("(a) %s" % HAND_MG)
    print("=" * 70)
    m = build(root, HAND_MG_SINIT)
    hand = parse_hand_mg(root / HAND_MG)
    derived = {}
    for r in m["rows"]:
        derived[r.addr] = None if r.hole else r.sym
    print("derived %d slots, %d distinct addresses; hand switch names %d"
          % (len(m["rows"]), len(derived), len(hand)))
    only_d = sorted(set(derived) - set(hand))
    only_h = sorted(set(hand) - set(derived))
    for a in only_d:
        print("  DIVERGE derived 0x%08x is NOT in the hand switch" % a)
        bad += 1
    for a in only_h:
        print("  DIVERGE hand 0x%08x is NOT derivable from the constructor"
              % a)
        bad += 1
    # A hand target that is NOT a ROM symbol name is a HOST-SIDE RULING, the
    # same rule arm (b) below already applies: a forwarding bridge, or a host
    # copy standing in for a body the port cannot compile. That is a lane's
    # judgment, not a contradiction, and the test is membership in the loaded
    # symbol tables rather than a name prefix, for the reason (b) states.
    #
    # This arm did not need the rule until the 2026-09-12 main -> port sync.
    # Before it, 0x020e1854 had no delink block on either side, so the hole
    # branch below let the hand's port_mg_curling_st_020e1854 through. main has
    # since decompiled that address, so the derived side now names
    # func_ov006_020e1854 and the SAME deliberate host copy started reading as
    # a divergence. Nothing about the port's ruling changed; the decomp caught
    # up with it, which is the case (b) was already written to recognise.
    romnames_mg = m["romnames"]
    bridges_mg = []
    mism = 0
    for a in sorted(set(derived) & set(hand)):
        d, h = derived[a], hand[a]
        if h not in romnames_mg:
            bridges_mg.append((a, d, h))
            continue
        if d is None:
            # the hole: the hand file must NOT call a ROM symbol for it
            if h.startswith("func_") or h.startswith("_Z"):
                print("  DIVERGE 0x%08x has no delink block but the hand "
                      "switch calls %s" % (a, h))
                bad += 1
                mism += 1
            continue
        if d != h:
            print("  DIVERGE 0x%08x derived %s, hand %s" % (a, d, h))
            bad += 1
            mism += 1
    if bridges_mg:
        print("host-side rulings (hand rows naming a port symbol rather "
              "than a ROM one -- a bridge or a host copy; a ruling, not a "
              "divergence): %d" % len(bridges_mg))
        for a, d, h in bridges_mg:
            print("    0x%08x  ROM %-38s hand %s"
                  % (a, d or "NO DELINK BLOCK", h))
    print("address sets %s; target symbols %s"
          % ("EQUAL" if not (only_d or only_h) else "DIFFER",
             "EQUAL" if not mism else "%d DIFFER" % mism))
    print("refusals: %d" % len(m["refusals"]))
    for x in m["refusals"]:
        print("  * %s" % x)

    # ---- (b) the 197-row player include -----------------------------------
    print("")
    print("=" * 70)
    print("(b) %s" % HAND_PL)
    print("=" * 70)
    p = build(root, HAND_PL_SINIT, arity_override=0)
    hand2, nrows = parse_hand_player(root / HAND_PL)
    derived2 = {}
    for r in p["rows"]:
        if r.addr in derived2 and derived2[r.addr] is not None:
            continue
        derived2[r.addr] = None if r.hole else r.sym
    print("derived %d slots, %d distinct addresses; hand include has %d rows, "
          "%d distinct" % (len(p["rows"]), len(derived2), nrows, len(hand2)))
    missing = sorted(set(hand2) - set(derived2))
    for a in missing:
        print("  DIVERGE hand 0x%08x is NOT derivable from the constructor"
              % a)
        bad += 1
    # NARROWNESS WORTH KNOWING: romnames holds arm9 plus the ONE overlay
    # this constructor lives in. A hand row calling a ROM symbol owned by
    # a THIRD overlay falls through to the host-side-ruling bin instead of
    # being checked. Neither precedent has such a row; a cross-overlay
    # class needs load_symbols widened first.
    #
    # A hand target that is NOT a ROM symbol name is a HOST-SIDE RULING --
    # a forwarding bridge, or a host copy of a body whose matched src
    # cannot compile. That is a lane's judgment, not a contradiction. The
    # test is membership in the loaded symbol tables, NOT a func_/_Z name
    # prefix: port/unmatched/func_ov002_020c6f3c.cpp is a host copy whose
    # name is spelled exactly like a config symbol and appears in no
    # symbols.txt at all, and a prefix rule reads it as a divergence.
    romnames = p["romnames"]
    mism2 = 0
    bridges = []
    for a in sorted(set(derived2) & set(hand2)):
        d, h = derived2[a], hand2[a]
        if h not in romnames:
            bridges.append((a, d, h))
            continue
        if d is None:
            print("  DIVERGE 0x%08x has no delink block but the hand "
                  "include calls the ROM symbol %s" % (a, h))
            bad += 1
            mism2 += 1
            continue
        if d != h:
            print("  DIVERGE 0x%08x derived %s, hand %s" % (a, d, h))
            bad += 1
            mism2 += 1
    print("all %d hand rows derivable: %s" % (nrows, "YES" if not missing
                                              else "NO"))
    print("target symbols: %s"
          % ("EQUAL on every shared row" if not mism2
             else "%d DIFFER" % mism2))
    if bridges:
        print("host-side rulings (hand rows naming a port symbol rather "
              "than a ROM one -- a bridge or a host copy; a ruling, not a "
              "divergence): %d" % len(bridges))
        for a, d, h in bridges:
            print("    0x%08x  ROM %-38s hand %s"
                  % (a, d or "NO DELINK BLOCK", h))
    extra = sorted(set(derived2) - set(hand2))
    cand = [a for a in extra if derived2[a] is not None]
    holed = [a for a in extra if derived2[a] is None]
    # Do NOT report these as one number. A row with a body is a CANDIDATE
    # the hand file is missing; a row with no delink block is this tool's
    # own refusal and could never have been a row. Counting them together
    # reads as "the hand file is short by N" and overstates it by the
    # refusals.
    print("derived rows ABSENT from the hand include: %d candidate(s) with "
          "a body, %d refusal(s) with no delink block"
          % (len(cand), len(holed)))
    for a in cand:
        r = [x for x in p["rows"] if x.addr == a][0]
        print("    CANDIDATE 0x%08x  %-38s %s"
              % (a, r.sym or "(no symbol)", r.owner))
    for a in holed:
        r = [x for x in p["rows"] if x.addr == a][0]
        print("    REFUSAL   0x%08x  %-38s NO DELINK BLOCK, never a row"
              % (a, r.sym or "(no symbol)"))
    print("refusals: %d" % len(p["refusals"]))
    for x in p["refusals"]:
        print("  * %s" % x)

    print("")
    if bad:
        print("stategen: %d DIVERGENCE(S). A divergence is a CONTRADICTION "
              "TO INVESTIGATE, not automatically the hand file's error -- "
              "adjudicate each against the ROM's own table data." % bad)
        return 1
    print("reconstruction: both hand artifacts reproduced on the address "
          "and target-symbol axes.")
    return 0


# ---------------------------------------------------------------------------


def selftest():
    """Every parse, read and refusal against fabricated fixtures.

    One synthetic overlay, no ROM bytes: .text 0x1000..0x2000 with a src
    block at 0x1100 and another at 0x1200 and a GAP at 0x1300, .data
    0x2000..0x3000 holding four pairs, .bss 0x3000..0x3100 holding two
    destination tables. The constructor declares a Pair and two
    destination structs and fills both. Expected: two tables, five slots,
    slot order from the struct declaration (NOT lexicographic), the gap
    slot reads as a hole, arity 1 derived through a struct wrapper and
    arity 0 from a bare typedef.
    """
    ok = True

    def expect(cond, what, got):
        nonlocal ok
        if not cond:
            print("FAIL: %s: %s" % (what, got))
            ok = False

    def caught(fn, needle, what):
        nonlocal ok
        try:
            fn()
        except SystemExit as e:
            if needle in str(e):
                return
            print("FAIL: %s: wrong refusal: %s" % (what, e))
            ok = False
            return
        print("FAIL: %s: ACCEPTED, no SystemExit" % what)
        ok = False

    with tempfile.TemporaryDirectory() as td:
        tdp = pathlib.Path(td)
        (tdp / "src").mkdir()
        (tdp / "port").mkdir()
        (tdp / "config" / "arm9" / "overlays" / "ov009").mkdir(parents=True)
        (tdp / "extracted" / "overlays").mkdir(parents=True)

        dl = tdp / "config/arm9/overlays/ov009/delinks.txt"
        dl.write_text(
            "    .text       start:0x1000 end:0x2000 kind:code align:32\n"
            "    .data       start:0x2000 end:0x3000 kind:data align:32\n"
            "    .bss        start:0x3000 end:0x3100 kind:bss align:32\n"
            "src/f_1100.c:\n"
            "    .text start:0x1100 end:0x1200\n"
            "src/f_1200.c:\n"
            "    .text start:0x1200 end:0x1300\n"
            "src/f_1400.c:\n"
            "    .text start:0x1400 end:0x1500\n")
        # symbols.txt NAMES the gap address on purpose: that is the
        # func_ov006_020e1854 shape, and the arm proves the tool keys the
        # hole off the delinks join and not off symbols.txt.
        (tdp / "config/arm9/overlays/ov009/symbols.txt").write_text(
            "f_1100 kind:function(arm,size=0x100) addr:0x00001100\n"
            "f_1200 kind:function(arm,size=0x100) addr:0x00001200\n"
            "f_1300 kind:function(arm,size=0x100) addr:0x00001300\n"
            "_ZN3Cls7St_MainEv kind:function(arm,size=0x100) "
            "addr:0x00001400\n")

        img = bytearray(0x2000)          # 0x1000..0x3000, .bss excluded

        def put(addr, code, adj=0):
            struct.pack_into("<II", img, addr - 0x1000, code, adj)

        put(0x2000, 0x1100)
        put(0x2008, 0x1200)
        put(0x2010, 0x1300)              # the GAP -> a hole
        put(0x2018, 0x1400)              # a Cls method
        put(0x2020, 0x1100, 0x00000001)  # nonzero adjustment, refusal arm
        put(0x2028, 0x00000000)          # zero code word, refusal arm
        (tdp / "extracted/overlays/overlay_0009.bin").write_bytes(img)

        sinit = tdp / "src/__sinit_ov009_00001000.c"
        base_sinit = (
            "struct Pair { int a, b; };\n"
            # a THREE-scalar struct, declared after Pair on purpose: the
            # pair is the two-scalar one and nothing else, and a detector
            # that accepts "two or more scalars" takes this one instead.
            "struct Hdr { int x, y, z; };\n"
            # two ARRAY fields of int. This is the shape that beat the
            # real pair in __sinit_ov006_02131fa4 when the candidate test
            # counted fields by type alone; a pair field is a scalar, so
            # this must not be a candidate and the base build proves it by
            # not raising the ambiguity refusal.
            "struct ArrPair { int x[3]; int y[3]; };\n"
            "struct D3 { struct Pair s0, s1, s2; };\n"
            "struct D2 { struct Pair lo, hi; };\n"
            "extern struct D3 data_ov009_00003000;\n"
            "extern struct D2 data_ov009_00003020;\n"
            "extern struct Pair data_ov009_00002000;\n"
            "extern struct Pair data_ov009_00002008;\n"
            "extern struct Pair data_ov009_00002010;\n"
            "extern struct Pair data_ov009_00002018;\n"
            "extern struct Pair data_ov009_00002020;\n"
            "extern struct Pair data_ov009_00002028;\n"
            "void __sinit_ov009_00001000(void)\n"
            "{\n"
            "    int ta, tb;\n"
            # deliberately OUT of declaration order, and the .tail.a
            # scalar-write shape the player constructor uses, so the arm
            # proves the ordering comes from the struct decl and that the
            # scalar write is not read as a slot.
            "    data_ov009_00003000.s2 = data_ov009_00002010;\n"
            "    data_ov009_00003000.s0 = data_ov009_00002000;\n"
            "    data_ov009_00003000.s1 = data_ov009_00002008;\n"
            "    data_ov009_00003020.hi = data_ov009_00002018;\n"
            "    data_ov009_00003020.lo = data_ov009_00002000;\n"
            "    data_ov009_00003020.lo.a = tb ? ta : ta;\n"
            # the WORD-INDEX pair copy, and it reaches PAST the declared
            # struct (D2 has two fields, this writes slot 2) exactly as
            # data_ov002_0211073c does.
            "    ((int *)&data_ov009_00003020)[4] = "
            "((int *)&data_ov009_00002008)[0];\n"
            "    ((int *)&data_ov009_00003020)[5] = "
            "((int *)&data_ov009_00002008)[1];\n"
            "}\n")
        sinit.write_text(base_sinit)

        # two dispatching TUs: one wraps its typedef in a struct (arity 1),
        # one uses a bare typedef (arity 0)
        (tdp / "src/disp_a.cpp").write_text(
            "struct C;\n"
            "typedef void (C::*PMF)(int);\n"
            "struct Entry { PMF pmf[1]; };\n"
            "extern Entry data_ov009_00003000[];\n")
        (tdp / "src/disp_b.cpp").write_text(
            "struct C;\n"
            "typedef void (C::*PMF0)();\n"
            "extern PMF0 data_ov009_00003020[];\n")
        (tdp / "port/ov009_syms.txt").write_text(
            "data_ov009_00003000 data_ov009_00003020\n")

        m = build(tdp, "src/__sinit_ov009_00001000.c")
        expect(len(m["tables"]) == 2, "table count", len(m["tables"]))
        expect(len(m["rows"]) == 6, "slot count", len(m["rows"]))
        d3 = m["by_table"]["data_ov009_00003000"]
        expect([r.addr for r in d3] == [0x1100, 0x1200, 0x1300],
               "slot order from the struct declaration, not the file order",
               ["%x" % r.addr for r in d3])
        d2 = m["by_table"]["data_ov009_00003020"]
        expect([r.addr for r in d2] == [0x1100, 0x1400, 0x1200],
               "word-index pair copy lands as slot 2, past the declared "
               "struct's two fields", ["%x" % r.addr for r in d2])
        expect([r.slotname for r in d2] == ["lo", "hi", "[2]"],
               "named-field and word-index slots are labelled apart",
               [r.slotname for r in d2])
        expect(m["arities"] == {"data_ov009_00003000": 1,
                                "data_ov009_00003020": 0},
               "arity derived through a struct wrapper and a bare typedef",
               m["arities"])
        holes = [r for r in m["rows"] if r.hole]
        expect([r.addr for r in holes] == [0x1300],
               "the gap address is the only hole even though symbols.txt "
               "names it", ["%x" % r.addr for r in holes])
        expect(len(m["refusals"]) == 1 and "no delink block"
               in m["refusals"][0] and "f_1300" in m["refusals"][0],
               "hole refusal names the symbol a name-shaped check would "
               "have called", m["refusals"])

        # THE TWO KINDS OF HOLE, run link60 lane MGB after review. The join
        # test above answers "is there a delink block", and the message used
        # to report that as "no delink block AND NO SRC FILE". Those are
        # independent questions and the tool never checked the second one.
        # The base fixture's f_1300 has no src, so it reads as it always did;
        # dropping a src file at that name must flip the message and must NOT
        # stop it being a hole. Both real precedents turned out to be the
        # second kind -- _ZN14dScMgMemory2_c14RoundShowCardsEv (NONMATCHING) and
        # _ZN6Player13St_Climb_MainEv (unbannered) -- so the old text was
        # wrong on live data, not just in principle.
        expect("HAS NO DECOMPILED BODY" in m["refusals"][0]
               and "no src file" in m["refusals"][0],
               "a hole with no src still reports as undecompiled",
               m["refusals"][0])
        (tdp / "src" / "f_1300.c").write_text("/* a decompiled body */\n")
        m2 = build(tdp, "src/__sinit_ov009_00001000.c")
        expect(len(m2["refusals"]) == 1
               and "IS NOT IN THE DELINKS JOIN" in m2["refusals"][0]
               and "src/f_1300.c exists" in m2["refusals"][0]
               and "HAS NO DECOMPILED BODY" not in m2["refusals"][0],
               "a hole WITH a src file reports as a tool boundary",
               m2["refusals"])
        expect([r.addr for r in m2["rows"] if r.hole] == [0x1300],
               "it is still a hole -- the src file does not create a delink "
               "block", [r.addr for r in m2["rows"] if r.hole])
        (tdp / "src" / "f_1300.c").unlink()

        sw = emit_switch(m, cls="Cls")
        expect("case 0x00001100:" in sw and "case 0x00001400:" in sw,
               "switch emits the resolved cases", sw)
        expect("((Cls *)self)->Cls::St_Main();" in sw,
               "a demangled method row emits the method-call shape", sw)
        expect("port_state_no_body_ov009(0x00001300)" in sw
               and "f_1300(" not in sw,
               "the hole emits a report and NEVER a call to its symbol", sw)
        expect("arity 1" in sw and "arity 0" in sw,
               "mixed arities are emitted as separate blocks", sw)
        host = emit_host(m, cls="Cls")
        expect("extern MgPmf data_ov009_00003000[];" in host
               and "struct MgPmf { unsigned code; int adj; };" in host,
               "host skeleton re-types the mount to the eight-byte pair",
               host)

        rep = emit_report(m)
        expect("REFUSALS (1)" in rep, "report carries the refusal", rep)

        # ---- the third shape: an ANONYMOUS TYPEDEF pair and a
        # destination declared as an ARRAY of it, written by index. Seven
        # of ov006's constructors use only this spelling. The int[]
        # permutation copy alongside it must NOT read as a state table:
        # those constructors move plain index tables between two data
        # symbols with a statement of the same shape.
        (tdp / "port/ov009_syms.txt").write_text(
            "data_ov009_00003000 data_ov009_00003020 "
            "data_ov009_00003040\n")
        arr = tdp / "src/__sinit_ov009_00001010.c"
        arr.write_text(
            "typedef struct { int a, b; } Ent;\n"
            "extern Ent data_ov009_00003040[3];\n"
            "extern int data_ov009_00003060[];\n"
            "extern int data_ov009_00002100[];\n"
            "extern Ent data_ov009_00002000;\n"
            "extern Ent data_ov009_00002008;\n"
            "extern Ent data_ov009_00002018;\n"
            "void __sinit_ov009_00001010(void)\n"
            "{\n"
            "    data_ov009_00003040[2] = data_ov009_00002018;\n"
            "    data_ov009_00003040[0] = data_ov009_00002000;\n"
            "    data_ov009_00003040[1] = data_ov009_00002008;\n"
            "    data_ov009_00003060[0] = data_ov009_00002100[0];\n"
            "    data_ov009_00003060[1] = data_ov009_00002100[3];\n"
            "}\n")
        ma = build(tdp, "src/__sinit_ov009_00001010.c", arity_override=0)
        expect(len(ma["tables"]) == 1,
               "the int[] permutation copy is not read as a pair table",
               list(ma["tables"]))
        rows = ma["by_table"]["data_ov009_00003040"]
        expect([r.addr for r in rows] == [0x1100, 0x1200, 0x1400],
               "array-index slots ordered by index, not by file order",
               ["%x" % r.addr for r in rows])

        arrbad = tdp / "src/__sinit_ov009_00001011.c"
        arrbad.write_text(
            arr.read_text()
            .replace("__sinit_ov009_00001010", "__sinit_ov009_00001011")
            .replace("    data_ov009_00003040[1] = data_ov009_00002008;\n",
                     "    data_ov009_00003040[1].a = 0;\n"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001011.c",
                             arity_override=0),
               "shape this parser does not know",
               "partial write to a pair array")

        # ---- the FOURTH shape, run link60 lane MGB: a destination declared
        # through an ANONYMOUS TYPEDEF whose one field is an ARRAY of the
        # pair type, written field-then-index. This is
        # __sinit_ov006_021314e4's spelling and it is one of the five the STG
        # lane recorded as unread. What made it worth an arm rather than a
        # tolerated gap is that the OLD failure was silent in the worst
        # direction: every line fell through the unknown-write branch (whose
        # dest_arr and dest_type tests both missed a typedef'd struct), so
        # the tool answered "no pair assignments" and the constructor read as
        # one that builds no state table at all -- indistinguishable, from
        # the report, from the eleven that genuinely build none.
        #
        # The arm is an EQUIVALENCE: the same three slots as the array
        # spelling above, so a parse that drifts fails against a table the
        # other shape already agrees on.
        td4 = tdp / "src/__sinit_ov009_00001020.c"
        td4.write_text(
            "typedef struct { int a, b; } Ent;\n"
            "typedef struct { Ent p[3]; } Dst3;\n"
            "extern Dst3 data_ov009_00003040;\n"
            "extern int data_ov009_00003060[];\n"
            "extern int data_ov009_00002100[];\n"
            "extern Ent data_ov009_00002000;\n"
            "extern Ent data_ov009_00002008;\n"
            "extern Ent data_ov009_00002018;\n"
            "void __sinit_ov009_00001020(void)\n"
            "{\n"
            "    data_ov009_00003040.p[2] = data_ov009_00002018;\n"
            "    data_ov009_00003040.p[0] = data_ov009_00002000;\n"
            "    data_ov009_00003040.p[1] = data_ov009_00002008;\n"
            "    data_ov009_00003060[0] = data_ov009_00002100[0];\n"
            "}\n")
        mt = build(tdp, "src/__sinit_ov009_00001020.c", arity_override=0)
        expect(len(mt["tables"]) == 1,
               "typedef'd array destination: the int[] copy beside it is "
               "still not a pair table", list(mt["tables"]))
        rows = mt["by_table"]["data_ov009_00003040"]
        expect([r.addr for r in rows] == [0x1100, 0x1200, 0x1400],
               "typedef'd .p[i] slots ordered by index, not by file order",
               ["%x" % r.addr for r in rows])
        expect([r.slotname for r in rows] == ["p[0]", "p[1]", "p[2]"],
               "typedef'd slots carry the field-and-index label",
               [r.slotname for r in rows])

        def td4_variant(n, old, new):
            rel = "src/__sinit_ov009_0000%s.c" % n
            (tdp / rel).write_text(
                td4.read_text()
                .replace("__sinit_ov009_00001020", "__sinit_ov009_0000" + n)
                .replace(old, new))
            return rel

        # out of range against the DECLARED extent is a refusal, not slot 3
        caught(lambda: build(tdp, td4_variant(
            "1021", "data_ov009_00003040.p[2] =",
            "data_ov009_00003040.p[3] ="), arity_override=0),
               "declared extent is 3", "index past the declared extent")

        # a field name the struct does not declare. The constructor's own
        # declaration is the slot order, so a mismatch is a parse error and
        # not a second field to invent an offset for.
        caught(lambda: build(tdp, td4_variant(
            "1024", "data_ov009_00003040.p[0] =",
            "data_ov009_00003040.q[0] ="), arity_override=0),
               "only field is p", "an undeclared field name")

        # a write to the table in a shape the parser cannot read must REFUSE
        # rather than fall through the way the whole spelling used to
        caught(lambda: build(tdp, td4_variant(
            "1022",
            "    data_ov009_00003040.p[1] = data_ov009_00002008;\n",
            "    data_ov009_00003040.p[1].a = 0;\n"), arity_override=0),
               "shape this parser does not know",
               "partial write to a typedef'd pair table")

        # TWO array fields: the second array's slot base depends on the
        # first's extent, so the tool must refuse rather than compute one.
        caught(lambda: build(tdp, td4_variant(
            "1023", "typedef struct { Ent p[3]; } Dst3;",
            "typedef struct { Ent p[3]; Ent q[2]; } Dst3;"),
            arity_override=0),
               "more than one array of the pair type",
               "two array fields in one destination struct")

        # ---- refusals -----------------------------------------------------
        caught(lambda: Overlay("cut", _write(tdp / "cut.bin", img[:-4]), dl),
               "stale or wrong image", "truncated image")

        nomount = tdp / "port/ov009_syms.txt"
        nomount.write_text("data_ov009_00003020\n")
        caught(lambda: build(tdp, "src/__sinit_ov009_00001000.c"),
               "NOT MOUNTED", "unmounted destination table")
        nomount.write_text("data_ov009_00003000 data_ov009_00003020\n")
        # a size- or address-pinned row IS mounted (ovdata.py reads
        # name:0xNN and name@0xADDR as the name with a pin)
        nomount.write_text("data_ov009_00003000:0x18 "
                           "data_ov009_00003020@0x3020\n")
        mp = build(tdp, "src/__sinit_ov009_00001000.c")
        expect(len(mp["tables"]) == 2, "a pinned mount row counts as mounted",
               len(mp["tables"]))
        nomount.write_text("data_ov009_00003000 data_ov009_00003020\n")

        adj = tdp / "src/__sinit_ov009_00001001.c"
        adj.write_text(base_sinit
                       .replace("__sinit_ov009_00001000",
                                "__sinit_ov009_00001001")
                       .replace("data_ov009_00003020.hi = "
                                "data_ov009_00002018;",
                                "data_ov009_00003020.hi = "
                                "data_ov009_00002020;"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001001.c"),
               "adjustment 0x00000001", "nonzero adjustment word")

        # a ZERO code word. Without its own arm this becomes
        # `case 0x00000000` routed to the no-body reporter: safe, and
        # silent about which of the three causes it is.
        zero = tdp / "src/__sinit_ov009_00001012.c"
        zero.write_text(base_sinit
                        .replace("__sinit_ov009_00001000",
                                 "__sinit_ov009_00001012")
                        .replace("data_ov009_00003020.hi = "
                                 "data_ov009_00002018;",
                                 "data_ov009_00003020.hi = "
                                 "data_ov009_00002028;"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001012.c"),
               "ZERO code word", "zero code word")

        # two competing pair structs, the coordinate-pair-declared-last
        # shape. Refusing must NAME both candidates rather than picking.
        two = tdp / "src/__sinit_ov009_00001013.c"
        two.write_text(base_sinit
                       .replace("__sinit_ov009_00001000",
                                "__sinit_ov009_00001013")
                       .replace("struct Hdr { int x, y, z; };",
                                "struct Hdr { int x, y, z; };\n"
                                "struct Coord { int cx, cy; };"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001013.c"),
               "candidate pair structs", "two competing pair structs")
        try:
            build(tdp, "src/__sinit_ov009_00001013.c")
        except SystemExit as e:
            expect("Coord" in str(e) and "Pair" in str(e),
                   "the ambiguity refusal names BOTH candidates", str(e))

        # a destination that is not in .bss
        notbss = tdp / "src/__sinit_ov009_00001002.c"
        notbss.write_text(base_sinit
                          .replace("__sinit_ov009_00001000",
                                   "__sinit_ov009_00001002")
                          .replace("data_ov009_00003000", "data_ov009_00002800"))
        (tdp / "port/ov009_syms.txt").write_text(
            "data_ov009_00002800 data_ov009_00003020\n")
        caught(lambda: build(tdp, "src/__sinit_ov009_00001002.c"),
               "not in ov009 .bss", "destination outside .bss")
        (tdp / "port/ov009_syms.txt").write_text(
            "data_ov009_00003000 data_ov009_00003020\n")

        # a pair source outside .data
        outdata = tdp / "src/__sinit_ov009_00001003.c"
        outdata.write_text(base_sinit
                           .replace("__sinit_ov009_00001000",
                                    "__sinit_ov009_00001003")
                           .replace("data_ov009_00002010",
                                    "data_ov009_00001800"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001003.c"),
               "not inside ov009 .data", "pair source outside .data")

        # arity underivable -> the named remedy, and --arity takes it
        (tdp / "src/disp_b.cpp").unlink()
        caught(lambda: build(tdp, "src/__sinit_ov009_00001000.c"),
               "--arity", "underivable arity names its remedy")
        m0 = build(tdp, "src/__sinit_ov009_00001000.c", arity_override=0)
        expect(set(m0["arities"].values()) == {0},
               "--arity overrides derivation", m0["arities"])

        # two TUs disagreeing about one table's arity
        (tdp / "src/disp_b.cpp").write_text(
            "struct C;\n"
            "typedef void (C::*PMF0)();\n"
            "extern PMF0 data_ov009_00003020[];\n")
        (tdp / "src/disp_c.cpp").write_text(
            "struct C;\n"
            "typedef void (C::*PMFX)(int);\n"
            "extern PMFX data_ov009_00003020[];\n")
        caught(lambda: build(tdp, "src/__sinit_ov009_00001000.c"),
               "two TUs disagree", "conflicting arities")
        (tdp / "src/disp_c.cpp").unlink()

        # a constructor with no pair assignments at all
        empty = tdp / "src/__sinit_ov009_00001004.c"
        empty.write_text(
            "struct Pair { int a, b; };\n"
            "struct D2 { struct Pair lo, hi; };\n"
            "extern struct D2 data_ov009_00003020;\n"
            "void __sinit_ov009_00001004(void)\n"
            "{\n"
            "}\n")
        caught(lambda: build(tdp, "src/__sinit_ov009_00001004.c"),
               "no pair assignments",
               "constructor with no slot assignments")

        # a raw word store into a destination that is NOT a pair copy.
        # Skipping it is how a switch comes out short by a state and still
        # looks complete, so it is a refusal.
        rawstore = tdp / "src/__sinit_ov009_00001006.c"
        rawstore.write_text(base_sinit
                            .replace("__sinit_ov009_00001000",
                                     "__sinit_ov009_00001006")
                            .replace("    data_ov009_00003020.lo.a = "
                                     "tb ? ta : ta;\n",
                                     "    ((int *)&data_ov009_00003020)[7]"
                                     " = ta;\n"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001006.c"),
               "shape this parser does not know", "raw word store")

        # a LONE half of a word-index pair copy
        lone = tdp / "src/__sinit_ov009_00001007.c"
        lone.write_text(base_sinit
                        .replace("__sinit_ov009_00001000",
                                 "__sinit_ov009_00001007")
                        .replace("    ((int *)&data_ov009_00003020)[5] = "
                                 "((int *)&data_ov009_00002008)[1];\n", ""))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001007.c"),
               "second half", "lone half of a pair copy")

        # a CROSSED pair copy: the two halves come from different sources
        crossed = tdp / "src/__sinit_ov009_00001008.c"
        crossed.write_text(base_sinit
                           .replace("__sinit_ov009_00001000",
                                    "__sinit_ov009_00001008")
                           .replace("((int *)&data_ov009_00003020)[5] = "
                                    "((int *)&data_ov009_00002008)[1];",
                                    "((int *)&data_ov009_00003020)[5] = "
                                    "((int *)&data_ov009_00002000)[1];"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001008.c"),
               "not the second half", "crossed pair copy")

        # one slot written twice from two different sources
        twice = tdp / "src/__sinit_ov009_00001009.c"
        twice.write_text(base_sinit
                         .replace("__sinit_ov009_00001000",
                                  "__sinit_ov009_00001009")
                         .replace("    data_ov009_00003000.s1 = "
                                  "data_ov009_00002008;\n",
                                  "    data_ov009_00003000.s1 = "
                                  "data_ov009_00002008;\n"
                                  "    data_ov009_00003000.s1 = "
                                  "data_ov009_00002018;\n"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001009.c"),
               "twice, from", "one slot written from two sources")

        # a field the destination struct does not declare
        badfield = tdp / "src/__sinit_ov009_00001005.c"
        badfield.write_text(base_sinit
                            .replace("__sinit_ov009_00001000",
                                     "__sinit_ov009_00001005")
                            .replace("data_ov009_00003000.s1",
                                     "data_ov009_00003000.s9"))
        caught(lambda: build(tdp, "src/__sinit_ov009_00001005.c"),
               "has no such field", "assignment to an undeclared field")

        # ---- --out scoping, the review's probe set --------------------
        # The two ESCAPES first, because a component test passes both and
        # they are the reason this guard was rewritten. Each was proven
        # against the shipped tool: build/../X wrote the worktree root,
        # and the absolute shape reached outside the tree entirely.
        (tdp / OUT_ALLOWED).mkdir(exist_ok=True)
        other = pathlib.Path(td) / "other_checkout" / "build"
        other.mkdir(parents=True)
        for probe, why in (
                ("build/../X.inc", "dotdot back out of build/"),
                ("build/sub/../../X.inc", "dotdot through a subdirectory"),
                (str(other / ".." / "src" / "x.inc"),
                 "absolute path through ANOTHER checkout's build/.."),
                ("src/x.inc", "under src/"),
                ("port/hal/x.inc", "under port/"),
                ("../x.inc", "above the tree"),
                ("x.inc", "tree root, no build component at all")):
            caught(lambda p=probe: check_out(p, tdp), "outside",
                   "--out %s (%s)" % (probe, why))
        expect(check_out("build/x.inc", tdp)
               == (tdp / "build" / "x.inc").resolve(),
               "--out under build/ is allowed and comes back resolved",
               check_out("build/x.inc", tdp))
        expect(check_out("build/stategen/deep/x.inc", tdp)
               == (tdp / "build/stategen/deep/x.inc").resolve(),
               "--out deeper under build/ is allowed", "refused")
        expect(check_out(str(tdp / "build" / "abs.inc"), tdp)
               == (tdp / "build" / "abs.inc").resolve(),
               "an ABSOLUTE path genuinely inside this tree's build/ is "
               "allowed", "refused")

        # the demangler, both directions
        expect(demangle_method("_ZN6Player12St_Wait_MainEv")
               == ("Player", "St_Wait_Main"), "demangle", "no")
        expect(demangle_method("func_ov002_020e200c") is None,
               "a C name does not demangle", "it did")
        expect(demangle_method("_ZN6Player12St_Wait_Main") is None,
               "a mangle with no E terminator is refused, not guessed",
               demangle_method("_ZN6Player12St_Wait_Main"))
        expect(demangle_method("_ZN6Player12St_Wait_MainQv") is None,
               "a mangle with the wrong terminator is refused",
               demangle_method("_ZN6Player12St_Wait_MainQv"))
        expect(mangle_method("Player", "St_Wait_Main")
               == "_ZN6Player12St_Wait_MainEv", "mangle", "no")

        # the hand-file parsers, on fabricated rows in both shapes
        hm = tdp / "hand_mg.cpp"
        hm.write_text(
            "    case 0x020e1b54u: func_ov006_020e1b54(c); return 1;\n"
            "    case 0x020e1854u: curling_no_body(-1);    return 1;\n"
            "    case 0x020e2eb8u: func_ov006_020e2eb8();  return 1;\n"
            # a case that calls NOTHING. A parser that takes the first
            # word after the colon reads `break` as the target symbol and
            # invents a row; requiring the call paren is what stops it.
            "    case 0x020e0000u: break;\n")
        expect(parse_hand_mg(hm) == {0x020e1b54: "func_ov006_020e1b54",
                                     0x020e1854: "curling_no_body",
                                     0x020e2eb8: "func_ov006_020e2eb8"},
               "hand mg switch parse: a case that calls nothing yields no "
               "row", parse_hand_mg(hm))
        hp = tdp / "hand_pl.inc"
        hp.write_text(
            "    case 0x020d2404: return ((Player *)self)->"
            "Player::St_Wait_Main();\n"
            "    case 0x020e22c0: return _ZN6Player12St_Jump_InitEv"
            "((char *)self);\n"
            "    case 0x020e200c: func_ov002_020e200c((char *)self); "
            "return 1;\n"
            "    case 0x020cbd34: return port_player_st_climb_init"
            "((void *)self);\n")
        got, n = parse_hand_player(hp)
        expect(n == 4 and got == {
            0x020d2404: "_ZN6Player12St_Wait_MainEv",
            0x020e22c0: "_ZN6Player12St_Jump_InitEv",
            0x020e200c: "func_ov002_020e200c",
            0x020cbd34: "port_player_st_climb_init"},
            "hand player include parse: method, C-linkage mangle, plain "
            "call and host bridge", got)

    print("selftest %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


def _write(p, b):
    p.write_bytes(bytes(b))
    return p


def check_out(path, root):
    """--out must RESOLVE to somewhere inside this tree's build/.

    The first version of this guard split on "/" and asked whether the
    first component was "build", which two shapes walk straight through:
    `build/../X.inc` passes the component test and lands in the worktree
    root, and the absolute branch accepted any path with a "build"
    component ANYWHERE, so an absolute path through another checkout's
    build/.. reached that checkout's src/. A component test cannot answer
    this question; only resolving the path and testing containment can,
    so that is what this does. Symlinks are resolved too, for the same
    reason dotdot is.
    """
    target = pathlib.Path(path)
    if not target.is_absolute():
        target = pathlib.Path(root) / target
    allowed = (pathlib.Path(root) / OUT_ALLOWED).resolve()
    resolved = target.resolve()
    try:
        resolved.relative_to(allowed)
    except ValueError:
        die("--out %s resolves to %s, which is outside %s. This tool "
            "writes stdout or this tree's build/ and nothing else, "
            "because a lane commits what it has verified rather than what "
            "a generator produced. Note the check is on the RESOLVED "
            "path: build/../X and an absolute path through another "
            "checkout both fail here even though both contain a build "
            "component" % (path, resolved, allowed))
    return resolved


def default_root():
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


def main():
    ap = argparse.ArgumentParser(
        description="address-switch and host-copy skeleton generator for "
                    "mwcc pointer-to-member state machines; see the "
                    "docstring")
    ap.add_argument("--root", default=default_root())
    ap.add_argument("--sinit", help="src/__sinit_ovNNN_XXXXXXXX.c")
    ap.add_argument("--overlay", type=int,
                    help="overlay number, when the constructor's own "
                         "symbols do not name it")
    ap.add_argument("--arity", type=int,
                    help="dispatch arity for every table, when no typed "
                         "extern declares them (the State-object shape)")
    ap.add_argument("--class", dest="cls",
                    help="host class name; rows whose symbol demangles to "
                         "one of its methods emit the method-call shape")
    ap.add_argument("--emit-switch", action="store_true")
    ap.add_argument("--emit-host", action="store_true")
    ap.add_argument("--out", help="write to this path (must be under "
                                  "build/); default stdout")
    ap.add_argument("--reconstruct", action="store_true")
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        sys.exit(selftest())
    if args.reconstruct:
        sys.exit(cmd_reconstruct(args.root))
    if not args.sinit:
        sys.exit("pass --sinit PATH (with --emit-switch or --emit-host), "
                 "--reconstruct, or --selftest")

    m = build(args.root, args.sinit, args.overlay, args.arity)
    if args.emit_switch:
        text = emit_switch(m, cls=args.cls)
    elif args.emit_host:
        text = emit_host(m, cls=args.cls)
    else:
        text = emit_report(m)
    if args.out:
        p = check_out(args.out, args.root)
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8")
        print("stategen: wrote %s (%d lines)" % (p, text.count("\n")))
    else:
        sys.stdout.write(text)
    return 0


if __name__ == "__main__":
    main()
