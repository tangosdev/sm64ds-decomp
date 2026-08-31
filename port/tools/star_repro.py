#!/usr/bin/env python3
"""Star-collect freeze repro driver (lane STAR, run vsdec).

Spawns a Power Star (actor 178) at the player through the level's own spawn
path, runs a bounded silent run, and captures the state of every gate the
star's post-collect sequence sits on.

Silent by construction, the battery.py way: CREATE_NO_WINDOW +
STARTUPINFO/SW_SHOWMINNOACTIVE (minimized, never activated), SM64DS_NO_FOCUS=1
and SM64DS_VOLUME=0.

Usage:
  star_repro.py --build DIR --out FILE [--level N] [--frames N]
                [--pad SPEC] [--windowed] [--extra K=V ...]
"""
import argparse
import os
import subprocess
import sys

SW_SHOWMINNOACTIVE = 7


def spawner_kwargs():
    """battery.py:765-787's own pattern: no console, minimized, never activated."""
    flags = getattr(subprocess, "CREATE_NO_WINDOW", 0)
    si = None
    if hasattr(subprocess, "STARTUPINFO"):
        si = subprocess.STARTUPINFO()
        si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        si.wShowWindow = SW_SHOWMINNOACTIVE
    return {"creationflags": flags, "startupinfo": si}


def build_env(args):
    env = dict(os.environ)
    # knobs an inherited environment must not get to decide
    for k in ("SM64DS_SKIP_CLASS", "SM64DS_DUAL_SCREEN", "SM64DS_CLICK_TEST",
              "SM64DS_VS_MAP", "SM64DS_VS_MODE", "SM64DS_SCENE",
              "SM64DS_COMMS_RELAY", "SM64DS_COMMS_ROLE", "SM64DS_COMMS_PORT",
              "SM64DS_COMMS_FANOUT", "SM64DS_MENU"):
        env.pop(k, None)
    env["SM64DS_LEVEL"] = str(args.level)
    env["SM64DS_NO_FOCUS"] = "1"        # never take the keyboard
    env["SM64DS_VOLUME"] = "0"          # muted
    env["SM64DS_NO_DIALOG"] = "1"       # never block on a modal
    if args.spawn:
        env["SM64DS_SPAWN_ACTOR"] = args.spawn
    if args.windowed:
        # windowed-with-a-budget: the only path SM64DS_PAD_TEST can reach
        env.pop("SM64DS_WINDOW_SELFTEST", None)
        env["SM64DS_SCENE_WINDOW"] = "1"
        env["SM64DS_SCENE_FRAMES"] = str(args.frames)
    else:
        env["SM64DS_WINDOW_SELFTEST"] = str(args.frames)
        env.pop("SM64DS_SCENE_WINDOW", None)
        env.pop("SM64DS_SCENE_FRAMES", None)
    if args.pad:
        env["SM64DS_PAD_TEST"] = args.pad
    for kv in args.extra or []:
        k, _, v = kv.partition("=")
        env[k] = v
    return env


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--build", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--level", type=int, default=6)
    ap.add_argument("--frames", type=int, default=600)
    ap.add_argument("--spawn", default="178")
    ap.add_argument("--pad", default=None)
    ap.add_argument("--windowed", action="store_true")
    ap.add_argument("--timeout", type=int, default=600)
    ap.add_argument("--extra", action="append")
    args = ap.parse_args()

    exe = os.path.join(args.build, "walk_window.exe")
    if not os.path.isfile(exe):
        print("no walk_window.exe at " + exe, file=sys.stderr)
        return 2
    env = build_env(args)
    try:
        p = subprocess.run([exe], cwd=args.build, env=env, timeout=args.timeout,
                           capture_output=True, text=True, errors="replace",
                           **spawner_kwargs())
        rc, out, err = p.returncode, p.stdout, p.stderr
    except subprocess.TimeoutExpired as e:
        rc = "TIMEOUT"
        out = (e.stdout or b"").decode("utf-8", "replace") if isinstance(e.stdout, bytes) else (e.stdout or "")
        err = (e.stderr or b"").decode("utf-8", "replace") if isinstance(e.stderr, bytes) else (e.stderr or "")

    with open(args.out, "w", encoding="utf-8", errors="replace") as f:
        f.write("=== cmd: %s (cwd %s)\n" % (exe, args.build))
        for k in sorted(env):
            if k.startswith("SM64DS_"):
                f.write("=== env %s=%s\n" % (k, env[k]))
        f.write("=== rc: %s\n" % (rc,))
        f.write("=== stdout ===\n")
        f.write(out or "")
        f.write("\n=== stderr ===\n")
        f.write(err or "")
    print("rc=%s  out=%s (%d+%d bytes)" % (rc, args.out, len(out or ""), len(err or "")))
    return 0


if __name__ == "__main__":
    sys.exit(main())
