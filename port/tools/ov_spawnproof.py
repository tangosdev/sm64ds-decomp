#!/usr/bin/env python3
"""ov_spawnproof -- did the level actually BUILD the class, or only fail to
crash without it?

A registry row is not evidence. This runs one headless level through
walk_window.exe with the battery's own launch (SM64DS_NO_FOCUS, minimized and
never activated, CREATE_NO_WINDOW, SM64DS_VOLUME=0, SM64DS_FAULTS_FATAL=1)
and reads the boot census back: an id must appear on a `+ <id> x<n>` SPAWNED
row, must NOT appear on a `- <id>` skipped row, and the process must exit 0.

It borrows run(), selftest_env(), NO_CONSOLE and SI_MIN from
port/tools/battery.py rather than spawning its own process, so a proof run
obeys the same windowed-slot lock and the same silent/muted presentation the
battery does.

    python port/tools/ov_spawnproof.py <root> --level 31 --id 314
    python port/tools/ov_spawnproof.py <root> --level 17 --id 348 --frames 300
"""
import argparse
import os
import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import battery as B  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--level", type=int, required=True)
    ap.add_argument("--id", type=lambda s: int(s, 0), action="append",
                    required=True)
    ap.add_argument("--frames", default=B.SELFTEST_FRAMES)
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    build = root / "build/port"
    exe = build / "walk_window.exe"
    if not exe.exists():
        sys.exit("no %s -- build first" % exe)

    env = B.selftest_env(a.level)
    env["SM64DS_WINDOW_SELFTEST"] = str(a.frames)
    env["SM64DS_VOLUME"] = "0"
    r = B.run([str(exe)], cwd=str(build), env=env)
    out = (r.stdout or "") + (r.stderr or "")

    print("level %d, %s frames, rc %d" % (a.level, a.frames, r.returncode))
    census = []
    keep = False
    for ln in out.splitlines():
        if ln.startswith("[census]"):
            keep = True
        elif keep and not ln.startswith("      "):
            keep = False
        if keep:
            census.append(ln)
    for ln in census[:1]:
        print("   " + ln)

    ok = r.returncode == 0
    for aid in a.id:
        sp = re.search(r"^\s+\+\s+%d\s+x(\d+)\s+(\S+)" % aid, out, re.M)
        sk = re.search(r"^\s+-\s+%d\s+x(\d+)" % aid, out, re.M)
        if sp:
            print("   SPAWNED  id %-4d x%-3s %s" % (aid, sp.group(1),
                                                    sp.group(2)))
        elif sk:
            print("   SKIPPED  id %-4d x%s  <-- NOT SEATED" % (aid, sk.group(1)))
            ok = False
        else:
            print("   ABSENT   id %-4d  (this level places none)" % aid)
            ok = False
    # A decline of an id this proof is NOT about is the level's own standing
    # skip list, not this seat's regression -- level 21 declines 0x63 and 0xc0
    # (CageLift and the ov002 0x021096b0 class, both majority inferred stubs)
    # on every build. Report those, fail only on ours.
    mine = {("0x%x" % i) for i in a.id}
    for ln in out.splitlines():
        if "FAULT" in ln or "UNHOSTED" in ln:
            print("   ! " + ln.strip())
            ok = False
        elif "spawn-declined" in ln:
            hit = any(m in ln for m in mine)
            print("   %s %s" % ("!" if hit else "(pre-existing)", ln.strip()))
            if hit:
                ok = False
    print("VERDICT:", "GREEN" if ok else "RED")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
