#!/usr/bin/env python3
"""Class sizes read out of the ROM: the literal the cartridge hands its own `operator new`.

`fBase_c::operator new(unsigned int)` -- `_ZN7fBase_cnwEj`, arm9 0x02043444 -- is this
game's allocator, and every `new Class` in the original source compiled to a `mov r0,
#sizeof(Class)` immediately before a `bl` to it.  That immediate is not inference: it is
`sizeof` as the original compiler computed it, frozen in the instruction stream.  There
are 391 such call sites in `config/**/relocs.txt`.

`evidence_rom.py` (pass 3 of gen_header) recovers FIELDS from loads and stores, and is
blind by construction to any field a class never touches -- including all the trailing
padding a class inherits or simply never reads.  It therefore cannot see a struct's
extent, only its occupied prefix.  This pass can see nothing but the extent.  The two
are complements: one says where the fields are, this one says where the object ends.

    python tools/opnew_sizes.py --report          # populations, then findings
    python tools/opnew_sizes.py --gap             # ROM size vs each header's declaration
    python tools/opnew_sizes.py --check           # fail unless the frozen census reproduces

Output: build/opnew_sizes.json -- per class, its ROM size, the factories that prove it,
the vtable it was attributed through, and whether any two sites disagree.

How a site is read
------------------
    bl _ZN7fBase_cnwEj      r0 on entry is the size.  Constant-propagate backwards
                            through the enclosing function to recover it.
    ...                     r0 on return is the object.  Follow it.
    str rV, [rObj]          rV a pool literal -> that is a vptr store.

The class is then whichever RTTI record in `build/rtti.json` owns that vtable.

Four traps, three of which have already cost this project landed work
---------------------------------------------------------------------
pair by vtable    Never by name.  PR #1556 had to retract 2 of 21 "wrong sizes" that
                  were artifacts of pairing a class to a factory by name; if a name
                  defect and a value defect share a table, the names are wrong first.
                  Nothing here ever reads a symbol's spelling to decide whose size it is.

take the LAST     #1559/#1560 landed 13 size corrections and then reverted 2 because the
vptr store        pairing walked to a base.  A factory inlines its base constructors and
                  each one stores ITS OWN vtable to [this] before the derived store; the
                  Itanium order guarantees the derived vptr is written last.  Reading the
                  first store makes every derived class report as its base.  Confirmed
                  live here: `_ZN6CameraC1Ev` stores three different vtables to [r4] and
                  only the third is Camera's.

bound the scan    Commit 3f760a354 is the post-mortem.  `actor_names.parse_spawnfunc`
                  scanned a fixed 0x50 bytes; most factories are 0x30-0x40, and the next
                  function is almost always the FOLLOWING class's D1, which stores its
                  own vtable in its first few instructions.  "Last store" then named the
                  next class -- the mechanism behind the 41 misnamed classes fixed in
                  #1418-#1423.  Every scan here is bounded by the function's own
                  `size=` from config/**/symbols.txt and stops there.

a vtable is not   The literal is `sizeof` of the class actually being newed; the vptr names
the class        the most-derived class that HAS a vtable.  Those are the same class unless
                  a subclass adds no virtual of its own and overrides none, in which case
                  mwccarm gives it no vtable and its constructor leaves the base's vptr in
                  place -- so its size is filed against the base.  Exactly one site in this
                  ROM does it: `StarCamera_Spawn` (ov002 0x020ebe68) allocates 0xd4, calls
                  `dActor_c::dActor_c` and stores nothing further, so `dActor_c` reads as
                  0xd4.  It is 0xd0: 120 classes in this tree inherit dActor_c and every one
                  of them agrees with its own ROM size at 0xd0, which they could not do if
                  the base were four bytes longer.  `gap` therefore refuses to call a class
                  with agreeing descendants SHORT -- it buckets it BASE, NOT SHORT.  Treat
                  the ROM size as an upper bound for any class that is somebody's base, and
                  as exact for a leaf.

raw images, not   `extracted/dsd/arm9_overlays/*.bin` are dsd's DELINKED images: every
delinked ones     relocated word is a placeholder, and a vtable pool literal is nothing
                  but a relocation.  Read through those and every overlay factory reports
                  NOVTABLE.  The ROM's linked bytes are `extracted/overlays/*.bin`; only
                  the base addresses come from the dsd yaml.  Same rule as
                  `rtti_extract.py` and `actor_names.py`.

Requires capstone and pyyaml, like `evidence_rom.py`.  Deterministic: every collection is
sorted before it is emitted.
"""

from __future__ import annotations

import argparse
import bisect
import collections
import json
import os
import pathlib
import re
import struct
import sys

try:
    from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
    from capstone.arm import (
        ARM_CC_AL, ARM_CC_INVALID, ARM_GRP_JUMP,
        ARM_INS_ADD, ARM_INS_AND, ARM_INS_BIC, ARM_INS_BL, ARM_INS_BLX, ARM_INS_CMN,
        ARM_INS_CMP, ARM_INS_EOR, ARM_INS_LDM, ARM_INS_LDR, ARM_INS_MOV, ARM_INS_MVN,
        ARM_INS_ORR, ARM_INS_POP, ARM_INS_RSB, ARM_INS_STM, ARM_INS_STR, ARM_INS_STRB,
        ARM_INS_STRD, ARM_INS_STRH, ARM_INS_SUB, ARM_INS_TEQ, ARM_INS_TST,
        ARM_OP_IMM, ARM_OP_MEM, ARM_OP_REG, ARM_SFT_INVALID,
    )
except ImportError:                                                    # pragma: no cover
    sys.exit("capstone not installed. Run: pip install capstone")
try:
    import yaml
except ImportError:                                                    # pragma: no cover
    sys.exit("pyyaml not installed. Run: pip install pyyaml")

REPO = pathlib.Path(__file__).resolve().parent.parent

ARM9_BASE = 0x02004000
OPERATOR_NEW = 0x02043444          # _ZN7fBase_cnwEj
OPERATOR_NEW_SYM = "_ZN7fBase_cnwEj"

# How deep to follow `bl <ctor>` looking for the vptr store the factory did not inline.
# Two levels covers factory -> ctor -> base ctor; deeper has never added a class and a
# cycle would otherwise not terminate.
MAX_CTOR_DEPTH = 3

SYM_FN = re.compile(
    r"^(\S+)\s+kind:function\(arm,size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)")
SYM_ANY = re.compile(r"^(\S+)\s+kind:\S+\s+addr:(0x[0-9a-fA-F]+)")
RELOC = re.compile(r"^from:0x([0-9a-fA-F]+)\s+kind:(\S+)\s+to:0x([0-9a-fA-F]+)")
LEN_PREFIX = re.compile(r"(\d+)")

# `typedef char Foo_size_must_be_0x330[sizeof(Foo) == 0x330 ? 1 : -1];` -- the tree's
# convention, 307 headers carry one and nothing here uses static_assert.
SIZE_ASSERT = re.compile(
    r"^[ \t]*typedef[ \t]+char[ \t]+(\w+)_size_must_be_0x([0-9a-fA-F]+)\s*\[",
    re.M)

NAMED_REG = {"sb": 9, "sl": 10, "fp": 11, "ip": 12, "sp": 13, "lr": 14, "pc": 15}

# Instructions whose first operand is a source, not a destination.
NO_DEST = {ARM_INS_STR, ARM_INS_STRB, ARM_INS_STRH, ARM_INS_STRD, ARM_INS_STM,
           ARM_INS_CMP, ARM_INS_CMN, ARM_INS_TST, ARM_INS_TEQ}


# --------------------------------------------------------------------------
# modules
# --------------------------------------------------------------------------

def find_extracted(root):
    """Locate extracted/, which is gitignored and so absent from a fresh worktree.

    Same search order as evidence_rom.find_extracted: the worktree, this file's own
    repo, $SM64DS_EXTRACTED, then the shared .git parent a linked worktree points at.
    Never a hardcoded developer path.
    """
    import subprocess
    cands = [root / "extracted", REPO / "extracted"]
    env = os.environ.get("SM64DS_EXTRACTED")
    if env:
        cands.append(pathlib.Path(env))
    try:
        r = subprocess.run(["git", "rev-parse", "--path-format=absolute",
                            "--git-common-dir"], cwd=str(root), capture_output=True,
                           text=True, timeout=10)
        if r.returncode == 0 and r.stdout.strip():
            cands.append(pathlib.Path(r.stdout.strip()).parent / "extracted")
    except (OSError, subprocess.SubprocessError):                      # pragma: no cover
        pass
    for c in cands:
        if (c / "dsd/arm9_overlays/overlays.yaml").exists():
            return c
    sys.exit("could not locate extracted/ -- set SM64DS_EXTRACTED or pass --extracted")


def coerce_addr(v):
    """overlays.yaml gives hex strings for some overlays and ints for others."""
    return v if isinstance(v, int) else int(str(v), 16)


def load_modules(ext, stats):
    """name -> (base, bytes).  Raw linked images; see the module docstring."""
    arm9 = (ext / "arm9_dec.bin").read_bytes()
    # arm9_dec.bin is the DECOMPRESSED arm9 at 0x02004000. extracted/dsd/arm9/arm9.bin
    # is the compressed payload and is garbage past its uncompressed prefix; the anchor
    # below is the same one actor_names.py asserts (Copy36Bytes).
    if arm9[0x0205A548 - ARM9_BASE:0x0205A54C - ARM9_BASE] == b"\0\0\0\0":
        sys.exit("arm9_dec.bin failed the known-code anchor (Copy36Bytes)")
    mods = {"arm9": (ARM9_BASE, arm9)}
    cfg = yaml.safe_load((ext / "dsd/arm9_overlays/overlays.yaml").read_text())
    for o in cfg["overlays"]:
        p = ext / "overlays" / ("overlay_%04d.bin" % o["id"])
        if p.is_file():
            mods["ov%03d" % o["id"]] = (coerce_addr(o["base_address"]), p.read_bytes())
        else:
            stats["overlay_images_missing"] += 1
    stats["modules_loaded"] = len(mods)
    return mods


def load_symbols(root, mods, stats):
    """module -> sorted [(addr, size, name)] of every function symbol."""
    out = {}
    paths = [("arm9", root / "config" / "arm9" / "symbols.txt")]
    for d in sorted((root / "config" / "arm9" / "overlays").glob("ov*")):
        paths.append((d.name, d / "symbols.txt"))
    for mod, p in paths:
        if mod not in mods or not p.is_file():
            continue
        fns = []
        for line in p.read_text(errors="replace").splitlines():
            m = SYM_FN.match(line)
            if m:
                fns.append((int(m.group(3), 16), int(m.group(2), 16), m.group(1)))
        fns.sort()
        out[mod] = fns
        stats["function_symbols"] += len(fns)
    stats["modules_with_symbols"] = len(out)
    return out


def load_vtable_symbols(root, mods, stats):
    """(module, addr) -> the tree's own name for the class whose vtable sits there.

    The cartridge's RTTI name and this tree's name for the same class are often
    different -- `_ZTV6Camera` and `dCamera_c` are one class -- because most of the
    English names predate the RTTI pass and have not been renamed yet.  A header can
    only be found under the name the tree uses, so both are carried.

    This is still a VTABLE-ADDRESS join, not a name join: the address decides which
    symbol is read, and the symbol only supplies the spelling.  Pairing a class to a
    factory by name is what PR #1556 had to retract 2 of 21 findings for.
    """
    out = {}
    paths = [("arm9", root / "config" / "arm9" / "symbols.txt")]
    for d in sorted((root / "config" / "arm9" / "overlays").glob("ov*")):
        paths.append((d.name, d / "symbols.txt"))
    for mod, p in paths:
        if mod not in mods or not p.is_file():
            continue
        for line in p.read_text(errors="replace").splitlines():
            if not line.startswith("_ZTV"):
                continue
            m = SYM_ANY.match(line)
            if m:
                out[(mod, int(m.group(2), 16))] = m.group(1)
    stats["vtable_symbols_named_in_config"] = len(out)
    return out


def demangle_ztv(sym):
    """`_ZTV6Camera` -> Camera;  `_ZTVN8dGraph_c10callback_cE` -> dGraph_c__callback_c.

    Nested names are joined with `__`, matching how this tree names their headers
    (`include/ActorBase__SceneNode.h`) -- the same convention evidence_rom.class_of uses.
    """
    if not sym.startswith("_ZTV"):
        return None
    body = sym[4:]
    if body.startswith("N") and body.endswith("E"):
        body = body[1:-1]
    parts, i = [], 0
    while i < len(body):
        m = LEN_PREFIX.match(body, i)
        if not m:
            return None
        n = int(m.group(1))
        i = m.end()
        if n == 0 or i + n > len(body):
            return None
        parts.append(body[i:i + n])
        i += n
    return "__".join(parts) if parts else None


def load_call_sites(root, mods, stats):
    """Every `bl` to operator new, as sorted (module, from_addr).

    The reloc's module is the one whose relocs.txt records it -- `from:` addresses are
    that module's own.  `to:` is a bare address, which is only unambiguous here because
    the callee is in arm9, always resident.
    """
    sites = []
    paths = [("arm9", root / "config" / "arm9" / "relocs.txt")]
    for d in sorted((root / "config" / "arm9" / "overlays").glob("ov*")):
        paths.append((d.name, d / "relocs.txt"))
    for mod, p in paths:
        if not p.is_file():
            continue
        if mod not in mods:
            stats["sites_in_a_module_with_no_image"] += 1
            continue
        for line in p.read_text(errors="replace").splitlines():
            m = RELOC.match(line)
            if not m or int(m.group(3), 16) != OPERATOR_NEW:
                continue
            if m.group(2) != "arm_call":
                stats["sites_not_arm_call"] += 1
                continue
            sites.append((mod, int(m.group(1), 16)))
    sites.sort()
    stats["call_sites"] = len(sites)
    return sites


# --------------------------------------------------------------------------
# a very small ARM abstract interpreter
# --------------------------------------------------------------------------

def reg_num(name):
    if not name:
        return -1
    if re.fullmatch(r"r\d+", name):
        return int(name[1:])
    return NAMED_REG.get(name, -1)


class Image:
    """The bytes of every module, and a cached disassembly per function."""

    def __init__(self, mods, syms):
        self.mods = mods
        self.syms = syms
        self.md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
        self.md.detail = True
        self._dis = {}

    def word(self, mod, va):
        """A word at a virtual address: the module's own image, else always-resident arm9."""
        for m in (mod, "arm9"):
            ent = self.mods.get(m)
            if not ent:
                continue
            base, blob = ent
            o = va - base
            if 0 <= o <= len(blob) - 4:
                return struct.unpack_from("<I", blob, o)[0]
        return None

    def enclosing(self, mod, addr):
        """(start, size, name) of the function containing addr, or None.

        This is the ONLY thing that bounds a scan.  See trap 3 in the module docstring:
        a window that is not the function's own runs into the next class's destructor.
        """
        lst = self.syms.get(mod, ())
        i = bisect.bisect_right(lst, (addr, 1 << 62, "￿")) - 1
        if i < 0:
            return None
        a, s, n = lst[i]
        return (a, s, n) if a <= addr < a + s else None

    def disasm(self, mod, start, size):
        key = (mod, start)
        if key not in self._dis:
            base, blob = self.mods[mod]
            o = start - base
            if o < 0 or o + size > len(blob):
                self._dis[key] = []
            else:
                self._dis[key] = list(self.md.disasm(blob[o:o + size], start))
        return self._dis[key]


class State:
    """Registers that provably hold a known constant, and those that hold the object."""

    __slots__ = ("const", "obj")

    def __init__(self, obj=()):
        self.const = {}
        self.obj = set(obj)

    def kill(self, r):
        self.const.pop(r, None)
        self.obj.discard(r)

    def clobber_call(self):
        """AAPCS: r0-r3 and r12 are call-clobbered; r4-r11 survive."""
        for r in (0, 1, 2, 3, 12, 14):
            self.kill(r)
        self.const = {k: v for k, v in self.const.items() if 4 <= k <= 11}


def _dest_regs(ins):
    if ins.id in NO_DEST or not ins.operands:
        return []
    if ins.id in (ARM_INS_POP, ARM_INS_LDM):
        return [reg_num(ins.reg_name(o.reg)) for o in ins.operands
                if o.type == ARM_OP_REG]
    o = ins.operands[0]
    return [reg_num(ins.reg_name(o.reg))] if o.type == ARM_OP_REG else []


def _evaluate(img, mod, ins, st):
    """(new_const|None, propagates_object) for this instruction's destination."""
    ops = ins.operands
    if ins.id == ARM_INS_MOV and len(ops) == 2:
        if ops[1].type == ARM_OP_IMM:
            return ops[1].imm & 0xFFFFFFFF, False
        if ops[1].type == ARM_OP_REG and ops[1].shift.type == ARM_SFT_INVALID:
            s = reg_num(ins.reg_name(ops[1].reg))
            return st.const.get(s), s in st.obj
    if ins.id == ARM_INS_MVN and len(ops) == 2 and ops[1].type == ARM_OP_IMM:
        return (~ops[1].imm) & 0xFFFFFFFF, False
    if ins.id == ARM_INS_LDR and len(ops) == 2 and ops[1].type == ARM_OP_MEM:
        m = ops[1].mem
        # A pc-relative load is a literal-pool read: the value is in the image.
        if ins.reg_name(m.base) == "pc" and m.index == 0:
            return img.word(mod, ins.address + 8 + m.disp), False
        return None, False
    if ins.id in (ARM_INS_ADD, ARM_INS_SUB) and len(ops) == 3 and \
            ops[1].type == ARM_OP_REG and ops[2].type == ARM_OP_IMM:
        s = reg_num(ins.reg_name(ops[1].reg))
        v = st.const.get(s)
        if v is not None:
            v = (v + ops[2].imm if ins.id == ARM_INS_ADD else v - ops[2].imm) & 0xFFFFFFFF
        # `subs rD, rObj, #0` is mwcc's copy-and-null-test; the pointer survives it.
        # `add rD, rObj, #n` is a SUBOBJECT and must not: a member's constructor stores
        # its own vtable to [this+n], and treating that as the object's vptr is how a
        # class ends up wearing a member's identity.
        return v, (ins.id == ARM_INS_SUB and ops[2].imm == 0 and s in st.obj)
    if ins.id in (ARM_INS_ORR, ARM_INS_AND, ARM_INS_EOR, ARM_INS_BIC, ARM_INS_RSB) and \
            len(ops) == 3 and ops[1].type == ARM_OP_REG and ops[2].type == ARM_OP_IMM:
        s = st.const.get(reg_num(ins.reg_name(ops[1].reg)))
        if s is None:
            return None, False
        i = ops[2].imm
        v = {ARM_INS_ORR: s | i, ARM_INS_AND: s & i, ARM_INS_EOR: s ^ i,
             ARM_INS_BIC: s & ~i, ARM_INS_RSB: i - s}[ins.id]
        return v & 0xFFFFFFFF, False
    return None, False


def _step(img, mod, ins, st):
    """Apply one instruction to the state.  Unmodelled writes invalidate."""
    conditional = ins.cc not in (ARM_CC_AL, ARM_CC_INVALID)
    val, is_obj = _evaluate(img, mod, ins, st)
    for d in _dest_regs(ins):
        if d < 0:
            continue
        if conditional:
            # The write may not have happened; neither the old nor the new value is known.
            st.kill(d)
            continue
        st.kill(d)
        if val is not None:
            st.const[d] = val
        if is_obj:
            st.obj.add(d)
    # writeback forms (`ldr rD, [rB, #n]!`, `stm rB!, {...}`) also change the base
    if ins.writeback:
        for o in ins.operands:
            if o.type == ARM_OP_MEM and o.mem.base:
                st.kill(reg_num(ins.reg_name(o.mem.base)))


def _vptr_store(ins, st):
    """The literal a `str rV, [rObj]` writes to offset 0, or None.

    Offset 0 only.  A store to [rObj, #n] is a member subobject's vptr, not this
    object's, and admitting one would attribute the class to whatever it contains.
    """
    ops = ins.operands
    if ins.id != ARM_INS_STR or len(ops) != 2 or ops[1].type != ARM_OP_MEM:
        return None
    m = ops[1].mem
    if m.index != 0 or m.disp != 0 or ins.writeback:
        return None
    if reg_num(ins.reg_name(m.base)) not in st.obj:
        return None
    if ops[0].type != ARM_OP_REG:
        return None
    return st.const.get(reg_num(ins.reg_name(ops[0].reg)))


def _branch_targets(inss):
    t = set()
    for ins in inss:
        if ins.group(ARM_GRP_JUMP) and ins.operands and \
                ins.operands[-1].type == ARM_OP_IMM:
            t.add(ins.operands[-1].imm)
    return t


def _walk(img, mod, start, size, st, callsite, stats, depth, seen):
    """Linear sweep of one function; returns the ordered vptr evidence it produced.

    Each element is ('store', addr, vtable) or ('call', addr, callee) -- a call made
    with the object in r0, i.e. a constructor that may install the vptr itself.  The
    caller takes the LAST, which is the derived-most: see trap 2.
    """
    inss = img.disasm(mod, start, size)
    if not inss:
        stats["functions_that_would_not_disassemble"] += 1
        return []
    targets = _branch_targets(inss)
    events = []
    armed = callsite is None            # None => r0 is `this` on entry (a ctor)
    for ins in inss:
        if ins.address in targets:
            # A join: another path reaches here, so no constant is provable any more.
            # The object pointer itself is kept -- it lives in a callee-saved register
            # across the null test every one of these factories performs, and dropping
            # it here loses the vptr store on the taken path.
            st.const.clear()
        if callsite is not None and ins.address == callsite:
            # r0 in  = the size; r0 out = the object.
            st.const.clear()
            st.obj = {0}
            armed = True
            continue
        if ins.id in (ARM_INS_BL, ARM_INS_BLX):
            if armed and 0 in st.obj and ins.operands and \
                    ins.operands[0].type == ARM_OP_IMM:
                events.append(("call", ins.address, ins.operands[0].imm))
            st.clobber_call()
            continue
        if armed:
            v = _vptr_store(ins, st)
            if v:
                events.append(("store", ins.address, v))
                continue
        _step(img, mod, ins, st)
    return events


def resolve_vptr(img, mod, events, stats, depth=0, seen=None):
    """The class's own vtable: the LAST piece of evidence, followed into a ctor if need be.

    Returns (vtable, how, via) or (None, reason, None).
    """
    seen = seen if seen is not None else set()
    for kind, _addr, val in reversed(events):
        if kind == "store":
            return val, "inline_store", None
        if depth >= MAX_CTOR_DEPTH:
            stats["ctor_chains_deeper_than_the_limit"] += 1
            continue
        # A call made with the object in r0: a constructor. Read its own last store.
        for m in (mod, "arm9"):
            e = img.enclosing(m, val)
            if e is None or e[0] != val:
                continue
            if (m, val) in seen:
                break
            seen.add((m, val))
            sub = _walk(img, m, e[0], e[1], State(obj={0}), None, stats, depth + 1, seen)
            vt, how, via = resolve_vptr(img, m, sub, stats, depth + 1, seen)
            if vt:
                return vt, "ctor_" + how, "%s:%s" % (m, e[2])
            break
    return None, "no_vptr_store_found", None


# --------------------------------------------------------------------------
# joining to RTTI
# --------------------------------------------------------------------------

def load_rtti(root, stats):
    """(vtable address -> [(module, class name)], class -> set of ancestor classes).

    An address alone is not an identity in this ROM -- overlays share address space and
    two addresses host two different classes each.  Resolution keeps (module, addr).
    The ancestor closure exists only to audit this file's central assumption; see
    `audit_derived_most`.
    """
    p = root / "build" / "rtti.json"
    if not p.is_file():
        sys.exit("build/rtti.json absent -- run tools/rtti_extract.py first")
    doc = json.loads(p.read_text(encoding="utf-8"))
    by_addr = collections.defaultdict(list)
    for rec in doc["records"].values():
        if rec.get("vtable"):
            by_addr[int(rec["vtable"], 16)].append((rec["vtable_module"], rec["name"]))
    direct = collections.defaultdict(set)
    for e in doc["edges"]:
        direct[e["derived"]].add(e["base"])
    anc = {}

    def ancestors(n, seen=None):
        if n in anc:
            return anc[n]
        seen = seen or set()
        if n in seen:
            return set()
        seen = seen | {n}
        out = set()
        for b in direct.get(n, ()):
            out.add(b)
            out |= ancestors(b, seen)
        anc[n] = out
        return out

    for n in list(direct):
        ancestors(n)
    stats["rtti_records"] = len(doc["records"])
    stats["rtti_vtable_addresses"] = len(by_addr)
    return by_addr, anc


def class_for_vtable(by_addr, mod, vt, stats):
    """(name, how) or (None, why).  Own module, then arm9, then residency, then give up."""
    cands = sorted(set(by_addr.get(vt, ())))
    if not cands:
        stats["vtables_with_no_rtti_record"] += 1
        return None, "no_rtti_record_at_vtable"
    own = [c for c in cands if c[0] == mod]
    if len(own) == 1:
        return own[0][1], "own_module"
    a9 = [c for c in cands if c[0] == "arm9"]
    if len(a9) == 1:
        return a9[0][1], "arm9"
    if len(cands) == 1:
        return cands[0][1], "unique"
    try:
        sys.path.insert(0, str(REPO / "tools"))
        import overlay_residency as RES
        left = [c for c in cands if RES.possible(c[0], mod)]
    except Exception:                                                  # noqa: BLE001
        left = cands
    if len(left) == 1:
        return left[0][1], "residency"
    stats["vtables_ambiguous_across_modules"] += 1
    return None, "ambiguous:" + ",".join("%s/%s" % c for c in cands)


# --------------------------------------------------------------------------
# build
# --------------------------------------------------------------------------

def audit_derived_most(by_addr, anc, mod, chosen, events, stats, warnings):
    """Prove, per site, that the LAST vptr store really was the derived-most one.

    This file's whole attribution rests on the Itanium ordering: base constructors run
    first and each writes its own vtable, so the derived class's vptr is written last.
    #1559/#1560 landed 13 size corrections on that rule and had to revert 2, so it is
    asserted rather than assumed.  Every OTHER vtable the factory stored to [this] must
    name a class that `chosen` derives from.  If `chosen` turns out to be an ANCESTOR of
    something else stored, the rule read the wrong store and the site is a finding.
    """
    others = set()
    for kind, _a, val in events:
        if kind != "store":
            continue
        name, _ = class_for_vtable(by_addr, mod, val, collections.Counter())
        if name:
            others.add(name)
    others.discard(chosen)
    if not others:
        return
    for o in sorted(others):
        if o in anc.get(chosen, ()):
            stats["cross_check_other_store_is_a_base_as_expected"] += 1
        elif chosen in anc.get(o, ()):
            stats["CROSS_CHECK_FAIL_chose_a_base_over_a_derived"] += 1
            warnings.append("%s: chose %s but %s (a derived class) was also stored"
                            % (mod, chosen, o))
        else:
            stats["cross_check_other_store_unrelated"] += 1
            warnings.append("%s: chose %s; also stored unrelated %s" % (mod, chosen, o))


def build(root, ext):
    stats = collections.Counter()
    mods = load_modules(ext, stats)
    syms = load_symbols(root, mods, stats)
    sites = load_call_sites(root, mods, stats)
    by_addr, anc = load_rtti(root, stats)
    vtsyms = load_vtable_symbols(root, mods, stats)
    img = Image(mods, syms)

    def tree_class(mod, vt):
        for m in (mod, "arm9"):
            s = vtsyms.get((m, vt))
            if s:
                return demangle_ztv(s)
        return None

    records, warnings = [], []
    for mod, site in sites:
        enc = img.enclosing(mod, site)
        if enc is None:
            stats["sites_with_no_enclosing_function"] += 1
            records.append({"module": mod, "site": "0x%08x" % site, "factory": None,
                            "factory_addr": None, "size": None,
                            "size_confidence": "no_enclosing_function",
                            "vtable": None, "attribution": "no_enclosing_function",
                            "via": None, "class": None})
            continue
        fa, fs, fn = enc
        events = _walk(img, mod, fa, fs, State(), site, stats, 0, set())
        size, size_conf = read_size(img, mod, fa, fs, site, stats)
        vt, how, via = resolve_vptr(img, mod, events, stats)
        cls, cls_how = None, how
        if vt:
            cls, cls_how = class_for_vtable(by_addr, mod, vt, stats)
        if cls:
            audit_derived_most(by_addr, anc, mod, cls, events, stats, warnings)
        tcls = tree_class(mod, vt) if vt else None
        if vt:
            stats["vtables_the_tree_has_not_named" if not tcls
                  else "vtables_named_by_the_tree"] += 1
            if tcls and cls and tcls != cls:
                stats["classes_the_tree_spells_differently_from_the_rtti"] += 1
        records.append({
            "module": mod, "site": "0x%08x" % site, "factory": fn,
            "factory_addr": "0x%08x" % fa, "factory_size": "0x%x" % fs,
            "size": size, "size_confidence": size_conf,
            "vtable": ("0x%08x" % vt) if vt else None,
            "attribution": how, "resolved_by": cls_how if vt else None,
            "via": via, "class": cls, "tree_class": tcls,
        })
        stats["size_" + size_conf] += 1
        stats["attribution_" + (cls_how if vt else how)] += 1

    # ---- fold to classes, keyed on the name the TREE uses (the one a header carries),
    # falling back to the RTTI name where the tree has not named the vtable yet.
    per = collections.defaultdict(list)
    for r in records:
        key = r["tree_class"] or r["class"]
        if key and r["size"] is not None:
            per[key].append(r)
    classes = {}
    for name, rs in sorted(per.items()):
        sizes = sorted({r["size"] for r in rs})
        classes[name] = {
            "size": sizes[0] if len(sizes) == 1 else None,
            "size_hex": ("0x%x" % sizes[0]) if len(sizes) == 1 else None,
            "ambiguous": len(sizes) > 1,
            "sizes_seen": ["0x%x" % s for s in sizes],
            "rtti_name": sorted({r["class"] for r in rs if r["class"]}),
            "named_by_the_tree": bool(rs[0]["tree_class"]),
            "sites": len(rs),
            "confidence": ("inline_store" if any(r["attribution"] == "inline_store"
                                                 for r in rs) else "via_constructor"),
            "factories": sorted({"%s:%s" % (r["module"], r["factory"]) for r in rs}),
            "vtables": sorted({r["vtable"] for r in rs if r["vtable"]}),
        }
        stats["classes_ambiguous" if len(sizes) > 1 else "classes_unambiguous"] += 1
    stats["classes_total"] = len(classes)
    stats["sites_attributed"] = sum(1 for r in records if r["class"])
    stats["sites_unattributed"] = len(records) - stats["sites_attributed"]

    return {
        "meta": {
            "operator_new": {"symbol": OPERATOR_NEW_SYM,
                             "addr": "0x%08x" % OPERATOR_NEW},
            "source": "config/**/relocs.txt + extracted/arm9_dec.bin + "
                      "extracted/overlays/*.bin + build/rtti.json",
            "stats": dict(sorted(stats.items())),
        },
        "classes": classes,
        "sites": records,
        "cross_check_warnings": sorted(set(warnings)),
    }


def read_size(img, mod, fa, fs, site, stats):
    """r0 at the `bl`, constant-propagated from the top of the enclosing function."""
    inss = img.disasm(mod, fa, fs)
    targets = _branch_targets(inss)
    st = State()
    for ins in inss:
        if ins.address in targets:
            st.const.clear()
        if ins.address == site:
            v = st.const.get(0)
            return (v, "exact") if v is not None else (None, "not_a_constant")
        if ins.id in (ARM_INS_BL, ARM_INS_BLX):
            st.clobber_call()
            continue
        _step(img, mod, ins, st)
    return None, "call_site_not_in_function"


# --------------------------------------------------------------------------
# gap report: ROM size vs the headers
# --------------------------------------------------------------------------

DEFINITION = re.compile(r"^[ \t]*(?:class|struct)[ \t]+(\w+)[ \t]*(?::[^;{]*)?\{", re.M)


def header_sizes(root):
    """class -> {"assert": size|None, "header": path|None}, over every include/**/*.h.

    Only the size assert is read for the declared extent.  A header's real extent is
    whatever the compiler makes of it and cannot be recovered by regex -- but the assert
    IS the tree's statement of that extent, and it is compiler-checked on every build,
    so where one exists it is exactly as authoritative and is the thing an edit changes.
    A class with a definition but no assert is reported as its own bucket rather than
    guessed at.
    """
    out = {}
    for h in sorted((root / "include").rglob("*.h")):
        txt = h.read_text(errors="replace")
        for m in DEFINITION.finditer(txt):
            out.setdefault(m.group(1), {"assert": None, "header": h})
        for m in SIZE_ASSERT.finditer(txt):
            e = out.setdefault(m.group(1), {"assert": None, "header": h})
            if e["assert"] is None:
                e["assert"] = int(m.group(2), 16)
                e["header"] = h
    return out


BUCKETS = ("AGREES", "HEADER SHORT", "HEADER LONG", "BASE, NOT SHORT", "NO SIZE",
           "NO HEADER", "AMBIGUOUS")

INHERITS = re.compile(r"^[ \t]*(?:class|struct)[ \t]+(\w+)[ \t]*:([^{;]+)\{", re.M)
ACCESS = {"public", "protected", "private", "virtual"}


def subclass_index(root):
    """class -> every class in include/ that transitively derives from it."""
    bases = collections.defaultdict(set)
    for h in sorted((root / "include").rglob("*.h")):
        for m in INHERITS.finditer(h.read_text(errors="replace")):
            for b in re.findall(r"[A-Za-z_]\w*", m.group(2)):
                if b not in ACCESS:
                    bases[m.group(1)].add(b)
    kids = collections.defaultdict(set)
    for d, bs in bases.items():
        for b in bs:
            kids[b].add(d)
    out = {}
    for b in list(kids):
        seen, stack = set(), list(kids[b])
        while stack:
            n = stack.pop()
            if n in seen:
                continue
            seen.add(n)
            stack.extend(kids.get(n, ()))
        out[b] = seen
    return out

PROBE_SRC = '#include "%s"\nchar opnew_probe[sizeof(%s)];\n'


def probe_one(exe, flags, include, tmp, cls, header):
    """`sizeof(cls)` as the build's own compiler computes it, or (None, why).

    A regex cannot measure a header.  Half of these classes are flat shadow structs
    with no size assert at all, and the ones that do carry an assert only restate a
    number a human typed.  This asks mwccarm: declare `char opnew_probe[sizeof(X)]`
    and read the symbol's st_size back out of the object.  One compile, exact answer,
    and it accounts for base classes, alignment and the vptr the same way the ROM build
    does because it IS the ROM build's compiler and flags.
    """
    import subprocess
    from elftools.elf.elffile import ELFFile
    src = tmp / ("probe_%s.cpp" % re.sub(r"\W", "_", cls))
    obj = src.with_suffix(".o")
    for spelling in (cls, "struct " + cls):
        src.write_text(PROBE_SRC % (header, spelling), encoding="utf-8")
        r = subprocess.run([str(exe), *flags.split(), "-i", str(include),
                            "-c", str(src), "-o", str(obj)],
                           capture_output=True, text=True, timeout=180)
        if r.returncode == 0 and obj.is_file():
            with open(obj, "rb") as fh:
                st = ELFFile(fh).get_section_by_name(".symtab")
                for s in st.iter_symbols():
                    if s.name == "opnew_probe":
                        return s["st_size"], None
            return None, "probe symbol absent"
    msg = (r.stderr or r.stdout or "").strip().splitlines()
    return None, (msg[0][:120] if msg else "compile failed")


def probe_sizes(root, wanted, stats):
    """{class: (size|None, why)} measured by the compiler, over `wanted` (cls -> header)."""
    import concurrent.futures
    import tempfile
    sys.path.insert(0, str(root / "tools"))
    try:
        from rombuild import CFLAGS, MW, VERSION
    except Exception as exc:                                           # noqa: BLE001
        stats["probe_unavailable"] = 1
        return {}, "rombuild unimportable: %s" % exc
    import importlib.util
    if importlib.util.find_spec("elftools") is None:
        stats["probe_unavailable"] = 1
        return {}, "pyelftools not installed"
    exe = MW / VERSION / "mwccarm.exe"
    if not exe.is_file():
        stats["probe_unavailable"] = 1
        return {}, "mwccarm %s not installed" % VERSION
    # The build's own flags, in C++ mode: a class with a base or a vtable has no size
    # in C, and `-lang c99` would silently measure the shadow struct instead.
    flags = CFLAGS.replace("-lang c99", "-lang c++")
    out = {}
    with tempfile.TemporaryDirectory(prefix="opnew-probe-") as td:
        tmp = pathlib.Path(td)
        with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
            futs = {pool.submit(probe_one, exe, flags, root / "include", tmp,
                                cls, hdr): cls for cls, hdr in sorted(wanted.items())}
            for f in concurrent.futures.as_completed(futs):
                cls = futs[f]
                try:
                    out[cls] = f.result()
                except Exception as exc:                               # noqa: BLE001
                    out[cls] = (None, "probe raised: %s" % exc)
    stats["probe_measured"] = sum(1 for v in out.values() if v[0] is not None)
    stats["probe_would_not_compile"] = sum(1 for v in out.values() if v[0] is None)
    return out, None


def gap(root, doc, probe=True):
    """Bucket every unambiguously-sized class against the tree's own declaration.

    The declared size is the compiler's answer where the probe can build, and the size
    assert only where it cannot -- the assert is a human's claim, `sizeof` is the fact.
    A header with neither is NO SIZE: unmeasured, never guessed at.
    """
    hdr = header_sizes(root)
    stats = collections.Counter()
    measured, why = ({}, "probe disabled")
    if probe:
        wanted = {n: hdr[n]["header"].name for n in doc["classes"] if n in hdr}
        measured, why = probe_sizes(root, wanted, stats)
    kids = subclass_index(root)

    def agreeing_descendants(name):
        """Descendants whose own declared size already matches their own ROM size.

        One of those is proof the base is NOT short: growing it moves every one of them.
        See the fifth trap in the module docstring -- this is what dActor_c is."""
        out = []
        for k in sorted(kids.get(name, ())):
            c = doc["classes"].get(k)
            if not c or c["ambiguous"]:
                continue
            d = measured.get(k, (None, None))[0]
            if d is None and k in hdr:
                d = hdr[k]["assert"]
            if d is not None and d == c["size"]:
                out.append(k)
        return out

    buckets = collections.defaultdict(list)
    for name, c in sorted(doc["classes"].items()):
        if c["ambiguous"]:
            buckets["AMBIGUOUS"].append((name, None, c["sizes_seen"], None, "-"))
            continue
        rom = c["size"]
        e = hdr.get(name)
        if e is None:
            buckets["NO HEADER"].append((name, None, rom, None, "-"))
            continue
        path = str(e["header"].relative_to(root)).replace("\\", "/")
        dec, how = measured.get(name, (None, None))
        how = "sizeof"
        if dec is None:
            dec, how = e["assert"], "assert"
        if dec is None:
            buckets["NO SIZE"].append((name, None, rom, path,
                                       measured.get(name, (None, why))[1] or "-"))
        elif dec == rom:
            buckets["AGREES"].append((name, dec, rom, path, how))
        elif dec < rom:
            agree = agreeing_descendants(name)
            if agree:
                buckets["BASE, NOT SHORT"].append(
                    (name, dec, rom, path,
                     "%d agreeing subclasses, e.g. %s" % (len(agree), agree[0])))
            else:
                buckets["HEADER SHORT"].append((name, dec, rom, path, how))
        else:
            buckets["HEADER LONG"].append((name, dec, rom, path, how))
    return buckets, hdr, stats


# --------------------------------------------------------------------------
# main
# --------------------------------------------------------------------------

# The frozen census.  A pass that silently matches nothing reports a clean, confident,
# wrong answer -- notes/plan-gen-header.md 2.  These are the numbers this file produced
# when it landed; a change to any of them is a finding, not a detail.
GATE = {
    "call_sites": 391,
    "size_exact": 391,
}


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--root", default=".", help="repo/worktree root")
    ap.add_argument("--extracted", default=None)
    ap.add_argument("-o", "--out", default=None,
                    help="default build/opnew_sizes.json under --root")
    ap.add_argument("--report", action="store_true", help="print populations first")
    ap.add_argument("--gap", action="store_true",
                    help="bucket every ROM size against the headers")
    ap.add_argument("--no-probe", action="store_true",
                    help="--gap: read size asserts instead of asking the compiler")
    ap.add_argument("--check", action="store_true",
                    help="fail unless the frozen census reproduces")
    a = ap.parse_args(argv)

    root = pathlib.Path(a.root).resolve()
    ext = pathlib.Path(a.extracted) if a.extracted else find_extracted(root)
    out = a.out or str(root / "build" / "opnew_sizes.json")

    doc = build(root, ext)
    st = doc["meta"]["stats"]

    if a.report:
        print("== populations ==")
        for k in sorted(st):
            print("  %-48s %s" % (k, st[k]))
        bad = [r for r in doc["sites"] if not r["class"]]
        print("\n== unattributed sites (%d) ==" % len(bad))
        for r in bad[:40]:
            print("  %-6s %s  %-40s %s" % (r["module"], r["site"],
                                           (r["factory"] or "-")[:40], r["attribution"]))
        if len(bad) > 40:
            print("  ... %d more" % (len(bad) - 40))
        print("\n== cross-check warnings (%d) ==" % len(doc["cross_check_warnings"]))
        for w in doc["cross_check_warnings"][:40]:
            print("  " + w)

    if a.gap:
        buckets, _hdr, gstats = gap(root, doc, probe=not a.no_probe)
        print("\n== gap vs headers ==")
        for k in sorted(gstats):
            print("  (%s %s)" % (k, gstats[k]))
        for b in BUCKETS:
            print("  %-13s %d" % (b, len(buckets[b])))
        for b in ("HEADER LONG", "HEADER SHORT", "BASE, NOT SHORT", "NO SIZE"):
            print("\n-- %s (%d) --" % (b, len(buckets[b])))
            for name, dec, rom, path, how in buckets[b]:
                d = ("0x%x" % dec) if dec is not None else how
                print("  %-34s header %-9s rom 0x%-6x %s" % (name, d, rom, path))

    os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
    with open(out, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(doc, fh, indent=1, sort_keys=False)
        fh.write("\n")
    print("wrote %s: %d sites, %d classes (%d unambiguous), %d sites unattributed"
          % (out, st.get("call_sites", 0), st.get("classes_total", 0),
             st.get("classes_unambiguous", 0), st.get("sites_unattributed", 0)))

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
