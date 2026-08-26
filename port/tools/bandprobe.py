"""The band feature's WHOLE-SCENE-SET probe for the crosser detector's
DISCLOSED RISK: the crosser detector is
source-agnostic, so anything ANIMATED PARKED against a hinge edge row would
also read as a crosser and get blurred into the band.

Sweeps every hosted scene with the shipped detector's own instrument
(SM64DS_BAND_TRACE=1) plus the OAM band census (SM64DS_GAP_PEEK_TRACE=1), and
reports per scene:

  * whether the scene composes a band at all, and its G against the band the
    layout draws (the G-clip question: a clip that changes pixels needs BOTH
    G < band AND an OAM entry reaching the band);
  * how many OAM census lines the peek trace printed -- 0 means no OAM entry's
    box reaches the band on that scene, which is what makes the G clip latent;
  * over a STEADY window (default the second half of the run, past the opening
    fade), the share of frames with any allowed crosser column. A transient
    crossing is a burst; ANIMATED CONTENT PARKED ON AN EDGE ROW is near 100%.

    python tools/bandprobe.py <outdir> [frames] [scene ...]
"""
import os
import re
import subprocess
import sys

TREE = os.path.abspath(os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, os.path.join(TREE, "port", "tools"))
import battery  # noqa: E402

CROSS = re.compile(r"\[band\] f(\d+) scene G (-?\d+) DS rows, layout draws "
                   r"(\d+) \((\d+) host\), upper engine writes band rows "
                   r"0\.\.(-?\d+), lower (-?\d+)\.\.(-?\d+); crossers: "
                   r"upper edge (-?\d+)/256 columns, lower edge (-?\d+)/256")
GHOST = re.compile(r"\[band\] f(\d+) ghost: (\d+) band cell")
PEEK = re.compile(r"\[gappeek\] ")


def run(outdir, scene, frames):
    bmp = os.path.join(outdir, "probe_s%d.bmp" % scene)
    r = subprocess.run(
        [sys.executable,
         os.path.join(os.path.dirname(os.path.abspath(__file__)), "bandrun.py"),
         "win", str(scene), str(frames), bmp, "ON",
         "SM64DS_BAND_TRACE=1", "SM64DS_GAP_PEEK_TRACE=1"],
        capture_output=True, text=True, errors="replace", timeout=900)
    return os.path.splitext(bmp)[0] + ".log", r.returncode


def analyse(log, frames):
    cross = {}
    ghost = {}
    geom = None
    npeek = 0
    rc = None
    if not os.path.exists(log):
        return None
    for ln in open(log, encoding="utf-8", errors="replace"):
        if ln.startswith("# rc="):
            rc = int(ln.strip()[5:])
            continue
        if PEEK.search(ln):
            npeek += 1
            continue
        m = CROSS.search(ln)
        if m:
            f = int(m.group(1))
            cross[f] = (int(m.group(8)), int(m.group(9)))
            if geom is None:
                geom = (int(m.group(2)), int(m.group(3)))
            continue
        m = GHOST.search(ln)
        if m:
            ghost[int(m.group(1))] = int(m.group(2))
    fs = sorted(cross)
    if not fs:
        return {"rc": rc, "band": 0, "npeek": npeek}
    f0 = fs[len(fs) // 2]
    win = [f for f in fs if f >= f0]
    any_frames = sum(1 for f in win if cross[f][0] > 0 or cross[f][1] > 0)
    ref = sum(1 for f in win if cross[f][0] < 0 or cross[f][1] < 0)
    carried = sum(1 for f in win if ghost.get(f, 0))
    peak = max((ghost.get(f, 0) for f in win), default=0)
    return {"rc": rc, "band": 1, "npeek": npeek, "geom": geom,
            "nframes": len(fs), "win": (win[0], win[-1]), "nwin": len(win),
            "any": any_frames, "refused": ref, "carried": carried,
            "peak": peak}


def main(outdir, frames, scenes):
    os.makedirs(outdir, exist_ok=True)
    print("%-6s %-4s %-9s %-6s %-8s %-9s %-8s %-7s %s"
          % ("scene", "rc", "G/band", "frames", "window", "crosser%",
             "carried%", "peak", "gappeek lines"))
    flagged = []
    for sc in scenes:
        log, rc = run(outdir, sc, frames)
        a = analyse(log, frames)
        if a is None:
            print("%-6d %-4s NO LOG" % (sc, "?"))
            continue
        if not a["band"]:
            print("%-6d %-4s %-9s %s" % (sc, a["rc"], "-",
                                         "no band composed (%d gappeek lines)"
                                         % a["npeek"]))
            continue
        pc = 100.0 * a["any"] / a["nwin"]
        pcar = 100.0 * a["carried"] / a["nwin"]
        print("%-6d %-4s %-9s %-6d %-8s %-9s %-8s %-7d %d"
              % (sc, a["rc"], "%d/%d" % a["geom"], a["nframes"],
                 "%d..%d" % a["win"], "%.1f%%" % pc, "%.1f%%" % pcar,
                 a["peak"], a["npeek"]))
        if pc >= 50.0:
            flagged.append((sc, pc, pcar, a["peak"]))
    print()
    if flagged:
        print("FLAGGED (a crosser is present on at least half the steady "
              "frames -- candidate parked animated edge content):")
        for sc, pc, pcar, peak in flagged:
            print("   scene %d: crosser on %.1f%% of steady frames, carry on "
                  "%.1f%%, peak %d band cells" % (sc, pc, pcar, peak))
    else:
        print("FLAGGED: none. No hosted scene keeps a crosser on its edge rows "
              "for half of a steady window.")


if __name__ == "__main__":
    out = sys.argv[1]
    fr = int(sys.argv[2]) if len(sys.argv) > 2 else 420
    sc = [int(a) for a in sys.argv[3:]] or list(battery.hosted_scenes(TREE))
    main(out, fr, sc)
