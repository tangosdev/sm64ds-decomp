#!/usr/bin/env python3
"""Exercise a save/load cycle on real levels, not just in the smoke harness.

tests/smoke_savestate.cpp proves the slot restores over one spawned actor in a
bare world. That is the unit test. This is the other half: the same save and
load driven through walk_window on levels that are actually mounted, where the
world is a real level's object graph and the out-of-arena state is whatever a
level boot left behind.

Two shapes, both headless:

  IN-LEVEL     save, run on, load, and assert that the hosted message lock and
               the hardware content stores came back (SM64DS_SS_LOCK +
               SM64DS_SS_ASSERT, the acceptance test walk_window already has).

  CROSS-LEVEL  the packed-gap regression. Boot a level that does NOT own ov009,
               save, warp to castle grounds so ov009's sinits run and write
               through a synthetic gap block, load, and assert the write rolled
               back (SM64DS_SS_WATCH_FLAG=2). This one goes RED on a build
               whose packed mounts emit their gap blocks outside .dsstate,
               which is what it is for. It refuses to pass vacuously: the run
               says VACUOUS if the watched halfword never moved.

    python port/tools/savestate_soak.py [repo-root] [--levels 1,2,6,7,8]

Exit 0 if every run passed, 1 on the first failure, with one line per run.
"""

import os
import re
import subprocess
import sys
from pathlib import Path

FRAMES = "300"
SAVE_AT = "60"
LOAD_AT = "260"
WARP_AT = "80"
# The cross-level arm re-enters castle grounds AFTER the load, because that is
# where the question actually is. The restore rolls ov009's image back to the
# raw DS bytes -- correctly, since the restored world is one in which ov009 was
# never mounted -- so a raw pointer immediately after the load is right, not
# wrong. What has to be true is that the NEXT entry patches it again. Before
# RELOAD2 it did not: the mount cache and the sinit flag were plain host
# statics that no restore rolled back, so the pass was skipped forever and the
# world held a raw pointer for the rest of the session.
REENTER_AT = "300"
CROSS_FRAMES = "380"
CASTLE_GROUNDS = 1
DEFAULT_LEVELS = (1, 2, 6, 7, 8)
TIMEOUT = 600


def run(exe, build, env):
    # SM64DS_NO_FOCUS: SM64DS_WINDOW_SELFTEST is the arm that OPENS A GAME
    # WINDOW, and a soak opens one per run over five levels, so this spawner
    # needs what battery.py's selftest_env gets. CREATE_NO_WINDOW below only
    # suppresses the console. In the BASE dict rather than after the update, so
    # a caller that genuinely wants the foreground can still say so through
    # `env` -- nothing in the tree does.
    e = dict(os.environ, SM64DS_FAULTS_FATAL="1",
             SM64DS_WINDOW_SELFTEST=FRAMES,
             SM64DS_NO_FOCUS="1")
    # never let an inherited knob decide what a soak run tests
    for k in ("SM64DS_SKIP_CLASS", "SM64DS_SCENE", "SM64DS_SS_DISKLOAD",
              "SM64DS_SS_DISK"):
        e.pop(k, None)
    e.update(env)
    try:
        # CREATE_NO_WINDOW: see battery.py's NO_CONSOLE. A console-subsystem
        # child launched with no console to inherit makes one, and a soak makes
        # a lot of children; capture_output already pipes all three handles.
        # SW_SHOWMINNOACTIVE via STARTUPINFO: same reason and same proof as
        # battery.py's SI_MIN -- the game window starts minimized instead of
        # appearing over the desk; walk_window honours the request.
        _si = None
        if hasattr(subprocess, "STARTUPINFO"):
            _si = subprocess.STARTUPINFO()
            _si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            _si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
        return subprocess.run([exe], cwd=build, env=e, timeout=TIMEOUT,
                              capture_output=True, text=True,
                              creationflags=getattr(subprocess,
                                                    "CREATE_NO_WINDOW", 0),
                              startupinfo=_si)
    except subprocess.TimeoutExpired:
        return None


def main():
    argv = sys.argv[1:]
    levels = DEFAULT_LEVELS
    if "--levels" in argv:
        i = argv.index("--levels")
        levels = tuple(int(x) for x in argv[i + 1].split(","))
        del argv[i:i + 2]
    root = Path(argv[0]) if argv else Path(__file__).resolve().parents[2]
    build = root / "build" / "port"
    exe = build / "walk_window.exe"
    if not exe.exists():
        sys.exit(f"savestate_soak: no {exe}; build first")

    bad = 0
    for lvl in levels:
        r = run(str(exe), str(build),
                {"SM64DS_LEVEL": str(lvl), "SM64DS_SS_SAVE": SAVE_AT,
                 "SM64DS_SS_LOAD": LOAD_AT, "SM64DS_SS_LOCK": "1",
                 "SM64DS_SS_ASSERT": "1"})
        if r is None:
            print(f"level {lvl} in-level save/load: FAIL (timeout)")
            bad += 1
            continue
        log = r.stdout + r.stderr
        ok = r.returncode == 0 and "[ss-repro] PASS" in log
        print(f"level {lvl} in-level save/load: {'ok' if ok else 'FAIL'}"
              + ("" if ok else f" rc={r.returncode}"))
        if not ok:
            for line in log.splitlines():
                if "ss-repro" in line or "FAIL" in line:
                    print("    " + line[:160])
            bad += 1

    # the cross-level packed-gap regression, from every level that is not
    # castle grounds itself (booting castle grounds runs the sinits before the
    # save, which makes the run vacuous and says so)
    for lvl in levels:
        if lvl == CASTLE_GROUNDS:
            continue
        r = run(str(exe), str(build),
                {"SM64DS_LEVEL": str(lvl),
                 "SM64DS_WARP_SEQ": f"{CASTLE_GROUNDS}@{WARP_AT},"
                                    f"{CASTLE_GROUNDS}@{REENTER_AT}",
                 "SM64DS_WINDOW_SELFTEST": CROSS_FRAMES,
                 "SM64DS_SS_SAVE": SAVE_AT, "SM64DS_SS_LOAD": LOAD_AT,
                 "SM64DS_SS_WATCH_FLAG": "2"})
        if r is None:
            print(f"level {lvl} -> castle grounds packed-gap: FAIL (timeout)")
            bad += 1
            continue
        log = r.stdout + r.stderr
        # THE THREE THINGS THIS ARM HAS TO SEE, in order, from the coverage of
        # ov009's own pointer rather than from a halfword that never moves.
        # The [ss-flag] line reports whether the record the mount's patch pass
        # rebased is reachable in .dsstate or is back on the raw DS
        # reservation, and it now fires on a coverage change as well as a value
        # change, so all three transitions are visible.
        seen = [(int(f), "in .dsstate" in s) for f, s in
                re.findall(r"\[ss-flag\] f(\d+) .*?, storage ([^\n]*)", log)]
        patched = [f for f, cov in seen if cov and f < int(LOAD_AT)]
        rolled = [f for f, cov in seen if not cov and f >= int(LOAD_AT)]
        repatched = [f for f, cov in seen if cov and rolled and f > rolled[0]]
        why = ""
        if not patched:
            why = (" (VACUOUS: the mount's patch pass never ran before the "
                   "save, so the load had nothing to roll back)")
        elif not rolled:
            why = (" (VACUOUS: the pointer never left .dsstate, so the load "
                   "cannot test a rollback)")
        elif not repatched:
            why = (" (the pass did NOT run again after the restore: the world "
                   "is holding a raw DS pointer for the rest of the session)")
        ok = r.returncode == 0 and patched and rolled and repatched
        print(f"level {lvl} -> castle grounds packed-gap: "
              f"{'ok' if ok else 'FAIL'}"
              + ("" if r.returncode == 0 else f" rc={r.returncode}") + why)
        if not ok:
            for line in log.splitlines():
                if "ss-flag" in line or "ss-gap" in line or "FAULT" in line:
                    print("    " + line[:160])
            bad += 1

    print(f"\nsavestate_soak: {'ALL GREEN' if not bad else f'{bad} FAILED'}")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
