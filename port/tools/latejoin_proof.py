#!/usr/bin/env python3
"""LATE JOIN: does every seated peer, including the late one, run ONE depth?

    python port/tools/latejoin_proof.py [--only A,B,...] [--out DIR]

The invariant the adaptive input delay must not weaken is that every console
in a session reads round k-N for frame k with the SAME N. A late joiner is the
one way a second number can enter a session after the parent has sized it,
and it can arrive in three different states of the session:

  BEFORE THE FREEZE   the parent has sized (or is about to) and the ROM has
                      not yet asked for round 0. A joiner here is seated, and
                      the depth it is handed has to be the one the session
                      will actually run. Commit 99c86bba2 moved the sizing in
                      front of the joiner's accept for exactly this window.
  AFTER THE FREEZE,   the session runs a depth every build can adopt (<= 8).
  SHALLOW             A joiner here is seated and handed the frozen number.
  AFTER THE FREEZE,   the session runs a depth an old build would DROP. A
  DEEP                joiner here is REFUSED a seat rather than handed a
                      number it might silently not run (rule 5 in
                      status/LAGDELAY.md).

and it can be one of two builds: a NEW build that reports its round trip and
adopts any published depth, or an OLD build (SM64DS_COMMS_LEGACY_PEER), which
never reports, drops a depth past 8, and reads the field on its first accept
only. The arms below cover the matrix that matters:

  A  OLD build, late, BEFORE the freeze, with its own SM64DS_COMMS_INPUT_DELAY
     set to a number that is NOT the mode default. This is the arm that tells
     the two orderings apart: sizing after the accept hands the joiner the
     raised number, its clamp drops it, and it keeps its own 7 while the
     others fall back to 5. Sizing before the accept hands it the withdrawn 5.
  B  NEW build, late, BEFORE the freeze. The parent must size from BOTH
     reports and every peer, the late one included, must close on the sized
     number.
  C  OLD build, late, AFTER the freeze, session at a depth <= 8. Seated, and
     it closes on the frozen number.
  D  NEW build, late, AFTER the freeze, session deeper than 8. Refused, out
     loud, and the running pair is undisturbed. This is gap 10 measured.
  E  OLD build present from the start (the original ARM 1): the sizing stands
     down and all three run the mode default.
  F  OLD build, late, AFTER the freeze, deep (the original ARM 2): refused.

For every arm the verdict is the same sentence: every peer the parent SEATED
completed at least 200 rounds and closed on one indelay, and every peer it did
not seat was refused out loud and completed no round. The "late" arms also
prove the joiner was late in the way the arm claims, from the ORDER of the
parent's own log lines, so an arm whose timing missed its window says so
instead of passing on a different case.

RELAY mode over the local reference relay with an induced one-way delay in the
carrier, because relay mode is the one with the problem and the sizing is
armed there only. Quiet and muted through mp2_proof.env_base/spawn.
"""

import argparse
import os
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M            # noqa: E402
import net_proof as N            # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "runs", "vsdec", "out", "LATEJOIN")
# ONE BASE FOR EVERY INSTANCE of an arm: the carrier derives each peer's port
# from base + slot itself. Inside this pid's own bucket, clear of mp2/mp3/
# mp_stall (+0..+63) and net_proof (+80, relay +112).
PORT = M.PORT_BASE + 16
RELAY0 = M.PORT_BASE + 40

CLOSE = re.compile(r"^\[comms:loopback\] closed after (\d+) rounds; indelay=(-?\d+)", re.M)


def close_line(t):
    m = re.search(r"^\[comms:loopback\] closed after .*$", t, re.M)
    return m.group(0) if m else "(no close line)"


def rounds_and_delay(t):
    m = CLOSE.search(t)
    return (int(m.group(1)), int(m.group(2))) if m else (0, -1)


def grep(t, pat):
    return [m.group(0) for m in re.finditer(pat, t, re.M)]


def first_index(t, pat):
    m = re.search(pat, t, re.M)
    return m.start() if m else None


def launch(arm_dir, name, relay, code, role, slot, frames, oneway, extra):
    run = os.path.join(arm_dir, name)
    os.makedirs(os.path.join(run, "tmp"), exist_ok=True)
    e = M.env_base(ROOT, run, "lj_%s_%s" % (os.path.basename(arm_dir), name))
    e["SM64DS_LEVEL"] = "1"
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_COMMS_PORT"] = str(PORT)
    e["SM64DS_COMMS_ROLE"] = role
    e["SM64DS_COMMS_SLOT"] = str(slot)
    e["SM64DS_COMMS_RELAY"] = relay
    e["SM64DS_COMMS_CODE"] = code
    e["SM64DS_COMMS_DELAY_MS"] = str(oneway)
    e["TEMP"] = e["TMP"] = os.path.join(run, "tmp")
    e.update(extra)
    log = os.path.join(arm_dir, name + ".log")
    return M.spawn(EXE, run, e, log), log


ARMS = {
    # name: (one-way ms, late offset s after child_new, late peer env,
    #        parent frames, child_new frames, late frames, expectation)
    "A": (50, 0.9, {"SM64DS_COMMS_LEGACY_PEER": "1",
                    "SM64DS_COMMS_INPUT_DELAY": "7"}, 1500, 1200, 900,
          "prefreeze_withdrawn"),
    "B": (50, 0.9, {}, 1500, 1200, 900, "prefreeze_sized"),
    "C": (35, 10.0, {"SM64DS_COMMS_LEGACY_PEER": "1"}, 3000, 2400, 900,
          "postfreeze_seated_shallow"),
    "D": (50, 10.0, {}, 3000, 2400, 900, "postfreeze_refused"),
    "E": (50, 0.0, {"SM64DS_COMMS_LEGACY_PEER": "1"}, 1500, 1200, 900,
          "together_default"),
    "F": (50, 10.0, {"SM64DS_COMMS_LEGACY_PEER": "1"}, 3000, 2400, 900,
          "postfreeze_refused"),
}
ORDER = "ABCDEF"

WHAT = {
    "A": "OLD build joins late BEFORE the freeze with its own delay 7: sized, then withdrawn, and the joiner is handed the withdrawn value",
    "B": "NEW build joins late BEFORE the freeze: sized from both reports, every peer on the sized number",
    "C": "OLD build joins late AFTER the freeze at a depth <= 8: seated on the frozen number",
    "D": "NEW build joins late AFTER the freeze at a depth > 8: refused, running pair undisturbed",
    "E": "OLD build present from the start: the sizing stands down, all three on the mode default",
    "F": "OLD build joins late AFTER the freeze at a depth > 8: refused (the original arm 2)",
}


def run_arm(letter, idx, out):
    oneway, late_s, late_env, pf, cf, lf, expect = ARMS[letter]
    arm_dir = os.path.join(out, "arm" + letter)
    os.makedirs(arm_dir, exist_ok=True)
    relay_port = RELAY0 + 2 * idx
    code = "LJ%s%05d" % (letter, os.getpid() % 99999)
    relay = N.start_relay(relay_port, os.path.join(arm_dir, "relay.log"))
    target = "127.0.0.1:%d" % relay_port
    procs = []
    try:
        p, lp = launch(arm_dir, "parent", target, code, "parent", 0, pf, oneway, {})
        procs.append(p)
        time.sleep(0.6)
        c1, lc1 = launch(arm_dir, "child_new", target, code, "child", 1, cf, oneway, {})
        procs.append(c1)
        if late_s:
            time.sleep(late_s)
        c2, lc2 = launch(arm_dir, "child_late", target, code, "child", 2, lf, oneway, late_env)
        procs.append(c2)
        rcs = [M.finish(x, 900) for x in procs]
    finally:
        relay.kill()
        relay._logfile.close()
    return rcs, [M.text(x) for x in (lp, lc1, lc2)]


def judge(letter, rcs, tp, t1, t2):
    _, _, late_env, _, _, _, expect = ARMS[letter]
    print("  rc parent/new/late =", rcs)
    for nm, t in (("parent", tp), ("child_new", t1), ("child_late", t2)):
        print("  %-10s %s" % (nm, close_line(t)))
    for l in grep(tp, r"^\[comms:loopback\] (adaptive input delay \d|a live peer "
                      r"has not reported|not every peer reported|input delay "
                      r"FROZEN|holding round|every peer has confirmed|REFUSED a "
                      r"late join|slot 2 joined).*$")[:10]:
        print("  P| " + l[:230])
    for l in grep(t2, r"^\[comms:loopback\] (accepted as slot|the parent runs "
                      r"input delay|SM64DS_COMMS_LEGACY_PEER).*$")[:4]:
        print("  L| " + l[:230])

    (rp, dp), (r1, d1), (r2, d2) = (rounds_and_delay(t) for t in (tp, t1, t2))
    seated_late = bool(grep(tp, r"slot 2 joined at round"))
    refused_late = bool(grep(tp, r"REFUSED a late join from slot 2"))
    # WHEN THE JOIN LANDED, from the parent's own line order. The freeze marker
    # is the FROZEN line when the sizing was still armed, and the stand-down
    # line when it was not (the freeze itself is silent after a stand-down).
    j = first_index(tp, r"^\[comms:loopback\] slot 2 joined at round")
    f = first_index(tp, r"^\[comms:loopback\] (input delay FROZEN|not every peer reported)")
    before_freeze = (j is not None and f is not None and j < f)
    sized = grep(tp, r"^\[comms:loopback\] adaptive input delay \d+ frame")
    withdrawn = grep(tp, r"WITHDRAWN|stands down")

    ok = True
    def need(cond, what):
        nonlocal ok
        print("  %s  %s" % ("ok  " if cond else "FAIL", what))
        ok &= bool(cond)

    # THE INVARIANT, the same in every arm.
    need(rp >= 200 and r1 >= 200, "parent and child_new both completed >= 200 rounds (%d/%d): a session that never ran must not read as agreement" % (rp, r1))
    need(dp == d1 and dp >= 0, "parent and child_new closed on one indelay (%d/%d)" % (dp, d1))
    if seated_late:
        need(r2 >= 200, "the late peer was seated and completed >= 200 rounds (%d)" % r2)
        need(d2 == dp, "the late peer closed on the session's indelay (%d vs %d)" % (d2, dp))
    else:
        need(refused_late, "the late peer was not seated, so the parent must have refused it out loud")
        need(r2 == 0, "an unseated peer completed no round (%d)" % r2)

    # THE ARM'S OWN CLAIM about when the joiner landed and what followed.
    if expect == "prefreeze_withdrawn":
        need(before_freeze, "the late join landed BEFORE the freeze (parent line order)")
        need(seated_late, "a pre-freeze joiner is seated")
        need(bool(sized) and bool(withdrawn), "the parent sized (%d line) and then withdrew/stood down (%d line)" % (len(sized), len(withdrawn)))
        need(dp == 5, "everyone fell back to the mode default 5 (got %d)" % dp)
        need(bool(grep(t2, r"ADOPTING 5")), "the old-build joiner adopted 5 off its FIRST accept, leaving its own 7 (the ordering this arm exists to prove)")
    elif expect == "prefreeze_sized":
        need(before_freeze, "the late join landed BEFORE the freeze (parent line order)")
        need(seated_late, "a pre-freeze joiner is seated")
        need(dp > 5, "the session ran a sized depth above the default (got %d)" % dp)
        need(bool(grep(t2, r"ADOPTING %d" % dp)), "the late joiner adopted the sized number %d" % dp)
        need(bool(grep(tp, r"per-child rtt s1=\d+ms s2=\d+ms")), "the parent sized from BOTH children's reports")
    elif expect == "postfreeze_seated_shallow":
        need(not before_freeze, "the late join landed AFTER the freeze (parent line order)")
        need(seated_late, "a post-freeze joiner at a depth <= 8 is seated")
        need(5 < dp <= 8, "the frozen depth is above the default and within every build's clamp (got %d)" % dp)
        need(bool(grep(t2, r"ADOPTING %d" % dp)), "the old-build joiner adopted the frozen number %d off its first accept" % dp)
    elif expect == "postfreeze_refused":
        need(not before_freeze if j is not None else True, "the late join did not land before the freeze")
        need(dp > 8, "the session froze deeper than 8 (got %d)" % dp)
        need(refused_late and not seated_late, "the late joiner was refused and never seated")
    elif expect == "together_default":
        need(seated_late, "the old build present from the start is seated")
        need(dp == 5, "the session ran the mode default (got %d)" % dp)
        need(bool(withdrawn), "the sizing stood down or withdrew")
    print("  ARM %s %s: %s" % (letter, "PASS" if ok else "FAIL", WHAT[letter]))
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only", default="")
    ap.add_argument("--out", default=OUT)
    a = ap.parse_args()
    if not os.path.exists(EXE):
        print("no exe at", EXE)
        return 2
    os.makedirs(a.out, exist_ok=True)
    want = [s.strip().upper() for s in a.only.split(",") if s.strip()]
    print("late-join proof: exe %s  sha %s  ports %d.. relays %d.."
          % (EXE, M.sha(EXE), PORT, RELAY0))
    ok = True
    for idx, letter in enumerate(ORDER):
        if want and letter not in want:
            continue
        oneway, late_s, late_env, pf, cf, lf, _ = ARMS[letter]
        print("\n=== ARM %s: %s ===" % (letter, WHAT[letter]))
        print("  one-way %d ms induced each end; late peer %s starts %.1fs after child_new; frames parent/new/late = %d/%d/%d"
              % (oneway, "OLD build" if "SM64DS_COMMS_LEGACY_PEER" in late_env else "NEW build", late_s, pf, cf, lf))
        try:
            rcs, (tp, t1, t2) = run_arm(letter, idx, a.out)
            ok &= judge(letter, rcs, tp, t1, t2)
        except Exception as e:                       # noqa: BLE001
            ok = False
            print("  ARM %s RAISED: %r" % (letter, e))
    print("\nlate-join proof:", "ALL GREEN" if ok else "FAILURES ABOVE")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
