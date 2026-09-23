#!/usr/bin/env python3
"""R2 census from the linked map: every address where a NARROW-return decorated
spelling and a WIDE-return decorated spelling (or a flat C name) share one body.

MSVC decorates a C++ function's return type (after the calling-convention
letter), and an /alternatename resolves an undefined spelling onto the target's
address, so the map lists the alias at the target's address and object. Two
spellings at one address that disagree about the return width are one body read
two ways.

usage: python port/tools/narrowret_map_routes.py build/port/walk_window.map"""
import re, sys, collections
mp = sys.argv[1]
NARROW = {"C": "s8", "D": "char", "E": "u8", "F": "s16", "G": "u16", "_N": "bool"}
WIDE = {"H": "int", "I": "u32", "J": "long", "K": "ulong"}
def retcode(dec):
    # ?name@scope...@@<fn-type>: free fn Y[AGI], member [A-Z][A-Z]... ; return type after it.
    m = re.match(r"\?[^?@][^@]*@(?:[^@]+@)*@(?:Y[AGI]|[A-Z](?:[A-Z]|[0-9]))(?:[A-Z]{0,2}?)", dec)
    # simpler: find the '@@' then the function-type letters
    i = dec.find("@@")
    if i < 0:
        return None
    rest = dec[i+2:]
    if rest.startswith("Y"):          # free function: Y + cc letter
        rest = rest[2:]
    elif rest[:1] in "AEIMQUCKSOBJRV" and len(rest) > 2:   # member: access + (this-cv) + cc
        # access letter; static members (C,D,K,L,S,T) have no this-cv
        a = rest[0]
        if a in "CDKLST":
            rest = rest[2:]
        else:
            rest = rest[3:]
    else:
        return None
    if rest.startswith("_N"):
        return "_N"
    return rest[:1]
by_addr = collections.defaultdict(list)
for line in open(mp, encoding="utf-8", errors="replace"):
    m = re.match(r"\s*0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+f\s+(?:i\s+)?(\S+)\s*$", line)
    if m:
        by_addr[m.group(2)].append((m.group(1), m.group(3)))
n = 0
for addr, syms in sorted(by_addr.items()):
    codes = [(s, o, retcode(s) if s.startswith("?") else "flat") for s, o in syms]
    nar = [c for c in codes if c[2] in NARROW]
    wid = [c for c in codes if c[2] in WIDE]
    flat = [c for c in codes if c[2] == "flat"]
    if nar and (wid or flat):
        n += 1
        print("%s  narrow: %s" % (addr, "; ".join("%s[%s] (%s)" % (s, NARROW[c], o) for s, o, c in nar)))
        for s, o, c in wid:
            print("          WIDE  : %s[%s]" % (s, WIDE[c]))
        for s, o, c in flat:
            print("          flat  : %s (%s)" % (s, o))
print("addresses with a narrow spelling and a wide/flat spelling on one body:", n)
