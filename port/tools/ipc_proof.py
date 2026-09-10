#!/usr/bin/env python3
"""IS THE IPC MODEL ACTUALLY LOAD-BEARING? Three rungs, headless.

    python port/tools/ipc_proof.py [--frames N] [--out DIR] [--only R1,R2,R3]

WHAT IT IS FOR. port/ntr/ipc.cpp models the ARM9<->ARM7 link and
port/hal/boot2_ipc.cpp is the host ARM7 that answers it, and on the strength of
that nine matched TUs joined the link out of port/slice_gate2ipc.txt. Two
claims have to be tested separately, because a green run proves neither on its
own:

  * the ROM's own PXI code RUNS and COMPLETES against the model, and
  * it could not have, without it.

The second is the one a passing build cannot show you. src/func_0205bad8.c's
handshake has no timeout: with nothing answering IPCSYNC its inner spin times
out, resets its counter and starts over forever. So the negative control is run
against a BOUNDED-SPIN GUARD (ntr/ipc.cpp, SM64DS_IPC_SPIN_CAP) which counts
IPCSYNC reads that make no progress and exits 23 with a diagnostic. The control
ends in an exit code, never in a hang.

  R1  MODEL ON. The ROM bring-up runs (src/func_0205b858.c ->
      src/func_0205bad8.c, then src/func_02059e48.c and src/func_0205fde8.c),
      the handshake reports sync=done, the receive self-test walks a word
      through the ROM's own IRQ::IPCRxFifoNotEmptyHandler and its callback
      table, and the game then runs --frames frames and exits clean.

  R2  MODEL OFF (SM64DS_IPC_MODEL=0). Same build, same binary. The bring-up
      still runs -- that is the point -- but nothing answers IPCSYNC, so
      func_0205bad8 cannot converge and the guard must trip: exit 23 with
      "BOUNDED-SPIN GUARD" in the log. A PASS here is a FAILED boot, and that
      is what makes R1 mean something.

  R3  MODEL ON, PLAIN BOOT. No knob at all: the ROM's own PXI arms run from the
      pre-main span (hal/boot_os.cpp) against the model, and the game reaches
      its frame budget and exits 0. SM64DS_IPC_ROM_BOOT is retired (run link100
      integration); the bring-up cannot be switched off any more, so this rung is
      the plain-launch sanity run.

Quiet launches only: CREATE_NO_WINDOW, SW_SHOWMINNOACTIVE, SM64DS_NO_FOCUS=1,
SM64DS_MINIMIZED=1, SM64DS_VOLUME=0. The same shape port/tools/mp2_proof.py
uses. This kills only the processes it started.
"""

import argparse
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import slot_lock

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")

NO_CONSOLE = getattr(subprocess, "CREATE_NO_WINDOW", 0)
if hasattr(subprocess, "STARTUPINFO"):
    SI_MIN = subprocess.STARTUPINFO()
    SI_MIN.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI_MIN.wShowWindow = 7          # SW_SHOWMINNOACTIVE
else:
    SI_MIN = None


def env_base(run_dir, extra):
    e = dict(os.environ)
    # Never inherit a lane's knobs into a proof run.
    for k in list(e):
        if k.startswith("SM64DS_"):
            e.pop(k)
    e["SM64DS_ASSET_ROOT"] = ROOT
    e["SM64DS_FAULTS_FATAL"] = "1"
    e["SM64DS_NO_DIALOG"] = "1"
    e["SM64DS_NO_FOCUS"] = "1"
    e["SM64DS_MINIMIZED"] = "1"
    e["SM64DS_VOLUME"] = "0"
    e["SM64DS_LEVEL"] = "1"
    e["TEMP"] = os.path.join(run_dir, "tmp")
    e["TMP"] = e["TEMP"]
    os.makedirs(e["TEMP"], exist_ok=True)
    e.update(extra)
    return e


def run(name, out_dir, extra, timeout=420):
    """One quiet launch, holding the MACHINE-WIDE WINDOWED SLOT for its whole
    lifetime. Every rung here opens a game window, and this box runs ten lanes:
    without the lock two windowed runs overlap and hand back a red that belongs
    to neither of them. slot_lock.slot() always takes it -- calling it is the
    opt-in -- so this does not depend on SM64DS_TEST_LOCK being set."""
    log = os.path.join(out_dir, "ipc_%s.log" % name)
    with slot_lock.slot(label="ipc_proof %s" % name):
        with open(log, "wb") as f:
            p = subprocess.Popen([EXE], cwd=ROOT, env=env_base(out_dir, extra),
                                 stdout=f, stderr=subprocess.STDOUT,
                                 creationflags=NO_CONSOLE, startupinfo=SI_MIN)
            try:
                rc = p.wait(timeout=timeout)
            except subprocess.TimeoutExpired:
                p.kill()
                p.wait()
                rc = -9
    with open(log, "r", encoding="utf-8", errors="replace") as f:
        return rc, f.read(), log


VERDICTS = []


def check(rung, ok, why):
    VERDICTS.append((rung, bool(ok), why))
    print("  %-4s %-5s %s" % (rung, "PASS" if ok else "FAIL", why))
    return bool(ok)


SYNC_RE = re.compile(r"\[ipc:rom-boot\] sync=(\w+) steps=(\d+) reads=(\d+)")
FRAMES_RE = re.compile(r"selftest: (\d+) frames")
# THE SEND COUNTER, AT THE END OF THE RUN AND NOT AT ROM-BOOT. The rom-boot
# report is taken before any ROM body has had a reason to send, so its send= is
# structurally zero and can test nothing; hal/boot2_ipc.cpp takes the same
# report again at exit (log-gated, and R1 sets the log). THAT one is where "did
# the ARM9 ever actually reach the model" lives, and it read zero for the whole
# life of this port until src/IPCSend.c was hostgen'd -- its store to
# IPCFIFOSEND latched in ntr's mapped I/O window. See GATE2IPC_SYMS.
EXIT_RE = re.compile(r"\[ipc:exit\] sync=\w+ steps=\d+ reads=\d+ send=(\d+)")
# THE RADIO'S THREE NUMBERS (run link100, lane WM3, rung W4). hal/wm_arm7.cpp is
# the host ARM7's WM driver: the ROM's own become-parent and become-child bodies
# are linked now and they send commands on PXI channel 0xa. Commands received,
# replies posted, replies the ROM's own dispatcher consumed -- and the three
# must agree, because a gap between the first two is a command the stub
# swallowed and a gap between the last two is the nested-dispatch drop that
# file's law 1 exists to prevent.
WM_CENSUS_RE = re.compile(
    r"\[wm7:census\] commands (\d+), replies posted (\d+), replies dispatched "
    r"(\d+), unanswered (\d+)")

# Gate A: the association line the same census prints (run link100, lane WM4).
# Gate B: the MP receive line, from the same census (run link100, lane WM4).
WM_MPRECV_RE = re.compile(
    r"\[wm7:census\] mp receive: (\d+) round\(s\) published, last mask "
    r"0x([0-9a-f]+), (\d+) slot\(s\) at peak, stride (\d+), wide buffer (ARMED|not needed)")

WM_ASSOC_RE = re.compile(
    r"\[wm7:census\] association 0x([0-9a-f]+) \(peak 0x([0-9a-f]+), (\d+) "
    r"change\(s\)\), status \+0x86 (\d+), \+0x17e (\d+)")


def rung1(out_dir, frames):
    print("R1  model ON: the ROM's PXI bring-up runs and completes")
    rc, txt, log = run("r1", out_dir, {
        "SM64DS_WINDOW_SELFTEST": str(frames),
        "SM64DS_IPC_LOG": "1",
        "SM64DS_IPC_SELFTEST": "1",
    })
    m = SYNC_RE.search(txt)
    check("R1", m is not None and m.group(1) == "done",
          "IPCSYNC handshake reported %s" % (m.group(1) if m else "NOTHING"))
    if m:
        # The ARM7's loop counts 8 down to -1: nine iterations, no more, and a
        # larger number would mean the handshake restarted.
        check("R1", m.group(2) == "9",
              "handshake took %s host-ARM7 iterations (9 = the ARM7's own "
              "8..0 countdown, no restart)" % m.group(2))
    check("R1", "[arm7] claimed 7 channels" in txt,
          "the host ARM7 claimed its channels in the shared block")
    check("R1", "[ipc:selftest] PASS" in txt,
          "the ROM's IRQ::IPCRxFifoNotEmptyHandler read both pushed words, "
          "dispatched the registered one and returned the unclaimed one")
    xm = EXIT_RE.search(txt)
    check("R1", xm is not None and int(xm.group(1)) > 0,
          "the ARM9 made %s real send(s) into the model over the run "
          "(src/IPCSend.c is hostgen'd, so its store to IPCFIFOSEND reaches "
          "ntr::ipc_reg_write instead of the mapped window)"
          % (xm.group(1) if xm else "0"))
    check("R1", "[arm7:census] tag  7" in txt,
          "the channel-7 sound-command path is among them: the host ARM7 saw "
          "src/func_0205b070.c's per-frame poke and declined it, so "
          "hal/sdat/consumer.cpp still owns the batch")
    # THE RADIO'S THREE NUMBERS. R1 IS A SOLO BOOT, so the honest reading here
    # is ZERO on all of them: the role byte data_020a0f04 never leaves 0, so
    # src/func_0203df40.c takes its solo arm, neither src/func_02040820.c nor
    # src/func_02040790.c is ever called, and nothing is asked of the radio. The
    # NONZERO half of the same measurement belongs to a loopback pair, which
    # this proof does not launch; the lane report carries that capture beside
    # this one. Checking that the line is PRESENT and reads zero is what makes
    # the zero a measurement rather than an absence of output.
    wm = WM_CENSUS_RE.search(txt)
    check("R1", wm is not None,
          "the host ARM7's WM driver reported its census (hal/wm_arm7.cpp)")
    if wm:
        cmds, posted, disp, unans = (int(wm.group(i)) for i in (1, 2, 3, 4))
        check("R1", cmds == 0 and posted == 0 and disp == 0 and unans == 0,
              "a SOLO boot asked the radio nothing: %d command(s), %d reply(ies) "
              "posted, %d dispatched, %d unanswered" % (cmds, posted, disp, unans))
    # GATE A's HALF OF THE SAME MEASUREMENT (run link100, lane WM4). The
    # association bitmap is now written by the ARM7 from the transport, so a
    # solo run owes the zero: no transport, no children, and the two status
    # fields src/func_02062428.c and src/func_020627e8.c read stay at 0. The
    # NONZERO half is a loopback pair and lives in the lane report's capture.
    am = WM_ASSOC_RE.search(txt)
    check("R1", am is not None,
          "the host ARM7 reported its association census (gate A)")
    if am:
        assoc, peak, ev, f86, f17e = (int(am.group(i)) for i in (1, 2, 3, 4, 5))
        check("R1", assoc == 0 and peak == 0 and ev == 0 and f86 == 0 and f17e == 0,
              "a SOLO boot has no association: bitmap 0x%04x (peak 0x%04x, %d "
              "change(s)), status +0x86 %d, +0x17e %d" %
              (assoc, peak, ev, f86, f17e))
    # AND GATE B's, same shape. A solo run completes no round, so the ROM's own
    # MP receive buffer is never published into and the port's wider buffer is
    # never armed. The nonzero half is the loopback capture in the lane report.
    mm = WM_MPRECV_RE.search(txt)
    check("R1", mm is not None,
          "the host ARM7 reported its MP receive census (gate B)")
    if mm:
        rounds, lastmask, peak, stride, wide = (mm.group(i) for i in range(1, 6))
        check("R1", rounds == "0" and peak == "0" and wide == "not needed",
              "a SOLO boot publishes no MP round: %s round(s), last mask 0x%s, "
              "%s slot(s) at peak, stride %s, wide buffer %s" %
              (rounds, lastmask, peak, stride, wide))
    check("R1", "[arm7:census] tag 10" not in txt,
          "and not one word reached tag 10 in a solo run, which is the same "
          "measurement rung W1 was gated on")
    fm = FRAMES_RE.search(txt)
    check("R1", fm is not None and int(fm.group(1)) >= frames,
          "the boot proceeded %s frames" % (fm.group(1) if fm else "0"))
    check("R1", rc == 0, "exit code %d" % rc)
    print("      log: %s" % log)


def rung2(out_dir, cap):
    print("R2  model OFF: the same bodies cannot converge (the control)")
    rc, txt, log = run("r2", out_dir, {
        "SM64DS_WINDOW_SELFTEST": "60",
        "SM64DS_IPC_MODEL": "0",
        "SM64DS_IPC_SPIN_CAP": str(cap),
    })
    check("R2", rc == 23, "exit code %d (23 = the bounded-spin guard)" % rc)
    check("R2", "BOUNDED-SPIN GUARD" in txt,
          "the guard named the failure instead of hanging")
    check("R2", "DISABLED" in txt,
          "the diagnostic says the ARM7 model is the missing piece")
    check("R2", "sync=PENDING" in txt,
          "the handshake was still pending when the guard fired")
    print("      log: %s" % log)


def rung3(out_dir, frames):
    print("R3  model ON, plain boot: the ROM's own bring-up, no knob")
    rc, txt, log = run("r3", out_dir, {
        "SM64DS_WINDOW_SELFTEST": str(frames),
    })
    fm = FRAMES_RE.search(txt)
    check("R3", fm is not None and int(fm.group(1)) >= frames,
          "the game ran %s frames through the ROM's own PXI bring-up"
          % (fm.group(1) if fm else "0"))
    check("R3", "sync=done" in txt, "the handshake completed on the plain launch")
    check("R3", rc == 0, "exit code %d" % rc)
    print("      log: %s" % log)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", type=int, default=300)
    ap.add_argument("--spin-cap", type=int, default=20000)
    ap.add_argument("--out", default=os.path.join(ROOT, "runs", "link2", "ipc"))
    ap.add_argument("--only", default="")
    a = ap.parse_args()

    if not os.path.exists(EXE):
        print("no walk_window.exe at " + EXE)
        return 2
    os.makedirs(a.out, exist_ok=True)
    only = set(x.strip().upper() for x in a.only.split(",") if x.strip())

    print("ipc_proof: %s" % EXE)
    if not only or "R1" in only:
        rung1(a.out, a.frames)
    if not only or "R2" in only:
        rung2(a.out, a.spin_cap)
    if not only or "R3" in only:
        rung3(a.out, a.frames)

    bad = [v for v in VERDICTS if not v[1]]
    print("")
    print("%d checks, %d failed" % (len(VERDICTS), len(bad)))
    if bad:
        for rung, _, why in bad:
            print("  FAILED %s: %s" % (rung, why))
        return 1
    print("ALL GREEN")
    return 0


if __name__ == "__main__":
    sys.exit(main())
