#!/usr/bin/env python3
"""The port's full verification battery, one command.

Everything the merge gate runs, in order, stopping at the first failure:

  1. build            port/build-port.cmd (32-bit MSVC, ninja)
  2. smoke suite      every smoke_*.exe in build/port, exit 0 each
  3. level selftests  walk_window.exe, SM64DS_WINDOW_SELFTEST=300, on every
                      mounted level (the PORT_LEVEL_OVERLAYS list's levels)
  4. linkage          port/tools/linkage.py -- the linked count is printed and
                      compared against --linked-floor if given (a merge must
                      never lower it)
  5. ptr_audit        port/tools/ptr_audit.py -- unhosted code pointers must
                      stay at zero

    python port/tools/battery.py [repo-root] [--linked-floor N] [--skip-build]

Exit 0 all green, 1 first red, with a one-line verdict per step so a log tail
reads as a checklist.
"""

import os
import re
import subprocess
import sys

# Every mounted level, port_level_table[] in hal/level_boot.cpp -- a new
# level mount adds its id here or the battery silently under-tests.
LEVELS = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 36, 37, 38, 40)
SELFTEST_FRAMES = "300"
STEP_TIMEOUT = 600


def run(cmd, cwd, env=None, timeout=STEP_TIMEOUT):
    return subprocess.run(cmd, cwd=cwd, env=env, timeout=timeout,
                          capture_output=True, text=True)


def main():
    args = [a for a in sys.argv[1:]]
    floor = 0
    if "--linked-floor" in args:
        i = args.index("--linked-floor")
        floor = int(args[i + 1])
        del args[i:i + 2]
    skip_build = "--skip-build" in args
    if skip_build:
        args.remove("--skip-build")
    root = os.path.abspath(args[0] if args else ".")
    build = os.path.join(root, "build", "port")

    if not skip_build:
        r = run(["cmd", "/c", os.path.join(root, "port", "build-port.cmd")],
                root)
        if r.returncode:
            print("build: FAIL")
            print(r.stdout[-2000:])
            print(r.stderr[-2000:])
            return 1
        print("build: ok")

    smokes = sorted(f for f in os.listdir(build)
                    if f.startswith("smoke") and f.endswith(".exe"))
    for exe in smokes:
        r = run([os.path.join(build, exe)], build)
        tail = (r.stdout.strip().splitlines() or [""])[-1][:90]
        if r.returncode:
            print(f"{exe}: FAIL rc={r.returncode} {tail}")
            return 1
        print(f"{exe}: ok  {tail}")

    for lvl in LEVELS:
        env = dict(os.environ,
                   SM64DS_LEVEL=str(lvl),
                   SM64DS_WINDOW_SELFTEST=SELFTEST_FRAMES)
        r = run([os.path.join(build, "walk_window.exe")], build, env=env)
        if r.returncode:
            print(f"selftest level {lvl}: FAIL rc={r.returncode}")
            print(r.stdout[-1500:])
            return 1
        print(f"selftest level {lvl}: ok")

    r = run([sys.executable, os.path.join(root, "port", "tools", "linkage.py"),
             root], root)
    m = re.search(r"linked into walk_window\s*:\s*(\d+)\s*\(([\d.]+)%\)",
                  r.stdout)
    if not m:
        print("linkage: FAIL (no linked count in output)")
        return 1
    linked = int(m.group(1))
    print(f"linkage: {linked} ({m.group(2)}%)")
    if floor and linked < floor:
        print(f"linkage: FAIL, below the floor of {floor}")
        return 1

    r = run([sys.executable, os.path.join(root, "port", "tools", "ptr_audit.py")],
            root)
    m = re.search(r"^(\d+) carry code pointers AND no host TU names them",
                  r.stdout, re.M)
    if not m:
        print("ptr_audit: FAIL (no verdict line)")
        return 1
    if int(m.group(1)) != 0:
        print(f"ptr_audit: FAIL, {m.group(1)} unhosted code pointers")
        return 1
    print("ptr_audit: 0 unhosted code pointers")

    print("battery: ALL GREEN")
    return 0


if __name__ == "__main__":
    sys.exit(main())
