#!/usr/bin/env python3
"""MP3's PLAY-MODE world rungs. The set that would have caught the field failure.

    python port/tools/mp3_play_proof.py [--only N[,N...]] [--seconds N]

WHY THIS FILE EXISTS, stated first because it is the whole lesson.

Rungs A and 9 to 11 in mp3_proof.py prove two players share a world, and they
are green, and the owner's two-window session was still broken four ways. Every
one of those rungs runs under SM64DS_WINDOW_SELFTEST. Play mode -- what he
actually runs -- differs in ways that turned out to matter, and the ONLY rung
that ever ran in play mode was rung 8, which asserts the WIRE is connected and
looks at nothing in the world.

That is the same blindness MP2 wrote up and did not fully close: "every headless
rung sets SM64DS_WINDOW_SELFTEST -- the one configuration where run.log works --
so the whole ladder was blind to the blindness." MP2 fixed the LOGGING half. The
WORLD half is this file.

WHAT PLAY MODE CHANGES, measured rather than assumed:
  * stderr goes to playlog/play_*.log, not to the launcher's handle, so every
    assertion here reads the playlog.
  * there is no frame budget, so the session is bounded by wall time
    (-PlaySeconds on the two-window script) rather than by a frame count.
  * the harness's scripted input paths (SM64DS_PAD_TEST, SM64DS_CLICK_TEST) are
    NOT gated off, and the selftest's automatic forward-walk is NOT running --
    so a play-mode instance sits still unless something drives it. That is why
    these rungs drive with SM64DS_COMMS_INJECT, which enters at the hardware.

FOUR RUNGS, one per symptom the owner reported:

  P1  EACH WINDOW IS ITS OWN PLAYER. The parent's local index is 0 and the
      child's is 1, and each window's local character matches its slot.
      (his: "P2 shows HIM as Mario, should be Luigi")
  P2  INPUT ISOLATION. Input injected on the child moves the child's OWN body
      and NOT the parent's body, in the child's own world.
      (his: "from P2 I can move BOTH Mario and Luigi")
  P3  THE LINK CARRIES BOTH WAYS IN PLAY MODE. Input on the parent reaches the
      child's copy of the parent.
      (his: "NOTHING I do on P1 shows up on P2")
  P4  ONE SHARED WORLD. The two windows agree about where both bodies are.
      (his: "the character positions drift apart")
"""

import os
import re
import subprocess
import sys
import glob

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, "runs", "mg16", "out", "MP3", "play")

VS = re.compile(r"^\[vs\] f(\d+) slot(\d) actor=([0-9A-Fa-f]+) no=(\d) char=(\d) "
                r"pos=\((-?\d+),(-?\d+),(-?\d+)\) touched=(\d+)", re.M)
SEAT = re.compile(r"^\s*\[a2\] VS: (\d+) players, I am slot (\d+)", re.M)
LINK = re.compile(r"^\[comms:level\] transport=loopback.*?slot=(\d+) players=(\d+) role=(\d+)",
                  re.M)


def newest_playlog(d):
    """The newest playlog under an instance directory, or ''."""
    g = sorted(glob.glob(os.path.join(d, "playlog", "play_*.log")),
               key=os.path.getmtime)
    return g[-1] if g else ""


def rows(t, slot):
    out = []
    for m in VS.finditer(t):
        if int(m.group(2)) != slot:
            continue
        out.append(dict(f=int(m.group(1)), actor=m.group(3), no=int(m.group(4)),
                        char=int(m.group(5)), x=int(m.group(6)),
                        y=int(m.group(7)), z=int(m.group(8)),
                        touched=int(m.group(9))))
    return out


def span(rs, key):
    if not rs:
        return 0
    v = [r[key] for r in rs]
    return max(v) - min(v)


def play_session(name, seconds, inj_p=None, inj_c=None):
    """A REAL play-mode two-window session, minimized and muted, bounded by time.

    Driven through the two-window script rather than around it, so what is
    proven is the thing the owner runs. Returns (p1_playlog_text,
    p2_playlog_text, script_stdout).
    """
    d = os.path.join(OUT, name)
    for sub in ("P1", "P2"):
        os.makedirs(os.path.join(d, sub, "tmp"), exist_ok=True)
    script = os.path.join(ROOT, "port", "tools", "mp2_two_windows.ps1")
    # ON THIS RUN'S OWN PORTS. See mp2_proof.PORT_BASE: the shipped default is
    # what a human's two-window session uses, and a harness that binds it can
    # end up in a session with somebody else's game.
    cmd = ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass",
           "-File", script, "-Minimized", "-PlaySeconds", str(seconds),
           "-Port", str(M.PORT_BASE + 48),
           "-Root", ROOT, "-RunDir", d]
    env = dict(os.environ)
    # The injections ride in through the environment the script forwards.
    if inj_p:
        env["MP3_INJECT_P1"] = inj_p
    if inj_c:
        env["MP3_INJECT_P2"] = inj_c
    log = os.path.join(d, "script.log")
    with open(log, "wb") as f:
        subprocess.run(cmd, cwd=d, stdout=f, stderr=subprocess.STDOUT,
                       creationflags=M.NO_CONSOLE, startupinfo=M.SI_MIN,
                       env=env, timeout=seconds + 300)
    t1 = M.text(newest_playlog(os.path.join(d, "P1"))) if newest_playlog(os.path.join(d, "P1")) else ""
    t2 = M.text(newest_playlog(os.path.join(d, "P2"))) if newest_playlog(os.path.join(d, "P2")) else ""
    return t1, t2, M.text(log)


# ---------------------------------------------------------------------------
def rungP1(seconds):
    """EACH WINDOW IS ITS OWN PLAYER.

    The child must know it is slot 1 BY THE TIME IT SEATS THE WORLD, and its
    local character must be the one its slot was given. His report was that P2
    showed him as Mario; the seat line in his own playlog says why.
    """
    t1, t2, _ = play_session("pP1_identity", seconds)
    ok = True
    ok &= M.verdict(bool(t1) and bool(t2),
                    "rungP1 both instances wrote a playlog | P1=%d bytes "
                    "P2=%d bytes" % (len(t1), len(t2)))
    if not (t1 and t2):
        return False

    s1, s2 = SEAT.search(t1), SEAT.search(t2)
    l1, l2 = LINK.search(t1), LINK.search(t2)
    ok &= M.verdict(bool(s1) and bool(s2) and bool(l1) and bool(l2),
                    "rungP1 both seated a world and joined the link")
    if not (s1 and s2 and l1 and l2):
        return False

    seat1, seat2 = int(s1.group(2)), int(s2.group(2))
    wire1, wire2 = int(l1.group(1)), int(l2.group(1))
    # THE SEAT MUST AGREE WITH THE WIRE. This is the assertion his session
    # fails: the child seated "I am slot 0" and the wire said slot 1, because
    # the level booted before the session joined and func_0203da9c() still
    # read the pre-join default.
    ok &= M.verdict(seat1 == wire1 and seat2 == wire2,
                    "rungP1 EACH WINDOW SEATED THE SLOT IT ACTUALLY HAS | "
                    "parent seated %d wire %d ;; child seated %d wire %d"
                    % (seat1, wire1, seat2, wire2))
    ok &= M.verdict(seat2 == 1,
                    "rungP1 THE CHILD IS PLAYER 2 | it seated local index %d "
                    "(1 = the second player; 0 means it believes it is the "
                    "host and drives the host's character)" % seat2)

    # And the character each window presents as must be its slot's character.
    a1, b1 = rows(t1, 0), rows(t1, 1)
    a2, b2 = rows(t2, 0), rows(t2, 1)
    if a1 and b1 and a2 and b2:
        ok &= M.verdict(a1[-1]["char"] == 0 and b1[-1]["char"] == 1 and
                        a2[-1]["char"] == 0 and b2[-1]["char"] == 1,
                        "rungP1 both windows agree on who is who | P1 sees "
                        "slot0 char=%d slot1 char=%d ;; P2 sees slot0 char=%d "
                        "slot1 char=%d (0 Mario, 1 Luigi)"
                        % (a1[-1]["char"], b1[-1]["char"],
                           a2[-1]["char"], b2[-1]["char"]))
    return ok


def rungP2(seconds):
    """INPUT ISOLATION -- the child must not drive the host's character.

    Injected on the CHILD only. In the CHILD's own world, its own body must
    move and the host's body must not. His report: from P2 he could move both.
    """
    t1, t2, _ = play_session("pP2_isolation", seconds, inj_c="key=0x20")
    ok = True
    ok &= M.verdict(bool(t2), "rungP2 the child wrote a playlog")
    if not t2:
        return False
    own = rows(t2, 1)      # the child's own body, in the child's world
    host = rows(t2, 0)     # the host's body, in the child's world
    ok &= M.verdict(bool(own) and bool(host),
                    "rungP2 the child's world has both bodies | slot1 rows=%d "
                    "slot0 rows=%d" % (len(own), len(host)))
    if not (own and host):
        return False
    mine = span(own, "x") + span(own, "z")
    theirs = span(host, "x") + span(host, "z")
    ok &= M.verdict(mine > 4096,
                    "rungP2 the child's OWN body moved | %.1f units" % (mine / 4096.0))
    ok &= M.verdict(theirs < 4096,
                    "rungP2 AND IT DID NOT DRIVE THE HOST'S BODY | the host's "
                    "character moved %.1f units in the child's world while "
                    "only the child was pressing anything; anything above 1.0 "
                    "means one pad is driving two characters"
                    % (theirs / 4096.0))
    return ok


def rungP3(seconds):
    """THE LINK CARRIES HOST -> CHILD IN PLAY MODE.

    Injected on the PARENT only; the CHILD's copy of the parent must move.
    His report: nothing he did on P1 showed up on P2.
    """
    t1, t2, _ = play_session("pP3_p1_to_p2", seconds, inj_p="key=0x20")
    ok = True
    if not t2:
        return M.verdict(False, "rungP3 the child wrote no playlog")
    host_in_child = rows(t2, 0)
    ok &= M.verdict(bool(host_in_child),
                    "rungP3 the child has a body for the host | rows=%d"
                    % len(host_in_child))
    if not host_in_child:
        return False
    moved = span(host_in_child, "x") + span(host_in_child, "z")
    ok &= M.verdict(moved > 4096,
                    "rungP3 WHAT THE HOST DOES SHOWS UP IN THE CHILD'S WORLD | "
                    "the host's body moved %.1f units in the CHILD's window, "
                    "driven only by the host's input" % (moved / 4096.0))
    return ok


def rungP4(seconds):
    """ONE SHARED WORLD -- the two windows must agree about both bodies.

    His report: the positions drift apart. Lockstep with identical inputs is
    deterministic, so the same body at the same round must be at the same place
    in both windows. Compared at the LAST frame both windows reported, which is
    where any drift has had longest to accumulate.
    """
    t1, t2, _ = play_session("pP4_shared", seconds, inj_c="key=0x20")
    ok = True
    if not (t1 and t2):
        return M.verdict(False, "rungP4 both instances did not write playlogs")
    out = []
    for slot in (0, 1):
        r1, r2 = rows(t1, slot), rows(t2, slot)
        if not (r1 and r2):
            out.append((slot, None))
            continue
        # Compare at the highest frame number BOTH windows reported.
        f = min(r1[-1]["f"], r2[-1]["f"])
        p1 = [r for r in r1 if r["f"] == f]
        p2 = [r for r in r2 if r["f"] == f]
        if not (p1 and p2):
            out.append((slot, None))
            continue
        d = (abs(p1[-1]["x"] - p2[-1]["x"]) + abs(p1[-1]["y"] - p2[-1]["y"]) +
             abs(p1[-1]["z"] - p2[-1]["z"]))
        out.append((slot, d))
    for slot, d in out:
        if d is None:
            ok &= M.verdict(False,
                            "rungP4 slot%d could not be compared (a window "
                            "never reported it)" % slot)
            continue
        # One DS unit is 4096. A shared world should agree far tighter than
        # that; a unit of slack absorbs a single frame of lockstep skew.
        ok &= M.verdict(d < 4096,
                        "rungP4 BOTH WINDOWS AGREE ABOUT slot%d | the two "
                        "windows place it %.2f units apart at the same frame "
                        "(they are meant to be running one world)"
                        % (slot, d / 4096.0))
    return ok


RUNGS = [("P1", rungP1), ("P2", rungP2), ("P3", rungP3), ("P4", rungP4)]


def main(argv):
    seconds = 25
    only = None
    for i, a in enumerate(argv):
        if a == "--only" and i + 1 < len(argv):
            only = set(argv[i + 1].split(","))
        if a == "--seconds" and i + 1 < len(argv):
            seconds = int(argv[i + 1])
    os.makedirs(OUT, exist_ok=True)
    ok = True
    import time
    for name, fn in RUNGS:
        if only and name not in only:
            continue
        t0 = time.time()
        ok &= bool(fn(seconds))
        print("  (rung %s took %ds)" % (name, int(time.time() - t0)))
    print()
    print("ALL GREEN" if ok else "RED")
    with open(os.path.join(OUT, "verdicts.txt"), "w") as f:
        f.write("\n".join(M.VERDICTS) + "\n")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
