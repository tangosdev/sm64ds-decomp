#!/usr/bin/env python3
"""_ZTV5Stage SLOT 6: the ROM's own Stage::Behavior in the frame, proved.

(run link100, lane FRAME.  port/slice_gate220.txt is the enrolment,
hal/stage_frame.cpp the faces, port/stage_lifecycle_map.txt section 13 the
accounting.)

WHAT IS BEING PROVED, and what deliberately is not.

Slot 6 of the Stage's own vtable held a host occupant -- a two-line body that
returned 1 -- through five Stage lanes.  It now holds the ROM's
Stage::Behavior, and port/tests/walk_window.cpp has stopped being the port's
hand copy of that function: Stage::CheckInput, ShadowModel::CleanAll,
ProcessKuppaScript, the message pump and the VS countdown are all retired from
the frame loop, and so is the freeze-mask latch in hal/actor_registry.cpp.

That is a bigger claim than "a word in a table changed", and each rung below is
one of the things that had to be true for the retirement to be honest rather
than merely green.

  RUNG 1  STATIC.  walk_window.map carries the gate's own bodies, each from ITS
          OWN matched object.  Presence is not enough and the rung does not
          settle for it: the map's object column is read, so a symbol that
          happened to be satisfied by some host stand-in fails here.  Before the
          gate /OPT:REF dropped every one of them for want of a reference.

  RUNG 2  THE STAGE HEADS THE BEHAVIOUR LIST.  This is the rung that licenses
          two of the retirements and it is the one worth reading twice.

          hal/actor_registry.cpp used to latch data_0209b464 = data_0209b454 at
          the head of phase 3, and tests/walk_window.cpp used to call
          ShadowModel::CleanAll immediately above port_actor_tick.  Both were
          stand-ins for statements of Stage::Behavior -- its first and its last
          -- placed there because "before every other actor's Behavior" is where
          the ROM runs them.  The ROM gets that for free: the Stage's spawn
          record at arm9 0x0209213c carries behaviour priority 3 against the
          hundreds every other class uses, so Stage::Behavior is the first body
          the behaviour walk reaches.  Retiring the two stand-ins is correct if
          and only if the port's list agrees.

          So the rung reads the list.  SM64DS_TRACE_LISTS=1 names every node on
          the behaviour list in walk order; SM64DS_PAUSE_WATCH prints the
          Stage's own object once.  The FIRST node on that line must be the
          Stage.  If a later change reorders the list this rung goes red and the
          two stand-ins have to come back.

  RUNG 3  ONCE PER FRAME, MEASURED ACROSS TWO LENGTHS.  The hazard
          port/stage_lifecycle_map.txt section 12b named is a seat that leaves a
          transcribed statement behind: ProcessKuppaScript would then advance
          the script VM twice a frame and every cutscene in the game would run
          at double speed.  Nothing about that shows in an exit code or a final
          position.

          hal/stage_frame.cpp's slot-6 thunk counts its own dispatches, so the
          number cannot disagree with what ran.  Two runs of different lengths
          are used rather than one, because ONE run can only say "the count is
          plausible": the frames before the Stage is spawned are not knowable
          from here.  Two lengths give the SLOPE, and the slope is the claim --
          the count must rise by exactly the extra frames.

  RUNG 4  THE KUPPA SCRIPT ADVANCES ONCE PER FRAME.  Rung 3 counts the thunk;
          this counts the effect, on the one statement that could not survive
          running twice.  SM64DS_COURSE_PROBE=kuppa launches the real star-get
          camera script and prints ProcessKuppaScript's own frame cursor
          data_0209b274 every frame.  Consecutive frames must differ by ONE.
          A double call would step it two.

  RUNG 5  THE PAUSE REFUSES, AND FOR THE ROM'S REASON.  This is the negative
          control, and it is a real measurement rather than a placeholder.
          Stage::Behavior's pause trigger is gated in adventure mode on
          data_0209caa0[2] & 0x80 -- the opening-seen bit, whose only writer in
          the whole image is src/func_ov085_0212d5dc.cpp:51.  A level selftest
          boots with that bit clear, so a scripted START must NOT open the pause
          screen, and the watch line must say seen=0 while it refuses.  A run
          that paused here would mean the port had invented a pause the
          cartridge does not offer.

  RUNG 6  THE PAUSE OPENS.  Same binary, same scripted START, with
          SM64DS_PAUSE_WATCH=2 setting the opening-seen bit the way the
          opening's own last state sets it.  data_0209f2c4 must go 0 -> 1 on the
          press, which is Stage::Behavior running PS_Init and PS_Update -- fifty
          matched TUs that had never been in a link.  And the run must exit 0
          with SM64DS_FAULTS_FATAL=1: the pause screen allocates, draws and
          reads input, and none of that may fault.

  RUNG 7  AND IT CLOSES.  A second START while paused must bring data_0209f2c4
          back to 0.  A pause that opens and cannot be left is the failure mode
          seaming PS_Update would have shipped, and it is the reason the whole
          menu tree is enrolled rather than seamed; this rung is what says the
          port did not ship it anyway.

WHAT IS NOT PROVED HERE.  Nothing about slot 9: Stage::Render keeps its host
occupant this lane.  And nothing about what the pause screen LOOKS like -- these
rungs read state words and exit codes, not pixels.  The picture is the owner's
call and section 13 says so.

    python port/tools/stage_pause_proof.py [--root R] [--exe E]
"""

import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mp2_proof as M  # noqa: E402

VERDICTS = []


def verdict(ok, line):
    VERDICTS.append(("PASS" if ok else "FAIL") + "  " + line)
    print(VERDICTS[-1], flush=True)
    return ok


def run(exe, root, rundir, name, frames, extra=None, level="1"):
    d = os.path.join(rundir, name)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    log = os.path.join(d, "run.log")
    e = M.env_base(root, d, name)
    e["SM64DS_LEVEL"] = level
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_FAULTS_FATAL"] = "1"
    if extra:
        e.update(extra)
    rc = M.finish(M.spawn(exe, d, e, log), 900)
    return rc, M.text(log)


# ---------------------------------------------------------------- rung 1
# symbol -> the object that must define it
GATE_BODIES = {
    "?Behavior@Stage@@QAEHXZ": "_ZN5Stage8BehaviorEv",
    "?PS_Init@Stage@@SAXXZ": "_ZN5Stage7PS_InitEv",
    "?PS_Update@Stage@@SAXXZ": "_ZN5Stage9PS_UpdateEv",
    "?LC_Update@Stage@@SAXXZ": "_ZN5Stage9LC_UpdateEv",
    "?PS_UpdateOptionsMenu@Stage@@SAXXZ": "_ZN5Stage20PS_UpdateOptionsMenuEv",
    "?PS_UpdateOkAndBackButtons@Stage@@SAX_N@Z":
        "_ZN5Stage25PS_UpdateOkAndBackButtonsEb",
    "?Display@Message@@QAEXI@Z": "_ZN7Message7DisplayEj",
    "?DisplayPauseTextVS@Message@@SAXG@Z": "_ZN7Message18DisplayPauseTextVSEt",
    "__ZN5Stage10PS_CleanupEv": "_ZN5Stage10PS_CleanupEv",
    "__ZN5Stage7VE_InitEv": "_ZN5Stage7VE_InitEv",
    "__ZN5Sound10PauseMusicEv": "_ZN5Sound10PauseMusicEv",
}
ROW = re.compile(r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+[0-9a-fA-F]{8}"
                 r"\s+\S*\s*(\S+)\s*$")


def rung1(mappath):
    seen = {}
    with open(mappath, encoding="utf-8", errors="replace") as f:
        for line in f:
            m = ROW.match(line.rstrip("\n"))
            if m and m.group(1) in GATE_BODIES:
                seen.setdefault(m.group(1), m.group(2))
    ok = True
    for sym, want_obj in sorted(GATE_BODIES.items()):
        obj = seen.get(sym)
        if obj is None:
            ok = verdict(False, "rung1  %s is NOT in the map" % sym) and ok
        elif want_obj not in obj:
            ok = verdict(False, "rung1  %s comes from %s, not %s"
                         % (sym, obj, want_obj)) and ok
    return verdict(ok, "rung1  all %d gate-220 bodies in walk_window.map from "
                       "their own matched objects" % len(GATE_BODIES)) and ok


# ---------------------------------------------------------------- rung 2
LIST_RE = re.compile(r"\[list\] behaviour head [0-9a-f]+:(.*)")
NODE_RE = re.compile(r"\{node (\S+) actor (\S+) id (\d+)")


def rung2(text):
    m = re.search(r"\[pause\] stage actor=(\S+)", text)
    if not m:
        return verdict(False, "rung2  the run never named the Stage's object")
    stage = m.group(1).lower().lstrip("0") or "0"
    lines = LIST_RE.findall(text)
    if not lines:
        return verdict(False, "rung2  no behaviour-list trace in the run")
    bad = 0
    checked = 0
    for body in lines[2:]:                       # skip the pre-spawn frames
        nodes = NODE_RE.findall(body)
        if not nodes:
            continue
        checked += 1
        first = nodes[0][1].lower().lstrip("0") or "0"
        if first != stage:
            bad += 1
    if not checked:
        return verdict(False, "rung2  the behaviour list was empty every frame")
    return verdict(bad == 0,
                   "rung2  the Stage (%s) heads the behaviour list on %d/%d "
                   "traced frames" % (m.group(1), checked - bad, checked))


# ---------------------------------------------------------------- rung 3
COUNT_RE = re.compile(r"\[stageframe\] Stage::Behavior ran (\d+) time")


def rung3(exe, root, rundir):
    counts = {}
    for frames in (120, 240):
        rc, t = run(exe, root, rundir, "r3_%d" % frames, frames,
                    {"SM64DS_STAGE_FRAME_COUNT": "1"})
        m = COUNT_RE.search(t)
        if rc != 0 or not m:
            return verdict(False, "rung3  %d-frame run rc=%d, count line %s"
                           % (frames, rc, "missing" if not m else "ok"))
        counts[frames] = int(m.group(1))
    slope = counts[240] - counts[120]
    ok = slope == 120
    return verdict(ok, "rung3  Stage::Behavior ran %d times in 120 frames and "
                       "%d in 240 -- slope %d, one per frame"
                   % (counts[120], counts[240], slope))


# ---------------------------------------------------------------- rung 4
KUPPA_RE = re.compile(r"\[kuppa\] f(\d+) fc48=(-?\d+) b274=(\d+)")


def rung4(exe, root, rundir):
    rc, t = run(exe, root, rundir, "r4_kuppa", 200,
                {"SM64DS_COURSE_PROBE": "kuppa,60"})
    rows = [(int(a), int(b), int(c)) for a, b, c in KUPPA_RE.findall(t)]
    live = [(f, cur) for f, fc, cur in rows if fc != 0]
    if rc != 0:
        return verdict(False, "rung4  the kuppa run exited %d" % rc)
    if len(live) < 10:
        return verdict(False, "rung4  the script never ran (%d live frames of "
                              "%d probe lines)" % (len(live), len(rows)))
    steps = {}
    for (f0, c0), (f1, c1) in zip(live, live[1:]):
        if f1 == f0 + 1:
            steps[c1 - c0] = steps.get(c1 - c0, 0) + 1
    ones = steps.get(1, 0)
    others = sum(v for k, v in steps.items() if k != 1)
    return verdict(ones > 0 and others == 0,
                   "rung4  ProcessKuppaScript's cursor stepped by 1 on %d "
                   "consecutive live frames, by anything else on %d"
                   % (ones, others))


# ---------------------------------------------------------------- rungs 5-7
PAUSE_RE = re.compile(r"\[pause\] f(\d+) f2c4=(\d+) seen=(\d+) msg=(\d+) "
                      r"fade=(\d+) trio=(\d+) held=([0-9a-f]+)")
PRESS = "150-155:START,220-225:START"


def pause_rows(t):
    return [tuple(int(x, 16) if i == 6 else int(x)
                  for i, x in enumerate(g)) for g in PAUSE_RE.findall(t)]


def rung5(exe, root, rundir):
    rc, t = run(exe, root, rundir, "r5_refuse", 260,
                {"SM64DS_PAUSE_WATCH": "1", "SM64DS_PROBE_INPUT": PRESS})
    rows = pause_rows(t)
    if rc != 0 or not rows:
        return verdict(False, "rung5  run rc=%d, %d watch lines" % (rc, len(rows)))
    seen_any = any(r[2] for r in rows)
    paused = [r for r in rows if r[1] != 0]
    start_seen = any(r[6] & 0x8 for r in rows)
    ok = (not seen_any) and (not paused) and start_seen
    return verdict(ok, "rung5  NEGATIVE CONTROL: opening-seen bit clear on "
                       "every one of %d frames, START arrived (%s), pause "
                       "refused on all of them (%d paused frames)"
                   % (len(rows), "yes" if start_seen else "NO", len(paused)))


def rung67(exe, root, rundir):
    rc, t = run(exe, root, rundir, "r67_open", 300,
                {"SM64DS_PAUSE_WATCH": "2", "SM64DS_PROBE_INPUT": PRESS})
    rows = pause_rows(t)
    if not rows:
        verdict(False, "rung6  no watch lines")
        return False
    ok6 = verdict(rc == 0, "rung6  the paused run exited %d with "
                           "SM64DS_FAULTS_FATAL=1" % rc)
    opened = [r[0] for r in rows if r[1] != 0]
    ok6 = verdict(bool(opened), "rung6  data_0209f2c4 went nonzero at frame "
                                "%s (the pause screen opened)"
                  % (opened[0] if opened else "NEVER")) and ok6
    # rung 7: it comes back to 0 after the second press
    closed = False
    if opened:
        after = [r for r in rows if r[0] > opened[0]]
        closed = any(r[1] == 0 for r in after)
    ok7 = verdict(closed, "rung7  data_0209f2c4 returned to 0 after the second "
                          "START (the pause screen closed)")
    return ok6 and ok7


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=os.path.abspath(os.path.join(HERE, "..", "..")))
    ap.add_argument("--exe", default="")
    args = ap.parse_args()
    root = os.path.abspath(args.root)
    exe = os.path.abspath(args.exe) if args.exe else os.path.join(
        root, "build", "port", "walk_window.exe")
    mappath = os.path.join(root, "build", "port", "walk_window.map")
    rundir = os.path.join(root, "build", "stage_pause_proof")
    os.makedirs(rundir, exist_ok=True)

    print("exe %s" % exe)
    ok = rung1(mappath)

    rc, t = run(exe, root, rundir, "r2_lists", 90,
                {"SM64DS_TRACE_LISTS": "1", "SM64DS_PAUSE_WATCH": "1"})
    if rc != 0:
        ok = verdict(False, "rung2  the list-trace run exited %d" % rc) and ok
    ok = rung2(t) and ok
    ok = rung3(exe, root, rundir) and ok
    ok = rung4(exe, root, rundir) and ok
    ok = rung5(exe, root, rundir) and ok
    ok = rung67(exe, root, rundir) and ok

    print("\n---- stage_pause_proof ----")
    for v in VERDICTS:
        print(v)
    print("ALL GREEN" if ok else "RED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
