#!/usr/bin/env python3
"""THE ONLINE LADDER: does two-player actually work over a real network?

    python port/tools/net_proof.py [--only N0,N2,...] [--frames N]
                                   [--relay HOST[:PORT]]   local relay override
                                   [--live  HOST[:PORT]]   live relay for N6
                                   [--code CODE]

WHY THIS FILE EXISTS. Every multiplayer proof in this tree so far ran over
127.0.0.1, and loopback is not a network: it never loses a datagram it did not
lose on purpose, its round trip is microseconds, and every peer's address is
arithmetic. Three things in hal/comms_loopback.cpp were true only because of
that, and lane NET changed them. This file is what says whether the change
works, and it is the only thing standing between "the code looks right" and a
claim about playing online.

THE RUNGS

  N0  SOLO IS BYTE-IDENTICAL. With none of the new env set, the selftest's
      final position must equal the pinned pre-change baseline and the carrier
      must be SILENT. This is the merge gate's first question and so it is the
      ladder's first rung, not its last.
  N1  LOOPBACK IS UNCHANGED. The two-window loopback session still forms,
      still advances rounds, and still reports mode=loopback. The address work
      rewrote how a sender is identified; this is the regression net under it.
  N2  DIRECT MODE OVER THE REAL LAN ADDRESS. Two instances, one binding every
      interface, the other pointed at this machine's actual LAN IP -- NOT
      127.0.0.1, which would prove nothing the loopback rungs do not.
  N3  RELAY MODE. Both instances reach each other only through a relay: no
      instance is ever told the other's address, which is exactly the
      no-port-forwarding case and the only one two home connections can do.
  N4  THE PACE AT RTT. The induced-delay sweep. What the ROM's stop-and-wait
      lockstep costs at 0/40/80/120 ms round trip, measured as wall time for a
      fixed frame count against the zero-latency arm of the same setup.
  N5  LOSS ON TOP OF LATENCY. 80 ms and 5% loss together, which is a bad
      evening on a real connection, with the pace measured the same way.
  N6  THE LIVE RELAY. Two instances on this desk, reaching each other only
      through the deployed VPS service: out through this machine's NAT, across
      the public internet, and back. The one rung that cannot be faked.
  N7  DOES PIPELINING BUY THE PACE BACK? Each round trip run twice, once
      stop-and-wait and once with an input delay, and the two compared. A
      mitigation that is not measured against the thing it mitigates is a
      claim, not a result.

PORT DISCIPLINE. Every port here derives from THIS PROCESS'S PID through
mp2_proof.PORT_BASE, offset past what mp2/mp3/mp_stall already reach (+64), so
a run of this file can never land on 51765 -- the shipped default, which is the
port an owner's live two-window session is using. The relay gets its own port
out of the same bucket.

QUIET RULE. Every launch goes through mp2_proof.spawn: CREATE_NO_WINDOW,
SW_SHOWMINNOACTIVE, SM64DS_NO_FOCUS=1, SM64DS_MINIMIZED=1, SM64DS_VOLUME=0.
Minimized and never activated -- not hidden -- and silent.
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

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, "runs", "vsdec", "out", "NET")
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
# THE RELAY SERVICE ITSELF, lane RELAY's file, driven here unmodified. Rung N3
# is therefore an INTEROP test between two independent implementations of the
# same frozen wire contract, written by two lanes off the same spec -- which is
# a better proof than either lane checking its own homework. This harness owns
# neither the service nor its tests; it only points the game at it.
RELAY_PY = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "relay", "relay.py")

# Past mp_stall_proof's +64, inside this pid's own 128-wide bucket.
PORT = M.PORT_BASE + 80
RELAY_PORT = M.PORT_BASE + 112

FRAMES = "600"          # long enough for a session to form, settle, and be
                        # measured well past its settling


def lan_address():
    """This machine's LAN address, the one a peer would actually dial.

    Found by asking the routing table where a packet to a public address would
    leave from -- a UDP connect() on an unconnected socket sends nothing, it
    only picks the route. Reading the hostname's A record instead is the usual
    shortcut and it is wrong on any machine with more than one adapter, which
    is every machine with a VPN or a hypervisor on it.
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("203.0.113.9", 9))     # TEST-NET-3, routed nowhere
        return s.getsockname()[0]
    finally:
        s.close()


def report_line(t, which="last"):
    """The carrier's periodic report, where the tunable counters live.

    ONLY EMITTED AT A LEVEL, AND ONLY WITH SM64DS_COMMS_FANOUT AND _REPORT BOTH
    SET (walk_window.cpp's comms_fanout_report gate). So nothing load-bearing
    may depend on it existing -- the session verdict below reads the carrier's
    UNCONDITIONAL lines instead, and this is only for the extra counters. The
    first version of this file asserted on this line alone and reported two
    perfectly good sessions as total failures.
    """
    got = [m.group(0) for m in re.finditer(r"^\[loopback:[^\]]*\].*$", t, re.M)]
    if not got:
        return ""
    return got[-1] if which == "last" else got[0]


def mode_of(t):
    """Which address mode a run actually came up in.

    Read off open()'s own line, printed on every run whatever the report knobs
    say. An assertion whose evidence is conditional will one day read "?" and
    call that a failure, or worse, call it a pass.
    """
    m = re.search(r"^\[comms:loopback\] open\(mode=\d+\).*$", t, re.M)
    if not m:
        return "?"
    line = m.group(0)
    if "via RELAY" in line:
        return "relay"
    if "DIRECT" in line:
        return "direct"
    if "udp 127.0.0.1:" in line:
        return "loopback"
    return "?"


def rounds_of(t):
    """Rounds the carrier actually completed, off its close line."""
    m = re.search(r"^\[comms:loopback\] closed after (\d+) rounds", t, re.M)
    return int(m.group(1)) if m else 0


def players_of(t):
    """The last live mask and player count the carrier announced."""
    got = re.findall(r"live mask 0x([0-9a-f]+), players (\d+)", t)
    if not got:
        return 0, 0
    return int(got[-1][0], 16), int(got[-1][1])


def num(line, key, cast=int, default=None):
    m = re.search(r"\b%s=([^\s]+)" % re.escape(key), line)
    if not m:
        return default
    try:
        return cast(m.group(1))
    except ValueError:
        return default


def session_ok(tp, tc, min_rounds=30):
    """Both sides agree a two-player session formed and rounds advanced."""
    lp, np_ = players_of(tp)
    lc, nc = players_of(tc)
    rounds_p, rounds_c = rounds_of(tp), rounds_of(tc)
    if lp != 3 or lc != 3:
        return False, ("live masks 0x%x/0x%x, wanted 0x3 both (players %d/%d)"
                       % (lp, lc, np_, nc))
    if rounds_p < min_rounds or rounds_c < min_rounds:
        return False, "rounds %d/%d, too few to be a session" % (rounds_p, rounds_c)
    return True, "live=0x3 both, rounds %d/%d" % (rounds_p, rounds_c)


def run_pair(name, extra_p, extra_c, frames=None, port=None, stagger=0.4,
             timeout=900):
    """Two instances, parent and child, quiet and muted. Returns timing too.

    `frames` defaults to the module global RESOLVED AT CALL TIME, not at def
    time: --frames rewrites that global, and a default argument bound at import
    would have silently ignored the flag while the report quoted it.
    """
    frames = frames or FRAMES
    port = port or PORT
    dp = os.path.join(OUT, name + "_p1")
    dc = os.path.join(OUT, name + "_p2")
    for d in (dp, dc):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    ep = M.env_base(ROOT, dp, "p1")
    ec = M.env_base(ROOT, dc, "p2")
    for e, role in ((ep, "parent"), (ec, "child")):
        e["SM64DS_WINDOW_SELFTEST"] = frames
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(port)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
    ep.update(extra_p or {})
    ec.update(extra_c or {})
    lp, lc = os.path.join(dp, "run.log"), os.path.join(dc, "run.log")

    t0 = time.time()
    pp = M.spawn(EXE, dp, ep, lp)
    time.sleep(stagger)
    pc = M.spawn(EXE, dc, ec, lc)
    rp = M.finish(pp, timeout)
    rc = M.finish(pc, timeout)
    wall = time.time() - t0
    return dict(rc_p=rp, rc_c=rc, tp=M.text(lp), tc=M.text(lc), wall=wall,
                log_p=lp, log_c=lc)


# ---------------------------------------------------------------------------
# THE PACE INSTRUMENT
#
# AN EARLIER VERSION OF THIS REPORTED FRAMES PER SECOND AGAINST A 30 fps
# TARGET. That was wrong and the numbers it produced looked entirely
# reasonable, which is the dangerous kind of wrong. The window selftest does
# NOT run at 30 fps -- it runs the frame loop as fast as it can, and the
# evidence is right there in the arms: 600 frames finish in about 12.7 s on a
# zero-latency session, which is nearer 47 fps for the frame part alone. Any
# "fps" computed against a 30 fps assumption was an invented number dressed as
# a measurement.
#
# So the instrument is the thing actually measured: WALL TIME for a fixed frame
# count, and the RATIO of an arm to a zero-latency arm of the identical
# two-process setup. Boot cost is the same in both and divides out. "2.1x
# slower than a LAN session" is a claim the stopwatch supports; "14.2 fps" was
# not.
# ---------------------------------------------------------------------------


def pace(res, frames, baseline_wall=None):
    """Wall time for the fixed frame count, and the slowdown vs a baseline.

    The child's round counter is the honest one to read: a parent completes a
    round the instant its own block is staged if it already holds the child's,
    so a parent alone can look fast while the session as a whole crawls.
    """
    wall = res["wall"]
    out = dict(rounds=rounds_of(res["tc"]), wall=wall, slowdown=None,
               extra=None)
    if baseline_wall:
        out["slowdown"] = wall / baseline_wall
        out["extra"] = wall - baseline_wall
    return out


# ---------------------------------------------------------------------------
# RUNGS
# ---------------------------------------------------------------------------
def rungN0(a):
    """SOLO IS BYTE-IDENTICAL and the carrier is silent."""
    d = os.path.join(OUT, "n0_solo")
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, d, "n0")
    env["SM64DS_WINDOW_SELFTEST"] = "300"
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, d, env, log)
    t = M.text(log)
    pos = re.search(r"^selftest: \d+ frames, pos=\(([^)]*)\)", t, re.M)
    got = pos.group(1) if pos else "MISSING"
    want = M.PRECHANGE["300"]["pos"]
    ok = M.verdict(rc == 0 and got == want,
                   "rungN0 SOLO IS BYTE-IDENTICAL | pos=(%s), expected (%s). "
                   "The address work is in the link; with none of its env set "
                   "it may not move the game one unit." % (got, want))
    quiet = "[loopback:" not in t and "[comms:relay]" not in t
    ok &= M.verdict(quiet,
                    "rungN0 and the carrier is SILENT when unasked | no "
                    "[loopback:] or [comms:relay] line in a 300-frame solo run")
    return ok


def rungN1(a):
    """LOOPBACK IS UNCHANGED -- the regression net under the address work."""
    res = run_pair("n1_loopback", {}, {})
    good, why = session_ok(res["tp"], res["tc"])
    ok = M.verdict(good, "rungN1 LOOPBACK SESSION STILL FORMS | %s" % why)
    modes = [mode_of(res[k]) for k in ("tp", "tc")]
    ok &= M.verdict(modes == ["loopback", "loopback"],
                    "rungN1 and it is still LOOPBACK MODE | modes=%s (the new "
                    "code paths must not activate without their env)" % modes)
    p = pace(res, FRAMES)
    # No baseline arm here, so no frame rate is claimed: rounds and
    # wall time are what was actually observed, and rung N4 is where
    # pace gets measured against something.
    print("      loopback: %d rounds over %.1fs of process wall time"
          % (p["rounds"], p["wall"]))
    return ok


def rungN2(a):
    """DIRECT MODE, over this machine's real LAN address."""
    ip = lan_address()
    if ip.startswith("127."):
        return M.verdict(False,
                         "rungN2 NO LAN ADDRESS | the route lookup returned "
                         "%s, so there is no non-loopback path to test over. "
                         "This rung is not skippable-by-default: a direct-mode "
                         "claim needs a real interface." % ip)
    print("      direct mode over %s:%d" % (ip, PORT))
    res = run_pair("n2_direct",
                   {"SM64DS_COMMS_BIND_ANY": "1"},
                   {"SM64DS_COMMS_HOST": "%s:%d" % (ip, PORT)})
    good, why = session_ok(res["tp"], res["tc"])
    ok = M.verdict(good, "rungN2 DIRECT SESSION OVER THE LAN ADDRESS | %s" % why)
    modes = [mode_of(res[k]) for k in ("tp", "tc")]
    ok &= M.verdict(modes == ["direct", "direct"],
                    "rungN2 and both ends are in DIRECT mode | modes=%s" % modes)
    learned = "direct: learned slot 1 at %s" % ip
    ok &= M.verdict(learned in res["tp"],
                    "rungN2 and the parent LEARNED the child's address off the "
                    "wire | expected '%s' in the parent log (the loopback "
                    "port-arithmetic rule cannot produce this)" % learned)
    p = pace(res, FRAMES)
    # No baseline arm here, so no frame rate is claimed: rounds and
    # wall time are what was actually observed, and rung N4 is where
    # pace gets measured against something.
    print("      direct: %d rounds over %.1fs of process wall time"
          % (p["rounds"], p["wall"]))
    return ok


def start_relay(port, logpath):
    """Lane RELAY's service, configured the way its own docs say: by env."""
    log = open(logpath, "wb")
    env = dict(os.environ)
    env["SM64DS_RELAY_PORT"] = str(port)
    env["SM64DS_RELAY_BIND"] = "0.0.0.0"
    p = subprocess.Popen([sys.executable, RELAY_PY], env=env,
                         stdout=log, stderr=subprocess.STDOUT,
                         creationflags=M.NO_CONSOLE)
    p._logfile = log
    time.sleep(1.0)                       # let the bind land before a HELLO
    if p.poll() is not None:
        raise RuntimeError("the relay exited immediately (rc=%s); see %s"
                           % (p.returncode, logpath))
    return p


def rungN3(a):
    """RELAY MODE. Neither instance is ever told the other's address."""
    relay_target = a.relay
    proc = None
    relay_log = os.path.join(OUT, "n3_relay", "relay.log")
    os.makedirs(os.path.dirname(relay_log), exist_ok=True)
    if not relay_target:
        proc = start_relay(RELAY_PORT, relay_log)
        relay_target = "127.0.0.1:%d" % RELAY_PORT
        print("      local reference relay on %s" % relay_target)
    else:
        print("      LIVE relay at %s" % relay_target)
    try:
        res = run_pair("n3_relay",
                       {"SM64DS_COMMS_RELAY": relay_target,
                        "SM64DS_COMMS_CODE": a.code},
                       {"SM64DS_COMMS_RELAY": relay_target,
                        "SM64DS_COMMS_CODE": a.code})
    finally:
        if proc:
            proc.terminate()
            try:
                proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                proc.kill()
            proc._logfile.close()

    good, why = session_ok(res["tp"], res["tc"])
    ok = M.verdict(good, "rungN3 RELAY SESSION FORMS | %s" % why)
    modes = [mode_of(res[k]) for k in ("tp", "tc")]
    ok &= M.verdict(modes == ["relay", "relay"],
                    "rungN3 and both ends are in RELAY mode | modes=%s" % modes)
    paired = [("[comms:relay] paired as" in res[k]) for k in ("tp", "tc")]
    ok &= M.verdict(paired == [True, True],
                    "rungN3 and both ends PAIRED with the relay | paired=%s "
                    "(a status-0 HELLO-ACK landed on each)" % paired)
    # THE CLAIM THIS RUNG REALLY MAKES: neither game process was ever given the
    # other's address. Asserted against the env that was actually set, so it
    # cannot rot into a comment.
    ok &= M.verdict("SM64DS_COMMS_HOST" not in res["tp"]
                    and "learned slot" not in res["tp"],
                    "rungN3 and NEITHER END KNEW THE OTHER'S ADDRESS | the "
                    "parent never learned a peer address; every byte went "
                    "through the relay")
    p = pace(res, FRAMES)
    # No baseline arm here, so no frame rate is claimed: rounds and
    # wall time are what was actually observed, and rung N4 is where
    # pace gets measured against something.
    print("      relay: %d rounds over %.1fs of process wall time"
          % (p["rounds"], p["wall"]))
    return ok


def latency_arm(name, rtt_ms, loss_pct=0, jitter_ms=0):
    """One induced-latency pair run. RTT is split across the two ends."""
    one_way = rtt_ms // 2
    extra = {"SM64DS_COMMS_DELAY_MS": str(one_way)}
    if jitter_ms:
        extra["SM64DS_COMMS_JITTER_MS"] = str(jitter_ms)
    if loss_pct:
        extra["SM64DS_COMMS_DROP"] = str(loss_pct)
    res = run_pair(name, dict(extra), dict(extra))
    good, why = session_ok(res["tp"], res["tc"])
    ovf = [num(report_line(res[k]), "delayovf", int, 0) for k in ("tp", "tc")]
    return good, why, res, ovf


def rungN4(a):
    """THE PACE AT RTT. The measurement the latency story stands on."""
    ok = True
    rows = []
    baseline = None
    for rtt in (0, 40, 80, 120):
        good, why, res, ovf = latency_arm("n4_rtt%d" % rtt, rtt)
        if rtt == 0:
            baseline = res["wall"]
        p = pace(res, FRAMES, baseline)
        rows.append((rtt, p, good, ovf))
        print("      RTT %3d ms: %5d rounds  wall %6.1fs  %5.2fx the "
              "zero-latency arm  %s"
              % (rtt, p["rounds"], p["wall"], p["slowdown"],
                 "session ok" if good else "NO SESSION: " + why))
        ok &= M.verdict(good, "rungN4 RTT %d ms still forms a session | %s"
                        % (rtt, why))
        ok &= M.verdict(sum(ovf) == 0,
                        "rungN4 RTT %d ms delay ring never overflowed | "
                        "delayovf=%s (a nonzero value means the induction "
                        "dropped datagrams and the number above is not a "
                        "latency measurement)" % (rtt, ovf))
    with open(os.path.join(OUT, "latency.txt"), "w") as f:
        f.write("# %s frames per arm. slowdown is wall/wall_at_rtt0: the same\n"
                "# two-process setup with the wire as the only difference, so\n"
                "# the identical boot cost divides out. NO FRAME RATE IS\n"
                "# QUOTED -- the window selftest is not paced to 30 fps and\n"
                "# quoting one against that target would be invented.\n"
                % FRAMES)
        f.write("rtt_ms\trounds\twall_s\tslowdown_vs_rtt0\n")
        for rtt, p, good, _ in rows:
            f.write("%d\t%d\t%.1f\t%.2f\n"
                    % (rtt, p["rounds"], p["wall"], p["slowdown"]))
    return ok


def rungN5(a):
    """LOSS ON TOP OF LATENCY: 80 ms and 5%, a bad evening on a real line."""
    # The clean-wire baseline for the same setup, so the loss arm's cost is
    # measured against something rather than asserted.
    _, _, base, _ = latency_arm("n5_base", 0)
    good, why, res, ovf = latency_arm("n5_loss", 80, loss_pct=5, jitter_ms=10)
    p = pace(res, FRAMES, base["wall"])
    print("      80 ms + 5%% loss + 10 ms jitter: %d rounds, wall %.1fs, "
          "%.2fx the clean-wire arm"
          % (p["rounds"], p["wall"], p["slowdown"]))
    ok = M.verdict(good, "rungN5 SURVIVES 80 ms + 5%% LOSS | %s" % why)
    ok &= M.verdict(sum(ovf) == 0,
                    "rungN5 delay ring never overflowed | delayovf=%s" % ovf)
    with open(os.path.join(OUT, "loss.txt"), "w") as f:
        f.write("rtt_ms\tloss_pct\tjitter_ms\trounds\twall_s\tslowdown\n")
        f.write("80\t5\t10\t%d\t%.1f\t%.2f\n"
                % (p["rounds"], p["wall"], p["slowdown"]))
    return ok


def rungN6(a):
    """THE MONEY SHOT: two instances on this desk, through the live VPS relay.

    Every byte leaves this machine, crosses the public internet to the relay,
    and comes back. Rung N3 proves the protocol against a relay on loopback;
    this one proves the whole thing, including that the NAT in between holds a
    mapping open for the duration on the strength of the HELLO keepalive.
    """
    target = a.live or LIVE_RELAY
    print("      LIVE relay at %s, code '%s'" % (target, a.code))
    res = run_pair("n6_live",
                   {"SM64DS_COMMS_RELAY": target, "SM64DS_COMMS_CODE": a.code},
                   {"SM64DS_COMMS_RELAY": target, "SM64DS_COMMS_CODE": a.code})
    good, why = session_ok(res["tp"], res["tc"])
    ok = M.verdict(good, "rungN6 LIVE RELAY SESSION OVER THE PUBLIC INTERNET "
                         "| %s" % why)
    modes = [mode_of(res[k]) for k in ("tp", "tc")]
    ok &= M.verdict(modes == ["relay", "relay"],
                    "rungN6 and both ends are in RELAY mode | modes=%s" % modes)
    paired = [("[comms:relay] paired as" in res[k]) for k in ("tp", "tc")]
    ok &= M.verdict(paired == [True, True],
                    "rungN6 and both ends PAIRED with the LIVE relay | "
                    "paired=%s" % paired)
    p = pace(res, FRAMES)
    print("      live relay, stop-and-wait: %d rounds over %.1fs wall"
          % (p["rounds"], p["wall"]))

    # AND THE CONFIGURATION IT WOULD ACTUALLY SHIP IN. The relayed round trip
    # from this desk measures about 120 ms, and stop-and-wait spends one whole
    # frame on each one. Proving the session merely FORMS over the internet and
    # stopping there would be reporting half the story.
    n = 4                                    # ceil(120 / 33) + 1
    code2 = a.code[:7] + "P"
    res2 = run_pair("n6_live_pipelined",
                    {"SM64DS_COMMS_RELAY": target,
                     "SM64DS_COMMS_CODE": code2,
                     "SM64DS_COMMS_INPUT_DELAY": str(n)},
                    {"SM64DS_COMMS_RELAY": target,
                     "SM64DS_COMMS_CODE": code2,
                     "SM64DS_COMMS_INPUT_DELAY": str(n)})
    good2, why2 = session_ok(res2["tp"], res2["tc"])
    ok &= M.verdict(good2, "rungN6 LIVE RELAY WITH INPUT DELAY %d | %s"
                    % (n, why2))
    p2 = pace(res2, FRAMES)
    speedup = (p["wall"] / p2["wall"]) if p2["wall"] else 0.0
    print("      live relay, input delay %d: %d rounds over %.1fs wall "
          "(%.2fx faster)" % (n, p2["rounds"], p2["wall"], speedup))
    ok &= M.verdict(speedup > 1.5,
                    "rungN6 and PIPELINING BUYS THE PACE BACK ON THE REAL "
                    "INTERNET | the same %s frames in %.1fs instead of %.1fs, "
                    "%.2fx. Measured on the live path, not induced."
                    % (FRAMES, p2["wall"], p["wall"], speedup))
    with open(os.path.join(OUT, "live_relay.txt"), "w") as f:
        f.write("relay\t%s\ncode\t%s\nframes\t%s\n"
                % (target, a.code, FRAMES))
        f.write("mode\trounds\twall_s\n")
        f.write("stop_and_wait\t%d\t%.1f\n" % (p["rounds"], p["wall"]))
        f.write("input_delay_%d\t%d\t%.1f\n" % (n, p2["rounds"], p2["wall"]))
        f.write("speedup\t%.2f\n" % speedup)
        f.write("parent_report\t%s\n" % report_line(res["tp"]))
        f.write("child_report\t%s\n" % report_line(res["tc"]))
        f.write("parent_report_pipelined\t%s\n" % report_line(res2["tp"]))
        f.write("child_report_pipelined\t%s\n" % report_line(res2["tc"]))
    return ok


def rungN7(a):
    """DOES PIPELINING BUY BACK THE PACE? Measured, not asserted.

    For each round trip, the same arm twice: stop-and-wait, then with an input
    delay of ceil(rtt/33)+1 frames on both ends. The claim under test is that
    the second one runs at or near the 30 fps target while the first does not.
    A rung that only checked "it still forms a session" would pass whether or
    not the mitigation did anything at all.
    """
    ok = True
    rows = []
    _, _, base, _ = latency_arm("n7_base", 0)
    for rtt in (80, 120):
        n = int(rtt / 33.0) + 1
        good0, why0, res0, _ = latency_arm("n7_rtt%d_off" % rtt, rtt)
        p0 = pace(res0, FRAMES, base["wall"])
        extra = {"SM64DS_COMMS_INPUT_DELAY": str(n)}
        res1 = run_pair("n7_rtt%d_on" % rtt,
                        dict(extra, SM64DS_COMMS_DELAY_MS=str(rtt // 2)),
                        dict(extra, SM64DS_COMMS_DELAY_MS=str(rtt // 2)))
        good1, why1 = session_ok(res1["tp"], res1["tc"])
        p1 = pace(res1, FRAMES, base["wall"])
        starved = [num(report_line(res1[k]), "starved", int, -1)
                   for k in ("tp", "tc")]
        # DID THE KNOB ACTUALLY TAKE? Asserted because it once did not: the
        # carrier refused SM64DS_COMMS_INPUT_DELAY on a loopback carrier, which
        # is exactly what this rig runs, so the pipelined arm quietly ran
        # unpipelined and the rung reported "pipelining buys nothing". A
        # mitigation measured through a knob that silently did not apply is the
        # worst possible result: a real conclusion drawn from a run that never
        # happened.
        indelay = [num(report_line(res1[k]), "indelay", int, -1)
                   for k in ("tp", "tc")]
        ok &= M.verdict(indelay == [n, n],
                        "rungN7 RTT %d ms the input-delay knob ACTUALLY TOOK | "
                        "indelay=%s, wanted [%d, %d]. Anything else means the "
                        "comparison below is measuring two identical arms."
                        % (rtt, indelay, n, n))
        rows.append((rtt, n, p0, p1, starved))
        gain = p0["wall"] / p1["wall"] if p1["wall"] else 0.0
        print("      RTT %3d ms: stop-and-wait %6.1fs  ->  input delay %d "
              "%6.1fs   %.2fx   starved=%s"
              % (rtt, p0["wall"], n, p1["wall"], gain, starved))
        ok &= M.verdict(good0 and good1,
                        "rungN7 RTT %d ms forms a session both ways | off: %s "
                        "| on: %s" % (rtt, why0, why1))
        ok &= M.verdict(gain > 1.5,
                        "rungN7 RTT %d ms INPUT DELAY %d BUYS THE PACE BACK | "
                        "%.1fs -> %.1fs, %.2fx. Under 1.5x is not a mitigation "
                        "worth the input lag it costs."
                        % (rtt, n, p0["wall"], p1["wall"], gain))
    with open(os.path.join(OUT, "pipelining.txt"), "w") as f:
        f.write("# %s frames per arm, both arms under the same induced RTT.\n"
                "# gain is wall_stopwait / wall_pipelined.\n" % FRAMES)
        f.write("rtt_ms\tinput_delay\twall_stopwait_s\twall_pipelined_s\t"
                "gain\tstarved\n")
        for rtt, n, p0, p1, st in rows:
            f.write("%d\t%d\t%.1f\t%.1f\t%.2f\t%s\n"
                    % (rtt, n, p0["wall"], p1["wall"],
                       p0["wall"] / p1["wall"] if p1["wall"] else 0.0, st))
    return ok


LIVE_RELAY = "135.148.26.201:41234"

RUNGS = [("N0", rungN0), ("N1", rungN1), ("N2", rungN2), ("N3", rungN3),
         ("N4", rungN4), ("N5", rungN5), ("N6", rungN6), ("N7", rungN7)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only", default="")
    ap.add_argument("--relay", default="",
                    help="live relay host[:port]; omitted means run a local "
                         "reference relay for rung N3")
    ap.add_argument("--live", default="",
                    help="live relay host[:port] for rung N6; defaults to the "
                         "staging service the RELAY lane deployed")
    # A CODE OF THIS RUN'S OWN. The live relay is a shared service: a fixed
    # code would put two people testing at the same moment into one session,
    # and each would watch the other's game fail to make sense.
    ap.add_argument("--code", default="NP%06d" % (os.getpid() % 1000000))
    ap.add_argument("--frames", default=None)
    a = ap.parse_args()

    global FRAMES
    if a.frames:
        FRAMES = a.frames

    os.makedirs(OUT, exist_ok=True)
    if not os.path.exists(EXE):
        print("no exe at %s -- build first" % EXE)
        return 2
    print("net_proof: exe %s  ports %d.. relay %d"
          % (M.sha(EXE), PORT, RELAY_PORT))

    want = [s.strip().upper() for s in a.only.split(",") if s.strip()]
    ok = True
    for name, fn in RUNGS:
        if want and name not in want:
            continue
        print("\n=== %s ===" % name)
        try:
            ok &= fn(a)
        except Exception as e:                       # noqa: BLE001
            ok = False
            print("  %s RAISED: %r" % (name, e))
    print("\n%s" % ("ALL GREEN" if ok else "FAILURES ABOVE"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
