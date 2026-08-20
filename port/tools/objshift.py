"""The gapless OBJECT SHIFT's evidence: the register, the picture, the crossing.

Read THE OBJECT SHIFT in port/ntr/include/ntr/ppu.h for what the mode is, and
tools/gapproof.py for the runner underneath this. Every claim here is a number
off a real run, for the reason gapproof's own header gives: the thing under test
is a picture, and on this machine a windowed capture is not reproducible run to
run, so "look at it" is not a check.

  python port/tools/objshift.py arms      the A/B on one binary: the shift on,
                                          the shift off, gap on, MinigameGap
                                          false, with the image size and md5 of
                                          each
  python port/tools/objshift.py register  every engine-A OBJ entry's engine row
                                          in each arm, frame for frame, over one
                                          headless run of each. THE FALSIFYING
                                          STEP: it reports, per sprite identity,
                                          how far gap-on's row is from gapless's,
                                          and the answer is +32 for one identity
                                          and 0 for all the others
  python port/tools/objshift.py diff      the two images against each other, by
                                          region: top screen, band, bottom
                                          screen
  python port/tools/objshift.py crossing   a scripted slingshot launch, windowed,
                                          with the seam straddle census on: the
                                          per-frame table of how much of a
                                          crossing ball each half of the picture
                                          really shows
  python port/tools/objshift.py timing     when a routed sprite enters the
                                          picture, in all three arms, off the
                                          per-entry trace rather than off a
                                          bounding box
  python port/tools/objshift.py touch      the stylus inverse over the bands
  python port/tools/objshift.py shots F    the side-by-side crops at frame F

THE SHIFT IS OFF and SM64DS_GAPLESS_OBJ_SHIFT=1 turns it on, on the SAME
BINARY, which is what notes/port-selftest-bmp-gate.md requires before two BMPs
may be compared at all. It is off because `register` below falsified it: only
the sprites the framework's own OAM router placed move with G, and the shift
moves the whole layer. Read obj_shift_ds in hal/screen_gap.cpp for the numbers.
These steps are kept because they are the measurement, not because the feature
is.

WHY GAP-ON IS THE GROUND TRUTH HERE. Bob-omb Squad's own code reads the
framework gap word nowhere (the audit is in hal/screen_gap.cpp), so the
simulation is bit-for-bit the same in both arms and the only thing that differs
is the mapping of world rows onto the two engines. The shifted arm's top screen
SHOULD therefore be the gap-on picture exactly, and `diff` is where that was
checked rather than assumed. It is not: 28152 of 196608 pixels differ with the
shift on against 2940 with it off, which is the whole finding.

FRAME COUNT IS PART OF THE PROBE. A headless run composes ONCE, at the end, when
the capture asks for the image, so asking for N frames gives the picture at
frame N and no other. The per-entry trace, however, prints every frame on the
headless path too, because the engine A compositor runs inside the frame loop
and only the COMPOSE waits for the end.
"""
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gapproof as G

OUT = os.environ.get("OBJSHIFTOUT",
                     os.path.abspath(os.path.join(G.BUILD, "..", "..", "runs",
                                                  "mg5", "out", "objshift")))
os.makedirs(OUT, exist_ok=True)

SCENE = 368
SCREEN_H = 384          # the 2x tier's screen, host rows
SCALE = 2               # host rows per DS row
GAPLESS = {"GaplessMinigames": True}
SHIFT_ON = {"SM64DS_GAPLESS_OBJ_SHIFT": "1"}
TRACE = {"SM64DS_OBJSHIFT_TRACE": "1"}

LAKITU = "211c"         # a2 of the 32x64 double-size affine entry, the report
BOMB = "1010"           # a2 of the 32x32 double-size parachuting bob-ombs

# [objshift] f12 oam 11 a0=03c2 a1=8249 a2=1010 32x32 dbl at (73,-62) shift 32
#            display -30..33: 30 above, 34 top, 0 band
ENT = re.compile(
    r"\[objshift\] f(\d+) oam\s*(\d+) a0=(\w{4}) a1=(\w{4}) a2=(\w{4}) "
    r"(\d+)x(\d+)(.*?) at \((-?\d+),(-?\d+)\) shift (-?\d+) "
    r"display (-?\d+)\.\.(-?\d+): (\d+) above, (\d+) top, (\d+) band")
# [hinge] f232 band 32 DS row(s) (world -32..-1, engine rows 160..191): 96 ...
HINGE = re.compile(r"\[hinge\] f(\d+) band (\d+) DS row\(s\).*?: (\d+) DS pix")
# [gapstraddle] f119 slot 0 16x16 at (125,-4 world) bottom half only, drawn
#     from B a0=... at (125,-4 world): before top 0/42 bot 166/166, after
#     top 42/42 bot 166/166, 42 synthesized, total 208/208
STR_DRAWN = re.compile(
    r"\[gapstraddle\] f(\d+) slot (\d+) (\d+)x(\d+) at \((-?\d+),(-?\d+) world\) "
    r"(.*?), drawn from (\w+) a0=(\w{4}) a1=(\w{4}) a2=(\w{4}) at "
    r"\((-?\d+),(-?\d+) world\): before top (\d+)/(\d+) bot (\d+)/(\d+), "
    r"after top (\d+)/(\d+) bot (\d+)/(\d+), (\d+) synthesized, "
    r"total (\d+)/(\d+)")
# [gapstraddle] f121 slot 0 16x16 at (126,-12 world): top no, bottom no -- ...
STR_NONE = re.compile(
    r"\[gapstraddle\] f(\d+) slot (\d+) (\d+)x(\d+) at \((-?\d+),(-?\d+) "
    r"world\): top (\S+ ?\S*), bottom (\S+ ?\S*) -- (.*)")


def run(tag, frames, settings=None, extra=None, windowed=False, keep_log=True):
    """One run, its capture and its whole log, both kept."""
    bmp = os.path.join(OUT, "%s.bmp" % tag)
    env = dict(extra or {})
    if windowed:
        env["SM64DS_SCENE_WINDOW"] = "1"
    r = G.scene_run(SCENE, frames, bmp, settings=settings, extra_env=env)
    text = r.stdout + r.stderr
    if keep_log:
        with open(os.path.join(OUT, "%s.log" % tag), "w") as f:
            f.write(text)
    return r, text, (bmp if os.path.exists(bmp) else None)


def entries(text):
    """[(frame, [entry...])] in frame order, one list per composited frame."""
    frames = {}
    for ln in text.splitlines():
        m = ENT.search(ln)
        if not m:
            continue
        f = int(m.group(1))
        frames.setdefault(f, []).append({
            "oam": int(m.group(2)), "a0": m.group(3), "a1": m.group(4),
            "a2": m.group(5), "w": int(m.group(6)), "h": int(m.group(7)),
            "kind": m.group(8).strip(), "x": int(m.group(9)),
            "y": int(m.group(10)), "shift": int(m.group(11)),
            "d0": int(m.group(12)), "d1": int(m.group(13)),
            "above": int(m.group(14)), "top": int(m.group(15)),
            "band": int(m.group(16))})
    return [(f, frames[f]) for f in sorted(frames)]


# ---- the arms ---------------------------------------------------------------

def arms(frames=300):
    """The A/B and the containment, one binary, md5 each.

    THE SHIFT OFF MUST BE THE OLD PICTURE EXACTLY, and gap on and MinigameGap
    false must be the pictures they were before this lane existed. That is the
    containment: the shift is reachable on one scene with one setting on, and
    every layout that is not that one never had a shift to suppress."""
    print("%-26s %-9s %s" % ("run", "image", "md5"))
    out = {}
    for tag, settings, extra in (
            ("gapless + shift", GAPLESS, SHIFT_ON),
            ("gapless, shift OFF", GAPLESS, None),
            ("gap on (the ROM)", None, None),
            ("MinigameGap false", {"MinigameGap": False}, None)):
        bmp = os.path.join(OUT, "arm_%s.bmp" % re.sub(r"\W+", "_", tag))
        r = G.scene_run(SCENE, frames, bmp, settings=settings, extra_env=extra)
        dims = "%dx%d" % G.read_bmp(bmp)[:2] if os.path.exists(bmp) else "-"
        md5 = G.md5(bmp) if os.path.exists(bmp) else "NO BMP"
        print("%-26s %-9s %s" % (tag, dims, md5))
        out[tag] = (dims, md5)
    return out


# ---- the register -----------------------------------------------------------

def register(frames=400):
    """WHERE EVERY TOP-SCREEN SPRITE IS DRAWN, in both arms, frame for frame.

    THE CLAIM THIS FALSIFIED. With the gap on, the framework's OAM router
    submits a top-screen sprite at world y + 0xc0 + G and the engine draws it
    there; with the mod on it submits at world y + 0xc0. If EVERY top-screen
    sprite went through that router, then every entry in the gap-on arm would
    sit exactly G rows below its twin in the gapless arm, and adding G back at
    the raster would restore the ROM's picture.

    THEY DO NOT. Per sprite identity, gap-on's engine row minus gapless's, over
    300 frames: a2=1010 gives +32 on all 116 of its sightings, and every other
    identity in the run gives 0 on every one of theirs. The score and timer rows
    at engine y 4 and y 168, and the three 32x64 double-size entries at x 0, 64
    and 128, are placed in SCREEN space and were never displaced. A layer shift
    moves them anyway.

    IT ALSO COUNTS WHAT THE MOD LOSES OUTRIGHT. OAM::Render culls at submission
    with `if (y + h < 0) return`, and a routed sprite's y is G lower under the
    mod, so entries reach OAM in gap-on that reach it in no form here. That is a
    submission the display never sees and no display change can recover."""
    _, on_txt, _ = run("reg_shift", frames, settings=GAPLESS,
                       extra=dict(list(TRACE.items()) +
                                  list(SHIFT_ON.items())))
    _, off_txt, _ = run("reg_noshift", frames, settings=GAPLESS, extra=TRACE)
    _, gap_txt, _ = run("reg_gapon", frames, settings=None, extra=TRACE)

    on, off, gap = entries(on_txt), entries(off_txt), entries(gap_txt)
    print("frames traced: shift %d, shift-off %d, gap-on %d"
          % (len(on), len(off), len(gap)))

    def key(e):
        return (e["oam"], e["a0"], e["a1"], e["a2"])

    same = diff = missing = 0
    worst = []
    gapmap = dict(gap)
    for f, es in on:
        ge = {key(e): e for e in gapmap.get(f, [])}
        for e in es:
            g = ge.get(key(e))
            if g is None:
                missing += 1
                continue
            if e["d0"] == g["d0"]:
                same += 1
            else:
                diff += 1
                if len(worst) < 12:
                    worst.append((f, e["oam"], e["a2"], e["d0"], g["d0"]))
    print("\nEVERY ENTRY, shifted arm's display row vs gap-on's own row:")
    print("  %d agree, %d differ, %d had no counterpart in the other arm"
          % (same, diff, missing))
    for w in worst:
        print("    f%-4d oam%-3d a2=%s shifted %d, gap-on %d" % w)

    for tag, a2 in (("LAKITU", LAKITU), ("BOB-OMBS", BOMB)):
        print("\n%s (a2=%s)" % (tag, a2))
        for name, rows in (("gapless, shift OFF", off), ("gapless + shift", on),
                           ("gap on (the ROM)", gap)):
            tops, cut, seen, band = [], 0, 0, 0
            for f, es in rows:
                for e in es:
                    if e["a2"] != a2:
                        continue
                    seen += 1
                    tops.append(e["d0"])
                    cut += e["above"]
                    band += e["band"]
            if not tops:
                print("  %-22s no entries" % name)
                continue
            print("  %-22s sprite top display row %d..%d (median %d) over %d "
                  "sighting(s); %d row(s) lost off the top, %d row(s) into the "
                  "band" % (name, min(tops), max(tops),
                            sorted(tops)[len(tops) // 2], seen, cut, band))
    return on, off, gap


# ---- the two pictures against each other ------------------------------------

def diff(frames=300):
    """THE TOP SCREEN AGAINST THE ROM'S OWN, and the rest of the image with it.

    Both arms compose 512x832 now and both put world row r at
    bottom_y + r * scale, so the two images are comparable row for row over
    their whole height. Anything that differs outside the band is the shift
    getting the top screen wrong."""
    a = os.path.join(OUT, "diff_gapon.bmp")
    b = os.path.join(OUT, "diff_shift.bmp")
    G.scene_run(SCENE, frames, a, settings=None)
    G.scene_run(SCENE, frames, b, settings=GAPLESS, extra_env=SHIFT_ON)
    wa, ha, ra = G.read_bmp(a)
    wb, hb, rb = G.read_bmp(b)
    print("gap on        %dx%d  %s" % (wa, ha, G.md5(a)))
    print("gapless+shift %dx%d  %s" % (wb, hb, G.md5(b)))
    if (wa, ha) != (wb, hb):
        print("THE TWO IMAGES ARE NOT THE SAME SHAPE, so nothing below is a "
              "comparison")
        return
    band_h = ha - SCREEN_H * 2
    regions = [("top screen", 0, SCREEN_H),
               ("band", SCREEN_H, SCREEN_H + band_h),
               ("bottom screen", SCREEN_H + band_h, ha)]
    for name, y0, y1 in regions:
        n = worst = 0
        for y in range(y0, y1):
            for x in range(wa):
                if ra[y][x] != rb[y][x]:
                    n += 1
                    d = max(abs(ra[y][x][c] - rb[y][x][c]) for c in range(3))
                    if d > worst:
                        worst = d
        tot = (y1 - y0) * wa
        print("  %-14s rows %3d..%3d  %d of %d pixels differ (%.2f%%), max "
              "per-channel %d" % (name, y0, y1 - 1, n, tot, 100.0 * n / tot,
                                  worst))
    return a, b


# ---- the crossing -----------------------------------------------------------

# The straddle lane's own probe, kept verbatim: two scripted slingshot launches,
# grab the ball where it rests, drag, release. Reusing it is the point -- a
# different probe would be a different crossing and the two tables could not be
# read against each other.
PROBE = ("200:128:40,201-210:128:180,211-215,"
         "600:128:40,601-610:128:70,611-615")


def crossing(frames=1400):
    """THE OBLIGATION LANE STRADDLE PROVED, RE-PROVEN UNDER THE SHIFT.

    A ball crossing world row 0 is routed to exactly ONE engine and that engine
    clips it at its own screen edge, so the rows hanging over the seam are drawn
    nowhere. The hinge rows do not answer that: they are engine A's own output
    for world -G..-1, and a ball the router gave to engine A at world -1 has
    fifteen more rows below world 0 that engine B was never given. The seam
    straddle pass is what draws them, and this is the table that says so.

    BEFORE and AFTER are counted off the composed image itself, not off the
    pass's bookkeeping: for every opaque texel of the entry the game submitted,
    whether the pixel standing at its place in the image IS that texel."""
    r, text, bmp = run("crossing", frames, settings=GAPLESS,
                       extra=dict([("SM64DS_GAP_STRADDLE_TRACE", "1")] +
                                  list(SHIFT_ON.items())),
                       windowed=True)
    rows = []
    for ln in text.splitlines():
        m = STR_DRAWN.search(ln)
        if m:
            rows.append({
                "f": int(m.group(1)), "slot": int(m.group(2)),
                "w": int(m.group(3)), "h": int(m.group(4)),
                "x": int(m.group(5)), "y": int(m.group(6)),
                "why": m.group(7), "eng": m.group(8),
                "ex": int(m.group(12)), "ey": int(m.group(13)),
                "bt": (int(m.group(14)), int(m.group(15))),
                "bb": (int(m.group(16)), int(m.group(17))),
                "at": (int(m.group(18)), int(m.group(19))),
                "ab": (int(m.group(20)), int(m.group(21))),
                "syn": int(m.group(22)),
                "tot": (int(m.group(23)), int(m.group(24)))})
            continue
        m = STR_NONE.search(ln)
        if m:
            rows.append({"f": int(m.group(1)), "slot": int(m.group(2)),
                         "w": int(m.group(3)), "h": int(m.group(4)),
                         "x": int(m.group(5)), "y": int(m.group(6)),
                         "why": m.group(9), "eng": None})
    return r, text, rows, bmp


# ---- the stylus -------------------------------------------------------------

def touch(frames=30):
    """The stylus inverse over the bands, in the shifted layout.

    A CLICK IN THE BAND MAPS TO NOTHING even here, and that is not an oversight.
    The band rows are DISPLAY: the top engine draws its own sprites into them so
    a player can watch an object cross, but the hardware has no panel there and
    the game has no stylus record for them. The bottom screen's own offset moves
    down by the band's height, which is the whole reason the layout is one
    struct: the picture and the touch transform move together or they do not
    move at all."""
    pts = [(4, 4), (4, 200), (4, 380), (4, 400), (4, 440), (4, 500), (4, 800),
           (4, 1000)]
    out = {}
    for tag, settings, extra in (("gapless + shift", GAPLESS, SHIFT_ON),
                                 ("gapless, shift OFF", GAPLESS, None),
                                 ("gap on (the ROM)", None, None)):
        env = dict(extra or {})
        env["SM64DS_TOUCH_CLIENT_PROBE"] = ";".join("%d,%d" % p for p in pts)
        bmp = os.path.join(OUT, "touch_%s.bmp" % re.sub(r"\W+", "_", tag))
        r = G.scene_run(SCENE, frames, bmp, settings=settings, extra_env=env)
        lines = [l.strip() for l in (r.stdout + r.stderr).splitlines()
                 if "[touchmap]" in l]
        print("== %s ==" % tag)
        for l in lines:
            print("  " + l)
        out[tag] = lines
    return out


# ---- the crops --------------------------------------------------------------

SHOTS = os.path.join(OUT, "shots")
SEP = (32, 32, 32)


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


def shot(frame, keep=192):
    """THE SIDE BY SIDE TANGO JUDGES: the same frame, the same frame shape.

    LEFT is gap-on mode's top screen, which is the ROM's ground truth. RIGHT is
    the gapless-with-shift top screen. MIDDLE, for the report, is the gapless
    arm with the shift OFF, which is the picture that was reported: Lakitu
    jammed against the beam.

    All three crops are the TOP SCREEN and nothing else, `keep` host rows of it
    from row top_y, so the three are the same rows of the same thing and a
    difference is a difference in the picture rather than in the crop."""
    os.makedirs(SHOTS, exist_ok=True)
    paths = {}
    for tag, settings, extra in (("gapon", None, None),
                                 ("noshift", GAPLESS, None),
                                 ("shift", GAPLESS, SHIFT_ON)):
        p = os.path.join(SHOTS, "f%d_%s.bmp" % (frame, tag))
        G.scene_run(SCENE, frame, p, settings=settings, extra_env=extra)
        paths[tag] = p
    crops = {}
    for tag, p in paths.items():
        w, h, rows = G.read_bmp(p)
        # top_y is 0 in every one of these layouts (the headroom is off), so
        # the top screen is rows 0..SCREEN_H-1 and the crop is its first `keep`
        top = rows[:min(keep, SCREEN_H)]
        crops[tag] = top
        write_bmp(os.path.join(SHOTS, "f%d_%s_top.bmp" % (frame, tag)), top)
    n = len(crops["gapon"])
    side = [crops["gapon"][y] + [SEP] * 8 + crops["noshift"][y] + [SEP] * 8 +
            crops["shift"][y] for y in range(n)]
    out = os.path.join(SHOTS, "f%d_topscreens.bmp" % frame)
    write_bmp(out, side)
    print("wrote %s: gap-on | gapless no shift | gapless + shift, "
          "top screen, %d host rows" % (out, n))
    same = sum(1 for y in range(n) for x in range(len(crops["gapon"][0]))
               if crops["gapon"][y][x] == crops["shift"][y][x])
    tot = n * len(crops["gapon"][0])
    print("  gap-on vs gapless+shift: %d of %d pixels identical (%.2f%%)"
          % (same, tot, 100.0 * same / tot))
    same2 = sum(1 for y in range(n) for x in range(len(crops["gapon"][0]))
                if crops["gapon"][y][x] == crops["noshift"][y][x])
    print("  gap-on vs gapless NO shift: %d of %d pixels identical (%.2f%%)"
          % (same2, tot, 100.0 * same2 / tot))
    return out


def full(frame):
    """ONE WHOLE COMPOSED FRAME, gap-on beside gapless+shift, all 832 rows.

    This is the frame that shows the band doing its job: in the left image those
    rows are fill over a hole the hardware cannot address, and in the right one
    they are the top engine's own texels for world -32..-1."""
    os.makedirs(SHOTS, exist_ok=True)
    a = os.path.join(SHOTS, "f%d_full_gapon.bmp" % frame)
    b = os.path.join(SHOTS, "f%d_full_shift.bmp" % frame)
    G.scene_run(SCENE, frame, a, settings=None)
    G.scene_run(SCENE, frame, b, settings=GAPLESS, extra_env=SHIFT_ON)
    wa, ha, ra = G.read_bmp(a)
    wb, hb, rb = G.read_bmp(b)
    n = min(ha, hb)
    side = [ra[y] + [SEP] * 8 + rb[y] for y in range(n)]
    out = os.path.join(SHOTS, "f%d_full.bmp" % frame)
    write_bmp(out, side)
    print("wrote %s: %dx%d beside %dx%d" % (out, wa, ha, wb, hb))
    return out


def timing(frames=400):
    """WHEN A ROUTED SPRITE ENTERS THE PICTURE, in all three arms.

    lane LAKITU's marker box was a bounding box read off the picture. It cannot
    be used here: with the shift on, eleven of the twelve identities on this
    screen have moved too, so a box drawn around "what changed" is drawn around
    the regression rather than around the object. The number that answers the
    same question exactly is the routed entry's own row, which the per-entry
    trace prints every frame in every arm.

    THE MAPPING CLAIM. World row r reaches the top screen at engine row
    r + 0xc0 + G, so a routed sprite's first frame with rows on the screen is a
    function of G alone. gap-on and the shifted arm should agree on it frame for
    frame, and gap-on and the unshifted arm should differ by G.

    AND THE SUBMISSION CULL, which no arm can display its way out of:
    OAM::Render drops an entry whose y + h is still negative, so under the mod a
    routed sprite reaches OAM later than it does with the gap on, or not at
    all."""
    logs = {}
    for tag, settings, extra in (("gap on (the ROM)", None, TRACE),
                                 ("gapless, shift OFF", GAPLESS, TRACE),
                                 ("gapless + shift", GAPLESS,
                                  dict(list(TRACE.items()) +
                                       list(SHIFT_ON.items())))):
        t = re.sub(r"\W+", "_", tag)
        _, txt, _ = run("timing_%s" % t, frames, settings=settings, extra=extra)
        logs[tag] = entries(txt)
    print("THE ROUTED SPRITE (a2=%s), per arm" % BOMB)
    print("  %-22s %8s %8s %8s %8s" % ("arm", "sighted", "first f", "rows d0",
                                       "on screen"))
    for tag, rows in logs.items():
        first = None
        seen = 0
        ons = []
        for f, es in rows:
            for e in es:
                if e["a2"] != BOMB:
                    continue
                seen += 1
                ons.append(e["d0"])
                if first is None and e["top"] > 0:
                    first = f
        print("  %-22s %8d %8s %8s %8d"
              % (tag, seen, first if first is not None else "-",
                 "%d..%d" % (min(ons), max(ons)) if ons else "-",
                 sum(1 for f, es in rows for e in es
                     if e["a2"] == BOMB and e["top"] > 0)))
    print("")
    print("AND THE SAME NUMBER PER FRAME, gap-on beside each gapless arm:")
    gapl = dict(logs["gap on (the ROM)"])
    for tag in ("gapless, shift OFF", "gapless + shift"):
        agree = differ = 0
        for f, es in logs[tag]:
            g = {(e["oam"], e["a2"]): e for e in gapl.get(f, [])}
            for e in es:
                if e["a2"] != BOMB:
                    continue
                o = g.get((e["oam"], e["a2"]))
                if o is None:
                    continue
                if o["d0"] == e["d0"]:
                    agree += 1
                else:
                    differ += 1
        print("  %-22s %d frame-entries agree with gap-on's row, %d differ"
              % (tag, agree, differ))
    return logs


if __name__ == "__main__":
    step = sys.argv[1] if len(sys.argv) > 1 else ""
    if step == "arms":
        arms()
    elif step == "register":
        register(int(sys.argv[2]) if len(sys.argv) > 2 else 400)
    elif step == "diff":
        diff(int(sys.argv[2]) if len(sys.argv) > 2 else 300)
    elif step == "crossing":
        crossing(int(sys.argv[2]) if len(sys.argv) > 2 else 1400)
    elif step == "touch":
        touch()
    elif step == "shots":
        for f in (sys.argv[2:] or ["300"]):
            shot(int(f))
    elif step == "full":
        for f in (sys.argv[2:] or ["300"]):
            full(int(f))
    elif step == "timing":
        timing(int(sys.argv[2]) if len(sys.argv) > 2 else 400)
    else:
        print(__doc__)
