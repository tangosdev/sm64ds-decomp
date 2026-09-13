#!/usr/bin/env python
"""THE FAN-OUT TAIL PROOF -- bit-identity across the REPLACE -> EXTEND change.

Run link100, lane FANTAIL, on lane R3C's ruling 2.

WHAT IT PROVES. port/hal/comms_fanout_wide.cpp used to REPLACE the ROM's two
fan-out bodies above four players: func_0203bb60 and func_0203bc7c resolved to
host copies of src/func_0203bb60.c and src/func_0203bc7c.c with the loop bound
moved from 4 to 16, and the matched TUs (linked under the per-source renames
func_0203bb60_narrow / func_0203bc7c_narrow) were called by nobody in a wide
session. It now EXTENDS them: the ROM's own body runs on every frame of every
session, and a host tail appends slots 4..15 when the session is wider than
four.

That is only allowed if it is EXACTLY the same function. It is, and the reason
is arithmetic rather than opinion: func_0203bb60 is purely per-slot, and
func_0203bc7c has exactly one cross-slot word (data_020a0e44), which it zeroes
at its head and OR-sets per slot. Zero-once-then-accumulate over 0..3 followed
by accumulate over 4..15 is the same function as zero-once-then-accumulate over
0..15. Every other write in both bodies is indexed by the slot.

This tool is the measurement that stands behind that argument. Build the tree
before the change, capture; build it after, capture; compare. The comparison is
a plain line-for-line diff of what the fan-out actually left in memory, hashed
once per frame at the tail of step 0x17 by the SM64DS_FANOUT_DUMP instrument in
comms_fanout_wide.cpp. Any difference at all is a red.

THE RUNS. One solo run, where the session is narrow and the dispatchers must
reduce to the ROM's body and nothing else, plus the loopback ladder's rungs --
two, four, five and sixteen windows -- where the tails actually carry slots
4..15. The solo run is spawned through mp2_proof's quiet spawner (minimized,
never activated, muted, no dialog) and the ladder is port/tools/vs16_ladder.sh,
which keeps the same rule. Nothing here launches a visible or audible window.

WHICH RUNGS ARE ACTUALLY REPRODUCIBLE, MEASURED. Run this against a directory
captured from the SAME build twice before reading any before/after difference
as a regression -- the lane did, and the answer is not what you would guess:

  solo                       byte-identical run to run
  rung16, all 16 windows     byte-identical run to run (11040 lines)
  rungs 2, 4 and 5           NOT reproducible against themselves

At sixteen windows every peer is input-bound on every other peer, so the
session is strictly lockstep and the capture is exact. At two, four and five
windows the machines finish their frames faster than the wire feeds them, and
the RECORD RUN the transport writes (the recs= field, which is this
instrument's INPUT, not the fan-out's output) picks up wall-clock jitter --
about 1900 differing lines between two runs of the SAME binary. The line
counts wander by a few frames as well. So on those rungs compare the fan-out's
own OUTPUT fields (e44, touch, pad, prev48, prev50), which --compare reports
separately, and read the strict line diff only on solo and rung16.

USAGE
  python port/tools/fanout_tail_proof.py --capture <dir> [--frames N]
                                         [--rungs 2,4,5,16] [--solo-only]
  python port/tools/fanout_tail_proof.py --compare <before-dir> <after-dir>
                                         [--alive-frames 590]

The capture directory is written OUTSIDE the tree by convention (the lane put
its two under the run's out/FANTAIL/). --compare prints a strict line-for-line
verdict per file AND the output-versus-input split, and exits non-zero if any
fan-out OUTPUT field differs anywhere -- which is the actual red. A strict line
difference on a rung that is not reproducible against itself is reported and
does not, by itself, set the exit code.
"""

import argparse
import difflib
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
LADDER = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                      "vs16_ladder.sh")

TAG = "[fanout]"


def harvest(logpath, outpath):
    """Pull the instrument's lines out of a run log, in order."""
    lines = []
    if os.path.exists(logpath):
        with open(logpath, "r", errors="replace") as f:
            for ln in f:
                if ln.startswith(TAG):
                    lines.append(ln.rstrip("\r\n"))
    with open(outpath, "w", newline="\n") as f:
        for ln in lines:
            f.write(ln + "\n")
    return len(lines)


def capture_solo(outdir, frames):
    """One window, no transport, the fan-out FORCED ON.

    SM64DS_COMMS_FANOUT is the override hal/comms_conductor.cpp keeps for the
    harnesses. Without it a transportless run skips the two steps entirely and
    the instrument prints nothing, which would make the solo diff vacuously
    empty rather than a measurement. With it the dispatchers run every frame
    with the session narrow, which is exactly the path that has to reduce to
    the ROM's body and nothing else.
    """
    d = os.path.join(outdir, "solo_run")
    os.makedirs(d, exist_ok=True)
    e = M.env_base(ROOT, d, "solo")
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_COMMS_FANOUT"] = "1"
    e["SM64DS_FANOUT_DUMP"] = "1"
    log = os.path.join(d, "run.log")
    p = M.spawn(EXE, d, e, log)
    rc = p.wait()
    n = harvest(log, os.path.join(outdir, "solo.txt"))
    print("solo   rc=%s  %d dump lines" % (rc, n))
    return n


def capture_rung(outdir, n, frames, vsmap):
    """One ladder rung: N windows, one loopback session, one arena."""
    ladder_out = os.path.join(outdir, "ladder")
    e = dict(os.environ)
    e["VS16_ROOT"] = ROOT.replace("\\", "/")
    e["VS16_ASSET_ROOT"] = ROOT
    e["VS16_OUT"] = ladder_out
    e["SM64DS_FANOUT_DUMP"] = "1"
    logpath = os.path.join(outdir, "rung%d_ladder.log" % n)
    with open(logpath, "wb") as lf:
        rc = subprocess.call(["bash", LADDER, str(n), str(frames), str(vsmap)],
                             env=e, stdout=lf, stderr=subprocess.STDOUT)
    total = 0
    for k in range(n):
        src = os.path.join(ladder_out, "rung%d" % n, "p%d" % k, "run.log")
        total += harvest(src, os.path.join(outdir, "rung%d_p%d.txt" % (n, k)))
    print("rung%-3d rc=%s  %d dump lines over %d windows" % (n, rc, total, n))
    return total


# The fields the FAN-OUT writes. recs= is deliberately not here: it is the
# record run the transport fills in before step 0x16 runs, so it is the
# instrument's input, and on the small rungs it is the half that jitters.
OUT_FIELDS = ("e44", "touch", "pad", "prev48", "prev50")

# Rungs whose capture is exact run to run, so a strict line diff on them is a
# verdict rather than a coin toss. Measured, not assumed -- see the banner.
STRICT = ("solo.txt", "rung16_")


def parse(path):
    rows = []
    for ln in open(path):
        parts = ln.split()
        if not parts or parts[0] != TAG:
            continue
        rows.append(dict(kv.split("=", 1) for kv in parts[1:] if "=" in kv))
    return rows


def is_strict(name):
    return any(name == s or name.startswith(s) for s in STRICT)


def compare(before, after, alive=590):
    names = sorted(set(os.listdir(before)) | set(os.listdir(after)))
    names = [x for x in names if x.endswith(".txt")]
    if not names:
        print("FAIL no capture files in either directory")
        return 1
    strict_bad = out_bad = soft = 0
    for name in names:
        b = os.path.join(before, name)
        a = os.path.join(after, name)
        if not (os.path.exists(b) and os.path.exists(a)):
            print("FAIL %-18s missing on one side" % name)
            strict_bad += 1
            continue
        bl = open(b).read().splitlines()
        al = open(a).read().splitlines()
        rb, ra = parse(b), parse(a)
        # On a reproducible rung the whole run is compared; on a jittery one
        # only the window in which every peer is still alive, because the
        # capture past a peer's exit is not a function of the code alone.
        n = min(len(rb), len(ra))
        if not is_strict(name):
            n = min(n, alive)
        od = sum(1 for i in range(n)
                 if tuple(rb[i][k] for k in OUT_FIELDS)
                 != tuple(ra[i][k] for k in OUT_FIELDS))
        rd = sum(1 for i in range(n) if rb[i].get("recs") != ra[i].get("recs"))
        out_bad += od
        if bl == al:
            print("OK   %-18s %d lines identical" % (name, len(bl)))
            continue
        tag = "FAIL" if is_strict(name) else "JITTER"
        if is_strict(name):
            strict_bad += 1
        else:
            soft += 1
        print("%-6s %-18s before %d lines, after %d lines | over the first %d "
              "frames: fan-out OUTPUT diffs %d, transport input recs diffs %d"
              % (tag, name, len(bl), len(al), n, od, rd))
        if is_strict(name):
            for ln in list(difflib.unified_diff(bl, al, "before/" + name,
                                                "after/" + name,
                                                lineterm=""))[:40]:
                print("     " + ln)
    print("%d file(s): %d strict difference(s), %d jittery rung(s), "
          "%d fan-out OUTPUT difference(s)"
          % (len(names), strict_bad, soft, out_bad))
    print("VERDICT: %s" % ("THE FAN-OUT'S OUTPUT IS IDENTICAL"
                           if (strict_bad == 0 and out_bad == 0)
                           else "THE FAN-OUT'S OUTPUT MOVED -- this is a red"))
    return 1 if (strict_bad or out_bad) else 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--capture", metavar="DIR")
    ap.add_argument("--compare", nargs=2, metavar=("BEFORE", "AFTER"))
    ap.add_argument("--frames", type=int, default=900)
    ap.add_argument("--solo-frames", type=int, default=600)
    ap.add_argument("--rungs", default="2,4,5,16")
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--solo-only", action="store_true")
    ap.add_argument("--alive-frames", type=int, default=590,
                    help="on the rungs that are not reproducible against "
                         "themselves, compare only this many frames -- the "
                         "window in which every peer is still alive")
    a = ap.parse_args()

    if a.compare:
        return compare(a.compare[0], a.compare[1], a.alive_frames)
    if not a.capture:
        ap.error("one of --capture or --compare is required")
    if not os.path.exists(EXE):
        print("FAIL no walk_window.exe at %s -- build first" % EXE)
        return 2
    os.makedirs(a.capture, exist_ok=True)
    capture_solo(a.capture, a.solo_frames)
    if not a.solo_only:
        for r in [int(x) for x in a.rungs.split(",") if x.strip()]:
            capture_rung(a.capture, r, a.frames, a.map)
    print("captured into %s" % a.capture)
    return 0


if __name__ == "__main__":
    sys.exit(main())
