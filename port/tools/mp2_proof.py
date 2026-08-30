#!/usr/bin/env python3
"""THE MP2 PROOF LADDER, one command. Run mg16, lane MP2.

Five rungs, in order, each one a verdict line. Nothing here launches a visible
window: every child is spawned through the same quiet plumbing
port/tools/battery.py uses, and the reason is a standing owner order after a
test window appeared over a live game.

  1. SOLO REGRESSION   knob off, the level-1 selftest is unchanged against a
                       stored baseline; knob on and solo, the MP1 report line
                       still says transport=solo.
  2. JOIN              two headless instances, parent and child, both rc 0,
                       connected on both sides, players=2, roles right, and the
                       exchange counter advancing on BOTH.
  3. DATA CROSSING     a scripted stylus/pad value injected on one side shows up
                       in the OTHER side's slot report, both directions.
  4. VS SCENE          both instances direct-booted into the scene the fan-out
                       call site serves, a full scripted run, FAULTS_FATAL=1 and
                       no UNHANDLED fault.
  5. TWO-WINDOW SCRIPT the milestone script's mechanics, spawned MINIMIZED.
                       THE VISIBLE COLD START IS NOT RUN HERE. It belongs to the
                       coordinator with the owner present.

    python port/tools/mp2_proof.py [repo-root] [--only N] [--out DIR]

Exit 0 all green, 1 at the first red.

WHY THE INSTANCES ARE SEPARATED THREE WAYS. Two copies of this game on one
machine collide on three different kinds of path, and each gets its own answer
(the long version is in port/hal/instance_tag.h):
  working directory  -- separates playlog/ and the selftest BMP
  TEMP               -- separates the %TEMP%\\sm64ds-crashes dir, which every
                        boot PRUNES TO FOUR, so a shared one has instance two
                        deleting instance one's dumps
  SM64DS_INSTANCE    -- separates the files that live NEXT TO THE EXE, which no
                        cwd and no TEMP can separate. Exactly which:
                        startup_error.txt and savestate.bin are suffixed, and so
                        is settings.json's sibling temp. crash.txt and exit.txt
                        ARE NOT -- editing fault_probe.h cost the port its fixed
                        address ranges at startup, so those two stay shared and
                        the per-instance TEMP makes the RICH crash dumps the
                        ones to read. Survey and bisect: port/hal/instance_tag.h
"""

import os
import re
import subprocess
import sys
import time
import glob
import hashlib

# ---------------------------------------------------------------------------
# THE QUIET LAUNCH. Copied in shape from port/tools/battery.py's NO_CONSOLE /
# SI_MIN pair, and it is not optional here. CREATE_NO_WINDOW silences the
# console; SW_SHOWMINNOACTIVE starts the game window minimized and unactivated;
# SM64DS_NO_FOCUS=1 (set in env_base below) is walk_window's own half, the
# WS_EX_NOACTIVATE + SW_SHOWNOACTIVATE pair. All three, every launch.
# ---------------------------------------------------------------------------
NO_CONSOLE = getattr(subprocess, "CREATE_NO_WINDOW", 0)
if hasattr(subprocess, "STARTUPINFO"):
    SI_MIN = subprocess.STARTUPINFO()
    SI_MIN.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI_MIN.wShowWindow = 7          # SW_SHOWMINNOACTIVE
else:
    SI_MIN = None

SELFTEST_FRAMES = "300"
# The MP1 review's count. 300 is a blind count for this layout class; the
# docstring that justifies a BMP comparison was citing a 296-frame measurement,
# so rung 1 compares BOTH and treats a disagreement at either as a red.
LAYOUT_FRAMES = "296"

# A PORT BASE OF THIS RUN'S OWN, NOT THE SHIPPED DEFAULT.
#
# Run mg16 lane MP3. kCommsLoopbackPortBase is 51765 and every harness in this
# tree used it, which means two harnesses -- or a harness and the owner playing
# a two-window session -- bind the SAME loopback ports at the same time. Their
# instances then find each other on the wire and form one session out of two
# unrelated runs: a proof measures somebody else's game, and tearing a proof
# down pulls the parent out from under a session a human is playing.
#
# Derived from this process's PID and kept well clear of 51765, so a proof never
# lands on the shipped default a human's session is using.
#
# THE STRIDE HAS TO EXCEED THE FOOTPRINT, and the first version's did not. A
# harness run reaches PORT_BASE + 48 and each two-window session binds its port
# and port+1, so one run occupies roughly a hundred ports; a stride of 16 meant
# ADJACENT buckets overlapped and two runs a few pids apart still collided. 128
# clears the footprint with room to spare.
#
# WHAT THIS DOES AND DOES NOT GUARANTEE, stated to the arithmetic rather than
# hopefully: 70 buckets of 128 span 56000..64960, inside the port range. Runs in
# DIFFERENT buckets can no longer overlap at all. Two runs whose pids are
# congruent mod 70 still land on the same base -- roughly a 1-in-70 chance
# rather than the near-certainty a 16-wide stride gave. rungP0 is what catches
# that case if it ever happens, which is why isolation is asserted and not
# assumed.
PORT_BASE = 56000 + (os.getpid() % 70) * 128


def sha(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        h.update(f.read())
    return h.hexdigest()[:16].upper()


def env_base(root, run_dir, instance):
    e = dict(os.environ)
    # Never inherit a lane's knobs into a proof run: the same discipline
    # battery.py keeps, and for the same reason.
    for k in list(e):
        if k.startswith("SM64DS_"):
            e.pop(k)
    e["SM64DS_ASSET_ROOT"] = root
    e["SM64DS_FAULTS_FATAL"] = "1"
    e["SM64DS_NO_DIALOG"] = "1"
    e["SM64DS_NO_FOCUS"] = "1"          # the game-window half of the quiet rule
    # run mg16 lane MP3: THE THIRD HALF OF THE QUIET RULE, which was missing.
    # The owner's standing order is nothing visible, nothing focused and NOTHING
    # AUDIBLE on a test launch. This file had the first two (CREATE_NO_WINDOW +
    # SW_SHOWMINNOACTIVE above, SM64DS_NO_FOCUS here) and never silenced the
    # audio, so every rung that reached a level was free to make noise on his
    # machine. Set here rather than per-rung so no future rung can forget it,
    # and set AFTER the SM64DS_ scrub above so an inherited value cannot win.
    e["SM64DS_VOLUME"] = "0"
    # BELT AND BRACES ON THE MINIMIZE. SI_MIN above already asks for
    # SW_SHOWMINNOACTIVE through STARTUPINFO and that is the authoritative
    # request here -- Python can make it and does. This env says the same thing
    # a second way, and it is set because the failure it guards against has now
    # happened twice in this tree: a cmd.exe shim ate the STARTUPINFO once, and
    # .NET dropped WindowStyle on the floor once. Both times the window came up
    # visible on the owner's desk and every log still said the run was quiet.
    # A guarantee that depends on one mechanism is one mechanism away from
    # being a comment.
    e["SM64DS_MINIMIZED"] = "1"
    # THE LEVEL THIS FILE HAS ALWAYS MEASURED, NOW SAID OUT LOUD (lane
    # boot-title). Every rung here reads level-path output -- the
    # "selftest: N frames, pos=(x, y, z)" line, the [comms:level] transport
    # report, the [loopback:level] and [sync:level] counters -- and it got that
    # level by NOT naming one, back when a bare launch defaulted to castle
    # grounds. The owner's ruling moved that default to the title screen, so a
    # bare launch is now a title run with no Player, no comms level report and
    # no position line, and every rung below would fail on a change that has
    # nothing to do with what they measure.
    #
    # THIS IS THE SAME RUN, NAMED. Level 1 IS castle grounds -- it is the
    # value hal/level_boot.cpp's port_level_env_want defaulted to and the one
    # PRECHANGE's positions and BMP hashes were recorded against -- so the
    # constants at the top of this file stand unchanged and are still being
    # compared against the identical boot. Nothing is relaxed: the assertions,
    # the hashes and the exit codes are untouched.
    #
    # The sibling harnesses already spell it this way and always have:
    # mp3_proof.py sets SM64DS_LEVEL on all three of its env builders, and
    # mp2_two_windows.ps1 sets it with a comment calling the direct boot
    # deliberate. This file was the one that relied on the default.
    e["SM64DS_LEVEL"] = "1"
    e["SM64DS_INSTANCE"] = instance
    e["TEMP"] = os.path.join(run_dir, "tmp")
    e["TMP"] = e["TEMP"]
    os.makedirs(e["TEMP"], exist_ok=True)
    return e


def spawn(exe, cwd, env, logpath):
    log = open(logpath, "wb")
    # BOTH streams into one log. The selftest's verdict line -- "selftest: N
    # frames, pos=(x, y, z)" -- goes to STDOUT while every [comms:*] and
    # [layout] line goes to stderr, and rung 1 needs both.
    p = subprocess.Popen([exe], cwd=cwd, env=env,
                         stdout=log, stderr=subprocess.STDOUT,
                         creationflags=NO_CONSOLE, startupinfo=SI_MIN)
    p._logfile = log
    return p


def finish(p, timeout):
    try:
        rc = p.wait(timeout=timeout)
    except subprocess.TimeoutExpired:
        p.kill()
        rc = -9
    p._logfile.close()
    return rc


def run_one(exe, cwd, env, logpath, timeout=600):
    return finish(spawn(exe, cwd, env, logpath), timeout)


def text(path):
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read()


VERDICTS = []


def verdict(ok, line):
    VERDICTS.append(("PASS" if ok else "FAIL") + "  " + line)
    print(VERDICTS[-1], flush=True)
    return ok


# ---------------------------------------------------------------------------
# RUNG 1 -- SOLO REGRESSION
# ---------------------------------------------------------------------------
# The pre-change measurement, taken in this same build directory before the
# transport existed or was in the link. runs/mg16/out/MP2/r1_baseline_prechange.txt
# carries the same numbers and the rule for reading a diff.
PRECHANGE = {
    "296": dict(pos="-4915200, 2949510, 11429316",
                layout="dsstate=00C9B000..00D86534",
                bmp="28B27CBCC0E5261F"),
    "300": dict(pos="-4915200, 2929633, 11141348",
                layout="dsstate=00C9B000..00D86534",
                bmp="9129CE6CF3A229D4"),
}


def rung1(root, exe, out):
    ok = True
    base = {}
    for frames in (LAYOUT_FRAMES, SELFTEST_FRAMES):
        d = os.path.join(out, "r1_solo_%s" % frames)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        env = env_base(root, d, "r1")
        env["SM64DS_WINDOW_SELFTEST"] = frames
        log = os.path.join(d, "run.log")
        rc = run_one(exe, d, env, log)
        t = text(log)
        bmp = os.path.join(d, "walk_window_selftest.bmp")
        pos = re.search(r"^selftest: \d+ frames, pos=\(([^)]*)\)", t, re.M)
        lay = re.search(r"^\[layout\] (dsstate=\S+)", t, re.M)
        base[frames] = dict(
            rc=rc,
            pos=pos.group(1) if pos else "MISSING",
            layout=lay.group(1) if lay else "MISSING",
            bmp=sha(bmp) if os.path.exists(bmp) else "MISSING")
        want = PRECHANGE[frames]
        same_pos = base[frames]["pos"] == want["pos"]
        same_lay = base[frames]["layout"] == want["layout"]
        same_bmp = base[frames]["bmp"] == want["bmp"]
        # POSITION IS THE REGRESSION SIGNAL. The BMP is only evidence when the
        # layout matched: battery.py's doctrine is that the rendered frame
        # depends on the ABSOLUTE ADDRESS of hosted globals, so a moved
        # .dsstate base changes the picture without anything having regressed.
        # A moved layout is therefore reported, not failed, and the verdict
        # falls back to position -- which is game state and moves for no
        # layout reason.
        ok &= verdict(rc == 0 and same_pos and (same_bmp or not same_lay),
                      "rung1 solo knob-off %s frames rc=%d pos=(%s) %s | "
                      "pos %s, layout %s, bmp %s%s"
                      % (frames, rc, base[frames]["pos"], base[frames]["layout"],
                         "SAME" if same_pos else "CHANGED",
                         "SAME" if same_lay else "MOVED",
                         "SAME" if same_bmp else "DIFFERS",
                         "" if same_lay else
                         "  (layout moved, so the BMP is not comparable and the "
                         "verdict rests on pos)"))

    # knob ON, still solo: the MP1 report must be exactly what MP1 measured.
    d = os.path.join(out, "r1_fanout")
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = env_base(root, d, "r1f")
    env["SM64DS_WINDOW_SELFTEST"] = SELFTEST_FRAMES
    env["SM64DS_COMMS_FANOUT"] = "1"
    env["SM64DS_COMMS_REPORT"] = "1"
    log = os.path.join(d, "run.log")
    rc = run_one(exe, d, env, log)
    t = text(log)
    # The SUMMARY line specifically. A bare [comms:level] match would take the
    # last of the four per-slot rows the same tag prints every frame.
    line = ""
    for m in re.finditer(r"^\[comms:level\] transport=.*$", t, re.M):
        line = m.group(0)
    want = ("transport=solo (no transport)" in line and "players=1" in line
            and "role=0" in line and "exchanges=0" in line
            and "connected=no" in line)
    # And the carrier must be SILENT: no transport is installed, so neither of
    # MP2's two extra report lines may appear.
    quiet = "[loopback:" not in t and "[lockstep:" not in t
    ok &= verdict(rc == 0 and want and quiet,
                  "rung1 solo knob-on rc=%d quiet=%s | %s"
                  % (rc, quiet, line or "NO [comms:level] LINE"))
    with open(os.path.join(out, "r1_baseline.txt"), "w") as f:
        for k, v in base.items():
            f.write("%s frames rc=%d pos=(%s) %s bmp=%s\n"
                    % (k, v["rc"], v["pos"], v["layout"], v["bmp"]))
    return ok


# ---------------------------------------------------------------------------
# RUNGS 2/3/4 -- TWO INSTANCES
# ---------------------------------------------------------------------------
def two_instances(root, exe, out, name, frames, extra_p=None, extra_c=None,
                  port=PORT_BASE, stagger=0.4):
    """Launch parent and child TOGETHER, minimized, and return both logs.

    `stagger` is how long to wait after starting the parent before starting the
    child. NEGATIVE means START THE CHILD FIRST by that many seconds, which is
    the ordering rung 7 uses: on a real desktop the launch order is not the
    order the two processes reach their frame loops, because boot time varies by
    seconds with asset loading.
    """
    dp = os.path.join(out, name + "_p1")
    dc = os.path.join(out, name + "_p2")
    for d in (dp, dc):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)

    ep = env_base(root, dp, "p1")
    ec = env_base(root, dc, "p2")
    for e, role in ((ep, "parent"), (ec, "child")):
        e["SM64DS_WINDOW_SELFTEST"] = frames
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(port)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
    for k, v in (extra_p or {}).items():
        ep[k] = v
    for k, v in (extra_c or {}).items():
        ec[k] = v

    lp = os.path.join(dp, "run.log")
    lc = os.path.join(dc, "run.log")
    # Parent first, as the DS does: the child needs a door to knock on. The gap
    # is small on purpose -- both boots take the same few seconds, so starting
    # them close keeps their frame loops overlapping.
    if stagger >= 0:
        pp = spawn(exe, dp, ep, lp)
        time.sleep(stagger)
        pc = spawn(exe, dc, ec, lc)
    else:
        pc = spawn(exe, dc, ec, lc)
        time.sleep(-stagger)
        pp = spawn(exe, dp, ep, lp)
    rp = finish(pp, 900)
    rc_ = finish(pc, 900)
    return rp, rc_, text(lp), text(lc)


def last(pattern, t):
    got = ""
    for m in re.finditer(pattern, t, re.M):
        got = m.group(0)
    return got


def field(line, key, cast=str):
    m = re.search(r"\b%s=([^\s]+)" % re.escape(key), line)
    return cast(m.group(1)) if m else None


def rung2(root, exe, out):
    rp, rc, tp, tc = two_instances(root, exe, out, "r2_join", SELFTEST_FRAMES)
    lp = last(r"^\[comms:level\] transport=.*$", tp)
    lc = last(r"^\[comms:level\] transport=.*$", tc)
    # run mg16 lane MP3: the [lockstep:level] line is GONE and this rung asserts
    # on the carrier's own report instead.
    #
    # MP2 printed lockstep ticks/rounds/timeouts/spins/peer_updates from inside
    # its transcription of src/func_0203ea5c.c. That TU is linked now and drives
    # itself, so those counters would have to be measured from inside a
    # byte-matched ROM body, which this repo does not edit. Rather than quietly
    # dropping an assertion, the rung moves to the INDEPENDENT witness that
    # still exists: the loopback carrier's own view of the session, which is
    # produced by different code from the seam readout above and therefore
    # actually corroborates it rather than restating it.
    kp = last(r"^\[loopback:level\] .*$", tp)
    kc = last(r"^\[loopback:level\] .*$", tc)

    ok = True
    ok &= verdict(rp == 0 and rc == 0,
                  "rung2 join exit codes parent=%d child=%d" % (rp, rc))

    # ASSERT ON THE CONNECTED WINDOW, NOT ON THE LAST LINE, and the reason is a
    # real behaviour rather than a convenience. Both instances run a FIXED frame
    # count, so whichever finishes first leaves while the other is still
    # ticking. The one left behind then spins its wait bound out and drops the
    # session to solo exactly the way src/func_0203ea5c.c:487 drops it --
    # role goes back to 0. That is the ROM's own shutdown behaviour working, so
    # the proof looks for a frame where the session was actually up.
    def joined(t, role, slot, link):
        for m in re.finditer(r"^\[comms:level\] transport=loopback.*$", t, re.M):
            l = m.group(0)
            if (field(l, "connected") == "yes" and field(l, "players") == "2"
                    and field(l, "role") == role and field(l, "slot") == slot
                    and field(l, "link") == link):
                return l
        return ""

    jp = joined(tp, "1", "0", "3")
    jc = joined(tc, "2", "1", "4")
    ok &= verdict(bool(jp), "rung2 join PARENT | " + (jp or "NO FULLY-JOINED "
                  "FRAME; last was: " + (lp or "no report line")))
    ok &= verdict(bool(jc), "rung2 join CHILD  | " + (jc or "NO FULLY-JOINED "
                  "FRAME; last was: " + (lc or "no report line")))
    ep = field(lp, "exchanges", int) or 0
    ec = field(lc, "exchanges", int) or 0
    rp_ = field(lp, "rounds", int) or 0
    rc2 = field(lc, "rounds", int) or 0
    ok &= verdict(ep > 1 and ec > 1 and rp_ > 1 and rc2 > 1,
                  "rung2 exchange counters advancing on BOTH: parent "
                  "exchanges=%d rounds=%d, child exchanges=%d rounds=%d"
                  % (ep, rp_, ec, rc2))
    # The carrier must agree with the seam: both sides live, both past round 0,
    # and each carrying the OTHER's traffic (a parent that never received is a
    # parent talking to itself).
    def carrier_ok(l):
        return (bool(l) and field(l, "live") not in ("", "0x0", "0x1")
                and (field(l, "round", int) or 0) > 1
                and (field(l, "recvd", int) or 0) > 1)
    ok &= verdict(carrier_ok(kp) and carrier_ok(kc),
                  "rung2 carrier corroborates the seam | parent %s ;; child %s"
                  % (kp or "NONE", kc or "NONE"))
    print("      final lines (after the first instance exited, so a drop to "
          "solo here is func_0203ea5c:487 working):")
    print("        P1 " + (lp or "none"))
    print("        P2 " + (lc or "none"))
    return ok


STYLUS = re.compile(r"^\[comms:level\]\s+slot(\d) frame=(-?\d+) "
                    r"stylus=\{(\d+),(\d+),(\d+)\} flag=([0-9a-f]+) "
                    r"key=([0-9a-f]+)", re.M)


def slot_rows(t, slot):
    """Every reported row for `slot`, newest last."""
    return [m for m in STYLUS.finditer(t) if int(m.group(1)) == slot]


def rung3(root, exe, out):
    """Inject a known value on one side; find it in the OTHER side's slot."""
    ok = True
    # CHILD -> PARENT. The child stuffs the ROM's own local-record fields the
    # way src/func_0203df40.c:31-39 does; the parent must see them in slot 1.
    ckey, cx, cy, ct = 0x0041, 0x5A, 0x2C, 1
    rp, rc, tp, tc = two_instances(
        root, exe, out, "r3_c2p", SELFTEST_FRAMES,
        extra_c={"SM64DS_COMMS_INJECT":
                 "key=0x%x,x=%d,y=%d,touch=%d" % (ckey, cx, cy, ct)})
    rows = slot_rows(tp, 1)
    hit = [m for m in rows
           if int(m.group(7), 16) == ckey and int(m.group(3)) == cx
           and int(m.group(4)) == cy and int(m.group(5)) == ct]
    ok &= verdict(rp == 0 and rc == 0 and len(hit) > 0,
                  "rung3 CHILD->PARENT injected key=0x%04x stylus={%d,%d,%d}: "
                  "parent slot1 carried it in %d of %d reported frames | %s"
                  % (ckey, cx, cy, ct, len(hit), len(rows),
                     hit[-1].group(0).strip() if hit else "NEVER SEEN"))

    # PARENT -> CHILD, the reverse, with a different value so a stale log
    # cannot pass for a fresh one.
    pkey, px, py, pt = 0x0802, 0x11, 0x77, 1
    rp, rc, tp, tc = two_instances(
        root, exe, out, "r3_p2c", SELFTEST_FRAMES,
        extra_p={"SM64DS_COMMS_INJECT":
                 "key=0x%x,x=%d,y=%d,touch=%d" % (pkey, px, py, pt)})
    rows = slot_rows(tc, 0)
    hit = [m for m in rows
           if int(m.group(7), 16) == pkey and int(m.group(3)) == px
           and int(m.group(4)) == py and int(m.group(5)) == pt]
    ok &= verdict(rp == 0 and rc == 0 and len(hit) > 0,
                  "rung3 PARENT->CHILD injected key=0x%04x stylus={%d,%d,%d}: "
                  "child slot0 carried it in %d of %d reported frames | %s"
                  % (pkey, px, py, pt, len(hit), len(rows),
                     hit[-1].group(0).strip() if hit else "NEVER SEEN"))
    return ok


def rung4(root, exe, out):
    """A full scripted run with FAULTS_FATAL=1 and no unhandled fault.

    THE SCENE. The fan-out call site (walk_window.cpp, immediately after the
    comms tick) is on the LEVEL path, so what it serves is a level boot, and
    level 1 is the one every battery row already treats as the reference. It is
    also the VS-capable one in the sense that matters here: hal/level_boot.cpp
    seats data_0209f344 from VS_STAR_SPAWN_ORDERS indexed by data_020a1040, the
    VS local-comms record, which is the record this lane now fills.
    """
    frames = "600"
    rp, rc, tp, tc = two_instances(root, exe, out, "r4_vs", frames,
                                   port=PORT_BASE + 8)
    ok = True
    for who, t, r in (("parent", tp, rp), ("child", tc, rc)):
        unh = t.count("FAULT code")
        # run mg16 lane MP3: reads the CARRIER's round counter, not the retired
        # transcription's. Same reasoning as rung 2 -- src/func_0203ea5c.c
        # drives itself now and its internal counters would have to be measured
        # from inside a byte-matched TU.
        lk = last(r"^\[loopback:level\] .*$", t)
        rounds = field(lk, "round", int) or 0
        ok &= verdict(r == 0 and unh == 0 and rounds > 100,
                      "rung4 VS %-6s rc=%d unhandled=%d over %s frames | %s"
                      % (who, r, unh, frames, lk or "NO CARRIER LINE"))
    return ok


def rung_depart(root, exe, out):
    """THE DEPARTED-PLAYER LIVE BIT. Review finding 1's consequence, measured.

    src/func_0203ea5c.c:275 clears the whole 0x24 record for every slot every
    round, BEFORE the null check. Without it a slot that stops sending keeps the
    0x8000 LIVE bit at +0x0C forever and the game reads a departed player's last
    frame as a current one.

    IT TAKES THREE INSTANCES, and the first version of this check used two and
    PASSED FOR THE WRONG REASON. With one child, that child leaving drops the
    parent below two players, the parent leaves the connected state, the
    lockstep stops running and func_0203e0ac -- the ROM's solo cascade -- takes
    over and rewrites all four records itself. The flag did clear, but the clear
    under test never executed. The giveaway was that SLOT 0, the parent's own
    record, froze at the departed child's frame number too.

    So: parent plus TWO children, and only the second child leaves early. The
    parent stays connected to child 1, keeps completing rounds, and keeps
    running the unpack -- which is the only thing that can clear slot 2.

    The assertion is on the WHOLE record, not just the flag: frame must go to 0
    as well, because the ROM clears 0x24 bytes and a flag-only check would pass
    against a clear that only touched the header.
    """
    dirs, envs, procs = {}, {}, []
    spec = [("P1", "parent", None, "700"), ("P2", "child", "1", "700"),
            ("P3", "child", "2", "150")]
    for tag, role, slot, frames in spec:
        d = os.path.join(out, "r6_depart_" + tag)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        e = env_base(root, d, tag)
        e["SM64DS_WINDOW_SELFTEST"] = frames
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(PORT_BASE + 24)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        if slot:
            e["SM64DS_COMMS_SLOT"] = slot
        dirs[tag], envs[tag] = d, e
    for tag, _, _, _ in spec:
        procs.append(spawn(exe, dirs[tag], envs[tag],
                           os.path.join(dirs[tag], "run.log")))
        time.sleep(0.4)
    rcs = [finish(p, 900) for p in procs]
    tp = text(os.path.join(dirs["P1"], "run.log"))

    rows2 = slot_rows(tp, 2)          # the leaver
    rows1 = slot_rows(tp, 1)          # the one that stays
    live2 = [i for i, m in enumerate(rows2) if int(m.group(6), 16) & 0x8000]
    # CLEARED means the whole record: flag 0 AND frame 0.
    clr2 = [i for i, m in enumerate(rows2)
            if int(m.group(6), 16) == 0 and int(m.group(2)) == 0]
    # And the session must still have been RUNNING when that happened, which is
    # what makes the clear the unpack's work and not the solo cascade's.
    still1 = [i for i, m in enumerate(rows1) if int(m.group(6), 16) & 0x8000]
    ok = (bool(live2) and bool(clr2) and max(clr2) > max(live2)
          and bool(still1) and max(still1) > max(live2))
    return verdict(all(r == 0 for r in rcs) and ok,
                   "rung6 departed-player record: slot2 live in %d frames (last "
                   "row %s), then FULLY cleared (flag=0 and frame=0) in %d "
                   "frames (last row %s), while slot1 stayed live to row %s so "
                   "the lockstep was still running | %s"
                   % (len(live2), max(live2) if live2 else "-",
                      len(clr2), max(clr2) if clr2 else "-",
                      max(still1) if still1 else "-",
                      rows2[-1].group(0).strip() if rows2 else "NO SLOT2 ROWS"))


def rung_childfirst(root, exe, out):
    """THE CHILD REACHES ITS FRAME LOOP FIRST. Field-report follow-up.

    The launch ORDER is not the order two processes reach their frame loops: a
    boot is seconds of asset loading and it varies. So a child can be retrying
    its JOIN before the parent has ever ticked, and the parent's socket is bound
    (func_020408b0 -> lb_open) a few statements BEFORE the game asks to be
    parent (func_02040820 -> lb_become_parent), in that same first tick, with a
    drain() in between. A JOIN handled in that window used to be accepted onto
    the wire while the parent's own state was still idle, and become_parent then
    reset the live mask and lost it -- the child believed it had joined and the
    parent never left link 2.

    Both sides must end up fully joined regardless of which starts first.
    """
    rp, rc, tp, tc = two_instances(root, exe, out, "r7_childfirst",
                                   SELFTEST_FRAMES, port=PORT_BASE + 32,
                                   stagger=-2.0)

    def joined(t, role, slot, link):
        for m in re.finditer(r"^\[comms:level\] transport=loopback.*$", t, re.M):
            l = m.group(0)
            if (field(l, "connected") == "yes" and field(l, "players") == "2"
                    and field(l, "role") == role and field(l, "slot") == slot
                    and field(l, "link") == link):
                return l
        return ""

    jp = joined(tp, "1", "0", "3")
    jc = joined(tc, "2", "1", "4")
    # run mg16 lane MP3: the carrier's line, for the retired-counter reason
    # rung 2 records.
    kp = last(r"^\[loopback:level\] .*$", tp)
    ok = bool(jp) and bool(jc) and rp == 0 and rc == 0
    return verdict(ok,
                   "rung7 CHILD STARTED FIRST (2s before the parent): parent %s "
                   "| child %s | parent carrier %s"
                   % (jp or "NEVER FULLY JOINED", jc or "NEVER FULLY JOINED",
                      kp or "none"))


def rung5(root, out):
    """The two-window script's MECHANICS, minimized.

    THE VISIBLE COLD START IS DELIBERATELY NOT RUN. A test window appeared over
    the owner's live game, and the standing order since is that every launch
    this lane makes is minimized and unactivated. The script therefore carries a
    -Minimized switch, this rung exercises exactly that path, and the visible
    run belongs to the coordinator with the owner present.
    """
    script = os.path.join(root, "port", "tools", "mp2_two_windows.ps1")
    d = os.path.join(out, "r5_script")
    os.makedirs(d, exist_ok=True)
    log = os.path.join(d, "run.log")
    cmd = ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass",
           "-File", script, "-Minimized", "-Frames", "600",
           # ON THIS RUN'S OWN PORTS. This rung invoked the script with
           # no -Port and fell through to its 51765 default, so the one
           # branch whose purpose is to eliminate the shared-base
           # collision still ran a 600-frame session on the shared base.
           "-Port", str(PORT_BASE + 48),
           "-Root", root, "-RunDir", d]
    with open(log, "wb") as f:
        p = subprocess.run(cmd, cwd=d, stdout=f, stderr=subprocess.STDOUT,
                           creationflags=NO_CONSOLE, startupinfo=SI_MIN,
                           timeout=900)
    t = text(log)
    ok = verdict(p.returncode == 0 and "MP2 TWO-WINDOW: CONNECTED" in t,
                 "rung5 two-window script (MINIMIZED variant) rc=%d | %s"
                 % (p.returncode,
                    last(r"^MP2 TWO-WINDOW: .*$", t) or "NO VERDICT LINE"))
    return ok


def rung_playmode(root, out):
    """THE TWO-WINDOW SCRIPT IN PLAY MODE. The rung that would have caught the
    field failure, and the reason it exists.

    Rung 5 runs the script with -Frames, which sets SM64DS_WINDOW_SELFTEST. That
    is the ONE configuration in which walk_window leaves stderr on the handle
    its launcher gave it. Unset -- which is what a human running -Visible gets --
    the flight recorder freopen()s stderr into playlog/play_<timestamp>.log, so
    the script's run.log stays empty and its CONNECTED grep sees nothing.

    A visible run reported NOT CONNECTED on exactly that, while both playlogs
    showed a clean join at round 0 and 6211 completed rounds per side. The
    session was fine; the verdict was blind, and every rung in this ladder was
    blind to the blindness because they all set the knob.

    So this rung runs the script the way a human does -- NO -Frames, play mode,
    flight recorder live -- and only adds -ExitWhenConnected so it terminates.
    Still minimized: the visible run is not ours to make.
    """
    script = os.path.join(root, "port", "tools", "mp2_two_windows.ps1")
    d = os.path.join(out, "r8_playmode")
    os.makedirs(d, exist_ok=True)
    log = os.path.join(d, "run.log")
    cmd = ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass",
           "-File", script, "-Minimized", "-ExitWhenConnected",
           "-Port", str(PORT_BASE + 40), "-Root", root, "-RunDir", d]
    with open(log, "wb") as f:
        p = subprocess.run(cmd, cwd=d, stdout=f, stderr=subprocess.STDOUT,
                           creationflags=NO_CONSOLE, startupinfo=SI_MIN,
                           timeout=900)
    t = text(log)
    # And prove the blindness is really gone: the verdict must have been read
    # out of a playlog, because in play mode run.log is empty by construction.
    pl = glob.glob(os.path.join(d, "P1", "playlog", "*.log"))
    empty_runlog = (os.path.exists(os.path.join(d, "P1", "run.log"))
                    and os.path.getsize(os.path.join(d, "P1", "run.log")) == 0)
    # THE PASS CONDITION IS THE MECHANISM, not just the outcome. run.log MUST be
    # empty -- that is the flight recorder having taken stderr, i.e. the exact
    # condition the field failure ran into -- AND the join must still have been
    # detected. Together those two say the verdict was read out of the playlog.
    # Requiring the emptiness is what makes this a regression test: if anyone
    # reverts Read-Log to run.log only, CONNECTED stops being found and this
    # goes red, which is precisely what did not happen before.
    return verdict(p.returncode == 0 and "MP2 TWO-WINDOW: CONNECTED" in t
                   and len(pl) > 0 and empty_runlog,
                   "rung8 two-window script in PLAY MODE, flight recorder live: "
                   "P1 run.log empty=%s (stderr went to the playlog, which is "
                   "the field-failure condition), %d playlog(s) read, rc=%d | %s"
                   % (empty_runlog, len(pl), p.returncode,
                      last(r"^MP2 TWO-WINDOW: .*$", t) or "NO VERDICT LINE"))


def main(argv):
    root = os.path.abspath(argv[0] if argv and not argv[0].startswith("-")
                           else ".")
    only = None
    out = os.path.join(root, "runs", "mg16", "out", "MP2")
    i = 0
    while i < len(argv):
        if argv[i] == "--only":
            i += 1
            only = int(argv[i])
        elif argv[i] == "--out":
            i += 1
            out = os.path.abspath(argv[i])
        i += 1
    os.makedirs(out, exist_ok=True)
    exe = os.path.join(root, "build", "port", "walk_window.exe")
    if not os.path.exists(exe):
        print("no walk_window.exe at " + exe)
        return 1

    rungs = [("1", lambda: rung1(root, exe, out)),
             ("2", lambda: rung2(root, exe, out)),
             ("3", lambda: rung3(root, exe, out)),
             ("4", lambda: rung4(root, exe, out)),
             ("5", lambda: rung5(root, out)),
             ("6", lambda: rung_depart(root, exe, out)),
             ("7", lambda: rung_childfirst(root, exe, out)),
             ("8", lambda: rung_playmode(root, out))]
    ok = True
    for n, fn in rungs:
        if only is not None and n != str(only):
            continue
        t0 = time.time()
        ok &= fn()
        print("  (rung %s took %.0fs)" % (n, time.time() - t0), flush=True)
        if not ok:
            break
    with open(os.path.join(out, "verdicts.txt"), "w") as f:
        f.write("\n".join(VERDICTS) + "\n")
    print("\n" + ("ALL GREEN" if ok else "RED"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
