#!/usr/bin/env python3
"""Member TYPES read out of the ROM: the destructor a destructor calls.

A class with an aggregate member of type `T` destroys it: the original source's
`~Owner()` ends with an implicit `mMember.~T()`, and mwccarm compiled that to

    add r0, r4, #0x124          ; r4 is `this`, 0x124 is the member's offset
    bl  _ZN10dBgW_KcMbgD1Ev     ; a RELOCATION, recorded in config/**/relocs.txt

The triple (owner, offset, member type) is therefore not inference.  The call target
is a relocation the ROM build already checks, the offset is an immediate frozen in the
instruction stream, and the owner is whichever function the call site sits inside.
This file harvests every such triple in the cartridge.

    python tools/dtor_members.py --report      # populations, then the true gap
    python tools/dtor_members.py --gap         # per-pair, against what headers declare
    python tools/dtor_members.py --adjacency   # test the two proposed adjacency rules
    python tools/dtor_members.py --apply       # type the decidable markers
    python tools/dtor_members.py --check       # fail unless the frozen census reproduces

Output: build/dtor_members.json.

Relation to what already existed
--------------------------------
`marker_census.py` asks the same question of `src/`: it greps decompiled destructor
BODIES for `_ZN9ModelAnimD1Ev(t + 0xd4)`.  That is the same evidence, but it can only
see the destructors this tree has already recovered as C.  This pass reads the
cartridge, so it also sees every destructor that is still `func_ov084_0212b344` or has
no source file at all.  `marker_census` reports 34 decidable; the cartridge proves 832
member pairs, of which 91 were decidable and untyped when this landed.

`opnew_sizes.py` recovers a class's EXTENT from the `operator new` literal and is blind
to what is inside it.  This recovers the INTERIOR and is blind to the extent.  Neither
sees the other's evidence; together they bracket a class.

How a site is read
------------------
For every `arm_call` relocation whose target address carries a `_ZN...D[012]Ev` symbol,
the enclosing function of the `from:` address is found from that module's own
`symbols.txt` (never a fixed window -- see opnew_sizes trap 3).  If the enclosing
function is itself a named destructor, a small `this`-relative abstract interpreter is
run over it to a fixed point and r0 at the call site is read out.  `this` is r0 on
entry; the lattice is (this + k), (sp + k), (imm k) or unknown, with stack slots tracked
so a spilled `this` survives and the literal pool read so an offset with no ARM
immediate encoding is still recovered.  Every one of the 2,426 sites yields an offset.

What the header side has to get right
-------------------------------------
The cartridge side of this file was right from the start; the HEADER side is what
lied.  The first census bucketed 181 pairs `NO FIELD AT OFFSET` -- read as "181 pad
splits waiting to be written" -- and 180 of them were this reader's own defects:

  144  the member is declared in a BASE.  mwccarm inlines a base destructor into the
       derived one, so `~ArmedRotatingPlatform` destroys dBgActor_c's `Model` at +0xd4
       itself, and reading only the derived class's own field list called that a hole.
       `declared_at` walks the chain; those now bucket `INHERITED` and are no work.
   26  a NAMED NESTED definition swallowed the rest of the class.  The reader closed a
       definition on a `}` in column zero and a nested `};` is indented, so the
       `struct State { ... };` these six open before their first real field took `cur`
       and never gave it back: Bullet, FlyGuy, HootTheOwl, Player, Snufit and Swoop
       each read as having ZERO declared fields.
    6  the offset comment carried prose AND the field is a `u8` ARRAY marker.  Now
       visible, now `DECIDABLE` -- and still held: `apply` refuses them because
       retyping `u8 mModel1[0x50]` rewrites every consumer that spells it as an array,
       and their owners' recovered C++ destructors call the member's D1 explicitly in
       an order Itanium auto-destruction cannot reproduce (their headers say so).
    4  the offset comment carried prose.  `Model mModel; /* 0x501c -- destroyed last */`
       did not match a reader that demanded `*/` right after the number.  Now it stops
       at the number, which is what `check_header_offsets.DECL` -- the gate this file
       must agree with -- always did.
    1  a real pad split: dScDSMT_c's dFdDummy_c at +0x54, inside `u8 unk_050[0x14]`.

`test_dtor_members.py` pins all of them.  The lesson generalises: a bucket named for what
the HEADERS say is only as good as the reader, and a census that is not cross-checked
against the gate's own parser will invent work.

Five things this file refuses to do
-----------------------------------
join on names     An earlier survey in this tree reported "127 classes with no header";
                  125 of them were RTTI ALIASES sharing one vtable address with a name
                  the tree does have a file for.  Attribution here is by ADDRESS -- the
                  reloc's `to:` address picks the member type, the `from:` address picks
                  the owner through the symbol table's own address ranges.  A name is
                  only ever used afterwards, to find a header, and then every spelling
                  at the class's vtable address is a candidate (`resolve_header_name`).
                  `test_dtor_members.py::test_alias_join_regression` pins this.

trust offset 0    `~Derived()` chains to its base at +0, which is an inheritance edge and
                  not a member.  Offset 0 is bucketed BASE-AT-0 and never applied.

trust D2 at +k    A D2 (base-object destructor) at a nonzero offset is a secondary base
                  under multiple inheritance, not an aggregate member.  Bucketed
                  SECONDARY-BASE.  `notes/collision-system.md` documents the family that
                  does this.  Only D1 at a nonzero offset is a member.

type without a    `check_header_offsets.py` sizes a member type from its
size assert       `T_size_must_be_0xN` typedef, and reports UNPARSED -- exit 1, not a
                  warning -- when there is none.  That is how PR #1659 failed CI.  A
                  proposal whose type carries no assert is bucketed NO ASSERT and held.

overrun the span  A member of size `sizeof(T)` may only be written where at least that
                  many bytes of marker + padding follow it.  Where the span is longer the
                  remainder stays as an explicit pad; where it is SHORTER `--apply`
                  refuses the finding rather than truncating it, because an evidenced
                  field is already declared inside the member and re-spelling it through
                  the member is a separate, per-consumer job.

Requires capstone and pyyaml, like `opnew_sizes.py` and `evidence_rom.py`.
Deterministic: every collection is sorted before it is emitted.
"""

from __future__ import annotations

import argparse
import bisect
import collections
import json
import os
import pathlib
import re
import sys
import textwrap

try:
    from capstone.arm import (
        ARM_CC_AL, ARM_CC_INVALID,
        ARM_INS_ADD, ARM_INS_B, ARM_INS_BL, ARM_INS_BLX, ARM_INS_BX,
        ARM_INS_LDM, ARM_INS_LDR, ARM_INS_MOV, ARM_INS_POP, ARM_INS_PUSH,
        ARM_INS_STM, ARM_INS_STR, ARM_INS_SUB,
        ARM_OP_IMM, ARM_OP_MEM, ARM_OP_REG, ARM_SFT_INVALID,
    )
except ImportError:                                                    # pragma: no cover
    sys.exit("capstone not installed. Run: pip install capstone")

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import opnew_sizes as O                                                # noqa: E402

SYM_FN = re.compile(
    r"^(\S+)\s+kind:function\((arm|thumb),size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)")
SYM_ANY = re.compile(r"^(\S+)\s+kind:\S+\s+addr:(0x[0-9a-fA-F]+)")
RELOC = re.compile(
    r"^from:(0x[0-9a-fA-F]+)\s+kind:(\S+)\s+to:(0x[0-9a-fA-F]+)\s+module:(\S+)")
DTOR_SYM = re.compile(r"^_ZN([0-9N].*?)D([012])Ev$")
LEN_PREFIX = re.compile(r"(\d+)")
SIZE_ASSERT = re.compile(r"(\w+)_size_must_be_(0x[0-9a-fA-F]+)")

# `u8 mFoo;   /* 0x124 */`  -- the original generator's "an object starts here" marker.
MARKER = re.compile(r"^(\s*)u8(\s+)(\w+)\s*;\s*/\*\s*(0x[0-9a-fA-F]+)\s*\*/\s*$")
PAD = re.compile(r"^(\s*)u8\s+(pad_[0-9a-fA-F]+)\s*\[\s*(0x[0-9a-fA-F]+|\d+)\s*\]\s*;\s*$")
# `dCcAc_c mdCcAc_c;   /* 0x110 */`, and also
# `Model mModel;        /* 0x501c -- 0x50, destroyed last, see banner */`.
# The offset comment is NOT required to end right after the offset.  It used to be,
# and every field whose comment carried a word of prose after the number was invisible
# -- which is most of the hand-written ones.  `check_header_offsets.DECL`, the gate
# this file has to agree with, has always stopped at the offset; now so does this.
ANY_FIELD = re.compile(r"^\s*([A-Za-z_]\w*)\s+(\w+)\s*(\[[^\]]*\])?\s*;\s*/\*\s*"
                       r"(0x[0-9a-fA-F]+)")
OPEN_DEF = re.compile(r"^[ \t]*(?:class|struct)[ \t]+(\w+)\b")
BASE_CLAUSE = re.compile(r"^[ \t]*(?:class|struct)[ \t]+\w+[ \t]*:([^{;]+)")
BASE_NOISE = re.compile(r"\b(?:public|protected|private|virtual)\b")
SKIPPABLE = re.compile(r"^\s*($|/\*|\*|//)")

# An offset above this is a literal-pool word that is really an ADDRESS folded into
# `this`, not a member.  `Stage` (0xa2b8) is the largest class in the cartridge.
MAX_OFFSET = 0x10000

# Frozen census.  Change these deliberately and inspect every delta: named evidence can
# legitimately increase as symbols land, while the unnamed bucket should decrease by
# the corresponding amount.  Exact equality catches losses and unexpected growth alike.
GATE = {
    "dtor_symbols": 805,
    "arm_call_relocs_to_a_destructor": 2715,
    "sites_inside_a_named_destructor": 2426,
    "sites_inside_an_unnamed_function": 289,
    "sites_cross_class": 2426,
    "sites_offset_recovered": 2426,
    "pairs_distinct": 1208,
    "pairs_member_evidence": 832,
    "pairs_base_at_0": 369,
    "pairs_secondary_base": 7,
    "owners_distinct": 374,
}


# --------------------------------------------------------------------------
# symbols and relocations
# --------------------------------------------------------------------------

def module_dirs(root):
    """(name, dir) for every module that has its own symbols.txt / relocs.txt."""
    out = [("arm9", root / "config" / "arm9")]
    for extra in ("itcm", "dtcm"):
        p = root / "config" / "arm9" / extra
        if p.is_dir():
            out.append((extra, p))
    for d in sorted((root / "config" / "arm9" / "overlays").glob("ov*")):
        out.append((d.name, d))
    return out


def load_functions(root, stats):
    """module -> sorted [(addr, size, name, mode)] over every function symbol."""
    out = {}
    for mod, d in module_dirs(root):
        p = d / "symbols.txt"
        if not p.is_file():
            continue
        fns = []
        for line in p.read_text(errors="replace").splitlines():
            m = SYM_FN.match(line)
            if m:
                fns.append((int(m.group(4), 16), int(m.group(3), 16),
                            m.group(1), m.group(2)))
        fns.sort()
        out[mod] = fns
        stats["function_symbols"] += len(fns)
    return out


def load_destructors(root, stats):
    """(module, addr) -> sorted [destructor symbol names at that address]."""
    out = collections.defaultdict(list)
    for mod, d in module_dirs(root):
        p = d / "symbols.txt"
        if not p.is_file():
            continue
        for line in p.read_text(errors="replace").splitlines():
            m = SYM_ANY.match(line)
            if m and DTOR_SYM.match(m.group(1)):
                out[(mod, int(m.group(2), 16))].append(m.group(1))
    for v in out.values():
        v.sort()
    stats["dtor_symbols"] = sum(len(v) for v in out.values())
    stats["dtor_addresses"] = len(out)
    stats["dtor_addresses_with_more_than_one_name"] = sum(
        1 for v in out.values() if len(v) > 1)
    return dict(out)


def target_modules(field):
    """`module:` names the module the target lives in, not the one recording it.

    Three spellings: `main`, `overlay(23)`, and `overlays(0,4)` for an address that
    two mutually exclusive overlays both cover.  The last is genuinely ambiguous and
    is counted, not silently collapsed.
    """
    if field == "main":
        return ["arm9"]
    m = re.fullmatch(r"overlays?\(([\d,]+)\)", field)
    if m:
        return ["ov%03d" % int(x) for x in m.group(1).split(",")]
    return [field]


def load_dtor_calls(root, dtors, stats):
    """Every `arm_call` relocation whose target address carries a destructor symbol."""
    sites = []
    for mod, d in module_dirs(root):
        p = d / "relocs.txt"
        if not p.is_file():
            continue
        for line in p.read_text(errors="replace").splitlines():
            m = RELOC.match(line)
            if not m:
                continue
            ta = int(m.group(3), 16)
            cands = [t for t in target_modules(m.group(4)) if (t, ta) in dtors]
            if not cands:
                continue
            if m.group(2) != "arm_call":
                stats["non_call_relocs_to_a_destructor"] += 1
                continue
            if len(cands) > 1:
                stats["target_module_ambiguous"] += 1
            sites.append((mod, int(m.group(1), 16), cands[0], ta))
    sites.sort()
    stats["arm_call_relocs_to_a_destructor"] = len(sites)
    return sites


# --------------------------------------------------------------------------
# names
# --------------------------------------------------------------------------

def demangle_dtor(sym):
    """`_ZN7daKrb_cD1Ev` -> ("daKrb_c", 1);  nested names join with `__`.

    Same convention as `evidence_rom.class_of` and `opnew_sizes.demangle_ztv`, which is
    how this tree names the headers for nested classes (`ActorBase__SceneNode.h`).
    """
    m = DTOR_SYM.match(sym or "")
    if not m:
        return None, None
    body, variant = m.group(1), int(m.group(2))
    if body.startswith("N"):
        body = body[1:]
    parts, i = [], 0
    while i < len(body):
        lm = LEN_PREFIX.match(body, i)
        if not lm:
            return None, None
        n = int(lm.group(1))
        i = lm.end()
        if n == 0 or i + n > len(body):
            return None, None
        parts.append(body[i:i + n])
        i += n
    return ("__".join(parts), variant) if parts else (None, None)


# --------------------------------------------------------------------------
# a `this`-relative abstract interpreter
# --------------------------------------------------------------------------
#
# Values are ("this", k), ("sp", k), ("imm", k) or absent (unknown).  Entry state is
# r0 = this+0, sp = sp+0.  Everything is a fixed point over the function's own
# instructions, so a loop or a forward branch cannot make a stale value survive into a
# join.
#
# ("imm", k) is not decoration.  An offset above 0xfff has no ARM rotated-8-bit
# encoding, so mwccarm spills it to the function's own literal pool and emits
# `ldr r1, [pc, #0x48]; add r0, r4, r1`.  `Stage::~Stage` does that for both its
# members.  Without pool reads, every member past the immediate range is invisible --
# which is the opposite of the bias you want, since those are the big classes.

def _merge(a, b):
    if a is None:
        return dict(b)
    return {k: v for k, v in a.items() if b.get(k) == v}


def _regs_written(ins):
    out = []
    if ins.id in (ARM_INS_POP, ARM_INS_LDM):
        return [o.reg for o in ins.operands if o.type == ARM_OP_REG]
    if ins.id in (ARM_INS_PUSH, ARM_INS_STM, ARM_INS_STR, ARM_INS_B,
                  ARM_INS_BX, ARM_INS_BL, ARM_INS_BLX):
        return []
    if ins.operands and ins.operands[0].type == ARM_OP_REG:
        out.append(ins.operands[0].reg)
    return out


def _rn(ins, reg):
    return ins.reg_name(reg)


class ThisFlow:
    """Reads r0 at every call site of `fn`, as an offset from `this`."""

    def __init__(self, img, mod, start, size):
        self.img, self.mod = img, mod
        self.ins = img.disasm(mod, start, size)
        self.start, self.size = start, size
        self.index = {i.address: n for n, i in enumerate(self.ins)}

    def _succ(self, n):
        ins = self.ins[n]
        nxt = [n + 1] if n + 1 < len(self.ins) else []
        if ins.id == ARM_INS_B:
            tgt = ins.operands[0].imm if ins.operands and \
                ins.operands[0].type == ARM_OP_IMM else None
            j = self.index.get(tgt)
            uncond = ins.cc in (ARM_CC_AL, ARM_CC_INVALID)
            out = ([j] if j is not None else [])
            return out if uncond else out + nxt
        if ins.id == ARM_INS_BX and ins.cc in (ARM_CC_AL, ARM_CC_INVALID):
            return []
        if ins.id in (ARM_INS_POP, ARM_INS_LDM) and "pc" in ins.op_str \
                and ins.cc in (ARM_CC_AL, ARM_CC_INVALID):
            return []
        return nxt

    def _transfer(self, st, ins):
        """Registers and stack after `ins`, given `st` before it."""
        regs, stk = dict(st.get("r", {})), dict(st.get("s", {}))
        cond = ins.cc not in (ARM_CC_AL, ARM_CC_INVALID)
        ops = ins.operands

        def rv(reg):
            return regs.get(_rn(ins, reg))

        new = dict(regs)
        if ins.id in (ARM_INS_PUSH, ARM_INS_STM) and ins.id == ARM_INS_PUSH:
            sp = regs.get("sp")
            if sp and sp[0] == "sp":
                new["sp"] = ("sp", sp[1] - 4 * len(ops))
        elif ins.id in (ARM_INS_POP, ARM_INS_LDM) and ins.id == ARM_INS_POP:
            for o in ops:
                if o.type == ARM_OP_REG:
                    new.pop(_rn(ins, o.reg), None)
            sp = regs.get("sp")
            if sp and sp[0] == "sp":
                new["sp"] = ("sp", sp[1] + 4 * len(ops))
        elif ins.id == ARM_INS_MOV and len(ops) == 2 and \
                ops[0].type == ARM_OP_REG and ops[1].shift.type == ARM_SFT_INVALID:
            d = _rn(ins, ops[0].reg)
            if ops[1].type == ARM_OP_REG:
                new[d] = rv(ops[1].reg) or None
            elif ops[1].type == ARM_OP_IMM:
                new[d] = ("imm", ops[1].imm)
            else:
                new[d] = None
        elif ins.id in (ARM_INS_ADD, ARM_INS_SUB) and len(ops) == 3 and \
                ops[0].type == ARM_OP_REG and ops[1].type == ARM_OP_REG and \
                ops[2].shift.type == ARM_SFT_INVALID:
            a = rv(ops[1].reg)
            b = (("imm", ops[2].imm) if ops[2].type == ARM_OP_IMM
                 else rv(ops[2].reg) if ops[2].type == ARM_OP_REG else None)
            d = _rn(ins, ops[0].reg)
            sign = 1 if ins.id == ARM_INS_ADD else -1
            if a and b and b[0] == "imm":
                new[d] = (a[0], a[1] + sign * b[1])
            elif a and a[0] == "imm" and b and sign > 0:
                new[d] = (b[0], b[1] + a[1])
            else:
                new[d] = None
        elif ins.id == ARM_INS_LDR and len(ops) == 2 and \
                ops[1].type == ARM_OP_MEM and ops[1].mem.base and \
                _rn(ins, ops[1].mem.base) == "pc" and ops[1].mem.index == 0:
            # the function's own literal pool: an offset with no ARM immediate encoding
            w = self.img.word(self.mod, ins.address + 8 + ops[1].mem.disp)
            new[_rn(ins, ops[0].reg)] = None if w is None else ("imm", w)
        elif ins.id == ARM_INS_STR and len(ops) == 2 and \
                ops[0].type == ARM_OP_REG and ops[1].type == ARM_OP_MEM:
            base = regs.get(_rn(ins, ops[1].mem.base)) if ops[1].mem.base else None
            if base and base[0] == "sp" and ops[1].mem.index == 0:
                slot = base[1] + ops[1].mem.disp
                if cond:
                    stk.pop(slot, None)
                else:
                    v = rv(ops[0].reg)
                    stk[slot] = v if v else None
        elif ins.id == ARM_INS_LDR and len(ops) == 2 and \
                ops[0].type == ARM_OP_REG and ops[1].type == ARM_OP_MEM:
            base = regs.get(_rn(ins, ops[1].mem.base)) if ops[1].mem.base else None
            d = _rn(ins, ops[0].reg)
            new[d] = (stk.get(base[1] + ops[1].mem.disp)
                      if base and base[0] == "sp" and ops[1].mem.index == 0 else None)
        else:
            for r in _regs_written(ins):
                new[_rn(ins, r)] = None

        if ins.id in (ARM_INS_BL, ARM_INS_BLX):
            for r in ("r0", "r1", "r2", "r3", "r12", "ip", "lr"):
                new[r] = None

        if cond and ins.id not in (ARM_INS_B, ARM_INS_BL, ARM_INS_BLX):
            # predicated: the write may not have happened.  Keep only agreement.
            new = {k: v for k, v in new.items() if regs.get(k) == v}

        return {"r": {k: v for k, v in new.items() if v is not None},
                "s": {k: v for k, v in stk.items() if v is not None}}

    def run(self):
        """address -> ("this", k) | ("sp", k) | None, for r0 at each instruction."""
        if not self.ins:
            return {}
        entry = {"r": {"r0": ("this", 0), "sp": ("sp", 0)}, "s": {}}
        state = [None] * len(self.ins)
        state[0] = entry
        work, guard = [0], 0
        while work and guard < 40000:
            guard += 1
            n = work.pop()
            st = state[n]
            if st is None:
                continue
            out = self._transfer(st, self.ins[n])
            for s in self._succ(n):
                merged = {"r": _merge(None if state[s] is None else state[s]["r"],
                                      out["r"]),
                          "s": _merge(None if state[s] is None else state[s]["s"],
                                      out["s"])}
                if state[s] is None or merged != state[s]:
                    state[s] = merged
                    work.append(s)
        return {i.address: (state[n]["r"].get("r0") if state[n] else None)
                for n, i in enumerate(self.ins)}


# --------------------------------------------------------------------------
# harvest
# --------------------------------------------------------------------------

def enclosing(fns, mod, addr):
    lst = fns.get(mod, ())
    i = bisect.bisect_right(lst, (addr, 1 << 62, "ï¿¿", "ï¿¿")) - 1
    if i < 0:
        return None
    a, s, n, m = lst[i]
    return (a, s, n, m) if a <= addr < a + s else None


def harvest(root, ext):
    stats = collections.Counter()
    mods = O.load_modules(ext, stats)
    fns = load_functions(root, stats)
    dtors = load_destructors(root, stats)
    sites = load_dtor_calls(root, dtors, stats)
    img = O.Image(mods, {m: [(a, s, n) for a, s, n, _ in v] for m, v in fns.items()})

    flows, rows = {}, []
    for mod, fa, tmod, ta in sites:
        e = enclosing(fns, mod, fa)
        if e is None:
            stats["sites_with_no_enclosing_function"] += 1
            continue
        owner_sym = e[2]
        owner, ovar = demangle_dtor(owner_sym)
        if owner is None:
            stats["sites_inside_an_unnamed_function"] += 1
            stats["unnamed_callers"] += 0
            rows.append({"module": mod, "site": "0x%08x" % fa, "owner": None,
                         "owner_symbol": owner_sym, "owner_variant": None,
                         "member": demangle_dtor(dtors[(tmod, ta)][0])[0],
                         "member_symbol": dtors[(tmod, ta)][0],
                         "member_variant": demangle_dtor(dtors[(tmod, ta)][0])[1],
                         "offset": None, "verdict": "OWNER NOT A NAMED DESTRUCTOR"})
            continue
        stats["sites_inside_a_named_destructor"] += 1
        if mod not in mods:
            stats["sites_in_a_module_with_no_image"] += 1
            continue
        key = (mod, e[0])
        if key not in flows:
            flows[key] = ThisFlow(img, mod, e[0], e[1]).run()
        v = flows[key].get(fa)
        off = v[1] if v and v[0] == "this" else None
        if off is not None and not 0 <= off < MAX_OFFSET:
            # a pool word that is really an address, folded into `this` by an `add`.
            # The largest class the cartridge news is under 0x4000 bytes.
            stats["sites_offset_implausible"] += 1
            off = None
        if off is None:
            stats["sites_offset_not_recovered"] += 1
        else:
            stats["sites_offset_recovered"] += 1

        member_sym = dtors[(tmod, ta)][0]
        member, mvar = demangle_dtor(member_sym)
        if member == owner:
            stats["sites_same_class_chain"] += 1
            verdict = "SAME CLASS"
        else:
            stats["sites_cross_class"] += 1
            if off is None:
                verdict = "OFFSET NOT RECOVERED"
            elif off == 0:
                verdict = "BASE-AT-0"
            elif mvar == 2:
                verdict = "SECONDARY-BASE"
            else:
                verdict = "MEMBER"
        rows.append({
            "module": mod, "site": "0x%08x" % fa, "owner": owner,
            "owner_symbol": owner_sym, "owner_variant": ovar,
            "member": member, "member_symbol": member_sym, "member_variant": mvar,
            "member_module": tmod, "member_addr": "0x%08x" % ta,
            "offset": None if off is None else "0x%03x" % off, "verdict": verdict,
        })

    # (owner, offset, member) -- the pair the ROM proves.  Sites agreeing is the norm:
    # D0 and D1 of one class both destroy the same members.
    pairs = collections.defaultdict(list)
    for r in rows:
        if r["owner"] and r["offset"] is not None and r["verdict"] in (
                "MEMBER", "BASE-AT-0", "SECONDARY-BASE"):
            pairs[(r["owner"], r["offset"], r["member"])].append(r)
    conflicts = collections.defaultdict(set)
    for (own, off, mem) in pairs:
        conflicts[(own, off)].add(mem)

    out = []
    for (own, off, mem), rs in sorted(pairs.items()):
        vs = sorted({r["verdict"] for r in rs})
        out.append({
            "owner": own, "offset": off, "member": mem,
            "member_symbol": rs[0]["member_symbol"],
            "verdict": vs[0] if len(vs) == 1 else "AMBIGUOUS VARIANT",
            "evidence": sorted({"D%d" % r["owner_variant"] for r in rs}),
            "member_variants": sorted({"D%d" % r["member_variant"] for r in rs}),
            "sites": [r["site"] for r in rs],
            "modules": sorted({r["module"] for r in rs}),
            "conflicting_types": sorted(conflicts[(own, off)] - {mem}) or None,
        })
    stats["pairs_distinct"] = len(out)
    stats["pairs_member_evidence"] = sum(1 for p in out if p["verdict"] == "MEMBER")
    stats["pairs_base_at_0"] = sum(1 for p in out if p["verdict"] == "BASE-AT-0")
    stats["pairs_secondary_base"] = sum(1 for p in out if p["verdict"] == "SECONDARY-BASE")
    stats["pairs_owner_offset_with_two_types"] = sum(
        1 for k, v in conflicts.items() if len(v) > 1)
    stats["owners_distinct"] = len({p["owner"] for p in out})

    return {"meta": {"stats": dict(sorted(stats.items()))},
            "pairs": out, "sites": rows}


# --------------------------------------------------------------------------
# the gap against the headers
# --------------------------------------------------------------------------

def header_index(root):
    """class -> {"header", "assert", "fields": {offset: [(type, name)]}, "bases": [...]}.

    A header in this tree routinely defines the SAME class twice -- a real C++ layout
    under `#ifdef __cplusplus` and a flat `u8`-array stand-in for C translation units
    below it.  `Wiggler` types +0x708 as `dBgCh_Actr` in the first and as
    `u8 mWithMeshClsn[0x1bc]` in the second.  Keeping only one definition per class
    made 15 already-typed members read as untyped work, purely on which block came
    last in the file, so every block's declaration is kept and a member counts as
    typed when ANY of them types it.

    Base classes are recorded too.  A member declared in a BASE is not missing from
    the derived class -- mwccarm inlines the base destructor into the derived one, so
    the cartridge shows `~Bullet` destroying dEnemyBase_c's members at their own
    offsets, and reading only the derived class's own field list called every one of
    them an untyped hole.  144 of the 181 findings in the first census were that.
    """
    out = {}

    def entry(name, h):
        return out.setdefault(name, {"header": h, "assert": None, "fields": {},
                                     "bases": []})

    for h in sorted((root / "include").rglob("*.h")):
        txt = h.read_text(errors="replace")
        for m in O.DEFINITION.finditer(txt):
            entry(m.group(1), h)
        for m in SIZE_ASSERT.finditer(txt):
            e = entry(m.group(1), h)
            if e["assert"] is None:
                e["assert"] = int(m.group(2), 16)
                e["header"] = h
    # fields, per definition block, so a header holding two structs is not merged.
    #
    # Brace depth, not "a `}` in column zero".  A NESTED definition -- the
    # `struct State { ... };` five enemy headers open before their first real field --
    # matched OPEN_DEF just as well as the outer one and took `cur` with it, and its
    # own `};` is indented so nothing ever gave `cur` back.  Every field below the
    # nested block was then dropped: Bullet, FlyGuy, HootTheOwl, Snufit and Swoop each
    # read as having ZERO declared fields, and all 20 of their already-typed members
    # were reported as work.  An ANONYMOUS `union {` is not a definition and its
    # members are the enclosing class's own, so only a NAMED nested one suppresses.
    for h in sorted((root / "include").rglob("*.h")):
        cur = cur_depth = nested_from = None
        depth = 0
        for line in h.read_text(errors="replace").splitlines():
            m = OPEN_DEF.match(line)
            is_def = bool(m) and not line.rstrip().endswith(";")
            if cur is None:
                if is_def:
                    cur, cur_depth = m.group(1), depth
                    b = BASE_CLAUSE.match(line)
                    if b and out.get(cur, {}).get("header") == h:
                        for x in BASE_NOISE.sub(" ", b.group(1)).split(","):
                            x = x.strip()
                            if re.fullmatch(r"\w+", x) and x not in out[cur]["bases"]:
                                out[cur]["bases"].append(x)
            elif is_def:
                if nested_from is None:
                    nested_from = depth
            elif nested_from is None and out.get(cur, {}).get("header") == h:
                f = ANY_FIELD.match(line)
                if f:
                    at = out[cur]["fields"].setdefault(int(f.group(4), 16), [])
                    d = (f.group(1), f.group(2) + (f.group(3) or ""))
                    if d not in at:
                        at.append(d)
            depth += line.count("{") - line.count("}")
            if nested_from is not None and depth <= nested_from:
                nested_from = None
            if cur is not None and depth <= cur_depth:
                cur = cur_depth = None
    return out


def declared_at(hdr, cls, off):
    """(own declarations, inherited declarations) at `off` for `cls`.

    Walks the base chain header_index recorded.  Cycle-guarded: a header pair that
    names each other as bases would otherwise recurse forever.
    """
    own = list(hdr.get(cls, {}).get("fields", {}).get(off, ()))
    inh, seen, stack = [], {cls}, list(hdr.get(cls, {}).get("bases", ()))
    while stack:
        b = stack.pop(0)
        if b in seen or b not in hdr:
            continue
        seen.add(b)
        for d in hdr[b]["fields"].get(off, ()):
            if d not in inh:
                inh.append(d)
        stack += hdr[b]["bases"]
    return own, inh


INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]+"([^"]+)"', re.M)


def _resolve(root, name):
    p = root / "include" / name
    return p if p.is_file() else None


def c_reachable(root):
    """Every header a `.c` translation unit can reach, transitively.

    A C TU cannot use a type whose header defines it only under `__cplusplus`, and
    `dBgW_KcMbg`, `TextureSequence`, `TextureTransformer` and `MaterialChanger` are all
    exactly that.  Typing a member as one of them inside a header that some `.c` file
    includes fails the build with `undefined identifier`, then `illegal constant
    expression` from the size assert -- which is what a first run of this pass did to
    eleven files.  Both halves of the test matter: the OWNER's reach and the MEMBER's
    C visibility.
    """
    graph = {}
    for h in sorted((root / "include").rglob("*.h")):
        graph[h] = [x for x in
                    (_resolve(root, n) for n in INCLUDE.findall(h.read_text(errors="replace")))
                    if x]
    seen, stack = set(), []
    for c in sorted((root / "src").rglob("*.c")):
        stack += [x for x in
                  (_resolve(root, n) for n in INCLUDE.findall(c.read_text(errors="replace")))
                  if x]
    while stack:
        n = stack.pop()
        if n in seen:
            continue
        seen.add(n)
        stack += graph.get(n, ())
    return seen


CPP_ONLY_OPEN = re.compile(r"^\s*#\s*if(?:def)?\s+.*__cplusplus")
C_TYPEDEF = re.compile(r"^\s*typedef\s+(?:struct\s+)?\w+\s+(\w+)\s*;")


def c_usable_types(root):
    """Class names a C translation unit can spell as a BARE name.

    Two separate things have to be true and only the first is obvious.  The type needs
    a definition outside the `#ifdef __cplusplus` branch -- and it also needs
    `typedef struct X X;`, because in C the tag alone is not a type name.  Only
    `Model`, `ModelAnim` and `BlendModelAnim` carry that typedef today; `ShadowModel`,
    `dBgCh_Actr`, `dCcAcPos_c` and `dCcAc_c` have flat C stand-ins and no typedef, and
    writing `dBgCh_Actr mWithMeshClsn;` into a C-reachable header gets
    `undefined identifier` from the tag, then `illegal constant expression` from the
    size assert.  Adding those typedefs is the cheapest next lever this pass has.

    A one-level `#ifdef __cplusplus` scan, which is the only shape this tree uses.
    """
    defined, typedefed = set(), set()
    for h in sorted((root / "include").rglob("*.h")):
        depth, skip_from = 0, None
        for line in h.read_text(errors="replace").splitlines():
            s = line.strip()
            if s.startswith("#if"):
                depth += 1
                if skip_from is None and CPP_ONLY_OPEN.match(line):
                    skip_from = depth
                continue
            if s.startswith("#else") and skip_from == depth:
                skip_from = None
                continue
            if s.startswith("#endif"):
                if skip_from == depth:
                    skip_from = None
                depth -= 1
                continue
            if skip_from is not None:
                continue
            m = OPEN_DEF.match(line)
            if m and not s.endswith(";"):
                defined.add(m.group(1))
            t = C_TYPEDEF.match(line)
            if t:
                typedefed.add(t.group(1))
    return defined & typedefed


def vtable_aliases(root, mods, stats):
    """class name -> every other spelling the tree gives the class at its vtable address.

    THE join defect this file refuses to repeat.  An earlier survey reported "127
    classes with no header" and 125 were aliases: `_ZTV12FortressWall`,
    `_ZTV9MontyMole` and `_ZTV11daChoropu_c` are one address and only one has a file.
    """
    by_addr = O.load_vtable_symbols(root, mods, stats)
    out = collections.defaultdict(set)
    for (_mod, _addr), syms in by_addr.items():
        names = sorted({n for n in (O.demangle_ztv(s) for s in syms) if n})
        for n in names:
            out[n].update(x for x in names if x != n)
    return {k: sorted(v) for k, v in out.items()}


def gap(root, doc, mods, stats):
    hdr = header_index(root)
    aliases = vtable_aliases(root, mods, stats)
    sizes = {k: v["assert"] for k, v in hdr.items() if v["assert"] is not None}
    from_c, in_c = c_reachable(root), c_usable_types(root)

    buckets = collections.defaultdict(list)
    for p in doc["pairs"]:
        if p["verdict"] != "MEMBER":
            buckets[p["verdict"]].append(p)
            continue
        own, how = O.resolve_header_name(p["owner"], aliases.get(p["owner"], ()), hdr)
        mem, _ = O.resolve_header_name(p["member"], aliases.get(p["member"], ()), hdr)
        rec = dict(p, owner_header=own, owner_join=how, member_header=mem)
        off = int(p["offset"], 16)
        if own is None:
            buckets["NO OWNER HEADER"].append(rec)
            continue
        rec["header_path"] = str(hdr[own]["header"].relative_to(root)).replace("\\", "/")
        cur, inh = declared_at(hdr, own, off)
        cur = cur or None
        rec["declared"] = None if cur is None else "; ".join("%s %s" % d for d in cur)
        rec["inherited"] = "; ".join("%s %s" % d for d in inh) or None
        if cur and any(t == mem for t, _ in cur):
            buckets["ALREADY TYPED"].append(rec)
        elif not cur and inh and any(t == mem for t, _ in inh):
            # the base owns it; the derived destructor destroys it because mwccarm
            # inlined the base destructor, not because the class repeats the member.
            buckets["INHERITED"].append(rec)
        elif mem is None or mem not in sizes:
            buckets["NO ASSERT"].append(rec)
        elif cur and any(t != "u8" for t, _ in cur):
            buckets["DECLARED OTHERWISE"].append(rec)
        elif not cur and inh:
            # a base declares SOMETHING ELSE here.  Either the base is wrong or the
            # derived class is; the ROM says a `mem` starts at this offset.
            buckets["INHERITED OTHERWISE"].append(rec)
        elif cur is None:
            buckets["NO FIELD AT OFFSET"].append(rec)
        elif hdr[own]["header"] in from_c and mem not in in_c:
            buckets["MEMBER TYPE IS C++ ONLY"].append(rec)
        else:
            rec["member_size"] = sizes[mem]
            buckets["DECIDABLE"].append(rec)
    return buckets, hdr, aliases, sizes


BUCKETS = ("DECIDABLE", "ALREADY TYPED", "INHERITED", "NO ASSERT",
           "DECLARED OTHERWISE", "INHERITED OTHERWISE", "NO FIELD AT OFFSET",
           "MEMBER TYPE IS C++ ONLY", "NO OWNER HEADER",
           "BASE-AT-0", "SECONDARY-BASE", "AMBIGUOUS VARIANT")


# --------------------------------------------------------------------------
# adjacency rules
# --------------------------------------------------------------------------

ADJACENCY = {"dBgW_KcMbg": ("Matrix4x3", 0x30), "ShadowModel": ("Matrix4x3", 0x30)}


def adjacency(doc, hdr, sizes, aliases):
    """Does `T` always sit immediately before its own `Matrix4x3`?

    `Matrix4x3` is `struct { s32 m[12]; }` (include/common.h) -- a POD with no
    destructor, so no destructor relocation can ever NAME one.  The rule is therefore
    not directly testable and only SPACE for it is: what the cartridge does prove is
    the offset of the NEXT member, and 0x30 bytes either fit between the two or they
    do not.  Per owner the verdict is

      REFUTED     the next proven member starts before off + sizeof(T) + 0x30
      CONFIRMED   the owner's header already declares a Matrix4x3 at off + sizeof(T)
      CONSISTENT  exactly 0x30 of room to the next proven member
      ROOM        more than 0x30 -- consistent, but so is anything else
      TAIL        no later proven member; the extent is opnew_sizes' question

    A single REFUTED is decisive and a thousand CONSISTENTs are not, which is why the
    counter-examples are printed and the agreements are only counted.
    """
    at = collections.defaultdict(dict)
    for p in doc["pairs"]:
        if p["verdict"] in ("MEMBER", "SECONDARY-BASE"):
            at[p["owner"]][int(p["offset"], 16)] = p["member"]
    res = collections.defaultdict(collections.Counter)
    detail = collections.defaultdict(list)
    for typ, (want, wsz) in sorted(ADJACENCY.items()):
        sz = sizes.get(typ)
        for owner, fields in sorted(at.items()):
            for off, m in sorted(fields.items()):
                if m != typ:
                    continue
                if sz is None:
                    res[typ]["NO SIZE FOR " + typ] += 1
                    continue
                hn, _ = O.resolve_header_name(owner, aliases.get(owner, ()), hdr)
                decl = hdr[hn]["fields"].get(off + sz) if hn else None
                later = sorted(o for o in fields if o > off)
                room = (later[0] - off) if later else None
                if decl and any(t == want for t, _ in decl):
                    v, note = "CONFIRMED", "the header declares a %s there" % want
                elif room is None:
                    v, note = "TAIL", "no later proven member"
                elif room < sz + wsz:
                    v, note = "REFUTED", "next proven member %s at +0x%03x, only 0x%x of room" % (
                        fields[later[0]], later[0], room - sz)
                elif room == sz + wsz:
                    v, note = "CONSISTENT", "exactly 0x%x to %s" % (wsz, fields[later[0]])
                else:
                    v, note = "ROOM", "0x%x to %s" % (room - sz, fields[later[0]])
                res[typ][v] += 1
                detail[typ].append((owner, off, v, note))
    return res, detail


# --------------------------------------------------------------------------
# apply
# --------------------------------------------------------------------------

def unconditional_includes(lines, before):
    """(includes visible on BOTH language paths above `before`, where to add another).

    "The string appears earlier in the file" is not visibility.  A header's C++ branch
    carries its own `#include "Model.h"`, textually above the flat C definition below
    the `#else` -- and invisible to it.  Three headers were typed against an include
    that could not be reached, and failed with `undefined identifier 'Model'`.  Only
    an include outside every `__cplusplus` conditional counts, and a new one is
    anchored after the last of those.
    """
    seen, anchor, stack = [], None, []
    for n, line in enumerate(lines[:before]):
        s = line.strip()
        if s.startswith("#if"):
            stack.append(bool(CPP_ONLY_OPEN.match(line)))
            continue
        if s.startswith("#endif"):
            if stack:
                stack.pop()
            continue
        if s.startswith("#else") or s.startswith("#elif"):
            continue
        if any(stack):
            continue
        m = INCLUDE.match(line)
        if m:
            seen.append(m.group(1))
            anchor = n
    return seen, anchor


def plan(root, buckets, hdr, sizes, only=None):
    """Per header, the edits a DECIDABLE finding implies -- or why it is held back."""
    picked, held = [], collections.Counter()
    for rec in buckets["DECIDABLE"]:
        name = "%s.%s" % (rec["owner_header"], rec["offset"])
        if only is not None and rec["owner_header"] not in only and name not in only:
            held["not in --only"] += 1
            continue
        if rec["conflicting_types"]:
            held["two types claimed at one offset"] += 1
            continue
        picked.append(rec)
    return picked, held


def apply(root, picked, sizes, write=False):
    """Replace `u8 marker; /* 0xNN */` + its pad with the real member type.

    The pad the marker stood on is the rest of the object.  Where it is longer than
    `sizeof(T)` the remainder stays as a pad at the new offset -- nothing declared is
    ever deleted.  Where it is shorter the finding is refused here, not silently
    truncated, because a field is already declared inside the member.
    """
    by_file = collections.defaultdict(list)
    for rec in picked:
        by_file[rec["header_path"]].append(rec)

    done, refused = [], []
    for path, recs in sorted(by_file.items()):
        p = root / path
        lines = p.read_text(errors="replace").splitlines()
        edits = []
        for rec in recs:
            off = int(rec["offset"], 16)
            # EVERY block, not the first.  A header routinely defines the class twice --
            # a C++ layout and a flat C stand-in -- and typing only one of them leaves
            # the two disagreeing about what lives at the offset.
            hits = [n for n, line in enumerate(lines)
                    if MARKER.match(line) and int(MARKER.match(line).group(4), 16) == off]
            if not hits:
                # Diagnose, don't just decline.  Since header_index learned to read an
                # offset comment that carries prose, six findings arrive here declared
                # as `u8 mModel1[0x50]; /* 0x4f38 -- Model, raw bytes */`.  There IS a
                # marker; it is an ARRAY, and "no marker" sent the last reader looking
                # in the wrong place.
                arr = [line.strip() for line in lines
                       if ANY_FIELD.match(line)
                       and int(ANY_FIELD.match(line).group(4), 16) == off
                       and ANY_FIELD.match(line).group(1) == "u8"
                       and ANY_FIELD.match(line).group(3)]
                refused.append((rec, ("declared as a u8 ARRAY (%s): retyping it also "
                                      "rewrites every consumer that spells it as an "
                                      "array, so it is not a header-only edit"
                                      % arr[0]) if arr else
                                "no u8 marker at that offset"))
                continue
            need = sizes[rec["member"]]
            ok = []
            for i in hits:
                j, in_comment = i + 1, False
                while j < len(lines):
                    ln = lines[j]
                    if in_comment:
                        in_comment = "*/" not in ln
                        j += 1
                        continue
                    if SKIPPABLE.match(ln):
                        in_comment = "/*" in ln and "*/" not in ln
                        j += 1
                        continue
                    break
                pm = PAD.match(lines[j]) if j < len(lines) else None
                span = 1 + int(pm.group(3), 0) if pm else 1
                if span < need:
                    refused.append((rec, "span 0x%x < sizeof(%s) 0x%x"
                                    % (span, rec["member"], need)))
                    ok = []
                    break
                ok.append((i, j, rec, span, need, pm))
            edits.extend(ok)
        # The include has to land ABOVE the definition it feeds, and `#include` lines
        # also appear inside the `#ifdef __cplusplus` branch and below the struct.
        # Anchoring on the file's LAST one put eleven of them after the use.
        first = min((e[0] for e in edits), default=len(lines))
        above, anchor = unconditional_includes(lines, first)
        for i, j, rec, span, need, pm in sorted(edits, reverse=True):
            m = MARKER.match(lines[i])
            indent, gap, field = m.group(1), m.group(2), m.group(3)
            typ = rec["member"]
            gap = " " * max(1, len("u8") + len(gap) - len(typ))
            body = ("%s member. The cartridge's own ~%s calls %s at +%s (%s), a "
                    "relocation the ROM build checks; recovered by "
                    "tools/dtor_members.py. D1 and not D2, so it is this type and "
                    "not an inlined base."
                    % (typ, rec["owner"], rec["member_symbol"], rec["offset"],
                       "/".join(rec["evidence"])))
            wrapped = textwrap.wrap(body, width=88 - len(indent) - 3) or [body]
            new = ["%s/* %s" % (indent, wrapped[0])]
            new += ["%s   %s" % (indent, w) for w in wrapped[1:-1]]
            if len(wrapped) > 1:
                new.append("%s   %s */" % (indent, wrapped[-1]))
            else:
                new[0] += " */"
            new.append("%s%s%s%s;            /* %s */"
                       % (indent, typ, gap, field, rec["offset"]))
            tail = span - need
            if pm is not None:
                del lines[j]
            if tail > 0:
                new.append("%su8  pad_%03x[0x%x];"
                           % (pm.group(1) if pm else indent, int(rec["offset"], 16) + need,
                              tail))
            lines[i:i + 1] = new
            done.append(rec)
        # Only an include ABOVE the first edit is visible to it.  A header's C++ branch
        # carries its own `#include` list, so "the string appears somewhere in the file"
        # said Model.h was already included when it was included 40 lines BELOW the
        # flat C definition that now needed it -- three headers failed exactly so.
        want = []
        for rec in recs:
            inc = '#include "%s.h"' % rec["member"]
            if rec["member"] + ".h" not in above and inc not in want and rec in done:
                want.append(inc)
        if want:
            if anchor is None:
                refused.append((recs[0], "no #include above the definition to anchor %s"
                                % want))
            else:
                lines[anchor + 1:anchor + 1] = want
        if write:
            p.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    return done, refused


# --------------------------------------------------------------------------

def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--root", default=".", help="repo/worktree root")
    ap.add_argument("--extracted", default=None)
    ap.add_argument("-o", "--out", default=None,
                    help="default build/dtor_members.json under --root")
    ap.add_argument("--report", action="store_true", help="print populations first")
    ap.add_argument("--gap", action="store_true",
                    help="bucket every pair against what the headers declare")
    ap.add_argument("--adjacency", action="store_true",
                    help="test the proposed `T is always followed by its Matrix4x3` rules")
    ap.add_argument("--apply", action="store_true", help="write the decidable members")
    ap.add_argument("--dry-run", action="store_true", help="--apply without writing")
    ap.add_argument("--only", default=None,
                    help="comma-separated Class or Class.0xNNN to restrict --apply")
    ap.add_argument("--check", action="store_true",
                    help="fail unless the frozen census reproduces")
    a = ap.parse_args(argv)

    root = pathlib.Path(a.root).resolve()
    ext = pathlib.Path(a.extracted) if a.extracted else O.find_extracted(root)
    out = a.out or str(root / "build" / "dtor_members.json")

    doc = harvest(root, ext)
    st = doc["meta"]["stats"]

    need_gap = a.gap or a.adjacency or a.apply or a.dry_run
    if need_gap:
        mods = O.load_modules(ext, collections.Counter())
        buckets, hdr, _al, sizes = gap(root, doc, mods, collections.Counter())
        doc["meta"]["buckets"] = {b: len(buckets[b]) for b in BUCKETS}

    if a.report:
        print("== populations ==")
        for k in sorted(st):
            print("  %-48s %s" % (k, st[k]))

    if a.gap:
        print("\n== pairs vs the headers ==")
        for b in BUCKETS:
            print("  %-20s %d" % (b, len(buckets[b])))
        for b in ("DECIDABLE", "NO ASSERT", "DECLARED OTHERWISE", "INHERITED OTHERWISE",
                  "NO FIELD AT OFFSET", "NO OWNER HEADER"):
            print("\n-- %s (%d) --" % (b, len(buckets[b])))
            for r in sorted(buckets[b], key=lambda r: (r["owner"], r["offset"])):
                print("  %-34s @%s -> %-22s %s"
                      % ((r.get("owner_header") or r["owner"]), r["offset"], r["member"],
                         r.get("declared") or r.get("inherited") or ""))

    if a.adjacency:
        res, detail = adjacency(doc, hdr, sizes, _al)
        print("\n== adjacency rules ==")
        for typ, (want, _wsz) in sorted(ADJACENCY.items()):
            print("  %s always followed by its own %s?  %s"
                  % (typ, want, dict(sorted(res[typ].items()))))
            for owner, off, verdict, note in detail[typ]:
                if verdict == "REFUTED":
                    print("      COUNTER-EXAMPLE  %-30s @0x%03x  %s" % (owner, off, note))

    if a.apply or a.dry_run:
        only = set(a.only.split(",")) if a.only else None
        picked, held = plan(root, buckets, hdr, sizes, only)
        done, refused = apply(root, picked, sizes, write=a.apply)
        # `done` is one entry per DECLARATION written, and a class defined twice in one
        # header takes two.  Report both, so neither number can be read as the other.
        wrote = {(r["owner"], r["offset"]) for r in done}
        print("\n%s: %d declaration(s), %d pair(s), %d header(s)"
              % ("applied" if a.apply else "would apply", len(done), len(wrote),
                 len({r["header_path"] for r in done})))
        for k, v in held.most_common():
            print("  held  %3d  %s" % (v, k))
        for rec, why in refused:
            print("  refused  %s @%s -> %s: %s"
                  % (rec["owner"], rec["offset"], rec["member"], why))

    os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
    with open(out, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(doc, fh, indent=1, sort_keys=False)
        fh.write("\n")
    print("wrote %s: %d sites, %d pairs (%d member evidence), %d owners"
          % (out, len(doc["sites"]), st.get("pairs_distinct", 0),
             st.get("pairs_member_evidence", 0), st.get("owners_distinct", 0)))

    if a.check:
        bad = [(k, v, st.get(k, 0)) for k, v in GATE.items() if st.get(k, 0) != v]
        for k, want, got in bad:
            print("GATE FAIL %s: want %d got %s" % (k, want, got))
        if bad:
            return 1
        print("GATE OK")
    return 0


if __name__ == "__main__":                                             # pragma: no cover
    sys.exit(main())
