"""The screen gap's evidence: BMP reading, a settings.json runner, and a SECOND
implementation of the ambient band.

Read tools/gapdrive.py for the steps; this is the library under it.

WHY IT EXISTS AT ALL. Everything the gap feature claims is a claim about pixels
and arithmetic, and on this machine a windowed capture is not reproducible run
to run -- so "look at it" is not a check. Every claim is taken instead through
the two paths that ARE reproducible: SM64DS_SCENE_BMP_STACKED, which writes the
composed image of the last frame to a file, and SM64DS_TOUCH_CLIENT_PROBE,
which drives the real mappers over client points and prints what they answer.

expect_ambient IS DELIBERATELY NOT A PORT OF THE C. It is the band built from
the description -- twenty-four columns, each sampling its own width plus a halo
on each side of both edge rows, a vertical ramp between them, interpolated
between column centres -- in the same truncating integer arithmetic. A
transcription of ntr/ppu_sub.cpp's expression would only prove that Python can
do what C did; two implementations agreeing to the byte is a check.

SETTINGS.JSON IS THE CHANNEL, deliberately. The launcher rebuilds the child
environment, so an environment variable never reaches a player's game, and a
proof driven through one would be proving a path nobody runs. The file is
written next to the exe -- the first candidate hal/host_settings.cpp tries --
and REMOVED again when the run ends, because a settings.json left behind would
be read by the next battery run.
"""
import hashlib
import json
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
BUILD = os.path.abspath(os.path.join(HERE, "..", "..", "build", "port"))


# ---- BMP reading -----------------------------------------------------------
# ntr::ppu_write_bmp_px writes a 24-bit bottom-up BMP with 4-byte row padding.
# Read it back into rows[y][x] = (r, g, b) with y = 0 at the TOP, which is the
# order the composed image is in and the order every claim below is stated in.

def read_bmp(path):
    with open(path, "rb") as f:
        data = f.read()
    if data[:2] != b"BM":
        raise ValueError("%s is not a BMP" % path)
    off = struct.unpack_from("<I", data, 10)[0]
    w, h = struct.unpack_from("<ii", data, 18)
    bits = struct.unpack_from("<H", data, 28)[0]
    if bits != 24:
        raise ValueError("%s is %d-bit, expected 24" % (path, bits))
    top_down = h < 0
    h = abs(h)
    stride = (w * 3 + 3) & ~3
    rows = []
    for ry in range(h):
        src = ry if top_down else (h - 1 - ry)
        base = off + src * stride
        row = []
        for x in range(w):
            b, g, r = data[base + x * 3], data[base + x * 3 + 1], data[base + x * 3 + 2]
            row.append((r, g, b))
        rows.append(row)
    return w, h, rows


def md5(path):
    return hashlib.md5(open(path, "rb").read()).hexdigest()


# ---- the ambient band, re-derived ------------------------------------------
# This is a SECOND implementation of ntr/ppu_sub.cpp's band_fill_ambient, from
# the same description rather than from that code, in the same integer
# arithmetic. Two implementations agreeing to the byte is the check; a port of
# the C expression to Python would only be checking that Python can do what C
# did.

AMBIENT_COLS = 24
# Half a column of overhang on each side, so the sample window is two column
# widths wide. See the note in ntr/ppu_sub.cpp: the design sentence says "half a
# column on each side" and glosses it "1.5 column widths", and those two do not
# agree. The code and this checker both take the words.
AMBIENT_HALO_NUM, AMBIENT_HALO_DEN = 1, 2


def col_bounds(w, c):
    """Column c's own span, and the span it SAMPLES (its own plus the halo)."""
    x0 = (c * w) // AMBIENT_COLS
    x1 = ((c + 1) * w) // AMBIENT_COLS
    halo = ((x1 - x0) * AMBIENT_HALO_NUM) // AMBIENT_HALO_DEN
    s0 = x0 - halo
    s1 = x1 + halo
    if s0 < 0:
        s0 = 0
    if s1 > w:
        s1 = w
    return x0, x1, s0, s1


def col_centre(w, c):
    x0, x1, _, _ = col_bounds(w, c)
    return (x0 + x1) // 2


def avg_span(row, s0, s1):
    n = s1 - s0
    r = sum(row[x][0] for x in range(s0, s1))
    g = sum(row[x][1] for x in range(s0, s1))
    b = sum(row[x][2] for x in range(s0, s1))
    return (r // n, g // n, b // n)


def sample_cols(w, edge):
    """One edge row's twenty-four column colours, as the fill reads them."""
    return [avg_span(edge, *col_bounds(w, c)[2:]) for c in range(AMBIENT_COLS)]


def expect_ambient(w, band_h, top_edge, bot_edge):
    """The band the description asks for, rows[k][x] = (r, g, b).

    THE COLD READ, which is what a band with no history draws: the first
    ambient frame of a scene, and every frame of a still one. A run whose
    band has been following a moving edge is band_from_cols() of what the
    follower says instead -- see AmbientGlow.
    """
    return band_from_cols(w, band_h, sample_cols(w, top_edge),
                          sample_cols(w, bot_edge))


def band_from_cols(w, band_h, tops, bots):
    """The band drawn from twenty-four column colours per edge."""
    centres = [col_centre(w, c) for c in range(AMBIENT_COLS)]

    def lerp_x(vals, x):
        if x <= centres[0]:
            return vals[0]
        if x >= centres[-1]:
            return vals[-1]
        c = 0
        while c + 1 < AMBIENT_COLS and centres[c + 1] <= x:
            c += 1
        a, b = vals[c], vals[c + 1]
        span = centres[c + 1] - centres[c]
        t = x - centres[c]
        return tuple((a[i] * (span - t) + b[i] * t) // span for i in range(3))

    out = []
    for k in range(band_h):
        # the vertical ramp, sampled at the row's own centre so the first row is
        # not the top edge exactly and the last is not the bottom edge exactly:
        # the band sits BETWEEN the two rows it interpolates.
        num = 2 * k + 1
        den = 2 * band_h
        row = []
        for x in range(w):
            t = lerp_x(tops, x)
            b = lerp_x(bots, x)
            row.append(tuple((t[i] * (den - num) + b[i] * num) // den
                             for i in range(3)))
        out.append(row)
    return out


# ---- the band's memory, re-derived -----------------------------------------
# A SECOND IMPLEMENTATION of ntr/ppu_sub.cpp's ambient follower, on the same
# terms as expect_ambient above: written from the description of what it does,
# not transcribed from the C, in the same integer arithmetic.
#
# THE DESCRIPTION. Each of the twenty-four columns of each of the two edges
# keeps two numbers per channel in 8-bit fixed point. `rest` is the settled
# light, and it moves towards this frame's sample slowly and unconditionally.
# `glow` is what gets drawn: it moves towards the sample FAST while the sample
# is further from rest than glow is -- the deviation is growing, so something
# is arriving -- and SLOWLY otherwise, which is a deviation collapsing and a
# glow that should fade rather than snap. Every move is the ceiling of its
# fraction of the remaining distance, at least one whole channel, and never
# past the target, so both numbers reach the sample exactly and neither can
# overshoot.
#
# WHY IT IS NOT UP-AND-DOWN. The obvious asymmetric follower attacks on a
# rising sample and decays on a falling one. Measured, the thing that crosses
# scene 368's seam is DARKER than the sky it crosses, so that shape would fade
# the crossing away instead of holding it. Growing-versus-shrinking deviation
# is direction-blind and holds both.

AMBIENT_FIX_BITS = 8
AMBIENT_ATTACK_NUM, AMBIENT_ATTACK_DEN = 7, 8      # 0.875 per frame
AMBIENT_DECAY_NUM, AMBIENT_DECAY_DEN = 1, 15       # half-life 10.05 frames
AMBIENT_REST_NUM, AMBIENT_REST_DEN = 1, 16         # half-life 10.71 frames
AMBIENT_UNIT = 1 << AMBIENT_FIX_BITS


def follow_step(d, num, den):
    """Ceiling of num/den of d, floored at a whole channel, clamped to d."""
    if d == 0:
        return 0
    a = -d if d < 0 else d
    s = (a * num + den - 1) // den
    if s < AMBIENT_UNIT:
        s = AMBIENT_UNIT
    if s > a:
        s = a
    return s if d > 0 else -s


class AmbientGlow(object):
    """The two edges' followers. Feed it sample() per composed frame, in order.

    Seeded from the first frame's own sample, so a scene's first band is the
    direct read exactly and not a ramp up out of black; the port drops the
    whole thing at every layout latch, so one minigame's glow never lights the
    next one's band.
    """

    def __init__(self):
        self.rest = None
        self.glow = None

    def sample(self, tops, bots):
        """One frame. tops/bots are 24 (r, g, b) each; returns the pair drawn."""
        want = [list(tops), list(bots)]
        if self.rest is None:
            self.rest = [[[v << AMBIENT_FIX_BITS for v in col] for col in edge]
                         for edge in want]
            self.glow = [[[v << AMBIENT_FIX_BITS for v in col] for col in edge]
                         for edge in want]
        out = []
        for e in range(2):
            edge = []
            for c in range(AMBIENT_COLS):
                px = []
                for i in range(3):
                    t = want[e][c][i] << AMBIENT_FIX_BITS
                    rest = self.rest[e][c][i]
                    rest += follow_step(t - rest, AMBIENT_REST_NUM,
                                        AMBIENT_REST_DEN)
                    self.rest[e][c][i] = rest
                    glow = self.glow[e][c][i]
                    dt = abs(t - rest)
                    dg = abs(glow - rest)
                    if dt >= dg:
                        glow += follow_step(t - glow, AMBIENT_ATTACK_NUM,
                                            AMBIENT_ATTACK_DEN)
                    else:
                        glow += follow_step(t - glow, AMBIENT_DECAY_NUM,
                                            AMBIENT_DECAY_DEN)
                    self.glow[e][c][i] = glow
                    px.append(glow >> AMBIENT_FIX_BITS)
                edge.append(tuple(px))
            out.append(edge)
        return out[0], out[1]


# ---- SM64DS_GAP_AMB_TRACE ---------------------------------------------------
# The port's own per-frame line, parsed. Four lists of twenty-four hex colours:
# what each edge's columns SAMPLED (t_raw, b_raw) and what the fill drew them
# as (t_out, b_out). The raws are this checker's input and the outs are what it
# has to land on.

TRACE_RE = re.compile(r"\[ambtrace\] f(\d+) (.*)")
TRACE_KEYS = ("t_raw", "t_out", "b_raw", "b_out")


def read_trace(text):
    """[{'f': n, 't_raw': [(r,g,b)]*24, ...}] in frame order."""
    out = []
    for line in text.splitlines():
        m = TRACE_RE.match(line.strip())
        if not m:
            continue
        rec = {"f": int(m.group(1))}
        for field in m.group(2).split():
            k, _, v = field.partition("=")
            if k not in TRACE_KEYS:
                continue
            rec[k] = [(int(c[0:2], 16), int(c[2:4], 16), int(c[4:6], 16))
                      for c in v.split(",")]
        if all(k in rec for k in TRACE_KEYS):
            out.append(rec)
    return out


# ---- running ---------------------------------------------------------------

def write_settings(**kv):
    path = os.path.join(BUILD, "settings.json")
    with open(path, "w") as f:
        json.dump(kv, f, indent=2)
    return path


def clear_settings():
    path = os.path.join(BUILD, "settings.json")
    if os.path.exists(path):
        os.remove(path)


def scene_run(scene, frames, out_bmp, settings=None, extra_env=None):
    """One headless scene run, writing the STACKED capture to out_bmp."""
    env = dict(os.environ)
    for k in ("SM64DS_LEVEL", "SM64DS_SKIP_CLASS", "SM64DS_SCENE_NO_RENDER",
              "SM64DS_SCENE_BMP", "SM64DS_SCENE_TRACE", "SM64DS_SCENE_SLOT9",
              "SM64DS_SCENE_SUBLEVEL", "SM64DS_DUAL_SCREEN", "PORT_WATCHDOG",
              "SM64DS_SCENE_WINDOW", "SM64DS_CLICK_TEST", "SM64DS_PAD_TEST",
              "SM64DS_TOUCH_PROBE", "SM64DS_MG_SCORE_TRACE",
              # run mg5 lane RNGSEED: pins the minigame RNG's menu dwell and
              # forces the seed off the .bss zero every capture here is
              # measured from. Deterministic, so an inherited one is stably
              # wrong rather than flaky -- the harder kind to spot.
              "SM64DS_RNG_MENU_FRAMES"):
        env.pop(k, None)
    env["SM64DS_SCENE"] = str(scene)
    env["SM64DS_SCENE_FRAMES"] = str(frames)
    env["SM64DS_FAULTS_FATAL"] = "1"
    env["SM64DS_SCENE_BMP_STACKED"] = out_bmp
    env["SM64DS_NO_PLAYLOG"] = "1"
    # SM64DS_NO_FOCUS. This function LOOKS headless -- it pops
    # SM64DS_SCENE_WINDOW a few lines up -- and on its own runs it is. But the
    # pop happens BEFORE the extra_env update below, so a caller that passes
    # SM64DS_SCENE_WINDOW=1 gets it back and opens a real window, and four call
    # sites do exactly that: headroom.run and objshift.run both set it under
    # their `windowed` argument (headroom.py:79, objshift.py:108), and
    # perentry.py:424 goes through the same door. Set here rather than in those
    # callers because this is the one place all of them funnel through, and
    # BEFORE the update so a caller that genuinely wants the foreground can
    # still say so -- nothing in the tree does.
    env["SM64DS_NO_FOCUS"] = "1"
    if extra_env:
        env.update(extra_env)
    clear_settings()
    if settings is not None:
        write_settings(**settings)
    try:
        # CREATE_NO_WINDOW: see battery.py's NO_CONSOLE. walk_window is a
        # console-subsystem exe, so a run launched with no console to inherit
        # gets a new console window on the desk; capture_output already pipes
        # every handle, so nothing is lost by not making one.
        # SW_SHOWMINNOACTIVE via STARTUPINFO: same reason and same proof as
        # battery.py's SI_MIN -- the game window starts minimized instead of
        # appearing over the desk; walk_window honours the request.
        _si = None
        if hasattr(subprocess, "STARTUPINFO"):
            _si = subprocess.STARTUPINFO()
            _si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            _si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
        r = subprocess.run([os.path.join(BUILD, "walk_window.exe")], cwd=BUILD,
                           env=env, capture_output=True, text=True,
                           errors="replace", timeout=600,
                           creationflags=getattr(subprocess,
                                                 "CREATE_NO_WINDOW", 0),
                           startupinfo=_si)
    finally:
        clear_settings()
    return r


if __name__ == "__main__":
    print(__doc__)
