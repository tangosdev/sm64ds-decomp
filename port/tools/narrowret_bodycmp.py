#!/usr/bin/env python3
"""Compare each narrow body's machine code between two builds.

usage: python port/tools/narrowret_bodycmp.py <base.exe> <base.map> <new.exe> <new.map> <rows.tsv>
rows.tsv: <label> <base symbol> <new symbol>   (tab separated)

Each body is disassembled (capstone, x86-32) from its map address to the next
public, trailing int3/nop padding dropped. Absolute operands inside the image
and call/jump targets are replaced by the symbol they name, so two builds that
only moved code compare equal. Prints SAME or DIFF per row and the first
differing instruction pair.
"""
import bisect
import re
import sys

import capstone
import pefile


def load_map(path):
    syms = []
    for line in open(path, encoding="utf-8", errors="replace"):
        m = re.match(r"\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+", line)
        if m:
            syms.append((int(m.group(2), 16), m.group(1)))
    syms.sort()
    by = {}
    for a, n in syms:
        by.setdefault(n, a)
    return syms, [s[0] for s in syms], by


def canon(n):
    n = n.replace("_hostgen_nrwide__ZN", "__ZN").replace("_hostgen_nrwide_", "_")
    return n


def name_at(syms, addrs, a):
    i = bisect.bisect_right(addrs, a) - 1
    if i < 0:
        return hex(a)
    base = syms[i][0]
    j = i
    names = []
    while j >= 0 and syms[j][0] == base:
        if not syms[j][1].startswith("."):
            names.append(canon(syms[j][1]))
        j -= 1
    n = min(names) if names else syms[i][1]
    return n if base == a else "%s+%x" % (n, a - base)


def body(pe, syms, addrs, by, name):
    if name not in by:
        return None
    a = by[name]
    i = bisect.bisect_right(addrs, a)
    while i < len(addrs) and addrs[i] == a:
        i += 1
    end = addrs[i] if i < len(addrs) else a + 0x400
    rva = a - pe.OPTIONAL_HEADER.ImageBase
    data = pe.get_data(rva, min(end - a, 0x2000))
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    out = []
    for ins in md.disasm(data, a):
        op = ins.op_str

        def repl(mm):
            v = int(mm.group(0), 16)
            if a <= v < end:
                return "<self+%x>" % (v - a)
            if 0x400000 <= v < 0x2000000:
                return "<" + name_at(syms, addrs, v) + ">"
            return mm.group(0)
        op = re.sub(r"0x[0-9a-f]+", repl, op)
        out.append((ins.mnemonic + " " + op).strip())
    while out and (out[-1].startswith("int3") or out[-1].startswith("nop")):
        out.pop()
    return out


bexe, bmap, nexe, nmap, rows = sys.argv[1:6]
bpe, npe = pefile.PE(bexe, fast_load=True), pefile.PE(nexe, fast_load=True)
B, N = load_map(bmap), load_map(nmap)
same = diff = miss = 0
for line in open(rows, encoding="utf-8"):
    if not line.strip() or line.startswith("#"):
        continue
    label, bs, ns = line.rstrip("\n").split("\t")
    x = body(bpe, *B, bs)
    y = body(npe, *N, ns)
    if x is None or y is None:
        miss += 1
        print("MISSING %-44s base=%s new=%s" % (label, x is not None, y is not None))
        continue
    # the renamed body's own name appears in its self-references; normalise it
    y = [s.replace(ns, bs) for s in y]
    if x == y:
        same += 1
        print("SAME    %-44s %d instructions" % (label, len(x)))
    else:
        diff += 1
        k = next((i for i in range(min(len(x), len(y))) if x[i] != y[i]), min(len(x), len(y)))
        print("DIFF    %-44s base %d / new %d instructions; first at %d: %r vs %r"
              % (label, len(x), len(y), k, x[k] if k < len(x) else None,
                 y[k] if k < len(y) else None))
print("bodies: %d SAME, %d DIFF, %d MISSING" % (same, diff, miss))
