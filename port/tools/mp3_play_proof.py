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

FIVE RUNGS. P0 is a gate on the others; P1..P4 are one per symptom the owner
reported.

  P0  PORT ISOLATION. This run's two instances are the ONLY consoles on this
      wire, for the whole cycle. Everything below it is meaningless if it is
      red -- see its own banner, which is the lane's most expensive lesson.

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
# RUNG P0 -- PORT ISOLATION. THE GATE EVERY OTHER RUNG DEPENDS ON.
#
# WHY THIS EXISTS, and it is the most expensive lesson in the lane.
# kCommsLoopbackPortBase is 51765 and every harness in this tree bound it,
# including a play-mode loop that was cycling two-window sessions every twenty
# seconds while the owner was live-driving a DIFFERENT two-window session on
# the same machine and the same base. Two unrelated runs on one loopback base do
# not fail to connect -- they SUCCEED, and form one session out of four
# instances. Nothing logs an error. Both runs report connected=yes and keep
# going, and every measurement either side takes is contaminated.
#
# So this rung runs FIRST and everything after it is only trustworthy if it is
# green. It asserts, for the WHOLE cycle rather than at one sample:
#   * the parent never saw more than two players -- a third joiner is somebody
#     else's console arriving on our wire,
#   * the carrier's live mask never went past our two slots (0x3),
#   * both instances are on the port base THIS run derived from its own pid.
#
# A FAILURE HERE INVALIDATES THE REST OF THE FILE and says so, rather than
# letting a contaminated run produce plausible numbers.
PIDLINE = re.compile(r"^MP2 TWO-WINDOW: (P\d) pid (\d+) port (\d+)", re.M)
PLAYERS = re.compile(r"^\[comms:level\] transport=loopback.*?players=(\d+)", re.M)
LIVE = re.compile(r"^\[loopback:level\].*?live=0x([0-9a-fA-F]+)", re.M)


def rungP0(seconds):
    t1, t2, script = play_session("pP0_isolation", seconds)
    ok = True

    pids = PIDLINE.findall(script)
    ok &= M.verdict(len(pids) == 2,
                    "rungP0 this run started exactly two instances | %s"
                    % (", ".join("%s pid=%s port=%s" % p for p in pids) or "NONE"))

    ok &= M.verdict(bool(t1) and bool(t2),
                    "rungP0 both wrote playlogs | P1=%d P2=%d bytes"
                    % (len(t1), len(t2)))
    if not (t1 and t2):
        return False

    # THE WHOLE CYCLE, not a sample. A stranger joining halfway through is
    # exactly the case a single end-of-run reading would miss.
    counts = [int(x) for x in PLAYERS.findall(t1)]
    worst = max(counts) if counts else 0
    ok &= M.verdict(bool(counts) and worst == 2,
                    "rungP0 THE PARENT NEVER SAW A THIRD CONSOLE | max "
                    "players=%d across %d reports (3+ means another session's "
                    "instance joined our wire and every measurement in this "
                    "file is contaminated)" % (worst, len(counts)))

    masks = [int(x, 16) for x in LIVE.findall(t1)]
    worstmask = max(masks) if masks else 0
    ok &= M.verdict(bool(masks) and worstmask <= 0x3,
                    "rungP0 and the carrier's live mask stayed our two slots | "
                    "widest mask 0x%x across %d reports (anything above 0x3 is "
                    "a slot we did not start)" % (worstmask, len(masks)))

    ports = set(int(p[2]) for p in pids)
    ok &= M.verdict(bool(ports) and min(ports) >= 56000,
                    "rungP0 on this run's OWN port base, clear of the shipped "
                    "default 51765 a human's session binds | %s"
                    % sorted(ports))
    if not ok:
        print("      *** rungP0 FAILED: the wire was not ours alone, so every "
              "verdict below this line is meaningless. Fix isolation first. ***")
    return ok


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
    # DIFFERENTIAL, because "the host's body did not move at all" is the wrong
    # question and asserting it produced a red on a correct build.
    #
    # The two bodies are spawned deliberately OVERLAPPING (40.0 units apart
    # against 40.0-unit radii, see hal/level_boot.cpp), so the ROM's cylinder
    # solver pushes them apart on the first frames whatever anybody presses --
    # and a child that walks into the host shoves it, which is rung 11's whole
    # point and is CORRECT multiplayer. A raw "the host moved 41.4 units"
    # cannot tell that push apart from one pad driving two characters.
    #
    # So run it twice, pressed and explicitly idle, and compare the HOST's
    # motion between the two. Collision is present in both arms and cancels;
    # what survives is the part the child's INPUT added to the host's body,
    # which is the thing that must be zero.
    t1a, t2a, _ = play_session("pP2_child_presses", seconds, inj_c="key=0x20")
    t1b, t2b, _ = play_session("pP2_child_idle", seconds, inj_c="key=0x0")
    ok = True
    ok &= M.verdict(bool(t2a) and bool(t2b),
                    "rungP2 the child wrote a playlog in both arms")
    if not (t2a and t2b):
        return False
    own_a, host_a = rows(t2a, 1), rows(t2a, 0)
    own_b, host_b = rows(t2b, 1), rows(t2b, 0)
    ok &= M.verdict(bool(own_a) and bool(host_a) and bool(own_b) and bool(host_b),
                    "rungP2 the child's world has both bodies in both arms | "
                    "pressed slot1=%d slot0=%d ;; idle slot1=%d slot0=%d"
                    % (len(own_a), len(host_a), len(own_b), len(host_b)))
    if not (own_a and host_a and own_b and host_b):
        return False

    mine = span(own_a, "x") + span(own_a, "z")
    ok &= M.verdict(mine > 4096,
                    "rungP2 the child's OWN body moved when it pressed | %.1f "
                    "units" % (mine / 4096.0))

    # Where the HOST's body ends up, pressed arm vs idle arm.
    host_delta = (abs(host_a[-1]["x"] - host_b[-1]["x"]) +
                  abs(host_a[-1]["z"] - host_b[-1]["z"]))
    ok &= M.verdict(host_delta < mine / 4,
                    "rungP2 AND THE CHILD'S INPUT DID NOT DRIVE THE HOST'S BODY "
                    "| pressing moved the host's character %.1f units versus "
                    "not pressing, against %.1f units of the child's own "
                    "travel. A pad driving two characters puts those two "
                    "numbers in the same class; contact between them does not."
                    % (host_delta / 4096.0, mine / 4096.0))
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


# P0 FIRST AND ALWAYS. It is the gate: if the wire was not ours alone, nothing
# below it measured this build.
RUNGS = [("P0", rungP0), ("P1", rungP1), ("P2", rungP2), ("P3", rungP3),
         ("P4", rungP4)]


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
