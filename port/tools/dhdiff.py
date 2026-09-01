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

DH = re.compile(r'^\[dh\] f(\d+) n=(\d+) w=([0-9a-f]+) o=([0-9a-f]+)')
# [dh=] f123 1:aabbccdd 5:11223344 ...   -- the level-2 compact digest
DHD = re.compile(r'^\[dh=\] f(\d+)((?: \d+:[0-9a-f]+)*)\s*$')
DHP = re.compile(
    r'^\[dh\+\] f(\d+) uid=(\d+) id=(\d+) al=(\d+) pos=(-?\d+),(-?\d+),(-?\d+) '
    r'ang=(-?\d+),(-?\d+),(-?\d+) spd=(-?\d+),(-?\d+) fl=([0-9a-f]+) '
    r'h=([0-9a-f]+)(?: w=([0-9a-f]*))?')

FIELDS = ("uid", "id", "alive", "posx", "posy", "posz",
          "angx", "angy", "angz", "spdh", "spdv", "flags", "hash", "win")


def load(path):
    """-> (frames: {f: (n, w, o)}, detail: {f: {uid: dict}}, digest: {f: {uid: hash}})"""
    frames, detail, digest = {}, {}, {}
    with open(path, "r", errors="replace") as fh:
        for line in fh:
            m = DH.match(line)
            if m:
                f = int(m.group(1))
                frames[f] = (int(m.group(2)), m.group(3), m.group(4))
                continue
            m = DHD.match(line)
            if m:
                f = int(m.group(1))
                digest[f] = dict(
                    (int(p.split(":")[0]), p.split(":")[1])
                    for p in m.group(2).split() if ":" in p)
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
    return frames, detail, digest


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
    args = ap.parse_args()

    f1, d1, g1 = load(args.log1)
    f2, d2, g2 = load(args.log2)

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
                if u not in e1:
                    print("    uid=%d ONLY IN p2" % u)
                elif u not in e2:
                    print("    uid=%d ONLY IN p1" % u)
                else:
                    print("    uid=%d  p1=%s  p2=%s" % (u, e1[u], e2[u]))
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
