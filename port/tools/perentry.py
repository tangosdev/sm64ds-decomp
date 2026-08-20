"""The gapless PER-ENTRY correction's evidence, arm by arm.

Read THE ROUTER HOOK at the foot of port/hal/screen_gap.cpp for what the
correction is, THE OBJECT SHIFT in port/ntr/include/ntr/ppu.h for the geometry
it lands in, and port/tools/objshift.py for the lane that falsified the LAYER
version of the same idea. This tool is that lane's tool with the arms changed:
its steps are re-run here against the mechanism that replaced the one it killed,
so the two sets of numbers are read off the same probes.

  python port/tools/perentry.py arms       every arm's image size and md5,
                                           including the two containment arms
                                           that must still be the tip's
  python port/tools/perentry.py register   THE ACCEPTANCE: per sprite identity,
                                           gap-on's display row minus each
                                           gapless arm's, frame for frame, plus
                                           the count of submissions gap-on makes
                                           that the arm does not
  python port/tools/perentry.py diff       the composed images against each
                                           other by region
  python port/tools/perentry.py crossing   lane STRADDLE's census, verbatim, in
                                           both arms
  python port/tools/perentry.py timing     when the routed sprite first has rows
                                           on the screen, per arm
  python port/tools/perentry.py touch      the stylus inverse over the bands
  python port/tools/perentry.py clipped    WHICH SPRITE IS CUT OFF AT THE TOP,
                                           per identity, in every arm
  python port/tools/perentry.py shots F    the side by side at frame F
  python port/tools/perentry.py full F     one whole composed frame at frame F

THE THREE ARMS, and all three are the same binary, which is what
notes/port-selftest-bmp-gate.md requires before two BMPs may be compared:

  gap on (the ROM)        GaplessMinigames off. The ground truth: Bob-omb
                          Squad's own code reads the framework gap word nowhere,
                          so the simulation is bit-for-bit the same in every arm
                          and the only thing that differs is the mapping of
                          world rows onto the two engines.
  gapless, per entry      the default. The correction is applied at the five
                          routers' own calls.
  gapless, per entry OFF  SM64DS_GAPLESS_PER_ENTRY=0, the picture the mod
                          composed before this lane.

PAIRING IS BY IDENTITY, NOT BY OAM SLOT AND NOT BY POSITION, and both halves of
that are a change from objshift.py's register on purpose.

NOT BY SLOT, because the correction lands before OAM::Render's culls: entries
the mod used to lose now take slots, and every later entry in that frame is
renumbered. A slot-keyed pairing reports the renumbering as a disagreement.

NOT BY POSITION EITHER, because OAM::Render's rotation mixes the two axes --
`x = ((sx * c - sy * s) >> 12) - w / 2` with sy taken from the submitted y -- so
correcting a SPINNING sprite by 32 rows moves its x as well. An x-keyed pairing
reported the routed 32x32 double-size sprite as absent from an arm that had all
385 of its sightings. See ident() for the key that is left: the tile, the
palette and the priority with the decoded size beside them, which is what the
sprite IS. Several of one identity in a frame pair nearest-first on x.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

OUT = os.path.abspath(os.path.join(HERE, "..", "..", "runs", "mg5", "out",
                                   "perentry"))
os.makedirs(OUT, exist_ok=True)
os.environ["OBJSHIFTOUT"] = OUT

import gapproof as G          # noqa: E402
import objshift as O          # noqa: E402

SCENE = O.SCENE
SCREEN_H = O.SCREEN_H
GAPLESS = O.GAPLESS
TRACE = O.TRACE
PER_OFF = {"SM64DS_GAPLESS_PER_ENTRY": "0"}
LAYER_ON = {"SM64DS_GAPLESS_OBJ_SHIFT": "1"}
BOMB = O.BOMB                 # a2=1010, the routed 32x32 double-size sprite
LAKITU = O.LAKITU             # a2=211c, what lane LAKITU named Lakitu

GAPON = ("gap on (the ROM)", None, {})
PER = ("gapless, per entry", GAPLESS, {})
OFF = ("gapless, per entry OFF", GAPLESS, PER_OFF)
LAYER = ("gapless, layer shift", GAPLESS, LAYER_ON)

SHOTS = os.path.join(OUT, "shots")


def tagof(name):
    return re.sub(r"\W+", "_", name)


def traced(arm, frames):
    """One traced headless run of an arm; returns [(frame, [entry...])]."""
    name, settings, extra = arm
    env = dict(TRACE)
    env.update(extra)
    _, txt, _ = O.run("trace_%s" % tagof(name), frames, settings=settings,
                      extra=env)
    return O.entries(txt)


# ---- the arms ---------------------------------------------------------------

def arms(frames=300):
    """Every arm's picture, and the two that must not have moved.

    CONTAINMENT IS TWO CLAIMS HERE, not one. gap on and MinigameGap false must
    be byte-identical to a build of the base commit, because neither can reach
    the correction at all -- it is gated on the mod being engaged for the scene
    now running. And `per entry OFF` must be byte-identical to the base's
    GAPLESS default, because that switch is the whole A/B: with it set the
    routers' wrappers push an int and pop it and nothing else in the program
    behaves differently."""
    print("%-26s %-9s %s" % ("run", "image", "md5"))
    out = {}
    for name, settings, extra in (PER, OFF, LAYER, GAPON,
                                  ("MinigameGap false", {"MinigameGap": False},
                                   {})):
        bmp = os.path.join(OUT, "arm_%s.bmp" % tagof(name))
        G.scene_run(SCENE, frames, bmp, settings=settings,
                    extra_env=dict(extra))
        dims = "%dx%d" % G.read_bmp(bmp)[:2] if os.path.exists(bmp) else "-"
        md5 = G.md5(bmp) if os.path.exists(bmp) else "NO BMP"
        print("%-26s %-9s %s" % (name, dims, md5))
        out[name] = (dims, md5)
    return out


# ---- the register -----------------------------------------------------------

def kind(e):
    return (" dbl" if "dbl" in e["kind"]
            else (" aff" if e["kind"] else ""))


def ident(e):
    """THE SPRITE, INDEPENDENT OF WHERE IT IS DRAWN, which is the only kind of
    key that can pair two arms of this A/B.

    NOT (a0, a1): attribute 0 carries the y, so a moved sprite has a different
    a0 by construction, and attribute 1 carries the x, which moves WITH the y
    for a rotated entry -- OAM::Render's rotation mixes them (`x = ((sx * c -
    sy * s) >> 12) - w / 2`), so a 32 row correction moves a spinning sprite
    sideways too. Keying on either of those reports the correction as a missing
    sprite, which it is not. a2 is the tile, the palette and the priority, and
    with the decoded size and affine kind beside it that is what the sprite IS.
    """
    return (e["a2"], e["w"], e["h"], kind(e))


def is_reset_entry(e):
    """The OAM RESET PATTERN, which is not a sprite and is not evidence.

    OAM::Reset writes 0x000000c0 into all 128 shadow entries, so every slot the
    frame does not use decodes as an 8x8 tile-0 sprite parked at engine row 192.
    The compositor rasterises them (they are not the disabled bit) and the trace
    prints them, ~115 a frame in every arm, and they are the same 115 in every
    arm. Counted separately below rather than left to drown the table."""
    return e["a0"] == "00c0" and e["a1"] == "0000" and e["a2"] == "0000"


def pair_frame(a, b):
    """[(entry_a, entry_b)] for one frame, paired by identity then by column.

    Where a frame holds several of one identity they are paired NEAREST FIRST
    on x: two bob-ombs of the same tile are told apart by where they are, and a
    correction of 32 rows cannot move one past another."""
    idx = {}
    for e in b:
        idx.setdefault(ident(e), []).append(e)
    out = []
    for e in a:
        q = idx.get(ident(e))
        if not q:
            out.append((e, None))
            continue
        best = min(range(len(q)), key=lambda i: (abs(q[i]["x"] - e["x"]),
                                                 abs(q[i]["d0"] - e["d0"])))
        out.append((e, q.pop(best)))
    return out


def register(frames=400):
    """WHERE EVERY TOP-SCREEN SPRITE IS DRAWN, in every arm, frame for frame.

    THE TWO NUMBERS THIS LANE IS JUDGED ON.

    1. THE ROUTED SPRITE MUST LAND ON GAP-ON'S ROW AND THE OTHERS MUST NOT MOVE.
       Lane OBJSHIFT measured that a2=1010 is the one identity of twelve on this
       scene that follows the framework's G word, and that the layer shift put
       the other eleven 32 rows down a screen they were never displaced from. So
       the correction is right exactly when the delta table below is 0 for every
       identity in the per-entry arm, including 1010.

    2. THE SUBMISSIONS THE MOD LOSES OUTRIGHT. OAM::Render culls at submission,
       so an entry the mod pushed above the screen never reaches OAM and no
       display pass can draw it. Counted here as a multiset difference per
       frame, per identity, which is renumbering-proof: how many sightings
       gap-on has that the arm does not."""
    rows = {}
    for arm in (GAPON, PER, OFF):
        rows[arm[0]] = traced(arm, frames)
        print("%-26s %d frame(s) traced" % (arm[0], len(rows[arm[0]])))
    gap = dict(rows[GAPON[0]])

    for arm in (PER[0], OFF[0]):
        print("\n== %s: gap-on's display row minus this arm's ==" % arm)
        per = {}
        for f, es in rows[arm]:
            mine = [e for e in es if not is_reset_entry(e)]
            theirs = [e for e in gap.get(f, []) if not is_reset_entry(e)]
            for e, g in pair_frame(mine, theirs):
                if g is None:
                    continue
                k = (e["a2"], "%dx%d%s" % (e["w"], e["h"], kind(e)))
                d = g["d0"] - e["d0"]
                s = per.setdefault(k, {"n": 0, "d": {}})
                s["n"] += 1
                s["d"][d] = s["d"].get(d, 0) + 1
        print("    %-8s %-12s %6s  %s" % ("a2", "size", "seen", "delta"))
        bad = 0
        for k in sorted(per, key=lambda k: (-per[k]["n"], k)):
            s = per[k]
            spread = ", ".join("%+d on %d of %d" % (d, n, s["n"])
                               for d, n in sorted(s["d"].items()))
            if set(s["d"]) != {0}:
                bad += 1
            print("    %-8s %-12s %6d  %s" % (k[0], k[1], s["n"], spread))
        print("    %d identity/ies are not at delta 0 on every sighting" % bad)

    print("\n== the submissions each arm never makes ==")
    print("  %-26s %8s %8s %8s"
          % ("arm", "sighted", "gap-on", "missing"))
    for arm in (PER[0], OFF[0]):
        here = dict(rows[arm])
        seen = miss = gapn = 0
        per_id = {}
        for f, ges in gap.items():
            mine = {}
            for e in here.get(f, []):
                if is_reset_entry(e):
                    continue
                mine[ident(e)] = mine.get(ident(e), 0) + 1
            want = {}
            for e in ges:
                if is_reset_entry(e):
                    continue
                want[ident(e)] = want.get(ident(e), 0) + 1
            for k, n in want.items():
                gapn += n
                d = n - mine.get(k, 0)
                if d > 0:
                    miss += d
                    per_id[k[0]] = per_id.get(k[0], 0) + d
            seen += sum(mine.values())
        print("  %-26s %8d %8d %8d" % (arm, seen, gapn, miss))
        for a2 in sorted(per_id, key=lambda a: -per_id[a]):
            print("      a2=%s: %d" % (a2, per_id[a2]))
    return rows


# ---- what is cut off at the top ---------------------------------------------

def clipped(frames=400):
    """WHICH SPRITE LOSES ROWS OFF THE TOP OF THE TOP SCREEN, per identity.

    THE ORIGINAL REPORT was a sprite jammed against the wooden beam and cut, and
    it has been attributed twice already: lane LAKITU named a2=211c, and lane
    OBJSHIFT measured 211c at the SAME rows in gap-on and in gapless on all 703
    of its sightings, which means the mod cannot be what cut it. This is the
    measurement that settles it, and it is not an opinion about a picture: the
    per-entry trace prints, for every drawable entry of every frame, how many of
    the sprite's own box rows fall above display row 0. Summed per identity, per
    arm, that is exactly `how much of this sprite is cut off at the top', and
    the identity the mod is responsible for is the one whose number the arms
    disagree about."""
    out = {}
    for arm in (GAPON, PER, OFF):
        out[arm[0]] = traced(arm, frames)
    ids = {}
    for name, rws in out.items():
        for f, es in rws:
            for e in es:
                if is_reset_entry(e):
                    continue
                k = (e["a2"], "%dx%d%s" % (e["w"], e["h"], kind(e)))
                s = ids.setdefault(k, {})
                a = s.setdefault(name, {"seen": 0, "above": 0, "cut": 0,
                                        "gone": 0})
                a["seen"] += 1
                a["above"] += e["above"]
                if e["above"] > 0:
                    a["cut"] += 1
                if e["top"] == 0 and e["band"] == 0:
                    a["gone"] += 1
    print("ROWS LOST OFF THE TOP OF THE TOP SCREEN, per identity, per arm")
    print("  %-8s %-12s %-26s %7s %7s %9s"
          % ("a2", "size", "arm", "seen", "cut", "rows lost"))
    for k in sorted(ids, key=lambda k: -sum(a["above"]
                                            for a in ids[k].values())):
        for name in (GAPON[0], OFF[0], PER[0]):
            a = ids[k].get(name)
            if not a:
                print("  %-8s %-12s %-26s %7s %7s %9s"
                      % (k[0], k[1], name, "-", "-", "-"))
                continue
            print("  %-8s %-12s %-26s %7d %7d %9d"
                  % (k[0], k[1], name, a["seen"], a["cut"], a["above"]))
        print("")
    return ids


# ---- the picture ------------------------------------------------------------

def diff(frames=300):
    """The composed images against each other, by region."""
    paths = {}
    for name, settings, extra in (GAPON, PER, OFF):
        p = os.path.join(OUT, "diff_%s.bmp" % tagof(name))
        G.scene_run(SCENE, frames, p, settings=settings, extra_env=dict(extra))
        paths[name] = p
        w, h, _ = G.read_bmp(p)
        print("%-26s %dx%d  %s" % (name, w, h, G.md5(p)))
    wa, ha, ra = G.read_bmp(paths[GAPON[0]])
    for name in (PER[0], OFF[0]):
        wb, hb, rb = G.read_bmp(paths[name])
        print("\n== gap on vs %s ==" % name)
        if (wa, ha) != (wb, hb):
            print("  different image shapes (%dx%d vs %dx%d): the top screen "
                  "is still row for row comparable, the rest is not"
                  % (wa, ha, wb, hb))
        n = 0
        for y in range(SCREEN_H):
            for x in range(wa):
                if ra[y][x] != rb[y][x]:
                    n += 1
        tot = SCREEN_H * wa
        print("  top screen     %d of %d pixels differ (%.2f%%), %.2f%% "
              "identical" % (n, tot, 100.0 * n / tot, 100.0 - 100.0 * n / tot))
        if (wa, ha) != (wb, hb):
            continue
        band_h = ha - SCREEN_H * 2
        for label, y0, y1 in (("band", SCREEN_H, SCREEN_H + band_h),
                              ("bottom screen", SCREEN_H + band_h, ha)):
            n = 0
            for y in range(y0, y1):
                for x in range(wa):
                    if ra[y][x] != rb[y][x]:
                        n += 1
            tot = (y1 - y0) * wa
            if not tot:
                continue
            print("  %-14s %d of %d pixels differ (%.2f%%)"
                  % (label, n, tot, 100.0 * n / tot))
    return paths


def shot(frame, keep=192):
    """THE SIDE BY SIDE TANGO JUDGES: gap on | per entry OFF | per entry.

    All three crops are the TOP SCREEN and nothing else, the same rows of the
    same thing, so a difference is a difference in the picture and not in the
    crop. top_y is 0 in all three layouts."""
    os.makedirs(SHOTS, exist_ok=True)
    crops = {}
    for name, settings, extra in (GAPON, OFF, PER):
        p = os.path.join(SHOTS, "f%d_%s.bmp" % (frame, tagof(name)))
        G.scene_run(SCENE, frame, p, settings=settings, extra_env=dict(extra))
        w, h, rows = G.read_bmp(p)
        crops[name] = rows[:min(keep, SCREEN_H)]
    n = len(crops[GAPON[0]])
    side = [crops[GAPON[0]][y] + [O.SEP] * 8 + crops[OFF[0]][y] +
            [O.SEP] * 8 + crops[PER[0]][y] for y in range(n)]
    out = os.path.join(SHOTS, "f%d_topscreens.bmp" % frame)
    O.write_bmp(out, side)
    print("wrote %s: gap on | per entry OFF | per entry, top screen, %d host "
          "rows" % (out, n))
    tot = n * len(crops[GAPON[0]][0])
    for name in (OFF[0], PER[0]):
        same = sum(1 for y in range(n) for x in range(len(crops[name][0]))
                   if crops[GAPON[0]][y][x] == crops[name][y][x])
        print("  gap on vs %-26s %d of %d pixels identical (%.2f%%)"
              % (name, same, tot, 100.0 * same / tot))
    return out


def full(frame):
    """ONE WHOLE COMPOSED FRAME, gap on beside the per-entry arm, all rows."""
    os.makedirs(SHOTS, exist_ok=True)
    a = os.path.join(SHOTS, "f%d_full_gapon.bmp" % frame)
    b = os.path.join(SHOTS, "f%d_full_perentry.bmp" % frame)
    G.scene_run(SCENE, frame, a, settings=None)
    G.scene_run(SCENE, frame, b, settings=GAPLESS)
    wa, ha, ra = G.read_bmp(a)
    wb, hb, rb = G.read_bmp(b)
    n = min(ha, hb)
    side = [ra[y] + [O.SEP] * 8 + rb[y] for y in range(n)]
    out = os.path.join(SHOTS, "f%d_full.bmp" % frame)
    O.write_bmp(out, side)
    print("wrote %s: %dx%d beside %dx%d" % (out, wa, ha, wb, hb))
    return out


# ---- the crossing -----------------------------------------------------------

def crossing(frames=1400):
    """LANE STRADDLE'S CENSUS, VERBATIM, IN BOTH ARMS.

    Same probe, same trace, same reader as lane OBJSHIFT ran it with, which is
    lane STRADDLE's own: two scripted slingshot launches through
    SM64DS_TOUCH_PROBE, grab the ball where it rests, drag, release. Reusing the
    probe verbatim is the point -- a different probe is a different crossing and
    the two tables could not be read against each other. The obligation is unchanged by the correction: a ball
    the router gave to engine A at world -1 has fifteen more rows below world 0
    that engine B was never given, and the seam pass is what draws them."""
    tables = {}
    for name, settings, extra in (OFF, PER):
        env = {"SM64DS_GAP_STRADDLE_TRACE": "1",
               "SM64DS_TOUCH_PROBE": O.PROBE}
        env.update(extra)
        r, text, bmp = O.run("cross_%s" % tagof(name), frames,
                             settings=settings, extra=env, windowed=True)
        rows = []
        for ln in text.splitlines():
            m = O.STR_DRAWN.search(ln)
            if m:
                rows.append({"f": int(m.group(1)), "slot": int(m.group(2)),
                             "x": int(m.group(5)), "y": int(m.group(6)),
                             "why": m.group(7), "eng": m.group(8),
                             "syn": int(m.group(22)),
                             "tot": (int(m.group(23)), int(m.group(24)))})
                continue
            m = O.STR_NONE.search(ln)
            if m:
                rows.append({"f": int(m.group(1)), "slot": int(m.group(2)),
                             "x": int(m.group(5)), "y": int(m.group(6)),
                             "why": m.group(9), "eng": None})
        tables[name] = rows
        done = [r for r in rows if r.get("eng")]
        whole = [r for r in done if r["tot"][0] == r["tot"][1]]
        print("%-26s %d census line(s), %d completion(s), %d of %d reach all "
              "their texels" % (name, len(rows), len(done), len(whole),
                                len(done)))
        for r in done:
            print("    f%-5d slot %d at (%d,%d) %-14s from %s, %d "
                  "synthesized, %d/%d" % (r["f"], r["slot"], r["x"], r["y"],
                                          r["why"], r["eng"], r["syn"],
                                          r["tot"][0], r["tot"][1]))
    a = ["%(f)d %(slot)d %(x)d %(y)d %(why)s" % r for r in tables[OFF[0]]]
    b = ["%(f)d %(slot)d %(x)d %(y)d %(why)s" % r for r in tables[PER[0]]]
    same = sum(1 for i in range(min(len(a), len(b))) if a[i] == b[i])
    print("\nthe two tables agree on %d of %d / %d line(s)"
          % (same, len(a), len(b)))
    return tables


# ---- when the routed sprite arrives -----------------------------------------

def timing(frames=400):
    """WHEN A ROUTED SPRITE FIRST HAS ROWS ON THE SCREEN, in every arm.

    Lane OBJSHIFT's method, unchanged: the routed entry's own row out of the
    per-entry trace rather than a bounding box read off the picture. World row r
    reaches the top screen at engine row r + 0xc0 + G, so the first frame with
    rows on the screen is a function of G alone and gap-on is the answer to
    match."""
    rows = {}
    for arm in (GAPON, PER, OFF):
        rows[arm[0]] = traced(arm, frames)
    print("THE ROUTED SPRITE (a2=%s), per arm" % BOMB)
    print("  %-26s %8s %9s %10s" % ("arm", "sighted", "first f", "rows d0"))
    for name in (GAPON[0], PER[0], OFF[0]):
        first, seen, ons = None, 0, []
        for f, es in rows[name]:
            for e in es:
                if e["a2"] != BOMB:
                    continue
                seen += 1
                ons.append(e["d0"])
                if first is None and e["top"] > 0:
                    first = f
        print("  %-26s %8d %9s %10s"
              % (name, seen, first if first is not None else "-",
                 "%d..%d" % (min(ons), max(ons)) if ons else "-"))
    gap = dict(rows[GAPON[0]])
    print("\nAND THE SAME NUMBER PER FRAME, gap-on beside each gapless arm:")
    for name in (PER[0], OFF[0]):
        agree = differ = 0
        for f, es in rows[name]:
            for e, g in pair_frame([e for e in es if e["a2"] == BOMB],
                                   [e for e in gap.get(f, [])
                                    if e["a2"] == BOMB]):
                if g is None:
                    continue
                if g["d0"] == e["d0"]:
                    agree += 1
                else:
                    differ += 1
        print("  %-26s %d frame-entries agree with gap-on's row, %d differ"
              % (name, agree, differ))
    return rows


# ---- the stylus -------------------------------------------------------------

def touch(frames=30):
    """The stylus inverse over the bands, in all three layouts."""
    pts = [(4, 4), (4, 200), (4, 380), (4, 400), (4, 440), (4, 500), (4, 800),
           (4, 1000)]
    out = {}
    for name, settings, extra in (PER, OFF, GAPON):
        env = dict(extra)
        env["SM64DS_TOUCH_CLIENT_PROBE"] = ";".join("%d,%d" % p for p in pts)
        bmp = os.path.join(OUT, "touch_%s.bmp" % tagof(name))
        r = G.scene_run(SCENE, frames, bmp, settings=settings, extra_env=env)
        lines = [l.strip() for l in (r.stdout + r.stderr).splitlines()
                 if "[touchmap]" in l]
        print("== %s ==" % name)
        for l in lines:
            print("  " + l)
        out[name] = lines
    a, b = out[PER[0]], out[GAPON[0]]
    print("\nper entry vs gap on: %d of %d touch answer(s) identical"
          % (sum(1 for i in range(min(len(a), len(b))) if a[i] == b[i]),
             len(b)))
    return out


# ---- the band with something in it ------------------------------------------

def cross_full(frame=636):
    """ONE WHOLE COMPOSED FRAME WITH A BALL MID-CROSSING, gap on beside this arm.

    PROBE-DRIVEN AND HEADLESS. Lane STRADDLE's slingshot script is what puts a
    ball on the seam at a known frame, and a headless run composes once, at the
    end, so asking for N frames gives the picture at frame N and no other. The
    frame count IS the probe here.

    AND THE [hinge] TRACE BESIDE IT, because the picture alone cannot say WHICH
    entries the band drew. That is the whole difference between this arm and the
    layer arm lane OBJSHIFT measured: at this same frame its band carried the
    crossing ball AND the score row, which lives at engine y 168 in every arm
    and belongs on the screen. Here the band is bound to the routed set, so the
    trace should name the ball and nothing else."""
    os.makedirs(SHOTS, exist_ok=True)
    caps = {}
    for name, settings, extra in (GAPON, PER, LAYER):
        env = {"SM64DS_TOUCH_PROBE": O.PROBE, "SM64DS_OBJSHIFT_TRACE": "1"}
        env.update(extra)
        p = os.path.join(SHOTS, "f%d_cross_%s.bmp" % (frame, tagof(name)))
        r = G.scene_run(SCENE, frame, p, settings=settings, extra_env=env)
        txt = r.stdout + r.stderr
        caps[name] = p
        lines = [l.strip() for l in txt.splitlines() if "[hinge]" in l]
        tail = [l for l in lines if ("f%d " % frame) in l]
        print("== %s ==" % name)
        for l in (tail or lines[-8:]):
            print("  " + l)
        if not lines:
            print("  no band in this arm")
    wa, ha, ra = G.read_bmp(caps[GAPON[0]])
    wb, hb, rb = G.read_bmp(caps[PER[0]])
    n = min(ha, hb)
    side = [ra[y] + [O.SEP] * 8 + rb[y] for y in range(n)]
    out = os.path.join(SHOTS, "f%d_cross_full.bmp" % frame)
    O.write_bmp(out, side)
    print("")
    print("wrote %s: gap on %dx%d beside per entry %dx%d"
          % (out, wa, ha, wb, hb))
    if (wa, ha) == (wb, hb):
        band_h = ha - SCREEN_H * 2
        n_diff = sum(1 for y in range(SCREEN_H, SCREEN_H + band_h)
                     for x in range(wa) if ra[y][x] != rb[y][x])
        print("  the band: %d of %d pixel(s) differ from gap-on's fill, which "
              "is the crossing object the hardware cannot show"
              % (n_diff, band_h * wa))
    return out


# ---- what the routed sprite actually IS -------------------------------------

BOMBLINE = re.compile(r"\[bomb\] f(\d+) G=(-?\d+) slot(\d+) world=\((-?\d+),"
                      r"(-?\d+)\) state=(-?\d+) topsy=(-?\d+) botsy=(-?\d+)")


def identify(frames=200):
    """WHAT a2=1010 IS, off the game's own slot table rather than off a picture.

    hal/scene_mg.cpp's SM64DS_PCH_BALL_TRACE prints dScMgPachinko_c's own
    parachuting bob-omb slots -- 30 of them at scene + 0x4660, world position,
    live flag, state -- and the row the top engine would take each to. The
    per-entry trace prints where the OAM entries really are. If the routed
    identity is the bob-ombs then on every frame the set of `topsy` values from
    the slot table is the set of rows the a2=1010 entries were submitted at, and
    that is a correspondence rather than a resemblance.

    Run with the gap ON, because that is the arm whose rows are the ROM's."""
    env = dict(TRACE)
    env["SM64DS_PCH_BALL_TRACE"] = "1"
    _, txt, _ = O.run("identify_gapon", frames, settings=None, extra=env)
    bombs = {}
    for ln in txt.splitlines():
        m = BOMBLINE.search(ln)
        if m:
            bombs.setdefault(int(m.group(1)), []).append(
                {"slot": int(m.group(3)), "wx": int(m.group(4)),
                 "wy": int(m.group(5)), "top": int(m.group(7))})
    ents = dict(O.entries(txt))
    hit = miss = frames_seen = 0
    sample = []
    agreed = []
    cels = {}
    for f in sorted(bombs):
        live = bombs[f]
        rows = [e for e in ents.get(f, []) if e["a2"] == BOMB]
        if not live and not rows:
            continue
        frames_seen += 1
        # the entry's SUBMITTED row is display minus the shift it carried
        got = sorted(e["d0"] - e["shift"] for e in rows)
        # OAM::Render puts the box at py + the cel offset, which is the same
        # constant for every one of these, so the two lists match up to that
        # one offset. Solve for it once per frame and require it to be equal.
        want = sorted(b["top"] for b in live)
        if len(got) != len(want):
            miss += 1
            if len(sample) < 8:
                sample.append((f, "%d slot(s) live, %d entr(ies)"
                               % (len(want), len(got))))
            continue
        offs = set(g - w for g, w in zip(got, want))
        if len(offs) == 1:
            hit += 1
            o = list(offs)[0]
            cels[o] = cels.get(o, 0) + 1
            if len(agreed) < 6 and want:
                agreed.append((f, "%d bob-omb(s), slot row(s) %s, entry row(s) "
                               "%s, cel offset %d" % (len(want), want, got, o)))
        else:
            miss += 1
            if len(sample) < 8:
                sample.append((f, "offsets %s" % sorted(offs)))
    print("a2=%s against dScMgPachinko_c's own parachuting bob-omb slots, "
          "gap on, %d frames" % (BOMB, frames))
    print("  %d frame(s) where the two agree exactly, %d where they do not"
          % (hit, miss))
    print("  the constant offset between the two, which is the sprite's own cel "
          "offset: %s" % ", ".join("%d on %d frame(s)" % (o, n)
                                   for o, n in sorted(cels.items())))
    print("  AGREEING frames:")
    for f, why in agreed:
        print("    f%-5d %s" % (f, why))
    print("  DISAGREEING frames:")
    for f, why in sample:
        print("    f%-5d %s" % (f, why))
    return hit, miss


if __name__ == "__main__":
    step = sys.argv[1] if len(sys.argv) > 1 else ""
    n = int(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[2].isdigit() else 0
    if step == "arms":
        arms(n or 300)
    elif step == "register":
        register(n or 400)
    elif step == "crossfull":
        for f in (sys.argv[2:] or ["636"]):
            cross_full(int(f))
    elif step == "identify":
        identify(n or 200)
    elif step == "clipped":
        clipped(n or 400)
    elif step == "diff":
        diff(n or 300)
    elif step == "crossing":
        crossing(n or 1400)
    elif step == "timing":
        timing(n or 400)
    elif step == "touch":
        touch()
    elif step == "shots":
        for f in (sys.argv[2:] or ["300"]):
            shot(int(f))
    elif step == "full":
        for f in (sys.argv[2:] or ["300"]):
            full(int(f))
    else:
        print(__doc__)
