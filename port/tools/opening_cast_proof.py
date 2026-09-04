"""Opening-cast visibility proof, measured on the REAL SCREEN.

WHAT IT ANSWERS. The new-file opening spawns three extra hero Players (Mario,
Luigi, Wario) that the kuppa cutscene script positions. They are not in the
data_0209f394[] entrance array, so tests/walk_window.cpp's per-slot draw loop
never reached them and the opening played with a cast of moving shadows and no
bodies. hal/level_boot.cpp's Player Render slot (ps_render) now draws any
player the array does not know. This script is the check that says so.

WHY A SCREEN GRAB AND NOT A FRAMEBUFFER BMP. The black-bottom bug read as a
FULL, correct framebuffer while the window on screen was visibly half black:
the defect lived in the blit, past the point a framebuffer dump can see. So
the input here is a GDI CopyFromScreen grab of walk_window's CLIENT AREA --
the composited desktop at the window's own screen coordinates, i.e. what a
camera pointed at the monitor would see. Produce one with

    powershell -File port/tools/opening_cast_capture.ps1 -Tag after

SCENE SCOPE. Only "grounds" frames are scored -- the castle-lawn shot with the
warp pipes, identified by a tan ground band across the lower middle. The
cutscene also flies over the castle, and the castle ROOF is red, so a red test
run over the whole cutscene passes on roof tiles alone and proves nothing.

THE TWO SIGNALS, per grounds frame:
  shadow -- a dark, desaturated blob on the ground band. ShadowModel::RenderAll
            walks every node, array or not, so the cast's shadow drew even
            while the cast did not. Shadow present == actor alive and placed.
  body   -- hero red: Mario's cap and shirt, Wario's cap trim. Pure red, so
            G ~= B, which the hill green, sky blue, tan ground and green pipes
            have none of. The minimap box is masked out; it has a red icon.

  BUG SIGNATURE  : shadow > 0, body == 0   (a shadow with nobody above it)
  FIXED SIGNATURE: shadow > 0, body >= 300

MEASURED, on this tree, over 19 grounds frames of the same 80s scripted run:
  fix reverted (ps_render back to `return 1`) : 15 shadow frames,  0 body -> FAIL
  fix in place                                : 15 shadow frames, 15 body -> PASS
That reverted-build run is the detector control: the check is only worth
something because it was shown to fail on a deliberately broken binary.
"""
import sys, os, glob
import numpy as np
from PIL import Image

BODY_THRESH = 300      # hero-red pixels; broken scores 0, fixed 1048..9972
PASS_FRAMES = 5        # frames that must show a body; separation is 0 vs 15


def _ground_band(a):
    H, W, _ = a.shape
    return a[int(.80 * H):int(.90 * H), int(.35 * W):int(.65 * W)]


def is_grounds(a):
    g = _ground_band(a)
    R, G, B = g[:, :, 0], g[:, :, 1], g[:, :, 2]
    return ((R > 175) & (G > 145) & (R - B > 40)).mean() > 0.35


def body_px(a):
    H, W, _ = a.shape
    R, G, B = a[:, :, 0], a[:, :, 1], a[:, :, 2]
    keep = np.ones((H, W), bool)
    keep[int(.70 * H):, int(.72 * W):] = False          # minimap box
    return int(((R > 110) & (R - G > 90) & (abs(G - B) < 25) & keep).sum())


def shadow_px(a):
    g = _ground_band(a)
    mx, mn = g.max(axis=2), g.min(axis=2)
    return int(((mx < 170) & (mx - mn < 60)).sum())


def run(d, verbose=True):
    rows = []
    for p in sorted(glob.glob(os.path.join(d, "*.png"))):
        a = np.asarray(Image.open(p).convert("RGB")).astype(np.int16)
        if is_grounds(a):
            rows.append((os.path.basename(p), body_px(a), shadow_px(a)))
    print("== %s ==" % d)
    if not rows:
        print("  no grounds frames in this capture")
        return 2
    if verbose:
        for n, b, s in rows:
            print("  %-24s body=%-6d shadow=%-6d %s" %
                  (n, b, s, "BODY ON SCREEN" if b >= BODY_THRESH else "INVISIBLE"))
    withbody = [r for r in rows if r[1] >= BODY_THRESH]
    withshad = [r for r in rows if r[2] > 0]
    print("  grounds=%d  with-shadow=%d  with-body=%d  max body=%d" %
          (len(rows), len(withshad), len(withbody), max(r[1] for r in rows)))
    ok = len(withbody) >= PASS_FRAMES
    print("  VERDICT: %s\n" % (
        "PASS -- cast body on screen in %d frames" % len(withbody) if ok else
        "FAIL -- cast invisible (shadow on %d frames, no body above it)"
        % len(withshad)))
    return 0 if ok else 1


if __name__ == "__main__":
    dirs = [a for a in sys.argv[1:] if not a.startswith("-")]
    if not dirs:
        print(__doc__)
        print("usage: opening_cast_proof.py <capture-dir> [<capture-dir> ...]")
        sys.exit(2)
    sys.exit(max(run(d, "--quiet" not in sys.argv) for d in dirs))
