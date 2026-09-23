#!/usr/bin/env python3
"""How does the caller read the result? Disassemble forward from a return address.

usage: python port/tools/narrowret_readwidth.py <walk_window.exe> <live.tsv> <out.tsv>

For each row of narrowret_live.py's table, walk the caller's instructions from
the return address (the instruction after the call) and follow the register the
result sits in (EAX, and any register it is copied to) until the value is
CONSUMED. Arithmetic whose low N bits depend only on the low N bits of its
inputs (add, sub, shl, imul, and, or, xor, neg, not, lea) keeps the junk above
bit N out of the low part, so it is followed rather than judged. The verdict:
  WIDE    the full register decides something: a 32-bit cmp/test, a right shift,
          a division, a 32-bit store, a 32-bit index
  NARROW  only the low part is used: a byte/word store, cmp/test of AL/AX,
          movzx/movsx/cwde/cbw of it, a mask that fits the width
  ARG     pushed as an argument (the callee's parameter width decides; read it)
  DEAD    overwritten before any use
  ?       a branch, call or return came first (not followed)
"""
import re
import sys

import capstone
import pefile

exe, live, outp = sys.argv[1:4]
pe = pefile.PE(exe, fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

R32 = ("eax", "ecx", "edx", "ebx", "esi", "edi", "ebp")
SUB = {"eax": ("ax", "al", "ah"), "ecx": ("cx", "cl", "ch"), "edx": ("dx", "dl", "dh"),
       "ebx": ("bx", "bl", "bh"), "esi": ("si",), "edi": ("di",), "ebp": ("bp",)}
KEEP = ("add", "sub", "shl", "sal", "imul", "and", "or", "xor", "neg", "not", "inc", "dec", "adc", "sbb")


def has(op, reg):
    return re.search(r"(?<![\w])%s(?![\w])" % reg, op) is not None


def classify(ra, kind):
    try:
        data = pe.get_data(ra - base, 256)
    except Exception:
        return "?", "unreadable"
    tracked = {"eax"}
    lines = []
    lim = 0xff if kind in ("u8", "s8", "bool") else 0xffff
    for ins in md.disasm(data, ra):
        mn, op = ins.mnemonic, ins.op_str
        lines.append(("%s %s" % (mn, op)).strip())
        txt = " ; ".join(lines)
        if mn == "call":
            # the callee clobbers EAX/ECX/EDX; a copy in a callee-saved
            # register survives the call and is followed past it
            tracked -= {"eax", "ecx", "edx"}
            if not tracked:
                return "DEAD", txt
            continue
        if mn in ("ret", "jmp") or mn.startswith("j") or mn.startswith("loop"):
            return "?", txt
        if mn in ("cwde", "cbw", "cwd", "cdq") and "eax" in tracked:
            return ("NARROW" if mn != "cdq" else "WIDE"), txt
        parts = [p.strip() for p in op.split(",")] if op else []
        dst = parts[0] if parts else ""
        srcs = parts[1:]
        hit = None
        for r in sorted(tracked):
            subs = SUB[r]
            in_dst_full, in_src_full = has(dst, r), any(has(s, r) for s in srcs)
            in_dst_low = any(has(dst, s) for s in subs)
            in_src_low = any(any(has(s, x) for x in subs) for s in srcs)
            if not (in_dst_full or in_src_full or in_dst_low or in_src_low):
                continue
            hit = r
            # memory operand that uses the register as an address or index
            if mn != "lea" and any("[" in s and (has(s, r)) for s in parts):
                if mn == "mov" and "[" in dst and srcs and srcs[0] == r:
                    return "WIDE", txt + "  (32-bit store)"
                if mn == "mov" and "[" in dst and srcs and srcs[0] in subs:
                    return "NARROW", txt + "  (narrow store)"
                return "WIDE", txt + "  (address/index)"
            if mn in ("movzx", "movsx") and in_src_low:
                return "NARROW", txt
            if mn == "mov" and srcs and srcs[0] in subs:
                return "NARROW", txt + ("  (narrow store)" if "[" in dst else "")
            if mn in ("test", "cmp"):
                return ("WIDE" if (in_dst_full or in_src_full) else "NARROW"), txt
            if mn == "push":
                return "ARG", txt
            if mn in ("shr", "sar", "div", "idiv", "mul", "bt", "bsr", "bsf", "sete", "setne"):
                if in_dst_full or in_src_full:
                    return "WIDE", txt
                return "NARROW", txt
            if mn == "and" and in_dst_full and srcs:
                m = re.match(r"(?:0x([0-9a-f]+)|(\d+))$", srcs[0])
                if m:
                    v = int(m.group(1), 16) if m.group(1) else int(m.group(2))
                    if v <= lim:
                        return "NARROW", txt + "  (masked)"
                continue
            if mn == "mov" and srcs and srcs[0] == r and dst in R32:
                tracked.add(dst)
                continue
            if mn == "mov" and srcs and srcs[0] == r:
                return "WIDE", txt
            if mn in KEEP or mn == "lea":
                if mn == "xor" and len(parts) == 2 and parts[0] == parts[1] == r:
                    tracked.discard(r)
                    continue
                if mn == "lea":
                    if in_src_full and dst in R32:
                        tracked.add(dst)      # an address computed from it
                    elif in_dst_full:
                        tracked.discard(r)    # overwritten by an unrelated address
                    continue
                if in_dst_full or in_dst_low:
                    continue          # result stays in r, low bits still clean
                # r used as a source into another register: follow that one
                if dst in R32:
                    tracked.add(dst)
                    continue
                return "WIDE", txt
            if in_dst_full and not in_src_full:
                tracked.discard(r)    # overwritten
                continue
        if not tracked:
            return "DEAD", " ; ".join(lines)
        if len(lines) >= 40:
            break
    return "?", " ; ".join(lines)


rows = open(live, encoding="utf-8").read().splitlines()
out = [rows[0] + "\tread\tafter_call"]
n = {}
for line in rows[1:]:
    c = line.split("\t")
    ra = int(c[5], 16)
    v, dis = classify(ra, c[1])
    out.append(line + "\t" + v + "\t" + dis[:300])
    if int(c[7]) > 0:
        n[v] = n.get(v, 0) + 1
open(outp, "w", encoding="utf-8").write("\n".join(out) + "\n")
print("sites with raw_bad > 0 by caller read:", n)
