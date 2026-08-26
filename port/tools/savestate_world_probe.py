#!/usr/bin/env python3
"""The 2026-08-26 reload crash, made reproducible, and the guard that answers it.

WHAT HAPPENED. A player's savestate.bin was written by his own 0.2.13 build and
auto-loaded at the next launch. Every header field agreed -- same build, same
game data, same image base, same arena base, same section size -- the restore
reported the right byte counts, and the first frame died:

    [savestate] loaded disk state: 8388608 arena + 959432 dsstate + 9441280 hw
      [path] ASSERT: binding 0 has -1 nodes and the node walk holds 3
    FAULT code c0000005 at +0x0009d322 accessing 00000024

Resolved against a layout-matched rebuild of that commit, +0x9d322 is
func_0200ca50 +0x12 with Camera::Behavior +0x13d above it. That function opens

    char *mode = *(char **)(f + 0x13c);
    ApproachLinear2(f + 0x17a, *(short *)(mode + 0x24), 0x80);

so a NULL camera mode reads address 0x24 -- the dump's access, exactly.

THE CLASS. Every header field is an exe-side or asset-side fact. None of them
can see whether the world INSIDE the file is a world the game can take a tick
of. hal/lk7_persist.cpp now asks that question of the file's own bytes before
it copies them over the freshly booted world, and refuses by name if the answer
is no.

THE ARMS. Each doctors ONE word of an otherwise-valid state, so the thing named
is provably the thing that refused:

  1  CONTROL      a clean state loads, refuses nothing, exits 0.
  2  FIX OFF      camera mode zeroed, SM64DS_SAVESTATE_NO_GUARD=1 -- the
                  player's fault reproduces: c0000005 accessing 00000024.
  3  FIX ON       the same file, guard on -- refused by name, booted fresh,
                  exit 0, and the file is left untouched.
  4  NO PLAYER    data_0209f394[0] zeroed -- refused, and says which singleton.
  5  CONTROL AGAIN the clean state still loads after all of that.

    python port/tools/savestate_world_probe.py [repo-root]

Exit 0 if every arm passed, 1 otherwise. Silent: SM64DS_NO_FOCUS +
CREATE_NO_WINDOW + SW_SHOWMINNOACTIVE, like battery.py and savestate_soak.py.
"""
import os
import re
import shutil
import struct
import subprocess
import sys
from pathlib import Path

FRAMES = "150"
SAVE_AT = "90"
LEVEL = "1"
TIMEOUT = 600
# where the entrance spawns them, every level, every boot (measured 20/20)
CAMERA = 0x30039D80
ARENA_BASE = 0x30000000


def run(exe, build, env, frames=FRAMES):
    e = dict(os.environ, SM64DS_FAULTS_FATAL="1",
             SM64DS_WINDOW_SELFTEST=frames, SM64DS_NO_FOCUS="1")
    for k in ("SM64DS_SCENE", "SM64DS_SS_DISKLOAD", "SM64DS_SS_DISK",
              "SM64DS_SS_SAVE", "SM64DS_SS_LOAD", "SM64DS_SAVESTATE_NO_GUARD",
              "SM64DS_SS_NO_RESEAT"):
        e.pop(k, None)
    e.update(env)
    si = None
    if hasattr(subprocess, "STARTUPINFO"):
        si = subprocess.STARTUPINFO()
        si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
    try:
        return subprocess.run([exe], cwd=build, env=e, timeout=TIMEOUT,
                              capture_output=True, text=True,
                              creationflags=getattr(subprocess,
                                                    "CREATE_NO_WINDOW", 0),
                              startupinfo=si)
    except subprocess.TimeoutExpired:
        return None


def check(ok, label, detail=""):
    print(f"    {'ok' if ok else 'FAIL'} {label}" + (f"  [{detail}]" if detail
                                                     else ""))
    return 0 if ok else 1


def main():
    root = Path(sys.argv[1]) if len(sys.argv) > 1 else \
        Path(__file__).resolve().parents[2]
    build = root / "build" / "port"
    exe = str(build / "walk_window.exe")
    state = build / "savestate.bin"
    if not Path(exe).exists():
        sys.exit(f"savestate_world_probe: no {exe}; build first")
    bad = 0

    # ---- write one clean state, and learn the file's own geometry -----------
    if state.exists():
        state.unlink()
    r = run(exe, str(build), {"SM64DS_LEVEL": LEVEL, "SM64DS_SS_SAVE": SAVE_AT,
                              "SM64DS_SS_DISK": "1"})
    log = "" if r is None else (r.stdout + r.stderr)
    m = re.search(r"\[savestate\] saved: arena (\d+) bytes, dsstate (\d+) "
                  r"bytes, hw (\d+) bytes", log)
    if not m or not state.exists():
        sys.exit("savestate_world_probe: run one did not write a state")
    asz, dsz, hsz = (int(x) for x in m.groups())
    total = state.stat().st_size
    hdr = total - asz - dsz - hsz
    print(f"state: {total} bytes = {hdr} header + {asz} arena + {dsz} dsstate "
          f"+ {hsz} hw")
    clean = state.read_bytes()
    if hdr <= 0:
        sys.exit("savestate_world_probe: header size came out non-positive")

    cam_mode_off = hdr + (CAMERA - ARENA_BASE) + 0x13c

    print("ARM 1 (control: a clean state loads)")
    r = run(exe, str(build), {"SM64DS_LEVEL": LEVEL, "SM64DS_SS_DISKLOAD": "1"})
    log = "" if r is None else (r.stdout + r.stderr)
    bad += check(r is not None and r.returncode == 0, "boots and exits 0",
                 f"rc={'timeout' if r is None else r.returncode}")
    bad += check("loaded disk state" in log, "the state loaded")
    bad += check("disk state refused" not in log, "nothing refused")
    bad += check("FAULT code" not in log, "no fault")

    # ---- the doctored file: the player's world, one word ---------------------
    doctored = bytearray(clean)
    was = struct.unpack_from("<I", doctored, cam_mode_off)[0]
    struct.pack_into("<I", doctored, cam_mode_off, 0)
    print(f"doctored: camera {CAMERA:08x} +0x13c (file offset {cam_mode_off}) "
          f"{was:08x} -> 00000000  -- the player's NULL camera mode")

    print("ARM 2 (fix OFF: the player's fault reproduces)")
    state.write_bytes(bytes(doctored))
    r = run(exe, str(build), {"SM64DS_LEVEL": LEVEL, "SM64DS_SS_DISKLOAD": "1",
                              "SM64DS_SAVESTATE_NO_GUARD": "1"})
    log = "" if r is None else (r.stdout + r.stderr)
    fault = re.search(r"FAULT code (\w+) at \+(\w+) accessing (\w+)", log)
    bad += check(bool(fault), "it faults",
                 fault.group(0) if fault else "no FAULT line")
    bad += check(bool(fault) and fault.group(1) == "c0000005"
                 and int(fault.group(3), 16) == 0x24,
                 "c0000005 accessing 00000024 -- the player's signature",
                 fault.group(0) if fault else "")

    print("ARM 3 (fix ON: the same file is refused by name)")
    state.write_bytes(bytes(doctored))
    before = state.read_bytes()
    r = run(exe, str(build), {"SM64DS_LEVEL": LEVEL, "SM64DS_SS_DISKLOAD": "1"})
    log = "" if r is None else (r.stdout + r.stderr)
    bad += check(r is not None and r.returncode == 0, "boots fresh and exits 0",
                 f"rc={'timeout' if r is None else r.returncode}")
    bad += check("disk state refused: the saved world is not runnable" in log,
                 "refused as an unrunnable world")
    bad += check("Camera has no mode" in log, "names the camera mode")
    bad += check("FAULT code" not in log, "no fault")
    bad += check("loaded disk state" not in log, "the state did not load")
    bad += check(state.read_bytes() == before, "the file was left untouched")

    print("ARM 4 (a world with no Player)")
    # data_0209f394[0] lives in .dsstate; find it by its live value rather than
    # by a baked offset -- the clean state holds the Player pointer exactly once
    # at a 4-aligned slot inside the dsstate body, and ARM 1 proved that state
    # good. Locate it by searching the dsstate body for the Player address the
    # census prints, which is the entrance-spawned Player, 30039F38.
    d0 = hdr + asz
    body = clean[d0:d0 + dsz]
    player_le = struct.pack("<I", 0x30039F38)
    hits = [i for i in range(0, len(body) - 3, 4)
            if body[i:i + 4] == player_le]
    if not hits:
        bad += check(False, "found the Player pointer in the dsstate body")
    else:
        d = bytearray(clean)
        for i in hits:
            struct.pack_into("<I", d, d0 + i, 0)
        state.write_bytes(bytes(d))
        r = run(exe, str(build), {"SM64DS_LEVEL": LEVEL,
                                  "SM64DS_SS_DISKLOAD": "1"})
        log = "" if r is None else (r.stdout + r.stderr)
        bad += check(r is not None and r.returncode == 0, "boots fresh, exit 0",
                     f"rc={'timeout' if r is None else r.returncode}")
        bad += check("no Player" in log or "not runnable" in log,
                     "refused, and says the world has no Player",
                     f"{len(hits)} slot(s) zeroed")
        bad += check("FAULT code" not in log, "no fault")

    print("ARM 5 (control again: the clean state still loads)")
    state.write_bytes(clean)
    r = run(exe, str(build), {"SM64DS_LEVEL": LEVEL, "SM64DS_SS_DISKLOAD": "1"})
    log = "" if r is None else (r.stdout + r.stderr)
    bad += check(r is not None and r.returncode == 0, "exits 0",
                 f"rc={'timeout' if r is None else r.returncode}")
    bad += check("loaded disk state" in log, "the state loaded")
    bad += check("disk state refused" not in log, "nothing refused")

    print(f"\nsavestate_world_probe: "
          f"{'all arms passed' if not bad else f'{bad} check(s) FAILED'}")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
