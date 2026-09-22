#!/usr/bin/env python
"""Boot every hosted level and every hosted scene IN A REAL WINDOW, 600 frames
each, keep going when one dies, and keep every crash report.

NOBODY HAS EVER MEASURED THIS SET. Three lanes reported level and scene boot
faults in passing today and each was scoped elsewhere, so the failing set was
only ever guessed at. This produces the table, and the per-row evidence a lane
needs to fix a row without re-deriving it.

FAULTS_FATAL IS ON FOR EVERY ROW AND THAT IS THE POINT. The port carries an
actor quarantine net that catches a faulting actor, freezes it and lets the
frame continue, so a broken boot can exit 0 and read as healthy. Earlier today
that net disguised a twenty-byte write to address 0 as "the screen draws
nothing". A row that only passes with the net on is not a passing row.

THE WINDOW IS DELIBERATE AND IT IS NOT FREE. battery.py keeps its rows headless
because a real window puts a live mouse into the touch path and a machine-global
key latch into the panel, and both can move the frame being measured. That is
accepted here: this run is to be WATCHED, and a row that passes on screen is the
thing being asked for. Rows that need a clean number get re-run headless.

Each row's crash evidence is copied to <out-dir>/<kind><id>/ while it is still
fresh, because the next row overwrites it.
"""

import os
import shutil
import subprocess
import sys
import time

# ABSOLUTE. Every row runs with cwd set to the exe's own directory (EXEDIR
# below) while this argument arrives relative to the tree root, so
# `python port/tools/bootsweep.py build/port/walk_window.exe ...` died on row
# one with FileNotFoundError [WinError 2]. port/tools/bootab.py has always
# called abspath here; this is that line.
EXE = os.path.abspath(sys.argv[1])
OUTDIR = sys.argv[2]
FRAMES = sys.argv[3] if len(sys.argv) > 3 else "600"
BUDGET = int(sys.argv[4]) if len(sys.argv) > 4 else 180

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import battery as B  # noqa: E402

EXEDIR = os.path.dirname(os.path.abspath(EXE))
ROOT = os.path.dirname(os.path.dirname(EXEDIR))
LEVELS = B.mounted_levels(ROOT)
SCENES = B.hosted_scenes(ROOT)

os.makedirs(OUTDIR, exist_ok=True)

# Every SM64DS_ knob is dropped before the row's own is applied, so nothing
# inherited from this shell can decide what a row runs. battery.py's rule.
#
# THE THREE SM64DS_TEST_LOCK* VARIABLES ARE THE EXCEPTION, exactly as
# port/tools/bootab.py has them. They are not a knob a row reads; they are how
# a launch takes the machine-wide windowed test slot. Dropping them made every
# row run OUTSIDE the slot, so a sweep here collided with any other lane's
# battery or capture and produced random rc=1 rows on levels that were fine.
# Export SM64DS_TEST_LOCK=1 SM64DS_TEST_LOCK_PATH=C:/tmp/sm64ds-test-slot/slot.lock
# SM64DS_TEST_LOCK_TIMEOUT=10800 before running and every row serialises.
DROP = [k for k in os.environ
        if k.startswith("SM64DS_") and not k.startswith("SM64DS_TEST_LOCK")]

# HEADLESS AND SILENT, because this runs beside somebody working. No
# SM64DS_SCENE_WINDOW: a headless row must stay headless, and an inherited one
# opens a REAL window, which puts a live mouse into the touch path and a
# machine-global key latch into the panel, both of which move the frame being
# measured. CREATE_NO_WINDOW keeps the console out of the way (walk_window is a
# console-subsystem binary, so a parent with no console of its own gets a fresh
# console window per child), and SW_SHOWMINNOACTIVE covers anything that does
# manage to open one: minimized, never activated, never focused.
SI = subprocess.STARTUPINFO()
SI.dwFlags |= subprocess.STARTF_USESHOWWINDOW
SI.wShowWindow = 7                                   # SW_SHOWMINNOACTIVE
NOCON = getattr(subprocess, "CREATE_NO_WINDOW", 0)

# What the port leaves behind after a fault, in the order the house notes say to
# read it: the play log first, then the crash and exit stamps.
ARTIFACTS = ("crash.txt", "exit.txt", "census.txt", "playlog")


def harvest(dest):
    os.makedirs(dest, exist_ok=True)
    kept = []
    for name in ARTIFACTS:
        src = os.path.join(EXEDIR, name)
        if not os.path.exists(src):
            continue
        dst = os.path.join(dest, name)
        try:
            if os.path.isdir(src):
                shutil.copytree(src, dst, dirs_exist_ok=True)
                n = sum(len(f) for _r, _d, f in os.walk(dst))
                kept.append("%s(%d)" % (name, n))
            else:
                shutil.copy2(src, dst)
                kept.append(name)
        except OSError as e:
            kept.append("%s:copyfail %s" % (name, e))
    return kept


def clear_artifacts():
    """So a row cannot inherit the previous row's crash report and read as
    broken when it is not. This is the stale-artifact trap in miniature."""
    for name in ARTIFACTS:
        p = os.path.join(EXEDIR, name)
        try:
            if os.path.isdir(p):
                shutil.rmtree(p, ignore_errors=True)
            elif os.path.exists(p):
                os.remove(p)
        except OSError:
            pass


def run(kind, ident, label):
    clear_artifacts()
    env = {k: v for k, v in os.environ.items() if k not in DROP}
    env[kind] = str(ident)
    env["SM64DS_SCENE_FRAMES"] = FRAMES
    env["SM64DS_FAULTS_FATAL"] = "1"
    env["SM64DS_VOLUME"] = "0"                # silent: this runs beside a person
    env["SM64DS_NO_FOCUS"] = "1"              # never take the keyboard
    t0 = time.time()
    timed_out = False
    try:
        p = subprocess.run([EXE], cwd=EXEDIR, env=env, capture_output=True,
                           text=True, timeout=BUDGET,
                           creationflags=NOCON, startupinfo=SI)
        rc = p.returncode
        out = (p.stdout or "") + (p.stderr or "")
    except subprocess.TimeoutExpired as e:
        timed_out = True
        rc = "TIMEOUT"
        # text=True, so these come back as str already. Decoding them raises,
        # and the raise lands in the one path nothing had exercised: the first
        # timeout killed the whole sweep on row one.
        def _s(v):
            if v is None:
                return ""
            return v if isinstance(v, str) else v.decode("utf-8", "replace")
        out = _s(e.stdout) + _s(e.stderr)
    dt = time.time() - t0

    ok = (rc == 0)
    dest = os.path.join(OUTDIR, "%s%d" % (label, ident))
    kept = []
    if not ok:
        kept = harvest(dest)
        with open(os.path.join(dest, "stdout.txt"), "w",
                  encoding="utf-8", errors="replace") as f:
            f.write(out)

    note = ""
    low = out.lower()
    for key in ("unhosted", "fs catalog", "loadarchive", "quarantine",
                "unhandled", "assert", "out of memory"):
        if key in low:
            note = key
            break
    if not note and timed_out:
        note = "timeout after %ds" % BUDGET
    if not note and isinstance(rc, int) and rc != 0:
        note = "rc 0x%08x" % (rc & 0xFFFFFFFF)
    return ok, rc, round(dt, 1), note, kept


rows = []
plan = [("SM64DS_LEVEL", LEVELS, "level"), ("SM64DS_SCENE", SCENES, "scene")]
total = sum(len(ids) for _k, ids, _l in plan)
n = 0
for kind, ids, label in plan:
    for i in ids:
        n += 1
        ok, rc, dt, note, kept = run(kind, i, label)
        rows.append((label, i, "PASS" if ok else "FAIL", rc, dt, note))
        print("[%2d/%2d] %-5s %-3d %-4s rc=%-12s %6.1fs  %s%s"
              % (n, total, label, i, "PASS" if ok else "FAIL", rc, dt, note,
                 ("  kept: " + ",".join(kept)) if kept else ""), flush=True)

tsv = os.path.join(OUTDIR, "bootsweep.tsv")
with open(tsv, "w") as f:
    f.write("kind\tid\tverdict\trc\tseconds\tnote\n")
    for r in rows:
        f.write("\t".join(str(x) for x in r) + "\n")

lv = [r for r in rows if r[0] == "level"]
sc = [r for r in rows if r[0] == "scene"]
print("\nlevels: %d/%d boot" % (sum(1 for r in lv if r[2] == "PASS"), len(lv)))
print("scenes: %d/%d boot" % (sum(1 for r in sc if r[2] == "PASS"), len(sc)))
fails = [r for r in rows if r[2] == "FAIL"]
print("failing rows: %d" % len(fails))
for r in fails:
    print("  %s %s  %s" % (r[0], r[1], r[5]))
print("table -> %s" % tsv)
