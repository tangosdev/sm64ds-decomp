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
        "SM64DS_SKIP_MENU": "1",
        "SM64DS_SCENE_WINDOW": "1",
        "SM64DS_SCENE_FRAMES": "100000",
        "SM64DS_TOUCH_PROBE": "560-563:44:68,620-623:44:68,680-683:44:68,"
                              "760-763:44:68,860-863:44:68",
        "SM64DS_NO_PLAYLOG": "1",
        "SM64DS_CUTSCENE_STATES": "1",
        "SM64DS_WINDOW_SELFTEST": str(a.frames),
    })
    log = out / "run.log"
    with log.open("wb") as f:
        rc = subprocess.call([str(exe)], stdout=f, stderr=subprocess.STDOUT,
                             env=e, cwd=str(root))
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
    return 0 if rc == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
