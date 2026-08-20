"""The gap's picture set: one BMP per (scene, fill, peek) into an output dir.

Named so a reader can tell them apart without opening them:

    sXXX_G<n>_<what>.bmp

    nogap      MinigameGap false -- the picture before this feature
    ambient    the default fill
    solid      a flat colour, so the band's extent is unmistakable
    peekon     the band's own hidden sprites on a plain black backdrop.
               PEEK OVERRIDES THE FILL, so this row's GapFillMode is ignored
               by design and the backdrop is black rather than the ambient
               wash; see hal/host_settings.cpp.
    peekoff    the same scene with peek off, which is the ambient fill again
               (the pair for peekon)

`python tools/gapshots.py <outdir>`.
"""
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gapproof as G

SHEET_SUFFIX = os.environ.get("GAPSHEET_SUFFIX", "")

GAP = {366: 48, 368: 32, 374: 32, 376: 48, 378: 32, 390: 32}

SHOTS = [
    # NO settings.json at all, which is what a player who never opens the
    # launcher's settings gets. It has to be in the set: every other row here
    # proves a key, and this one proves the defaults.
    ("default", None),
    ("nogap",   {"MinigameGap": False}),
    ("ambient", {"GapFillMode": "ambient"}),
    ("solid",   {"GapFillMode": "solid", "GapColor": "#1e6fb8"}),
    ("peekoff", {"GapFillMode": "ambient", "GapPeek": False}),
    ("peekon",  {"GapFillMode": "ambient", "GapPeek": True}),
]


def to_png(bmp, png):
    """A PNG beside the BMP when Pillow is here, because a 1.3 MB BMP per shot
    is a set nobody wants to open one at a time. The BMP is what the program
    wrote and stays the artifact of record; the PNG is a convenience."""
    try:
        from PIL import Image
    except ImportError:
        return False
    Image.open(bmp).save(png)
    return True


def seam_sheet(outdir, sc, g, suffix=""):
    """One PNG per scene showing the seam four ways, side by side.

    A full 512x864 capture is mostly two screens nobody is judging; the
    question is what the sixty rows either side of the seam look like. This
    crops exactly that band out of nogap, ambient, solid and peek-on and lays
    them left to right in that order, which is the picture to actually look at.
    Every crop is the same rows of the same frame, so a difference between two
    of them is the setting and nothing else."""
    try:
        from PIL import Image
    except ImportError:
        return None
    margin = 60
    tags = ["nogap", "ambient", "solid", "peekon"]
    crops = []
    for tag in tags:
        p = os.path.join(outdir, "s%d_G%d_%s.bmp" % (sc, g, tag))
        if not os.path.exists(p):
            return None
        im = Image.open(p).convert("RGB")
        band = im.height - 768                  # 0 for nogap, 2G otherwise
        top = 384 - margin
        bot = 384 + band + margin
        crops.append(im.crop((0, top, im.width, bot)))
    hmax = max(c.height for c in crops)
    pad = 8
    sheet = Image.new("RGB",
                      (sum(c.width for c in crops) + pad * (len(crops) - 1),
                       hmax), (24, 24, 24))
    x = 0
    for c in crops:
        sheet.paste(c, (x, (hmax - c.height) // 2))
        x += c.width + pad
    # the suffix keeps an earlier sheet beside a new one, which is the
    # whole point when the change under review is what one panel looks like
    out = os.path.join(outdir,
                       "s%d_G%d_seam_nogap-ambient-solid-peek%s.png"
                       % (sc, g, suffix))
    sheet.save(out)
    return out


def main(outdir, scenes):
    os.makedirs(outdir, exist_ok=True)
    for sc in scenes:
        for tag, settings in SHOTS:
            name = "s%d_G%d_%s" % (sc, GAP[sc], tag)
            tmp = os.path.join(G.BUILD, "gapshot_tmp.bmp")
            if os.path.exists(tmp):
                os.remove(tmp)
            r = G.scene_run(sc, 300, tmp, settings=settings)
            if r.returncode or not os.path.exists(tmp):
                print("%-28s FAILED rc=%d" % (name, r.returncode))
                continue
            w, h, _ = G.read_bmp(tmp)
            dst = os.path.join(outdir, name + ".bmp")
            shutil.move(tmp, dst)
            png = to_png(dst, os.path.join(outdir, name + ".png"))
            print("%-28s %dx%d%s" % (name, w, h, "  +png" if png else ""))
        sheet = seam_sheet(outdir, sc, GAP[sc], SHEET_SUFFIX)
        if sheet:
            print("%-28s %s" % ("seam sheet", os.path.basename(sheet)))


if __name__ == "__main__":
    out = sys.argv[1]
    sc = [int(a) for a in sys.argv[2:]] or [368, 374, 376]
    main(out, sc)
