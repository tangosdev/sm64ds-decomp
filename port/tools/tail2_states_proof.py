#!/usr/bin/env python3
"""Does the port actually DISPATCH CUTSCENE_OBJECT's state tables? -- lane TAIL2.

WHY THIS EXISTS. hal/cutscene_states_link100.cpp seats sixty-six cells and
links forty-nine translation units, and a linkage count cannot tell a table the
game indexes from a table nothing reaches. Without a measurement the two look
identical: both are +49 and both are silent. So the file carries an env-gated
per-cell census (SM64DS_CUTSCENE_STATES=1, inert unset) and this drives the one
path that reaches it.

THE PATH IS THE ROM'S OWN OPENING, not a poke. SM64DS_SKIP_MENU taps through
the title and stops on the file select; the touch probes pick the NEW-FILE slot
at (44, 68); the ROM's own intro gate then decides, and hal/level_boot.cpp
prints "[intro] the opening is ARMED for this entry" when it does. The save file
is written into a scratch directory and deleted first, because a save with the
intro already seen makes this run measure something else -- the recipe
port/tools/opening_audio_measure.py established, reused verbatim.

WHAT IT PRINTS: the census, plus the join back to port/slice_gate219.txt --
which ROM body each entered cell named, so an entered cell is a named TU and
not just a number.

  python port/tools/tail2_states_proof.py <root> [--frames N] [--tag NAME]

The run is SILENT AND MUTED: SM64DS_NO_FOCUS=1 keeps the window from taking
focus, SM64DS_VOLUME=0 and SM64DS_NO_AUDIO=1 keep it quiet, and the machine-wide
windowed test slot is held for the duration.
"""
import argparse
import csv
import os
import pathlib
import re
import shutil
import subprocess
import sys


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--frames", type=int, default=4000)
    ap.add_argument("--tag", default="states")
    ap.add_argument("--outroot",
                    default=os.environ.get("TEMP", "/tmp") + "/tail2_states")
    ap.add_argument("--no-opening-gate", action="store_true",
                    help="report the opening gate below but do not fail on it")
    a = ap.parse_args()

    root = pathlib.Path(a.root).resolve()
    exe = root / "build/port/walk_window.exe"
    if not exe.exists():
        sys.exit(f"not built: {exe}")

    out = pathlib.Path(a.outroot) / a.tag
    shutil.rmtree(out, ignore_errors=True)
    out.mkdir(parents=True)

    e = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    e.update({
        "SM64DS_TEST_LOCK": "1",
        "SM64DS_TEST_LOCK_PATH": r"C:\tmp\sm64ds-test-slot\windowed_test.lock",
        "SM64DS_TEST_LOCK_TIMEOUT": "5400",
        "SM64DS_ASSET_ROOT": str(root),
        "SM64DS_SAVE_PATH": str(out / "sm64ds.sav"),
        "SM64DS_NO_AUDIO": "1",
        "SM64DS_VOLUME": "0",
        "SM64DS_NO_FOCUS": "1",
        "SM64DS_MINIMIZED": "1",
        "SM64DS_SKIP_MENU": "1",
        "SM64DS_SCENE_WINDOW": "1",
        "SM64DS_SCENE_FRAMES": "100000",
        "SM64DS_TOUCH_PROBE": "560-563:44:68,620-623:44:68,680-683:44:68,"
                              "760-763:44:68,860-863:44:68",
        "SM64DS_NO_PLAYLOG": "1",
        "SM64DS_CUTSCENE_STATES": "1",
        "SM64DS_WINDOW_SELFTEST": str(a.frames),
    })
    # Run hd1, lane MDL: the model-smoothing level is the one SM64DS_* this
    # proof has to be able to carry through the scrub above, because the gate
    # is run BOTH with the key absent and with it set -- smoothing is picture
    # only, so the opening must take the same distinct positions either way,
    # and there is no way to ask that question without passing the key. With
    # the variable unset this line does nothing and the proof is byte-identical
    # to one from before it existed.
    if os.environ.get("SM64DS_SMOOTH_MODELS"):
        e["SM64DS_SMOOTH_MODELS"] = os.environ["SM64DS_SMOOTH_MODELS"]
    # The improved minimap's three keys, carried for the same reason and in the
    # same shape. The option draws a panel on the BOTTOM screen and nothing
    # else, so the opening has to take the same distinct positions with it on
    # as with it absent -- and this gate cannot ask that question unless the
    # keys survive the scrub above. Every one of them unset is a loop that does
    # nothing, so a run that names none is byte-identical to one from before
    # this existed.
    for k in ("SM64DS_IMPROVED_MINIMAP", "SM64DS_MINIMAP_SCALE",
              "SM64DS_MINIMAP_DIR"):
        if os.environ.get(k):
            e[k] = os.environ[k]
    log = out / "run.log"
    with log.open("wb") as f:
        # The quiet spawner every other launcher in port/tools uses (battery.py's
        # run(), bootab.py): minimized, never activated, no console. NO_FOCUS and
        # VOLUME alone still put a visible window on the screen for the whole run.
        si = None
        flags = 0
        if os.name == "nt":
            si = subprocess.STARTUPINFO()
            si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
            flags = subprocess.CREATE_NO_WINDOW
        rc = subprocess.call([str(exe)], stdout=f, stderr=subprocess.STDOUT,
                             env=e, cwd=str(root), startupinfo=si,
                             creationflags=flags)
    text = log.read_text(encoding="utf-8", errors="replace")

    armed = "the opening is ARMED for this entry" in text
    script = "a cutscene script is running" in text
    print(f"walk_window rc={rc}  frames={a.frames}  log={log}")
    print(f"  intro armed      : {armed}")
    print(f"  cutscene running : {script}")

    rows = re.findall(r"\[cutscene-states\] ([0-9a-f]{8})\[(\d+)\] x(\d+)", text)
    total = re.search(r"\[cutscene-states\] (\d+) distinct cells entered", text)
    if not rows:
        print("  NO CELL WAS ENTERED. That is a finding, not a pass: the "
              "tables are linked and nothing dispatched them on this path.")
    tsv = root / "build/tail2_cells.tsv"
    cells = {}
    if tsv.exists():
        for r in csv.DictReader(tsv.open(), delimiter="\t"):
            cells[(int(r["array"], 16), int(r["idx"]))] = int(r["fn"], 16)
    print(f"  distinct cells entered: {total.group(1) if total else 0}")
    for arr, idx, n in rows:
        key = (int(arr, 16), int(idx))
        fn = cells.get(key)
        print(f"    data_ov002_{arr}[{idx:>2}] x{n:<6} "
              + (f"func_ov002_{fn:08x}" if fn else "(not in the ROM read)"))

    for bad in ("FATAL: data_ov002_", "UNHOSTED: data_ov002_"):
        for line in text.splitlines():
            if bad in line:
                print("  REFUSAL: " + line.strip())

    gate = opening_gate(text)
    if a.no_opening_gate:
        gate = 0
    return 1 if (rc != 0 or gate) else 0


def opening_gate(text):
    """THE OPENING MUST STILL BE MOVING AT THE END OF THE RUN -- run link100
    lane STAGEBUG.

    rc AND THE CELL CENSUS ARE BOTH BLIND TO THE FAILURE THIS CATCHES, which is
    why it is a rung and not a paragraph. port_dispatch_guarded catches an
    access violation inside an actor's phase callback, writes a dump, FREEZES
    that actor and resumes the walk, so the process runs its full frame budget
    and exits 0 with the cutscene dead. That is exactly what
    port/stage_lifecycle_map.txt sections 15 and 16 measured: one quarantined
    fault on the level-change frame, the Stage frozen from then on, and every
    later frame printing the player parked at the same coordinates with a fixed
    camera.

    Three assertions, in the order they fail informatively:
      QUARANTINE  no actor was frozen. One line here is a real defect even
                  though rc is 0.
      REACH       the run got past the opening's level change at all, so a
                  harness that never left the title cannot pass this vacuously.
      MOTION      the player's position is not CONSTANT across the frames after
                  that level change. A frozen Stage pins it; a running cutscene
                  does not.
    """
    bad = 0

    quarantined = [l.strip() for l in text.splitlines() if "[quarantine]" in l]
    if quarantined:
        bad = 1
        print(f"  OPENING GATE: FAIL -- {len(quarantined)} quarantined "
              f"fault(s); an actor was frozen and the run still exited 0")
        for l in quarantined[:4]:
            print("    " + l)

    change = [m.start() for m in re.finditer(r"\[lvl\] change: level ", text)]
    if not change:
        print("  OPENING GATE: FAIL -- the run never reached the opening's "
              "level change, so it proves nothing about the cutscene. Check "
              "SM64DS_TOUCH_PROBE, the fresh save, and --frames.")
        return 1

    after = text[change[-1]:]
    tail = [(int(m.group(1)), m.group(2))
            for m in re.finditer(r"\[f(\d+)\] pos=\(([^)]*)\)", after)]
    if len(tail) < 8:
        print(f"  OPENING GATE: FAIL -- only {len(tail)} frame(s) logged after "
              f"the level change; raise --frames so the tail is measurable")
        return 1

    distinct = len({p for _, p in tail})
    if distinct < 2:
        bad = 1
        print(f"  OPENING GATE: FAIL -- the player is PARKED at ({tail[0][1]}) "
              f"for all {len(tail)} frames after the level change (frames "
              f"{tail[0][0]}..{tail[-1][0]}). That is the frozen-Stage "
              f"signature of stage_lifecycle_map.txt section 15.")
    elif not bad:
        print(f"  OPENING GATE: PASS -- 0 quarantined faults, and the player "
              f"takes {distinct} distinct positions over frames "
              f"{tail[0][0]}..{tail[-1][0]}, after the level change")
    return bad


if __name__ == "__main__":
    sys.exit(main())
