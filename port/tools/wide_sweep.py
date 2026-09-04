#!/usr/bin/env python3
"""The per-STATE widescreen acceptance sweep: photograph every distinct UI state
at each aspect, and JUDGE the photographs for stretching, tearing, clipping and
mis-anchoring.

WHAT IT IS FOR. The port's widescreen HUD is placed per ELEMENT: each element
gets one anchor chosen from its own centre, so an element that used to straddle
a band boundary now moves in one piece. That claim is about what a player SEES,
so it is settled with pictures of the real window and a checker that reads their
pixels, not with a framebuffer dump and an assertion.

THE THREE RULES THIS TOOL EXISTS TO OBEY
----------------------------------------
1. THE PIXELS ARE THE WINDOW'S, NOT THE FRAMEBUFFER'S. Every image comes from
   port/tools/wide_sweep_grab.ps1, which is GDI CopyFromScreen over the window's
   own client rectangle. This tree has already paid for the alternative: the
   black-bottom present bug wrote a full and correct framebuffer while the glass
   showed half a black screen, so a BMP the game wrote cannot answer a question
   about what is on screen.

2. EVERY CLAIM HAS A CONTROL. Every state is captured TWICE at each wide aspect:
   once with the per-element rule and once with SM64DS_HUD_BANDSPLIT=1, which
   puts the old per-column band split back on the SAME binary at the SAME state.
   A check that does not FAIL on the control half is not a check, it is a
   sentence, and this tool says so per state rather than reporting a row of
   passes that were never at risk.

3. THE RECIPE IS PRINTED, SO IT REPRODUCES. Each row records the exact
   environment its image was taken under, into the report. A number nobody else
   can re-derive was withdrawn from this project once already because the recipe
   silently measured a different scene from the one it named.

HOW THE HUD IS ISOLATED FROM THE 3D FIELD
-----------------------------------------
A photograph of a level is mostly world, and the checks are about HUD ELEMENTS,
so the HUD has to be separated from the picture behind it. It is separated the
way this tree already separates it (build/hudband/nolayer.py did the same thing
for the original band measurement): capture the state twice at one aspect, once
normally and once with SM64DS_ENGINE_A_LAYERS=0, which composites NO engine-A 2D
layer and leaves the 3D field alone. The pixels that differ are exactly the 2D
HUD.

THAT SUBTRACTION IS ONLY VALID IF THE TWO CAPTURES SHOW THE SAME FRAME, which is
why the sweep is built on STATIC states and checks staticness instead of
assuming it. Every run grabs two frames about a second apart; if they are not
identical the state is DYNAMIC, the subtraction is not trustworthy, and the row
is reported as such rather than being scored. A pause screen, a menu, a title
and a held banner are all genuinely still, which is fortunate, because those are
also where the straddling elements live.

WHAT EACH CHECK MEANS
---------------------
TEAR      the defect this work exists to fix. A band split cuts a straddling
          element in two and opens the whole spare width through the middle of
          it, so the signature is an element whose bounding box contains an
          internal horizontal VOID about margin/2 wide (128 px at 16:9, 288 at
          21:9). Measured as the widest fully-empty column run strictly inside
          an element's own bounding box.
STRETCH   the HUD must keep its shape. Every element is composited at a UNIFORM
          scale, so an element's width at a wide aspect must be its width at the
          native aspect times uni/sx -- 1.5 at both 16:9 and 21:9, since both
          fit the same 576-row buffer. A element stretched to fill the width
          instead comes out at 2.0 or more.
CLIP      an anchored element must not be pushed off the frame. Fails if an
          element's box touches column 0 or the last column.
ANCHOR    an element that rode a screen edge must still ride it, at its native
          margin. Fails if a left-band element's left margin or a right-band
          element's right margin does not match the native margin times uni/sx.

USAGE
-----
    python port/tools/wide_sweep.py --capture        # take every photograph
    python port/tools/wide_sweep.py --judge          # score what was taken
    python port/tools/wide_sweep.py --capture --judge --states pause,message

Images and the report land under --out (default C:\\tmp\\wide169sweep).
"""
import argparse
import json
import os
import subprocess
import sys

import numpy as np
from PIL import Image

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
GRAB = os.path.join(ROOT, "port", "tools", "wide_sweep_grab.ps1")

# The aspects. 0 is the native sentinel and is the REFERENCE every wide image is
# measured against, so it is not optional and is captured for every state.
#
# 21:9 IS CAPTURED BUT IS NOT OFFERED IN THE LAUNCHER. That is the point of
# capturing it: the dropdown gets a 21:9 row when this sweep says it is clean,
# and not before. It is the aspect most likely to expose a placement fault,
# because the spare width it opens between elements is 576 rather than 256, so a
# fault that 16:9 hides by a few pixels is obvious here.
ASPECTS = [("native", "0"), ("16x9", "1.7777778"), ("21x9", "2.3333333")]

# THE SCALES ARE DERIVED PER IMAGE, NOT ASSUMED, and the first pass of this tool
# assumed them and was wrong twice over.
#
# First, the client area is not the framebuffer. On a high-DPI desktop Windows
# hands the window a client rectangle a whole multiple larger than the extent the
# port renders: the 512x384 native window came back as a 1024x768 photograph. So
# every measurement in host pixels has to be divided by that factor before it
# means anything in the port's own terms, and the factor is read off the NATIVE
# capture, whose extent is known to be 512x384.
#
# Second, the uniform scale is NOT 3 at every wide aspect. It is active_h/192,
# and active_h is whatever the aspect fit leaves inside the 1024x576 buffer. At
# 16:9 that is 576, so uni is 3. At 21:9 the fit is 1024x440 and uni is 2 -- the
# HUD is drawn at a SMALLER native scale there, and the spare width is 512 rather
# than 256. A checker carrying 3 as a constant would have measured 21:9 against a
# margin twice the real one and called every element misplaced.
NATIVE_EXTENT_W, NATIVE_EXTENT_H = 512, 384


def geom(native_shape, wide_shape):
    """(dpi, uni_native, uni_wide, margin_client) for one aspect's images."""
    dpi = native_shape[1] / NATIVE_EXTENT_W
    uni_n = int(round((native_shape[0] / dpi) / 192))
    aw = wide_shape[1] / dpi
    ah = wide_shape[0] / dpi
    uni_w = int((ah + 0.5) // 192)
    margin = int(round((aw - 256 * uni_w) * dpi))
    return dpi, uni_n, uni_w, margin

# The element bridge, in HOST pixels at the wide scale. It is the compositor's
# own DS-pixel bridge (16 x 8, see the hudelem banner in
# port/hal/message_compositor.cpp) carried into host pixels, so the checker
# groups pixels into elements by the same rule the placement did. A checker that
# grouped differently would be measuring its own opinion of where the elements
# are rather than the one the code acted on.
BRIDGE_X_DS, BRIDGE_Y_DS = 16, 8

# A pixel counts as "changed" between two captures when any channel moves by
# more than this. It is not zero because the capture path is a real desktop
# composite: a window can pick up a pixel of edge blending at its border. It is
# small enough that a HUD glyph, which is opaque and high contrast, is never
# missed.
DIFF_TOL = 24


def states():
    """The distinct UI states the sweep covers, and how each is put on screen.

    Every entry is the COMPLETE extra environment for that state. The aspect,
    the control knob and the layer mask are added by the runner, so a row here
    is only ever about which screen the game is showing.

    A state whose recipe is not settled is absent rather than guessed. An
    invented recipe that boots something adjacent is worse than a gap, because
    it produces images that look like evidence for a state nobody photographed.
    """
    return {
        # THE MENUS. All four are scene-path boots held open by a frame budget
        # nothing reaches, and all four are FROZEN by SM64DS_SCENE_MENU once
        # their screen has formed. The freeze is not decoration: measured
        # unfrozen, two launches of the title differ in 4.5 percent of the
        # frame and the star select in 0.3 to 0.7 percent, which is the screen
        # animating, and the HUD isolation below subtracts one capture from
        # another and means nothing between two different frames. The minigame
        # menu was already still (0.000 percent) and is frozen anyway, so every
        # row in this table is measured the same way. SM64DS_SCENE_WINDOW=1 is required alongside a
        # SCENE_FRAMES budget: the scene path is windowed only when
        # SCENE_FRAMES is UNSET unless SCENE_WINDOW says otherwise
        # (walk_window.cpp:6508-6518).
        "title":      {"SM64DS_SCENE": "1", "SM64DS_SCENE_WINDOW": "1",
                       "SM64DS_SCENE_FRAMES": "100000",
                       "SM64DS_SCENE_MENU": "120"},
        "starselect": {"SM64DS_SCENE": "4", "SM64DS_SCENE_WINDOW": "1",
                       "SM64DS_SCENE_FRAMES": "100000",
                       "SM64DS_SCENE_MENU": "120"},
        "mgmenu":     {"SM64DS_SCENE": "5", "SM64DS_SCENE_WINDOW": "1",
                       "SM64DS_SCENE_FRAMES": "100000",
                       "SM64DS_SCENE_MENU": "120"},
        "vsmenu":     {"SM64DS_SCENE": "6", "SM64DS_SCENE_WINDOW": "1",
                       "SM64DS_SCENE_FRAMES": "100000",
                       "SM64DS_SCENE_MENU": "120"},

        # THE FILE SELECT IS REACHED BY SM64DS_SKIP_MENU, NOT SM64DS_TITLE_ENTRY.
        # TITLE_ENTRY only ARMS the bridge that lets a picked file fall through
        # into the adventure (title_entry.cpp:321-343); it does not advance the
        # title. SKIP_MENU is the driver that taps the title and stops on the
        # file select without picking a file (title_entry.cpp:567-655). The
        # screen lands around frame 550-600, so this row needs a long wait.
        "fileselect": {"SM64DS_SCENE_WINDOW": "1", "SM64DS_SCENE_FRAMES": "100000",
                       "SM64DS_SKIP_MENU": "1",
                       "SM64DS_SCENE_MENU": "700"},

        # THE COURSE HUD. Level 6 (Bob-omb Battlefield) and not the castle
        # family, because RenderCoinCount takes an early-out on course 0x1d and
        # the coin counter is the element this sweep is about.
        #
        # SM64DS_MENU_AT=<frame> FREEZES THE GAME TICK once the screen has
        # formed, and it is here for a measurement reason rather than convenience. The HUD is isolated by subtracting a
        # 2D-layers-off capture from a normal one, and that subtraction is only
        # meaningful between two captures of the SAME frame. A live level
        # animates, so two launches photograph two different frames and the
        # subtraction returns the animation as well as the HUD. The menu's pause
        # is "skip the tick" and nothing else (walk_window.cpp:9626), so the
        # picture stays fully composited and completely still, and the panel it
        # draws is identical in every capture of the row and cancels in the
        # difference.
        "coursehud":  {"SM64DS_LEVEL": "6", "SM64DS_MENU_AT": "240"},

        # THE MESSAGE BOX: the probe opens one at frame 90 and nothing dismisses
        # it, because the pump only advances on an A/B press and there is nobody
        # at the keyboard (message_pump.cpp:237). Frozen for the same reason as
        # above. This is the "message boxes keep their shape" state.
        "message":    {"SM64DS_LEVEL": "6", "SM64DS_PROBE_MESSAGE": "42",
                       "SM64DS_MENU_AT": "150"},

        # THE VS HUD, AND THE STATE THIS WHOLE CHANGE IS ABOUT. The VS timer's
        # TIME label sits at DS x 140-179, which crosses band_r (160), so it is
        # the straddling element the band split tears and the one the control
        # half has to show torn. It draws on engine A (sub=0), which is the
        # screen the reanchor acts on.
        "vshud":      {"SM64DS_VS_MAP": "0", "SM64DS_MENU_AT": "420"},
    }


def env_for(state_env, aspect, bandsplit, layers_off):
    e = dict(os.environ)
    # Scrub every SM64DS_* the parent happens to be carrying, so a run is
    # exactly this row's recipe and not the shell's history. The lock variables
    # are put back below because the grab script needs them.
    for k in [k for k in e if k.startswith("SM64DS_")]:
        del e[k]
    e.update(state_env)
    e["SM64DS_ASPECT"] = aspect
    e["SM64DS_VOLUME"] = "0"
    e["SM64DS_NO_PLAYLOG"] = "1"
    e["SM64DS_TEST_LOCK"] = "1"
    e["SM64DS_TEST_LOCK_PATH"] = r"C:\tmp\sm64ds-test-slot\windowed_test.lock"
    e["SM64DS_TEST_LOCK_TIMEOUT"] = "5400"
    # A FIXED, ON-SCREEN WINDOW POSITION, and this is not tidiness. CopyFromScreen
    # reads the DESKTOP at the window's client coordinates, so a window the OS
    # placed partly or wholly off the desktop photographs as whatever is at those
    # coordinates, which is nothing. Measured: a first pass left the native window
    # at y=-1117 and its captures were unusable, and two launches of one state
    # landed at two different positions, so the images were not even comparable
    # with each other. Pinning the corner makes every image in the sweep the same
    # rectangle of glass.
    e["SM64DS_WINDOW_POS"] = "0,0"
    if bandsplit:
        e["SM64DS_HUD_BANDSPLIT"] = "1"
    if layers_off:
        e["SM64DS_ENGINE_A_LAYERS"] = "0x00"
    return e


def recipe_text(state_env, aspect, bandsplit, layers_off):
    bits = [f"SM64DS_ASPECT={aspect}"]
    bits += [f"{k}={v}" for k, v in sorted(state_env.items())]
    if bandsplit:
        bits.append("SM64DS_HUD_BANDSPLIT=1")
    if layers_off:
        bits.append("SM64DS_ENGINE_A_LAYERS=0x00")
    return " ".join(bits)


def grab(png, state_env, aspect, bandsplit=False, layers_off=False, wait=14.0):
    """One launch, one photograph. Returns (ok, note)."""
    cmd = ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass",
           "-File", GRAB, "-OutPng", png, "-WaitSec", str(wait)]
    try:
        r = subprocess.run(cmd, env=env_for(state_env, aspect, bandsplit, layers_off),
                           capture_output=True, text=True, timeout=wait + 90)
    except subprocess.TimeoutExpired:
        return False, "grab timed out"
    if not os.path.exists(png):
        return False, (r.stderr or r.stdout or "no image").strip().splitlines()[-1:][0] \
            if (r.stderr or r.stdout) else "no image"
    return True, (r.stdout or "").strip().splitlines()[-1] if r.stdout else "ok"


def load(png):
    return np.asarray(Image.open(png).convert("RGB")).astype(np.int16)


def changed_mask(a, b):
    """Where two captures of the SAME frame differ, past the capture tolerance."""
    if a.shape != b.shape:
        return None
    return (np.abs(a - b).max(axis=2) > DIFF_TOL)


def label(mask, bx, by):
    """Bridge by (bx, by) host pixels, then label 8-connected, and return the
    ORIGINAL mask's per-element bounding boxes.

    The bridge is why a line of text is one element instead of a row of loose
    glyphs, and the extents come from the unbridged mask so an element's real
    edges -- which the margin checks read -- are not inflated by the bridge.
    """
    h, w = mask.shape
    d = mask.copy()
    # Horizontal bridge: two running sweeps, left-to-right then right-to-left.
    # A running count of "columns since the last set pixel" makes each sweep a
    # one-sided dilation, so the whole radius costs one add per pixel instead of
    # a bx-wide window scan at every one.
    for cols in (range(w), range(w - 1, -1, -1)):
        run = np.full(h, bx + 1)
        for x in cols:
            run = np.where(mask[:, x], 0, run + 1)
            d[:, x] |= (run <= bx)
    # Vertical bridge, the same two sweeps down the columns, reading the
    # horizontally bridged mask so the result is the rectangular dilation.
    src = d.copy()
    for rows in (range(h), range(h - 1, -1, -1)):
        run = np.full(w, by + 1)
        for y in rows:
            run = np.where(src[y, :], 0, run + 1)
            d[y, :] |= (run <= by)

    # Two-pass 8-connected labelling with union-find over the bridged mask.
    lbl = np.zeros(d.shape, dtype=np.int32)
    par = [0]

    def find(a):
        while par[a] != a:
            par[a] = par[par[a]]
            a = par[a]
        return a

    def unite(a, b):
        a, b = find(a), find(b)
        if a != b:
            par[max(a, b)] = min(a, b)

    nxt = 1
    for y in range(h):
        for x in range(w):
            if not d[y, x]:
                continue
            best = 0
            for dy, dx in ((-1, -1), (-1, 0), (-1, 1), (0, -1)):
                ny, nx = y + dy, x + dx
                if ny < 0 or nx < 0 or nx >= w:
                    continue
                nl = lbl[ny, nx]
                if nl:
                    best = nl if not best else (unite(best, nl) or best)
            if not best:
                best = nxt
                par.append(nxt)
                nxt += 1
            lbl[y, x] = best
    for y in range(h):
        row = lbl[y]
        nz = row > 0
        if nz.any():
            row[nz] = [find(v) for v in row[nz]]

    boxes = {}
    ys, xs = np.nonzero(mask)          # extents from the ORIGINAL mask
    for y, x in zip(ys, xs):
        r = int(lbl[y, x])
        if not r:
            continue
        b = boxes.get(r)
        if b is None:
            boxes[r] = [x, x, y, y, 1]
        else:
            b[0] = min(b[0], x); b[1] = max(b[1], x)
            b[2] = min(b[2], y); b[3] = max(b[3], y)
            b[4] += 1
    return [dict(x0=int(v[0]), x1=int(v[1]), y0=int(v[2]), y1=int(v[3]),
                 px=int(v[4]), w=int(v[1] - v[0] + 1))
            for v in boxes.values() if v[4] >= 12]


def tear_pairs(elems, margin, tol=14, vmin=0.75):
    """Find the TEAR SIGNATURE: two elements the split cut one element into.

    THIS IS NOT AN "INTERNAL GAP" TEST, and the difference matters enough to
    write down, because the obvious version of this check is silently dead. A
    band split opens margin/2 (128 host px at 16:9, 288 at 21:9) through the
    middle of a straddling element -- which is FAR wider than the bridge the
    labeller groups by, so the torn element does not come back as one element
    with a hole in it. It comes back as TWO ELEMENTS. Any check that looks for a
    void inside one element's bounding box therefore measures 0 on a genuinely
    torn frame and passes it. That version was written first here and caught
    nothing on a synthetic tear; this is what replaced it.

    The real signature is a PAIR: two elements sitting at the same rows,
    separated horizontally by very close to margin/2. Same rows because the
    split is horizontal only -- nothing about the reanchor moves a pixel
    vertically -- and margin/2 because that is exactly the width the split
    opens.

    Returns the list of offending pairs, each with the gap it measured.
    """
    out = []
    half = margin // 2
    if half <= 0:
        return out
    es = sorted(elems, key=lambda e: e["x0"])
    for i, a in enumerate(es):
        for b in es[i + 1:]:
            gap = b["x0"] - a["x1"] - 1
            if gap < half - tol:
                continue
            if gap > half + tol:
                break
            lo, hi = max(a["y0"], b["y0"]), min(a["y1"], b["y1"])
            ov = hi - lo + 1
            if ov <= 0:
                continue
            span = min(a["y1"] - a["y0"] + 1, b["y1"] - b["y0"] + 1)
            if ov / span >= vmin:
                out.append({"gap": int(gap), "rows": [int(lo), int(hi)],
                            "left": [a["x0"], a["x1"]],
                            "right": [b["x0"], b["x1"]]})
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=r"C:\tmp\wide169sweep")
    ap.add_argument("--capture", action="store_true")
    ap.add_argument("--judge", action="store_true")
    ap.add_argument("--states", default="")
    ap.add_argument("--wait", type=float, default=14.0)
    a = ap.parse_args()
    if not (a.capture or a.judge):
        ap.error("give --capture, --judge, or both")

    st = states()
    if a.states:
        want = [s.strip() for s in a.states.split(",") if s.strip()]
        missing = [w for w in want if w not in st]
        if missing:
            ap.error(f"unknown state(s): {', '.join(missing)}; "
                     f"have: {', '.join(sorted(st))}")
        st = {k: v for k, v in st.items() if k in want}

    os.makedirs(a.out, exist_ok=True)
    meta_path = os.path.join(a.out, "recipes.json")
    meta = {}
    if os.path.exists(meta_path):
        meta = json.load(open(meta_path))

    if a.capture:
        for name, senv in st.items():
            for aname, aval in ASPECTS:
                # Four images per state per aspect: the shipped rule, the same
                # frame again (the staticness witness), the band-split control,
                # and the 2D-layers-off frame the HUD is subtracted out of.
                jobs = [("fix", False, False), ("fix2", False, False),
                        ("band", True, False), ("no2d", False, True)]
                if aname == "native":
                    jobs = [("fix", False, False), ("fix2", False, False),
                            ("no2d", False, True)]
                for tag, band, off in jobs:
                    png = os.path.join(a.out, f"{name}__{aname}__{tag}.png")
                    ok, note = grab(png, senv, aval, band, off, a.wait)
                    key = f"{name}__{aname}__{tag}"
                    meta[key] = {"recipe": recipe_text(senv, aval, band, off),
                                 "ok": ok, "note": note}
                    print(f"[{'ok ' if ok else 'FAIL'}] {key}  {note}", flush=True)
        json.dump(meta, open(meta_path, "w"), indent=1)

    if a.judge:
        judge(a.out, st, meta)


def judge(out, st, meta):
    print("\n==== PER-STATE WIDESCREEN SWEEP ====\n")
    rows = []
    for name in st:
        npx = os.path.join(out, f"{name}__native__fix.png")
        nof = os.path.join(out, f"{name}__native__no2d.png")
        nat_img = load(npx) if os.path.exists(npx) else None
        nat_mask = (changed_mask(nat_img, load(nof))
                    if nat_img is not None and os.path.exists(nof) else None)

        for aname, _ in ASPECTS:
            if aname == "native":
                continue
            res = {"state": name, "aspect": aname}
            f1 = os.path.join(out, f"{name}__{aname}__fix.png")
            f2 = os.path.join(out, f"{name}__{aname}__fix2.png")
            fo = os.path.join(out, f"{name}__{aname}__no2d.png")
            fb = os.path.join(out, f"{name}__{aname}__band.png")
            if nat_img is None or not all(os.path.exists(p)
                                          for p in (f1, f2, fo, fb)):
                res["verdict"] = "NO IMAGES"
                rows.append(res)
                continue

            i1, i2 = load(f1), load(f2)
            # STATICNESS, AND THE ONE TOLERANCE IN THIS TOOL. The HUD
            # subtraction below is only meaningful between two captures of the
            # SAME frame, so two launches of a frozen state must photograph the
            # same picture. They very nearly do: measured on the VS HUD at 16:9,
            # two launches differ in 372 pixels out of 2,359,296 -- 0.016% --
            # and every one of them is inside a single 26x26 box at a FIXED
            # SCREEN position that is the same box in every pair, including
            # pairs that differ in nothing else. That is desktop furniture
            # caught by CopyFromScreen, not the game.
            #
            # So instability is MEASURED, BOUNDED AND EXCLUDED rather than
            # waved through. The unstable pixels are masked out of every mask
            # below, so nothing untrustworthy reaches an element; the count is
            # carried into the report so a reader can see how much was dropped;
            # and if it ever exceeds a tenth of a percent the row is refused
            # outright, because at that point it is the game moving and not the
            # desktop.
            unstable = changed_mask(i1, i2)
            if unstable is None:
                res["verdict"] = "SIZE MISMATCH - not scored"
                rows.append(res)
                continue
            res["unstable_px"] = int(unstable.sum())
            res["unstable_pct"] = round(float(unstable.mean()) * 100.0, 4)
            res["static"] = bool(res["unstable_pct"] <= 0.1)
            if not res["static"]:
                res["verdict"] = "DYNAMIC - not scored"
                rows.append(res)
                continue
            trust = ~unstable

            dpi, uni_n, uni_w, margin = geom(nat_img.shape, i1.shape)
            res["dpi"] = float(dpi)
            res["uni_native"], res["uni_wide"] = uni_n, uni_w
            res["margin"] = int(margin)
            expect = uni_w / uni_n

            ib = load(fb)
            hud_fix = changed_mask(i1, load(fo)) & trust
            hud_band = changed_mask(ib, load(fo)) & trust
            # What the fix actually MOVED, on this state, at this aspect: the
            # pixels where the shipped placement and the band-split control
            # disagree. Empty means no element on this screen straddles a split,
            # so the two rules agree and there is nothing here for either to get
            # wrong -- which is a real result and is reported as one.
            moved = changed_mask(i1, ib) & trust
            res["moved_px"] = int(moved.sum())
            # The bridge is the compositor's own 16 x 8 DS-pixel rectangle taken
            # into the host pixels of THIS image, so the checker groups by the
            # rule the placement acted on rather than by one of its own.
            ef = label(hud_fix, int(BRIDGE_X_DS * uni_w * dpi),
                       int(BRIDGE_Y_DS * uni_w * dpi))
            eb = label(hud_band, int(BRIDGE_X_DS * uni_w * dpi),
                       int(BRIDGE_Y_DS * uni_w * dpi))
            nat = label(nat_mask, int(BRIDGE_X_DS * uni_n * dpi),
                        int(BRIDGE_Y_DS * uni_n * dpi)) if nat_mask is not None else []

            W = i1.shape[1]
            res["elements"] = len(ef)
            res["elements_native"] = len(nat)
            tf = tear_pairs(ef, margin)
            tb = tear_pairs(eb, margin)
            res["tears_fix"], res["tears_band"] = tf, tb
            res["tear_fix"], res["tear_band"] = bool(tf), bool(tb)
            res["clip"] = bool(any(e["x0"] <= 0 or e["x1"] >= W - 1 for e in ef))

            # STRETCH. Every element is composited at ONE uniform scale, so the
            # widest element at the wide aspect must be the widest at native
            # times uni_wide/uni_native. A HUD stretched to fill the width comes
            # back well above that; a torn one comes back above it too, because
            # the fragment pair inflates nothing but the whole-element span is
            # what the widest measures.
            wn = max((e["w"] for e in nat), default=0)
            wf = max((e["w"] for e in ef), default=0)
            res["widest_native"], res["widest_wide"] = int(wn), int(wf)
            res["expect_ratio"] = round(expect, 3)
            res["ratio"] = round(wf / wn, 3) if wn else None
            res["stretch"] = bool(res["ratio"] is not None
                                  and abs(res["ratio"] - expect) > 0.12)
            rows.append(res)

    hdr = (f"{'state':<12}{'aspect':<8}{'stat':<6}{'elem':<6}"
           f"{'tears_fix':<10}{'tears_ctl':<10}{'marg':<6}{'moved':<8}"
           f"{'TEAR fix':<10}{'TEAR ctl':<10}{'ratio':<8}{'verdict'}")
    print(hdr)
    print("-" * len(hdr))
    for r in rows:
        if "verdict" in r and r["verdict"] in ("NO IMAGES", "DYNAMIC - not scored"):
            print(f"{r['state']:<12}{r['aspect']:<8}{'':<6}{'':<6}{'':<10}{'':<10}"
                  f"{'':<6}{'':<10}{'':<10}{'':<8}{r['verdict']}")
            continue
        bad = [k for k in ("tear_fix", "clip", "stretch") if r.get(k)]
        verdict = "PASS" if not bad else "FAIL:" + ",".join(bad)
        # A control that did not break is not evidence either way, and is said
        # so rather than being quietly counted as a win.
        if not r.get("tear_band"):
            verdict += "  (control not torn here: no straddling element)"
        else:
            verdict += "  (control TORN, detector proven)"
        print(f"{r['state']:<12}{r['aspect']:<8}{str(r.get('static')):<6}"
              f"{r.get('elements', 0):<6}{len(r.get('tears_fix', [])):<10}"
              f"{len(r.get('tears_band', [])):<10}{r.get('margin', 0):<6}"
              f"{r.get('moved_px', 0):<8}"
              f"{str(r.get('tear_fix')):<10}{str(r.get('tear_band')):<10}"
              f"{str(r.get('ratio')):<8}{verdict}")
    json.dump(rows, open(os.path.join(out, "verdict.json"), "w"), indent=1)
    print("\nrecipes:", os.path.join(out, "recipes.json"))
    print("verdict:", os.path.join(out, "verdict.json"))


if __name__ == "__main__":
    sys.exit(main())
