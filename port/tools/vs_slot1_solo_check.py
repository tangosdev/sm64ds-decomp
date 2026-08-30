"""Solo check for the fc5c width fix: does player 2 SIMULATE on a VS arena?

One instance, no comms env at all -- the reproduction shape the frozen-slot1
report used. Two runs:

    arena    SM64DS_VS_MAP=0 (level 51) + SM64DS_VS_PLAYERS=2
    control  level 1               + SM64DS_VS_PLAYERS=2

and three assertions on the arena run that were all false before the fix:

  1. NO "[vs] port-supplied start" line: the ROM's own loop seats slot 1
     from entrance record p3+1, so the stand-in finds the slot filled.
  2. Slot 1's first probed position is record 1 of ov059's entrance table,
     (-1387,254,6667) << 12 -- the arena's real second start, not the
     fabricated record-0-plus-40-units stand-in.
  3. Slot 1 TICKS: its state leaves St_LevelEnter_Main (020c6f3c) and its
     position row count is not 1. Before the fix it sat in one state at one
     position for 900 straight frames.

The control run asserts slot 1 still ticks on a non-VS level (the case that
always worked), so the count-hold retirement did not trade one freeze for
another.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vs_slot1_solo")

REC1_LVL51 = (-1387 << 12, 254 << 12, 6667 << 12)

PROBE = re.compile(
    r"^\[vs\] f(\d+) slot(\d+) actor=([0-9A-Fa-f]+|NULL)"
    r"(?: no=(\d+) char=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\).*"
    r"state=([0-9a-f]+))?", re.M)


def run_one(name, frames, extra_env):
    d = os.path.join(OUT, name)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    e = M.env_base(ROOT, d, name)
    e["SM64DS_VS_PLAYERS"] = "2"
    e["SM64DS_VS_PROBE"] = "1"
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    for k in ("SM64DS_COMMS_ROLE", "SM64DS_COMMS_RELAY", "SM64DS_COMMS_CODE",
              "SM64DS_COMMS_FANOUT", "SM64DS_COMMS_REPORT",
              "SM64DS_COMMS_INJECT"):
        e.pop(k, None)
    if "SM64DS_VS_MAP" in extra_env:
        # A VS boot names its own destination and must not carry a second --
        # the same line vs_online_proof.py draws over env_base's level.
        e.pop("SM64DS_LEVEL", None)
    e.update(extra_env)
    log = os.path.join(d, "run.log")
    p = M.spawn(EXE, d, e, log)
    rc = M.finish(p, 600)
    return rc, M.text(log), log


def slot_rows(t, slot):
    rows = []
    for m in PROBE.finditer(t):
        if int(m.group(2)) != slot:
            continue
        if m.group(3) == "NULL":
            rows.append((int(m.group(1)), None, None, None))
            continue
        rows.append((int(m.group(1)),
                     (int(m.group(6)), int(m.group(7)), int(m.group(8))),
                     m.group(9), int(m.group(5))))
    return rows


def check(name, rc, t, log, expect_first_pos, expect_char):
    ok = True
    rows = slot_rows(t, 1)
    live = [r for r in rows if r[1] is not None]
    print("== %s == rc=%s log=%s" % (name, rc, log))
    if "[vs] port-supplied start" in t:
        line = [l for l in t.splitlines() if "port-supplied start" in l][0]
        print("  FAIL: stand-in fired: %s" % line.strip())
        ok = False
    else:
        print("  ok: no port-supplied start (the loop seated every slot)")
    if not live:
        print("  FAIL: no live slot1 probe rows at all")
        return False
    first = live[0]
    if expect_first_pos is not None:
        if first[1] == expect_first_pos:
            print("  ok: slot1 first pos %s == entrance record 1" % (first[1],))
        else:
            print("  FAIL: slot1 first pos %s, wanted %s"
                  % (first[1], expect_first_pos))
            ok = False
    if expect_char is not None:
        if first[3] == expect_char:
            print("  ok: slot1 char=%d" % first[3])
        else:
            print("  FAIL: slot1 char=%d, wanted %d" % (first[3], expect_char))
            ok = False
    states = sorted({r[2] for r in live})
    npos = len({r[1] for r in live})
    last = live[-1]
    # ALIVENESS IS THE STATE, NOT THE POSITION. A solo run feeds slot 1 no
    # input at all, and an uncommanded player standing on its start is correct
    # physics -- the frozen-actor signature was state PINNED at
    # St_LevelEnter_Main (020c6f3c) with the position identical for 900
    # frames. Movement under command is the networked proof's assertion
    # (vs_online_proof.py injects a key per instance); asserting it here
    # would fail every healthy solo boot.
    if states == ["020c6f3c"]:
        print("  FAIL: slot1 never left St_LevelEnter_Main over %d rows"
              % len(live))
        ok = False
    else:
        print("  ok: slot1 states %s" % ",".join(states))
    print("  info: %d distinct positions over %d rows; last f%d %s state=%s"
          % (npos, len(live), last[0], last[1], last[2]))
    return ok


def main():
    frames = int(sys.argv[1]) if len(sys.argv) > 1 else 600
    rc, t, log = run_one("arena51", frames, {"SM64DS_VS_MAP": "0"})
    ok = check("arena51 (level 51, VS mode)", rc, t, log,
               REC1_LVL51, 3)
    rc2, t2, log2 = run_one("control1", frames, {"SM64DS_LEVEL": "1"})
    ok2 = check("control1 (level 1, no VS flag)", rc2, t2, log2, None, None)
    print("VERDICT: %s" % ("PASS" if (ok and ok2) else "FAIL"))
    return 0 if (ok and ok2) else 1


if __name__ == "__main__":
    sys.exit(main())
