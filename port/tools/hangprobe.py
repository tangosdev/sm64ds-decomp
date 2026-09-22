#!/usr/bin/env python
"""Run one level or scene, wait for it to wedge, then say WHERE and WHETHER IT SPINS.

A hang leaves no crash report, so this makes one. Two measurements, both taken
on the same wedged process:

  CPU. Sampled over a wall-clock window while the process is stuck. A deadlock
  burns no CPU; an infinite loop burns a core. This is the measurement that
  tells the two apart, and it is taken before the debugger touches anything.

  STACKS. cdb.exe attaches NONINVASIVELY (-pv) and walks every thread. The
  port runs its game on fibers, and a fiber that is RUNNING is on its thread's
  stack, so the wedged one is in this dump. Taken twice, a few seconds apart,
  so a spin shows a moving pc and a block shows the same frame.

Usage: hangprobe.py <exe> <VAR=val ...> [--wait S] [--tag name] [--out dir]
"""

import os
import subprocess
import sys
import time

import psutil

CDB = r"C:\Program Files (x86)\Windows Kits\10\Debuggers\x86\cdb.exe"


def main():
    exe = sys.argv[1]
    args = sys.argv[2:]
    wait = 35.0
    tag = "hang"
    out = os.path.dirname(os.path.abspath(exe))
    setenv = {}
    i = 0
    while i < len(args):
        a = args[i]
        if a == "--wait":
            wait = float(args[i + 1]); i += 2
        elif a == "--tag":
            tag = args[i + 1]; i += 2
        elif a == "--out":
            out = args[i + 1]; i += 2
        else:
            k, _, v = a.partition("=")
            setenv[k] = v
            i += 1

    os.makedirs(out, exist_ok=True)
    exedir = os.path.dirname(os.path.abspath(exe))

    env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    env.update(setenv)

    # Silent and out of the way: no console window, minimised, never activated.
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
    CREATE_NO_WINDOW = 0x08000000

    log = open(os.path.join(out, "%s.stdout.txt" % tag), "wb")
    t0 = time.time()
    p = subprocess.Popen([exe], cwd=exedir, env=env, stdout=log,
                         stderr=subprocess.STDOUT, startupinfo=si,
                         creationflags=CREATE_NO_WINDOW)

    # Give it the wait, but return early if it exits on its own.
    while time.time() - t0 < wait:
        if p.poll() is not None:
            dt = time.time() - t0
            log.close()
            print("EXITED rc=%s after %.1fs -- not a hang" % (p.returncode, dt))
            return 0
        time.sleep(0.5)

    try:
        proc = psutil.Process(p.pid)
    except psutil.NoSuchProcess:
        log.close()
        print("process gone")
        return 0

    report = []
    report.append("pid %d, wedged at %.1fs" % (p.pid, time.time() - t0))
    report.append("threads: %d" % proc.num_threads())

    # 1. CPU over a 6 second window, with per-thread detail.
    def snap():
        try:
            tot = proc.cpu_times()
            per = {t.id: (t.user_time, t.system_time) for t in proc.threads()}
            return (tot.user + tot.system), per
        except psutil.Error:
            return None, {}

    a_tot, a_per = snap()
    time.sleep(6.0)
    b_tot, b_per = snap()
    if a_tot is not None and b_tot is not None:
        burn = b_tot - a_tot
        report.append("CPU over 6.0s wall: %.3fs  (%.0f%% of one core)"
                      % (burn, 100.0 * burn / 6.0))
        report.append("VERDICT: %s" % ("SPIN (burning a core)" if burn > 1.5
                                       else "BLOCKED (no CPU)"))
        for tid, (u, s) in sorted(b_per.items()):
            pu, ps = a_per.get(tid, (u, s))
            d = (u + s) - (pu + ps)
            report.append("  thread %-8d %.3fs over the window" % (tid, d))

    # 2. Stacks, twice.
    for n in (1, 2):
        cmd = [CDB, "-pv", "-p", str(p.pid), "-lines", "-c",
               ".lines -e; ~*kv 200; q"]
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, timeout=180)
            txt = r.stdout + r.stderr
        except Exception as e:
            txt = "cdb failed: %r" % (e,)
        with open(os.path.join(out, "%s.stacks%d.txt" % (tag, n)), "w") as f:
            f.write(txt)
        report.append("stacks%d written (%d bytes)" % (n, len(txt)))
        if n == 1:
            time.sleep(4.0)

    try:
        proc.kill()
    except psutil.Error:
        pass
    log.close()

    text = "\n".join(report)
    with open(os.path.join(out, "%s.verdict.txt" % tag), "w") as f:
        f.write(text + "\n")
    print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
