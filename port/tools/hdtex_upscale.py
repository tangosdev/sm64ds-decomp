#!/usr/bin/env python3
"""Build an HD texture pack from a texture dump.

    python port/tools/hdtex_upscale.py <dump_dir> <out_dir> [--scale N]
                                       [--mode scale4x|nearest] [--force]

Two modes, for two different jobs.

  --mode scale4x (default, --scale 4)
      scale2x applied twice: edge-preserving pixel-art scaling. This is the
      baseline pack carried over from the parked tangOS edition's
      tools/upscale_pack.py, unchanged in what it computes. Cleaner edges than
      the cartridge art, not a remaster; the slots accept better art later.
      --scale 2 runs one scale2x pass.

  --mode nearest (--scale N, 1..8)
      Every pixel repeated N times in both directions. This is the IDENTITY
      PACK used to prove the loader: nearest sampling of a nearest-upscaled
      texture through scaled texel coordinates picks the same colour, so a
      capture with the pack on must be byte-identical to a capture with it
      off. Any differing pixel is a bug in scale handling, not in the art.

Pure PIL + numpy, both of which are already installed on this machine. It
installs nothing and downloads nothing.
"""

import argparse
import sys
from pathlib import Path

import numpy as np
from PIL import Image


def scale2x(px):
    """px: HxW uint32 (RGBA packed). Returns 2H x 2W. The parked tool's rule."""
    h, w = px.shape
    B = np.vstack([px[:1], px[:-1]])         # up
    H = np.vstack([px[1:], px[-1:]])         # down
    D = np.hstack([px[:, :1], px[:, :-1]])   # left
    F = np.hstack([px[:, 1:], px[:, -1:]])   # right

    e0 = np.where((D == B) & (B != F) & (D != H), D, px)
    e1 = np.where((B == F) & (B != D) & (F != H), F, px)
    e2 = np.where((D == H) & (D != B) & (H != F), D, px)
    e3 = np.where((H == F) & (D != H) & (B != F), F, px)

    out = np.empty((h * 2, w * 2), dtype=np.uint32)
    out[0::2, 0::2] = e0
    out[0::2, 1::2] = e1
    out[1::2, 0::2] = e2
    out[1::2, 1::2] = e3
    return out


def nearest(px, n):
    return np.repeat(np.repeat(px, n, axis=0), n, axis=1)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("dump_dir")
    ap.add_argument("out_dir")
    ap.add_argument("--scale", type=int, default=4)
    ap.add_argument("--mode", choices=("scale4x", "nearest"), default="scale4x")
    ap.add_argument("--force", action="store_true",
                    help="rewrite files that already exist in out_dir")
    a = ap.parse_args(argv)

    dump = Path(a.dump_dir)
    out = Path(a.out_dir)
    if not dump.is_dir():
        sys.exit("no such dump directory: %s" % dump)
    if a.mode == "scale4x" and a.scale not in (2, 4):
        sys.exit("scale4x mode does one or two scale2x passes: --scale 2 or 4")
    if a.mode == "nearest" and not 1 <= a.scale <= 8:
        sys.exit("nearest mode takes --scale 1..8 (the loader clamps there)")
    out.mkdir(parents=True, exist_ok=True)

    files = sorted(dump.glob("*.png"))
    if not files:
        sys.exit("no PNG files in %s" % dump)

    done = skipped = 0
    for f in files:
        dst = out / f.name
        if dst.exists() and not a.force:
            skipped += 1
            continue
        img = Image.open(f).convert("RGBA")
        px = np.frombuffer(img.tobytes(), dtype=np.uint32).reshape(
            img.height, img.width).copy()
        if a.mode == "nearest":
            px = nearest(px, a.scale)
        else:
            px = scale2x(px)
            if a.scale == 4:
                px = scale2x(px)
        hd = Image.frombuffer("RGBA", (px.shape[1], px.shape[0]),
                              px.tobytes(), "raw", "RGBA", 0, 1)
        hd.save(dst)
        done += 1
        if done % 100 == 0:
            print("  %d/%d" % (done, len(files)))

    print("pack: %d written, %d already present, %d total -> %s/ "
          "(mode %s, scale %d)"
          % (done, skipped, len(files), out, a.mode, a.scale))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
