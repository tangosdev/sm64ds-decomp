#!/usr/bin/env python
"""Yoshi's egg lay, driven headless, with a verdict.

WHAT IT PROVES. Yoshi tongues a Goomba on Bob-omb Battlefield, swallows it, lays
an egg, and walks away. Five assertions, all of which the pre-fix binary fails:

  1. the Player reaches St_Swallow          (state 0x020d666c / 0x020d6474)
  2. NOTHING is quarantined                 (a faulting actor is a frozen actor)
  3. a YOSHI_EGG actor exists and TICKS     (the egg was really laid)
  4. the Player LEAVES St_Swallow           (he is not stuck in the lay)
  5. the Player is still MOVING at the end  (he is not frozen in place)

Assertion 5 is the one that speaks to the report ("yoshi freezes"). Assertions 3
and 4 are the ones that stop a "no longer hangs" non-fix from passing: an egg
that is never laid, or a Yoshi who never returns to walking, is not a fix.

WHY THERE IS A DRIVER AT ALL. Two env knobs, both off by default, both in the
host test layer, neither one touching game logic:

  SM64DS_SELFTEST_TONGUE_ONCE  press B once instead of every 40 frames. A second
                               B while an enemy is in the mouth is the SPIT
                               (St_YoshiPower_Main case 4), and the swallow
                               hand-off needs the 0x5a-frame lockout at
                               Player+0x6c6 to run down first -- 90 frames,
                               longer than the 40-frame period -- so the
                               repeating press can never reach a swallow.
  SM64DS_YOSHI_SWALLOW         make the two writes St_YoshiPower_Main makes at
                               body-anim frame 0xa (unk_0b0 |= 0x40000,
                               &= ~0x20000: the enemy moves from the tongue to
                               the mouth). The port never crosses that frame
                               because the p+0x160 head ModelAnim does not
                               advance -- a separate, already-documented defect
                               -- so St_YoshiPower_Cleanup drops the enemy and
                               the eat ends early. The driver steps over that
                               and nothing else; func_ov002_020d6790 still reads
                               the enemy's OnYoshiTryEat itself and picks the
                               state itself, and every frame from St_Swallow_Init
                               onward is the game's.

RECIPE (reproducible from a clean tree):

    python tools/asset_catalog.py generate "<path to the .nds>"
    cmd /c port\\build-port.cmd
    python -u port/tools/yoshi_egg_proof.py

Exit 0 = all five hold. Exit 1 = at least one fails, with the reason printed.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
EXE = os.path.join(REPO, "build", "port", "walk_window.exe")

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
    # keep the tongue pointed at the first live Goomba (id 200) from f200
    "SM64DS_YOSHI_EGG_REPRO": "200",
    "SM64DS_YOSHI_EGG_CLASS": "200",
    "SM64DS_YOSHI_EGG_WIN": "400",
    "SM64DS_YOSHI_SWALLOW": "1",
    # readers
    "SM64DS_TRACE_STATE": "2",
    "SM64DS_RS_PROBE": "1",
    # other lanes run concurrently
    "SM64DS_TEST_LOCK": "1",
    "SM64DS_TEST_LOCK_PATH": r"C:\tmp\sm64ds-test-slot\windowed_test.lock",
    "SM64DS_TEST_LOCK_TIMEOUT": "5400",
}

# The player's per-frame line: [f317] pos=(-3695.0,0.0,4030.1) ... st=020d666c ...
FRAME = re.compile(r"^\[f(\d+)\] pos=\(([-\d.]+),([-\d.]+),([-\d.]+)\).*?st=([0-9a-f]{8})")


def run(log_path):
    env = dict(os.environ)
    env.update(ENV)
    with open(log_path, "wb") as fh:
        rc = subprocess.call([EXE], cwd=REPO, env=env, stdout=fh,
                             stderr=subprocess.STDOUT)
    with open(log_path, "r", errors="ignore") as fh:
        return rc, fh.read()


def check(text, rc):
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

    if rc != 0:
        fails.append("0 EXIT: walk_window returned %d" % rc)
    return fails


def main():
    if not os.path.exists(EXE):
        print("no walk_window.exe -- build the port first")
        return 2
    log = os.path.join(REPO, "build", "port", "yoshi_egg_proof.log")
    print("yoshi_egg_proof: BoB, Yoshi, one tongue flick at f210, 600 frames")
    rc, text = run(log)
    fails = check(text, rc)
    print("  log: %s" % log)
    if fails:
        print("YOSHI EGG PROOF: FAIL")
        for f in fails:
            print("  " + f)
        return 1
    print("YOSHI EGG PROOF: PASS (5/5)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
