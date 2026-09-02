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

SDAT = os.path.join(ROOT, "extracted", "dsd", "files", "data", "sound_data.sdat")


def sdat_sequences():
    """[(start, end, id, name, bank)] for every SSEQ, out of the ROM's SDAT.

    PORT_SSEQ_TRACE names a started stream by its offset into the SDAT and by
    nothing else, so this is what turns '[sseq] start player 0 ... (sdat+0x...)'
    into 'NCS_BGM_VSATHRETIC'. Worth having rather than trusting the id the
    game ASKED for: 0x4d and 0x41 (NCS_BGM_ATHRETIC) are the same SSEQ file
    byte for byte and differ only in the bank their INFO record names, so
    'the right song is playing' and 'the right entry was started' are two
    different questions."""
    import struct
    d = open(SDAT, "rb").read()
    nblk = struct.unpack("<H", d[0xE:0x10])[0]
    blocks = [struct.unpack("<II", d[0x10 + i * 8:0x18 + i * 8])[0]
              for i in range(nblk)]
    symb, info, fat = blocks[0], blocks[1], blocks[2]

    def recs(base):
        return [struct.unpack("<I", d[base + 8 + i * 4:base + 12 + i * 4])[0]
                for i in range(8)]

    seq_info = info + recs(info)[0]
    seq_sym = symb + recs(symb)[0]
    count = struct.unpack("<I", d[seq_info:seq_info + 4])[0]
    fat_n = struct.unpack("<I", d[fat + 8:fat + 12])[0]
    out = []
    for i in range(count):
        p = struct.unpack("<I", d[seq_info + 4 + i * 4:seq_info + 8 + i * 4])[0]
        if not p:
            continue
        fid, _unk, bank = struct.unpack("<HHH", d[info + p:info + p + 6])
        if fid >= fat_n:
            continue
        off, size = struct.unpack("<II", d[fat + 12 + fid * 16:fat + 20 + fid * 16])
        sp = struct.unpack("<I", d[seq_sym + 4 + i * 4:seq_sym + 8 + i * 4])[0]
        name = "?"
        if sp:
            end = d.index(b"\0", symb + sp)
            name = d[symb + sp:end].decode()
        out.append((off, off + size, i, name, bank))
    return sorted(out)


def which_sequence(seqs, rel):
    """The SSEQ whose FILE the trace's offset lands in.

    A range lookup and not an equality one: PORT_SSEQ_TRACE prints
    seqBase + startOff, which is inside the SSEQ file past its own header,
    never the FAT offset itself."""
    for start, end, i, name, bank in seqs:
        if start <= rel < end:
            return "0x%02x %s bank %d (sdat+0x%x, +0x%x into the file)" % (
                i, name, bank, rel, rel - start)
    return "sdat+0x%x (inside no SSEQ file)" % rel


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
    # names every sequence the sequencer actually STARTS, by its SDAT offset
    env["PORT_SSEQ_TRACE"] = "1"
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
    seqs = sdat_sequences()
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
        started = [which_sequence(seqs, int(o, 16)) for o in
                   re.findall(r"\[sseq\] start player \d+ .*\(sdat\+0x([0-9a-f]+)\)", t)]
        print("  sequences actually started, in order:")
        for s in started or ["      none"]:
            print("      %s" % s)
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
