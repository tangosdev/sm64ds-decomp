"""TWO WINDOWS, ONE VERSUS MATCH -- the VS boot's own online proof.

port/tools/net_proof.py proved the address modes on the LEVEL path
(SM64DS_LEVEL=1 plus SM64DS_WINDOW_SELFTEST), which is the path its rungs
were written against. Nothing proved the VS path, and that gap is exactly
where the 0.2.15 demo failed: two windows booted SM64DS_VS_MAP, both reached
the arena, and neither ever installed a transport.

So this file measures the boot a VS demo actually runs:

    SM64DS_VS_MAP=<0..3>            the ROM's own map list
    SM64DS_COMMS_RELAY=<host:port>  the rendezvous
    SM64DS_COMMS_CODE=<up to 8>     the session both ends share
    SM64DS_COMMS_ROLE=parent|child  (or the relay's own 0|1 spelling)

and asserts the three things a player would call "it worked":

    INSTALL   both instances say [comms:loopback] installed ... via RELAY
    PAIRING   both instances say the relay ACKed their HELLO, and the ROM's
              own conductor says the session came up with two players
    WIRE      each instance's [vs] probe shows TWO player actors, and the
              slot that is NOT mine carries the pad the OTHER process was
              injecting -- a value this process never pressed, so it cannot
              have come from anywhere but the wire. THESE TWO ASSERTIONS ARE
              THE PROOF. Everything else here can pass without a transport.

WHICH IS NOT A FIGURE OF SPEECH, AND THE PRECISE CLAIM IS THIS. A control pair
with NO comms env at all -- zero [comms] lines in either log, plus
SM64DS_VS_PLAYERS=2 so the census has two actors to compare -- fails INSTALL,
fails PAIRING and fails both WIRE assertions, exactly as it should. It PASSES
the DETERMINISM check at the bottom, with the identical distinct-position count
a live run scores. So DETERMINISM is the one assertion here that carries no
signal by itself: it is reported as CORROBORATING, its verdict line states
whether the WIRE assertions held, and echo_of's docstring carries the numbers.

IF A LATER RUNG IS ADDED HERE, hold it to the same question before believing
it: WOULD A CONTROL WITH NO TRANSPORT FAIL IT? If not, it is a description of
a run, not a proof of a session.

Every run is quiet and muted the way mp2_proof.env_base makes them
(SW_SHOWMINNOACTIVE, CREATE_NO_WINDOW, SM64DS_NO_FOCUS, SM64DS_MINIMIZED,
SM64DS_VOLUME=0), because these run on the owner's desk.

    python port/tools/vs_online_proof.py --relay tangos.dev:41234
    python port/tools/vs_online_proof.py --local-relay
"""

import argparse
import os
import re
import socket
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vs_online_proof")

# BOTH ENDS INJECT, AND WITH DIFFERENT KEYS, because the obvious version of
# this test proves nothing. The selftest holds W on BOTH instances and W lands
# on the pad as 0x0040, so a run where both slots read 0040 is exactly what a
# COMPLETELY DEAD WIRE would also produce -- each process reading its own key
# into both slots. Pinning a distinct key per process fixes that: 0x80 (DOWN)
# and 0x20 (LEFT) are values the OTHER process never produces, so a slot that
# carries the other end's key carries something that can only have arrived over
# the wire, and the assertion is a real one in both directions.
PARENT_KEY = 0x80
CHILD_KEY = 0x20


def run_pair(name, code, relay, frames, vs_map, timeout=900, stagger=0.6,
             roles=("parent", "child"), port_base=0):
    dp = os.path.join(OUT, name + "_p1")
    dc = os.path.join(OUT, name + "_p2")
    for d in (dp, dc):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    ep = M.env_base(ROOT, dp, "p1")
    ec = M.env_base(ROOT, dc, "p2")
    for e, role in ((ep, roles[0]), (ec, roles[1])):
        # env_base names a LEVEL because every rung in mp2_proof measures one.
        # A VS boot names its own destination and must not carry a second.
        e.pop("SM64DS_LEVEL", None)
        e["SM64DS_VS_MAP"] = str(vs_map)
        e["SM64DS_WINDOW_SELFTEST"] = str(frames)
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_RELAY"] = relay
        e["SM64DS_COMMS_CODE"] = code
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_VS_PROBE"] = "1"
    ep["SM64DS_COMMS_INJECT"] = "key=0x%02x" % PARENT_KEY
    ec["SM64DS_COMMS_INJECT"] = "key=0x%02x" % CHILD_KEY
    if port_base:
        # The loopback's default base is one fixed number, so ANY other
        # instance of the game on the machine -- another lane's test, a
        # straggler -- makes the parent's bind fail with winsock 10048 and
        # the whole proof degrades to two solo boots. Measured, not
        # theoretical. A caller-picked base keeps proofs out of each
        # other's way without touching anyone else's process.
        ep["SM64DS_COMMS_PORT"] = str(port_base)
        ec["SM64DS_COMMS_PORT"] = str(port_base)
    lp, lc = os.path.join(dp, "run.log"), os.path.join(dc, "run.log")
    t0 = time.time()
    pp = M.spawn(EXE, dp, ep, lp)
    time.sleep(stagger)
    pc = M.spawn(EXE, dc, ec, lc)
    rp = M.finish(pp, timeout)
    rc = M.finish(pc, timeout)
    return dict(rc_p=rp, rc_c=rc, tp=M.text(lp), tc=M.text(lc),
                log_p=lp, log_c=lc, wall=time.time() - t0)


def installed_relay(t):
    """Did this instance install a transport, in RELAY mode?"""
    return (re.search(r"^\[comms:loopback\] installed as (parent|child)", t,
                      re.M) is not None and
            re.search(r"^\[comms:loopback\] open\(mode=2\).*via RELAY", t,
                      re.M) is not None)


def paired(t):
    """Did a status-0 HELLO-ACK land on this instance?"""
    return re.search(r"^\[comms:relay\] paired as (parent|child) on code",
                     t, re.M) is not None


def session_line(t):
    m = re.search(r"^\[comms:conductor\] session up after .*$", t, re.M)
    return m.group(0) if m else ""


def probe_rows(t):
    """[vs] fN slotK ... -> {(frame, slot): dict}."""
    out = {}
    for m in re.finditer(r"^\[vs\] f(\d+) slot(\d) actor=(\S+) no=(-?\d+) "
                         r"char=(\d+) pos=\((-?\d+),\s*(-?\d+),\s*(-?\d+)\) "
                         r"touched=(\d+) pad=([0-9a-fA-F]{4})", t, re.M):
        out[(int(m.group(1)), int(m.group(2)))] = dict(
            actor=m.group(3), no=int(m.group(4)), char=int(m.group(5)),
            x=int(m.group(6)), y=int(m.group(7)), z=int(m.group(8)),
            pad=int(m.group(10), 16))
    return out


def my_slot(t):
    m = None
    for m in re.finditer(r"^\[vs\] f\d+ count=(\d+) me=(\d+)", t, re.M):
        pass
    return (int(m.group(1)), int(m.group(2))) if m else (0, -1)


def remote_pad_seen(t, mine, want):
    """Did a slot that is NOT mine ever carry `want` on the pad?"""
    for (f, s), r in probe_rows(t).items():
        if s != mine and r["pad"] == want:
            return f
    return -1


def track(t, slot):
    """[(frame, (x,y,z))] for one slot, in frame order."""
    rows = probe_rows(t)
    return [(f, (r["x"], r["y"], r["z"]))
            for (f, s), r in sorted(rows.items()) if s == slot]


def echo_of(tp, tc, slot):
    """Do the two processes' worlds agree on this actor, frame for frame?

    READ THIS BEFORE QUOTING THE RESULT AS A NETWORKING PROOF.

    THE ASSERTION IS VACUOUS ON ITS OWN, and that is MEASURED rather than
    argued. A control pair with NO SM64DS_COMMS_* IN THE ENVIRONMENT AT ALL --
    no role, no relay, no code, no fan-out, zero [comms] lines in either log,
    with SM64DS_VS_PLAYERS=2 so there are two actors to compare -- passes it:

        600 frames, no transport   DETERMINISM PASS, 569 distinct positions
        600 frames, live relay     DETERMINISM PASS, 569 distinct positions
        900 frames, no transport   DETERMINISM PASS, 869 distinct positions
        900 frames, live relay     DETERMINISM PASS, 809 distinct positions

    The same number, on the same frame count, with the wire unplugged. Of
    course: both processes run the same deterministic selftest over the same
    build and the same level, so their worlds agree frame for frame whether or
    not one datagram is ever exchanged, and the actor moves in both. Neither
    half of the check -- the agreement or the movement -- discriminates, and
    the position count is not a tie-breaker either.

    WHAT MAKES IT MEAN ANYTHING is the pair of CROSS-INJECT assertions above
    it, and nothing else. Each process pins a key the other never presses and
    must read the OTHER's value on the remote slot; in the no-transport
    control those two FAIL, and they are what actually catches a dead wire.
    Once they hold, this adds the second and weaker claim on top: that what
    crossed produced the SAME SIMULATION on both sides rather than merely
    arriving. Worth having -- it is the determinism claim, not the
    connectivity one -- but corroborating, and the verdict says so in words.

    The distinct-position count is printed because it is the tell: a number in
    the 800s is normal for a live session AND for a dead one, so a reader who
    sees it cannot mistake a healthy-looking count for evidence.
    """
    a, b = dict(track(tp, slot)), dict(track(tc, slot))
    common = sorted(set(a) & set(b))
    if not common:
        return False, 0, "no frame is in both censuses"
    bad = [f for f in common if a[f] != b[f]]
    moved = len({a[f] for f in common})
    if bad:
        return False, moved, ("%d of %d frames disagree, first at f%d"
                              % (len(bad), len(common), bad[0]))
    if moved < 2:
        return False, moved, "the two agree but the actor never moved"
    return True, moved, ("%d frames identical on both, %d distinct positions "
                         "(a no-transport control scores about the same -- "
                         "this line is only evidence once the two cross-inject "
                         "assertions above have passed)"
                         % (len(common), moved))


def two_actors(t):
    """The last frame that reported a non-null actor in two distinct slots."""
    rows = probe_rows(t)
    best = -1
    frames = sorted({f for (f, _s) in rows})
    for f in frames:
        live = [s for s in range(4)
                if (f, s) in rows and rows[(f, s)]["actor"] != "NULL"]
        if len(live) >= 2:
            best = f
    return best


def start_local_relay(port):
    relay = os.path.join(ROOT, "port", "tools", "relay", "relay.py")
    env = dict(os.environ)
    env["SM64DS_RELAY_PORT"] = str(port)
    env["SM64DS_RELAY_BIND"] = "127.0.0.1"
    log = open(os.path.join(OUT, "relay.log"), "wb")
    p = subprocess.Popen([sys.executable, relay], env=env,
                         stdout=log, stderr=subprocess.STDOUT)
    p._logfile = log
    time.sleep(1.0)
    return p


def free_port():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(("127.0.0.1", 0))
    p = s.getsockname()[1]
    s.close()
    return p


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--relay", default="tangos.dev:41234")
    ap.add_argument("--local-relay", action="store_true")
    ap.add_argument("--frames", type=int, default=900)
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--code", default="VSPROOF")
    ap.add_argument("--roles", default="parent,child")
    ap.add_argument("--name", default="relay")
    ap.add_argument("--port-base", type=int, default=0,
                    help="local UDP base for the two instances; use when "
                         "another game instance holds the default 51765")
    a = ap.parse_args()

    os.makedirs(OUT, exist_ok=True)
    if not os.path.exists(EXE):
        print("no exe at " + EXE)
        return 2
    print("exe %s  sha %s" % (EXE, M.sha(EXE)))

    proc = None
    relay = a.relay
    if a.local_relay:
        port = free_port()
        proc = start_local_relay(port)
        relay = "127.0.0.1:%d" % port
        print("local relay on " + relay)
    try:
        res = run_pair(a.name, a.code, relay, a.frames, a.map,
                       roles=tuple(a.roles.split(",")),
                       port_base=a.port_base)
    finally:
        if proc:
            proc.terminate()
            proc._logfile.close()

    tp, tc = res["tp"], res["tc"]
    ok = True
    ok &= M.verdict(res["rc_p"] == 0 and res["rc_c"] == 0,
                    "both instances exited 0 (%d/%d)"
                    % (res["rc_p"], res["rc_c"]))
    ok &= M.verdict(installed_relay(tp) and installed_relay(tc),
                    "both installed a RELAY transport")
    ok &= M.verdict(paired(tp) and paired(tc), "both paired with the relay")
    sp, sc = session_line(tp), session_line(tc)
    ok &= M.verdict(bool(sp) and bool(sc),
                    "the conductor brought the session up on both")
    np_, mp_ = my_slot(tp)
    nc, mc = my_slot(tc)
    ok &= M.verdict(np_ == 2 and nc == 2,
                    "both booted a TWO player arena (count %d/%d)" % (np_, nc))
    ok &= M.verdict(mp_ == 0 and mc == 1,
                    "the two agree who is who (me=%d/%d)" % (mp_, mc))
    ok &= M.verdict(two_actors(tp) >= 0 and two_actors(tc) >= 0,
                    "two player actors in each census (last frame %d/%d)"
                    % (two_actors(tp), two_actors(tc)))
    # THE TWO THAT ACTUALLY PROVE TRAFFIC. The assertions above prove a SESSION
    # FORMED -- a no-transport control fails INSTALL and PAIRING too -- but
    # none of them shows a byte of game state crossing. These do, and they are
    # the only ones the DETERMINISM check below can lean on.
    f = remote_pad_seen(tp, mp_, CHILD_KEY)
    wire_p = M.verdict(f >= 0,
                       "WIRE: the parent reads the CHILD's injected pad %04x "
                       "on the remote slot, a value it never pressed "
                       "(frame %d)" % (CHILD_KEY, f))
    g = remote_pad_seen(tc, mc, PARENT_KEY)
    wire_c = M.verdict(g >= 0,
                       "WIRE: the child reads the PARENT's injected pad %04x "
                       "on the remote slot, a value it never pressed "
                       "(frame %d)" % (PARENT_KEY, g))
    ok &= wire_p and wire_c
    # AND THE CORROBORATING ONE, which is worthless without the two above --
    # measured, not assumed: a no-transport control passes it. See echo_of.
    good, moved, why = echo_of(tp, tc, 0)
    ok &= M.verdict(good,
                    "DETERMINISM (corroborating, %s): the two worlds agree on "
                    "the parent's player | %s"
                    % ("meaningful -- the WIRE assertions hold"
                       if (wire_p and wire_c)
                       else "MEANINGLESS HERE -- a WIRE assertion above "
                            "FAILED, so this says only that two copies of one "
                            "deterministic selftest agree", why))
    print("")
    for line in (sp, sc):
        if line:
            print("  " + line)
    print("")
    print("logs: %s\n      %s" % (res["log_p"], res["log_c"]))
    print("ALL GREEN" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
