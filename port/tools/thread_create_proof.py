#!/usr/bin/env python3
"""Did the ROM's own thread CREATION make a thread, and did that thread run?

WHAT THIS MEASURES, and why a link count is not enough. Run link100 lane THREAD
put src/func_02058200.c (the ROM's OS_CreateThread), src/func_02058538.c (its
slot allocator), src/func_020581a8.cpp (its exit thunk), src/func_02058048.c
and the card driver's own bring-up src/func_0206002c.c into walk_window.
port/tools/linkage.py will report all of them whether or not one instruction
executes. This runs the binary and asks the harder question, in four rungs:

    did func_02058200 really write the ROM's record, on the ROM's own bytes;
    was the thread it built really ENTERED; did it really SLEEP through the
    ROM's own OS_SleepThread; and does the ROM's own func_02058048 really wake
    it and put it back to sleep through func_02057f54?

THE CHAIN, and every link of it except the fiber primitives is ROM code:

    hal/boot_os.cpp                func_02042f68's first arm, at its own point
      src/func_0206002c.c          the card driver's bring-up               ROM
        src/func_02058200.c        allocate a slot, paint the stack, build   ROM
                                   the context, store the exit thunk
          src/func_02058538.c      the free-slot scan                        ROM
          hal/scene_vs_menu.cpp    OS_InitContext (hand-asm)              HOST
          src/func_020584d0.c      priority insert into the ready list       ROM
          src/MultiStore_Int       zero the stack interior                   ROM
        src/func_02058048.c        state = 1, reschedule                     ROM
          src/func_02057f54.c      pick and switch                           ROM
            ARMSaveContext /       bind and SwitchToFiber                  HOST
            ARMRestoreContext      (adoption reads the record above)
              src/func_020602bc.cpp  the card thread's own service loop      ROM
                src/OS_SleepThread.c not runnable, reschedule                ROM

TWO RUNS, because one is not a measurement:

  armed    SM64DS_THREAD_CREATE_PROOF=1. Expect VERDICT THREAD-RAN with
           created=1 entered=1 slept=1 woken=1, at least one adoption, and at
           least two switches out of the wake.

  control  the same plus SM64DS_THREAD_NOCREATE=1, which makes
           hal/boot2_thread.cpp refuse to adopt a created record. Expect
           created=1 STILL -- the ROM's creation really happened and the
           control must not be able to hide it -- and entered=0, woken=0,
           VERDICT NOT-ENTERED, with nocreate >= 1 so the refusal is on the
           record rather than assumed.

THE CONTROL IS BOUNDED BY CONSTRUCTION, not by a timeout, for the same reason
port/tools/thread_proof.py's is: nothing on the refusal path loops. The probe
also puts the two ROM writes the refusal stranded back where func_02058200 left
them, so the rest of the control run is not poisoned.

BOTH RUNS OPEN A GAME WINDOW, so both take the machine-wide windowed test slot
(port/tools/slot_lock.py) exactly the way port/tools/battery.py does, and both
run minimized-never-activated, unfocused and muted.

    python port/tools/thread_create_proof.py [repo-root] [--frames N] [--level L]
"""

import argparse
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import slot_lock  # noqa: E402

# The probe fires inside hal/boot_os.cpp's boot spine, before frame 0. The
# frames are what make the EXIT-STATS line a statement about GAMEPLAY -- a
# created thread that is woken by the game rather than only by this probe.
DEFAULT_FRAMES = 300
DEFAULT_LEVEL = 1
RUN_TIMEOUT = 600

NO_CONSOLE = getattr(subprocess, "CREATE_NO_WINDOW", 0)
if hasattr(subprocess, "STARTUPINFO"):
    SI_MIN = subprocess.STARTUPINFO()
    SI_MIN.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI_MIN.wShowWindow = 7  # SW_SHOWMINNOACTIVE
else:
    SI_MIN = None


def launch(build, extra, frames, level):
    env = dict(os.environ)
    env["SM64DS_LEVEL"] = str(level)
    env["SM64DS_WINDOW_SELFTEST"] = str(frames)
    env["SM64DS_NO_FOCUS"] = "1"
    env["SM64DS_VOLUME"] = "0"
    env["SM64DS_THREAD_CREATE_PROOF"] = "1"
    env["SM64DS_THREAD_TRACE"] = "1"
    env.update(extra)
    cmd = [os.path.join(build, "walk_window.exe")]
    with slot_lock.slot(label="thread_create_proof %s"
                              % (",".join(extra) or "armed")):
        return subprocess.run(cmd, cwd=build, env=env, timeout=RUN_TIMEOUT,
                              capture_output=True, text=True,
                              creationflags=NO_CONSOLE, startupinfo=SI_MIN)


def parse(text, prefix):
    """Pull the key=value pairs off a `[thrc] <prefix> ...` line."""
    for line in (text or "").splitlines():
        line = line.strip()
        if not line.startswith("[thrc] " + prefix):
            continue
        out = {}
        for tok in line.split():
            if "=" in tok:
                k, _, v = tok.partition("=")
                out[k] = v
        toks = line.split()
        if "VERDICT" in toks:
            i = toks.index("VERDICT")
            if i + 1 < len(toks):
                out["VERDICT"] = toks[i + 1]
        return out
    return None


def num(d, k):
    try:
        return int(d.get(k, "0"))
    except (TypeError, ValueError):
        return 0


def show(tag, r):
    print("---- %s: rc=%d ----" % (tag, r.returncode))
    for line in (r.stderr or "").splitlines():
        if line.startswith("[thrc]"):
            print("  " + line)
        elif line.startswith("[thr] EXIT-STATS"):
            print("  " + line)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root", nargs="?", default=".")
    ap.add_argument("--frames", type=int, default=DEFAULT_FRAMES)
    ap.add_argument("--level", type=int, default=DEFAULT_LEVEL)
    args = ap.parse_args()

    build = os.path.join(os.path.abspath(args.root), "build", "port")
    exe = os.path.join(build, "walk_window.exe")
    if not os.path.exists(exe):
        print("no %s -- build walk_window first" % exe)
        return 2

    failures = []

    # ---- ARM 1: armed ---------------------------------------------------
    armed = launch(build, {}, args.frames, args.level)
    show("ARMED", armed)
    p = parse(armed.stderr, "PROOF created=")
    c1 = parse(armed.stderr, "CREATED by ")
    if p is None:
        failures.append("armed: no [thrc] PROOF line -- the probe never ran")
    else:
        if p.get("VERDICT") != "THREAD-RAN":
            failures.append("armed: VERDICT %s, wanted THREAD-RAN"
                            % p.get("VERDICT"))
        for rung in ("created", "entered", "slept", "woken"):
            if num(p, rung) != 1:
                failures.append("armed: %s=%s, wanted 1"
                                % (rung, p.get(rung)))
        if num(p, "adopted") < 1:
            failures.append("armed: adopted=%s, wanted >= 1 -- the created "
                            "record was never turned into a fiber"
                            % p.get("adopted"))
        if num(p, "switches") < 2:
            failures.append("armed: switches=%s on the wake, wanted >= 2 "
                            "(into the card thread and back out of its sleep)"
                            % p.get("switches"))
        if num(p, "state_after") != 0:
            failures.append("armed: state_after=%s, wanted 0 -- the woken "
                            "thread has to run and sleep again"
                            % p.get("state_after"))
    if c1 is not None and c1.get("rung1") != "OK":
        failures.append("armed: rung1=%s -- func_02058200's own record does "
                        "not read back" % c1.get("rung1"))
    if armed.returncode != 0:
        failures.append("armed: walk_window rc=%d" % armed.returncode)

    # ---- ARM 2: the negative control -------------------------------------
    ctrl = launch(build, {"SM64DS_THREAD_NOCREATE": "1"}, args.frames,
                  args.level)
    show("CONTROL (SM64DS_THREAD_NOCREATE=1)", ctrl)
    c = parse(ctrl.stderr, "PROOF created=")
    if c is None:
        failures.append("control: no [thrc] PROOF line")
    else:
        if num(c, "created") != 1:
            failures.append("control: created=%s, wanted 1 -- the control "
                            "removes the FIBER, not the ROM's creation, so "
                            "rung 1 must still pass" % c.get("created"))
        if num(c, "entered") != 0:
            failures.append("control: entered=%s, wanted 0" % c.get("entered"))
        if num(c, "woken") != 0:
            failures.append("control: woken=%s, wanted 0" % c.get("woken"))
        if num(c, "nocreate") < 1:
            failures.append("control: nocreate=%s, wanted >= 1 -- the guard "
                            "never fired, so the control proves nothing"
                            % c.get("nocreate"))
        if c.get("VERDICT") == "THREAD-RAN":
            failures.append("control: VERDICT THREAD-RAN -- the knob did not "
                            "disable adoption")
    if ctrl.returncode != 0:
        failures.append("control: walk_window rc=%d" % ctrl.returncode)

    print()
    if failures:
        print("thread_create_proof FAIL")
        for f in failures:
            print("  " + f)
        return 1
    print("thread_create_proof PASS: the ROM's own func_02058200 built the "
          "card thread's record on the ROM's own bytes, the ROM's own "
          "func_02058048 ran it through func_02057f54, it slept in "
          "OS_SleepThread and was woken back into the same sleep; with "
          "adoption off the same record is built and never entered.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
