#!/usr/bin/env python3
"""Voice chat changes the sound and NOTHING ELSE. Lane VOICE.

TWO CLAIMS, AND THE SECOND ONE IS THE STRONG ONE.

  1. A wide session with voice on all windows still reports NO DIVERGENCE
     between every pair of windows. That is the standing bar for any change
     that touches the comms path.

  2. The world hashes a window produces with voice ON are FRAME-FOR-FRAME
     IDENTICAL to the ones the same window produces with voice OFF. Claim 1
     alone can be satisfied by a change that moved every window the same wrong
     way; this one cannot. It is the same cross-run hash comparison the VS16
     host-loop lane used, run here between two configurations rather than two
     builds.

Both passes hold one direction per window (SM64DS_COMMS_INJECT) so the bodies
actually move and the hashes have something to be about, and both are driven
from the same fixed per-slot key table, which is what makes the two runs
comparable at all.

Quiet and muted through mp2_proof.spawn, plus SM64DS_NO_AUDIO=1 and the tone
hook on every window, so no recording device and no output device is opened by
any of them.

--REAL-MIC DROPS THE TONE HOOK, and it exists for one thing the default run
cannot reach: the AUTO-PICK SCAN. With the tone hook on, no capture device is
opened at all, so the scan that hal/voice_capture_win.cpp runs on the first
cap_open -- opening every recording device in turn and sampling it, a couple
hundred milliseconds of wall clock on the host frame loop -- never happens, and
claim 2 above is silent about it. With --real-mic the ON pass opens the real
hardware and really scans, and the same frame-for-frame comparison against the
voice-OFF pass is then a statement about the scan too.

It is a smaller claim per window on purpose. A machine has one microphone and
several windows, so with real hardware they contend for it and only some of
them capture; what is asserted per window is that voice was ON and the tone hook
was OFF, plus that AT LEAST ONE window really captured. The hash comparison,
which is the point, is unchanged and is over every window either way.

    python port/tools/voice_determinism.py [--windows N] [--frames N]
                                           [--real-mic]

Exit 0 all green, 1 on the first red.
"""
# THE WINDOWED TEST SLOT. walk_window.exe opens a real OS window and drives the
# port's single windowed render/input path, so two of them on one machine
# collide and throw a random rc=1 on a DIFFERENT level or scene each run --
# reds that come back clean when repeated in isolation. This harness launches
# walk_window, so it takes the machine-wide slot lock for the WHOLE run.
#
# THE WHOLE RUN, not each launch, and that is the load-bearing part: a VS pass
# here needs several windows ALIVE AT ONCE, so a per-launch lock would have
# window 0 holding the slot while it waits for window 1 to join and never let
# go. The unit of exclusion is the pass, not the process.

import argparse
import json
import os
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M
import slot_lock

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
OUT = os.path.join(ROOT, "runs", "voice", "out", "det")
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
DHDIFF = os.path.join(HERE, "dhdiff.py")

PORT = M.PORT_BASE + 64
VS_MAP = "0"

# One held direction per window, so every body walks a different way. The same
# table in both passes: two runs that pressed different buttons could not be
# compared frame for frame and the second claim would be empty.
KEYS = ["0x0040", "0x0080", "0x0020", "0x0010"]


def settings(d, on):
    cfg = {
        "VoiceEnabled": bool(on),
        "VoiceMicDevice": "",
        "VoiceVolume": 100,
        # Radii wide enough that every window is inside the near radius, so
        # the voice path is running at FULL tilt for the whole pass. Proving
        # determinism with the falloff holding everything at zero would prove
        # nothing about the loud case.
        "VoiceNearRadius": 100000,
        "VoiceFarRadius": 200000,
    }
    with open(os.path.join(d, "settings.json"), "w", encoding="utf-8") as f:
        json.dump(cfg, f, indent=2)


def run_pass(tag, n, frames, port, voice_on, real_mic=False):
    """N windows, one session, one arena. Returns the per-window log paths."""
    base = os.path.join(OUT, tag)
    dirs, procs, logs = [], [], []
    # The parent outlives everyone: at five windows and up, a parent given the
    # children's budget spends it WAITING for the last joiner and sends BYE
    # while the last window is still booting, which reads exactly like a
    # desync and is not one. Same allowance vs16_ladder.sh makes.
    pbudget = str(int(frames) + 60 * (n + 8))
    for k in range(n):
        d = os.path.join(base, "p%d" % k)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        settings(d, voice_on)
        dirs.append(d)
    for k in range(n):
        e = M.env_base(ROOT, dirs[k], "%s_p%d" % (tag, k))
        e.pop("SM64DS_LEVEL", None)
        e.pop("SM64DS_SCENE", None)
        e["SM64DS_VS_MAP"] = VS_MAP
        e["SM64DS_VS_PLAYERS"] = str(n)
        e["SM64DS_VS_STATE_HASH"] = "2"
        e["SM64DS_COMMS_PORT"] = str(port)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_COMMS_INJECT"] = "key=%s" % KEYS[k % len(KEYS)]
        e["SM64DS_NO_AUDIO"] = "1"
        # The tone hook REPLACES the capture device, so it is exactly what has
        # to come off for the auto-pick scan to run. Off only on the ON pass:
        # the OFF pass opens nothing whatever this says, and leaving the hook on
        # there keeps the two passes' environments as close as they can be.
        if not (real_mic and voice_on):
            e["SM64DS_VOICE_TEST_TONE"] = "1"
        e["SM64DS_VOICE_REPORT"] = "1"
        if k == 0:
            e["SM64DS_COMMS_ROLE"] = "parent"
            e["SM64DS_WINDOW_SELFTEST"] = pbudget
        else:
            e["SM64DS_COMMS_ROLE"] = "child"
            e["SM64DS_COMMS_SLOT"] = str(k)
            e["SM64DS_WINDOW_SELFTEST"] = frames
        log = os.path.join(dirs[k], "run.log")
        logs.append(log)
        procs.append(M.spawn(EXE, dirs[k], e, log))
        if k == 0:
            time.sleep(1.0)
    rcs = [M.finish(p, 1200) for p in procs]
    return logs, rcs


def dhdiff(a, b):
    """-> (rc, first line of output). rc 0 agree, 1 diverge, 2 nothing to
    compare."""
    p = subprocess.run([sys.executable, DHDIFF, a, b],
                       capture_output=True, text=True)
    lines = [x for x in (p.stdout or p.stderr or "").strip().splitlines()
             if x.strip()]
    # THE VERDICT IS THE LAST LINE, not the first. dhdiff opens with a file
    # census and closes with "NO DIVERGENCE ... for all N common frames" or the
    # first differing frame, and quoting the census made every verdict here read
    # as a header rather than an answer.
    return p.returncode, (lines[-1] if lines else "(no output)")



# ---------------------------------------------------------------------------
# THE CROSS-RUN COMPARISON, KEYED BY COMMS ROUND
#
# dhdiff compares two logs at the same FRAME INDEX, and refuses outright when
# the rounds= columns disagree, because at a drifted index it would be
# comparing two different game moments and manufacturing a divergence. That
# refusal is right for the case it was written for (two windows of one live
# session) and it is the wrong tool for the CROSS-RUN pair, which is the same
# window in two SEPARATE runs:
#
#   the parent window boots first and then waits for its peer to join, and how
#   many host frames it spends inside one comms round is wall clock. Measured
#   here on a loaded machine: the voice-OFF parent spent TWO host frames on
#   rounds 521, 522 and 523 where the voice-ON parent spent one. Three frames
#   of skew, and from then on every frame index in one run names a different
#   moment in the other, so dhdiff refused 598 of 1200 frames.
#
# Note WHICH run had the extra frames: the OFF one, which has no voice code in
# it at all. The skew is the machine, not the feature -- but frame-index
# comparison cannot say that, and a harness that goes red on the machine being
# busy is a harness people learn to ignore.
#
# So the cross-run claim is made on the ROUND instead. rounds= is the exchanged
# comms counter, the one both consoles agree on, and the END-OF-ROUND world
# hash is the state the simulation actually reached in that round -- the number
# of host frames it took to get there is not simulation state. Comparing the
# last hash of each shared round is therefore the same claim, stated on an axis
# the wall clock cannot move. Where there is no skew at all the two comparisons
# are identical, and dhdiff is still run and still believed for every pair that
# it can align.
#
# The final round is dropped: once the session ends the round counter stops
# advancing while frames keep being hashed, so its "last frame" is an arbitrary
# point in a tail of different lengths.
DH_LINE = re.compile(r'^\[dh\] f(\d+) n=(\d+) w=([0-9a-f]+) o=([0-9a-f]+)'
                     r' rounds=(\d+)')


def end_of_round_hashes(path):
    """{comms round -> the world/other hash pair as of that round's LAST
    hashed frame}, plus how many frames each round took."""
    last, frames = {}, {}
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for line in f:
            m = DH_LINE.match(line.strip())
            if not m:
                continue
            rd = int(m.group(5))
            last[rd] = (m.group(3), m.group(4))
            frames[rd] = frames.get(rd, 0) + 1
    return last, frames


def cross_run_by_round(a, b):
    """-> (ok, message). Compares the END-OF-ROUND world hash of two runs of
    the same window over every comms round they share."""
    la, fa = end_of_round_hashes(a)
    lb, fb = end_of_round_hashes(b)
    if not la or not lb:
        return False, ("no rounds= data in at least one log, so the round-keyed "
                       "comparison could not run at all")
    tail = min(max(la), max(lb))
    shared = [r for r in sorted(set(la) & set(lb)) if r < tail]
    if not shared:
        return False, "the two runs share no completed comms round"
    bad = [r for r in shared if la[r] != lb[r]]
    skew = sorted(r for r in shared if fa[r] != fb[r])
    if bad:
        return False, ("the end-of-round world hash DIFFERS on %d of %d shared "
                       "rounds, first at round %d" % (len(bad), len(shared), bad[0]))
    return True, ("IDENTICAL end-of-round world hash on all %d shared comms "
                  "rounds (1..%d)%s" % (len(shared), shared[-1],
                  "" if not skew else
                  "; %d round(s) took a different number of HOST FRAMES in the "
                  "two runs (%s) -- wall clock, not state, and the reason the "
                  "frame-index comparison cannot be used here"
                  % (len(skew), ",".join(str(r) for r in skew[:6]))))



def _selftest():
    """The round-keyed comparison, on fixtures, including the case that matters.

    A CHECKER THAT STOPS REFUSING IS A SILENT HAZARD. This one exists to let a
    cross-run claim be made on a machine whose wall clock skewed the frame
    index, so the failure mode to guard against is not "it goes red", it is "it
    goes green on two runs that really did diverge". Every arm below is about
    that. Runs on synthetic logs, needs no game and no recording device.
    """
    import tempfile

    def write(rows):
        # rows: (frame, round, world hash)
        fd, path = tempfile.mkstemp(suffix=".log", text=True)
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            for fr, rd, w in rows:
                f.write("[dh] f%d n=3 w=%s o=aaaaaaaa rounds=%d\n" % (fr, w, rd))
        return path

    fails = []

    def check(name, ok, detail=""):
        print(("ok    " if ok else "FAIL  ") + name + ("" if ok else "   <- " + detail))
        if not ok:
            fails.append(name)

    # One frame per round, identical: the no-skew case, and the tail round is
    # dropped so the comparison is over rounds 1..3.
    a = write([(i, i, "0000000%d" % i) for i in range(1, 5)])
    b = write([(i, i, "0000000%d" % i) for i in range(1, 5)])
    ok, msg = cross_run_by_round(a, b)
    check("identical runs agree", ok and "all 3 shared" in msg, msg)

    # THE CASE THIS EXISTS FOR. One run spent two host frames on round 2. The
    # END of round 2 is the same state in both, so this must still agree -- and
    # must SAY that the frame counts differed rather than hiding it.
    c = write([(1, 1, "00000001"), (2, 2, "0000ffff"), (3, 2, "00000002"),
               (4, 3, "00000003"), (5, 4, "00000004")])
    ok, msg = cross_run_by_round(a, c)
    check("a round that took an extra HOST FRAME still agrees", ok, msg)
    check("and the extra frame is reported, not hidden", "HOST FRAMES" in msg, msg)

    # THE ARM THAT MATTERS. One world hash differs at the end of a round. If
    # this passes, every green above means nothing.
    d = write([(1, 1, "00000001"), (2, 2, "0000dead"), (3, 3, "00000003"), (4, 4, "00000004")])
    ok, msg = cross_run_by_round(a, d)
    check("a changed end-of-round world hash is caught", not ok, msg)
    check("and it names the round", "round 2" in msg, msg)

    # A divergence hidden UNDER a skewed round: the extra frame is the one that
    # matches, the end-of-round frame is the one that does not. Keying on the
    # last frame is what makes this visible; keying on the first would miss it.
    e = write([(1, 1, "00000001"), (2, 2, "00000002"), (3, 2, "0000dead"),
               (4, 3, "00000003"), (5, 4, "00000004")])
    ok, msg = cross_run_by_round(a, e)
    check("a divergence hidden under a skewed round is caught", not ok, msg)

    # No rounds= at all -> refuse, never silently pass.
    fd, bare = tempfile.mkstemp(suffix=".log", text=True)
    with os.fdopen(fd, "w", encoding="utf-8") as f:
        f.write("[dh] f1 n=3 w=h01 o=aaaaaaaa\n")
    ok, msg = cross_run_by_round(a, bare)
    check("a log with no rounds= is REFUSED, not passed", not ok, msg)

    # No shared completed round -> refuse.
    g = write([(1, 90, "00000090"), (2, 91, "00000091")])
    ok, msg = cross_run_by_round(a, g)
    check("runs sharing no completed round are REFUSED", not ok, msg)

    for path in (a, b, c, d, e, g, bare):
        try:
            os.remove(path)
        except OSError:
            pass

    print("selftest %s (%d failed)" % ("PASS" if not fails else "FAIL", len(fails)))
    return 0 if not fails else 1


def agree(ok_list, a, b, label):
    """One pair, compared the same way everywhere: the END-OF-ROUND world hash
    over every shared comms round, with dhdiff run beside it as a cross-check.

    WHY BOTH, AND WHY THIS WAY ROUND. dhdiff compares at the FRAME INDEX and
    refuses when the rounds= columns disagree, which is the honest answer to
    "these two logs are not at the same moment at the same index" -- but on a
    loaded machine that is the normal condition, not a defect, because how many
    HOST FRAMES a window spends inside one comms round is wall clock. Measured
    here with another lane's battery running: the two VOICE-OFF windows, which
    contain no voice code at all, drifted on ten rounds and dhdiff refused.

    So the claim is made on the round, where the wall clock cannot reach it,
    and dhdiff still gets a vote: an rc of 1 is a real frame-index divergence
    and stays a red on its own. Only rc 2, "I will not align these", is treated
    as answered -- and it is answered, by the line printed right above it.
    """
    rok, rline = cross_run_by_round(a, b)
    ok_list[0] &= M.verdict(rok, "%s | %s" % (label, rline))
    rc, dline = dhdiff(a, b)
    ok_list[0] &= M.verdict(rc != 1, "%s frame-index cross-check | %s"
                            % (label, dline))


def voice_counts(text):
    m = None
    for m in re.finditer(r"\[voice\] \S+: on=(\d+) tone=(\d+) dev=(\d+) "
                         r"cap=(\d+) tx=(\d+) rx=(\d+) bad=(\d+) dup=(\d+)",
                         text):
        pass
    if not m:
        return None
    return dict(on=int(m.group(1)), tone=int(m.group(2)), dev=int(m.group(3)),
                cap=int(m.group(4)), tx=int(m.group(5)), rx=int(m.group(6)),
                bad=int(m.group(7)), dup=int(m.group(8)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--windows", type=int, default=4)
    ap.add_argument("--frames", default="900")
    ap.add_argument("--selftest", action="store_true",
                    help="run the round-keyed comparison against fixtures and "
                         "exit; no game, no recording device, no windowed slot")
    ap.add_argument("--real-mic", action="store_true",
                    help="open the real recording hardware on the ON pass, so "
                         "the auto-pick scan is inside what the hash "
                         "comparison covers")
    a = ap.parse_args()

    if a.selftest:
        return _selftest()

    if not os.path.exists(EXE):
        print("no walk_window.exe at %s -- build first" % EXE)
        return 1
    for shadow in (os.path.join(os.path.dirname(EXE), "settings.json"),
                   os.path.join(ROOT, "settings.json")):
        if os.path.exists(shadow):
            print("refusing: %s would shadow the per-window settings.json"
                  % shadow)
            return 1
    os.makedirs(OUT, exist_ok=True)
    # See THE WINDOWED TEST SLOT above. Held across BOTH passes, because the
    # cross-run claim only means anything if the ON pass and the OFF pass ran
    # on a machine nobody else was driving a window on.
    with slot_lock.slot(label="voice_determinism"):
        return _passes(a)


def _passes(a):
    n = a.windows
    ok = True

    on_logs, on_rcs = run_pass("on", n, a.frames, PORT, True,
                               real_mic=a.real_mic)
    ok &= M.verdict(all(r == 0 for r in on_rcs),
                    "voice-det every window with voice ON exited clean | rc %s"
                    % (on_rcs,))
    # THE ON PASS HAS TO ACTUALLY BE TALKING, or the cross-run comparison at
    # the bottom is trivially true and proves nothing. Every window sends, every
    # window receives from its peers, and nothing is malformed.
    counts = [voice_counts(M.text(on_logs[k])) for k in range(n)]
    if a.real_mic:
        # Real hardware: several windows, one microphone. Per window all that
        # can be asserted is that voice was on with no tone hook standing in
        # for the device; the capture claim is made once, over the pass.
        for k in range(n):
            c = counts[k]
            ok &= M.verdict(c is not None and c["on"] == 1 and c["tone"] == 0
                            and c["bad"] == 0,
                            "voice-det ON p%d had voice on with the REAL "
                            "capture path, no tone hook | %s" % (k, c))
        scanned = sum(1 for k in range(n)
                      if "auto-pick: measuring" in M.text(on_logs[k]))
        ok &= M.verdict(scanned > 0,
                        "voice-det ON the auto-pick scan really ran | %d of %d "
                        "window(s) scanned" % (scanned, n))
        ok &= M.verdict(any(c and c["cap"] > 0 for c in counts),
                        "voice-det ON at least one window captured real audio "
                        "| cap=%s" % ([c["cap"] if c else None for c in counts],))
    else:
        for k in range(n):
            c = counts[k]
            ok &= M.verdict(c is not None and c["on"] == 1 and c["tx"] > 0 and
                            c["rx"] > 0 and c["bad"] == 0 and c["dev"] == 0,
                            "voice-det ON p%d was really talking and listening "
                            "| %s" % (k, c))
    box = [ok]
    for i in range(n):
        for j in range(i + 1, n):
            agree(box, on_logs[i], on_logs[j],
                  "voice-det ON p%d vs p%d" % (i, j))
    ok = box[0]

    off_logs, off_rcs = run_pass("off", n, a.frames, PORT + 32, False)
    ok &= M.verdict(all(r == 0 for r in off_rcs),
                    "voice-det every window with voice OFF exited clean | rc %s"
                    % (off_rcs,))
    for k in range(n):
        c = voice_counts(M.text(off_logs[k]))
        ok &= M.verdict(c is None,
                        "voice-det OFF p%d sent and received NOTHING (the "
                        "report line is only emitted with voice on) | %s"
                        % (k, c))
    box = [ok]
    for i in range(n):
        for j in range(i + 1, n):
            agree(box, off_logs[i], off_logs[j],
                  "voice-det OFF p%d vs p%d" % (i, j))
    ok = box[0]

    # THE STRONG CLAIM. Same window, same slot, same held key, voice on against
    # voice off. Any world state voice touched would show here and nowhere
    # else.
    #
    # Made on the COMMS ROUND, not the frame index -- see cross_run_by_round
    # above for why, and why that is the same claim rather than a smaller one.
    # dhdiff is still run and its answer is still printed beside it: when it can
    # align the two logs, a divergence IT reports is a red on its own, because
    # two tools agreeing is worth more than one.
    box = [ok]
    for k in range(n):
        agree(box, on_logs[k], off_logs[k],
              "voice-det CROSS-RUN p%d: voice ON world state is identical to "
              "voice OFF" % k)
    ok = box[0]

    print("")
    for v in M.VERDICTS:
        print(v)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
