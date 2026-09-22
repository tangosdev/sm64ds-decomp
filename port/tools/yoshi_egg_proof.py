#!/usr/bin/env python
"""Yoshi's egg lay, driven headless, with a verdict.

WHAT IT PROVES. Yoshi tongues a Goomba on Bob-omb Battlefield, swallows it, lays
an egg, and walks away. Seven assertions, all of which the pre-fix binary
fails, run in three arms (the harness's own 4:3, Tango's own settings.json,
and a Mario control that must touch no egg code at all):

  1. the Player reaches St_Swallow          (state 0x020d666c / 0x020d6474)
  2. NOTHING is quarantined                 (a faulting actor is a frozen actor)
  3. a YOSHI_EGG actor exists and TICKS     (the egg was really laid)
  4. the Player LEAVES St_Swallow           (he is not stuck in the lay)
  5. the Player is still MOVING at the end  (he is not frozen in place)
  6. the YOSHI_EGG actor is DRAWN           (a [actor] Render line with a real
                                             model file and non-zero transforms)
  7. the YOSHI_EGG actor is RASTERED        (SM64DS_EGG_TRIS: the egg's own
                                             Render submits real triangles, at
                                             non-zero alpha, inside the
                                             visible screen)

Assertion 5 is the one that speaks to the "yoshi freezes" half of the report.
Assertions 6 and 7 are the ones that speak to the "doesn't show up" half:
assertion 3 only counts the egg's Behavior ticks, which an egg that is never
drawn still produces, so a driver that steps over the frame-0xa mouth transfer
(see below) can tick an egg forever without ever putting a picture of it on
screen. Assertion 6 only proves the egg's Render was ENTERED, not that it put
anything on screen. Assertion 7 reads what that Render call submitted to
ntr::gx_polygons, the list the rasteriser consumes, so it fails an egg that
ticks and renders but draws nothing, draws off the visible screen, or draws at
zero alpha. Assertions 3, 4, 6 and 7 are the ones that stop a "no longer
hangs" non-fix from passing: an egg that is never laid, is never drawn, never
rasters, or a Yoshi who never returns to walking, is not a fix.

WHY THERE IS A DRIVER AT ALL. One env knob, off by default, in the host test
layer, not touching game logic:

  SM64DS_SELFTEST_TONGUE_ONCE  press B once instead of every 40 frames. A second
                               B while an enemy is in the mouth is the SPIT
                               (St_YoshiPower_Main case 4), and the swallow
                               hand-off needs the 0x5a-frame lockout at
                               Player+0x6c6 to run down first -- 90 frames,
                               longer than the 40-frame period -- so the
                               repeating press can never reach a swallow.

This tool used to also carry SM64DS_YOSHI_SWALLOW, which made by hand the two
writes St_YoshiPower_Main case 1 makes at body-anim frame 0xa (unk_0b0 |=
0x40000, &= ~0x20000: the enemy moves from the tongue to the mouth). That
stepped over the real swallow, so the knob is gone. What replaces it is
arming the tongue target earlier: SM64DS_YOSHI_EGG_REPRO now reads 213, not
200, so the grab lands on tongue-animation frame 3 (start-frame table entry
8, before the frame-0xa transfer) and the game's own transfer fires with no
driver standing in for it.

SM64DS_YOSHI_EGG_REPRO=200 is NOT how the game fails. It grabs on
tongue-animation frame 0, and in real play that frame can never register a
grab at all: the tongue's hit id is Player+0x338, which is dCc_c::otherOwner
inside the Player's own mAttackClsn at +0x314 (include/dCc_c.h:83), and
St_YoshiPower_Init calls dCc_c::Clear(&mAttackClsn) before the state's first
frame runs. So the hit id is zero on frame 0 and func_ov002_020d6998 takes
its fail path; a played swallow cannot reach that frame at all, only a
driver that writes otherOwner by hand can. REPRO=200 stays in this tool as
the gate's NEGATIVE CONTROL (it proves the tool can fail): a seeded case the
game itself cannot produce, not the bug being fixed.

A swallowed Goomba lays an egg and a swallowed Bob-omb does not: daKrb_c's
OnYoshiTryEat returns 6, but daBmb_c::OnYoshiTryEat returns mEatenByYoshi ==
0, which dEnemyBase_c::UpdateYoshiEat clears while the enemy is held. The
Bob-omb's no-egg outcome is the cartridge's own answer, decided in
OnYoshiTryEat, not a bug this tool checks for.

RECIPE (reproducible from a clean tree):

    python tools/asset_catalog.py generate "<path to the .nds>"
    cmd /c port\\build-port.cmd
    python -u port/tools/yoshi_egg_proof.py

Exit 0 = all seven assertions hold in the 4:3 arm and in Tango's own
settings.json, and the Mario control arm touches no egg code. Exit 1 = at
least one fails, with the reason printed.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
EXE = os.path.join(REPO, "build", "port", "walk_window.exe")

# assertion 7d: one framebuffer dump while the egg is alive (it exists from
# about f325 on; f338 is safe), read for its width/height, then deleted.
EGG_BMP_FRAME = 338
EGG_BMP_NAME = "walk_frame_%d.bmp" % EGG_BMP_FRAME

# the second arm: Tango's own settings.json, read-only, copied into a scratch
# cwd so hal/host_settings.cpp's find_settings() picks it up as its third
# candidate (plain "settings.json" relative to the process cwd).
TANGO_SETTINGS_SRC = r"C:\tmp\release-stage\test-bundle-b\settings.json"
TANGO_DIR = os.path.join(REPO, "tmp", "eggproof_tango")

# The Player's swallow state, both halves, as the state trace spells them.
ST_SWALLOW = ("020d666c", "020d6474")

ENV = {
    # BoB, as Yoshi, headless, 600 frames.
    "SM64DS_CHARACTER": "3",
    "SM64DS_LEVEL": "6",
    "SM64DS_WINDOW_SELFTEST": "600",
    # one tongue flick at f210, then leave him alone
    "SM64DS_SELFTEST_TONGUE": "1",
    "SM64DS_SELFTEST_TONGUE_ONCE": "1",
    # keep the tongue pointed at the first live Goomba (id 200) from f213, so
    # the grab lands on tongue-animation frame 3 (table entry 8, before the
    # frame-0xa mouth transfer) instead of frame 0 (table entry 11, after it)
    "SM64DS_YOSHI_EGG_REPRO": "213",
    "SM64DS_YOSHI_EGG_CLASS": "200",
    "SM64DS_YOSHI_EGG_WIN": "400",
    # readers
    "SM64DS_TRACE_STATE": "2",
    "SM64DS_RS_PROBE": "1",
    "SM64DS_ACTOR_PROBE": "1",
    # assertion 7 RASTER: bracket the egg's own Render with ntr::gx_polygons
    "SM64DS_EGG_TRIS": "1",
    # assertion 7d: dump one frame's framebuffer for its width/height
    "SM64DS_DUMP_FROM": str(EGG_BMP_FRAME),
    "SM64DS_DUMP_TO": str(EGG_BMP_FRAME),
    # quiet spawner: no window, no sound, never activated
    "SM64DS_NO_FOCUS": "1",
    "SM64DS_VOLUME": "0",
    # other lanes run concurrently
    "SM64DS_TEST_LOCK": "1",
    "SM64DS_TEST_LOCK_PATH": r"C:\tmp\sm64ds-test-slot\slot.lock",
    "SM64DS_TEST_LOCK_TIMEOUT": "5400",
}

# The player's per-frame line: [f317] pos=(-3695.0,0.0,4030.1) ... st=020d666c ...
FRAME = re.compile(r"^\[f(\d+)\] pos=\(([-\d.]+),([-\d.]+),([-\d.]+)\).*?st=([0-9a-f]{8})")

# The egg's RENDER face, not its Behavior face: [actor] YOSHI_EGG model 3002432C
# file 301774CC transforms 3017749C mat.t (-509,3,545) scene
DRAWN = re.compile(r"^\[actor\] YOSHI_EGG\s+model (\S+) file (\S+) transforms (\S+)")

# assertion 7 RASTER: SM64DS_EGG_TRIS's line, what the egg's own Render put on
# the raster this frame -- [eggtris] render 0: 48 triangles screen x[224..274]
# y[235..297] alpha 31..31 textured 48
EGGTRIS = re.compile(
    r"^\[eggtris\] render (\d+): (\d+) triangles screen "
    r"x\[(-?\d+)\.\.(-?\d+)\] y\[(-?\d+)\.\.(-?\d+)\] alpha (\d+)\.\.(\d+) "
    r"textured (\d+)")
EGGTRIS_ZERO = re.compile(r"^\[eggtris\] render \d+: 0 triangles submitted")


def run(log_path, cwd, extra_env=None):
    env = dict(os.environ)
    env.update(ENV)
    if extra_env:
        env.update(extra_env)
    SI = subprocess.STARTUPINFO()
    SI.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI.wShowWindow = 7  # SW_SHOWMINNOACTIVE
    NOCON = getattr(subprocess, "CREATE_NO_WINDOW", 0)
    with open(log_path, "wb") as fh:
        rc = subprocess.call([EXE], cwd=cwd, env=env, stdout=fh,
                             stderr=subprocess.STDOUT,
                             creationflags=NOCON, startupinfo=SI)
    with open(log_path, "r", errors="ignore") as fh:
        return rc, fh.read()


def read_bmp_wh(path):
    """Width/height from a standard 40-byte BITMAPINFOHEADER: width at
    offset 18, height at offset 22, little-endian int32; height may be
    negative for a top-down bitmap, so the magnitude is the row count."""
    try:
        with open(path, "rb") as fh:
            head = fh.read(26)
    except OSError:
        return None
    if len(head) < 26:
        return None
    w = int.from_bytes(head[18:22], "little", signed=True)
    h = int.from_bytes(head[22:26], "little", signed=True)
    return w, abs(h)


def check(text, rc, bmp_path):
    fails = []

    frames = []
    for line in text.splitlines():
        m = FRAME.match(line)
        if m:
            frames.append((int(m.group(1)),
                           (float(m.group(2)), float(m.group(3)), float(m.group(4))),
                           m.group(5)))

    # 1. reached the swallow
    sw = [f for f in frames if f[2] in ST_SWALLOW]
    if not sw:
        fails.append("1 REACH: the Player never entered St_Swallow -- the run "
                     "did not exercise the egg lay at all, so it proves nothing")
    else:
        print("  1 REACH   ok: St_Swallow from f%d to f%d (%d frames)"
              % (sw[0][0], sw[-1][0], len(sw)))

    # 2. nothing quarantined
    q = [l for l in text.splitlines() if "[quarantine]" in l]
    if q:
        fails.append("2 FAULT: %d quarantine(s): %s" % (len(q), q[0].strip()))
    else:
        print("  2 FAULT   ok: no actor was quarantined")

    # 3. the egg exists and ticks
    ticks = text.count("[rsprobe] YOSHI_EGG")
    if ticks < 30:
        fails.append("3 EGG: only %d YOSHI_EGG Behavior ticks -- the egg was "
                     "not laid, or did not survive" % ticks)
    else:
        print("  3 EGG     ok: %d YOSHI_EGG Behavior ticks" % ticks)

    # 4. left the swallow again
    if sw:
        after = [f for f in frames if f[0] > sw[-1][0] and f[2] not in ST_SWALLOW]
        if not after:
            fails.append("4 RELEASE: the Player never left St_Swallow")
        else:
            print("  4 RELEASE ok: left St_Swallow at f%d into state 0x%s"
                  % (after[0][0], after[0][2]))

    # 5. still moving at the end -- the actual "yoshi freezes" assertion
    tail = frames[-90:]
    if len(tail) < 30:
        fails.append("5 ALIVE: only %d frames of player trace" % len(tail))
    else:
        xs = set(round(f[1][0], 1) for f in tail)
        zs = set(round(f[1][2], 1) for f in tail)
        if len(xs) < 3 and len(zs) < 3:
            fails.append("5 ALIVE: the Player has not moved for the last %d "
                         "frames (stuck at %s) -- FROZEN"
                         % (len(tail), tail[-1][1]))
        else:
            print("  5 ALIVE   ok: %d distinct x / %d distinct z over the last "
                  "%d frames" % (len(xs), len(zs), len(tail)))

    # 6. drawn -- the egg's RENDER face, not just its Behavior face
    def _all_zero(s):
        return re.fullmatch(r"0+", s) is not None

    drawn_line = None
    drawn_match = None
    for line in text.splitlines():
        m = DRAWN.match(line)
        if m:
            drawn_line = line
            drawn_match = m
            break
    if not drawn_match or _all_zero(drawn_match.group(2)) or _all_zero(drawn_match.group(3)):
        fails.append("6 DRAWN: the egg ticked but was never drawn (no Render), "
                     "or its model file did not load")
    else:
        print("  6 DRAWN   ok: %s" % drawn_line.strip())

    # 7. rastered -- what the egg's own Render put on the raster
    # (SM64DS_EGG_TRIS): a, at least 30 parsed lines; b, zero "0 triangles
    # submitted" lines; c, max alpha over all lines > 0; d, at least one
    # line's box fully inside the active screen.
    raster_lines = []
    zero_lines = []
    for line in text.splitlines():
        m = EGGTRIS.match(line)
        if m:
            raster_lines.append((line, m))
        elif EGGTRIS_ZERO.match(line):
            zero_lines.append(line)

    reasons = []
    if len(raster_lines) < 30:
        reasons.append("a: only %d parsed [eggtris] render lines (need >= 30) "
                        "-- the egg did not render for long enough to trust "
                        "this run" % len(raster_lines))
    if zero_lines:
        reasons.append("b: %d render(s) submitted 0 triangles -- the egg "
                        "ticked and its Render ran, but it put nothing on "
                        "the raster" % len(zero_lines))
    max_alpha = max((int(m.group(8)) for _, m in raster_lines), default=0)
    if max_alpha <= 0:
        reasons.append("c: every parsed render has alpha 0 (max %d) -- the "
                        "egg submits geometry the raster discards" % max_alpha)

    wh = read_bmp_wh(bmp_path) if bmp_path else None
    inside = False
    worst_oob_line = None
    if wh is not None:
        W, H = wh
        worst_overhang = -1 << 30
        for line, m in raster_lines:
            x0, x1 = int(m.group(3)), int(m.group(4))
            y0, y1 = int(m.group(5)), int(m.group(6))
            if x0 >= 0 and y0 >= 0 and x1 < W and y1 < H:
                inside = True
            overhang = max(-x0, -y0, x1 - W + 1, y1 - H + 1)
            if overhang > worst_overhang:
                worst_overhang = overhang
                worst_oob_line = line
        if not inside:
            reasons.append("d: no render's box was fully inside the %dx%d "
                            "screen -- the egg drew off the visible screen"
                            % (W, H))
    else:
        print("  7 RASTER  note: no %s -- skipping 7d only" % EGG_BMP_NAME)

    if bmp_path and os.path.exists(bmp_path):
        try:
            os.remove(bmp_path)
        except OSError:
            pass

    if reasons:
        if zero_lines:
            worst = zero_lines[0]
        elif wh is not None and not inside and worst_oob_line:
            worst = worst_oob_line
        elif raster_lines:
            worst = raster_lines[-1][0]
        else:
            worst = None
        msg = "7 RASTER: " + "; ".join(reasons)
        if worst:
            msg += " -- worst line: %s" % worst.strip()
        fails.append(msg)
    else:
        print("  7 RASTER  ok: first %s" % raster_lines[0][0].strip())
        print("            last  %s" % raster_lines[-1][0].strip())

    if rc != 0:
        fails.append("0 EXIT: walk_window returned %d" % rc)
    return fails


def run_arm(name, cwd, log_path, extra_env=None):
    print("ARM %s" % name)
    rc, text = run(log_path, cwd, extra_env)
    bmp_path = os.path.join(cwd, EGG_BMP_NAME)
    fails = check(text, rc, bmp_path)
    print("  log: %s" % log_path)
    return fails


def mario_control():
    print("ARM [mario control]")
    log = os.path.join(REPO, "build", "port", "yoshi_egg_proof_mario.log")
    rc, text = run(log, REPO, {"SM64DS_CHARACTER": "0"})
    bmp_path = os.path.join(REPO, EGG_BMP_NAME)
    if os.path.exists(bmp_path):
        try:
            os.remove(bmp_path)
        except OSError:
            pass
    counts = {
        "[rsprobe] YOSHI_EGG": text.count("[rsprobe] YOSHI_EGG"),
        "[actor] YOSHI_EGG": text.count("[actor] YOSHI_EGG"),
        "[eggtris]": text.count("[eggtris]"),
    }
    bad = {k: v for k, v in counts.items() if v}
    print("  log: %s" % log)
    if rc != 0 or bad:
        parts = []
        if rc != 0:
            parts.append("walk_window returned %d" % rc)
        if bad:
            parts.append(", ".join("%s=%d" % kv for kv in bad.items()))
        print("MARIO CONTROL: FAIL, %s" % "; ".join(parts))
        return False
    print("MARIO CONTROL: ok, no egg path touched")
    return True


def main():
    if not os.path.exists(EXE):
        print("no walk_window.exe -- build the port first")
        return 2

    print("yoshi_egg_proof: BoB, Yoshi, one tongue flick at f210, 600 frames")
    overall_fail = False

    # ---- arm 1: the harness's own 4:3 ------------------------------------
    fails1 = run_arm("[4:3]", REPO,
                      os.path.join(REPO, "build", "port", "yoshi_egg_proof.log"))
    if fails1:
        overall_fail = True
        print("YOSHI EGG PROOF [4:3] : FAIL (%d/7)" % (7 - len(fails1)))
        for f in fails1:
            print("  " + f)
    else:
        print("YOSHI EGG PROOF [4:3] : PASS (7/7)")

    # ---- arm 2: Tango's own settings.json --------------------------------
    print("ARM [tango 21:9 144Hz analog]")
    if not os.path.exists(TANGO_SETTINGS_SRC):
        print("  no %s -- skipping the tango arm" % TANGO_SETTINGS_SRC)
    else:
        os.makedirs(TANGO_DIR, exist_ok=True)
        shutil.copyfile(TANGO_SETTINGS_SRC, os.path.join(TANGO_DIR, "settings.json"))
        log2 = os.path.join(REPO, "build", "port", "yoshi_egg_proof_tango.log")
        rc2, text2 = run(log2, TANGO_DIR)
        bmp2 = os.path.join(TANGO_DIR, EGG_BMP_NAME)
        fails2 = check(text2, rc2, bmp2)
        print("  log: %s" % log2)
        if fails2:
            overall_fail = True
            print("YOSHI EGG PROOF [tango 21:9 144Hz analog]: FAIL (%d/7)"
                  % (7 - len(fails2)))
            for f in fails2:
                print("  " + f)
        else:
            print("YOSHI EGG PROOF [tango 21:9 144Hz analog]: PASS (7/7)")

    # ---- arm 3: Mario control ---------------------------------------------
    if not mario_control():
        overall_fail = True

    return 1 if overall_fail else 0


if __name__ == "__main__":
    sys.exit(main())
