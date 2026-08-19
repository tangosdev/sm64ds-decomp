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
              "SM64DS_TOUCH_PROBE", "SM64DS_SCENE", "SM64DS_SCENE_BMP",
              "SM64DS_SCENE_BMP_STACKED"):
        env.pop(k, None)
    env["SM64DS_LEVEL"] = "1"
    env["SM64DS_FAULTS_FATAL"] = "1"
    env["SM64DS_WINDOW_SELFTEST"] = "300"
    G.clear_settings()
    r = subprocess.run([os.path.join(G.BUILD, "walk_window.exe")], cwd=G.BUILD,
                       env=env, capture_output=True, text=True,
                       errors="replace", timeout=1800)
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
    g = GAP[scene]
    band_h = 2 * g
    fill = (0xff, 0x00, 0xff)
    off = cap(scene, "peekoff", {"GapFillMode": "solid", "GapColor": MAGENTA,
                                 "GapPeek": False})
    on = cap(scene, "peekon", {"GapFillMode": "solid", "GapColor": MAGENTA,
                               "GapPeek": True})
    n_off, rows_off, w, h = band_stats(off, band_h, fill)
    n_on, rows_on, _, _ = band_stats(on, band_h, fill)
    print("  peek OFF: %d non-fill band pixels" % n_off)
    print("  peek ON : %d non-fill band pixels, in host band rows %s" %
          (n_on, rowspans(rows_on)))
    print("  DS band rows carrying peek pixels: %s of 0..%d" %
          (rowspans([(k // 2, c) for k, c in rows_on if k % 2 == 0]), g - 1))
    return n_off, n_on, rows_on


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
    elif step == "touch":
        touchmap(376, [(4, 4), (4, 500), (4, 800), (4, 900), (4, 1000)])
    elif step == "cap":
        cap(int(sys.argv[2]), sys.argv[3])
