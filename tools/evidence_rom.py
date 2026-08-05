"""Pass 3 of gen_header: field evidence read out of the ROM's own instructions.

The only source-independent authority for struct layout. For every matched function
whose mangled name places it in a class, disassemble its byte range and record loads
and stores whose base register provably still holds the entry `this` (r0, AAPCS).

It does not lead. Measured over the corpus it reaches well under half the fields, and
it is blind by construction to an inherited field a derived class never touches --
see notes/plan-gen-header.md 3. It verifies, and it breaks ties, and it is the only
pass that can speak to signedness.

    python tools/evidence_rom.py --report
    python tools/evidence_rom.py -o build/evidence_rom.json

Requires capstone and pyyaml.
"""
import argparse
import collections
import json
import os
import pathlib
import re
import subprocess
import sys

try:
    from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
    from capstone.arm import (ARM_OP_REG, ARM_OP_MEM, ARM_INS_BL, ARM_INS_BLX,
                              ARM_INS_BX, ARM_INS_POP, ARM_INS_MOV, ARM_INS_LDM,
                              ARM_CC_AL, ARM_CC_INVALID)
except ImportError:
    sys.exit("capstone not installed. Run: pip install capstone")
try:
    import yaml
except ImportError:
    sys.exit("pyyaml not installed. Run: pip install pyyaml")

BANNER = "AUTO-GENERATED from matched-function evidence"
MANGLE = re.compile(r"_ZN(\d+)([A-Za-z_][A-Za-z_0-9]*)")
SYM = re.compile(r"^(\S+)\s+kind:function\(arm,size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)")

# width in bytes, and whether the mnemonic itself proves signedness
LOADS = {"ldrsb": (1, "s"), "ldrsh": (2, "s"), "ldrb": (1, "u"), "ldrh": (2, "u"),
         "ldr": (4, None), "ldrd": (8, None)}
STORES = {"strb": (1, None), "strh": (2, None), "str": (4, None), "strd": (8, None)}
# longest first: 'ldrsb' must beat 'ldr'
MNEMONICS = sorted(set(LOADS) | set(STORES), key=len, reverse=True)

NAMED_REG = {"sb": 9, "sl": 10, "fp": 11, "ip": 12, "sp": 13, "lr": 14, "pc": 15}

ARM9_BASE = 0x02004000       # where arm9_dec.bin loads; NOT the 0x02000000 RAM base

# A wrong load address does not fail -- it silently decodes the wrong bytes into a
# confident report. Real ARM functions overwhelmingly open by saving registers, so the
# share of symbols whose first instruction is a push/stmdb separates a correct base
# (~65%) from a wrong one (~2%, i.e. chance) by more than an order of magnitude.
MIN_PROLOGUE_RATE = 0.30


def prologue_rate(md, data, base, addrs):
    """Fraction of the given symbols whose first instruction saves registers."""
    hit = seen = 0
    for a in addrs:
        o = a - base
        if o < 0 or o + 4 > len(data):
            continue
        seen += 1
        ins = next(md.disasm(data[o:o + 4], a), None)
        if ins and (ins.mnemonic.startswith("stmdb") or ins.mnemonic.startswith("push")):
            hit += 1
    return (hit / seen if seen else 0.0), seen


def reg_num(name):
    if not name:
        return -1
    if re.fullmatch(r"r\d+", name):
        return int(name[1:])
    return NAMED_REG.get(name, -1)


def is_unconditional_return(insn):
    """`bx lr`, `mov pc, lr`, or a pop/ldm that writes pc -- and only unconditionally.

    A conditional return (`bxeq lr`, `popne {..,pc}`) leaves a live fall-through path,
    so state must survive it.
    """
    if insn.cc not in (ARM_CC_AL, ARM_CC_INVALID):
        return False
    if insn.id == ARM_INS_BX:
        return bool(insn.operands) and insn.reg_name(insn.operands[0].reg) == "lr"
    if insn.id in (ARM_INS_POP, ARM_INS_LDM):
        return any(o.type == ARM_OP_REG and insn.reg_name(o.reg) == "pc"
                   for o in insn.operands)
    if insn.id == ARM_INS_MOV and len(insn.operands) == 2:
        return (insn.reg_name(insn.operands[0].reg) == "pc"
                and insn.operands[1].type == ARM_OP_REG
                and insn.reg_name(insn.operands[1].reg) == "lr")
    return False


def class_of(sym):
    """Nested classes need every component, not just the first.

    `_ZN` is followed by length-prefixed components and terminated by `E`. The last
    component is the method name unless the terminator is a constructor (`C1`/`C2`)
    or destructor (`D0`/`D1`/`D2`), in which case every component is a class name.
    Headers join nested names with `__`, matching `include/ActorBase__SceneNode.h`.

    Truncating to the first component filed all 41 `_ZN8Particle7Manager...`
    functions under `Particle` -- contaminating that class's evidence and starving
    17 nested headers of any at all.
    """
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
    if not parts:
        return None
    # a ctor/dtor terminator means the trailing component is still a class
    if not (i < len(sym) and sym[i] in "CD"):
        parts = parts[:-1]
    return "__".join(parts) if parts else None


def find_extracted(root):
    """Locate extracted/.

    It is gitignored, so a fresh worktree does not have one and must borrow the
    primary checkout's. Search the worktree, then this file's own repo, then
    $SM64DS_EXTRACTED -- never a hardcoded developer path.
    """
    cands = [root / "extracted",
             pathlib.Path(__file__).resolve().parents[1] / "extracted"]
    env = os.environ.get("SM64DS_EXTRACTED")
    if env:
        cands.append(pathlib.Path(env))
    # A linked worktree shares the main checkout's .git, so git itself can point at
    # the one place extracted/ is likely to live.
    try:
        common = subprocess.run(["git", "rev-parse", "--path-format=absolute",
                                 "--git-common-dir"], cwd=root, capture_output=True,
                                text=True, timeout=10)
        if common.returncode == 0 and common.stdout.strip():
            cands.append(pathlib.Path(common.stdout.strip()).parent / "extracted")
    except (OSError, subprocess.SubprocessError):
        pass
    for cand in cands:
        if (cand / "dsd/arm9_overlays/overlays.yaml").exists():
            return cand
    sys.exit("could not locate extracted/ -- set SM64DS_EXTRACTED or see "
             "notes/rom-build.md (--extracted also works)")


def coerce_addr(v):
    """overlays.yaml gives hex strings for some overlays and ints for others."""
    return v if isinstance(v, int) else int(str(v), 16)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".", help="repo/worktree root")
    ap.add_argument("--extracted", default=None)
    ap.add_argument("-o", "--out", default="build/evidence_rom.json")
    ap.add_argument("--report", action="store_true")
    args = ap.parse_args()

    root = pathlib.Path(args.root).resolve()
    ext = pathlib.Path(args.extracted) if args.extracted else find_extracted(root)

    # ---- the classes we care about: those with a bannered header
    classes = {h.stem for h in (root / "include").glob("*.h")
               if BANNER in h.read_text(errors="replace")}

    # ---- module -> (blob path, load address)
    # arm9_dec.bin loads at 0x02004000, not at the ARM9 RAM base. Every other tool in
    # the tree already knew this (match.py, modules.py, overlay_residency.py); this one
    # did not, and decoded every arm9 record from bytes 0x4000 away. It reported those
    # with full confidence, because garbage still disassembles.
    bases = {"arm9": (ext / "arm9_dec.bin", ARM9_BASE)}
    ovl = yaml.safe_load((ext / "dsd/arm9_overlays/overlays.yaml").read_text())
    for o in ovl["overlays"]:
        bases[f"ov{o['id']:03d}"] = (ext / "overlays" / f"overlay_{o['id']:04d}.bin",
                                     coerce_addr(o["base_address"]))

    # ---- functions attributable to one of those classes
    funcs = []
    for st in (root / "config" / "arm9").rglob("symbols.txt"):
        mod = st.parent.name
        if mod not in bases:
            continue
        for line in st.read_text(errors="replace").splitlines():
            m = SYM.match(line)
            if not m:
                continue
            cls = class_of(m.group(1))
            if cls in classes:
                funcs.append((cls, m.group(1), mod,
                              int(m.group(3), 16), int(m.group(2), 16)))

    md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
    md.detail = True

    blobs = {}

    # --- fail closed on a wrong load address, per module, before decoding anything
    rates = {}
    by_mod = collections.defaultdict(list)
    for _c, _s, mod, addr, _sz in funcs:
        by_mod[mod].append(addr)
    bad = []
    for mod, addrs in sorted(by_mod.items()):
        path, base = bases[mod]
        if not path.exists():
            continue
        rate, seen = prologue_rate(md, path.read_bytes(), base, addrs)
        rates[f"prologue_rate_{mod}"] = round(rate, 3)
        if seen >= 20 and rate < MIN_PROLOGUE_RATE:
            bad.append(f"{mod}: {rate:.1%} of {seen} symbols open with push/stmdb "
                       f"at base 0x{base:08x}")
    if bad:
        sys.exit("refusing to report: load address looks wrong for\n  "
                 + "\n  ".join(bad)
                 + f"\n(expected >{MIN_PROLOGUE_RATE:.0%}; a wrong base decodes the "
                   "wrong bytes and still produces a confident report)")

    def blob(mod):
        if mod not in blobs:
            p, b = bases[mod]
            blobs[mod] = (p.read_bytes() if p.exists() else None, b)
        return blobs[mod]

    ev = collections.defaultdict(lambda: collections.defaultdict(
        lambda: {"widths": collections.Counter(), "signed": collections.Counter(),
                 "address_only": 0, "provenance": [], "funcs": set()}))
    r = collections.Counter()
    r.update(rates)
    r["classes_targeted"] = len(classes)
    r["functions_attributed"] = len(funcs)

    for cls, sym, mod, addr, size in funcs:
        data, base = blob(mod)
        if data is None:
            r["func_blob_missing"] += 1
            continue
        off = addr - base
        if off < 0 or off + size > len(data):
            r["func_out_of_range"] += 1
            continue
        r["functions_disassembled"] += 1
        tracked = {0}          # registers provably still holding entry r0
        derived = {}           # reg -> byte offset, from `add rX, this, #imm`

        for insn in md.disasm(data[off:off + size], addr):
            mnem = insn.mnemonic.lower()
            op = next((m for m in MNEMONICS if mnem.startswith(m)), None)

            if op and insn.operands:
                memop = next((o for o in insn.operands if o.type == ARM_OP_MEM), None)
                if memop is not None:
                    r["mem_total"] += 1
                    bnum = reg_num(insn.reg_name(memop.mem.base))
                    is_load = op in LOADS
                    width, sign = (LOADS if is_load else STORES)[op]
                    if bnum == 13:
                        r["mem_stack"] += 1
                    elif bnum == 15:
                        r["mem_pcrel"] += 1
                    elif memop.mem.index != 0 or insn.writeback:
                        # register-indexed or writeback: offset is not a constant
                        r["mem_nonconstant_offset"] += 1
                    elif bnum in tracked or bnum in derived:
                        disp = memop.mem.disp + derived.get(bnum, 0)
                        if disp < 0:
                            r["mem_negative_disp"] += 1
                            continue
                        r["mem_this"] += 1
                        if bnum in derived:
                            r["mem_via_derived_base"] += 1
                        e = ev[cls][disp]
                        e["widths"][width] += 1
                        e["funcs"].add(sym)
                        # Stores never prove signedness -- there is no strsh. Only a
                        # sub-word LOAD does, and ldrb/ldrh are the weak direction.
                        if is_load and sign:
                            e["signed"][sign] += 1
                        if len(e["provenance"]) < 5:
                            e["provenance"].append(f"{mod}:0x{insn.address:08x} {mnem}")
                    else:
                        r["mem_unproven_base"] += 1

            # --- propagate / kill the tracked set
            handled = False
            if mnem == "mov" and len(insn.operands) == 2 \
                    and insn.operands[0].type == ARM_OP_REG \
                    and insn.operands[1].type == ARM_OP_REG:
                d = reg_num(insn.reg_name(insn.operands[0].reg))
                s = reg_num(insn.reg_name(insn.operands[1].reg))
                if s in tracked:
                    tracked.add(d)
                    derived.pop(d, None)
                    handled = True
                elif s in derived:
                    derived[d] = derived[s]
                    tracked.discard(d)
                    handled = True

            # `add rX, this, #imm` is the address OF a member -- typed evidence a
            # scalar load cannot give. Recorded, with the offset rebased.
            if not handled and mnem == "add" and len(insn.operands) == 3 \
                    and insn.operands[1].type == ARM_OP_REG:
                s = reg_num(insn.reg_name(insn.operands[1].reg))
                d = reg_num(insn.reg_name(insn.operands[0].reg))
                imm = insn.operands[2]
                if s in tracked and imm.type != ARM_OP_REG:
                    disp = imm.imm + derived.get(s, 0)
                    if disp >= 0:
                        r["addr_of_member"] += 1
                        ev[cls][disp]["address_only"] += 1
                        ev[cls][disp]["funcs"].add(sym)
                        derived[d] = disp
                        tracked.discard(d)
                        handled = True

            if not handled:
                # Match on instruction id, not a string prefix: `startswith("bl")`
                # also catches `ble`/`bls`/`blt`, which are plain branches and do not
                # clobber anything.
                if insn.id in (ARM_INS_BL, ARM_INS_BLX):
                    for k in (0, 1, 2, 3, 12, 14):
                        tracked.discard(k)
                        derived.pop(k, None)
                written = set()
                for reg in insn.regs_access()[1]:
                    written.add(reg_num(insn.reg_name(reg)))
                tracked -= written
                for k in written:
                    derived.pop(k, None)

            # An unconditional return ends the body. A symbol's byte range can hold a
            # literal pool, or run into a neighbouring function, and carrying entry
            # state across that manufactures facts about the wrong code.
            if is_unconditional_return(insn):
                tracked.clear()
                derived.clear()
                r["returns_seen"] += 1

    out = {
        "meta": {
            "pass": "rom",
            "note": "verifier and tie-breaker; never the generator. See plan 3.",
            "recall": dict(sorted(r.items())),
        },
        "classes": {
            cls: {hex(o): {"widths": {str(k): v for k, v in sorted(d["widths"].items())},
                           "signed": {k: d["signed"].get(k, 0) for k in ("s", "u")},
                           "address_only": d["address_only"],
                           "functions": len(d["funcs"]),
                           "provenance": d["provenance"]}
                  for o, d in sorted(offs.items())}
            for cls, offs in sorted(ev.items())
        },
    }

    dest = root / args.out
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(json.dumps(out, indent=1), encoding="utf-8")

    mt = max(1, r["mem_total"])
    print(f"classes with a bannered header : {r['classes_targeted']}")
    print(f"functions attributed           : {r['functions_attributed']}")
    print(f"  disassembled                 : {r['functions_disassembled']}")
    print(f"memory accesses seen           : {r['mem_total']}")
    print(f"  provably this-based    KEPT  : {r['mem_this']}  ({100*r['mem_this']/mt:.1f}%)")
    print(f"    of which via add-derived   : {r['mem_via_derived_base']}")
    print(f"  stack                  drop  : {r['mem_stack']}  ({100*r['mem_stack']/mt:.1f}%)")
    print(f"  pc-relative            drop  : {r['mem_pcrel']}  ({100*r['mem_pcrel']/mt:.1f}%)")
    print(f"  unproven base          drop  : {r['mem_unproven_base']}  ({100*r['mem_unproven_base']/mt:.1f}%)")
    print(f"  non-constant offset    drop  : {r['mem_nonconstant_offset']}")
    print(f"address-of-member (add)        : {r['addr_of_member']}")
    print(f"classes with evidence          : {len(out['classes'])}")
    print(f"distinct offsets               : {sum(len(v) for v in out['classes'].values())}")
    ndist = sum(len(v) for v in out["classes"].values())
    single = sum(1 for offs in out["classes"].values()
                 for d in offs.values() if d["functions"] == 1)
    print(f"offsets resting on ONE function: {single}  ({100*single/max(1,ndist):.0f}%)")
    print(f"\nwrote {args.out}")
    print()
    print("BLIND SPOTS: ldm/stm and vfp accesses are not decoded; a field only ever "
          "reached through a register-indexed or writeback address is not recorded.")
    print("STATICS: a static member function takes its first argument in r0, so this "
          "pass reads it as `this`, and nothing in the mangled name distinguishes one. "
          "The per-offset `functions` count is the mitigation -- a fact contributed by "
          "a single function is the shape that risk takes.")


if __name__ == "__main__":
    main()
