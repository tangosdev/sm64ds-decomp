"""_ZTV5Stage SEAT PROOF: the three slots run link100 lane STAGE took off the
trap, checked statically in the map and dynamically in a live boot, with a
negative control that has to be able to print the other answer.

WHAT IS BEING PROVED, and what deliberately is not.

Slots 3, 16 and 17 of the Stage's own vtable now hold the ROM's own bodies
(Stage::CleanupResources, ~Stage D2 and ~Stage D0) instead of the named-abort
trap hal_fill_stage_vtable writes into every slot at boot.

SLOTS 16 AND 17 ARE NOT DISPATCHED: both are reached by a delete through the
vptr, and nothing in the port deletes the Stage. So there is no execution to
observe for them and this file does not pretend to observe one -- exactly the
shape port/stage_lifecycle_map.txt section 9 uses for slot 12.

SLOT 3 IS DIFFERENT, and the first version of this file said it was not. Its
dispatch IS reachable: port/stage_lifecycle_map.txt section 5 MEASURED a VS
match-end scene request arriving at it, and section 9 traces a live death-plane
chain into it. The ROM's body is a real level teardown, so seating it
unconditionally removed a deliberate safety stop. The ROM call therefore sits
behind SM64DS_STAGE_SLOT3_ROM -- presence = on, default off, in
hal/stage_bridges.cpp's st_clean thunk -- which is the same shape
hal/stage_slot0.cpp uses to gate the ROM's Stage::InitResources. Rungs 6 and 7
MEASURE that gate rather than asserting it: the same binary, one run each way.

What IS observable, and what the seven rungs check:

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

  RUNG 4  NO TRAP FIRED.  Neither run prints the trap's own abort line and both
          exit 0. Mode 2 leaves the traps in place for the whole run, so rung 4
          also says the pre-seat program still boots -- the seat is not
          load-bearing for the frame.

  RUNG 5  THE OVERLAY TRANCHE, checked by DEFINING OBJECT and not by presence.
          hal/stage_slot0.cpp used to define __Z19UnloadLevelOverlaysi as an
          empty host body, so that symbol was in reach before and after and only
          the map's object column says which body the image carries. The rung
          fails if any of the three overlay names comes from anything but its
          own matched TU's object -- which is the assertion "the host body is
          RETIRED, not kept beside" actually reduces to.

  RUNG 6  THE GATE, SHUT.  SM64DS_STAGE_SLOT3_DISPATCH=1 dispatches
          _ZTV5Stage[3] on the Stage once, at process exit, the way the cleanup
          Process would. With SM64DS_STAGE_SLOT3_ROM unset -- the shipped
          default -- the run must print the trap's own line, naming slot 3 and
          CleanupResources, must NOT print the ROM-body note, and must not exit
          0. That is the safety stop, observed rather than argued.

  RUNG 7  THE GATE, OPEN.  The same dispatch with SM64DS_STAGE_SLOT3_ROM set:
          the ROM-body note must print, the trap line must not, and the probe
          must report the ROM's own return of 1 -- which only happens if
          Stage::CleanupResources ran to its last statement. The pair is what
          makes "gated" a measurement: rung 6 alone cannot tell a gate from a
          revert, and rung 7 alone cannot tell a gate from an unconditional
          seat.

          AT EXIT, and that is why this is safe to run at all: the ROM's
          teardown releases the level's file handles, deletes the skybox and
          the area transformers, zeroes the area table and the Camera and calls
          Deallocate. Nothing in the port may run behind that, so the dispatch
          is registered with std::atexit and happens after main returns.
          Neither rung is a mode anyone ships in, the same standing as
          SM64DS_STAGE_SEAT_PROBE=2.

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

# The overlay tranche, checked by DEFINING OBJECT rather than by presence.
# Presence proves nothing here: hal/stage_slot0.cpp used to define
# __Z19UnloadLevelOverlaysi as an empty host body, so the symbol was available
# before and after and only the object column says which body the image
# actually carries. {map symbol: the object that must define it}.
WANT_FROM = {
    "__Z19UnloadLevelOverlaysi": "_Z19UnloadLevelOverlaysi.c.obj",
    "?LoadLevelOverlays@@YAXH@Z": "_Z17LoadLevelOverlaysi.cpp.obj",
    "?LoadOrUnloadObjectOverlays@@YAXP6AXH@ZH@Z":
        "_Z26LoadOrUnloadObjectOverlaysPFviEi.cpp.obj",
}

SEATED_SLOTS = (3, 16, 17)
TRAP_SLOTS = (18, 19)

# The four lines rungs 6 and 7 read the gate through, spelled exactly as the
# port prints them. The trap's is the WHOLE prefix plus the slot number and the
# slot name, not the "is not hosted" fragment -- rung 4's comment below is the
# reason that fragment is not safe to match on.
SLOT3_TRAP_LINE = "FATAL: Stage vtable slot 3 (CleanupResources) is not hosted"
SLOT3_ROM_NOTE = ("[stage] slot 3: SM64DS_STAGE_SLOT3_ROM is set, so the ROM's "
                  "Stage::CleanupResources is running")
SLOT3_DISPATCHED = "[stage-slot3] dispatching _ZTV5Stage[3]"
SLOT3_RETURNED = "[stage-slot3] Stage::CleanupResources returned 1"

# One map row: "  0001:00012345  _name  10012345 f  obj". The object is the
# last token; closure.py reads the same column the same way.
MAP_ROW = re.compile(r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+"
                     r"[0-9a-fA-F]{8}")

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


def run_dispatch(root, exe, out, frames, rom):
    """One slot-3 dispatch run. `rom` is the gate: False leaves
    SM64DS_STAGE_SLOT3_ROM unset, which is the shipped default and the named
    abort; True sets it, which is the ROM's own body.

    env_base scrubs every inherited SM64DS_ variable before it sets its own, so
    a lane's shell cannot leak the gate into the run that is supposed to be
    without it -- which is the one way this pair could quietly stop measuring
    anything.
    """
    tag = "slot3_%s" % ("rom" if rom else "trap")
    d = os.path.join(out, tag)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    log = os.path.join(d, "run.log")
    e = M.env_base(root, d, tag)
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_STAGE_SLOT3_DISPATCH"] = "1"
    if rom:
        e["SM64DS_STAGE_SLOT3_ROM"] = "1"
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
    # THE WHOLE PREFIX, not "is not hosted". The first draft matched that
    # fragment and went red on both runs against
    # "[sub] LoadControllerModeText(640): top-screen text is not hosted", which
    # hal/sub_screen.cpp prints on every boot and which has nothing to do with
    # this table. The trap's own line is
    #     FATAL: Stage vtable slot %d (%s) is not hosted
    # so the prefix up to the slot number is what identifies it and cannot
    # collide with another subsystem's wording.
    for name, t in (("probe", t1), ("control", t2)):
        check("FATAL: Stage vtable slot" not in t,
              "%s run never hit the Stage trap" % name)

    print("RUNG 5 -- the overlay bodies in the image are the ROM's, not the "
          "host stubs")
    if os.path.exists(mapf):
        owner = {}
        for line in M.text(mapf).splitlines():
            m = MAP_ROW.match(line)
            if m and m.group(1) not in owner:
                owner[m.group(1)] = line.split()[-1]
        for sym, obj in WANT_FROM.items():
            check(owner.get(sym) == obj,
                  "%s comes from %s" % (sym, obj),
                  owner.get(sym, "ABSENT"))

    print("RUNG 6 -- gate shut: a slot-3 dispatch still aborts by name")
    rc6, t6 = run_dispatch(root, exe, out, a.frames, False)
    check(SLOT3_DISPATCHED in t6, "the probe dispatched _ZTV5Stage[3]")
    check(SLOT3_TRAP_LINE in t6,
          "the dispatch landed on the named abort for slot 3")
    check(SLOT3_ROM_NOTE not in t6,
          "the ROM's Stage::CleanupResources did NOT run")
    # An aborting run must not look like a clean one. This is the assertion
    # that would catch a gate wired the wrong way round -- a run that seated
    # the ROM body and finished quietly would pass the three checks above only
    # if the trap line were also missing, and this one closes that door.
    check(rc6 != 0, "the aborting run did not exit 0", "rc=%d" % rc6)

    print("RUNG 7 -- gate open: the same dispatch reaches the ROM's body")
    rc7, t7 = run_dispatch(root, exe, out, a.frames, True)
    check(SLOT3_DISPATCHED in t7, "the probe dispatched _ZTV5Stage[3]")
    check(SLOT3_ROM_NOTE in t7, "the ROM's Stage::CleanupResources ran")
    check("FATAL: Stage vtable slot" not in t7,
          "no Stage trap fired in the gated-open run")
    # The ROM's own last statement is `return 1`, so this line is only printed
    # if the body ran all the way through rather than faulting somewhere in the
    # teardown.
    check(SLOT3_RETURNED in t7,
          "Stage::CleanupResources returned the ROM's own 1")

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
