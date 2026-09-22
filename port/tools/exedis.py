#!/usr/bin/env python
"""Disassemble a range of walk_window.exe by RVA, straight out of the file.

There is no PDB in this build, so a question about what offset a compiled
method actually uses for a field cannot be answered by reading the header --
the header is the ROM's layout and MSVC lays the host object out its own way.
This reads the instruction stream instead, which is the thing that runs.

Usage: exedis.py <exe> <rva-or-va> [count]
"""
import struct
import sys

import capstone


def sections(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    optsz = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    off = pe + 24 + optsz
    out = []
    for i in range(nsec):
        s = off + i * 40
        vaddr = struct.unpack_from("<I", data, s + 12)[0]
        vsize = struct.unpack_from("<I", data, s + 8)[0]
        praw = struct.unpack_from("<I", data, s + 20)[0]
        out.append((vaddr, vsize, praw))
    return out, base


def main():
    data = open(sys.argv[1], "rb").read()
    secs, base = sections(data)
    a = int(sys.argv[2], 16)
    if a >= base:
        a -= base
    n = int(sys.argv[3]) if len(sys.argv) > 3 else 120
    for vaddr, vsize, praw in secs:
        if vaddr <= a < vaddr + vsize:
            fo = praw + (a - vaddr)
            md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
            for ins in md.disasm(data[fo:fo + n * 8], base + a):
                print("%08x  %-22s %s %s"
                      % (ins.address, ins.bytes.hex(), ins.mnemonic, ins.op_str))
                n -= 1
                if n <= 0:
                    return
            return
    print("rva 0x%x not in any section" % a)


if __name__ == "__main__":
    main()
