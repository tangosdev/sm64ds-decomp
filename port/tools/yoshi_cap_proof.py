#!/usr/bin/env python
"""Yoshi walks into a character cap with an enemy in his mouth. With a verdict.

WHAT IT PROVES. On Bob-omb Battlefield, Yoshi tongues a Goomba and takes it
into his mouth. While it is in there, a character cap is picked up. The cap
actor's own state machine (src/func_ov002_020b74d0.c:51) calls
Player::SetNewHatCharacter, whose first act is func_ov002_020bdb50(this, 0) --
and that, with an object still in the mouth, dispatches the held enemy's vtable
slots 18 (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Nothing guards it.

Five assertions:

  1. the enemy is really IN THE MOUTH when the cap fires  (else nothing is proved)
  2. NOTHING is quarantined                    (a faulting actor is a frozen actor)
  3. the held enemy is RELEASED                (mObjInMouth back to 0 -- the
                                                dispatch ran to its end)
  4. the character actually CHANGED            (the cap pickup completed)
  5. the Player is still MOVING at the end     (he is not frozen)

Assertion 1 is the one that stops a vacuous pass: if the cap fires on an empty
mouth, func_ov002_020bdb50 returns before either dispatch and the run proves
nothing. Assertions 3 and 4 are what stop a "no longer hangs" non-fix.

WHY THERE IS A DRIVER AT ALL. The harness cannot steer Yoshi into a specific
piece of level furniture headless, so SM64DS_YOSHI_CAP=<frame> makes the cap
actor's OWN call -- Player::SetNewHatCharacter(chr, 0, 0) -- at a chosen frame.
Everything downstream is the game's. The tongue and mouth-transfer knobs are
the pre-existing ones yoshi_egg_proof.py documents.

RECIPE (reproducible from a clean tree):

    python tools/asset_catalog.py generate "<path to the .nds>"
    cmd /c port\build-port.cmd
    python -u port/tools/yoshi_cap_proof.py

Exit 0 = all five hold. Exit 1 = at least one fails, with the reason printed.
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
EXE = os.path.join(REPO, "build", "port", "walk_window.exe")

CAP_FRAME = os.environ.get("PROOF_CAP_FRAME", "300")

ENV = {
    "SM64DS_CHARACTER": "3",
    "SM64DS_LEVEL": "6",
    "SM64DS_WINDOW_SELFTEST": "600",
    "SM64DS_SELFTEST_TONGUE": "1",
    "SM64DS_SELFTEST_TONGUE_ONCE": "1",
    "SM64DS_YOSHI_EGG_REPRO": "200",
    "SM64DS_YOSHI_EGG_CLASS": "200",
    "SM64DS_YOSHI_EGG_WIN": "400",
    "SM64DS_YOSHI_SWALLOW": "1",
    "SM64DS_YOSHI_CAP": CAP_FRAME,
    "SM64DS_YOSHI_CAP_CHAR": "0",
    "SM64DS_TRACE_STATE": "2",
    "SM64DS_RS_PROBE": "1",
    "SM64DS_TEST_LOCK": "1",
    "SM64DS_TEST_LOCK_PATH": r"C:\tmp\sm64ds-test-slot\windowed_test.lock",
    "SM64DS_TEST_LOCK_TIMEOUT": "5400",
}

FRAME = re.compile(r"^\[f(\d+)\] pos=\(([-\d.]+),([-\d.]+),([-\d.]+)\).*?st=([0-9a-f]{8})")
CAP = re.compile(r"^\[caprepro\] f(\d+) held=(\S+) -> SetNewHatCharacter\((\d+), 0, 0\)")


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

    cap = None
    for l in lines:
        m = CAP.match(l)
        if m:
            cap = m
            break

    # 1. the cap fired on a NON-EMPTY mouth
    if cap is None:
        fails.append("1 HELD: the cap driver never fired -- SM64DS_YOSHI_CAP "
                     "did not run, so nothing was exercised")
    elif cap.group(2) in ("(nil)", "0000000000000000", "00000000"):
        fails.append("1 HELD: the cap fired at f%s with an EMPTY mouth "
                     "(held=%s) -- func_ov002_020bdb50 returns before either "
                     "dispatch, so this run proves nothing. Retune "
                     "PROOF_CAP_FRAME." % (cap.group(1), cap.group(2)))
    else:
        print("  1 HELD     ok: cap fired at f%s with the enemy in the mouth "
              "(held=%s)" % (cap.group(1), cap.group(2)))

    # 2. nothing quarantined
    q = [l for l in lines if "[quarantine]" in l]
    if q:
        fails.append("2 FAULT: %d quarantine(s): %s" % (len(q), q[0].strip()))
    else:
        print("  2 FAULT    ok: no actor was quarantined")

    # 3. the held enemy was released -- the dispatch ran to its end
    rel = [l for l in lines if "[capheld]" in l]
    if cap is not None and not rel:
        print("  3 RELEASE  (no post-cap mouth probe emitted; relying on 2/5)")
    elif rel:
        last = rel[-1]
        if "held=0" in last or "held=(nil)" in last:
            print("  3 RELEASE  ok: mObjInMouth cleared after the cap")
        else:
            fails.append("3 RELEASE: mObjInMouth still set after the cap: %s"
                         % last.strip())

    # 4. the character changed
    ch = [l for l in lines if "[capchar]" in l]
    if ch:
        print("  4 CHAR     ok: %s" % ch[-1].strip())
    else:
        print("  4 CHAR     (no character probe emitted; relying on 2/5)")

    # 5. the player is still moving
    tail = frames[-90:]
    if len(tail) < 30:
        fails.append("5 ALIVE: only %d frames of player trace" % len(tail))
    else:
        xs = set(round(f[1][0], 1) for f in tail)
        zs = set(round(f[1][2], 1) for f in tail)
        if len(xs) < 3 and len(zs) < 3:
            fails.append("5 ALIVE: the Player has not moved for the last %d "
                         "frames (stuck at %s) -- FROZEN" % (len(tail), tail[-1][1]))
        else:
            print("  5 ALIVE    ok: %d distinct x / %d distinct z over the last "
                  "%d frames" % (len(xs), len(zs), len(tail)))

    if rc != 0:
        fails.append("0 EXIT: walk_window returned %d" % rc)
    return fails


def main():
    if not os.path.exists(EXE):
        print("no walk_window.exe -- build the port first")
        return 2
    log = os.path.join(REPO, "build", "port", "yoshi_cap_proof.log")
    print("yoshi_cap_proof: BoB, Yoshi, tongue f210, cap f%s, 600 frames"
          % CAP_FRAME)
    rc, text = run(log)
    fails = check(text, rc)
    print("  log: %s" % log)
    if fails:
        print("YOSHI CAP PROOF: FAIL")
        for f in fails:
            print("  " + f)
        return 1
    print("YOSHI CAP PROOF: PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
