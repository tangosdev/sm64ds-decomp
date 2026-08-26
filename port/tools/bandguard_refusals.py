"""Where does the width guard refuse, across every hosted scene?

The guard's own note claimed "nothing in these games is wide enough to be
refused wrongly", off two scenes. This asks the whole set, off the banked
sweep logs: for each scene, how many frames the guard refused, whether those
frames are contiguous with the scene's OPENING (frame 1), and what the widest
ALLOWED frame was. A refusal run that starts at frame 1 and ends is a fade; a
refusal in the middle of a steady window is the guard eating a real crossing.

    python refusals.py <sweepdir>
"""
import glob
import os
import re
import sys

CROSS = re.compile(r"\[band\] f(\d+) scene .*crossers: upper edge (-?\d+)/256 "
                   r"columns, lower edge (-?\d+)/256")


def runs(fs):
    out = []
    if not fs:
        return out
    s = p = fs[0]
    for f in fs[1:]:
        if f != p + 1:
            out.append((s, p))
            s = f
        p = f
    out.append((s, p))
    return out


def main(d):
    print("%-6s %-8s %-9s %-11s %s"
          % ("scene", "frames", "refused", "widest-ok", "refusal runs"))
    mid = []
    for p in sorted(glob.glob(os.path.join(d, "probe_s*.log")),
                    key=lambda q: int(re.search(r"_s(\d+)\.log$", q).group(1))):
        sc = int(re.search(r"probe_s(\d+)\.log$", p).group(1))
        w = {}
        for ln in open(p, encoding="utf-8", errors="replace"):
            m = CROSS.search(ln)
            if m:
                w[int(m.group(1))] = (int(m.group(2)), int(m.group(3)))
        if not w:
            continue
        ref = sorted(f for f, (a, b) in w.items() if a < 0 or b < 0)
        ok = [max(a, b) for a, b in w.values() if a >= 0 and b >= 0]
        rr = runs(ref)
        # a refusal run is "an opening" if it starts within the first 5 frames
        late = [(a, b) for a, b in rr if a > 5]
        print("%-6d %-8d %-9d %-11d %s"
              % (sc, len(w), len(ref), max(ok) if ok else 0,
                 ", ".join("%d..%d" % r for r in rr) or "none"))
        if late:
            mid.append((sc, late))
    print()
    if mid:
        print("REFUSALS AWAY FROM THE SCENE OPENING (candidate real crossings "
              "the guard ate):")
        for sc, late in mid:
            print("   scene %d: %s" % (sc, ", ".join("%d..%d" % r
                                                     for r in late)))
    else:
        print("EVERY refusal in the whole hosted set begins within the first "
              "five composed frames -- i.e. every one is a scene opening, and "
              "the guard has never refused a frame in a steady window.")


main(sys.argv[1])
