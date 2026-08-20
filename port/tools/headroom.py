"""The gapless HEADROOM's evidence: the census, the timing, the seam, the arms.

Read port/ntr/ppu_sub.cpp's head_paint block for what the headroom is and
tools/gapproof.py for the runner underneath this. Every claim here is a number
off a real run, for the reason gapproof's own header gives: the thing under test
is a picture, and on this machine a windowed capture is not reproducible run to
run, so "look at it" is not a check.

  python port/tools/headroom.py census   the per-frame engine-A OAM census, the
                                         objects that reach above the top screen
                                         and how much of each one the strip
                                         shows against how much the top screen
                                         was already showing
  python port/tools/headroom.py timing   when each object first becomes VISIBLE
                                         with the headroom and when it would
                                         have without it, off one run
  python port/tools/headroom.py seam     the strip's fill against the top
                                         screen's first real row, per column
  python port/tools/headroom.py arms     the A/B on one binary: headroom on,
                                         headroom off, and the containment set
  python port/tools/headroom.py touch    the stylus inverse over the four bands
  python port/tools/headroom.py shots F  the before/after crops at frame F

THE TWO ARMS COME OFF ONE BINARY. SM64DS_GAPLESS_HEADROOM=0 suppresses the
headroom and nothing else, which is what notes/port-selftest-bmp-gate.md
requires before two BMPs may be compared at all.

THE CENSUS IS A WINDOWED RUN AND THE CAPTURES ARE NOT. A headless scene run
composes ONCE, at the end, when the capture asks for the image, so it has
exactly one census frame -- which makes it the right mode for every claim about
a finished picture and useless for a claim about WHEN something appeared.
SM64DS_SCENE_WINDOW=1 runs the real frame loop and composes every frame. It
flashes a real window; that is expected.
"""
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gapproof as G

OUT = os.environ.get("HEADOUT",
                     os.path.abspath(os.path.join(G.BUILD, "..", "..", "runs",
                                                  "mg5", "out", "lakitu")))
os.makedirs(OUT, exist_ok=True)

SCENE = 368
SCREEN_H = 384          # the 2x tier's top screen, host rows
GAPLESS = {"GaplessMinigames": True}

# [headroom] A oam 11 a0=03c2 a1=8249 a2=1010 32x32 dbl at (73,-62): box
#            reaches 34 row(s), 190 pixel(s) drawn, ALL OF IT OFF-SCREEN
ENT = re.compile(
    r"\[headroom\] (\w+) oam\s*(\d+) a0=(\w{4}) a1=(\w{4}) a2=(\w{4}) "
    r"(\d+)x(\d+)(.*?) at \((-?\d+),(-?\d+)\): box reaches (\d+) row")
# [headroom] A oam 11 split: 190 px in the strip, 0 px on the screen
SPLIT = re.compile(r"\[headroom\] (\w+) oam\s*(\d+) split: (\d+) px in the "
                   r"strip, (\d+) px on the screen")
# [headroom] f1 strip 32 DS row(s) ...: 372 DS pixel(s) drawn, 24 of 512 ...
FRAME = re.compile(r"\[headroom\] f(\d+) strip (\d+) DS row\(s\).*?: (\d+) DS "
                   r"pixel\(s\) drawn, (\d+) of (\d+) column")


def run(tag, frames, settings=None, extra=None, windowed=False):
    """One run, its capture and its whole log, both kept."""
    bmp = os.path.join(OUT, "%s.bmp" % tag)
    env = dict(extra or {})
    if windowed:
        env["SM64DS_SCENE_WINDOW"] = "1"
    r = G.scene_run(SCENE, frames, bmp, settings=settings, extra_env=env)
    text = r.stdout + r.stderr
    with open(os.path.join(OUT, "%s.log" % tag), "w") as f:
        f.write(text)
    return r, text, (bmp if os.path.exists(bmp) else None)


def parse(text):
    """[(frame, {oam: entry})], in frame order. An entry carries its decoded
    geometry and the two pixel counts the strip pass split it into."""
    frames = []
    cur = {}
    for ln in text.splitlines():
        m = ENT.search(ln)
        if m:
            cur[int(m.group(2))] = {
                "eng": m.group(1), "oam": int(m.group(2)),
                "a0": m.group(3), "a1": m.group(4), "a2": m.group(5),
                "w": int(m.group(6)), "h": int(m.group(7)),
                "kind": m.group(8).strip(), "x": int(m.group(9)),
                "y": int(m.group(10)), "box_rows": int(m.group(11)),
                "strip": 0, "screen": 0}
            continue
        m = SPLIT.search(ln)
        if m:
            e = cur.get(int(m.group(2)))
            if e:
                e["strip"] = int(m.group(3))
                e["screen"] = int(m.group(4))
            continue
        m = FRAME.search(ln)
        if m:
            frames.append((int(m.group(1)), cur, int(m.group(3)),
                           int(m.group(4)), int(m.group(5))))
            cur = {}
    return frames


def ident(e):
    """An object's IDENTITY across frames: its attribute words minus position.

    Tile, palette, size, shape and mode; not x and not y. Two frames of the
    same object agree on all of it and two different objects drawn from one
    template do too, which is why the census below reports the group rather
    than pretending to track an individual."""
    a0 = int(e["a0"], 16) & ~0x00FF      # drop y
    a1 = int(e["a1"], 16) & ~0x01FF      # drop x
    return (a0, a1, e["a2"], e["w"], e["h"], e["kind"])


def census(frames=400):
    r, text, bmp = run("census", frames, GAPLESS,
                       {"SM64DS_HEADROOM_TRACE": "1"}, windowed=True)
    fr = parse(text)
    print("frames censused: %d (rc=%d)" % (len(fr), r.returncode))
    if not fr:
        print(text[-2000:])
        return fr
    groups = {}
    for f, ents, drawn, recon, cols in fr:
        for e in ents.values():
            k = ident(e)
            g = groups.setdefault(k, {"n": 0, "strip": 0, "screen": 0,
                                      "first_strip": None, "first_screen": None,
                                      "ys": [], "xs": [], "e": e})
            g["n"] += 1
            g["strip"] += e["strip"]
            g["screen"] += e["screen"]
            g["ys"].append(e["y"])
            g["xs"].append(e["x"])
            if e["strip"] and g["first_strip"] is None:
                g["first_strip"] = f
            if e["screen"] and g["first_screen"] is None:
                g["first_screen"] = f
    print("engine-A OAM entries reaching above the top screen, by identity:")
    print("  %-26s %5s %7s %8s %9s %9s %s" %
          ("tile/pal/size", "seen", "strip", "screen", "1st strip",
           "1st scrn", "engine y"))
    for k, g in sorted(groups.items(), key=lambda kv: -kv[1]["strip"]):
        e = g["e"]
        print("  a2=%s %2dx%-2d %-9s %5d %7d %8d %9s %9s %d..%d" %
              (e["a2"], e["w"], e["h"], e["kind"] or "plain", g["n"],
               g["strip"], g["screen"],
               "f%d" % g["first_strip"] if g["first_strip"] else "-",
               "f%d" % g["first_screen"] if g["first_screen"] else "-",
               min(g["ys"]), max(g["ys"])))
    tot_strip = sum(f[2] for f in fr)
    print("strip pixels drawn: %d over %d frames, %.1f per frame; frames with "
          "none: %d" % (tot_strip, len(fr), tot_strip / float(len(fr)),
                        sum(1 for f in fr if not f[2])))
    print("columns reconstructed (a sprite over the top screen's first row): "
          "min %d, max %d, of %d" %
          (min(f[3] for f in fr), max(f[3] for f in fr), fr[0][4]))
    return fr


def timing(frames=400):
    """WHEN an object becomes visible, with the headroom and without it.

    Both numbers come off THE SAME RUN, which is the whole point: the headroom
    is display-only, so the simulation is identical either way, and an object's
    first strip pixel is the frame it becomes visible WITH the feature while its
    first screen pixel is the frame it would have become visible WITHOUT it.
    Comparing two runs would add run-to-run variance to a difference this is
    trying to measure."""
    r, text, bmp = run("timing", frames, GAPLESS,
                       {"SM64DS_HEADROOM_TRACE": "1"}, windowed=True)
    fr = parse(text)
    print("frames: %d (rc=%d)" % (len(fr), r.returncode))
    # An OAM SLOT's own history: a slot is reused, so a run of frames in which
    # it holds the same identity is one object's life in that slot.
    lives = []
    live = {}
    for f, ents, _, _, _ in fr:
        for oam in list(live):
            if oam not in ents or ident(ents[oam]) != live[oam]["id"]:
                lives.append(live.pop(oam))
        for oam, e in ents.items():
            k = ident(e)
            if oam not in live:
                live[oam] = {"oam": oam, "id": k, "f0": f, "e": e,
                             "strip": None, "screen": None, "ys": []}
            L = live[oam]
            L["ys"].append((f, e["y"]))
            if e["strip"] and L["strip"] is None:
                L["strip"] = f
            if e["screen"] and L["screen"] is None:
                L["screen"] = f
    lives.extend(live.values())
    print("  %-5s %-6s %-14s %8s %8s %6s  %s" %
          ("oam", "first", "sprite", "1st strip", "1st scrn", "gain",
           "engine y at those frames"))
    gains = []
    for L in sorted(lives, key=lambda L: L["f0"]):
        if L["strip"] is None and L["screen"] is None:
            continue
        e = L["e"]
        gain = ""
        if L["strip"] is not None and L["screen"] is not None:
            g = L["screen"] - L["strip"]
            gain = "%d" % g
            if g > 0:
                gains.append(g)
        ys = dict(L["ys"])
        print("  %-5d f%-5d %2dx%-2d %-8s %8s %8s %6s  %s" %
              (L["oam"], L["f0"], e["w"], e["h"], e["kind"] or "plain",
               "f%d" % L["strip"] if L["strip"] else "-",
               "f%d" % L["screen"] if L["screen"] else "-", gain,
               ", ".join("f%d y=%d" % (f, ys[f])
                         for f in (L["strip"], L["screen"]) if f in ys)))
    if gains:
        print("objects the headroom makes visible EARLIER: %d, by %d..%d "
              "frames (mean %.1f)" % (len(gains), min(gains), max(gains),
                                      sum(gains) / float(len(gains))))
    return lives


def seam(frames=300):
    """The strip's own continuity, off the finished picture.

    THE COLUMNS ARE NOT ALL THE SAME KIND and a single average over all of them
    would hide the one number that matters. Three populations:

      PURE FILL     no sprite anywhere in the column's strip rows and no sprite
                    over the top screen's first row either, so the backdrop is
                    the direct sample. The seam delta here must be EXACTLY ZERO
                    and the strip must be flat top to bottom, because the fill
                    is that row copied upward. This is the continuity claim.
      RECONSTRUCTED a sprite covers the top screen's first row, so the backdrop
                    is the ramp between the nearest uncovered columns. The seam
                    delta here is nonzero ON PURPOSE -- the composed row holds
                    the sprite and the strip holds the sky behind it -- and the
                    number is reported so the reconstruction can be seen to be
                    doing something rather than assumed to be.
      SPRITE        the strip drew a sprite in this column. Not a fill question.

    And the SOURCE's own flatness, which is the ceiling on how faithful any
    upward extension of one row can be: if the picture's top two DS rows already
    differ, no fill built from one of them can look like more of the other."""
    r, text, bmp = run("seam", frames, GAPLESS)
    if not bmp:
        print("no capture (rc=%d)" % r.returncode)
        return
    w, h, rows = G.read_bmp(bmp)
    head = h - 2 * SCREEN_H
    print("image %dx%d, headroom %d host rows, top screen starts at row %d"
          % (w, h, head, head))

    flat, recon, sprite = [], [], []
    for x in range(w):
        col = [rows[y][x] for y in range(head)]
        if len(set(col)) > 1:
            sprite.append(x)
        elif col[0] == rows[head][x]:
            flat.append(x)
        else:
            recon.append(x)

    def stats(xs, a, b):
        if not xs:
            return 0, 0.0
        worst = tot = 0
        for x in xs:
            d = max(abs(rows[a][x][i] - rows[b][x][i]) for i in range(3))
            worst = max(worst, d)
            tot += d
        return worst, tot / float(len(xs))

    m, av = stats(flat, head - 1, head)
    print("  PURE FILL     %d of %d columns: seam (strip row %d vs top screen "
          "row 0) max per-channel delta %d, mean %.2f" %
          (len(flat), w, head - 1, m, av))
    m, av = stats(flat, 0, head)
    print("                strip row 0 vs top screen row 0: max %d, mean %.2f "
          "(the whole strip is that row, so both are 0 or the fill is broken)"
          % (m, av))
    m, av = stats(recon, head - 1, head)
    print("  RECONSTRUCTED %d of %d columns: the sky rebuilt behind a sprite "
          "that sits on the top screen's first row; delta against that row max "
          "%d, mean %.2f -- nonzero is the feature working" %
          (len(recon), w, m, av))
    print("  SPRITE        %d of %d columns carry a strip sprite" %
          (len(sprite), w))
    m, av = stats(range(w), head, head + 2)
    print("  SOURCE        top screen DS row 0 vs DS row 1: max %d, mean %.2f "
          "over all %d columns (the flatness the fill inherits)" % (m, av, w))
    fillcol = [rows[0][x] for x in range(w)]
    n = sum(1 for y in range(head) for x in range(w)
            if rows[y][x] != fillcol[x])
    print("  CONTENT       %d of %d strip pixels differ from their column's "
          "top pixel (%.2f%%)" % (n, head * w, 100.0 * n / (head * w)))
    return bmp


def arms(frames=300):
    """The A/B and the containment, one binary, md5 each.

    HEADROOM OFF MUST BE THE OLD PICTURE EXACTLY. That is the containment claim
    this feature stands on: a run with the headroom suppressed composes the same
    512x768 image gapless composed before the headroom existed, and every layout
    that is not gapless never had a headroom to suppress."""
    print("%-24s %-9s %s" % ("run", "image", "md5"))
    out = {}
    for tag, settings, extra in (
            ("gapless + headroom", GAPLESS, None),
            ("gapless, headroom OFF", GAPLESS,
             {"SM64DS_GAPLESS_HEADROOM": "0"}),
            ("gap on (the ROM)", None, None),
            ("MinigameGap false", {"MinigameGap": False}, None)):
        bmp = os.path.join(OUT, "arm_%s.bmp" % re.sub(r"\W+", "_", tag))
        r = G.scene_run(SCENE, frames, bmp, settings=settings, extra_env=extra)
        dims = "%dx%d" % G.read_bmp(bmp)[:2] if os.path.exists(bmp) else "-"
        md5 = G.md5(bmp) if os.path.exists(bmp) else "NO BMP"
        print("%-24s %-9s %s" % (tag, dims, md5))
        out[tag] = md5
    return out


def touch(frames=30):
    """The stylus inverse over the three bands and the headroom.

    A HEADROOM CLICK MAPS TO NOTHING, the gap band's own rule: those rows are
    picture with no panel under them. The points are chosen off the layout --
    one in the headroom, one on each screen, one in the letterbox -- and the
    probe drives the REAL mappers rather than restating their arithmetic."""
    pts = [(4, 4), (4, 40), (4, 120), (4, 500), (4, 800), (4, 1000)]
    bmp = os.path.join(OUT, "touch.bmp")
    r = G.scene_run(SCENE, frames, bmp, settings=GAPLESS,
                    extra_env={"SM64DS_TOUCH_CLIENT_PROBE":
                               ";".join("%d,%d" % p for p in pts)})
    for ln in (r.stdout + r.stderr).splitlines():
        if "[touchmap]" in ln:
            print("  " + ln.strip())
    return r


# ---- the crops --------------------------------------------------------------
#
# BOTH ARMS AT ONE FRAME, cropped to the top of the picture, with the SAME WORLD
# ROW on the same output row: the off arm is padded by the headroom's height,
# because that is exactly the picture it is missing. The pad is magenta so
# nobody can mistake it for a fill this feature draws.
#
# A HEADLESS RUN COMPOSES ONLY ITS LAST FRAME, which is what makes this exact:
# asking for N frames gives the picture at frame N and no other, so a crop is a
# named frame rather than "somewhere around there".
SHOTS = os.path.join(OUT, "shots")
PAD = (255, 0, 255)          # the rows the off arm does not have


def write_bmp(path, rows):
    h = len(rows)
    w = len(rows[0])
    stride = (w * 3 + 3) & ~3
    data = bytearray()
    for y in range(h - 1, -1, -1):
        line = bytearray()
        for x in range(w):
            r, g, b = rows[y][x]
            line += bytes((b, g, r))
        line += b"\0" * (stride - len(line))
        data += line
    hdr = struct.pack("<2sIHHI", b"BM", 14 + 40 + len(data), 0, 0, 14 + 40)
    hdr += struct.pack("<IiiHHIIiiII", 40, w, h, 1, 24, 0, len(data),
                       2835, 2835, 0, 0)
    with open(path, "wb") as f:
        f.write(hdr + bytes(data))


def shot(frame, keep=128):
    """One frame, both arms, cropped to the top `keep` DS-equivalent rows."""
    os.makedirs(SHOTS, exist_ok=True)
    off = os.path.join(SHOTS, "f%d_off.bmp" % frame)
    on = os.path.join(SHOTS, "f%d_on.bmp" % frame)
    G.scene_run(SCENE, frame, off, settings=GAPLESS,
                extra_env={"SM64DS_GAPLESS_HEADROOM": "0"})
    r = G.scene_run(SCENE, frame, on, settings=GAPLESS,
                    extra_env={"SM64DS_HEADROOM_TRACE": "1"})
    with open(os.path.join(SHOTS, "f%d_on.log" % frame), "w") as f:
        f.write(r.stdout + r.stderr)
    wo, ho, ro = G.read_bmp(off)
    wn, hn, rn = G.read_bmp(on)
    head = hn - ho
    a = [[PAD] * wo for _ in range(head)] + ro[:keep]
    b = rn[:head + keep]
    write_bmp(os.path.join(SHOTS, "f%d_before.bmp" % frame), a)
    write_bmp(os.path.join(SHOTS, "f%d_after.bmp" % frame), b)
    gap = 8
    side = [a[y] + [(32, 32, 32)] * gap + b[y] for y in range(len(a))]
    write_bmp(os.path.join(SHOTS, "f%d_compare.bmp" % frame), side)
    strip_lines = [l for l in (r.stdout + r.stderr).splitlines()
                   if "[headroom]" in l]
    drawn = 0
    for l in strip_lines:
        m = FRAME.search(l)
        if m:
            drawn = int(m.group(3))
    print("f%-4d off %dx%d, on %dx%d, headroom %d host rows, %d DS pixel(s) "
          "in the strip" % (frame, wo, ho, wn, hn, head, drawn))
    for l in strip_lines:
        if "split:" in l:
            print("   " + l.strip())
    return drawn


if __name__ == "__main__":
    step = sys.argv[1] if len(sys.argv) > 1 else "census"
    n = int(sys.argv[2]) if len(sys.argv) > 2 else None
    if step == "shots":
        for f in [int(a) for a in sys.argv[2:]] or [83, 202, 234, 300]:
            shot(f)
        raise SystemExit(0)
    fn = {"census": census, "timing": timing, "seam": seam, "arms": arms,
          "touch": touch}[step]
    fn(n) if n else fn()
