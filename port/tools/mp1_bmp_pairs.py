#!/usr/bin/env python3
"""MP1's solo-regression BMP capture: three scenes and one level arm.

    python port/tools/mp1_bmp_pairs.py <build-dir> <out-dir> <tag>

Runs the same rows on any build dir, so the seam build and a base build can be
compared. SILENT: CREATE_NO_WINDOW and STARTUPINFO SW_SHOWMINNOACTIVE, exactly
as port/tools/battery.py does it, plus SM64DS_NO_FOCUS. Nothing appears.

WHAT MAKES THIS COMPARISON MEAN ANYTHING. port/tools/battery.py's header:
the rendered frame depends on the ABSOLUTE ADDRESSES of hosted DS globals, so
an insertion INTERIOR to .dsstate changes the picture (its own table: 1354
pixels for 64 bytes at 296 frames) and an equal .dsstate BASE is necessary but
not sufficient. MP1's three new hosted globals are therefore placed at
.dsstate$wcomms, which sorts above every family in the tree and below the $zzz
sentinel, so no existing hosted global moves. The dsstate_guard line at link
time is the check: same base, span grown only at the top.
"""
import hashlib
import os
import subprocess
import sys

FRAMES = "300"
SCENES = ["1", "374", "384"]
LEVEL = "1"

NO_CONSOLE = getattr(subprocess, "CREATE_NO_WINDOW", 0)
if hasattr(subprocess, "STARTUPINFO"):
    SI_MIN = subprocess.STARTUPINFO()
    SI_MIN.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI_MIN.wShowWindow = 7          # SW_SHOWMINNOACTIVE
else:
    SI_MIN = None

# Every knob that could let the caller's shell decide what a comparison run
# renders. Same list and same reason as battery.py's two env builders.
DROP = ("SM64DS_LEVEL", "SM64DS_SCENE", "SM64DS_SKIP_CLASS",
        "SM64DS_SCENE_NO_RENDER", "SM64DS_SCENE_BMP",
        "SM64DS_SCENE_BMP_STACKED", "SM64DS_SCENE_TRACE", "SM64DS_SCENE_SLOT9",
        "SM64DS_SCENE_SUBLEVEL", "SM64DS_DUAL_SCREEN", "SM64DS_SCENE_WINDOW",
        "SM64DS_CLICK_TEST", "SM64DS_PAD_TEST", "SM64DS_TOUCH_PROBE",
        "SM64DS_MG_SCORE_TRACE", "SM64DS_RNG_MENU_FRAMES",
        "SM64DS_WINDOW_SELFTEST", "SM64DS_SCENE_FRAMES", "PORT_WATCHDOG",
        "SM64DS_SCENE_NO_TOUCH")


def base_env():
    env = dict(os.environ)
    for k in DROP:
        env.pop(k, None)
    env["SM64DS_NO_FOCUS"] = "1"
    env["SM64DS_FAULTS_FATAL"] = "1"
    return env


def sha(path):
    if not os.path.exists(path):
        return None
    h = hashlib.sha256()
    with open(path, "rb") as f:
        h.update(f.read())
    return h.hexdigest()


def main():
    if len(sys.argv) < 4:
        print(__doc__)
        return 2
    build, out, tag = os.path.abspath(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
    os.makedirs(out, exist_ok=True)
    exe = os.path.join(build, "walk_window.exe")
    rows = []

    for scene in SCENES:
        dst = os.path.join(out, "%s_scene%s_stacked.bmp" % (tag, scene))
        if os.path.exists(dst):
            os.remove(dst)
        env = base_env()
        env["SM64DS_SCENE"] = scene
        env["SM64DS_SCENE_FRAMES"] = FRAMES
        env["SM64DS_SCENE_BMP_STACKED"] = dst
        r = subprocess.run([exe], cwd=build, env=env, timeout=900,
                           capture_output=True, text=True,
                           creationflags=NO_CONSOLE, startupinfo=SI_MIN)
        rows.append(("scene %s stacked" % scene, r.returncode, dst, sha(dst)))

    # The level arm is walk_window's own selftest BMP, written next to the exe.
    src = os.path.join(build, "walk_window_selftest.bmp")
    if os.path.exists(src):
        os.remove(src)
    env = base_env()
    env["SM64DS_LEVEL"] = LEVEL
    env["SM64DS_WINDOW_SELFTEST"] = FRAMES
    r = subprocess.run([exe], cwd=build, env=env, timeout=900,
                       capture_output=True, text=True,
                       creationflags=NO_CONSOLE, startupinfo=SI_MIN)
    dst = os.path.join(out, "%s_level%s_selftest.bmp" % (tag, LEVEL))
    if os.path.exists(src):
        with open(src, "rb") as a, open(dst, "wb") as b:
            b.write(a.read())
    rows.append(("level %s selftest" % LEVEL, r.returncode, dst, sha(dst)))

    print("build : %s" % build)
    print("exe   : %s  sha256 %s" % (exe, sha(exe)))
    for name, rc, path, digest in rows:
        print("%-22s rc=%-3d %s  %s" %
              (name, rc, digest or "(NO FILE)", os.path.basename(path)))
    return 0 if all(rc == 0 and d for _, rc, _, d in rows) else 1


if __name__ == "__main__":
    sys.exit(main())
