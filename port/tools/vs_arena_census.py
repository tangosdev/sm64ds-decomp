#!/usr/bin/env python3
"""What a VS arena SPAWNS, one quiet boot per map, against what the ROM places.

port/tools/vs_objcensus.py reads the cartridge's own object tables and says what
each arena asks for. This boots the port on the same four maps and says what it
got, then differences the two, so "the arena has no enemies" is settled by two
counts rather than by a memory of a match.

One process per map, through mp2_proof.env_base -- so the run is minimised,
unfocused and MUTED like every other proof in this tree -- with the ROM's own VS
start (SM64DS_VS_MAP), the actor census on, and both streams into one log
because the census goes to stdout while every [vs]/[vsgo]/[course] line goes to
stderr.

    python port/tools/vs_arena_census.py [--frames N] [--maps 0,1,2,3]
"""
import argparse
import collections
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vs_arena_census")

# map index -> level id, the ROM's data_ov075_0211c6ec
MAP_LEVEL = {0: 51, 1: 43, 2: 29, 3: 42}


def rom_placement(star=2):
    """{level: Counter(actor id)} from vs_objcensus's own roll-up.

    Shelled out rather than imported: vs_objcensus.py runs main() at import, and
    a second copy of its reader here is a second thing to keep true."""
    out = subprocess.run([sys.executable,
                          os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                       "vs_objcensus.py"),
                          "--star", str(star)],
                         capture_output=True, text=True, check=True).stdout
    want = {}
    lvl = None
    mode = None
    for ln in out.splitlines():
        m = re.match(r"VS map \d+\s+level (\d+)", ln)
        if m:
            lvl = int(m.group(1))
            want[lvl] = collections.Counter()
            mode = None
            continue
        if ln.startswith("  PLACED under"):
            mode = "placed"
            continue
        if ln.startswith("  FILTERED OUT"):
            mode = None
            continue
        m = re.match(r"\s+(\d+) 0x[0-9a-f]+\s+x(\d+)", ln)
        if m and mode == "placed" and lvl is not None:
            want[lvl][int(m.group(1))] += int(m.group(2))
    return want


def run_map(mi, frames):
    d = os.path.join(OUT, "map%d" % mi)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, d, "census%d" % mi)
    env.pop("SM64DS_LEVEL", None)          # the VS start stages the level
    env["SM64DS_VS_MAP"] = str(mi)
    env["SM64DS_WINDOW_SELFTEST"] = str(frames)
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, d, env, log)
    return rc, M.text(log)


def spawned(text):
    """The registry census: '+ 269 x6  CAP' rows, and the declined rows."""
    got = collections.Counter()
    declined = collections.Counter()
    for ln in text.splitlines():
        m = re.match(r"\s+\+\s+(\d+) x(\d+)\s", ln)
        if m:
            got[int(m.group(1))] += int(m.group(2))
        m = re.match(r"\s+-\s+(\d+) x(\d+)", ln)
        if m:
            declined[int(m.group(1))] += int(m.group(2))
    return got, declined


def alive(text, names):
    """Live actors on the BEHAVIOUR list, from port_actor_positions' [pos] rows.

    The census counts spawn ATTEMPTS -- it is the pre-spawn gate -- so a class
    whose InitResources returns 0 is counted there and is not in the world.
    This is the other number, and the pair is what tells "declined" apart from
    "spawned and immediately gone"."""
    live = collections.Counter()
    for ln in text.splitlines():
        m = re.match(r"\[pos\] (\S+).*\[(\d+)\]\s*$", ln)
        if m and m.group(1) in names:
            live[names[m.group(1)]] += int(m.group(2))
    return live


def class_names():
    """NAME -> id, from the same hal/actor_classes.inc the census names come
    from, so the [pos] rows can be counted per actor id."""
    out = {}
    p = os.path.join(ROOT, "port", "hal", "actor_classes.inc")
    for ln in open(p, errors="ignore"):
        m = re.match(r'\s*\{\s*(\d+)\s*,\s*"([A-Z0-9_]+)"', ln)
        if m:
            out[m.group(2)] = int(m.group(1))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", type=int, default=420)
    ap.add_argument("--maps", default="0,1,2,3")
    args = ap.parse_args()

    want = rom_placement(2)
    names = class_names()
    fails = 0
    for mi in [int(x) for x in args.maps.split(",")]:
        lvl = MAP_LEVEL[mi]
        rc, t = run_map(mi, args.frames)
        got, declined = spawned(t)
        live = alive(t, names)
        seq = re.findall(r"LoadAndSetMusic_Layer1\((0x[0-9a-f]+)\)", t)
        grp = re.search(r"sound row: group=(\d+) bank=(0x[0-9a-f]+) bgm=(-?\d+)", t)
        star = re.search(r"star filter (\d+)", t)
        print("")
        print("=" * 72)
        print("VS map %d  level %d   rc %d" % (mi + 1, lvl, rc))
        print("  star filter : %s" % (star.group(1) if star else "MISSING"))
        print("  sound group : %s bank %s (table bgm column %s, unread in VS)"
              % (grp.group(1), grp.group(2), grp.group(3)) if grp
              else "  sound group : MISSING")
        print("  layer-1 seq : %s" % (", ".join(seq) if seq else "none issued"))
        print("  ROM places / port spawns / port keeps alive, by class:")
        for aid in sorted(set(want.get(lvl, {})) | set(got)):
            w = want.get(lvl, {}).get(aid, 0)
            g = got.get(aid, 0)
            a = live.get(aid, 0)
            if w == 0:
                mark = "   (not a placed class: HUD, player, minimap...)"
            elif g < w:
                mark = "   <-- NEVER SPAWNED, %d short" % (w - g)
                fails += 1
            elif a < w:
                mark = "   <-- SPAWNED THEN GONE, %d short alive" % (w - a)
                fails += 1
            else:
                mark = ""
            print("      %3d  rom x%-3d  spawn x%-3d  alive x%-3d%s"
                  % (aid, w, g, a, mark))
        if declined:
            print("  DECLINED by the registry gate:")
            for aid, c in sorted(declined.items()):
                print("      %3d x%d" % (aid, c))
        if rc != 0:
            fails += 1
            print("  RC IS NOT 0")
    print("")
    print("VERDICT: %s" % ("PASS" if fails == 0 else "FAIL (%d)" % fails))
    return 0 if fails == 0 else 1


sys.exit(main())
