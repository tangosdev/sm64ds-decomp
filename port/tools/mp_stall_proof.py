#!/usr/bin/env python3
"""THE STALL LADDER: what a paused peer does to a live lockstep session.

    python port/tools/mp_stall_proof.py [--expect survive|collapse]
                                        [--stall SECONDS] [--only S1,S2,S3]

WHY THIS FILE EXISTS. Field failure, owner live, 2026-08-28 13:03
(runs/mg16/out/MP2/two_windows, the play_20260828_1303* pair): a healthy
two-window session COLLAPSED at round 83, ~3 seconds into the level, the
first time the owner grabbed a window by its title bar. The grabbed window's
frame loop stops inside the Win32 modal move loop; its peer's ROM lockstep
(src/func_0203ea5c.c) waited its whole bound -- 0x4B0 = 1200 turns, visible
in the playlog as exchanges jumping 161 -> 1361 in one frame -- and the
port's pump paced those turns at ~1 ms each, so the ROM's 20-second bound
(1200 VBlanks on the DS) fired after ~1.5-4 wall seconds. Bound expiry is
permanent: data_020a0f04 = 0, the dispatcher's tail closes the carrier, the
woken child knocks at a closed port for its own bound and follows. Every
window drag longer than ~2 seconds killed the session for good.

The stall here is NtSuspendProcess, which stops every thread the way the
modal move loop stops the (single) game thread -- same starvation, scriptable
and exact. Three rungs:

  S1  A PAUSED PEER IS NOT A DEPARTED PEER. Mid-session, the child is
      suspended for --stall seconds (default 2.5, the field shape) and
      resumed. With the VBlank-paced pump the session must survive: no
      "closed after", connected=yes players=2 at the end, rounds advancing
      past the stall. --expect collapse inverts the assertion and is how the
      pre-fix build documents the failure it reproduces.
  S2  S1 WITH THE SYNC LAYER OFF. Same stall, SM64DS_SYNC unset. The verdict
      matching S1's is what exonerates (or convicts) the sync layer and the
      aux queue: the field collapse reproduced with sync off, so the cause is
      carrier/pacing-side, not a sync interaction.
  S3  A DEAD PEER IS STILL A DEATH, AND THE SYNC GATE SHOWS IT. The child is
      KILLED (no Bye -- a crash, not a quit). The parent must still fall back
      to solo by the ROM's own bound (within --s3-window wall seconds of the
      kill), and its sync report afterwards must show gated=N incrementing:
      the liveness gate holding the sync layer quiet on a dead session. This
      rung's expectation does not flip with the fix; only the wall time to
      solo does (about 2 s at the old pacing, about 20 s -- the DS's real
      0x4B0 -- at VBlank pacing).

Port discipline: PID-derived ports (never 5176x), quiet spawns via
mp2_proof's env_base/spawn (CREATE_NO_WINDOW, SW_SHOWMINNOACTIVE,
SM64DS_NO_FOCUS=1, SM64DS_VOLUME=0), and this run kills only the pids it
spawned.
"""

import argparse
import ctypes
import os
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, "runs", "mg16", "out", "MPSTALL")

# Inside this pid's own 128-wide bucket, past mp2_proof's +48 reach, so a
# concurrent mp2/mp3 run from ANOTHER pid cannot collide (different bucket)
# and one from this pid cannot either (different offset).
PORT = M.PORT_BASE + 64

FRAMES = "900"

_nt = ctypes.windll.ntdll
_k32 = ctypes.windll.kernel32


def suspend(p):
    rc = _nt.NtSuspendProcess(int(p._handle))
    if rc != 0:
        raise RuntimeError("NtSuspendProcess failed: 0x%08x" % (rc & 0xffffffff))


def resume(p):
    rc = _nt.NtResumeProcess(int(p._handle))
    if rc != 0:
        raise RuntimeError("NtResumeProcess failed: 0x%08x" % (rc & 0xffffffff))


def tail_text(path):
    try:
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            return f.read()
    except OSError:
        return ""


def wait_for_rounds(logpath, want, timeout_s):
    """Poll a live run.log until the carrier's round counter reaches `want`."""
    deadline = time.monotonic() + timeout_s
    got = 0
    while time.monotonic() < deadline:
        t = tail_text(logpath)
        for m in re.finditer(r"\[loopback:level\].*\bround=(\d+)", t):
            got = int(m.group(1))
        if got >= want:
            return got
        time.sleep(0.25)
    return got


def launch_pair(name, sync_on):
    dp = os.path.join(OUT, name + "_p1")
    dc = os.path.join(OUT, name + "_p2")
    for d in (dp, dc):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    exe = os.path.join(ROOT, "build", "port", "walk_window.exe")
    ep = M.env_base(ROOT, dp, "p1")
    ec = M.env_base(ROOT, dc, "p2")
    for e, role in ((ep, "parent"), (ec, "child")):
        e["SM64DS_WINDOW_SELFTEST"] = FRAMES
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(PORT)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_VS_PROBE"] = "1"
        if sync_on:
            e["SM64DS_SYNC"] = "1"
            e["SM64DS_SYNC_REPORT"] = "1"
    lp = os.path.join(dp, "run.log")
    lc = os.path.join(dc, "run.log")
    pp = M.spawn(exe, dp, ep, lp)
    time.sleep(0.6)
    pc = M.spawn(exe, dc, ec, lc)
    return pp, pc, lp, lc


def last_level_line(t):
    got = ""
    for m in re.finditer(r"^\[comms:level\] transport=.*$", t, re.M):
        got = m.group(0)
    return got


def stall_rung(name, sync_on, stall_s, expect_survive):
    pp, pc, lp, lc = launch_pair(name, sync_on)
    ok = True
    try:
        at = wait_for_rounds(lp, 30, 60)
        if at < 30:
            M.verdict(False, "%s: session never reached round 30 (got %d)"
                      % (name, at))
            return False
        suspend(pc)
        t0 = time.monotonic()
        time.sleep(stall_s)
        resume(pc)
        stall_real = time.monotonic() - t0
        # Let the pair run on well past the stall before the frame budget ends.
        after = wait_for_rounds(lp, at + 120, 40)
    finally:
        rp = M.finish(pp, 300)
        rc_ = M.finish(pc, 300)
    tp, tc = tail_text(lp), tail_text(lc)
    # "[comms:loopback] closed after N rounds" alone is NOT the failure: the
    # atexit close at the end of a healthy full-budget run prints it too. The
    # mid-session death has a marker a shutdown never has -- [comms:level]
    # report frames with connected=no -- because the collapsed pair keeps
    # running solo and reporting, while a shutdown stops reporting first.
    dead_p = "connected=no" in tp
    dead_c = "connected=no" in tc
    lvl_p, lvl_c = last_level_line(tp), last_level_line(tc)
    survived = (not dead_p and not dead_c
                and "connected=yes" in lvl_p and "connected=yes" in lvl_c
                and "players=2" in lvl_p and after > at)
    detail = ("stall %.2fs rounds %d->%d dead_p=%s dead_c=%s p1='%s'"
              % (stall_real, at, after, dead_p, dead_c, lvl_p))
    if expect_survive:
        ok = M.verdict(survived, "%s: session survives a %.1fs peer stall -- %s"
                       % (name, stall_s, detail))
    else:
        collapsed = (dead_p and dead_c and "closed after" in tp
                     and "the parent left" in tc)
        ok = M.verdict(collapsed, "%s: EXPECT-COLLAPSE documents the pre-fix "
                       "failure -- %s" % (name, detail))
    return ok


def s3_dead_peer(s3_window):
    name = "s3_kill"
    pp, pc, lp, lc = launch_pair(name, sync_on=True)
    try:
        at = wait_for_rounds(lp, 30, 60)
        if at < 30:
            M.verdict(False, "s3: session never reached round 30 (got %d)" % at)
            return False
        pc.kill()
        t0 = time.monotonic()
        solo_at = None
        while time.monotonic() - t0 < s3_window:
            t = tail_text(lp)
            line = last_level_line(t)
            if line and "connected=no" in line:
                solo_at = time.monotonic() - t0
                break
            time.sleep(0.5)
        # Give the parent a few frames of solo life so the gate counter shows.
        time.sleep(3)
    finally:
        rp = M.finish(pp, 300)
        try:
            pc.kill()
        except OSError:
            pass
        M.finish(pc, 30)
    tp = tail_text(lp)
    gated = [int(m.group(1))
             for m in re.finditer(r"\[sync:level\].*\bgated=(\d+)", tp)]
    gate_line = "[sync] gated:" in tp
    ok1 = M.verdict(solo_at is not None,
                    "s3: a killed peer still ends the session by the ROM's own "
                    "bound (solo after %s s, window %ds)"
                    % ("%.1f" % solo_at if solo_at is not None else "never",
                       s3_window))
    ok2 = M.verdict(bool(gated) and gated[-1] > 0 and gate_line,
                    "s3: the sync liveness gate held on the dead session "
                    "(gated=%s, transition line %s)"
                    % (gated[-1] if gated else "absent",
                       "present" if gate_line else "MISSING"))
    return ok1 and ok2


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--expect", choices=("survive", "collapse"),
                    default="survive")
    ap.add_argument("--stall", type=float, default=2.5)
    ap.add_argument("--s3-window", type=int, default=45)
    ap.add_argument("--only", default="")
    args = ap.parse_args()
    only = set(x.strip().lower() for x in args.only.split(",") if x.strip())

    exe = os.path.join(ROOT, "build", "port", "walk_window.exe")
    if not os.path.isfile(exe):
        print("no walk_window.exe at " + exe)
        return 2
    os.makedirs(OUT, exist_ok=True)
    expect_survive = args.expect == "survive"

    ok = True
    if not only or "s1" in only:
        ok &= stall_rung("s1_sync", True, args.stall, expect_survive)
    if not only or "s2" in only:
        ok &= stall_rung("s2_nosync", False, args.stall, expect_survive)
    if not only or "s3" in only:
        ok &= s3_dead_peer(args.s3_window)

    print("stall ladder: %s" % ("ALL GREEN" if ok else "RED"), flush=True)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
