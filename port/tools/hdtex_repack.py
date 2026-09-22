#!/usr/bin/env python3
"""Rename a foreign HD texture pack to the names this port's loader asks for.

    python port/tools/hdtex_repack.py <old_pack> <old_dump> <port_dump> <out>

The loader finds a replacement by content hash, so a pack authored against a
different hash is invisible here even though every file in it is the right
picture. This tool bridges that by MATCHING PICTURES rather than names:

  old_dump   the foreign edition's own dump: its names, its textures at
             native DS dimensions
  port_dump  this port's dump (SM64DS_HD_TEXTURES_DUMP), same textures, the
             names the loader wants
  old_pack   the foreign edition's upscaled art, named like old_dump
  out        the same art, named like port_dump

Pictures are compared at 5 bits per channel plus a transparent/not alpha bit,
because two decoders of the same DS texture legitimately widen 5-bit colour
and 3- or 5-bit alpha differently.

This is the FALLBACK. port/ntr/hdtex.cpp reproduces the parked tangOS
edition's hash bit for bit, so that edition's 206 PNGs already load unrenamed
and need nothing from this file. It exists for the next pack that does not.
"""

import argparse
import shutil
import sys
from pathlib import Path

import numpy as np
from PIL import Image


def fingerprint(path):
    im = Image.open(path).convert("RGBA")
    a = np.frombuffer(im.tobytes(), dtype=np.uint8).reshape(-1, 4).copy()
    q = (a[:, :3] >> 3).astype(np.uint8)
    al = (a[:, 3:4] >= 128).astype(np.uint8)
    return (im.width, im.height, np.hstack([q, al]).tobytes())


def index(d, label):
    p = Path(d)
    if not p.is_dir():
        sys.exit("no such directory (%s): %s" % (label, d))
    out = {}
    for f in sorted(p.glob("*.png")):
        if len(f.stem) == 16:
            out[f.stem.lower()] = fingerprint(f)
    if not out:
        sys.exit("no hash-named PNG files in %s (%s)" % (d, label))
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("old_pack")
    ap.add_argument("old_dump")
    ap.add_argument("port_dump")
    ap.add_argument("out")
    a = ap.parse_args(argv)

    old = index(a.old_dump, "old_dump")
    new = index(a.port_dump, "port_dump")
    out = Path(a.out)
    out.mkdir(parents=True, exist_ok=True)

    by_pic = {}
    for name, fp in old.items():
        by_pic.setdefault(fp, []).append(name)

    copied = same = missing_art = unmatched = 0
    for name, fp in sorted(new.items()):
        hit = by_pic.get(fp)
        if not hit:
            unmatched += 1
            continue
        src = Path(a.old_pack) / (hit[0] + ".png")
        if not src.exists():
            missing_art += 1
            continue
        if hit[0] == name:
            same += 1
        shutil.copyfile(src, out / (name + ".png"))
        copied += 1

    print("old dump   : %d" % len(old))
    print("port dump  : %d" % len(new))
    print("copied     : %d  -> %s/" % (copied, out))
    print("  of which the name was already right : %d" % same)
    print("art missing for a matched picture     : %d" % missing_art)
    print("port textures with no match in the old dump : %d" % unmatched)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
