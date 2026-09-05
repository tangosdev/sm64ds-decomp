#!/usr/bin/env python
"""Yoshi's egg THROW, driven headless, with a verdict.

WHAT IT PROVES. Yoshi tongues a Goomba on Bob-omb Battlefield, swallows it,
lays an egg, and THROWS it. The thrown egg homes at a target actor, and every
frame of that flight func_ov002_020ecd18 dispatches the target's vtable slot 30
(Actor::OnAimedAtWithEggReturnVec).  Six assertions:

  1. the egg is laid                        (YOSHI_EGG ticks at all)
  2. the egg reaches STATE 1, thrown         ([eggstate] state=1 lines)
  3. the egg ACQUIRES A TARGET               (state=1 with target != 0)
  4. NOTHING is quarantined                  (a faulting actor is a frozen actor)
  5. no slot-30 decline is reported          (the trap's own message)
  6. the Player is still MOVING at the end   (he is not frozen)

Assertions 2 and 3 are what stop a "nothing happened, so nothing crashed"
non-fix from passing: an egg that is never thrown, or is thrown but never
acquires a target, never reaches slot 30 and proves nothing.

WHY THERE IS A DRIVER AT ALL. Four env knobs, all off by default, all in the
host test layer, none of them touching game logic:

  SM64DS_SELFTEST_TONGUE / _ONCE  one B flick, the tongue (pre-existing)
  SM64DS_YOSHI_SWALLOW            the tongue-to-mouth transfer the port's
                                  frozen head animation never reaches
                                  (pre-existing; see yoshi_egg_proof.py)
  SM64DS_SELFTEST_TONGUE_THROW    a SECOND B press. That is the throw: it
                                  enters St_YoshiPower_Init, which does
                                  func_ov002_020ed63c(mHeldObj, 1). New here --
                                  without it no test could ever reach the
                                  flight, which is why slot 30 went unmeasured.
  SM64DS_EGG_TRACE                one read-only line per egg tick

RECIPE (reproducible from a clean tree):

    python tools/asset_catalog.py generate "<path to the .nds>"
    cmd /c port\build-port.cmd
    python -u port/tools/yoshi_egg_throw_proof.py

Exit 0 = all six hold. Exit 1 = at least one fails, with the reason printed.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
EXE = os.path.join(REPO, "build", "port", "walk_window.exe")

THROW_FRAME = os.environ.get("PROOF_THROW_FRAME", "380")

ENV = {
    "SM64DS_CHARACTER": "3",
    "SM64DS_LEVEL": "6",
    "SM64DS_WINDOW_SELFTEST": "900",
    "SM64DS_SELFTEST_TONGUE": "1",
    "SM64DS_SELFTEST_TONGUE_ONCE": "1",
    "SM64DS_SELFTEST_TONGUE_THROW": THROW_FRAME,
    "SM64DS_YOSHI_EGG_REPRO": "200",
    "SM64DS_YOSHI_EGG_CLASS": "200",
    "SM64DS_YOSHI_EGG_WIN": "700",
    "SM64DS_YOSHI_SWALLOW": "1",
    "SM64DS_EGG_TRACE": "1",
    "SM64DS_TRACE_STATE": "2",
    "SM64DS_RS_PROBE": "1",
    "SM64DS_TEST_LOCK": "1",
    "SM64DS_TEST_LOCK_PATH": r"C:\tmp\sm64ds-test-slot\windowed_test.lock",
    "SM64DS_TEST_LOCK_TIMEOUT": "5400",
}

FRAME = re.compile(r"^\[f(\d+)\] pos=\(([-\d.]+),([-\d.]+),([-\d.]+)\).*?st=([0-9a-f]{8})")
EGG = re.compile(r"^\[eggstate\] self=(\S+) state=(\d+) target=(\d+) hops=(\d+)")


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
    lines = text.splitlines()

    frames = []
    for line in lines:
        m = FRAME.match(line)
        if m:
            frames.append((int(m.group(1)),
                           (float(m.group(2)), float(m.group(3)), float(m.group(4))),
                           m.group(5)))

    eggs = [EGG.match(l) for l in lines]
    eggs = [m for m in eggs if m]

    # 1. the egg exists
    ticks = text.count("[rsprobe] YOSHI_EGG")
    if ticks < 30 and not eggs:
        fails.append("1 EGG: only %d YOSHI_EGG ticks and no [eggstate] lines -- "
                     "the egg was never laid" % ticks)
    else:
        print("  1 EGG      ok: %d YOSHI_EGG ticks, %d egg-state lines"
              % (ticks, len(eggs)))

    # 2. reached state 1 (thrown)
    thrown = [m for m in eggs if m.group(2) == "1"]
    if not thrown:
        seen = sorted(set(m.group(2) for m in eggs))
        fails.append("2 THROW: the egg never reached state 1 (thrown). States "
                     "seen: %s -- the throw did not happen, so the flight and "
                     "slot 30 were never exercised" % (seen or "none"))
    else:
        print("  2 THROW    ok: %d frames in state 1 (thrown, in flight)"
              % len(thrown))

    # 3. acquired a target
    aimed = [m for m in thrown if m.group(3) != "0"]
    if not aimed:
        fails.append("3 TARGET: the egg flew but never acquired a target "
                     "(+0x410 stayed 0) -- func_ov002_020ecd18 never reached "
                     "the slot-30 dispatch, so this run proves nothing")
    else:
        print("  3 TARGET   ok: %d flight frames with a target acquired "
              "(uid %s)" % (len(aimed), aimed[0].group(3)))

    # 4. nothing quarantined
    q = [l for l in lines if "[quarantine]" in l]
    if q:
        fails.append("4 FAULT: %d quarantine(s): %s" % (len(q), q[0].strip()))
    else:
        print("  4 FAULT    ok: no actor was quarantined")

    # 5. no slot-30 decline
    d = [l for l in lines if "slot 30" in l and "UNHOSTED" in l]
    if d:
        fails.append("5 SLOT30: %d slot-30 decline(s): %s" % (len(d), d[0].strip()))
    else:
        print("  5 SLOT30   ok: no slot-30 decline was reported")

    # 6. the player is still moving
    tail = frames[-90:]
    if len(tail) < 30:
        fails.append("6 ALIVE: only %d frames of player trace" % len(tail))
    else:
        xs = set(round(f[1][0], 1) for f in tail)
        zs = set(round(f[1][2], 1) for f in tail)
        if len(xs) < 3 and len(zs) < 3:
            fails.append("6 ALIVE: the Player has not moved for the last %d "
                         "frames (stuck at %s) -- FROZEN" % (len(tail), tail[-1][1]))
        else:
            print("  6 ALIVE    ok: %d distinct x / %d distinct z over the last "
                  "%d frames" % (len(xs), len(zs), len(tail)))

    if rc != 0:
        fails.append("0 EXIT: walk_window returned %d" % rc)
    return fails


def main():
    if not os.path.exists(EXE):
        print("no walk_window.exe -- build the port first")
        return 2
    log = os.path.join(REPO, "build", "port", "yoshi_egg_throw_proof.log")
    print("yoshi_egg_throw_proof: BoB, Yoshi, tongue f210, throw f%s, 900 frames"
          % THROW_FRAME)
    rc, text = run(log)
    fails = check(text, rc)
    print("  log: %s" % log)
    if fails:
        print("YOSHI EGG THROW PROOF: FAIL")
        for f in fails:
            print("  " + f)
        return 1
    print("YOSHI EGG THROW PROOF: PASS (6/6)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
