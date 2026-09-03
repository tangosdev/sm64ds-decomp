#!/usr/bin/env python3
"""HUD::InitResources retirement proof (lane HUD-INITRES).

The matched TU src/_ZN3HUD13InitResourcesEv.cpp replaced the host copy
port/unmatched/HUD_InitResources.cpp. The claim is that the HUD draws
IDENTICALLY, so the proof is a level selftest under SM64DS_FAULTS_FATAL=1 and
a pixel compare of the presented image (SM64DS_STACK_BMP, both engines, host
overlays included) plus the 512x384 selftest framebuffer, before the change
against after it, in the same build directory.

    python port/tools/hud_initres_proof.py capture <tag> [--level N] [--frames N] [--cap N]
    python port/tools/hud_initres_proof.py compare <tag_a> <tag_b>

`capture` runs walk_window.exe once on the level (default 6, Bob-omb
Battlefield: a real course, so the coin counter is on screen beside the stars
and lives), asserts rc 0, copies stack.bmp + walk_window_selftest.bmp + the run
log into build/hud_initres_proof/<tag>/ and prints the [hud] lines and the
layout line (the .dsstate base, which notes/port-selftest-bmp-gate.md requires
to agree before two selftest BMPs may be compared byte for byte).

`compare` diffs the two captures pixel for pixel, whole image and per screen
(the stacked image is two 256x192 screens at 2x, upper at the row the run
reports), and prints md5s and differing-pixel counts. Exit 0 when the presented
images agree, 1 otherwise.

Quiet spawner, the tree's rule: mp2_proof.env_base gives NO_FOCUS + VOLUME=0 +
MINIMIZED + SW_SHOWMINNOACTIVE and drops every inherited SM64DS_ knob.
"""
import argparse
import hashlib
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "hud_initres_proof")


def md5(path):
    with open(path, "rb") as f:
        return hashlib.md5(f.read()).hexdigest()


def capture(tag, level, frames, cap):
    d = os.path.join(OUT, tag)
    run_dir = os.path.join(d, "run")
    if os.path.isdir(d):
        shutil.rmtree(d, ignore_errors=True)
    os.makedirs(os.path.join(run_dir, "tmp"), exist_ok=True)
    env = M.env_base(ROOT, run_dir, "hudproof")
    env["SM64DS_LEVEL"] = str(level)
    env["SM64DS_WINDOW_SELFTEST"] = str(frames)
    # A level presents INSET by default and the inset layout has no stacked
    # image (tests/walk_window.cpp, stack_capture). Force the stacked layout
    # so SM64DS_STACK_BMP has a presented image to write; the selftest BMP is
    # the 512x384 framebuffer in either layout.
    env["SM64DS_DUAL_SCREEN"] = "1"
    stack = os.path.join(d, "stack.bmp")
    env["SM64DS_STACK_BMP"] = "%s,%d" % (stack, cap)
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, run_dir, env, log, timeout=600)
    txt = M.text(log)
    # the selftest BMP lands in the run's cwd
    src_bmp = os.path.join(run_dir, "walk_window_selftest.bmp")
    if os.path.exists(src_bmp):
        shutil.copy2(src_bmp, os.path.join(d, "walk_window_selftest.bmp"))
    print("capture %s: level %d, %d frames, rc=%d" % (tag, level, frames, rc))
    for line in txt.splitlines():
        if ("[hud]" in line or "[layout]" in line or "[stack]" in line
                or line.startswith("selftest:") or "FAULT" in line
                or "fs: load-at" in line and "not found" in line):
            print("  " + line.rstrip())
    for name in ("stack.bmp", "walk_window_selftest.bmp"):
        p = os.path.join(d, name)
        print("  %-26s %s" % (name, md5(p) if os.path.exists(p) else "MISSING"))
    return 0 if rc == 0 and os.path.exists(stack) else 1


def load_bmp(path):
    from PIL import Image
    return Image.open(path).convert("RGB")


def diff_region(a, b, box, label):
    from PIL import ImageChops
    ra, rb = a.crop(box), b.crop(box)
    if ra.size != rb.size:
        print("  %-22s SIZE DIFFERS %s vs %s" % (label, ra.size, rb.size))
        return 1
    d = ImageChops.difference(ra, rb)
    bbox = d.getbbox()
    n = 0
    if bbox:
        px = d.load()
        for y in range(d.size[1]):
            for x in range(d.size[0]):
                if px[x, y] != (0, 0, 0):
                    n += 1
    print("  %-22s %dx%d  differing pixels: %d%s"
          % (label, ra.size[0], ra.size[1], n,
             ("  bbox=%s" % (bbox,)) if bbox else ""))
    return n


def upper_row(tag):
    log = os.path.join(OUT, tag, "run.log")
    if not os.path.exists(log):
        return None
    m = re.search(r"upper screen at row (\d+)", M.text(log))
    return int(m.group(1)) if m else None


def compare(ta, tb):
    bad = 0
    for name in ("stack.bmp", "walk_window_selftest.bmp"):
        pa = os.path.join(OUT, ta, name)
        pb = os.path.join(OUT, tb, name)
        if not (os.path.exists(pa) and os.path.exists(pb)):
            print("%s: MISSING in one capture" % name)
            bad += 1
            continue
        a, b = load_bmp(pa), load_bmp(pb)
        print("%s: %s %s / %s %s" % (name, ta, md5(pa), tb, md5(pb)))
        if a.size != b.size:
            print("  SIZE DIFFERS %s vs %s" % (a.size, b.size))
            bad += 1
            continue
        w, h = a.size
        n = diff_region(a, b, (0, 0, w, h), "whole")
        if name == "stack.bmp":
            top = upper_row(ta)
            if top is None:
                top = 0
            half = h // 2
            n += diff_region(a, b, (0, top, w, top + half), "upper screen")
            lower = 0 if top else half
            n += diff_region(a, b, (0, lower, w, lower + half), "lower screen")
        else:
            n += diff_region(a, b, (0, 0, w, h // 2), "top half")
            n += diff_region(a, b, (0, h // 2, w, h), "bottom half")
        if name == "stack.bmp" and n:
            bad += 1
    print("VERDICT: %s" % ("IDENTICAL presented image" if not bad
                           else "DIFFERS, read the regions above"))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("capture")
    c.add_argument("tag")
    c.add_argument("--level", type=int, default=6)
    c.add_argument("--frames", type=int, default=300)
    c.add_argument("--cap", type=int, default=220)
    k = sub.add_parser("compare")
    k.add_argument("tag_a")
    k.add_argument("tag_b")
    a = ap.parse_args()
    if a.cmd == "capture":
        sys.exit(capture(a.tag, a.level, a.frames, a.cap))
    sys.exit(compare(a.tag_a, a.tag_b))


if __name__ == "__main__":
    main()
