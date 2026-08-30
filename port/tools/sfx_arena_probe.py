#!/usr/bin/env python3
"""Arena vs control sound-request diff (lane SFX, run vsdec).

Boots walk_window.exe twice with the SAME sound instrumentation armed -- once
into a VS arena (SM64DS_VS_MAP) and once into a normal course (SM64DS_LEVEL) --
and writes both logs side by side. The coin probe in hal/sdat/consumer.cpp
fires the ROM's own Sound::PlayBank3(0x11/0x12, player+0x74) on a fixed period
in both runs, so the two logs answer the same question with the same request.

Every launch is silent and muted, the way port/tools/battery.py launches:
CREATE_NO_WINDOW for the console, SW_SHOWMINNOACTIVE for the game window,
SM64DS_NO_FOCUS=1 so it never takes the keyboard, SM64DS_VOLUME=0 so nothing
is audible. Sound behaviour is read off the logs, never off a speaker.

    python port/tools/sfx_arena_probe.py <build-dir> <out-dir> [frames]
"""
import os
import subprocess
import sys

NO_CONSOLE = getattr(subprocess, "CREATE_NO_WINDOW", 0)
if hasattr(subprocess, "STARTUPINFO"):
    SI_MIN = subprocess.STARTUPINFO()
    SI_MIN.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI_MIN.wShowWindow = 7          # SW_SHOWMINNOACTIVE
else:
    SI_MIN = None

# The four VS arenas, in SM64DS_VS_MAP order (hal/level_boot.cpp mounts
# 51->ov059, 43->ov051, 29->ov037, 42->ov050).
VS_MAPS = {0: 51, 1: 43, 2: 29, 3: 42}


def base_env(frames):
    env = dict(os.environ)
    # Instrumentation.
    env["SM64DS_SND_REQLOG"] = "1"
    env["SM64DS_SND_TRACE"] = "1"
    env["SM64DS_SND_COINPROBE"] = "60"
    # Silent and muted, always.
    env["SM64DS_NO_FOCUS"] = "1"
    env["SM64DS_VOLUME"] = "0"
    env["SM64DS_WINDOW_SELFTEST"] = str(frames)
    # Do not inherit anything that would change what the run does.
    for k in ("SM64DS_SKIP_CLASS", "SM64DS_DUAL_SCREEN", "SM64DS_CLICK_TEST",
              "SM64DS_TOUCH_PROBE", "SM64DS_COMMS_FANOUT", "SM64DS_COMMS_REPORT",
              "SM64DS_COMMS_ROLE", "SM64DS_COMMS_PORT", "SM64DS_COMMS_SLOT",
              "SM64DS_COMMS_INJECT", "SM64DS_RNG_MENU_FRAMES", "SM64DS_LEVEL",
              "SM64DS_VS_MAP", "SM64DS_COURSE_MUSIC"):
        env.pop(k, None)
    return env


def run(build, out, name, extra, frames):
    env = base_env(frames)
    env.update(extra)
    exe = os.path.join(build, "walk_window.exe")
    try:
        r = subprocess.run([exe], cwd=build, env=env, timeout=300,
                           capture_output=True, text=True,
                           creationflags=NO_CONSOLE, startupinfo=SI_MIN)
        rc, log = r.returncode, (r.stderr or "") + (r.stdout or "")
    except subprocess.TimeoutExpired as e:
        rc = "TIMEOUT"
        log = ((e.stderr or b"").decode("utf-8", "replace")
               + (e.stdout or b"").decode("utf-8", "replace"))
    path = os.path.join(out, name + ".log")
    with open(path, "w", encoding="utf-8") as f:
        f.write(log)
    reqs = [l for l in log.splitlines() if l.startswith("[req]")]
    drops = [l for l in reqs if "DROPPED" in l]
    print("  %-16s exit=%-8s %5d requests, %4d dropped  -> %s"
          % (name, rc, len(reqs), len(drops), path))
    return path, log


def main():
    build = sys.argv[1] if len(sys.argv) > 1 else r"C:\tmp\build\port"
    out = sys.argv[2] if len(sys.argv) > 2 else "."
    frames = sys.argv[3] if len(sys.argv) > 3 else "600"
    os.makedirs(out, exist_ok=True)

    print("CONTROL (normal courses):")
    for lvl in (1, 6):
        run(build, out, "control_lvl%d" % lvl, {"SM64DS_LEVEL": str(lvl)}, frames)

    print("ARENAS (SM64DS_VS_MAP):")
    for mi, lvl in sorted(VS_MAPS.items()):
        run(build, out, "arena_map%d_lvl%d" % (mi, lvl),
            {"SM64DS_VS_MAP": str(mi)}, frames)


if __name__ == "__main__":
    main()
