#!/usr/bin/env python3
"""Check the game card model's served blocks against a real cartridge image.

ntr/card.cpp answers the ROM card driver's B7h commands out of a VIRTUAL ROM
image (hal/fs_names.cpp's port_nitrofs_rom_read): the FNT, the FAT and the ARM9
overlay table the asset catalog cuts out of the cartridge, every FAT-ranged file
out of extracted/dsd/files, and 0xFF wherever none of those lie. The port never
opens the player's .nds at run time. This is the dev-box check that the image
it assembles is the cartridge, on every block a run actually read.

A run made with SM64DS_CARD_TRACE=1 prints one line per block,

    [card] B7 <rom address> +<bytes> fnv <FNV-1a 64 of the bytes served>

and this reads those lines out of any number of logs (a run's stdout, its
flight-recorder playlog, a sweep's per-row output), dedupes them, and for each
block reads the same span out of the .nds (a block never leaves its 4-KB page:
GBATEK's wrap, which ntr/card.cpp applies too; an address below 0x8000 reads
0x8000 + (addr & 0x1FF)) and compares digests. A block that differs is listed
against the sources the virtual image lays under it: FNT, FAT, OVT (the
overlay table), a FAT file id, or GAP (0xFF filler), so the reader knows which
source to open. (A digest cannot say which byte differs.)

    python port/tools/card_image_check.py --nds <cartridge.nds> <log> [<log> ...]
    python port/tools/card_image_check.py --nds <cartridge.nds> --dir <folder>

--dir walks a folder for *.txt / *.log. --root names the repo whose
build/assets describe the image (default: the one holding this file), used
only to label a differing byte. Exit 0 when every block matches, 1 when any
differs, 2 on bad input. The .nds is read, never written, and its path is an
argument: nothing about it is kept.
"""

import argparse
import os
import re
import struct
import sys

LINE = re.compile(r"\[card\] B7 ([0-9a-fA-F]{8}) \+([0-9a-fA-F]+) fnv ([0-9a-fA-F]{16})")


def fnv1a64(data):
    h = 0xcbf29ce484222325
    for b in data:
        h ^= b
        h = (h * 0x100000001b3) & 0xFFFFFFFFFFFFFFFF
    return h


def block_bytes(rom, addr, size):
    """The cartridge's answer to B7h at `addr` for `size` bytes (ntr/card.cpp's rule)."""
    if addr < 0x8000:
        addr = 0x8000 + (addr & 0x1FF)
    page, off, out = addr & ~0xFFF, addr & 0xFFF, bytearray()
    while len(out) < size:
        n = min(size - len(out), 0x1000 - off)
        chunk = rom[page + off:page + off + n]
        out += chunk + b"\xff" * (n - len(chunk))   # past the end of the dump
        off = 0
    return bytes(out)


class Labels:
    """What the virtual image puts at a ROM offset, for naming a differing byte."""

    def __init__(self, root):
        self.spans = []
        tsv = os.path.join(root, "build", "assets", "nitrofs.tsv")
        words = {}
        try:
            with open(tsv, encoding="utf-8") as f:
                next(f, None)
                for line in f:
                    k, _, v = line.strip().partition("\t")
                    if v.isdigit():
                        words[k] = int(v)
        except OSError:
            return
        for name, key in (("FNT", "fnt"), ("FAT", "fat"), ("OVT", "ovt9")):
            if words.get(key + "_size"):
                self.spans.append((words[key + "_offset"],
                                   words[key + "_offset"] + words[key + "_size"], name))
        fat = os.path.join(root, "build", "assets", "nitrofs_fat.bin")
        try:
            blob = open(fat, "rb").read()
        except OSError:
            return
        for i in range(len(blob) // 8):
            lo, hi = struct.unpack_from("<II", blob, i * 8)
            if hi > lo:
                self.spans.append((lo, hi, "file %d" % i))

    def at(self, off):
        for lo, hi, name in self.spans:
            if lo <= off < hi:
                return name
        return "GAP"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--nds", required=True, help="the cartridge image to compare against")
    ap.add_argument("--root", default=os.path.dirname(os.path.dirname(
        os.path.dirname(os.path.abspath(__file__)))))
    ap.add_argument("--dir", action="append", default=[], help="walk a folder for logs")
    ap.add_argument("logs", nargs="*")
    a = ap.parse_args()

    try:
        rom = open(a.nds, "rb").read()
    except OSError as e:
        print("card_image_check: cannot read the cartridge image: %s" % e)
        return 2
    logs = list(a.logs)
    for d in a.dir:
        for dp, _, fns in os.walk(d):
            logs += [os.path.join(dp, fn) for fn in fns if fn.endswith((".txt", ".log"))]
    if not logs:
        print("card_image_check: no logs given")
        return 2

    seen, lines = {}, 0
    for p in logs:
        try:
            text = open(p, encoding="utf-8", errors="replace").read()
        except OSError:
            continue
        for m in LINE.finditer(text):
            lines += 1
            key = (int(m.group(1), 16), int(m.group(2), 16))
            seen.setdefault(key, set()).add(int(m.group(3), 16))

    labels = Labels(a.root)
    bad = 0
    for (addr, size), digests in sorted(seen.items()):
        want = block_bytes(rom, addr, size)
        wd = fnv1a64(want)
        if digests == {wd}:
            continue
        bad += 1
        print("DIFFERS  B7 %08x +%x: served %s, cartridge %016x" % (
            addr, size, " ".join("%016x" % d for d in sorted(digests)), wd))
        if len(digests) > 1:
            print("         (the same block was served with %d different digests)" % len(digests))
        base = 0x8000 + (addr & 0x1FF) if addr < 0x8000 else addr
        # which bytes differ cannot be recovered from a digest; name what lies
        # under the block instead, so the reader knows which source to open
        names = []
        for off in range(base, base + size):
            n = labels.at(off)
            if not names or names[-1][2] != n:
                names.append([off, off + 1, n])
            else:
                names[-1][1] = off + 1
        for lo, hi, n in names:
            print("         %08x..%08x  %s" % (lo, hi, n))
    print("card_image_check: %d block line(s) in %d log(s), %d distinct block(s); "
          "%d identical to the cartridge, %d differ" % (
              lines, len(logs), len(seen), len(seen) - bad, bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
