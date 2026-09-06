#!/usr/bin/env python3
"""Did the ROM's own sleep/wake round trip actually happen?

WHAT THIS MEASURES, and why a link count is not enough. Run link2 lane THR put
src/OS_SleepThread.c, src/OS_WakeupThread.c, src/func_02057f54.c and the idle
thread src/func_02057e34.c into walk_window, replacing the host pair
hal/os_thread.cpp used to define. port/tools/linkage.py will happily report the
four extra TUs whether or not a single instruction of them ever executes. This
runs the binary and asks the harder question:

    did the game context really SLEEP, did the ROM's scheduler really pick the
    idle thread, did the idle thread's halt really wake it through the ROM's
    OS_WakeupThread, and did control really come back?

THE CHAIN, and every link of it except three is ROM code:

    src/func_0201a4d0.c          OS_SleepThread(data_0209d4fc)        ROM
      src/OS_SleepThread.c       set my bit, mark me not-runnable      ROM
        src/func_02057f54.c      reschedule                            ROM
          src/func_0205801c.c    pick the highest-priority runnable    ROM
          ARMSaveContext         bind this thread to its fiber        HOST
          ARMRestoreContext      SwitchToFiber                        HOST
            src/func_02057e34.c  the idle thread: halt forever         ROM
              CP15 WaitForInterrupt   deliver the interrupt work      HOST
              src/OS_WakeupThread.c   clear the word, mark runnable    ROM
                src/func_02057f54.c   reschedule back                  ROM

The three HOST rows are the hand-asm register-file primitives and the CP15 halt
instruction, all three PORT_HOST_ABI in port/hal/boot2_thread.cpp.

TWO RUNS, because one is not a measurement:

  armed    SM64DS_THREAD_PROOF=1 SM64DS_THREAD_TRACE=1. Expect VERDICT
           ROUND-TRIP with switches >= 2 (out to the idle thread and back) and
           vbl_wakes >= 1 (the wake came from OS_WakeupThread, not from the
           starvation bound).

  control  the same plus SM64DS_THREAD_NOFIBER=1, which makes ARMRestoreContext
           refuse to switch. Expect VERDICT NO-SWITCH with refused >= 1 and
           switches == 0: with no fiber to go to, the ROM's sleep cannot reach
           the idle thread.

THE CONTROL IS BOUNDED BY CONSTRUCTION, not by a timeout. src/OS_SleepThread.c
has no loop in it -- it sets a bit, marks itself not-runnable and reschedules --
so a refused reschedule RETURNS. That is the port's behaviour before this lane
and it is why the control cannot hang. The subprocess timeout below is a
backstop for the window, not the proof's mechanism.

BOTH RUNS OPEN A GAME WINDOW, so both take the machine-wide windowed test slot
(port/tools/slot_lock.py) exactly the way port/tools/battery.py does, and both
run minimized-never-activated, unfocused and muted.

    python port/tools/thread_proof.py [repo-root] [--frames N] [--level L]
"""

import argparse
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import slot_lock  # noqa: E402

# Long enough to be a real level run rather than a boot, short enough that two
# of them plus the slot wait fit inside a lane's patience. The proof itself
# fires at static-init time, before frame 0; the frames are what make the
# EXIT-STATS line a statement about GAMEPLAY.
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
    env["SM64DS_THREAD_PROOF"] = "1"
    env["SM64DS_THREAD_TRACE"] = "1"
    env.update(extra)
    cmd = [os.path.join(build, "walk_window.exe")]
    with slot_lock.slot(label="thread_proof %s" % (",".join(extra) or "armed")):
        return subprocess.run(cmd, cwd=build, env=env, timeout=RUN_TIMEOUT,
                              capture_output=True, text=True,
                              creationflags=NO_CONSOLE, startupinfo=SI_MIN)


def parse(text, prefix):
    """Pull the key=value pairs off a `[thr] <prefix> ...` line."""
    for line in (text or "").splitlines():
        line = line.strip()
        if not line.startswith("[thr] " + prefix):
            continue
        out = {}
        for tok in line.split():
            if "=" in tok:
                k, _, v = tok.partition("=")
                out[k] = v
            elif tok == "VERDICT":
                out["VERDICT"] = None
        # VERDICT is the last bare word on the PROOF line.
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
    except ValueError:
        return 0


def show(tag, r):
    print("---- %s: rc=%d ----" % (tag, r.returncode))
    for line in (r.stderr or "").splitlines():
        if line.startswith("[thr]"):
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

    # ---- ARM 1: armed --------------------------------------------------
    armed = launch(build, {}, args.frames, args.level)
    show("ARMED", armed)
    p = parse(armed.stderr, "PROOF switches=")
    if p is None:
        failures.append("armed: no [thr] PROOF line -- the probe never ran")
    else:
        if p.get("VERDICT") != "ROUND-TRIP":
            failures.append("armed: VERDICT %s, wanted ROUND-TRIP"
                            % p.get("VERDICT"))
        if num(p, "switches") < 2:
            failures.append("armed: switches=%s, wanted >= 2 (out and back)"
                            % p.get("switches"))
        if num(p, "vbl_wakes") < 1:
            failures.append("armed: vbl_wakes=%s, wanted >= 1 -- the wake has "
                            "to come from OS_WakeupThread, not the starvation "
                            "bound" % p.get("vbl_wakes"))
    if armed.returncode != 0:
        failures.append("armed: walk_window rc=%d" % armed.returncode)

    # ---- ARM 2: the negative control ------------------------------------
    ctrl = launch(build, {"SM64DS_THREAD_NOFIBER": "1"}, args.frames,
                  args.level)
    show("CONTROL (SM64DS_THREAD_NOFIBER=1)", ctrl)
    c = parse(ctrl.stderr, "PROOF switches=")
    if c is None:
        failures.append("control: no [thr] PROOF line")
    else:
        if num(c, "refused") < 1:
            failures.append("control: refused=%s, wanted >= 1 -- the guard "
                            "never fired, so the control proves nothing"
                            % c.get("refused"))
        if num(c, "switches") != 0:
            failures.append("control: switches=%s, wanted 0"
                            % c.get("switches"))
        if c.get("VERDICT") == "ROUND-TRIP":
            failures.append("control: VERDICT ROUND-TRIP -- the knob did not "
                            "disable the fibers")
    if ctrl.returncode != 0:
        failures.append("control: walk_window rc=%d" % ctrl.returncode)

    # ---- what the whole run did, not just the probe ---------------------
    for tag, r in (("ARMED", armed), ("CONTROL", ctrl)):
        e = parse(r.stderr, "EXIT-STATS")
        if e:
            print("%s whole-run: halts=%s switches=%s vbl_wakes=%s "
                  "starved=%s unknown=%s"
                  % (tag, e.get("halts"), e.get("switches"),
                     e.get("vbl_wakes"), e.get("starved"), e.get("unknown")))

    print()
    if failures:
        print("thread_proof FAIL")
        for f in failures:
            print("  " + f)
        return 1
    print("thread_proof PASS: the ROM's sleep reached the ROM's idle thread "
          "and the ROM's wake brought it back; with the fibers off the same "
          "sleep is refused and returns.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
