"""Driver for the screen-gap proofs. `python tools/gapdrive.py <step>`.

Every step prints numbers, because the thing under test is a picture and this
box cannot reproduce a windowed capture. See tools/gapproof.py's header.
"""
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gapproof as G

OUT = os.environ.get("GAPOUT", os.path.join(G.BUILD, "gapout"))
os.makedirs(OUT, exist_ok=True)

SCREEN_H = 384          # the 2x tier's top screen, host rows
GAP = {368: 32, 374: 32, 376: 48, 378: 32, 366: 48, 390: 32}
MAGENTA = "#ff00ff"


def selftest():
    env = dict(os.environ)
    for k in ("SM64DS_SKIP_CLASS", "SM64DS_DUAL_SCREEN", "SM64DS_CLICK_TEST",
              "SM64DS_TOUCH_PROBE", "SM64DS_RNG_MENU_FRAMES",
              "SM64DS_SCENE", "SM64DS_SCENE_BMP",
              "SM64DS_SCENE_BMP_STACKED"):
        env.pop(k, None)
    env["SM64DS_LEVEL"] = "1"
    env["SM64DS_FAULTS_FATAL"] = "1"
    env["SM64DS_WINDOW_SELFTEST"] = "300"
    # SM64DS_NO_FOCUS: the two lines above are the arm that OPENS A GAME WINDOW
    # (a level selftest goes through main's window path), so this spawner needs
    # the same treatment battery.py's selftest_env gets. CREATE_NO_WINDOW below
    # only suppresses the console; without this line the game window still took
    # the desk. Measured invisible to the artifact this function prints: the
    # flag is WS_EX_NOACTIVATE plus SW_SHOWNOACTIVATE, and level 1 at 300 frames
    # gives a byte-identical walk_window_selftest.bmp with it off and on, which
    # is the md5 this function reports.
    env["SM64DS_NO_FOCUS"] = "1"
    G.clear_settings()
    # CREATE_NO_WINDOW: walk_window is a console-subsystem exe, so a run
    # launched from something with no console of its own gets a NEW CONSOLE
    # WINDOW on the desk. capture_output already pipes all three handles, so
    # nothing here reads it. Same one-liner and same reason as battery.py's
    # NO_CONSOLE; the game window's half is SM64DS_NO_FOCUS.
    # SW_SHOWMINNOACTIVE via STARTUPINFO: same reason and same proof as
    # battery.py's SI_MIN -- the game window starts minimized instead of
    # appearing over the desk; walk_window honours the request.
    _si = None
    if hasattr(subprocess, "STARTUPINFO"):
        _si = subprocess.STARTUPINFO()
        _si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        _si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
    r = subprocess.run([os.path.join(G.BUILD, "walk_window.exe")], cwd=G.BUILD,
                       env=env, capture_output=True, text=True,
                       errors="replace", timeout=1800,
                       creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0),
                       startupinfo=_si)
    bmp = os.path.join(G.BUILD, "walk_window_selftest.bmp")
    print("selftest rc=%d md5=%s" % (r.returncode, G.md5(bmp)))
    for ln in r.stdout.splitlines():
        if "[pos] PLAYER" in ln:
            print("   ", ln.strip())
    return r


def canary():
    r = G.scene_run(374, 300, os.path.join(OUT, "canary374.bmp"))
    print("scene 374 rc=%d" % r.returncode)
    for ln in (r.stdout + r.stderr).splitlines():
        if "state dispatch: " in ln:
            print("   ", ln.strip())
    return r


def cap(scene, tag, settings=None, frames=300, quiet=False):
    path = os.path.join(OUT, "s%d_%s.bmp" % (scene, tag))
    r = G.scene_run(scene, frames, path, settings=settings)
    ok = os.path.exists(path)
    dims = ""
    if ok:
        w, h, _ = G.read_bmp(path)
        dims = "%dx%d" % (w, h)
    if not quiet:
        print("scene %d %-14s rc=%d %s %s" % (scene, tag, r.returncode, dims,
                                              "" if ok else "NO BMP"))
        for ln in (r.stdout + r.stderr).splitlines():
            if "[gap]" in ln or "[settings] MinigameGap" in ln:
                print("     ", ln.strip())
    if r.returncode:
        print(r.stderr[-2500:])
    return path if ok else None


# ---- the claims ------------------------------------------------------------

def check_layout(scene, path):
    g = GAP[scene]
    w, h, rows = G.read_bmp(path)
    want_h = SCREEN_H * 2 + 2 * g
    print("  layout: %dx%d, expected %dx%d  %s" %
          (w, h, 512, want_h, "OK" if (w, h) == (512, want_h) else "WRONG"))
    print("  bands:  top 0..%d, gap %d..%d (%d rows), bottom %d..%d" %
          (SCREEN_H - 1, SCREEN_H, SCREEN_H + 2 * g - 1, 2 * g,
           SCREEN_H + 2 * g, h - 1))
    return rows, w, h, 2 * g


def check_ambient(scene, path):
    rows, w, h, band_h = check_layout(scene, path)
    top_edge = rows[SCREEN_H - 1]           # the top screen's bottom row
    bot_edge = rows[SCREEN_H + band_h]      # the bottom screen's top row
    want = G.expect_ambient(w, band_h, top_edge, bot_edge)
    worst = [0, 0, 0]
    bad = 0
    for k in range(band_h):
        got = rows[SCREEN_H + k]
        exp = want[k]
        for x in range(w):
            d = [abs(got[x][i] - exp[x][i]) for i in range(3)]
            if max(d) > 0:
                bad += 1
                for i in range(3):
                    worst[i] = max(worst[i], d[i])
    print("  ambient: %d of %d band pixels differ from the re-derivation, "
          "max per-channel delta r%d g%d b%d" %
          (bad, band_h * w, worst[0], worst[1], worst[2]))
    # a couple of columns spelled out, so the numbers are readable and not
    # only a verdict
    for c in (0, 12, 23):
        x0, x1, s0, s1 = G.col_bounds(w, c)
        t = G.avg_span(top_edge, s0, s1)
        b = G.avg_span(bot_edge, s0, s1)
        mid = rows[SCREEN_H + band_h // 2][(x0 + x1) // 2]
        print("    col %2d x[%3d,%3d) samples[%3d,%3d): top %s bottom %s, "
              "band midrow at its centre %s" % (c, x0, x1, s0, s1, t, b, mid))
    return bad, worst


def check_solid(scene, path, color):
    rows, w, h, band_h = check_layout(scene, path)
    want = (int(color[1:3], 16), int(color[3:5], 16), int(color[5:7], 16))
    off = 0
    for k in range(band_h):
        for x in range(w):
            if rows[SCREEN_H + k][x] != want:
                off += 1
    print("  solid:  %d of %d band pixels are not %s (want %s)" %
          (off, band_h * w, color, want))
    return off


def band_stats(path, band_h, fill):
    """Band pixels that are not the fill colour, and which rows carry them."""
    w, h, rows = G.read_bmp(path)
    n = 0
    live_rows = []
    for k in range(band_h):
        c = sum(1 for x in range(w) if rows[SCREEN_H + k][x] != fill)
        if c:
            live_rows.append((k, c))
        n += c
    return n, live_rows, w, h


def rowspans(rows):
    """[(k, count)] -> a compact "a-b" listing of the ks."""
    ks = [k for k, _ in rows]
    if not ks:
        return "none"
    out, a, p = [], ks[0], ks[0]
    for k in ks[1:]:
        if k == p + 1:
            p = k
            continue
        out.append("%d-%d" % (a, p) if a != p else str(a))
        a = p = k
    out.append("%d-%d" % (a, p) if a != p else str(a))
    return ",".join(out)


def check_peek(scene):
    """Peek's contract in two halves.

    PEEK ON paints the band BLACK and draws only what the two engines really
    submitted for those rows, so every non-black band pixel is content and the
    count of them is the measurement. The fill keys are set to a loud magenta
    on this run ON PURPOSE: peek is supposed to ignore them, so a single
    magenta pixel in the band would be the override failing, and the check
    reports magenta separately from non-black for exactly that reason.

    PEEK OFF is the pair, with the same magenta fill, and its band must be
    magenta everywhere -- which is what says the two paths did not swap."""
    g = GAP[scene]
    band_h = 2 * g
    magenta = (0xff, 0x00, 0xff)
    black = (0, 0, 0)
    off = cap(scene, "peekoff", {"GapFillMode": "solid", "GapColor": MAGENTA,
                                 "GapPeek": False})
    on = cap(scene, "peekon", {"GapFillMode": "solid", "GapColor": MAGENTA,
                               "GapPeek": True})
    n_off, _, w, _ = band_stats(off, band_h, magenta)
    n_on, rows_on, _, _ = band_stats(on, band_h, black)
    n_on_mag, _, _, _ = band_stats(on, band_h, magenta)
    total = band_h * w
    print("  peek OFF: %d of %d band pixels are not the magenta fill" %
          (n_off, total))
    print("  peek ON : %d of %d band pixels are not black (%.4f%%), in host "
          "band rows %s" % (n_on, total, 100.0 * n_on / total,
                            rowspans(rows_on)))
    print("  peek ON : %d of %d band pixels are BLACK (%.4f%%)" %
          (total - n_on, total, 100.0 * (total - n_on) / total))
    print("  peek ON : %d band pixels carry the magenta fill colour "
          "(0 = the fill keys are correctly ignored)" % (total - n_on_mag))
    print("  DS band rows carrying peek pixels: %s of 0..%d" %
          (rowspans([(k // 2, c) for k, c in rows_on if k % 2 == 0]), g - 1))
    return n_off, n_on, rows_on


# ---- the band's memory -----------------------------------------------------
#
# WHY THESE RUNS ARE WINDOWED AND THE REST ARE NOT. A headless scene run
# composes ONCE, at the end, when the capture asks for the image -- so it has
# exactly one ambient frame and a follower cannot be seen at all: it is seeded
# and read on the same frame, and the band is the cold read by construction.
# That makes headless the right mode for every other claim here and useless for
# this one. SM64DS_SCENE_WINDOW=1 runs the real frame loop, so the band gets a
# history, and the capture at the end is the last frame of it.
#
# A WINDOWED RUN FLASHES A REAL WINDOW. Expected.

# The scripted slingshot launch in Bob-omb Squad: grab the ball where it rests
# (128, 40), drag to (128, 180), let go. See src/func_ov006_020fe2e4.c for the
# grab -- it arms on the press EDGE and captures ball-minus-stylus, so the press
# has to land on the ball or the pull holds the wrong offset -- and
# src/func_ov006_020fe394.c for the release, which fires towards (128, 32) with
# a speed the pull distance sets. Straight up, across the band.
LAUNCH = "200:128:40,201-210:128:180,211-215"


def ambrun(tag, frames=400, probe=None):
    """One windowed ambient run: the capture, and the per-frame trace under it."""
    path = os.path.join(OUT, "amb_%s.bmp" % tag)
    extra = {"SM64DS_SCENE_WINDOW": "1", "SM64DS_GAP_AMB_TRACE": "1"}
    if probe:
        extra["SM64DS_TOUCH_PROBE"] = probe
    r = G.scene_run(368, frames, path, settings={"GapFillMode": "ambient"},
                    extra_env=extra)
    text = r.stdout + r.stderr
    open(os.path.join(OUT, "amb_%s.log" % tag), "w").write(text)
    return r, text, (path if os.path.exists(path) else None)


def check_glow(tag, text, path):
    """The three parities, from the trace and the capture and nothing else.

    ONE  the follower. Feed this checker's OWN AmbientGlow the t_raw/b_raw
         series the port traced, and it must produce the port's t_out/b_out on
         every frame. Two implementations of the memory agreeing.
    TWO  the samples. The LAST traced frame is the frame the capture holds, so
         its t_raw/b_raw must equal what this checker reads off the capture's
         own two edge rows. That is what stops ONE from being a check of the
         port against itself: the input series is anchored to real pixels.
    THREE the band. Draw the band from the last frame's SMOOTHED columns and
         diff it against the capture's band, which must be zero pixels.
    """
    fr = G.read_trace(text)
    print("  frames traced: %d" % len(fr))
    if not fr or not path:
        print("  NO TRACE OR NO CAPTURE -- nothing checked")
        return False

    glow = G.AmbientGlow()
    bad_frames, first_bad = 0, None
    last = None
    for rec in fr:
        tops, bots = glow.sample(rec["t_raw"], rec["b_raw"])
        if tops != rec["t_out"] or bots != rec["b_out"]:
            bad_frames += 1
            if first_bad is None:
                first_bad = rec["f"]
        last = (tops, bots)
    print("  follower: %d of %d frames differ from the re-derivation%s"
          % (bad_frames, len(fr),
             "" if first_bad is None else " (first at f%d)" % first_bad))

    w, h, rows = G.read_bmp(path)
    band_h = h - 2 * SCREEN_H
    got_t = G.sample_cols(w, rows[SCREEN_H - 1])
    got_b = G.sample_cols(w, rows[SCREEN_H + band_h])
    off = sum(1 for c in range(G.AMBIENT_COLS)
              if got_t[c] != fr[-1]["t_raw"][c] or got_b[c] != fr[-1]["b_raw"][c])
    print("  samples : %d of %d columns of the last traced frame disagree with "
          "the capture's own edge rows" % (off, 2 * G.AMBIENT_COLS))

    want = G.band_from_cols(w, band_h, last[0], last[1])
    worst, pix = [0, 0, 0], 0
    for k in range(band_h):
        for x in range(w):
            d = [abs(rows[SCREEN_H + k][x][i] - want[k][x][i]) for i in range(3)]
            if max(d):
                pix += 1
                worst = [max(worst[i], d[i]) for i in range(3)]
    print("  band    : %d of %d band pixels differ from the re-derivation, "
          "max per-channel delta r%d g%d b%d"
          % (pix, band_h * w, worst[0], worst[1], worst[2]))
    return bad_frames == 0 and off == 0 and pix == 0


def glow_series(text, col, lo, hi):
    """The per-frame edge colours of one column: the claim about the picture."""
    fr = {r["f"]: r for r in G.read_trace(text)}
    print("  frame   top edge raw    drawn        bottom edge raw drawn")
    for f in range(lo, hi + 1):
        r = fr.get(f)
        if not r:
            continue
        print("  f%-6d %-15s %-12s %-15s %s"
              % (f, r["t_raw"][col], r["t_out"][col], r["b_raw"][col],
                 r["b_out"][col]))


def touchmap(scene, points):
    """Drive the real mappers over client points and print what they answer."""
    path = os.path.join(OUT, "s%d_touch.bmp" % scene)
    r = G.scene_run(scene, 30, path,
                    extra_env={"SM64DS_TOUCH_CLIENT_PROBE":
                               ";".join("%d,%d" % p for p in points)})
    print("scene %d touch rc=%d" % (scene, r.returncode))
    for ln in (r.stdout + r.stderr).splitlines():
        if "[touchmap]" in ln or "[gap] G " in ln:
            print("   ", ln.strip())
    return r


if __name__ == "__main__":
    step = sys.argv[1]
    if step == "selftest":
        selftest()
    elif step == "canary":
        canary()
    elif step == "layout":
        for sc in (368, 376):
            p = cap(sc, "ambient", {"GapFillMode": "ambient"})
            if p:
                check_layout(sc, p)
    elif step == "ambient":
        for sc in (368, 376):
            p = cap(sc, "ambient", {"GapFillMode": "ambient"})
            if p:
                check_ambient(sc, p)
    elif step == "solid":
        for sc in (368, 376):
            p = cap(sc, "solid", {"GapFillMode": "solid",
                                  "GapColor": "#2d5f8a"})
            if p:
                check_solid(sc, p, "#2d5f8a")
    elif step == "peek":
        for sc in (376, 374, 368):
            print("scene %d peek" % sc)
            check_peek(sc)
    elif step == "nogap":
        for sc in (368, 376):
            p = cap(sc, "nogap", {"MinigameGap": False})
            if p:
                w, h, _ = G.read_bmp(p)
                print("  MinigameGap false -> %dx%d (want 512x768)" % (w, h))
    elif step == "glow":
        # STILL first: with nothing moving at the edges the follower has to be
        # invisible, and both the re-derivation and the cold read must land on
        # the capture. That is the regression half.
        print("scene 368 windowed, no launch")
        r, text, path = ambrun("still")
        ok_still = check_glow("still", text, path)
        if path:
            w, h, rows = G.read_bmp(path)
            band_h = h - 2 * SCREEN_H
            cold = G.expect_ambient(w, band_h, rows[SCREEN_H - 1],
                                    rows[SCREEN_H + band_h])
            n = sum(1 for k in range(band_h) for x in range(w)
                    if rows[SCREEN_H + k][x] != cold[k][x])
            print("  cold    : %d of %d band pixels differ from the COLD read "
                  "(0 = the band has converged on a still scene)"
                  % (n, band_h * w))
        # then the crossing, which is the behaviour half
        print("scene 368 windowed, scripted slingshot launch")
        r, text, path = ambrun("cross", probe=LAUNCH)
        ok_cross = check_glow("cross", text, path)
        col = int(sys.argv[2]) if len(sys.argv) > 2 else 12
        lo = int(sys.argv[3]) if len(sys.argv) > 3 else 228
        print("  column %d through the crossing:" % col)
        glow_series(text, col, lo, lo + 24)
        print("verdict: %s" % ("OK" if ok_still and ok_cross else "FAILED"))
    elif step == "touch":
        touchmap(376, [(4, 4), (4, 500), (4, 800), (4, 900), (4, 1000)])
    elif step == "cap":
        cap(int(sys.argv[2]), sys.argv[3])
