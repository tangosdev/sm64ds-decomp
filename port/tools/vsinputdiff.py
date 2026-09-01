#!/usr/bin/env python3
"""Audit the lockstep INPUT plumbing from two VS windows' playlogs.

Two-window VS trades controller records and simulates the world from them, so
before anyone reads a line of enemy code a desync report needs the wire ruled
in or out. This does that from the `[comms:level]` dump both windows already
write (SM64DS_COMMS_REPORT), with no rebuild and no re-run, and answers the two
questions that between them cover the plumbing:

  CROSS   For every frame, does window A's copy of each slot's record match
          window B's copy? A mismatch means the transport dropped, reordered
          or corrupted something and the two worlds were fed different inputs.

  SELF    Within ONE window, does the record it PUBLISHED for a frame match
          the record it READS BACK for its own slot? Everything the game reads
          comes from the slot array, so a window whose own slot disagrees with
          what it published is steering by one value while the world is
          simulated from another. head= is the camera heading, exactly the kind
          of value that would do this quietly.

SELF applies a one-frame shift by default. That is not a fudge: the `local`
line carries this frame's outgoing record while the slot lines carry the
exchanged records, which are stamped one behind. The shift is reported, and
--self-shift lets you check that the offset really is uniform (a genuine
corruption does not vanish at exactly one shift, a labelling offset does).

    python port/tools/vsinputdiff.py p1.log p2.log

Exit 0 when both audits are clean, 1 when either finds a mismatch, 2 when
there is nothing to compare.
"""

import argparse
import collections
import re
import sys

# [comms:level]   slot0 frame=17 stylus={..} flag=.. key=.. head=.. | touch{..} pad{..}
SLOT = re.compile(r'^\[comms:level\]\s+slot(\d) frame=(\d+) ([^|]*?)(?:\s*\|\s*(.*))?$')
# [comms:level]   local  frame=18 stylus={..} flag=.. key=.. head=..
LOCAL = re.compile(r'^\[comms:level\]\s+local\s+frame=(\d+) (.*)$')


def load(path):
    """-> (slots{frame:{slot:(core, full, line)}}, local{frame:(core, line)}, conflicts)"""
    slots = collections.defaultdict(dict)
    local = {}
    conflicts = []
    with open(path, 'r', errors='replace') as fh:
        for ln, line in enumerate(fh, 1):
            line = line.rstrip()
            m = LOCAL.match(line)
            if m:
                local.setdefault(int(m.group(1)), (m.group(2).strip(), ln))
                continue
            m = SLOT.match(line)
            if m:
                slot, frame = int(m.group(1)), int(m.group(2))
                core = m.group(3).strip()
                full = core + (" | " + m.group(4).strip() if m.group(4) else "")
                if slot not in slots[frame]:
                    slots[frame][slot] = (core, full, ln)
                elif slots[frame][slot][1] != full:
                    conflicts.append((frame, slot, slots[frame][slot], (core, full, ln)))
    return slots, local, conflicts


def audit_cross(a, b, nslots, limit):
    common = sorted(set(a) & set(b))
    print("CROSS-WINDOW: %d common frames (p1-only %d, p2-only %d)"
          % (len(common), len(set(a) - set(b)), len(set(b) - set(a))))
    if not common:
        print("  NOTHING TO COMPARE.")
        return None
    bad = 0
    for fr in common:
        for slot in range(nslots):
            pa, pb = a[fr].get(slot), b[fr].get(slot)
            if pa is None or pb is None:
                continue
            if pa[1] != pb[1]:
                bad += 1
                if bad <= limit:
                    print("  MISMATCH frame=%d slot=%d" % (fr, slot))
                    print("     p1 (line %d): %s" % (pa[2], pa[1]))
                    print("     p2 (line %d): %s" % (pb[2], pb[1]))
    print("  mismatches: %d over %d frames -> %s"
          % (bad, len(common), "CLEAN" if bad == 0 else "DIVERGED"))
    return bad


def audit_self(tag, slots, local, own, shift, limit):
    slot_own = {f: v[own] for f, v in slots.items() if own in v}
    common = [f for f in sorted(slot_own) if (f + shift) in local]
    if not common:
        print("SELF (%s, own slot %d): nothing to compare." % (tag, own))
        return None
    bad = 0
    for fr in common:
        pub, publn = local[fr + shift]
        got = slot_own[fr]
        if pub != got[0]:
            bad += 1
            if bad <= limit:
                print("  MISMATCH frame=%d" % fr)
                print("     published (line %d): %s" % (publn, pub))
                print("     read back (line %d): %s" % (got[2], got[0]))
    print("SELF (%s, own slot %d, shift %+d): %d mismatches over %d -> %s"
          % (tag, own, shift, bad, len(common), "CLEAN" if bad == 0 else "DIVERGED"))
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("log1")
    ap.add_argument("log2")
    ap.add_argument("--slots", type=int, default=4)
    ap.add_argument("--self-shift", type=int, default=1,
                    help="frames the exchanged record's stamp lags the local "
                         "counter (default 1; try 0 and -1 to confirm the "
                         "offset is uniform rather than corruption)")
    ap.add_argument("--limit", type=int, default=10,
                    help="max mismatches to print per audit")
    args = ap.parse_args()

    a_slots, a_local, a_conf = load(args.log1)
    b_slots, b_local, b_conf = load(args.log2)
    for tag, conf in (("p1", a_conf), ("p2", b_conf)):
        if conf:
            print("%s: %d SELF-CONFLICTS (same frame+slot reported twice with "
                  "different payloads) -- the log itself disagrees, read these "
                  "before anything else:" % (tag, len(conf)))
            for row in conf[:args.limit]:
                print("   frame=%d slot=%d: %r vs %r" % (row[0], row[1], row[2][1], row[3][1]))

    if not a_slots or not b_slots:
        print("NOTHING TO COMPARE: no [comms:level] slot lines. Was "
              "SM64DS_COMMS_REPORT set on both windows?")
        return 2

    bad = 0
    c = audit_cross(a_slots, b_slots, args.slots, args.limit)
    bad += c or 0
    print()
    for tag, slots, local, own in (("p1", a_slots, a_local, 0),
                                   ("p2", b_slots, b_local, 1)):
        s = audit_self(tag, slots, local, own, args.self_shift, args.limit)
        bad += s or 0
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
