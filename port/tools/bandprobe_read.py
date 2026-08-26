"""Read the whole-scene-set probe logs and answer two questions per scene.

  (f) THE DISCLOSED RISK. Over a STEADY window (the second half of the run,
      past the opening fade), what share of composed band frames has the
      shipped detector calling something a crosser? A transient crossing is a
      burst; ANIMATED CONTENT PARKED ON AN EDGE ROW is near 100%.

  THE G CLIP'S LATENCY. band_window changes a pixel only when BOTH the game's
  G is smaller than the band AND an OAM entry actually reaches the band. The
  peek census answers the second: `box reaches N row(s)` with N > 1 can only
  come from a BAND raster (gap_ds 32); N == 1 is the edge-row mask raster
  band_edge_obj_masks runs at gap_ds 1, which is not the band.

    python tools/bandprobe_read.py <probedir>
"""
import glob
import os
import re
import sys  # noqa: F401

CROSS = re.compile(r"\[band\] f(\d+) scene G (-?\d+) DS rows, layout draws "
                   r"(\d+) \((\d+) host\)")
CROSSN = re.compile(r"crossers: upper edge (-?\d+)/256 columns, lower edge "
                    r"(-?\d+)/256")
GHOST = re.compile(r"\[band\] f(\d+) ghost: (\d+) band cell")
PEEK = re.compile(r"\[gappeek\] .*box reaches (\d+) row\(s\), (\d+) pixel")


def one(path):
    sc = int(re.search(r"probe_s(\d+)\.log$", path).group(1))
    cross = {}
    ghost = {}
    geom = None
    rc = None
    peek1 = 0
    peekN = 0
    peek_px = 0
    for ln in open(path, encoding="utf-8", errors="replace"):
        if ln.startswith("# rc="):
            rc = int(ln.strip()[5:])
            continue
        m = PEEK.search(ln)
        if m:
            if int(m.group(1)) > 1:
                peekN += 1
                peek_px += int(m.group(2))
            else:
                peek1 += 1
            continue
        m = CROSS.search(ln)
        if m:
            f = int(m.group(1))
            n = CROSSN.search(ln)
            cross[f] = (int(n.group(1)), int(n.group(2))) if n else (0, 0)
            if geom is None:
                geom = (int(m.group(2)), int(m.group(3)))
            continue
        m = GHOST.search(ln)
        if m:
            ghost[int(m.group(1))] = int(m.group(2))
    return sc, rc, geom, cross, ghost, peek1, peekN, peek_px


def main(d):
    rows = []
    for p in sorted(glob.glob(os.path.join(d, "probe_s*.log")),
                    key=lambda q: int(re.search(r"_s(\d+)\.log$", q).group(1))):
        rows.append(one(p))
    print("%-6s %-3s %-9s %-7s %-9s %-9s %-7s %-12s %s"
          % ("scene", "rc", "G/band", "frames", "crosser%", "carry%", "peak",
             "OAM-in-band", "clip?"))
    flagged = []
    clip_live = []
    for sc, rc, geom, cross, ghost, peek1, peekN, peek_px in rows:
        if not cross:
            print("%-6d %-3s %-9s %s" % (sc, rc, "-",
                                         "no band composed  (edge-mask peek "
                                         "lines %d, band peek lines %d)"
                                         % (peek1, peekN)))
            continue
        fs = sorted(cross)
        win = fs[len(fs) // 2:]
        anyc = sum(1 for f in win if cross[f][0] > 0 or cross[f][1] > 0)
        car = sum(1 for f in win if ghost.get(f, 0))
        peak = max((ghost.get(f, 0) for f in win), default=0)
        pc = 100.0 * anyc / len(win)
        clip = "INERT (G==band)" if geom[0] >= geom[1] or geom[0] <= 0 else (
            "LIVE" if peekN else "latent (no OAM in band)")
        print("%-6d %-3s %-9s %-7d %-9s %-9s %-7d %-12s %s"
              % (sc, rc, "%d/%d" % geom, len(fs), "%.1f%%" % pc,
                 "%.1f%%" % (100.0 * car / len(win)), peak,
                 "%d lines" % peekN, clip))
        if pc >= 50.0:
            flagged.append((sc, pc, peak))
        if clip == "LIVE":
            clip_live.append(sc)
    print()
    if flagged:
        print("(f) FLAGGED -- a crosser present on >=50%% of steady frames:")
        for sc, pc, peak in flagged:
            print("    scene %d: %.1f%% of steady frames, peak %d band cells"
                  % (sc, pc, peak))
    else:
        print("(f) FLAGGED: NONE. No hosted scene keeps the detector's crosser "
              "signal up across half of a steady window.")
    print()
    if clip_live:
        print("G CLIP CHANGES PIXELS on scene(s): %s" % clip_live)
    else:
        print("G CLIP: latent on every hosted scene -- no scene has BOTH "
              "G < band AND an OAM entry reaching the band.")


if __name__ == "__main__":
    main(sys.argv[1])
