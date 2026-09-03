#!/usr/bin/env python3
"""Photograph the adventure HUD (top screen) and the minimap (bottom), and a VS
frame for the placement-unchanged control. Quiet spawner throughout: env_base
gives NO_FOCUS + VOLUME=0 + MINIMIZED + SW_SHOWMINNOACTIVE, the same rule every
proof in this tree launches under.

The presented image is captured with SM64DS_STACK_BMP (both engines, host
overlays included) and, belt-and-braces, the framebuffer with SM64DS_DUMP_FROM/
TO (walk_frame_%03d.bmp). Level 6 is Bob-omb Battlefield -- a real course, not
the castle-family hub, so RenderCoinCount does not take its 0x1d early-out and
the coin counter is on screen next to the stars and lives.
"""
import os
import sys
import shutil
import subprocess

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "status_shots", "advhud")
FRAMES = 260
CAP = 220           # capture frame: HUD is up and settled by here


def run(name, level, vsmap, adv_top, extra=None):
    d = os.path.join(ROOT, "build", "advhud_shots", name)
    if os.path.isdir(d):
        shutil.rmtree(d, ignore_errors=True)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, d, "advhud")
    env.pop("SM64DS_LEVEL", None)
    if level is not None:
        env["SM64DS_LEVEL"] = str(level)
    if vsmap is not None:
        env["SM64DS_VS_MAP"] = str(vsmap)
    env["SM64DS_WINDOW_SELFTEST"] = str(FRAMES)
    env["SM64DS_ADV_HUD_TOP"] = "1" if adv_top else "0"
    stack = os.path.join(d, "stack.bmp")
    env["SM64DS_STACK_BMP"] = "%s,%d" % (stack, CAP)
    env["SM64DS_DUMP_FROM"] = str(CAP)
    env["SM64DS_DUMP_TO"] = str(CAP + 2)
    if extra:
        env.update(extra)
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, d, env, log, timeout=600)
    return rc, d, stack, log


def save_png(bmp, png):
    from PIL import Image
    if not os.path.exists(bmp):
        return False
    os.makedirs(os.path.dirname(png), exist_ok=True)
    Image.open(bmp).save(png)
    return True


def main():
    os.makedirs(OUT, exist_ok=True)
    jobs = [
        # name             level vsmap adv_top
        ("adv_top_on",     6,    None, True),
        ("adv_top_off",    6,    None, False),
        ("vs_unchanged",   None, 0,    True),
    ]
    for name, level, vsmap, adv_top in jobs:
        rc, d, stack, log = run(name, level, vsmap, adv_top)
        # prefer the presented stacked image; fall back to the dumped fb frame
        srcs = [stack] + [os.path.join(d, "walk_frame_%03d.bmp" % f)
                          for f in (CAP, CAP + 1, CAP + 2)]
        png = os.path.join(OUT, name + ".png")
        saved = None
        for s in srcs:
            if save_png(s, png):
                saved = s
                break
        print("%-16s rc=%d  ->  %s  (from %s)"
              % (name, rc, png if saved else "NO IMAGE",
                 os.path.basename(saved) if saved else "-"))
        print("                 log: %s" % log)


main()
