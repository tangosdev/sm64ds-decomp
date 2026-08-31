#!/usr/bin/env python3
"""MP3's proof rungs: the ones about ACTORS rather than about the wire.

    python port/tools/mp3_proof.py [--only N[,N...]]

WHY THIS IS A SEPARATE FILE FROM mp2_proof.py. Rungs 1 to 8 all pass against a
port that spawns exactly one player, and they did, for weeks. They prove a
SESSION is healthy: two processes join, exchange blocks, carry each other's
input into the four comms records, and clean up after a departure. Not one of
them can tell you whether anything in the WORLD happened, because none of them
looks at the world.

These look at the world, and they read it out of the game's own per-slot actor
array (data_0209f394) through hal/comms_conductor.cpp's SM64DS_VS_PROBE. The
exit test the owner set is that two characters move each other, and a wire
counter cannot answer it.

    RUNG A   TWO BODIES, ONE PROCESS. Two Player actors exist, they carry
             DIFFERENT mPlayerNo, and the ROM's own cylinder solver pushes
             them apart. No network at all: this separates "the spawn and
             collision path works" from "the wire works", which are the two
             halves it is easy to debug at once and hard to debug together.
    RUNG 9   THE CHILD'S INPUT MOVES THE PARENT'S REMOTE ACTOR. Two processes;
             injected movement on the child; asserted in the PARENT's world,
             on the parent's own slot-1 actor position.
    RUNG 10  CLEAN LEAVE. The child exits, its actor stops being driven in the
             parent's world, and the parent keeps running.
    RUNG 11  PHYSICAL INTERACTION ACROSS THE WIRE. The two characters are
             driven together and the contact changes both positions.

EVERY LAUNCH IS QUIET. This imports mp2_proof's env_base/spawn/finish rather
than rolling its own, so CREATE_NO_WINDOW + SW_SHOWMINNOACTIVE +
SM64DS_NO_FOCUS=1 + SM64DS_VOLUME=0 apply here by construction and cannot be
forgotten by a rung added later.
"""

import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PORT_BASE = M.PORT_BASE + 64        # clear of every mp2 rung's port range

# [vs] f<frame> slot<i> actor=<hex> no=<n> pos=(x,y,z) touched=<n>
VS = re.compile(r"^\[vs\] f(\d+) slot(\d) actor=([0-9A-Fa-f]+) no=(\d) "
                r"char=(\d) "
                r"pos=\((-?\d+),(-?\d+),(-?\d+)\) touched=(\d+)", re.M)


def rows(t, slot):
    """Every probe row for `slot`, oldest first, as dicts."""
    out = []
    for m in VS.finditer(t):
        if int(m.group(2)) != slot:
            continue
        out.append(dict(f=int(m.group(1)), actor=m.group(3), no=int(m.group(4)),
                        char=int(m.group(5)),
                        x=int(m.group(6)), y=int(m.group(7)), z=int(m.group(8)),
                        touched=int(m.group(9))))
    return out


def span(rs, key):
    """How far `key` moved across the run. 0 for a body that never moved."""
    if not rs:
        return 0
    vals = [r[key] for r in rs]
    return max(vals) - min(vals)


def one_instance(name, frames, extra=None, level="1"):
    d = os.path.join(OUT, name)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, d, name[:6])
    env["SM64DS_WINDOW_SELFTEST"] = frames
    env["SM64DS_LEVEL"] = level
    env["SM64DS_VS_PROBE"] = "1"
    for k, v in (extra or {}).items():
        env[k] = v
    log = os.path.join(d, "run.log")
    rc = M.run_one(os.path.join(ROOT, "build", "port", "walk_window.exe"),
                   d, env, log)
    return rc, M.text(log), d


def two_instances(name, frames, extra_p=None, extra_c=None, port=PORT_BASE,
                  stagger=0.4, level="1"):
    """Parent and child, both with the VS probe on and the fan-out live."""
    base_p = {"SM64DS_LEVEL": level, "SM64DS_VS_PROBE": "1"}
    base_c = dict(base_p)
    base_p.update(extra_p or {})
    base_c.update(extra_c or {})
    return M.two_instances(ROOT, os.path.join(ROOT, "build", "port",
                                              "walk_window.exe"),
                           OUT, name, frames, extra_p=base_p, extra_c=base_c,
                           port=port, stagger=stagger)


# ---------------------------------------------------------------------------
# RUNG A -- TWO BODIES IN ONE PROCESS, AND THE ROM'S SOLVER BETWEEN THEM
# ---------------------------------------------------------------------------
OVERLAP_AT = 90     # frame the fixture fires: both bodies are out of
                    # St_LevelEnter_Main (slot 0 leaves around f31) and idle


def rungA():
    """No network. Two players, two cylinders, one solver.

    THE FIXTURE IS FORCED, NOT SPAWNED. This rung used to lean on the
    fabricated stand-in spawn that put the pair 40 units apart at boot; since
    the fc5c width fix the ROM's own entrance loop seats every player on its
    real start, and castle grounds' records 0 and 1 -- both PLAYER STARTS,
    identical to the VS arena's -- are 229 units apart, so two correct spawns
    never touch and a spawn-time overlap no longer exists to measure. The
    fixture is SM64DS_VS_OVERLAP_AT instead (hal/input_probe.cpp): at frame
    90 slot 1 is placed at slot 0 + 40 units against body cylinders of
    radius 40.0 each, so they overlap by 40.0 and CylinderClsn::Process's
    symmetric branch has real work to do. The assertion is that it did it.
    """
    rc, t, d = one_instance("rA_two_bodies", "300",
                            {"SM64DS_VS_PLAYERS": "2",
                             "SM64DS_VS_OVERLAP_AT": str(OVERLAP_AT)})
    r0, r1 = rows(t, 0), rows(t, 1)
    ok = True
    ok &= M.verdict(rc == 0 and bool(r0) and bool(r1),
                    "rungA two Player actors exist | slot0 rows=%d slot1 rows=%d"
                    % (len(r0), len(r1)))
    if not (r0 and r1):
        return False

    ok &= M.verdict(r0[-1]["no"] == 0 and r1[-1]["no"] == 1,
                    "rungA each actor knows its own slot | slot0 mPlayerNo=%d "
                    "slot1 mPlayerNo=%d (the (i<<6) spawn flag unpacked by "
                    "Player::InitResources)"
                    % (r0[-1]["no"], r1[-1]["no"]))
    ok &= M.verdict(r0[-1]["actor"] != r1[-1]["actor"],
                    "rungA they are DIFFERENT actors | %s vs %s"
                    % (r0[-1]["actor"], r1[-1]["actor"]))
    # THE CHARACTER CONTRACT IS THE ROM'S, read off each actor's own +0x6d9.
    # THIS RUNG BOOTS CASTLE GROUNDS -- mp2_proof.env_base pins
    # SM64DS_LEVEL = 1 -- which is an ADVENTURE level, not a versus match. The
    # cartridge only ever enters VS through PrepareVsMode, which is followed
    # immediately by LoadLevelNoReturn into one of the four arenas, so mode 1
    # on level 1 is a state the ROM cannot reach. On this boot
    # _Z19LoadEntranceObjects... therefore takes its NON-VS arm: every slot
    # gets f2 = data_0209caa0[0x41], the save file's single character, which
    # the port's own boot seats to 0. One save, one character, every slot.
    #
    # ASSERT THE VALUE, NOT THE AGREEMENT. The original form here was
    # `r0.char == r1.char`, and it passes on 0,0 AND on 3,3 -- so it could not
    # tell which arm of the ROM's four lines ran, and a revision that wrongly
    # promoted the mode byte sailed straight through it while its own note
    # inverted underneath. Both halves are pinned now: the character value,
    # and the game mode the a2 seat reports, so a future promotion fails here
    # instead of passing quietly.
    #
    # (The VS arm of the same four lines -- character 3 on EVERY slot -- is
    # asserted by vs_slot1_solo_check.py, which boots a real arena through the
    # ROM's own start. It belongs there and not here, because only that boot
    # is genuinely in VS.)
    ok &= M.verdict(r0[-1]["char"] == 0 and r1[-1]["char"] == 0,
                    "rungA BOTH slots carry the save-file character | slot0 "
                    "char=%d slot1 char=%d, read off each actor's +0x6d9 "
                    "(the ROM's non-VS contract on an adventure level: one "
                    "save, one character, every slot)"
                    % (r0[-1]["char"], r1[-1]["char"]))
    ok &= M.verdict("game mode 0" in t,
                    "rungA AND THE BOOT IS NOT IN VS MODE | the a2 seat "
                    "reports game mode 0. Mode 1 on an adventure level is a "
                    "state the cartridge cannot reach, and it would change "
                    "100 compiled files' behaviour, not just the character.")

    ok &= M.verdict("[vsfix]" in t,
                    "rungA the overlap fixture FIRED (SM64DS_VS_OVERLAP_AT=%d)"
                    % OVERLAP_AT)
    a0 = [r for r in r0 if r["f"] >= OVERLAP_AT]
    a1 = [r for r in r1 if r["f"] >= OVERLAP_AT]
    if not (a0 and a1):
        M.verdict(False, "rungA no probe rows after the overlap frame")
        return False
    gap0 = abs(a1[0]["x"] - a0[0]["x"])
    gapN = abs(a1[-1]["x"] - a0[-1]["x"])
    # 40.0 and 80.0 units in Fix12 (<< 12).
    ok &= M.verdict(gap0 < 80 * 4096,
                    "rungA they OVERLAP at the fixture frame | gap %.1f units "
                    "against a combined body radius of 80.0, so the solver "
                    "has real work to do" % (gap0 / 4096.0))
    ok &= M.verdict(gapN > gap0,
                    "rungA THE SOLVER PUSHED THEM APART | gap %.1f -> %.1f units"
                    % (gap0 / 4096.0, gapN / 4096.0))

    # THE DECISIVE ONE, and it got sharper when the per-player input gate went
    # in. Slot 1 has NO INPUT AT ALL in this rung: one process, no transport,
    # and data_0209fc68 set so each Player reads its own (empty) pad rather than
    # mirroring player 0's. A body with no input does not walk. So every unit
    # slot 1 travels sideways AFTER the fixture frame is a unit something
    # PUSHED it, and the only thing in contact with it is player 0's body
    # cylinder. (The span is taken over post-fixture rows only, so the
    # placement itself contributes nothing to it.)
    ok &= M.verdict(span(a1, "x") > 0,
                    "rungA THE UN-DRIVEN BODY WAS PUSHED | slot1 has no input "
                    "of its own and still travelled %d Fix12 (%.1f units) on "
                    "x after the fixture frame; the only thing touching it is "
                    "slot0's body cylinder"
                    % (span(a1, "x"), span(a1, "x") / 4096.0))
    print("      evidence: %s" % os.path.join(d, "run.log"))
    return ok


# ---------------------------------------------------------------------------
# RUNG 9 -- THE CHILD'S INPUT MOVES THE PARENT'S COPY OF THE CHILD
# ---------------------------------------------------------------------------
SHADOW = re.compile(r"^\[shadow\] frame (\d+): (\d+) triangles", re.M)


def rungS():
    """ONE SHADOW PER BODY. The orphan-caster rung.

    A tester saw the remote player walking with a SECOND shadow attached. The
    cause was not the shadow pass -- ShadowModel::RenderAll walks a linked list
    and is already correctly per-caster. It was an ORPHAN ACTOR, and the
    discard was the fc5c stride bug, not the entrance records: the ROM's
    entrance loop spawns player i from entrance record p3 + i (record 1 here
    is a real player start, byte-identical to the VS arena's), spawned a real
    second player, then read byte data_0209fc5c[1] as 0 through the
    int-stride seat and DISCARDED the pointer (data_0209f394[1] measured
    NULL) -- leaving the actor LINKED INTO THE PROCESSING LIST. It ticked,
    registered a shadow, and no body was ever drawn for it. With the flags at
    the ROM's byte stride the loop keeps what it spawns, which is why this
    census is green for a real reason and stays a rung.

    The census is arithmetic and that is what makes it a rung rather than an
    eyeball: a player casts 64 shadow triangles and this level's scenery casts
    24, so one player is 88 and two players must be 152. It measured 216 --
    one whole extra player-sized caster. Anything that reintroduces a
    phantom actor moves this number by a multiple of 64.
    """
    ok = True
    counts = {}
    for players in (1, 2):
        rc, t, d = one_instance("rS_shadow_%d" % players, "240",
                                {"SM64DS_VS_PLAYERS": str(players),
                                 "SM64DS_SHADOW_TRIS": "1"})
        # AT A FIXED FRAME, not the peak. A shadow's triangle count varies
        # with the ground it falls on, so a peak over 240 frames compares two
        # different bits of terrain and drifts (measured: 152 at frame 2, 166
        # as a peak). Frame 2 is the first frame with geometry and is the same
        # ground in both runs.
        byframe = {int(m.group(1)): int(m.group(2)) for m in SHADOW.finditer(t)}
        counts[players] = byframe.get(2, 0)
        ok &= M.verdict(rc == 0 and counts[players] > 0,
                        "rungS %d-player run produced shadow geometry | peak "
                        "%d triangles" % (players, counts[players]))
    one, two = counts.get(1, 0), counts.get(2, 0)
    ok &= M.verdict(one > 0 and two == one + 64,
                    "rungS EXACTLY ONE CASTER PER BODY | 1 player %d "
                    "triangles, 2 players %d; the second body must add exactly "
                    "64 (one player-sized caster). A multiple of 64 over that "
                    "is a phantom actor casting a shadow with no body."
                    % (one, two))
    return ok


def rung9():
    """Inject movement on the CHILD; assert it in the PARENT's world.

    A DIFFERENTIAL TEST, AND THE FIRST VERSION OF THIS RUNG WAS NOT, WHICH IS
    WHY IT IS WRITTEN DOWN. That version asserted only that the parent's slot-1
    actor moved, and it passed -- with the parent's OWN player, given no input
    at all, moving 4977 units against the remote's 4959. Both bodies simply
    fall and slide on castle grounds, so "it moved" is true of a body nobody is
    driving and the assertion proved nothing. The control is what caught it,
    which is the entire reason the rung printed one.

    So the question is asked properly: run the SAME two-instance configuration
    TWICE, once with the child pressing a direction and once with the child
    pressing nothing, and compare the parent's slot-1 trajectory between them.
    Gravity, terrain and spawn point are identical across the pair, so a
    difference in where the remote body ends up can only have come from the
    child's input crossing the wire.

    The parent's own player is the second control: its endpoint should be
    roughly the same in both runs, because nothing about IT changed.
    """
    # THE CONTROL ARM MUST PRESS *NOTHING*, EXPLICITLY, and the first attempt at
    # this rung got it wrong in a way worth recording. Its control simply left
    # SM64DS_COMMS_INJECT unset -- and an uninjected instance is not an idle
    # one: the harness's own selftest walks the character forward every frame,
    # so the child put 0x40 on the wire in BOTH arms and the difference came out
    # as exactly 0. Two runs that do the same thing agree perfectly, which reads
    # like a dead wire and is really a dead experiment.
    #
    # So the control injects key=0x0 -- the knob is ON and the value is "no
    # buttons" -- which overrides the selftest walk and is the only way to say
    # "this player is standing still" through this interface.
    #
    # 0x20 is DS KEYINPUT bit 5, LEFT on the d-pad, chosen over 0x40/UP so the
    # pressed arm differs from the selftest's own default direction as well as
    # from the control. If the wire were somehow carrying the harness's walk
    # instead of the injection, this would still show 0.
    rp1, rc1, tp1, _ = two_instances(
        "r9_child_presses", "600", port=PORT_BASE,
        extra_c={"SM64DS_COMMS_INJECT": "key=0x20", "SM64DS_VS_PLAYERS": "2"},
        extra_p={"SM64DS_VS_PLAYERS": "2"})
    rp2, rc2, tp2, _ = two_instances(
        "r9_child_idle", "600", port=PORT_BASE + 8,
        extra_c={"SM64DS_COMMS_INJECT": "key=0x0", "SM64DS_VS_PLAYERS": "2"},
        extra_p={"SM64DS_VS_PLAYERS": "2"})

    ok = True
    ok &= M.verdict(rp1 == 0 and rc1 == 0 and rp2 == 0 and rc2 == 0,
                    "rung9 exit codes | pressed run parent=%d child=%d ;; idle "
                    "run parent=%d child=%d" % (rp1, rc1, rp2, rc2))

    a1, a0 = rows(tp1, 1), rows(tp1, 0)     # pressed run: remote, local
    b1, b0 = rows(tp2, 1), rows(tp2, 0)     # idle run
    ok &= M.verdict(bool(a1) and bool(b1),
                    "rung9 the parent has a remote actor in BOTH runs | "
                    "pressed rows=%d idle rows=%d" % (len(a1), len(b1)))
    if not (a1 and b1 and a0 and b0):
        return False
    ok &= M.verdict(a1[-1]["no"] == 1 and b1[-1]["no"] == 1,
                    "rung9 and it is the REMOTE slot's actor | mPlayerNo=%d"
                    % a1[-1]["no"])

    # The signal: how far apart the remote body ends up between the two runs.
    dx = abs(a1[-1]["x"] - b1[-1]["x"])
    dz = abs(a1[-1]["z"] - b1[-1]["z"])
    remote_delta = dx + dz
    # The control: the same difference for the parent's OWN player.
    cx = abs(a0[-1]["x"] - b0[-1]["x"])
    cz = abs(a0[-1]["z"] - b0[-1]["z"])
    local_delta = cx + cz

    ok &= M.verdict(remote_delta > 4096,
                    "rung9 THE CHILD'S INPUT MOVED THE REMOTE ACTOR IN THE "
                    "HOST WORLD | the parent's slot-1 body ends %d Fix12 "
                    "(%.1f units) away from where it ends when the child "
                    "presses nothing"
                    % (remote_delta, remote_delta / 4096.0))
    ok &= M.verdict(remote_delta > local_delta,
                    "rung9 and the CONTROL says it was the input, not the run "
                    "| remote moved %.1f units between the two runs, the "
                    "parent's own untouched player moved %.1f"
                    % (remote_delta / 4096.0, local_delta / 4096.0))
    return ok


# ---------------------------------------------------------------------------
# RUNG 10 -- CLEAN LEAVE
# ---------------------------------------------------------------------------
def rung10():
    """The child exits first; the parent must survive and stop being driven.

    The child runs a SHORT selftest and the parent a long one, so the child
    leaves while the parent is still ticking -- which is the real shape of one
    player quitting. The parent must exit 0 (it kept running) and its record
    for the departed slot must go dead, which is src/func_0203ea5c.c:275's
    per-record clear doing its job now that the ROM owns it.
    """
    d = os.path.join(OUT, "r10_leave")
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    exe = os.path.join(ROOT, "build", "port", "walk_window.exe")

    dp = os.path.join(d, "p1")
    dc = os.path.join(d, "p2")
    for x in (dp, dc):
        os.makedirs(os.path.join(x, "tmp"), exist_ok=True)
    ep = M.env_base(ROOT, dp, "r10p")
    ec = M.env_base(ROOT, dc, "r10c")
    for e, role, frames in ((ep, "parent", "900"), (ec, "child", "200")):
        e["SM64DS_WINDOW_SELFTEST"] = frames
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(PORT_BASE + 16)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_LEVEL"] = "1"
        e["SM64DS_VS_PLAYERS"] = "2"
        e["SM64DS_VS_PROBE"] = "1"

    lp, lc = os.path.join(dp, "run.log"), os.path.join(dc, "run.log")
    pp = M.spawn(exe, dp, ep, lp)
    import time
    time.sleep(0.4)
    pc = M.spawn(exe, dc, ec, lc)
    rc_c = M.finish(pc, 900)
    rc_p = M.finish(pp, 900)
    tp = M.text(lp)

    ok = True
    ok &= M.verdict(rc_p == 0,
                    "rung10 THE PARENT SURVIVED THE CHILD LEAVING | parent "
                    "rc=%d, child rc=%d (child ran 200 frames, parent 900)"
                    % (rc_p, rc_c))
    # The comms record for the departed slot must go fully dead. This is the
    # ROM's own clear at src/func_0203ea5c.c:275 now, not a transcription of it.
    live_after = []
    for m in re.finditer(r"^\[comms:level\]\s+slot1 frame=(-?\d+).*flag=([0-9a-f]+)",
                         tp, re.M):
        live_after.append((int(m.group(1)), int(m.group(2), 16)))
    ever_live = any((f & 0x8000) for _, f in live_after)
    ended_dead = bool(live_after) and (live_after[-1][1] & 0x8000) == 0
    ok &= M.verdict(ever_live and ended_dead,
                    "rung10 the departed slot's record went DEAD | was live "
                    "at some point=%s, final flag=0x%04x (bit 0x8000 clear "
                    "means src/func_0203ea5c.c:275's per-record clear ran)"
                    % (ever_live, live_after[-1][1] if live_after else 0))
    print("      evidence: %s" % lp)
    return ok


# ---------------------------------------------------------------------------
# RUNG 11 -- PHYSICAL INTERACTION
# ---------------------------------------------------------------------------
def rung11():
    """Two processes, and the two characters push each other.

    Both instances force the pair overlapped at frame 90 (rung A's fixture,
    set in BOTH environments so the lockstep worlds stay identical); what
    rung 11 adds over rung A is that the SECOND body is being driven from
    another process. The assertion is that in the PARENT's world both bodies
    move and the contact pushed the pair out to at least the solver's
    resting distance, with the child's own input on the wire the whole time.
    """
    fix = {"SM64DS_VS_PLAYERS": "2",
           "SM64DS_VS_OVERLAP_AT": str(OVERLAP_AT)}
    rp, rc, tp, tc = two_instances(
        "r11_contact", "600", port=PORT_BASE + 32,
        extra_c=dict(fix, SM64DS_COMMS_INJECT="key=0x40"),
        extra_p=dict(fix))
    p0, p1 = rows(tp, 0), rows(tp, 1)
    ok = True
    ok &= M.verdict(rp == 0 and rc == 0 and bool(p0) and bool(p1),
                    "rung11 exit codes parent=%d child=%d | parent has both "
                    "bodies (slot0 rows=%d slot1 rows=%d)"
                    % (rp, rc, len(p0), len(p1)))
    if not (p0 and p1):
        return False

    ok &= M.verdict("[vsfix]" in tp and "[vsfix]" in tc,
                    "rung11 the overlap fixture FIRED in both worlds "
                    "(SM64DS_VS_OVERLAP_AT=%d)" % OVERLAP_AT)
    a0 = [r for r in p0 if r["f"] >= OVERLAP_AT]
    a1 = [r for r in p1 if r["f"] >= OVERLAP_AT]
    if not (a0 and a1):
        M.verdict(False, "rung11 no probe rows after the overlap frame")
        return False

    # THE GAP IS THE EVIDENCE, not the contact flag, and the reason is a real
    # behaviour rather than a convenience. The flag this probe reads is the body
    # cylinder's otherOwner at +0x2f8, which CylinderClsn::Process writes on a
    # pair hit and Player::Behavior CLEARS at the top of its own next tick. The
    # probe runs before the actor tick, so it reliably samples the flag after
    # the clear and reads 0 even on frames where a push happened. Asserting on
    # it would be asserting on the sampling point.
    #
    # The push is asserted as the LARGEST separation after the fixture frame,
    # not the final one: both bodies are walking under real input here (the
    # parent's selftest key, the child's injected key over the wire), so where
    # they END is geometry, but two bodies that were 40 units overlapped and
    # later measure at least the sum of their radii apart were pushed there by
    # the solver -- walking alone cannot be told apart from pushing at any
    # single later frame, which is exactly why the pre-fixture version of this
    # assertion went vacuous the day the spawns stopped overlapping.
    gap0 = abs(a1[0]["x"] - a0[0]["x"])
    by_f = {r["f"]: r["x"] for r in a0}
    gap_max = max(abs(r["x"] - by_f[r["f"]])
                  for r in a1 if r["f"] in by_f)
    contact = any(r["touched"] for r in a0) or any(r["touched"] for r in a1)
    ok &= M.verdict(span(a0, "x") > 0 and span(a1, "x") > 0,
                    "rung11 BOTH characters moved in the host world after the "
                    "fixture | slot0 x-span=%d slot1 x-span=%d"
                    % (span(a0, "x"), span(a1, "x")))
    ok &= M.verdict(gap0 < 80 * 4096,
                    "rung11 they OVERLAP at the fixture frame, so the solver "
                    "has work to do | gap %.1f units against a combined "
                    "radius of 80.0" % (gap0 / 4096.0))
    ok &= M.verdict(gap_max >= 80 * 4096 - 4096,
                    "rung11 AND THE CONTACT PUSHED THEM APART | gap %.1f -> "
                    "max %.1f units; 80.0 is where CylinderClsn::Process's "
                    "overlap reaches zero and it stops pushing. The remote "
                    "body is driven by the OTHER PROCESS's input the whole "
                    "time. (cylinder contact flag sampled non-zero: %s -- see "
                    "the note above, the flag is cleared before this probe "
                    "runs)"
                    % (gap0 / 4096.0, gap_max / 4096.0, contact))
    return ok


RUNGS = [("A", rungA), ("S", rungS), ("9", rung9), ("10", rung10), ("11", rung11)]


def main(argv):
    global OUT
    OUT = os.path.join(ROOT, "runs", "mg16", "out", "MP3")
    os.makedirs(OUT, exist_ok=True)
    only = None
    for i, a in enumerate(argv):
        if a == "--only" and i + 1 < len(argv):
            only = set(argv[i + 1].split(","))
    ok = True
    for name, fn in RUNGS:
        if only and name not in only:
            continue
        import time
        t0 = time.time()
        ok &= bool(fn())
        print("  (rung %s took %ds)" % (name, int(time.time() - t0)))
    print()
    print("ALL GREEN" if ok else "RED")
    with open(os.path.join(OUT, "mp3_verdicts.txt"), "w") as f:
        f.write("\n".join(M.VERDICTS) + "\n")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
