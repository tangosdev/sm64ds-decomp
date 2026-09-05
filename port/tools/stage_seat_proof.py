"""_ZTV5Stage SEAT PROOF: the three slots run link100 lane STAGE took off the
trap, checked statically in the map and dynamically in a live boot, with a
negative control that has to be able to print the other answer.

WHAT IS BEING PROVED, and what deliberately is not.

Slots 3, 16 and 17 of the Stage's own vtable now hold the ROM's own bodies
(Stage::CleanupResources, ~Stage D2 and ~Stage D0) instead of the named-abort
trap hal_fill_stage_vtable writes into every slot at boot. NOTHING IN THE PORT
DISPATCHES ANY OF THE THREE: the cleanup Process walks only actors marked for
destruction and hal/level_change.cpp keeps one Stage alive across every level
change, and nothing deletes the Stage at all. So there is no execution to
observe and this file does not pretend to observe one -- exactly the shape
port/stage_lifecycle_map.txt section 9 uses for slot 12.

What IS observable, and what the three rungs check:

  RUNG 1  STATIC.  walk_window.map carries all five matched symbols the gate
          enrolled: ?CleanupResources@Stage@@QAEHXZ (slot 3's body),
          __ZN5StageD2Ev and __ZN5StageD0Ev (slots 16 and 17), and slot 3's two
          leaves __ZN5Sound21ResetPlayerVoiceGroupEv and
          __ZN7Message15ResetAllGlobalsEv. Before the seat /OPT:REF dropped
          every one of them for want of a reference.

  RUNG 2  DYNAMIC.  A live boot with SM64DS_STAGE_SEAT_PROBE=1 prints all
          twenty words of the table as the fill leaves them, each classified
          TRAP or SEATED by comparing the word against this build's own trap
          thunk -- the only comparison that can tell them apart, since both are
          host addresses. Slots 3, 16 and 17 must read SEATED; slots 18 and 19,
          which are past the ROM's eighteen-word table and are host storage,
          must still read TRAP. That last pair is what stops the rung passing
          on a probe that has been broken into printing SEATED for everything.

  RUNG 3  NEGATIVE CONTROL.  =2 re-installs the trap in those three slots after
          the seat and prints the table a second time in the SAME run. The
          second pass must report TRAP for 3/16/17 while the first reported
          SEATED. A probe that cannot print the other answer is not evidence,
          and this is the rung that makes rung 2 mean something.

  RUNG 4  NO TRAP FIRED.  Neither run prints "is not hosted", the trap's own
          abort line, and both exit 0. Mode 2 leaves the traps in place for the
          whole run, so rung 4 also says the pre-seat program still boots -- the
          seat is not load-bearing for the frame.

Quiet and muted through mp2_proof.env_base: SM64DS_NO_FOCUS, SM64DS_MINIMIZED,
SM64DS_VOLUME=0, CREATE_NO_WINDOW and SW_SHOWMINNOACTIVE. Never raw-start
walk_window.exe.

    python port/tools/stage_seat_proof.py [--root DIR] [--frames N]
"""
import argparse
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

DEFAULT_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))

# The five matched symbols gate 213 enrolled, spelled exactly as a 32-bit MSVC
# /MAP writes them: one leading underscore of cdecl decoration on the C names,
# none on the MSVC-mangled method.
WANT_SYMS = (
    "?CleanupResources@Stage@@QAEHXZ",
    "__ZN5Sound21ResetPlayerVoiceGroupEv",
    "__ZN7Message15ResetAllGlobalsEv",
    "__ZN5StageD2Ev",
    "__ZN5StageD0Ev",
)

SEATED_SLOTS = (3, 16, 17)
TRAP_SLOTS = (18, 19)

ROW = re.compile(r"^\[stage-seat\] slot\s+(\d+)\s+\S.*?\s(TRAP|SEATED)\s*$", re.M)
CONTROL = re.compile(r"^\[stage-seat\] NEGATIVE CONTROL:", re.M)

FAILS = []


def check(ok, label, detail=""):
    print(("  PASS  " if ok else "  FAIL  ") + label + (("  -- " + detail)
                                                        if detail else ""))
    if not ok:
        FAILS.append(label)
    return ok


def passes(text):
    """Split the probe's rows into one dict per pass, in print order.

    The negative-control line separates them. Returned as a list so rung 3 can
    compare pass 0 against pass 1 rather than reading a merged table -- a merge
    would silently let the control's TRAP overwrite the seat's SEATED and the
    rung would pass on nothing.
    """
    out = [{}]
    for line in text.splitlines():
        if CONTROL.match(line):
            out.append({})
            continue
        m = ROW.match(line)
        if m:
            out[-1][int(m.group(1))] = m.group(2)
    return out


def run(root, exe, out, frames, mode):
    d = os.path.join(out, "probe%d" % mode)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    log = os.path.join(d, "run.log")
    e = M.env_base(root, d, "stage%d" % mode)
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_STAGE_SEAT_PROBE"] = str(mode)
    rc = M.run_one(exe, d, e, log, timeout=600)
    return rc, M.text(log)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=DEFAULT_ROOT)
    ap.add_argument("--frames", type=int, default=120)
    a = ap.parse_args()
    root = os.path.abspath(a.root)
    exe = os.path.join(root, "build", "port", "walk_window.exe")
    mapf = os.path.join(root, "build", "port", "walk_window.map")
    out = os.path.join(root, "build", "stage_seat_proof")
    os.makedirs(out, exist_ok=True)

    print("stage_seat_proof: %s" % exe)

    print("RUNG 1 -- the five matched symbols are in the map")
    if not os.path.exists(mapf):
        check(False, "walk_window.map exists", mapf)
    else:
        mt = M.text(mapf)
        for s in WANT_SYMS:
            # Whole-token match: a substring test would let
            # __ZN5StageD2Ev pass on a row that only mentions it inside
            # another name, and the D0/D2 pair is exactly the shape that
            # invites that mistake.
            check(re.search(r"(?<![A-Za-z0-9_?@$])" + re.escape(s) +
                            r"(?![A-Za-z0-9_@$])", mt) is not None,
                  "map carries " + s)

    print("RUNG 2 -- a live boot reports slots 3/16/17 SEATED")
    rc1, t1 = run(root, exe, out, a.frames, 1)
    p1 = passes(t1)
    check(rc1 == 0, "probe run exits 0", "rc=%d" % rc1)
    check(len(p1) == 1 and len(p1[0]) == 20,
          "the probe printed one full 20-slot table",
          "passes=%d rows=%s" % (len(p1), [len(x) for x in p1]))
    if p1 and len(p1[0]) == 20:
        for i in SEATED_SLOTS:
            check(p1[0][i] == "SEATED", "slot %d is SEATED" % i, p1[0].get(i))
        for i in TRAP_SLOTS:
            check(p1[0][i] == "TRAP",
                  "slot %d (past the ROM table) is still TRAP" % i,
                  p1[0].get(i))

    print("RUNG 3 -- the negative control prints the other answer")
    rc2, t2 = run(root, exe, out, a.frames, 2)
    p2 = passes(t2)
    check(rc2 == 0, "control run exits 0", "rc=%d" % rc2)
    check(len(p2) == 2, "the control printed two tables", "passes=%d" % len(p2))
    if len(p2) == 2 and len(p2[0]) == 20 and len(p2[1]) == 20:
        for i in SEATED_SLOTS:
            check(p2[0][i] == "SEATED" and p2[1][i] == "TRAP",
                  "slot %d reads SEATED then TRAP in one run" % i,
                  "%s -> %s" % (p2[0].get(i), p2[1].get(i)))

    print("RUNG 4 -- no trap fired in either run")
    for name, t in (("probe", t1), ("control", t2)):
        check("is not hosted" not in t,
              "%s run never hit the Stage trap" % name)

    print("")
    if FAILS:
        print("stage_seat_proof: RED (%d)" % len(FAILS))
        for f in FAILS:
            print("   " + f)
        return 1
    print("stage_seat_proof: ALL GREEN")
    return 0


if __name__ == "__main__":
    sys.exit(main())
