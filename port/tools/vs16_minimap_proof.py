#!/usr/bin/env python3
"""VS16 MINIMAP proof: sixteen windows, the high seats do not fault in
Minimap::Render.

WHAT THIS EXISTS FOR. src/_ZN7Minimap6RenderEv.cpp and
src/_ZN7Minimap8BehaviorEv.cpp index per-player tables by PLAYER NUMBER:
the self-icon path reads this->mPlayerIconX/Y[idx] and
OAM::MM_VS_PLAYER_ICONS[pl->unk8 + idx*4] with idx = data_0209f250, the
LOCAL slot. On the cartridge idx is 0..3 and the tables are four wide. At
sixteen players a console seated on slot 8/12/14/15 walks the icon table off
its end, hands OAM::RenderSub a garbage attribute pointer, and faults
(c0000005) in Minimap::Render at the frame the minimap first draws.

src/ is byte-frozen, so the fix is a HOST COPY of those two methods
(port/unmatched/Minimap_Render.cpp / Minimap_Behavior.cpp) that keeps the
per-player storage sixteen wide and bounds the blip loop by the live player
count. This proof reproduces the crash on the unfixed base and shows it gone.

    python port/tools/vs16_minimap_proof.py [--frames N] [--map 0..3]
                                            [--players N] [--port P]

Every window is QUIET and MUTED through mp2_proof.env_base (CREATE_NO_WINDOW,
SW_SHOWMINNOACTIVE, SM64DS_NO_FOCUS, SM64DS_VOLUME=0). SM64DS_FAULTS_FATAL=1
turns the per-actor quarantine OFF so a real fault reaches the UEF and the
process dies non-zero -- the raw-crash mode the fault probe documents.
"""
import argparse
import os
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
MAP = os.path.join(ROOT, "build", "port", "walk_window.map")
FAULTMAP = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "faultmap.py")
OUT = os.path.join(ROOT, "build", "vs16_minimap")

# distinct injected keys so no two consoles push the identical pad
KEYS = (0x0040, 0x0080, 0x0020, 0x0010, 0x0001, 0x0002, 0x0004, 0x0008,
        0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x0040)

SEAT = re.compile(r"\[a2\] VS: (\d+) players, I am slot (\d+)")
FAULT = re.compile(r"FAULT code ([0-9a-fA-F]+) at \+0x([0-9a-fA-F]+)")
# [vs] f<frame> slot<i> actor=<hex> no=<n> char=<c> pos=(x,y,z) ...
VS = re.compile(r"^\[vs\] f(\d+) slot(\d+) actor=([0-9A-Fa-f]+) no=(\d+) "
                r"char=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)", re.M)


def port_base():
    return 45000 + (os.getpid() % 500) * 20


def resolve(offset_hex):
    """Name the symbol a fault image-offset lands in, via faultmap.py."""
    try:
        r = subprocess.run([sys.executable, FAULTMAP, "--map", MAP,
                            "+0x" + offset_hex],
                           capture_output=True, text=True, timeout=60)
        return (r.stdout or r.stderr).strip()
    except Exception as e:  # noqa: BLE001
        return "<faultmap failed: %s>" % e


def rows(t, slot):
    return [dict(f=int(m.group(1)), actor=m.group(3), no=int(m.group(4)),
                 char=int(m.group(5)), x=int(m.group(6)), y=int(m.group(7)),
                 z=int(m.group(8)))
            for m in VS.finditer(t) if int(m.group(2)) == slot]


def launch(out, frames, vsmap, players, base, stagger):
    procs, dirs, logs = [], [], []
    for k in range(players):
        d = os.path.join(out, "p%02d" % k)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        dirs.append(d)
        logs.append(os.path.join(d, "run.log"))
    for k in range(players):
        e = M.env_base(ROOT, dirs[k], "p%02d" % k)
        e.pop("SM64DS_LEVEL", None)          # a VS boot names its own map
        e["SM64DS_VS_MAP"] = str(vsmap)
        e["SM64DS_WINDOW_SELFTEST"] = str(frames)
        e["SM64DS_VS_PROBE"] = "1"
        e["SM64DS_VS_PLAYERS"] = str(players)
        e["SM64DS_FAULTS_FATAL"] = "1"       # raw crash, no quarantine
        e["SM64DS_COMMS_ROLE"] = "parent" if k == 0 else "child"
        e["SM64DS_COMMS_PORT"] = str(base)
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_COMMS_INJECT"] = "key=0x%04x" % KEYS[k % len(KEYS)]
        if k:
            e["SM64DS_COMMS_SLOT"] = str(k)   # pin the seat
        procs.append(M.spawn(EXE, dirs[k], e, logs[k]))
        if k < players - 1:
            time.sleep(stagger)
    res = []
    for k in range(players):
        rc = M.finish(procs[k], 1200)
        res.append((rc, M.text(logs[k]), logs[k]))
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", default="240")
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--players", type=int, default=16)
    ap.add_argument("--port", type=int, default=0)
    ap.add_argument("--stagger", type=float, default=0.25)
    args = ap.parse_args()

    base = args.port or port_base()
    n = args.players
    print("vs16_minimap_proof: %d windows, map %d, FAULTS_FATAL, port base %d"
          % (n, args.map, base), flush=True)

    res = launch(OUT, args.frames, args.map, n, base, args.stagger)
    ok = True
    faulted = []       # slots that faulted
    seats = []

    for k in range(n):
        rc, t, log = res[k]
        ms = SEAT.search(t)
        seat = int(ms.group(2)) if ms else -1
        seats.append(seat)
        mf = FAULT.search(t)
        if mf:
            code, off = mf.group(1), mf.group(2)
            sym = resolve(off)
            faulted.append((k, seat, code, off, sym, log))
            print("  window %02d seat %2d  rc=%d  FAULT %s at +0x%s\n"
                  "        %s" % (k, seat, rc, code, off, sym), flush=True)
        else:
            # liveness: the slot's own body exists and has a position
            r = rows(t, seat if seat >= 0 else k)
            live = "live pos=(%d,%d,%d)" % (r[-1]["x"], r[-1]["y"], r[-1]["z"]) \
                if r else "NO PROBE ROWS"
            print("  window %02d seat %2d  rc=%d  clean  %s"
                  % (k, seat, rc, live), flush=True)
        ok &= M.verdict(rc == 0 and not mf,
                        "window %02d (seat %d) clean, no fault | rc=%d %s"
                        % (k, seat, rc, log))

    ok &= M.verdict(sorted(seats) == list(range(n)),
                    "sixteen distinct seats 0..%d | seats=%s"
                    % (n - 1, sorted(seats)))

    if faulted:
        print("\nFAULTED SEATS:", sorted(s for _, s, *_ in faulted))
    print("\nVERDICT:", "ALL GREEN" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
