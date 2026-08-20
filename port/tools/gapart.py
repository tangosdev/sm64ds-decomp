"""The gap band's hand-drawn art: make one, convert one, and check one.

The band art contract lives in port/hal/gap_art.h and the reader that enforces
it is port/hal/gap_art.cpp. This is the other end of it: the converter that
turns what an artist drew into a file that reader accepts, and the checker that
proves a capture really carries those pixels.

  python tools/gapart.py test <G> <out.bmp> [--2x]
      A PLACEHOLDER BAND, no art program needed. Eight colour bars down the
      left half, the magenta key over the right half, and a white rule along
      the band's first and last row. Every one of those is chosen to be
      checkable rather than pretty: the bars say the art landed at the right x,
      the rules say it landed at the right y and was not scrolled by a row, and
      the keyed half says a transparent pixel really does show the backdrop.

  python tools/gapart.py from-png <in.png> <G> <out.bmp> [--2x] [--band-top N]
      THE CONVERTER FOR THE TEMPLATE. The template is 256 wide with 64 rows of
      top-screen context, then the G band rows, then 64 rows of bottom-screen
      context; --band-top says where the band starts (default 64) for a
      template cut differently. Only the band rows are written. Needs Pillow.

  python tools/gapart.py check <capture.bmp> <art.bmp> <G> [--backdrop black]
      THE PROOF. Reads a stacked capture, lifts its band, and compares every
      band pixel against the art: a painted art pixel must be there exactly,
      and a keyed one must be whatever the backdrop is (`black` under peek, or
      `any` when the backdrop is the ambient wash and this is not the thing
      under test). Prints the two counts and the first few disagreements.

WHY BMP AND NOT PNG. The program has no PNG decoder and already writes BMPs
everywhere, so a BMP is a file the reader can be twenty lines long for. The art
is drawn as a PNG and converted here, once, by the same person who owns the
band.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gapproof as G

KEY = (0xFF, 0x00, 0xFF)

# eight bars, all far apart in every channel so a one-bar slip is obvious and
# none of them can be confused with the key or with black
BARS = [(0xFF, 0x00, 0x00), (0x00, 0xFF, 0x00), (0x00, 0x00, 0xFF),
        (0xFF, 0xFF, 0x00), (0x00, 0xFF, 0xFF), (0xFF, 0x80, 0x00),
        (0x80, 0x00, 0xFF), (0x00, 0x80, 0x40)]


def write_bmp24(path, rows):
    """rows[y][x] = (r, g, b), y = 0 at the TOP. Written bottom-up, which is
    what the contract asks for and what every other writer in this tree emits."""
    h = len(rows)
    w = len(rows[0])
    stride = (w * 3 + 3) & ~3
    pad = b"\0" * (stride - w * 3)
    body = b"".join(
        bytes(bytearray([c for x in range(w)
                         for c in (rows[y][x][2], rows[y][x][1],
                                   rows[y][x][0])])) + pad
        for y in range(h - 1, -1, -1))
    hdr = struct.pack("<2sIHHI", b"BM", 14 + 40 + len(body), 0, 0, 14 + 40)
    dib = struct.pack("<IiiHHIIiiII", 40, w, h, 1, 24, 0, len(body),
                      2835, 2835, 0, 0)
    with open(path, "wb") as f:
        f.write(hdr + dib + body)
    return w, h


def make_test(gap_ds, two_x):
    """The placeholder band, in DS pixels; the 2x file is this doubled, so the
    loader's nearest downsample has to give back exactly this."""
    rows = []
    for k in range(gap_ds):
        row = []
        for x in range(256):
            if x >= 128:
                row.append(KEY)                       # the keyed half
            elif k == 0 or k == gap_ds - 1:
                row.append((0xFF, 0xFF, 0xFF))        # the top and bottom rule
            else:
                row.append(BARS[(x // 16) % len(BARS)])
        rows.append(row)
    if not two_x:
        return rows
    big = []
    for r in rows:
        d = [p for p in r for _ in (0, 1)]
        big.append(d)
        big.append(list(d))
    return big


def from_png(path, gap_ds, two_x, band_top):
    from PIL import Image
    im = Image.open(path).convert("RGB")
    step = 2 if two_x else 1
    want_w = 256 * step
    if im.width != want_w:
        raise SystemExit("%s is %d wide, the template is %d at this scale"
                         % (path, im.width, want_w))
    top = band_top * step
    if top + gap_ds * step > im.height:
        raise SystemExit("%s is %d tall: no %d band rows at row %d"
                         % (path, im.height, gap_ds, band_top))
    px = im.load()
    return [[px[x, top + y] for x in range(im.width)]
            for y in range(gap_ds * step)]


def read_art(path):
    """The art file back as DS pixels, keyed pixels marked None. Reads both
    accepted sizes, the same way the C reader does."""
    w, h, rows = G.read_bmp(path)
    step = 2 if w == 512 else 1
    out = []
    for k in range(h // step):
        r = []
        for x in range(256):
            p = rows[k * step][x * step]
            r.append(None if tuple(p) == KEY else tuple(p))
        out.append(r)
    return out


def check(capture, art_path, gap_ds, backdrop):
    art = read_art(art_path)
    if len(art) != gap_ds:
        raise SystemExit("art is %d rows, G is %d" % (len(art), gap_ds))
    w, h, rows = G.read_bmp(capture)
    band_y = 384                       # the 2x tier's top screen, host rows
    scale = 2
    painted = keyed = bad_paint = bad_key = 0
    misses = []
    for k in range(gap_ds):
        for x in range(256):
            want = art[k][x]
            for oy in range(scale):
                for ox in range(scale):
                    got = tuple(rows[band_y + k * scale + oy][x * scale + ox])
                    if want is not None:
                        painted += 1
                        if got != want:
                            bad_paint += 1
                            if len(misses) < 8:
                                misses.append(("paint", k, x, want, got))
                    else:
                        keyed += 1
                        if backdrop == "black" and got != (0, 0, 0):
                            bad_key += 1
                            if len(misses) < 8:
                                misses.append(("key", k, x, (0, 0, 0), got))
    print("  art: %d painted band pixels, %d wrong" % (painted, bad_paint))
    print("  art: %d keyed band pixels, %d not the %s backdrop"
          % (keyed, bad_key, backdrop))
    for m in misses:
        print("    %s row %d col %d: wanted %s, got %s" % m)
    return bad_paint, bad_key


if __name__ == "__main__":
    cmd = sys.argv[1]
    if cmd == "test":
        g = int(sys.argv[2])
        out = sys.argv[3]
        two = "--2x" in sys.argv
        w, h = write_bmp24(out, make_test(g, two))
        print("wrote %s, %dx%d for G=%d" % (out, w, h, g))
    elif cmd == "from-png":
        src, g, out = sys.argv[2], int(sys.argv[3]), sys.argv[4]
        two = "--2x" in sys.argv
        top = 64
        if "--band-top" in sys.argv:
            top = int(sys.argv[sys.argv.index("--band-top") + 1])
        w, h = write_bmp24(out, from_png(src, g, two, top))
        print("wrote %s, %dx%d for G=%d (band rows %d..%d of %s)"
              % (out, w, h, g, top, top + g - 1, src))
    elif cmd == "check":
        cap, art, g = sys.argv[2], sys.argv[3], int(sys.argv[4])
        back = "any"
        if "--backdrop" in sys.argv:
            back = sys.argv[sys.argv.index("--backdrop") + 1]
        bp, bk = check(cap, art, g, back)
        sys.exit(1 if (bp or bk) else 0)
    else:
        raise SystemExit(__doc__)
