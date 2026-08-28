"""Quiet A/B capture for the Luigi-head lane.

Launches walk_window.exe with the battery's quiet-spawner pattern
(CREATE_NO_WINDOW + SW_SHOWMINNOACTIVE + SM64DS_NO_FOCUS + SM64DS_VOLUME=0),
once per requested character, and moves the selftest BMP to a per-char name.

Usage: python port/tools/luigi_capture.py [outdir] [frames] [level]
"""
import os
import shutil
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")

NO_CONSOLE = getattr(subprocess, "CREATE_NO_WINDOW", 0)
if hasattr(subprocess, "STARTUPINFO"):
    SI_MIN = subprocess.STARTUPINFO()
    SI_MIN.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI_MIN.wShowWindow = 7  # SW_SHOWMINNOACTIVE
else:
    SI_MIN = None


def capture(char, outdir, frames, level, extra=None, tag=""):
    env = dict(os.environ,
               SM64DS_LEVEL=str(level),
               SM64DS_WINDOW_SELFTEST=str(frames),
               SM64DS_CHARACTER=str(char),
               SM64DS_NO_FOCUS="1",
               SM64DS_VOLUME="0",
               SM64DS_FAULTS_FATAL="1")
    for k in ("SM64DS_SKIP_CLASS", "SM64DS_DUAL_SCREEN", "SM64DS_CLICK_TEST",
              "SM64DS_TOUCH_PROBE", "SM64DS_COMMS_FANOUT", "SM64DS_COMMS_REPORT",
              "SM64DS_COMMS_ROLE", "SM64DS_VS_PLAYERS"):
        env.pop(k, None)
    if extra:
        env.update(extra)
    bmp = os.path.join(ROOT, "walk_window_selftest.bmp")
    if os.path.exists(bmp):
        os.remove(bmp)
    p = subprocess.run([EXE], cwd=ROOT, env=env, timeout=300,
                       capture_output=True, text=True,
                       creationflags=NO_CONSOLE, startupinfo=SI_MIN)
    name = "char%d%s" % (char, tag)
    log = os.path.join(outdir, name + ".log")
    with open(log, "w", encoding="utf-8", errors="replace") as f:
        f.write("=== exit %s ===\n" % p.returncode)
        f.write("=== stdout ===\n")
        f.write(p.stdout or "")
        f.write("\n=== stderr ===\n")
        f.write(p.stderr or "")
    dst = os.path.join(outdir, name + ".bmp")
    if os.path.exists(bmp):
        shutil.move(bmp, dst)
        print("%s: exit=%s bmp=%s" % (name, p.returncode, dst))
    else:
        print("%s: exit=%s NO BMP (see %s)" % (name, p.returncode, log))
    return p.returncode


def main():
    outdir = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "luigi_evidence")
    frames = sys.argv[2] if len(sys.argv) > 2 else "300"
    level = sys.argv[3] if len(sys.argv) > 3 else "1"
    os.makedirs(outdir, exist_ok=True)
    for c in (0, 1):
        capture(c, outdir, frames, level)


if __name__ == "__main__":
    main()
