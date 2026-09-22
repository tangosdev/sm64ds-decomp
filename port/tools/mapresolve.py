#!/usr/bin/env python
"""Resolve `walk_window+0xRVA` return addresses against the linker map.

There is no PDB in this build, so a hang dump's frames come back as raw module
offsets. The map's "Publics by Value" table carries every public symbol's
Rva+Base, so the nearest symbol AT OR BELOW an address names the function the
frame is in -- with the offset into it, so a wrong answer is visible rather
than quiet.

Usage: mapresolve.py <map> <addr> [addr ...]      addr as 0x14c414 (an RVA) or
                                                  walk_window+0x14c414
"""
import re
import sys


def load(path):
    syms = []
    base = 0x400000
    started = False
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            if "Preferred load address is" in line:
                base = int(line.split()[-1], 16)
            if "Publics by Value" in line:
                started = True
                continue
            if not started:
                continue
            m = re.match(r"\s+([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s*(.*)",
                         line)
            if not m:
                continue
            name = m.group(3)
            rvab = int(m.group(4), 16)
            lib = m.group(5).strip()
            syms.append((rvab - base, name, lib))
    syms.sort()
    return syms, base


def resolve(syms, rva):
    lo, hi = 0, len(syms) - 1
    best = None
    while lo <= hi:
        mid = (lo + hi) // 2
        if syms[mid][0] <= rva:
            best = syms[mid]
            lo = mid + 1
        else:
            hi = mid - 1
    if best is None:
        return "?"
    return "%s+0x%x   [%s]" % (best[1], rva - best[0], best[2])


def main():
    syms, base = load(sys.argv[1])
    for a in sys.argv[2:]:
        a = a.split("+")[-1] if "+" in a else a
        rva = int(a, 16)
        if rva >= base:
            rva -= base
        print("0x%08x  %s" % (rva, resolve(syms, rva)))


if __name__ == "__main__":
    main()
