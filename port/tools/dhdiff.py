#!/usr/bin/env python3
"""Find the FIRST frame on which two lockstep VS windows stopped agreeing.

Reads the [dh] / [dh+] lines the divergence detector in port/hal/comms_sync.cpp
writes (arm it with SM64DS_VS_STATE_HASH=1, =2 for per-actor detail) out of two
playlogs, one per window, and reports the earliest frame whose world state
differs -- and, when detail lines are present, exactly which actor owns the
difference and which of its fields moved.

Two-window input lockstep means both windows simulate the same world from the
same inputs, so the first differing frame IS the bug. Every later difference is
that one compounding, which is why this reports the first and then stops
instead of printing a diff of the whole run.

    python port/tools/dhdiff.py p1.log p2.log
    python port/tools/dhdiff.py p1.log p2.log --context 3

Exit status is 0 when the two logs agree on every common frame, 1 when they
diverge, 2 when there is nothing to compare (detector was not armed).
"""

import argparse
import re
import sys

DH = re.compile(r'^\[dh\] f(\d+) n=(\d+) w=([0-9a-f]+) o=([0-9a-f]+)'
                r'(?: rounds=(\d+))?')
# [dh=] f123 1:191:aabbccdd 5:337:11223344 ...  -- the level-2 compact digest
# (uid:actorID:hash). The older uid:hash shape is still accepted.
DHD = re.compile(r'^\[dh=\] f(\d+)((?: \d+(?::\d+)?:[0-9a-f]+)*)\s*$')
DHP = re.compile(
    r'^\[dh\+\] f(\d+) uid=(\d+) id=(\d+) al=(\d+) pos=(-?\d+),(-?\d+),(-?\d+) '
    r'ang=(-?\d+),(-?\d+),(-?\d+) spd=(-?\d+),(-?\d+) fl=([0-9a-f]+) '
    r'h=([0-9a-f]+)(?: w=([0-9a-f]*))?')

FIELDS = ("uid", "id", "alive", "posx", "posy", "posz",
          "angx", "angy", "angz", "spdh", "spdv", "flags", "hash", "win")


def load(path):
    """-> (frames{f:(n,w,o)}, detail{f:{uid:rec}}, digest{f:{uid:(id,hash)}},
           rounds{f:int})"""
    frames, detail, digest, rounds = {}, {}, {}, {}
    with open(path, "r", errors="replace") as fh:
        for line in fh:
            m = DH.match(line)
            if m:
                f = int(m.group(1))
                frames[f] = (int(m.group(2)), m.group(3), m.group(4))
                if m.group(5) is not None:
                    rounds[f] = int(m.group(5))
                continue
            m = DHD.match(line)
            if m:
                f = int(m.group(1))
                d = {}
                for p in m.group(2).split():
                    bits = p.split(":")
                    if len(bits) == 3:      # uid:actorID:hash
                        d[int(bits[0])] = (int(bits[1]), bits[2])
                    elif len(bits) == 2:    # legacy uid:hash
                        d[int(bits[0])] = (None, bits[1])
                digest[f] = d
                continue
            m = DHP.match(line)
            if m:
                f = int(m.group(1))
                g = m.groups()
                rec = dict(zip(FIELDS, (
                    int(g[1]), int(g[2]), int(g[3]),
                    int(g[4]), int(g[5]), int(g[6]),
                    int(g[7]), int(g[8]), int(g[9]),
                    int(g[10]), int(g[11]), g[12], g[13], g[14] or "")))
                detail.setdefault(f, {})[rec["uid"]] = rec
    return frames, detail, digest, rounds


def check_alignment(r1, r2, common):
    """Is frame N in one log the SAME GAME FRAME as frame N in the other?

    It is not safe to assume so. The detector is called from inside
    `if (real_camera)` in walk_window's LEVEL loop, so its frame counter counts
    frames on which it ran -- not the game's frame number -- and two windows
    that spend different amounts of time in menus or level loads can end up
    with the same index meaning different moments. Comparing on a drifted index
    would report a divergence that is not there, and a false positive on a
    single live capture is worse than no capture.

    rounds= is the comms round counter, the exchanged one both consoles agree
    on. Returns (ok, message). ok False means REFUSE rather than guess.
    """
    if not r1 or not r2:
        return True, ("rounds= absent from at least one log (older detector "
                      "build). ALIGNMENT UNVERIFIED -- comparing on the raw "
                      "frame index; treat a reported divergence as needing "
                      "confirmation.")
    shared = [f for f in common if f in r1 and f in r2]
    if not shared:
        return False, "no frame carries rounds= in both logs; cannot align."
    bad = [f for f in shared if r1[f] != r2[f]]
    if not bad:
        return True, ("aligned: rounds= agrees on all %d shared frames."
                      % len(shared))
    # A uniform offset means the two logs are the same run, just indexed from
    # different starts -- recoverable, and worth saying exactly.
    offs = set(r1[f] - r2[f] for f in shared)
    if len(offs) == 1:
        off = offs.pop()
        return False, (
            "MISALIGNED BY A CONSTANT: p1 rounds runs %+d against p2 on all "
            "%d shared frames. The two logs are the same session indexed from "
            "different starting points, so a frame-index comparison would be "
            "comparing different game frames. Re-run the capture, or trim the "
            "leading frames off one log so the rounds= columns line up."
            % (off, len(shared)))
    return False, (
        "MISALIGNED: rounds= disagrees on %d of %d shared frames with no "
        "constant offset (%d distinct offsets seen). The two windows were not "
        "at the same point in the session at the same frame index, which is "
        "exactly the condition that manufactures a false divergence. REFUSING "
        "to compare." % (len(bad), len(shared), len(offs)))


def describe_actor(fr, a, b):
    """Name the fields that differ between one actor's two records."""
    out = []
    for k in FIELDS:
        if k in ("hash",):
            continue
        if a.get(k) != b.get(k):
            out.append("      %-6s p1=%s  p2=%s" % (k, a.get(k), b.get(k)))
    if not out:
        out.append("      (no dumped field differs -- the difference is in a "
                   "word this detector does not read; widen "
                   "SM64DS_VS_STATE_HASH_WIN)")
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("log1")
    ap.add_argument("log2")
    ap.add_argument("--context", type=int, default=2,
                    help="frames of agreeing history to print before the "
                         "divergence (default 2)")
    ap.add_argument("--ignore-alignment", action="store_true",
                    help="compare even when the rounds= check says the two "
                         "logs are not at the same point in the session. This "
                         "makes a false divergence possible; only pass it if "
                         "you know why the columns differ.")
    args = ap.parse_args()

    f1, d1, g1, r1 = load(args.log1)
    f2, d2, g2, r2 = load(args.log2)

    print("p1 %s: %d hashed frames, %d digest, %d verbose" %
          (args.log1, len(f1), len(g1), len(d1)))
    print("p2 %s: %d hashed frames, %d digest, %d verbose" %
          (args.log2, len(f2), len(g2), len(d2)))
    if not f1 or not f2:
        print("NOTHING TO COMPARE: no [dh] lines. Was the detector armed "
              "(SM64DS_VS_STATE_HASH=1) on both windows?")
        return 2

    common = sorted(set(f1) & set(f2))
    only1, only2 = sorted(set(f1) - set(f2)), sorted(set(f2) - set(f1))
    print("common frames: %d   p1-only: %d   p2-only: %d" %
          (len(common), len(only1), len(only2)))
    if not common:
        print("NOTHING TO COMPARE: the two logs share no frame number.")
        return 2

    # ALIGNMENT BEFORE COMPARISON, always. See check_alignment's docstring:
    # the frame index is not guaranteed to mean the same moment in two logs,
    # and comparing a drifted index is the one way this tool invents a
    # divergence that never happened.
    aligned, why = check_alignment(r1, r2, common)
    print("alignment: %s" % why)
    if not aligned:
        if not args.ignore_alignment:
            print()
            print("REFUSING TO COMPARE. A divergence reported off misaligned "
                  "logs would be an artefact of the indexing, not a defect in "
                  "the game. Pass --ignore-alignment only if you know why the "
                  "rounds= columns differ.")
            return 2
        print("  --ignore-alignment given: comparing anyway, results suspect.")
    print()

    first = None
    for fr in common:
        if f1[fr] != f2[fr]:
            first = fr
            break

    if first is None:
        print()
        print("NO DIVERGENCE. The two windows agree on the world hash for all "
              "%d common frames (f%d..f%d)." %
              (len(common), common[0], common[-1]))
        return 0

    print()
    print("FIRST DIVERGENCE AT FRAME %d" % first)
    print()
    idx = common.index(first)
    for fr in common[max(0, idx - args.context):idx]:
        n, w, o = f1[fr]
        print("  f%-6d agree   n=%d w=%s o=%s" % (fr, n, w, o))
    n1, w1, o1 = f1[first]
    n2, w2, o2 = f2[first]
    print("  f%-6d p1      n=%d w=%s o=%s" % (first, n1, w1, o1))
    print("  f%-6d p2      n=%d w=%s o=%s" % (first, n2, w2, o2))
    print()
    if n1 != n2:
        print("  ACTOR COUNT DIFFERS (%d vs %d): one window spawned or killed "
              "something the other did not. That is the divergence; the state "
              "hash difference is downstream of it." % (n1, n2))
    elif o1 != o2 and w1 == w2:
        print("  ORDER ONLY: every actor's state matches but the list came out "
              "in a different order. Look at spawn/kill ordering, not at "
              "behaviour.")

    # THE DIGEST NAMES THE ACTOR, the verbose lines name the FIELD. A live
    # capture runs at level 2 and only has the digest, which is enough to say
    # "uid 5, the chomp" -- the field then comes from a level-3 re-run at the
    # frame this just identified.
    e1, e2 = g1.get(first, {}), g2.get(first, {})
    if e1 and e2:
        moved = [u for u in sorted(set(e1) | set(e2))
                 if e1.get(u) != e2.get(u)]
        if moved:
            print("  DIGEST: %d actor(s) differ at f%d:" % (len(moved), first))
            for u in moved:
                rec = e1.get(u) or e2.get(u)
                aid = rec[0]
                who = "uid=%d actorID=%s" % (u, aid if aid is not None
                                             else "? (legacy uid:hash log)")
                if u not in e1:
                    print("    %s ONLY IN p2" % who)
                elif u not in e2:
                    print("    %s ONLY IN p1" % who)
                else:
                    print("    %s  p1=%s  p2=%s" % (who, e1[u][1], e2[u][1]))
            print()

    a1 = d1.get(first, {})
    a2 = d2.get(first, {})
    if not a1 or not a2:
        if e1 and e2:
            print("  No VERBOSE detail at this frame, so the field that moved "
                  "is not named above. Re-run at SM64DS_VS_STATE_HASH=3 (or "
                  "=1 with SM64DS_VS_STATE_HASH_ID=<actorID> for just the "
                  "suspect) to get it.")
        else:
            print("  No per-actor detail at this frame. Re-run with "
                  "SM64DS_VS_STATE_HASH=2 to name the actor, or =3 to name "
                  "the field.")
        return 1

    culprits = []
    for uid in sorted(set(a1) | set(a2)):
        r1, r2 = a1.get(uid), a2.get(uid)
        if r1 is None or r2 is None:
            culprits.append((uid, r1, r2))
        elif r1["hash"] != r2["hash"]:
            culprits.append((uid, r1, r2))

    if not culprits:
        print("  Per-actor hashes all agree at this frame even though the "
              "world hash does not -- that means an actor is present in one "
              "detail dump and not the other, or the detail lines are "
              "filtered. Re-run at level 2 without a _ID filter.")
        return 1

    print("  %d actor(s) diverged at f%d:" % (len(culprits), first))
    for uid, r1, r2 in culprits:
        if r1 is None:
            print("    uid=%d id=%d ONLY IN p2" % (uid, r2["id"]))
            continue
        if r2 is None:
            print("    uid=%d id=%d ONLY IN p1" % (uid, r1["id"]))
            continue
        print("    uid=%d actorID=%d" % (uid, r1["id"]))
        for line in describe_actor(first, r1, r2):
            print(line)
    return 1


if __name__ == "__main__":
    sys.exit(main())
