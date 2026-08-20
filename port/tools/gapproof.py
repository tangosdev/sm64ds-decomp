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


def expect_ambient(w, band_h, top_edge, bot_edge):
    """The band the description asks for, rows[k][x] = (r, g, b)."""
    tops = [avg_span(top_edge, *col_bounds(w, c)[2:]) for c in range(AMBIENT_COLS)]
    bots = [avg_span(bot_edge, *col_bounds(w, c)[2:]) for c in range(AMBIENT_COLS)]
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
              "SM64DS_TOUCH_PROBE", "SM64DS_MG_SCORE_TRACE"):
        env.pop(k, None)
    env["SM64DS_SCENE"] = str(scene)
    env["SM64DS_SCENE_FRAMES"] = str(frames)
    env["SM64DS_FAULTS_FATAL"] = "1"
    env["SM64DS_SCENE_BMP_STACKED"] = out_bmp
    env["SM64DS_NO_PLAYLOG"] = "1"
    if extra_env:
        env.update(extra_env)
    clear_settings()
    if settings is not None:
        write_settings(**settings)
    try:
        r = subprocess.run([os.path.join(BUILD, "walk_window.exe")], cwd=BUILD,
                           env=env, capture_output=True, text=True,
                           errors="replace", timeout=600)
    finally:
        clear_settings()
    return r


if __name__ == "__main__":
    print(__doc__)
