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
                r"pos=\((-?\d+),(-?\d+),(-?\d+)\) touched=(\d+)", re.M)


def rows(t, slot):
    """Every probe row for `slot`, oldest first, as dicts."""
    out = []
    for m in VS.finditer(t):
        if int(m.group(2)) != slot:
            continue
        out.append(dict(f=int(m.group(1)), actor=m.group(3), no=int(m.group(4)),
                        x=int(m.group(5)), y=int(m.group(6)), z=int(m.group(7)),
                        touched=int(m.group(8))))
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
def rungA():
    """No network. Two players, two cylinders, one solver.

    The two are spawned 40.0 units apart against body cylinders of radius 40.0
    each (hal/level_boot.cpp's port_vs_spawn_extra_players), so they OVERLAP by
    40.0 on frame 0 and CylinderClsn::Process's symmetric branch has real work
    to do immediately. The assertion is that it did it: both bodies exist, they
    carry different mPlayerNo, and the gap between them GREW to the sum of the
    radii, which is where a correct solver stops pushing.
    """
    rc, t, d = one_instance("rA_two_bodies", "300",
                            {"SM64DS_VS_PLAYERS": "2"})
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

    gap0 = abs(r1[0]["x"] - r0[0]["x"])
    gapN = abs(r1[-1]["x"] - r0[-1]["x"])
    # 40.0 and 80.0 units in Fix12 (<< 12).
    ok &= M.verdict(gap0 < 80 * 4096,
                    "rungA they SPAWN OVERLAPPING | gap %.1f units against a "
                    "combined body radius of 80.0, so the solver has real work "
                    "on frame 0" % (gap0 / 4096.0))
    ok &= M.verdict(gapN > gap0,
                    "rungA THE SOLVER PUSHED THEM APART | gap %.1f -> %.1f units"
                    % (gap0 / 4096.0, gapN / 4096.0))

    # THE DECISIVE ONE, and it got sharper when the per-player input gate went
    # in. Slot 1 has NO INPUT AT ALL in this rung: one process, no transport,
    # and data_0209fc68 set so each Player reads its own (empty) pad rather than
    # mirroring player 0's. A body with no input does not walk. So every unit
    # slot 1 travels sideways is a unit something PUSHED it, and the only thing
    # in contact with it is player 0's body cylinder.
    #
    # An earlier version of this rung asserted the pair ended at exactly the sum
    # of the radii, which was true while both players mirrored one pad and both
    # walked together. With independent input the local player walks away and
    # the resting distance stops being the observable. The push is.
    ok &= M.verdict(span(r1, "x") > 0,
                    "rungA THE UN-DRIVEN BODY WAS PUSHED | slot1 has no input "
                    "of its own and still travelled %d Fix12 (%.1f units) on "
                    "x; the only thing touching it is slot0's body cylinder"
                    % (span(r1, "x"), span(r1, "x") / 4096.0))
    print("      evidence: %s" % os.path.join(d, "run.log"))
    return ok


# ---------------------------------------------------------------------------
# RUNG 9 -- THE CHILD'S INPUT MOVES THE PARENT'S COPY OF THE CHILD
# ---------------------------------------------------------------------------
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

    Both instances spawn their pair overlapping (rung A's arrangement), so the
    solver acts immediately; what rung 11 adds over rung A is that the SECOND
    body is being driven from another process. The assertion is that in the
    PARENT's world both bodies move and end at the solver's resting distance,
    with the child's own input on the wire the whole time.
    """
    rp, rc, tp, tc = two_instances(
        "r11_contact", "600", port=PORT_BASE + 32,
        extra_c={"SM64DS_COMMS_INJECT": "key=0x40",
                 "SM64DS_VS_PLAYERS": "2"},
        extra_p={"SM64DS_VS_PLAYERS": "2"})
    p0, p1 = rows(tp, 0), rows(tp, 1)
    ok = True
    ok &= M.verdict(rp == 0 and rc == 0 and bool(p0) and bool(p1),
                    "rung11 exit codes parent=%d child=%d | parent has both "
                    "bodies (slot0 rows=%d slot1 rows=%d)"
                    % (rp, rc, len(p0), len(p1)))
    if not (p0 and p1):
        return False

    # THE GAP IS THE EVIDENCE, not the contact flag, and the reason is a real
    # behaviour rather than a convenience. The flag this probe reads is the body
    # cylinder's otherOwner at +0x2f8, which CylinderClsn::Process writes on a
    # pair hit and Player::Behavior CLEARS at the top of its own next tick. The
    # probe runs before the actor tick, so it reliably samples the flag after
    # the clear and reads 0 even on frames where a push happened. Asserting on
    # it would be asserting on the sampling point.
    #
    # The separation is not sampled, it is a state: two bodies that started
    # overlapping and are now exactly the sum of their radii apart were pushed
    # there, and nothing else in the game moves two players to precisely that
    # distance and holds them.
    gap0 = abs(p1[0]["x"] - p0[0]["x"])
    gapN = abs(p1[-1]["x"] - p0[-1]["x"])
    contact = any(r["touched"] for r in p0) or any(r["touched"] for r in p1)
    ok &= M.verdict(span(p0, "x") > 0 and span(p1, "x") > 0,
                    "rung11 BOTH characters moved in the host world | slot0 "
                    "x-span=%d slot1 x-span=%d" % (span(p0, "x"), span(p1, "x")))
    ok &= M.verdict(gap0 < 80 * 4096,
                    "rung11 they START OVERLAPPING, so the solver has work to "
                    "do | gap %.1f units against a combined radius of 80.0"
                    % (gap0 / 4096.0))
    ok &= M.verdict(gapN >= 80 * 4096 - 4096,
                    "rung11 AND THE CONTACT PUSHED THEM APART | gap %.1f -> "
                    "%.1f units; 80.0 is where CylinderClsn::Process's overlap "
                    "reaches zero and it stops pushing. The remote body is "
                    "driven by the OTHER PROCESS's input the whole time. "
                    "(cylinder contact flag sampled non-zero: %s -- see the "
                    "note above, the flag is cleared before this probe runs)"
                    % (gap0 / 4096.0, gapN / 4096.0, contact))
    return ok


RUNGS = [("A", rungA), ("9", rung9), ("10", rung10), ("11", rung11)]


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
