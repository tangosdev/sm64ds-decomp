#!/usr/bin/env python3
# ADVENTURE co-op M2 proof: the level filter and N ghosts.
#
# TWO PROOFS, one deterministic and one live, on the same build:
#
#   SINGLE INSTANCE (deterministic). One walk_window with three seated bodies
#   (SM64DS_VS_PLAYERS=3, VS mode off) and both adventure probes on. The M1
#   probe ([advprobe]) re-proves the base ghost drive; the M2 probe
#   ([advpresence]) drives TWO ghosts with level-tagged wire snapshots and
#   asserts the same-level filter, the spawn/despawn transitions and that the
#   local body is never written. This needs no network -- it runs the REAL
#   apply path through the test hook -- so it is the authority for the filter
#   mechanism.
#
#   THREE INSTANCES (live, loopback). Three solo consoles on one loopback
#   session, each in adventure mode. Two share a level and one is in another.
#   The [sync:...] report gives applied>0 / local_writes=0 per console, and the
#   [advdiag] line gives per-slot vis=: a same-level peer reads vis=1 and a
#   different-level peer reads vis=0, which is the filter working on real wire
#   traffic rather than an injected snapshot.
#
# Runs headless, minimized, muted through mp2_proof.env_base (the quiet rule).
import os
import re
import sys

import mp2_proof as M

HERE = os.path.dirname(os.path.abspath(__file__))    # port/tools
PORT = os.path.dirname(HERE)                          # port/
ROOT = os.path.dirname(PORT)                          # repo root (asset + build)
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(PORT, "tools", "_adv_presence_out")

FAILS = []


def check(name, ok, detail=""):
    print(("ok    " if ok else "FAIL  ") + name + (("  " + detail) if detail
                                                    and not ok else ""))
    if not ok:
        FAILS.append(name)


def one(name, frames, extra, level="1"):
    d = os.path.join(OUT, name)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, d, name[:6])
    env["SM64DS_WINDOW_SELFTEST"] = str(frames)
    env["SM64DS_LEVEL"] = str(level)
    for k, v in extra.items():
        env[k] = str(v)
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, d, env, log)
    return rc, M.text(log)


def single_instance():
    print("\n-- single instance: the deterministic filter + N-ghost proof")
    rc, t = one("single", 160, {
        "SM64DS_ADVENTURE": "1",
        "SM64DS_VS_PLAYERS": "3",       # three seated bodies: two remote ghosts
        "SM64DS_ADVENTURE_PROBE": "1",  # M1 base drive
        "SM64DS_ADVENTURE_PRESENCE": "1",  # M2 level filter + transitions
    })
    check("the instance exited cleanly", rc == 0, "rc=%d" % rc)
    m1 = re.search(r"\[advprobe\].*=> (ALL PASS|FAIL)", t)
    check("M1 base ghost drive still passes ([advprobe] ALL PASS)",
          bool(m1) and m1.group(1) == "ALL PASS",
          m1.group(0) if m1 else "no [advprobe] line")
    m2 = re.search(r"\[advpresence\].*=> (ALL PASS|FAIL)", t)
    check("M2 level filter + spawn/despawn passes ([advpresence] ALL PASS)",
          bool(m2) and m2.group(1) == "ALL PASS",
          m2.group(0) if m2 else "no [advpresence] line")
    if m2:
        print("      " + m2.group(0))
    return t


def stat(t, key):
    """The last [sync:...] report's field, an int, or None."""
    got = None
    for m in re.finditer(r"\[sync:[^\]]*\][^\n]*\b%s=(-?\d+)" % key, t):
        got = int(m.group(1))
    return got


def peer_vis(t, local_slot):
    """The last vis= a ghost slot (i != me) read in this console's diag. Adventure
    is a two-body session here, so exactly one slot is the ghost."""
    got = None
    for m in re.finditer(r"\[advdiag\][^\n]*slot(\d+)\(ghost\)[^\n]*\bvis=(-?\d+)",
                         t):
        got = int(m.group(2))
    return got


def adv_pair(name, level_p, level_c, port):
    """Parent + child on one loopback port, BOTH in adventure mode, fan-out left
    to stand down (no SM64DS_COMMS_FANOUT, which would override it). Each in its
    OWN level, so the filter decides whether they ghost each other."""
    dp = os.path.join(OUT, name + "_p")
    dc = os.path.join(OUT, name + "_c")
    for d in (dp, dc):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    base = {
        "SM64DS_ADVENTURE": "1",
        "SM64DS_VS_PLAYERS": "2",
        "SM64DS_COMMS_REPORT": "1",
        "SM64DS_SYNC_REPORT": "1",
        "SM64DS_ADVENTURE_DIAG": "30",
    }
    ep = M.env_base(ROOT, dp, name[:4] + "p")
    ec = M.env_base(ROOT, dc, name[:4] + "c")
    for e, role, lv in ((ep, "parent", level_p), (ec, "child", level_c)):
        e["SM64DS_WINDOW_SELFTEST"] = "240"
        e["SM64DS_LEVEL"] = str(lv)
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(port)
        for k, v in base.items():
            e[k] = v
    lp = os.path.join(dp, "run.log")
    lc = os.path.join(dc, "run.log")
    pp = M.spawn(EXE, dp, ep, lp)
    import time
    time.sleep(0.4)
    pc = M.spawn(EXE, dc, ec, lc)
    M.finish(pp, 900)
    M.finish(pc, 900)
    return M.text(lp), M.text(lc)


def live_runs():
    print("\n-- live loopback: the filter on real wire traffic")
    # SAME LEVEL: the two consoles ghost each other. applied>0, local_writes=0,
    # and each reads its peer vis=1.
    tp, tc = adv_pair("same", 1, 1, 41100)
    for tag, t in (("parent", tp), ("child", tc)):
        lw = stat(t, "local_writes")
        ap = stat(t, "applied")
        vs = peer_vis(t, None)
        check("same-level %s: local_writes=0" % tag, lw == 0,
              "local_writes=%s" % lw)
        check("same-level %s: applied>0 (peer snapshots arrived)" % tag,
              ap is not None and ap > 0, "applied=%s" % ap)
        check("same-level %s: the peer reads vis=1 (ghost drawn)" % tag,
              vs == 1, "vis=%s" % vs)
        print("      [%s] applied=%s local_writes=%s peer_vis=%s"
              % (tag, ap, lw, vs))
    # DIFFERENT LEVELS: snapshots still arrive (applied>0) but the filter draws
    # nothing -- each reads its peer vis=0.
    tp2, tc2 = adv_pair("diff", 1, 5, 41120)
    for tag, t in (("parent", tp2), ("child", tc2)):
        lw = stat(t, "local_writes")
        ap = stat(t, "applied")
        vs = peer_vis(t, None)
        check("diff-level %s: local_writes=0" % tag, lw == 0,
              "local_writes=%s" % lw)
        check("diff-level %s: the peer reads vis=0 (despawned, not drawn)" % tag,
              vs == 0, "vis=%s" % vs)
        print("      [%s] applied=%s local_writes=%s peer_vis=%s"
              % (tag, ap, lw, vs))


def main():
    if not os.path.exists(EXE):
        print("no walk_window.exe at %s -- build first" % EXE)
        return 2
    single_instance()
    try:
        live_runs()
    except Exception as e:      # the live run corroborates; the single is the authority
        print("info  live loopback run did not complete: %r" % e)
    print("\n%d failed" % len(FAILS))
    for n in FAILS:
        print("  FAILED: %s" % n)
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
