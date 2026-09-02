#!/usr/bin/env python3
"""Photograph a named VS arena object, and say what the camera was pointed at.

status/VSMAP.md settles by COUNTING that the CAPs and the VS `!` blocks are
alive on every arena. A count does not convince anyone who looked at the arena
and saw none, so this takes the picture -- and, because a picture of an empty
patch of grass proves nothing on its own, it also records the two numbers that
say where the lens was: how far the object was from the camera on that frame,
and how many degrees off the camera's own axis it sat.

The run is a solo VS boot through mp2_proof.env_base, so it is minimised,
unfocused and MUTED like every other proof in this tree.

HOW THE OBJECT GETS INTO SHOT. The DS camera looks at the player, so the player
is put next to the object with SM64DS_SPAWN (walk_window.cpp:6903, which writes
Player+0x5c after the entrance has already placed him) -- OFFSET rather than on
top of it, because a CAP is a pickup and standing in one is how you make it stop
existing. SM64DS_SELFTEST_ORBIT then holds the DS camera-rotate bit from frame
20 (walk_window.cpp:8762), which swings the camera all the way around him, so
some frame in the dump window has the object in front of the lens whatever
direction he happens to face. SM64DS_DUMP_FROM/TO writes one BMP a frame across
that window.

THE CAMERA IS THE GAME'S. cam_mode stays CAM_DS, so the Clipper cull deciding
what renders is the ROM's own, not the harness rig's -- walk_window.cpp:1985-89
is explicit that an actor the rig can see and the game camera cannot stays
dormant. A frame captured here is a frame the game would have drawn.

    python port/tools/vs_shots.py --map 1 --class CAP
    python port/tools/vs_shots.py --map 3 --class EXCLAMATION_BLOCK_VS
    python port/tools/vs_shots.py --all --out C:/tmp/vsmap/status_shots
"""
import argparse
import math
import os
import re
import shutil
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
WORK = os.path.join(ROOT, "build", "vs_shots")

MAP_LEVEL = {0: 51, 1: 43, 2: 29, 3: 42}

# The render probe in hal/actor_classes.cpp:902 names a class by the string its
# vtable veneer passes, and EXCLAMATION_BLOCK_VS shares _ZTV13QuestionBlock with
# the plain block, so its line reads QUESTION_BLOCK. On maps 2 and 4 that is not
# ambiguous: id 21 EXCLAMATION_BLOCK is star group 1 and never loads at filter
# 2, so the only question-block-family actor in a match is id 22.
PROBE_NAME = {"CAP": "CAP", "EXCLAMATION_BLOCK_VS": "QUESTION_BLOCK"}

# Candidate positions, verbatim from the level records
# `python port/tools/vs_objcensus.py --records` prints. EVERY placement of the
# class is a candidate and the runner boots once per candidate, because which
# one photographs well is a property of the arena's geometry rather than
# something to guess: a cap on a ledge over a pit drops the player 1300 units
# below it and no camera angle recovers that shot.
TARGETS = {
    (0, "CAP"): [(-1299, 729, 3236), (6048, 449, 414), (2292, 43, 4544),
                 (-4562, 500, 4628), (150, 964, 725), (804, 647, 2974)],
    (1, "CAP"): [(-100, 845, -1900), (-900, 582, -1300), (341, 356, -1106),
                 (-2614, 158, 2208), (297, 100, 3606), (3700, 94, 300)],
    (1, "EXCLAMATION_BLOCK_VS"): [(1295, 550, -3493)],
    (3, "CAP"): [(-300, 0, -300), (-1050, 400, 1000), (1000, 400, -1050),
                 (-400, 0, 1800), (1800, 0, -400)],
    (3, "EXCLAMATION_BLOCK_VS"): [(-2800, 1550, -2800), (2800, 650, 2800)],
}

DUMP_FROM, DUMP_TO = 150, 430   # after the 3-2-1 (it ends by frame 109);
                                # wide enough that the orbit completes a
                                # full turn for every candidate
# Where to put the player, per class. A CAP is a PICKUP, so he is stood off to
# one side of it; a block is scenery he can stand on, and dropping him straight
# down onto it is the one placement that cannot put him in the water beside it
# (level 43's block sits on a mound in the sea, and a horizontal offset landed
# him swimming with the block out of shot).
STANDOFF = {"CAP": (260, 60), "EXCLAMATION_BLOCK_VS": (0, 600)}
AIM_DEG = 8.0                   # wanted off-axis angle, see best_frame


def run(mapi, cls, pos, idx, frames):
    d = os.path.join(WORK, "map%d_%s_%d" % (mapi, cls, idx))
    if os.path.isdir(d):
        shutil.rmtree(d, ignore_errors=True)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, d, "shot")
    env.pop("SM64DS_LEVEL", None)
    env["SM64DS_VS_MAP"] = str(mapi)
    env["SM64DS_WINDOW_SELFTEST"] = str(frames)
    off, up = STANDOFF[cls]
    env["SM64DS_SPAWN"] = "%d,%d,%d" % (pos[0] + off, pos[1] + up,
                                        pos[2] + off)
    env["SM64DS_SELFTEST_ORBIT"] = "1"
    env["SM64DS_ACTOR_PROBE"] = "1"
    # Lane CAPSHOW: the cap render probe (hal/actor_classes_bob_world.cpp,
    # cap_render) prints one [cap] line per cap per verdict change, so a run
    # log says which caps the manager func_ov001_020aa420 turned on.
    env["SM64DS_CAP_PROBE"] = "1"
    env["SM64DS_DUMP_FROM"] = str(DUMP_FROM)
    env["SM64DS_DUMP_TO"] = str(DUMP_TO)
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, d, env, log)
    return rc, M.text(log), d


def best_frame(text, pos):
    """The dumped frame whose camera axis points closest at the object.

    Off-axis angle, in degrees, between the camera's own forward vector
    (at - eye) and the vector from the eye to the object. Both come off the
    [cam] line, which prints the GAME camera's +0x8c eye and +0x80 look-at
    (walk_window.cpp:10850), so this measures where the game was looking and
    not where the harness rig was."""
    best = None
    for m in re.finditer(
            r"\[cam\] f(\d+) eye\(([-\d.]+),([-\d.]+),([-\d.]+)\) "
            r"at\(([-\d.]+),([-\d.]+),([-\d.]+)\)", text):
        f = int(m.group(1))
        if not (DUMP_FROM <= f <= DUMP_TO):
            continue
        eye = [float(m.group(i)) for i in (2, 3, 4)]
        at = [float(m.group(i)) for i in (5, 6, 7)]
        fwd = [at[i] - eye[i] for i in range(3)]
        tov = [pos[i] - eye[i] for i in range(3)]
        nf = math.sqrt(sum(v * v for v in fwd))
        nt = math.sqrt(sum(v * v for v in tov))
        if nf < 1e-6 or nt < 1e-6:
            continue
        dot = sum(fwd[i] * tov[i] for i in range(3)) / (nf * nt)
        ang = math.degrees(math.acos(max(-1.0, min(1.0, dot))))
        # NOT the smallest angle. Zero degrees off-axis is the one place the
        # object is guaranteed to be behind something: the camera looks THROUGH
        # the player, so the screen centre is where the player and whatever
        # scenery he is standing against already are. AIM_DEG puts the object
        # beside him instead, still well inside a ~23 degree lens (the [cam]
        # line's own fov field, in DS angle units) but clear of the middle.
        score = abs(ang - AIM_DEG)
        if best is None or score < best[3]:
            best = (f, ang, nt, score)
    return best[:3] if best else None


def cap_verdicts(text):
    """{(x,y,z): 'draws' | 'RETURNS EARLY...'} -- the LAST [cap] verdict per
    cap, keyed on the position of its FIRST line. The probe's uid is the actor
    itself; a showing cap drops to the floor and its later lines carry the
    moved position, and two records can share a position (map 4 puts a 0x1004
    and a 0x0004 record both at (-300,0,-300)), so neither the current
    position nor the position alone is a key. The manager runs in
    Stage::Render and its flag is read by the next frame's Behavior, so every
    cap's first line is the early return; the last line is what the level
    settled on. A position with two records reports "draws" if either does."""
    first = {}
    last = {}
    for m in re.finditer(r"\[cap\] uid (\d+) pos\((-?\d+),(-?\d+),(-?\d+)\)"
                         r".*?-> Render (draws|RETURNS EARLY[^\n]*)", text):
        uid = m.group(1)
        if uid not in first:
            first[uid] = (int(m.group(2)), int(m.group(3)), int(m.group(4)))
        last[uid] = m.group(5)
    v = {}
    for uid, pos in first.items():
        if v.get(pos) == "draws":
            continue
        v[pos] = last[uid]
    return v


def one_class(mapi, cls, frames, out, cands=None):
    """Boot once per candidate placement and keep the closest-framed shot.

    A candidate whose own [cap] verdict is "draws" beats one whose cap the
    manager left hidden, whatever the angle: a well-framed picture of a hidden
    cap is the empty-grass picture this tool exists to avoid."""
    from PIL import Image
    probe = PROBE_NAME[cls]
    best = None
    for idx, pos in enumerate(TARGETS[(mapi, cls)]):
        if cands is not None and idx not in cands:
            continue
        rc, t, d = run(mapi, cls, pos, idx, frames)
        rendered = [ln for ln in t.splitlines()
                    if ln.startswith("[actor] " + probe)]
        alive = [ln for ln in t.splitlines() if ln.startswith("[pos] " + cls)]
        b = best_frame(t, pos)
        verdicts = cap_verdicts(t)
        mine = verdicts.get(tuple(pos), "no [cap] line")
        ndraw = sum(1 for x in verdicts.values() if x == "draws")
        print("    candidate %d at (%d,%d,%d): rc %d, %s, %s, cap %s "
              "(%d of %d caps draw)"
              % (idx, pos[0], pos[1], pos[2], rc,
                 "render path reached" if rendered else "NO RENDER LINE",
                 ("closest %.1f deg at f%03d" % (b[1], b[0])) if b
                  else "no [cam] frame",
                 mine, ndraw, len(verdicts)))
        if b is None:
            continue
        draws = 1 if mine == "draws" else 0
        key = (draws, -abs(b[1] - AIM_DEG))
        if best is None or key > best[6]:
            best = (pos, b, d, rendered, alive, rc, key, mine, ndraw,
                    len(verdicts))
    if best is None:
        return None
    pos, (f, ang, dist), d, rendered, alive, rc, _, mine, ndraw, ncap = best
    src = os.path.join(d, "walk_frame_%03d.bmp" % f)
    name = "map%d_level%d_%s_f%03d.png" % (mapi + 1, MAP_LEVEL[mapi],
                                           cls.lower(), f)
    dst = os.path.join(out, name)
    if not os.path.exists(src):
        return None
    Image.open(src).save(dst)
    return dict(pos=pos, frame=f, angle=ang, dist=dist, file=dst,
                rendered=rendered, alive=alive, rc=rc, verdict=mine,
                ndraw=ndraw, ncap=ncap)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=os.path.join(ROOT, "status_shots"))
    ap.add_argument("--frames", type=int, default=470)
    ap.add_argument("--map", type=int)
    ap.add_argument("--class", dest="cls")
    ap.add_argument("--cand", default=None,
                    help="comma list of candidate indices to boot (default all)")
    args = ap.parse_args()
    cands = None
    if args.cand:
        cands = set(int(x) for x in args.cand.split(","))

    keys = sorted(TARGETS)
    if args.map is not None:
        keys = [k for k in keys if k[0] == args.map]
    if args.cls:
        keys = [k for k in keys if k[1] == args.cls]
    os.makedirs(args.out, exist_ok=True)

    results = []
    for mapi, cls in keys:
        print("VS map %d (level %d), %s: %d candidate placements"
              % (mapi + 1, MAP_LEVEL[mapi], cls, len(TARGETS[(mapi, cls)])))
        r = one_class(mapi, cls, args.frames, args.out, cands)
        results.append((mapi, cls, r))

    print("")
    for mapi, cls, r in results:
        print("=" * 74)
        print("VS map %d (level %d)  %s" % (mapi + 1, MAP_LEVEL[mapi], cls))
        if r is None:
            print("  NO SHOT: no candidate produced a dumped frame")
            continue
        print("  object photographed         : (%d,%d,%d)" % r["pos"])
        print("  process exit                : rc %d" % r["rc"])
        print("  alive on the behaviour list : %s"
              % (r["alive"][0] if r["alive"] else "NO [pos] ROW"))
        print("  render path reached         : %s"
              % (r["rendered"][0] if r["rendered"] else
                 "NO [actor] LINE -- Render never ran for this class"))
        if cls == "CAP":
            print("  cap probe verdict           : this cap %s; %d of %d caps "
                  "on the arena draw" % (r["verdict"], r["ndraw"], r["ncap"]))
        print("  frame chosen                : f%03d, %.1f degrees off the "
              "camera axis, %.0f units from the eye"
              % (r["frame"], r["angle"], r["dist"]))
        print("  file                        : %s" % r["file"])


main()
