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
      child's is 1, and every slot carries the character the ROM's VS arm
      gives it: 3, Yoshi, on both, told apart by palette row rather than by
      model. (his: "P2 shows HIM as Mario, should be Luigi". The IDENTITY half
      of that report was real and the seat assertions below are what catch it.
      The CHARACTER half was not the ROM's -- Luigi was never the right answer
      for a VS slot -- and the assertion that encoded it is corrected below,
      with the reasoning at the character check.)
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
                r"pos=\((-?\d+),(-?\d+),(-?\d+)\) touched=(\d+) "
                r"pad=([0-9a-f]+) ctrl0=([0-9a-f]+) ang=([0-9a-f]+) "
                r"state=([0-9a-f]+)(?: st_timer=\d+ pal=(-?\d+))?", re.M)
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
                        touched=int(m.group(9)),
                        # THE STATE THE PLAYER IS RUNNING, as a DS address --
                        # the word Player::Behavior's own dispatcher switches
                        # on. rungP6 asserts on this because a button press is
                        # a STATE CHANGE and barely moves a body, which is how
                        # three button seams survived a position-only rung.
                        state=int(m.group(13), 16),
                        # Player+0x61C, the VS palette base: material[0]'s
                        # palette plus (playerNo << 1). yoshi_all_16p_pl is
                        # four stacked 16-colour rows, so consecutive slots
                        # differ by exactly 2. None on a build whose probe
                        # predates the column.
                        pal=(int(m.group(14)) if m.group(14) is not None
                             else None)))
    return out


def span(rs, key):
    if not rs:
        return 0
    v = [r[key] for r in rs]
    return max(v) - min(v)


def play_session(name, seconds, inj_p=None, inj_c=None, extra_env=None):
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
    # Forwarded to BOTH instances by the script's own environment inheritance.
    for k, v in (extra_env or {}).items():
        env[k] = v
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


def isolated(tag, t1, script=""):
    """Assert THIS rung's own cycle was ours alone. Every rung calls it.

    rungP0 proves isolation for the cycle rungP0 ran. It says nothing about the
    cycle rungP1 ran, or P3's, because each rung starts its own pair of
    instances -- so a stranger joining midway through any LATER cycle was
    invisible, and that cycle's numbers would look perfectly reasonable. The
    data was already sitting in every rung's playlog; it just was not read.

    Cheap enough to run everywhere: two regex sweeps of the parent's log.
    """
    counts = [int(x) for x in PLAYERS.findall(t1)]
    masks = [int(x, 16) for x in LIVE.findall(t1)]
    worst = max(counts) if counts else 0
    worstmask = max(masks) if masks else 0
    ok = bool(counts) and worst == 2 and worstmask <= 0x3
    if script:
        ports = [int(m[2]) for m in PIDLINE.findall(script)]
        if ports and min(ports) < 56000:
            ok = False
    return M.verdict(ok,
                     "%s THIS CYCLE'S WIRE WAS OURS ALONE | max players=%d, "
                     "widest live mask 0x%x across %d reports%s"
                     % (tag, worst, worstmask, len(counts),
                        "" if ok else "  -- A STRANGER WAS ON THE WIRE, so "
                        "every other verdict in this rung is meaningless"))


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

    ok &= isolated("rungP1", t1)

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

    # THE CHARACTER CONTRACT IS THE ROM'S, read off each actor's own +0x6d9.
    # A two-player session IS a VS match -- the cartridge has exactly one
    # multiplayer mode, the star battle -- so the port seats data_0209f2d8 = 1
    # the way the VS menu would (hal/level_boot.cpp's a2 seat), and
    # _Z19LoadEntranceObjects...'s loop then runs its VS arm: f2 = 3 for EVERY
    # slot. Four Yoshis, told apart by COLOUR and not by model -- Player::Render
    # re-points every Yoshi material at the body palette row (playerNo << 1)
    # picks, on the same data_0209f2d8 == 1 test.
    #
    # WHAT THIS ASSERTION USED TO SAY was "slot0 char=0 slot1 char=1 (0 Mario,
    # 1 Luigi)", and that was never a ROM property on any path. It encoded a
    # retired port-side stand-in that wrote slot index into data_02092128 --
    # which the loop packs as f1, landing at Player+0x6da, NOT the character at
    # +0x6d9. So the stand-in could not have produced the two characters this
    # line demanded, and it did not: the rung failed here reading char=0 on
    # every slot, which was the save file's character arriving through f2
    # exactly as the ROM's non-VS arm says it should.
    #
    # (rungA in mp3_proof.py asserts the OTHER arm of the same four lines -- a
    # non-VS boot, where every slot carries the save file's one character --
    # and vs_slot1_solo_check.py asserts this arm against a real arena.)
    a1, b1 = rows(t1, 0), rows(t1, 1)
    a2, b2 = rows(t2, 0), rows(t2, 1)
    if a1 and b1 and a2 and b2:
        seen = (a1[-1]["char"], b1[-1]["char"],
                a2[-1]["char"], b2[-1]["char"])
        ok &= M.verdict(all(c == 3 for c in seen),
                        "rungP1 EVERY SLOT IS YOSHI IN BOTH WINDOWS | P1 sees "
                        "slot0 char=%d slot1 char=%d ;; P2 sees slot0 char=%d "
                        "slot1 char=%d (the ROM's VS contract: character 3 on "
                        "every slot, told apart by colour)" % seen)

        # AND THE COLOUR, which is the half that makes four Yoshis playable.
        # func_ov002_020e5948 sets Player+0x61C to the Yoshi body model's
        # material[0] palette base plus (playerNo << 1), and Player::Render
        # writes it into every body and head material record on the same
        # data_0209f2d8 == 1 and character 3 test. yoshi_all_16p_pl is 128
        # bytes -- four stacked 16-colour rows, greens, reds, blues, yellows
        # -- so one step of playerNo is exactly one row and consecutive slots
        # must differ by 2. Asserting the DIFFERENCE rather than the absolute
        # value, because the base is wherever the model got loaded.
        pals = (a1[-1]["pal"], b1[-1]["pal"], a2[-1]["pal"], b2[-1]["pal"])
        if any(v is None for v in pals):
            M.verdict(False, "rungP1 the probe reported no pal column -- this "
                             "build predates it, so the colour half is "
                             "UNMEASURED here, not passing")
            ok = False
        else:
            ok &= M.verdict(pals[1] - pals[0] == 2 and pals[3] - pals[2] == 2,
                            "rungP1 THE TWO YOSHIS ARE DIFFERENT COLOURS | "
                            "P1 pal slot0=%d slot1=%d ;; P2 pal slot0=%d "
                            "slot1=%d -- consecutive slots must differ by 2, "
                            "one 16-colour row of yoshi_all_16p_pl" % pals)
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
    ok &= isolated("rungP2(pressed)", t1a)
    ok &= isolated("rungP2(idle)", t1b)

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
    ok &= isolated("rungP3", t1)

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
    ok &= isolated("rungP4", t1)

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
def rungP5(seconds):
    """BUTTONS ROUTE TO THE PRESSER'S OWN SLOT -- the crouch-bleed rung.

    The tester's precise repro: crouch pressed in the CHILD's window made
    MARIO crouch in the CHILD's world, and not in the host's. So a whole button
    family routed to the wrong slot, on the child only, and inconsistently
    between the two worlds.

    Two seams caused it and both were the PadData[0] clobber's shape one layer
    in: the harness published the local BUTTONS into Ctrl slot 0 unconditionally
    (tests/walk_window.cpp), and the per-player split-symbol fan copied only the
    STICK fields, leaving the button words with nowhere per-player to live --
    hal/actor_vtables.cpp hosted data_0209f49c/f49e as BARE SHORTS.

    Asserted the way rungP2 asserts the stick: differentially. A button held on
    the child must move the child's own body relative to not holding it, and
    must NOT move the host's body relative to not holding it -- in the CHILD's
    own world, which is where the bleed showed.
    """
    # 0x08 is START-adjacent in DS bit order; 0x100 is R, the crouch modifier.
    # Held rather than tapped so a one-frame edge cannot be the difference.
    t1a, t2a, _ = play_session("pP5_btn_pressed", seconds, inj_c="key=0x100")
    t1b, t2b, _ = play_session("pP5_btn_idle", seconds, inj_c="key=0x0")
    ok = True
    ok &= isolated("rungP5(pressed)", t1a)
    ok &= isolated("rungP5(idle)", t1b)
    own_a, host_a = rows(t2a, 1), rows(t2a, 0)
    own_b, host_b = rows(t2b, 1), rows(t2b, 0)
    ok &= M.verdict(bool(own_a) and bool(host_a) and bool(own_b) and bool(host_b),
                    "rungP5 the child's world has both bodies in both arms")
    if not (own_a and host_a and own_b and host_b):
        return False
    host_delta = (abs(host_a[-1]["x"] - host_b[-1]["x"]) +
                  abs(host_a[-1]["z"] - host_b[-1]["z"]))
    ok &= M.verdict(host_delta < 4096,
                    "rungP5 A BUTTON ON THE CHILD DID NOT REACH THE HOST'S "
                    "CHARACTER | holding it moved the host's body %.2f units "
                    "versus not holding it, in the child's own world. Above "
                    "1.0 means a button family is still routing to slot 0."
                    % (host_delta / 4096.0))
    return ok


# ---------------------------------------------------------------------------
# THE MP4 STATE-SYNC CHECKPOINT RUNGS.
#
# Ordered by the gate: SY4 and SY0 run BEFORE any sync code sends anything.
# That order is the point. SY4 proves the layer costs nothing when off, which is
# what makes "the DS path is the shipped default" a measured claim rather than a
# stated intention; SY0 proves the refusal path works, so a transport that
# cannot carry sync degrades to the DS path loudly instead of half-enabling.
# ---------------------------------------------------------------------------
SYNCLINE = re.compile(r"^\[sync\] (.*)$", re.M)


def rungSY4(seconds):
    """SYNC OFF IS UNCHANGED -- the regression gate for the whole layer.

    A new TU is in the link and a new call is in the frame loop. With
    SM64DS_SYNC unset, neither may cost anything: the solo selftest's POSITION
    must be identical to the pre-MP4 baseline, which is the same signal rung 1
    has always used and the only one that is layout-independent.

    Asserted here rather than trusted to rung 1 because rung 1 predates the
    layer: this rung names MP4 as the thing on trial.
    """
    d = os.path.join(OUT, "sy4_off")
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, d, "sy4")
    env["SM64DS_WINDOW_SELFTEST"] = "300"
    log = os.path.join(d, "run.log")
    rc = M.run_one(os.path.join(ROOT, "build", "port", "walk_window.exe"),
                   d, env, log)
    t = M.text(log)
    pos = re.search(r"^selftest: \d+ frames, pos=\(([^)]*)\)", t, re.M)
    got = pos.group(1) if pos else "MISSING"
    want = M.PRECHANGE["300"]["pos"]
    ok = M.verdict(rc == 0 and got == want,
                   "rungSY4 SYNC OFF IS BYTE-IDENTICAL TO THE BASELINE | "
                   "pos=(%s), expected (%s). The sync TU is linked and its "
                   "decide() runs every frame; with the knob unset neither may "
                   "move the game." % (got, want))
    quiet = "[sync]" not in t
    ok &= M.verdict(quiet,
                    "rungSY4 and the layer is SILENT when off | no [sync] line "
                    "in a 300-frame solo run (a default-off layer that "
                    "announces itself is still a behaviour change)")
    return ok


def rungSY0(seconds):
    """A v1 TRANSPORT IS REFUSED, LOUDLY, AND THE SESSION STILL WORKS.

    The contract's v2 entries are optional, so the SEAM installs a v1 transport
    happily -- this layer is the only place the difference is noticed. The
    failure mode being guarded against is a half-enabled sync: a session where
    remote bodies mysteriously never correct and nothing says why.

    SM64DS_SYNC_FORCE_V1 makes the carrier present as v1. It is test
    scaffolding and hal/comms_sync.cpp says so at the knob: the only transport
    in this tree is v2, and the alternative to a knob is shipping a second
    crippled carrier, which is more code and less honest.

    THE SECOND HALF IS THE IMPORTANT HALF: refused must mean DEGRADED, not
    broken. The two instances must still join and run the DS lockstep.
    """
    t1, t2, _ = play_session("sy0_refuse", seconds,
                             extra_env={"SM64DS_SYNC": "1",
                                        "SM64DS_SYNC_FORCE_V1": "1"})
    ok = True
    ok &= isolated("rungSY0", t1)
    refusal = [m.group(1) for m in SYNCLINE.finditer(t1) if "REFUSED" in m.group(1)]
    ok &= M.verdict(bool(refusal),
                    "rungSY0 THE v1 TRANSPORT WAS REFUSED WITH A REASON | %s"
                    % (refusal[0][:150] if refusal else "NO [sync] REFUSED LINE"))
    ok &= M.verdict(len(refusal) <= 1,
                    "rungSY0 and said so ONCE, not every frame | %d refusal "
                    "lines in the whole session" % len(refusal))
    # And the session is still a session.
    l1, l2 = LINK.search(t1), LINK.search(t2)
    ok &= M.verdict(bool(l1) and bool(l2),
                    "rungSY0 REFUSED MEANS DEGRADED, NOT BROKEN | both "
                    "instances still joined and ran the DS lockstep")
    a1, b1 = rows(t1, 0), rows(t1, 1)
    ok &= M.verdict(bool(a1) and bool(b1),
                    "rungSY0 and both bodies are still in the host's world | "
                    "slot0 rows=%d slot1 rows=%d" % (len(a1), len(b1)))
    return ok


SYNCSTAT = re.compile(
    r"^\[sync:level\] enabled=(\w+) sent=(\d+) recvd=(\d+) dropped=(\d+) "
    r"applied=(\d+) lerps=(\d+) snaps=(\d+) worst_err=(-?\d+)", re.M)
ROUND = re.compile(r"^\[loopback:level\].*?round=(\d+)", re.M)


def sync_stats(t):
    """The LAST sync readout in a log, as a dict, or None."""
    last = None
    for m in SYNCSTAT.finditer(t):
        last = m
    if not last:
        return None
    return dict(enabled=last.group(1) == "yes", sent=int(last.group(2)),
                recvd=int(last.group(3)), dropped=int(last.group(4)),
                applied=int(last.group(5)), lerps=int(last.group(6)),
                snaps=int(last.group(7)), worst=int(last.group(8)))


SYNC_ON = {"SM64DS_SYNC": "1", "SM64DS_SYNC_REPORT": "1"}


def rungSY1(seconds):
    """THE LAYER RUNS END TO END. PLUMBING ONLY -- it does not test correction.

    Sync on, both instances: under per-body owner authority (mp-sync-coopdx
    item 1) BOTH consoles must send their own body and BOTH must receive and
    apply the peer's -- the wire corrects in both directions now. A run where
    the layer is enabled and either direction applied nothing is not a pass,
    it is half a layer, which is exactly the defect item 1 closed: before it,
    the host's counters read recvd=0 applied=0 in every session and remote
    bodies on the host's screen were never corrected at all. What must still
    be zero is each console's correction of its OWN body, which rungSY2
    asserts trajectory-for-trajectory.

    WHAT THIS RUNG DOES NOT ASSERT, said plainly because an earlier version of
    this docstring claimed it: that corrections are LERPs rather than snaps. It
    cannot, because over loopback neither ever happens -- worst error is about
    1.26 units against a 2.0 ignore threshold, so lerps=0 in every run and the
    2..60-unit band has never executed. Asserting lerps > 0 here would fail on a
    correct build. The band needs induced LATENCY to reach, and that tool now
    exists: SM64DS_SYNC_DELAY_MS (the mp-sync-coopdx item 6 rig; see the banner
    in hal/comms_sync.cpp). This rung deliberately keeps running WITHOUT it, so
    what it proves stays what it proved.
    """
    t1, t2, _ = play_session("sy1_converge", seconds,
                             inj_c="key=0x20", extra_env=SYNC_ON)
    ok = isolated("rungSY1", t1)
    st1, st2 = sync_stats(t1), sync_stats(t2)
    ok &= M.verdict(bool(st1) and bool(st2) and st1["enabled"] and st2["enabled"],
                    "rungSY1 the layer is ENABLED on both instances | host=%s "
                    "child=%s" % (st1, st2))
    if not (st1 and st2):
        return False
    ok &= M.verdict(st1["sent"] > 0 and st2["sent"] > 0,
                    "rungSY1 BOTH CONSOLES SENT THEIR OWN BODY | host=%d "
                    "child=%d messages" % (st1["sent"], st2["sent"]))
    ok &= M.verdict(st2["recvd"] > 0 and st2["applied"] > 0,
                    "rungSY1 the child applied the host's body | "
                    "recvd=%d applied=%d" % (st2["recvd"], st2["applied"]))
    ok &= M.verdict(st1["recvd"] > 0 and st1["applied"] > 0,
                    "rungSY1 AND THE HOST APPLIED THE CHILD'S BODY | "
                    "recvd=%d applied=%d -- this direction measured 0/0 for "
                    "the whole life of the host-authority design"
                    % (st1["recvd"], st1["applied"]))
    return ok


def rungSY2(seconds):
    """THE LOCAL BODY IS NEVER CORRECTED.

    The single most important property of the layer, asserted twice, and the
    SHAPE of the assertion changed with per-body authority (mp-sync-coopdx
    item 1) for a reason worth the docstring it costs.

    The old shape -- the walker's own trajectory byte-identical between a
    sync-on and a sync-off run -- is NOT a property this system has any more,
    and measuring it red is not a local write. Under owner authority both
    directions correct, a correction drags a remote COPY toward a stale point
    (~30-100 ms old), the perturbed copy touches that console's sim (camera,
    proximity, collision pushback), and the LOCKSTEP republishes the
    perturbed console's record every frame -- so the perturbation rides the
    wire back into the walker's own sim. Measured while item 1 landed: both-
    walking arms diverged ~18-29 units; even one-walker arms kept ~0.4-2.3
    units of wire-feedback residue. That is corrections doing their job.

    Even zero motion does not rescue trajectory equality: the two bodies
    SPAWN OVERLAPPING and the collision pushback that separates them runs
    during the same frames the first corrections land on the remote copy, so
    a standing on/off pair measured 2.49 units apart from frame 34 forever,
    with zero input and zero local writes. The probe was measuring the sim.

    What IS asserted, and it is mechanism-level rather than end-to-end:

      * ZERO LOCAL WRITES: apply_snapshot reads the local body's pos/yaw
        before its entry loop and compares after (single-threaded frame loop,
        so any change across that window is the layer writing the local
        player). local_writes must be 0 on both consoles, under live
        corrections, in both arms.
      * ZERO OWN-CLAIMS: both consoles must report own_claims=0 -- no
        received entry EVER named the local slot. Under owner authority
        nobody publishes another console's body, so the never-correct-local
        skip must go the whole session with nothing to refuse. Not only did
        the layer not correct the local body, nothing on the wire asked it to.
    """
    # Arm A: both stand (the spawn-pushback window still runs corrections).
    t1a, t2a, _ = play_session("sy2_still_on", seconds, extra_env=SYNC_ON)
    # Arm B: both walk (apart -- host left, child right): live corrections
    # on both screens for the whole session.
    t1c, t2c, _ = play_session("sy2_walk_on", seconds, inj_p="key=0x20",
                               inj_c="key=0x10", extra_env=SYNC_ON)
    ok = isolated("rungSY2(still)", t1a)
    ok &= isolated("rungSY2(walk)", t1c)

    OWN = re.compile(r"own_claims=(\d+)")
    LOCW = re.compile(r"local_writes=(\d+)")
    APPL = re.compile(r"applied=(\d+)")
    for arm, tag, t in (("still", "host", t1a), ("still", "child", t2a),
                        ("walk", "host", t1c), ("walk", "child", t2c)):
        applied = [int(x) for x in APPL.findall(t)]
        ok &= M.verdict(bool(applied) and max(applied) > 0,
                        "rungSY2(%s) the %s applied corrections, so the "
                        "assertions below are exercised | applied=%s"
                        % (arm, tag, max(applied) if applied else -1))
        claims = [int(x) for x in OWN.findall(t)]
        worstc = max(claims) if claims else -1
        ok &= M.verdict(worstc == 0,
                        "rungSY2(%s) NOTHING ON THE WIRE CLAIMED THE %s'S OWN "
                        "BODY | own_claims=%s (nonzero means a peer published "
                        "a body it does not own; -1 means no report line)"
                        % (arm, tag.upper(), worstc))
        writes = [int(x) for x in LOCW.findall(t)]
        worstw = max(writes) if writes else -1
        ok &= M.verdict(worstw == 0,
                        "rungSY2(%s) AND THE LAYER NEVER WROTE THE %s'S OWN "
                        "BODY | local_writes=%s measured across every "
                        "apply_snapshot call (-1 means no report line)"
                        % (arm, tag.upper(), worstw))
    return ok


def rungSY3(seconds):
    """NO SNAPS IN A HEALTHY SESSION.

    A snap is a bug report, not a feature: it means a remote body was so far
    from authority that interpolating would have looked worse than teleporting.
    Over loopback with no induced loss there is no honest reason for one.
    """
    t1, t2, _ = play_session("sy3_snaps", seconds, inj_c="key=0x20",
                             extra_env=SYNC_ON)
    ok = isolated("rungSY3", t1)
    st = sync_stats(t2)
    ok &= M.verdict(bool(st), "rungSY3 the child reported sync counters")
    if not st:
        return False
    ok &= M.verdict(st["snaps"] == 0,
                    "rungSY3 ZERO SNAPS across the session | snaps=%d "
                    "lerps=%d worst error %.1f units"
                    % (st["snaps"], st["lerps"], st["worst"] / 4096.0))
    return ok


def rungSY5(seconds):
    """IT SURVIVES PACKET LOSS, because the channel is specified unreliable.

    20% of aux messages dropped on the receive side. The world must still
    converge and must still not snap: a lost message is a slightly staler remote
    body and nothing else. "It works on loopback" proves nothing about the
    internet, where loss is the normal condition.
    """
    env = dict(SYNC_ON)
    env["SM64DS_SYNC_DROP"] = "20"
    t1, t2, _ = play_session("sy5_loss", seconds, inj_c="key=0x20",
                             extra_env=env)
    ok = isolated("rungSY5", t1)
    st = sync_stats(t2)
    ok &= M.verdict(bool(st), "rungSY5 the child reported sync counters")
    if not st:
        return False
    ok &= M.verdict(st["dropped"] > 0,
                    "rungSY5 loss was actually induced | dropped=%d of %d "
                    "received" % (st["dropped"], st["recvd"] + st["dropped"]))
    ok &= M.verdict(st["applied"] > 0 and st["snaps"] == 0,
                    "rungSY5 AND THE WORLD STILL CONVERGED | applied=%d "
                    "snaps=%d worst error %.1f units -- a dropped message is a "
                    "staler body, not a broken one"
                    % (st["applied"], st["snaps"], st["worst"] / 4096.0))
    return ok


def rungSY6(seconds):
    """AUX TRAFFIC DOES NOT TAX THE LOCKSTEP -- the one-socket ruling's cost check.

    Ordered at the MP4 gate as the consequence to enforce after choosing one
    socket. The input record is sent FIRST every pump and aux after it, and this
    measures whether that discipline held: the number of lockstep ROUNDS
    completed in a fixed wall-clock session must be the same with sync on as
    with sync off, within noise.

    Rounds-per-session is the right proxy and better than timing one send: the
    lockstep is what the game blocks on, so if aux ever delayed an input
    datagram the round count is exactly what would fall.
    """
    t1a, _, _ = play_session("sy6_on", seconds, extra_env=SYNC_ON)
    t1b, _, _ = play_session("sy6_off", seconds)
    ok = isolated("rungSY6(on)", t1a)
    ok &= isolated("rungSY6(off)", t1b)

    def last_round(t):
        r = ROUND.findall(t)
        return int(r[-1]) if r else 0

    on, off = last_round(t1a), last_round(t1b)
    ok &= M.verdict(on > 0 and off > 0,
                    "rungSY6 both arms completed rounds | on=%d off=%d" % (on, off))
    if not (on and off):
        return False
    # 10% is the noise floor for a wall-clock-bounded session on a busy desktop.
    delta = abs(on - off) * 100.0 / max(on, off)
    ok &= M.verdict(delta <= 10.0,
                    "rungSY6 THE LOCKSTEP RUNS AT THE SAME RATE WITH SYNC ON | "
                    "%d rounds on vs %d off in %ds, %.1f%% apart (aux shares "
                    "the socket and must never delay the thing the game blocks "
                    "on)" % (on, off, seconds, delta))
    return ok


def rungP6(seconds):
    """BUTTONS CROSS THE WIRE AND FIRE ACTIONS -- the coverage hole his hands found.

    THE EXISTING RUNGS PROVED STICKS AND NEVER BUTTONS, and that is exactly why
    three button seams survived three green ladders. rungP3 asserts the host's
    movement reaches the child, and movement rides the STICK fields, which
    nothing was clobbering. rungP5 asserted a button did not move the host's
    BODY -- and a punch barely moves a body, so it passed while the bug was
    live. Position was the wrong observable for a button.

    THE RIGHT ONE IS STATE. A crouch, a backflip and a punch are all STATE
    CHANGES, and hal/player_fields.h::state_id reads the DS address of the state
    the player is currently running -- the same word Player::Behavior's own
    dispatcher switches on, so this asserts on the thing the game branches on
    rather than on an invented enum.

    TWO DIRECTIONS, because the two failures were different:
      the host presses -> the HOST's body changes state IN THE CHILD's world
      the child presses -> the CHILD's body changes state in the HOST's world,
                           and the host's body does NOT
    """
    # THE INJECTED VALUE IS A RAW PAD BIT, NOT A Ctrl BIT -- the injection
    # enters at the key register, and Stage::CheckInput's remap (Nintendo's
    # mode-0 map at data_02075650, hosted since run mg16 lane MPBTN) sits
    # between the register and the word the state machine reads. The Ctrl bit
    # the states branch on is 0x400 (St_Crawl_Main:45, St_Shell_Main:103 test
    # data_0209f49c & 0x400) and the RAW bit the map turns into it is R =
    # 0x100. An earlier revision injected 0x400 raw -- that is X, which the map
    # sends to Ctrl 0x8000, a bit no state reads -- an identity-map assumption
    # from the era when the maps were zeroed and NOTHING crossed. Held, so the
    # state has time to change and the assertion is not racing a one-frame
    # edge.
    t1a, t2a, _ = play_session("p6_host_presses", seconds, inj_p="key=0x100")
    t1b, t2b, _ = play_session("p6_child_presses", seconds, inj_c="key=0x100")
    ok = isolated("rungP6(host)", t1a)
    ok &= isolated("rungP6(child)", t1b)

    def states(t, slot):
        return set(r["state"] for r in rows(t, slot) if r["state"])

    # --- host presses: the child's copy of the HOST must react ---------------
    host_in_child = states(t2a, 0)
    host_in_host = states(t1a, 0)
    ok &= M.verdict(len(host_in_child) > 1,
                    "rungP6 THE HOST'S BUTTON FIRED AN ACTION IN THE CHILD'S "
                    "WORLD | the host's body ran %d distinct states in the "
                    "child's window (one state for the whole session means the "
                    "buttons never crossed)" % len(host_in_child))
    ok &= M.verdict(bool(host_in_child & host_in_host),
                    "rungP6 and they are the SAME states the host ran | %d "
                    "shared of %d/%d" % (len(host_in_child & host_in_host),
                                         len(host_in_child), len(host_in_host)))

    # --- child presses: the host's body must NOT react ------------------------
    #
    # DIFFERENTIAL, and the first version of this half was not -- it asserted
    # the host's body ran at most ONE state while only the child pressed, and
    # went red on a correct build showing three. A body with no input does not
    # sit in one state: it falls, it lands, it drops into wait. "No input" and
    # "no state changes" are different claims and only the second one is wrong.
    #
    # THAT IS THE THIRD TIME THIS LANE HAS MADE THE SAME MISTAKE (rung 9's first
    # two versions, rungP2's first version, this). The shape is always the same:
    # asserting an absolute where the world has its own motion. The control arm
    # is what separates "the game did this" from "the input did this", and it is
    # cheap enough that there is no excuse for skipping it.
    own_in_child = states(t2b, 1)
    ok &= M.verdict(len(own_in_child) > 1,
                    "rungP6 THE CHILD'S BUTTON FIRED ITS OWN ACTION | %d "
                    "distinct states on the child's own body" % len(own_in_child))

    t1c, t2c, _ = play_session("p6_nobody_presses", seconds)
    ok &= isolated("rungP6(control)", t1c)
    pressed = states(t2b, 0)      # host's body, child pressing
    control = states(t2c, 0)      # host's body, nobody pressing
    extra = pressed - control
    ok &= M.verdict(not extra,
                    "rungP6 AND IT DID NOT FIRE THE HOST'S | the host's body "
                    "ran %d states in the child's world with the child "
                    "pressing and %d with nobody pressing, and the pressed set "
                    "adds NOTHING (%s). A state the control does not have is "
                    "the punch-bleed: one pad driving two characters."
                    % (len(pressed), len(control),
                       ",".join("%08x" % x for x in sorted(extra)) or "none"))
    return ok


def rungP7(seconds):
    """SNAPS RETURN TO ZERO ONCE BUTTONS ROUTE -- the field counter, as a rung.

    Ordered after the owner's first sync session, which reported lerps=103 and
    SNAPS=123 with a worst error of about 174 units. Those snaps were not a
    sync defect: every un-delivered crouch and backflip ran the child's copy of
    Mario into a different life, the two worlds genuinely parted, and the layer
    did exactly what it should -- yanked the remote body back. THE BUTTON FIX IS
    THEREFORE THE SNAP FIX, and this rung is what stops that regressing.

    It is SY3's healthy-session invariant extended to the case SY3 could not
    reach: SY3 runs with no button traffic at all, so it proves zero snaps in a
    session where nothing could diverge. This one exercises buttons on both
    sides -- the pattern that produced 123 snaps in the field -- and requires
    zero anyway.
    """
    env = dict(SYNC_ON)
    # raw R (0x100) -> Ctrl 0x400 through the mode-0 remap: the crouch family,
    # the exact pattern that produced the field's 123 snaps. See rungP6's
    # comment for why the raw bit and the Ctrl bit differ.
    t1, t2, _ = play_session("p7_snapfree", seconds, inj_p="key=0x100",
                             inj_c="key=0x100", extra_env=env)
    ok = isolated("rungP7", t1)
    st = sync_stats(t2)
    ok &= M.verdict(bool(st), "rungP7 the child reported sync counters")
    if not st:
        return False
    ok &= M.verdict(st["applied"] > 0,
                    "rungP7 the layer was actually running | applied=%d"
                    % st["applied"])
    ok &= M.verdict(st["snaps"] == 0,
                    "rungP7 ZERO SNAPS WITH BUTTONS EXERCISED ON BOTH SIDES | "
                    "snaps=%d lerps=%d worst error %.1f units. The owner's "
                    "pre-fix session showed 123 snaps at ~174 units on this "
                    "same pattern; a snap here means a button is diverging the "
                    "worlds again." % (st["snaps"], st["lerps"],
                                       st["worst"] / 4096.0))
    return ok


def rungP8(seconds):
    """THE ANALOG STICK PATH ROUTES TO ITS OWN SLOT -- the fourth writer's rung.

    THIS BLOCK WAS INVISIBLE TO EVERY PREVIOUS RUNG BY CONSTRUCTION. The
    RUN_ANALOG stick publish is guarded on pad_live, which is XInputGetState
    succeeding, and no harness has a physical gamepad -- so every green ladder
    in this campaign ran with it switched off. It writes the local stick into
    Ctrl slot 0 after the fan, the same shape as the three button/pad writers
    already gated, and on a child it would drive the HOST's character.

    IT IS REACHABLE WITHOUT HARDWARE, and that is worth more than a comment
    naming the hole. SM64DS_PAD_TEST fakes a pad and sets pad_live=1
    (walk_window.cpp:3442) and is deliberately live in PLAY mode; the run-mode
    half needed a knob, so SM64DS_FORCE_ANALOG forces it -- the same class of
    test scaffolding as SM64DS_SYNC_FORCE_V1 and SM64DS_SYNC_DROP.

    Asserted differentially, like every input rung here: with the analog path
    forced on both instances and the CHILD holding a stick direction, the host's
    body in the child's world must end where it ends when nobody holds anything.
    """
    env = {"SM64DS_FORCE_ANALOG": "1", "SM64DS_PAD_TEST": "0x1@30"}
    t1a, t2a, _ = play_session("p8_analog_child", seconds,
                               inj_c="key=0x20", extra_env=env)
    t1b, t2b, _ = play_session("p8_analog_idle", seconds,
                               inj_c="key=0x0", extra_env=env)
    ok = isolated("rungP8(pressed)", t1a)
    ok &= isolated("rungP8(idle)", t1b)
    host_a, host_b = rows(t2a, 0), rows(t2b, 0)
    own_a = rows(t2a, 1)
    ok &= M.verdict(bool(host_a) and bool(host_b) and bool(own_a),
                    "rungP8 both bodies observed in both arms")
    if not (host_a and host_b and own_a):
        return False
    mine = span(own_a, "x") + span(own_a, "z")
    host_delta = (abs(host_a[-1]["x"] - host_b[-1]["x"]) +
                  abs(host_a[-1]["z"] - host_b[-1]["z"]))
    ok &= M.verdict(host_delta < max(4096, mine // 4),
                    "rungP8 THE ANALOG PATH DID NOT DRIVE THE HOST'S BODY | "
                    "with the analog stick publish forced live, holding a "
                    "direction on the child moved the host's character %.2f "
                    "units versus not holding it, against %.1f units of the "
                    "child's own travel."
                    % (host_delta / 4096.0, mine / 4096.0))
    return ok


RUNGS = [("P0", rungP0), ("P1", rungP1), ("P2", rungP2), ("P3", rungP3),
         ("P4", rungP4), ("P5", rungP5), ("SY4", rungSY4), ("SY0", rungSY0), ("SY1", rungSY1),
         ("SY2", rungSY2), ("SY3", rungSY3), ("SY5", rungSY5),
         ("SY6", rungSY6), ("P6", rungP6), ("P7", rungP7), ("P8", rungP8)]


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
