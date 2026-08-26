"""Does a REFUSED edge still contribute anything of this lane's?

The right question, and the one a whole-file sha of a windowed capture cannot
answer. The ambient wash's drawn endpoint is an IIR follower: its state at
frame N depends on every earlier frame, so once the two arms have legitimately
differed anywhere (a crossing, a non-refused edge doing its job) the DRAWN band
keeps differing for as long as the follower takes to reconverge -- even on a
frame where the refused edge contributes nothing at all.

So the history-free statement is about the RAW SAMPLE, which is a pure function
of this frame's inputs:

    on every frame where the width guard refused an edge, is that edge's raw
    sample byte-identical between the fix-on and fix-off arms?

If yes, the refused edge draws exactly the pre-lane picture and nothing of the
lane's reaches it. Anything the drawn band still shows on such a frame is the
follower remembering frames where the arms were ALLOWED to differ.

    python refused_leak.py <on.log> <off.log>
"""
import re
import sys

AMB = re.compile(r"\[ambtrace\] f(\d+) t_raw=(\S+) t_out=(\S+) b_raw=(\S+) "
                 r"b_out=(\S+)")
BAND = re.compile(r"\[band\] f(\d+) scene .*crossers: upper edge (-?\d+)/256 "
                  r"columns, lower edge (-?\d+)/256")


def read(p):
    amb, ref = {}, {}
    for ln in open(p, encoding="utf-8", errors="replace"):
        m = AMB.search(ln)
        if m:
            amb[int(m.group(1))] = (m.group(2), m.group(4))   # t_raw, b_raw
            continue
        m = BAND.search(ln)
        if m:
            ref[int(m.group(1))] = (int(m.group(2)) < 0, int(m.group(3)) < 0)
    return amb, ref


def main(on, off):
    a_amb, a_ref = read(on)
    b_amb, _ = read(off)
    frames = sorted(set(a_amb) & set(b_amb) & set(a_ref))
    nref = [0, 0]
    leak = [[], []]
    difftotal = 0
    for f in frames:
        for e in (0, 1):
            if not a_ref[f][e]:
                continue
            nref[e] += 1
            if a_amb[f][e] != b_amb[f][e]:
                leak[e].append(f)
        if a_amb[f] != b_amb[f]:
            difftotal += 1
    print("frames compared                     : %d" % len(frames))
    print("frames where the arms' RAW samples "
          "differ at all (any edge)         : %d" % difftotal)
    print()
    for e, name in ((0, "upper"), (1, "lower")):
        print("%s edge refused on %d frame(s)" % (name, nref[e]))
        if not nref[e]:
            continue
        if leak[e]:
            print("   *** LEAK: its raw sample differs from the pre-lane arm "
                  "on %d of them: %s" % (len(leak[e]), leak[e][:12]))
        else:
            print("   raw sample byte-identical to the pre-lane arm on ALL "
                  "%d -- nothing of this lane's reaches a refused edge"
                  % nref[e])


main(sys.argv[1], sys.argv[2])
