#!/usr/bin/env python3
"""The two ARM9-RESIDENT SCENE CLASSES run, and the proof that says so.

Run link100, lane SCENE. hal/scene_link100_boot.cpp seats dScBoot_c (actor id
0) and hal/scene_link100_mb.cpp seats dScMB_c (id 360) -- the ROM's two boot
scenes, the fork Scene::PrepareToSpawnBoot picks between.

NOT NAMED scene_menu_proof.py, and the difference is the finding rather than a
preference: the lane brief called id 360 "the main-menu scene, on the default
route". It is not. src/GetSceneOverlayID.c gives it no overlay, its own D2 is
named MultiBootScene::~MultiBootScene in src/func_02034a78.c, and
src/_ZN5Scene18PrepareToSpawnBootEv.c parks either 0 or 0x168 as the FIRST
scene of the process. The pair is a boot fork; the file is named for what it
proves.

WHAT A GREEN RUN CLAIMS, and it is deliberately narrower than "the scene
works": the ROM's own spawn spine builds the object, the port's fill put host
faces in all eighteen slots of a table the ROM's own factory writes, and the
class's OWN InitResources / Behavior / Render bodies were ENTERED, N times for
N frames, with no fault, no unhosted slot and no refused-subtree entry. A
scene that boots and a scene that RUNS are two different claims and only the
per-slot counters separate them.

    python port/tools/scene_bootpair_proof.py [--frames 300] [--keep]

FIVE RUNGS, and two of them are negative controls -- rungs that must FAIL to
produce their evidence if the seat were fake:

  1  scene 0   spawns, ticks, draws, tears down clean
  2  scene 360 spawns, ticks, draws, tears down clean
  3  NEGATIVE: an id the registry does not carry is REFUSED. If the harness
     ran any id handed to it, rungs 1 and 2 would prove nothing about the
     registry.
  4  NEGATIVE: a run of some OTHER scene prints NEITHER counter line. The
     counter reports are armed inside the fills on port_scene_env_want(), and
     the fills run on every boot -- so if the reports were unconditional,
     rungs 1 and 2 would be reading a line every process prints.
  5  NEGATIVE: the default boot route is UNCHANGED. A bare launch must still
     reach the title (scene 1), not either of the two scenes this lane seated.
     This lane deliberately did not move the ROM's boot order into the port,
     and this rung is what holds that promise.

Quiet spawner, the tree's rule: mp2_proof.env_base gives NO_FOCUS + VOLUME=0 +
MINIMIZED + SW_SHOWMINNOACTIVE and drops every inherited SM64DS_ knob, so no
lane's environment can decide what these runs do.
"""
import argparse
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "scene_bootpair_proof")

# The two rows this lane added to port_scene_classes[] in hal/scene_boot.cpp.
SCENES = {0: "SCENE_BOOT", 360: "SCENE_MULTIBOOT"}

# The per-class counter line each fill's atexit report prints.
REPORT = {0: "[boot] slots entered:", 360: "[mb] slots entered:"}

# An id that is genuinely unhosted at RUNTIME: no row of port_scene_classes[]
# AND no row of hal/actor_classes.inc, because data_020a4bb8 is ONE shared
# array (actorID/sceneID is one namespace) that hal/actor_registry.cpp's
# install and port_scene_registry_install both write into. 9 was tried first
# and is WRONG in exactly that way: it is not merely a row the ROM's own spawn
# table carries (config/arm9/relocs.txt from:0x02090888 to:0x0210ad90
# module:overlay(2)) -- it is YOSHI_EGG, hal/actor_classes.inc:3093, a regular
# actor the port ALREADY hosts. hal/actor_registry.cpp's install runs before
# any scene boot and writes data_020a4bb8[9] to YoshiEgg_SpawnInfo, so
# port_scene_boot(9) does not refuse -- it calls YoshiEgg_Spawn() as if it
# were a Scene factory and walks the returned object's ActorBase vtable as an
# 18-slot Scene table, which is exactly the kind of type confusion this rung
# exists to rule out, not exercise. Measured: the run printed "[scene] 9 = ?"
# (a name lookup miss, not a refusal) and "spawned ..., vptr ...", then the
# process exited 0xC0000005 (access violation).
#
# 3 is the port's own long-documented negative case instead: hal/
# title_entry.cpp's header states outright that "data_020a4bb8[3] is null" --
# id 3 is the Stage/level scene (relocs.txt from:0x02090870 to:0x0209213c
# module:main), which the port deliberately does not seat as a
# port_scene_classes[] row (the level boot takes it through
# port_stage_a_boot instead) and which has no hal/actor_classes.inc row
# either, so nothing ever writes data_020a4bb8[3]. The registry must refuse
# it.
UNHOSTED_ID = 3


def run_scene(tag, scene, frames, extra=None, bmp=False):
    d = os.path.join(OUT, tag)
    run_dir = os.path.join(d, "run")
    shutil.rmtree(d, ignore_errors=True)
    os.makedirs(os.path.join(run_dir, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, run_dir, "scnproof")
    # mp2_proof.env_base FORCES SM64DS_LEVEL=1 -- read its own header before
    # this line: that module's rungs measure the level-1 boot and always have,
    # so it treats a bare launch as a request for that level. This proof's
    # rung 5 wants the OPPOSITE bare launch, the one hal/title_entry.cpp's
    # port_boot_default_scene() resolves to the title -- and that function
    # checks PRESENCE, not value: `!bf_present("SM64DS_LEVEL")`. Left in place,
    # every "default" run here would silently boot castle grounds instead of
    # the title, which is exactly what happened before this fix (rc=-9, no
    # "[scene] 1 = SCENE_TITLE" line, a level-boot log instead). Only the
    # scene is None call (rung_default_route) takes this path; the id-bearing
    # runs already win the race by setting SM64DS_SCENE below, which
    # port_scene_env_want reads before ever consulting the default.
    if scene is None:
        env.pop("SM64DS_LEVEL", None)
    if scene is not None:
        env["SM64DS_SCENE"] = str(scene)
    env["SM64DS_SCENE_FRAMES"] = str(frames)
    # port/tests/walk_window.cpp freopen()s stderr onto its own timestamped
    # playlog/play_*.log a few statements into main UNLESS SM64DS_NO_PLAYLOG or
    # SM64DS_WINDOW_SELFTEST is set (the exact condition at that call site:
    # `if (!getenv("SM64DS_NO_PLAYLOG") && !getenv("SM64DS_WINDOW_SELFTEST"))`).
    # port_scene_boot's "FATAL: scene %d is not a hosted scene" and its
    # caller's "scene %d did not spawn" are BOTH fprintf(stderr, ...), so on an
    # unhosted-id run they were landing in the playlog and never reaching this
    # file's run.log -- rung_unhosted's own text checks were looking in the one
    # place the message never went. Confirmed the hard way: the refusal itself
    # was always correct (rc 3, the exact "did not spawn" branch, and both new
    # rows present in the FATAL line's hosted list) -- only the CAPTURE was
    # blind, the identical shape port/tools/mp2_proof.py's rung_playmode header
    # documents for the two-window script. SM64DS_NO_PLAYLOG=1 is the
    # established fix for it elsewhere in this tree (tools/gapproof.py,
    # tools/opening_audio_measure.py, tools/wide_sweep.py all set it for the
    # same reason) and is the narrower of the two knobs that clear the
    # condition: SM64DS_WINDOW_SELFTEST also gates keyboard reads and the
    # missing-ROM dialog, which this proof has no reason to touch.
    env["SM64DS_NO_PLAYLOG"] = "1"
    if bmp:
        env["SM64DS_SCENE_BMP"] = os.path.join(d, "last.bmp")
    if extra:
        env.update(extra)
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, run_dir, env, log, timeout=900)
    return rc, M.text(log), log


def bmp_spread(path):
    """(distinct 24-bit pixel values, total pixels) in a BMP, or (0, 0).

    A BLANK FRAME IS THE FAILURE THIS MEASURES. The counters below say the
    scene's slots were entered; they say nothing about anything reaching the
    framebuffer. A scene whose InitResources decompressed its artwork into BG
    VRAM and turned the display on presents MANY distinct values; one that
    faulted before the display-on, or that the port never presented, comes
    back as a single flat colour. Distinctness, not a reference image: this
    file must not carry Nintendo's pixels and must not need a golden capture
    to say the frame is not blank.
    """
    if not os.path.isfile(path):
        return 0, 0
    with open(path, "rb") as f:
        raw = f.read()
    if len(raw) < 54 or raw[:2] != b"BM":
        return 0, 0
    off = int.from_bytes(raw[10:14], "little")
    bpp = int.from_bytes(raw[28:30], "little")
    if bpp not in (24, 32):
        return 0, 0
    step = bpp // 8
    body = raw[off:]
    vals = set()
    n = 0
    for i in range(0, len(body) - step + 1, step):
        vals.add(body[i:i + 3])
        n += 1
    return len(vals), n


def counters(txt, scene):
    """{name: count} out of the class's own report line, or None."""
    m = re.search(re.escape(REPORT[scene]) + r"([^\n]*)", txt)
    if not m:
        return None
    return {k: int(v) for k, v in re.findall(r"([a-z0-9]+)\s+(\d+)",
                                             m.group(1))}


def rung_scene(scene, frames):
    tag = "scene%d" % scene
    rc, txt, log = run_scene(tag, scene, frames, bmp=True)
    name = SCENES[scene]
    ok = M.verdict(rc == 0, "rung: scene %d (%s) exited 0 (rc=%s, %s)"
                   % (scene, name, rc, log))
    ok &= M.verdict("[scene] %d = %s" % (scene, name) in txt,
                    "rung: the registry named it -- '[scene] %d = %s'"
                    % (scene, name))
    clean = "[scene] %d frames of scene %d (%s), clean" % (frames, scene, name)
    ok &= M.verdict(clean in txt, "rung: '%s'" % clean)
    ok &= M.verdict("UNHOSTED" not in txt,
                    "rung: no UNHOSTED slot report")
    ok &= M.verdict("VTABLE SHAPE" not in txt,
                    "rung: the fill's own count check passed (no VTABLE SHAPE "
                    "line -- every raw DS word in the 18-slot table was "
                    "accounted for)")
    ok &= M.verdict("refused subtree entered" not in txt,
                    "rung: the refused boot-worker face was never entered")
    c = counters(txt, scene)
    ok &= M.verdict(c is not None, "rung: the class's own report printed")
    if c:
        # THE CLAIM THAT SEPARATES 'SPAWNED' FROM 'RAN'. init once, and the
        # per-frame slots entered on the order of once per frame. A scene that
        # spawned and never ticked shows init 1 and beh 0, which is a green
        # rc and a dead object.
        ok &= M.verdict(c.get("init", 0) == 1,
                        "rung: InitResources entered exactly once (%s)"
                        % c.get("init"))
        ok &= M.verdict(c.get("beh", 0) >= frames // 2,
                        "rung: Behavior entered %s times over %d frames"
                        % (c.get("beh"), frames))
        ok &= M.verdict(c.get("render", 0) >= frames // 2,
                        "rung: Render entered %s times over %d frames"
                        % (c.get("render"), frames))
        print("      counters: %s" % c, flush=True)
    # AND SOMETHING REACHED THE FRAMEBUFFER.
    #
    # THE TWO SCENES ARE HELD TO DIFFERENT BARS AND THE REASON IS THE ROM'S,
    # read out of their own InitResources bodies before either was ever run:
    #
    #   scene 0    src/func_02005a58.c ENABLES LAYERS and fills them --
    #              `*(u32*)0x4000000 = (... & ~0x1f00) | 0x100` turns main BG0
    #              on, `0x4001000 ... | 0x400` turns sub BG2 on, and it
    #              decompresses data_020918c4 / data_020916d8 / data_02091570
    #              into their character and screen memory. A presented frame
    #              must therefore be a PICTURE.
    #
    #              THE BAR WAS 64 AND IT WAS A GUESS, made before this file
    #              ever ran. The finisher pass that first ran it captured the
    #              real frame: the ROM's own Nintendo logo (an orange/red mark
    #              on white, so a SOLID-COLOUR logo, not a photographic
    #              texture), 23 distinct 24-bit values -- almost all of them
    #              the anti-aliased blend between the mark's red and the white
    #              backdrop. 64 was never reachable by a correct render of
    #              THIS artwork and would have red-lit every future green run.
    #              8 is the recalibrated bar: comfortably under the 23 a
    #              correct render measures, comfortably over the 1-3 a flat
    #              field's own dithering could produce.
    #
    #   scene 360  src/func_0203506c.c does the opposite: it CLEARS every layer
    #              enable on both engines (`&= ~0x1f00` twice) and then calls
    #              GX::DispOn, leaving the backdrop -- which the same body has
    #              just written white, MultiStore16(0xffff, 0x5000000, 2). The
    #              layers come back on in Behavior state 0, and only when
    #              func_0203d8fc() is true; that reads bit 0x2000 of
    #              data_020a1154[6], the wireless status word, which is zero on
    #              a port with no multiboot session. So a CORRECT run of this
    #              scene presents a flat white field, and asserting a picture
    #              here would be asserting a bug.
    #
    # Both captures are kept under --keep and BOTH WANT A HUMAN EYE. Nothing in
    # this file can say the frame is RIGHT, only that it is what the ROM's own
    # code path produces.
    bmp = os.path.join(OUT, tag, "last.bmp")
    distinct, total = bmp_spread(bmp)
    ok &= M.verdict(total > 0, "rung: a frame was presented and captured "
                               "(%d pixels, %s)" % (total, bmp))
    if scene == 0:
        ok &= M.verdict(distinct >= 8,
                        "rung: the presented frame is a picture -- %d distinct "
                        "pixel values over %d pixels (the scene's own "
                        "InitResources enables BG0 and sub BG2 and fills them)"
                        % (distinct, total))
    else:
        print("      presented frame: %d distinct pixel values over %d pixels "
              "(a flat field is the ROM's own answer here -- see the block "
              "above)" % (distinct, total), flush=True)
    return ok


def rung_unhosted(frames):
    """NEGATIVE CONTROL. The registry must refuse an id it does not carry."""
    rc, txt, log = run_scene("unhosted", UNHOSTED_ID, frames)
    ok = M.verdict("is not a hosted scene" in txt,
                   "negative: scene %d was REFUSED by the registry (%s)"
                   % (UNHOSTED_ID, log))
    ok &= M.verdict(rc != 0,
                    "negative: the refused run did not exit 0 (rc=%s)" % rc)
    # and the refusal must LIST the two this lane seated, which is the same
    # table rungs 1 and 2 read.
    for s, n in SCENES.items():
        ok &= M.verdict("%d (%s)" % (s, n) in txt,
                        "negative: the refusal listed %d (%s) as hosted"
                        % (s, n))
    return ok


def rung_reports_are_armed(frames):
    """NEGATIVE CONTROL. Another scene's run prints NEITHER counter line."""
    rc, txt, log = run_scene("scene1", 1, frames)
    ok = M.verdict(rc == 0, "negative: the title (scene 1) still runs "
                            "(rc=%s, %s)" % (rc, log))
    for s in SCENES:
        ok &= M.verdict(REPORT[s] not in txt,
                        "negative: '%s' did NOT print on a scene-1 run"
                        % REPORT[s].strip())
    return ok


def rung_default_route(frames):
    """NEGATIVE CONTROL. A bare launch still reaches the TITLE."""
    rc, txt, log = run_scene("default", None, frames)
    ok = M.verdict(rc == 0, "negative: the default boot still exits 0 "
                            "(rc=%s, %s)" % (rc, log))
    ok &= M.verdict("[scene] 1 = SCENE_TITLE" in txt,
                    "negative: a bare launch still resolves to the TITLE, not "
                    "to either scene this lane seated")
    for s, n in SCENES.items():
        ok &= M.verdict("[scene] %d = %s" % (s, n) not in txt,
                        "negative: the default route did not become %s" % n)
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", type=int, default=300)
    ap.add_argument("--keep", action="store_true",
                    help="leave build/scene_bootpair_proof/ in place")
    a = ap.parse_args()
    if not os.path.isfile(EXE):
        sys.exit("scene_bootpair_proof: no %s -- build walk_window first" % EXE)

    ok = True
    for scene in (0, 360):
        ok &= rung_scene(scene, a.frames)
    ok &= rung_unhosted(a.frames)
    ok &= rung_reports_are_armed(a.frames)
    ok &= rung_default_route(a.frames)

    print("")
    for v in M.VERDICTS:
        print(v)
    print("\n%s  (%d rungs)" % ("ALL GREEN" if ok else "RED", len(M.VERDICTS)))
    if not a.keep:
        shutil.rmtree(OUT, ignore_errors=True)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
