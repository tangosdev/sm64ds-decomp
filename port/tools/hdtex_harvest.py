#!/usr/bin/env python3
"""Harvest the game's textures into an HD-pack dump, headless and silent.

    python port/tools/hdtex_harvest.py <walk_window.exe> <dump_dir>
           [frames] [budget_s] [levels=all | scenes=all | levels=1,2 | scenes=4]

Boots every level and every scene once with SM64DS_HD_TEXTURES_DUMP pointed at
a per-row directory, then merges the rows into <dump_dir>. Every file is named
by the content hash the loader looks a replacement up by, so what comes out of
here is exactly the set of names a pack may contain.

Quiet, like every other launcher in this tree: SW_SHOWMINNOACTIVE +
CREATE_NO_WINDOW, SM64DS_NO_FOCUS=1, SM64DS_VOLUME=0, every inherited SM64DS_*
dropped except SM64DS_TEST_LOCK* so each row takes the machine-wide test slot.
Nothing is ever shown and nothing ever makes a sound.

It prints the unique-texture count per row and the merged total, which is the
number a pack author needs: how many slots exist, and which level each came
from.
"""

import os
import shutil
import subprocess
import sys
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "port", "tools"))
import battery as B  # noqa: E402  (same import shape bootab.py uses)

if len(sys.argv) < 3:
    sys.exit(__doc__)

EXE = os.path.abspath(sys.argv[1])
EXEDIR = os.path.dirname(EXE)
DUMP = os.path.abspath(sys.argv[2])
FRAMES = sys.argv[3] if len(sys.argv) > 3 else "600"
BUDGET = int(sys.argv[4] if len(sys.argv) > 4 else 300)
FILTER = sys.argv[5] if len(sys.argv) > 5 else ""

LEVELS = B.mounted_levels(ROOT)
SCENES = B.hosted_scenes(ROOT)
if FILTER.startswith("levels="):
    sel = FILTER[7:]
    SCENES = ()
    if sel != "all":
        LEVELS = tuple(i for i in LEVELS if str(i) in sel.split(","))
elif FILTER.startswith("scenes="):
    sel = FILTER[7:]
    LEVELS = ()
    if sel != "all":
        SCENES = tuple(i for i in SCENES if str(i) in sel.split(","))

SI = subprocess.STARTUPINFO()
SI.dwFlags |= subprocess.STARTF_USESHOWWINDOW
SI.wShowWindow = 7  # SW_SHOWMINNOACTIVE: minimized, never activated
NOCON = getattr(subprocess, "CREATE_NO_WINDOW", 0)
BASE_ENV = {k: v for k, v in os.environ.items()
            if not k.startswith("SM64DS_") or k.startswith("SM64DS_TEST_LOCK")}

os.makedirs(DUMP, exist_ok=True)
ROWS = os.path.join(DUMP, "rows")
os.makedirs(ROWS, exist_ok=True)


def run_row(kind, ident):
    tag = ("level" if kind == "SM64DS_LEVEL" else "scene") + str(ident)
    rowdir = os.path.join(ROWS, tag)
    if os.path.isdir(rowdir):
        shutil.rmtree(rowdir, ignore_errors=True)
    os.makedirs(rowdir, exist_ok=True)

    env = dict(BASE_ENV)
    env[kind] = str(ident)
    if kind == "SM64DS_LEVEL":
        env["SM64DS_WINDOW_SELFTEST"] = FRAMES
    else:
        env["SM64DS_SCENE_FRAMES"] = FRAMES
    env["SM64DS_FAULTS_FATAL"] = "1"
    env["SM64DS_NO_FOCUS"] = "1"
    env["SM64DS_VOLUME"] = "0"
    env["SM64DS_HD_TEXTURES_DUMP"] = rowdir

    t0 = time.time()
    rc, out = "TIMEOUT", ""
    try:
        p = subprocess.run([EXE], cwd=EXEDIR, env=env, capture_output=True,
                           text=True, errors="replace", timeout=BUDGET,
                           creationflags=NOCON, startupinfo=SI)
        rc = p.returncode
        out = (p.stdout or "") + (p.stderr or "")
    except subprocess.TimeoutExpired as e:
        def s(v):
            if v is None:
                return ""
            return v if isinstance(v, str) else v.decode("utf-8", "replace")
        out = s(e.stdout) + s(e.stderr)
    dt = time.time() - t0
    names = sorted(f for f in os.listdir(rowdir) if f.endswith(".png"))
    if rc != 0:
        with open(os.path.join(rowdir, "stdout.txt"), "w",
                  encoding="utf-8", errors="replace") as f:
            f.write(out[-20000:])
    return tag, rc, dt, names


def main():
    merged = set(f for f in os.listdir(DUMP) if f.endswith(".png"))
    started = len(merged)
    rows = []
    plan = ([("SM64DS_LEVEL", i) for i in LEVELS] +
            [("SM64DS_SCENE", i) for i in SCENES])
    for kind, ident in plan:
        tag, rc, dt, names = run_row(kind, ident)
        new = [n for n in names if n not in merged]
        for n in names:
            dst = os.path.join(DUMP, n)
            if not os.path.exists(dst):
                shutil.copyfile(os.path.join(ROWS, tag, n), dst)
        merged.update(names)
        rows.append((tag, rc, dt, len(names), len(new)))
        print("%-10s rc=%-10s %6.1fs  unique=%-4d new=%-4d  total=%d"
              % (tag, rc, dt, len(names), len(new), len(merged)),
              flush=True)

    print()
    print("HARVEST SUMMARY")
    print("  rows run          : %d (%d level, %d scene)"
          % (len(rows), len(LEVELS), len(SCENES)))
    print("  rows rc=0         : %d" % sum(1 for r in rows if r[1] == 0))
    print("  textures at start : %d" % started)
    print("  textures now      : %d" % len(merged))
    print("  new this harvest  : %d" % (len(merged) - started))
    with open(os.path.join(DUMP, "harvest.tsv"), "w", encoding="utf-8") as f:
        f.write("row\trc\tseconds\tunique\tnew\n")
        for tag, rc, dt, n, new in rows:
            f.write("%s\t%s\t%.1f\t%d\t%d\n" % (tag, rc, dt, n, new))
    print("  per-row table     : %s" % os.path.join(DUMP, "harvest.tsv"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
