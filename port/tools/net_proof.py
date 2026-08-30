#!/usr/bin/env python3
"""THE ONLINE LADDER: does two-player actually work over a real network?

    python port/tools/net_proof.py [--only N0,N2,...] [--seconds N]
                                   [--relay HOST[:PORT]] [--code CODE]
                                   [--frames N]

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
      lockstep costs at 0/40/80/120 ms round trip, measured as frames advanced
      per wall second against the 30 fps target.
  N5  LOSS ON TOP OF LATENCY. 80 ms and 5% loss together, which is a bad
      evening on a real connection, with the pace measured the same way.

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

FRAMES = "600"          # 20 s at the 30 fps target; long enough for a session
                        # to form, settle, and be measured after it settles


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
    """The carrier's own report line, which is where every counter lives."""
    got = [m.group(0) for m in re.finditer(r"^\[loopback:[^\]]*\].*$", t, re.M)]
    if not got:
        return ""
    return got[-1] if which == "last" else got[0]


def num(line, key, cast=int, default=None):
    m = re.search(r"\b%s=([^\s]+)" % re.escape(key), line)
    if not m:
        return default
    try:
        return cast(m.group(1))
    except ValueError:
        return default


def session_ok(tp, tc):
    """Both sides agree a two-player session formed and rounds advanced."""
    rp, rc = report_line(tp), report_line(tc)
    if not rp or not rc:
        return False, "no [loopback:] report line (p=%r c=%r)" % (bool(rp), bool(rc))
    lp, lc = num(rp, "live", lambda s: int(s, 16), 0), num(rc, "live", lambda s: int(s, 16), 0)
    rounds_p, rounds_c = num(rp, "round", int, 0), num(rc, "round", int, 0)
    if lp != 3 or lc != 3:
        return False, "live masks %s/%s, wanted 0x3 both" % (hex(lp), hex(lc))
    if rounds_p < 30 or rounds_c < 30:
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


TARGET_FPS = 30.0


def pace(res, frames, baseline_wall=None):
    """What the session actually ran at, with boot time taken back out.

    THE NAIVE VERSION OF THIS IS WRONG AND THE WRONG NUMBER LOOKS PLAUSIBLE.
    Total wall time for a fixed-frame run is boot (asset loading, several
    seconds, and the same in every arm) PLUS the paced frame loop. Dividing
    frames by total wall would charge the boot to the network and report, say,
    17 fps for a session that is really running at 30 -- and it would do it
    consistently enough to look like a measurement.

    So the arms are compared against each other. `baseline_wall` is the wall
    time of the SAME two-process setup with zero induced delay: same exe, same
    assets, same boot, same everything but the wire. The difference between an
    arm and that baseline is therefore entirely the latency cost, and the
    in-session frame rate is

        frames / (frames/30 + (wall_arm - wall_baseline))

    With no baseline given, only the raw numbers come back and `fps` is None --
    an absent measurement, not a guessed one.

    The child's round counter is the honest one to read: a parent completes a
    round the instant its own block is staged if it already holds the child's,
    so a parent alone can look fast while the session as a whole crawls.
    """
    wall = res["wall"]
    rounds = num(report_line(res["tc"]), "round", int, 0)
    n = int(frames)
    out = dict(rounds=rounds, wall=wall, fps=None, rps=None, extra=None)
    if baseline_wall is not None:
        extra = max(wall - baseline_wall, 0.0)
        span = n / TARGET_FPS + extra
        out["extra"] = extra
        out["fps"] = n / span
        out["rps"] = rounds / span
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
    modes = [num(report_line(res[k]), "mode", str, "?") for k in ("tp", "tc")]
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
    modes = [num(report_line(res[k]), "mode", str, "?") for k in ("tp", "tc")]
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
    modes = [num(report_line(res[k]), "mode", str, "?") for k in ("tp", "tc")]
    ok &= M.verdict(modes == ["relay", "relay"],
                    "rungN3 and both ends are in RELAY mode | modes=%s" % modes)
    paired = [num(report_line(res[k]), "paired", int, 0) for k in ("tp", "tc")]
    ok &= M.verdict(paired == [1, 1],
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
        print("      RTT %3d ms: %5.1f fps  %5d rounds  %5.1f rounds/s  "
              "wall %5.1fs (+%.1fs)  %s"
              % (rtt, p["fps"], p["rounds"], p["rps"], p["wall"], p["extra"],
                 "session ok" if good else "NO SESSION: " + why))
        ok &= M.verdict(good, "rungN4 RTT %d ms still forms a session | %s"
                        % (rtt, why))
        ok &= M.verdict(sum(ovf) == 0,
                        "rungN4 RTT %d ms delay ring never overflowed | "
                        "delayovf=%s (a nonzero value means the induction "
                        "dropped datagrams and the number above is not a "
                        "latency measurement)" % (rtt, ovf))
    with open(os.path.join(OUT, "latency.txt"), "w") as f:
        f.write("# frames=%s target=%.0f fps. fps and rounds_per_s are over\n"
                "# frames/30 + (wall - wall_at_rtt0); the rtt=0 arm is the\n"
                "# baseline so boot time is not charged to the network.\n"
                % (FRAMES, TARGET_FPS))
        f.write("rtt_ms\tfps\trounds\trounds_per_s\twall_s\textra_s\n")
        for rtt, p, good, _ in rows:
            f.write("%d\t%.2f\t%d\t%.2f\t%.1f\t%.1f\n"
                    % (rtt, p["fps"], p["rounds"], p["rps"], p["wall"],
                       p["extra"]))
    return ok


def rungN5(a):
    """LOSS ON TOP OF LATENCY: 80 ms and 5%, a bad evening on a real line."""
    # The clean-wire baseline for the same setup, so the loss arm's cost is
    # measured against something rather than asserted.
    _, _, base, _ = latency_arm("n5_base", 0)
    good, why, res, ovf = latency_arm("n5_loss", 80, loss_pct=5, jitter_ms=10)
    p = pace(res, FRAMES, base["wall"])
    print("      80 ms + 5%% loss + 10 ms jitter: %.1f fps, %d rounds, "
          "%.1f rounds/s, wall %.1fs (+%.1fs)"
          % (p["fps"], p["rounds"], p["rps"], p["wall"], p["extra"]))
    ok = M.verdict(good, "rungN5 SURVIVES 80 ms + 5%% LOSS | %s" % why)
    ok &= M.verdict(sum(ovf) == 0,
                    "rungN5 delay ring never overflowed | delayovf=%s" % ovf)
    with open(os.path.join(OUT, "loss.txt"), "w") as f:
        f.write("rtt_ms\tloss_pct\tjitter_ms\tfps\trounds\trounds_per_s\twall_s\n")
        f.write("80\t5\t10\t%.2f\t%d\t%.2f\t%.1f\n"
                % (p["fps"], p["rounds"], p["rps"], p["wall"]))
    return ok


RUNGS = [("N0", rungN0), ("N1", rungN1), ("N2", rungN2), ("N3", rungN3),
         ("N4", rungN4), ("N5", rungN5)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only", default="")
    ap.add_argument("--relay", default="",
                    help="live relay host[:port]; omitted means run a local "
                         "reference relay for rung N3")
    ap.add_argument("--code", default="NETPROOF")
    ap.add_argument("--frames", default=FRAMES)
    a = ap.parse_args()

    global FRAMES
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
