"""The gap's picture set: one BMP per (scene, fill, peek) into an output dir.

Named so a reader can tell them apart without opening them:

    sXXX_G<n>_<what>.bmp

    nogap      MinigameGap false -- the picture before this feature
    ambient    the default fill
    solid      a flat colour, so the band's extent is unmistakable
    peekon     ambient, with the band's own hidden sprites over it
    peekoff    ambient, without them (the pair for peekon)

`python tools/gapshots.py <outdir>`.
"""
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gapproof as G

GAP = {366: 48, 368: 32, 374: 32, 376: 48, 378: 32, 390: 32}

SHOTS = [
    ("nogap",   {"MinigameGap": False}),
    ("ambient", {"GapFillMode": "ambient"}),
    ("solid",   {"GapFillMode": "solid", "GapColor": "#1e6fb8"}),
    ("peekoff", {"GapFillMode": "ambient", "GapPeek": False}),
    ("peekon",  {"GapFillMode": "ambient", "GapPeek": True}),
]


def main(outdir, scenes):
    os.makedirs(outdir, exist_ok=True)
    for sc in scenes:
        for tag, settings in SHOTS:
            name = "s%d_G%d_%s.bmp" % (sc, GAP[sc], tag)
            tmp = os.path.join(G.BUILD, "gapshot_tmp.bmp")
            if os.path.exists(tmp):
                os.remove(tmp)
            r = G.scene_run(sc, 300, tmp, settings=settings)
            if r.returncode or not os.path.exists(tmp):
                print("%-28s FAILED rc=%d" % (name, r.returncode))
                continue
            w, h, _ = G.read_bmp(tmp)
            shutil.move(tmp, os.path.join(outdir, name))
            print("%-28s %dx%d" % (name, w, h))


if __name__ == "__main__":
    out = sys.argv[1]
    sc = [int(a) for a in sys.argv[2:]] or [368, 374, 376]
    main(out, sc)
