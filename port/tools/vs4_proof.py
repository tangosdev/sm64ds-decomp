"""FOUR players, one session, one arena -- the exit proof for run vs4p.

WHY THIS FILE EXISTS AND WHY IT IS NOT A ROW IN mp3_proof.py. mp3_proof asks
whether a SECOND player exists and moves; every rung it has passes on a build
that can never seat a third. The questions below are the ones that only appear
at three and four, and each of them was a real failure at cons 577b48832
before this lane:

  rung 1  THE ROSTER.   All four consoles agree the session has four players.
                        Before the fix the parent seated its world on the FIRST
                        peer, so four windows reported counts 2 / 2 / 3 / 4 and
                        the two that seated early had slots 2 and 3 NULL
                        forever. This is the rung that fails on a regression of
                        hal/comms_conductor.cpp's wait.
  rung 2  THE ROUND.    All four consoles START on the same comms round. The
                        count alone is not enough: with SM64DS_VS_PLAYERS=4
                        forced but the wait unfixed, all four seated four
                        actors and STILL disagreed, because they had seated at
                        rounds 0, 0, 54 and 99 and so gave slot 3 a different
                        number of input-less frames each. Player 3 was on the
                        arena in two windows and fallen out of it in the other
                        two.
  rung 3  THE BODIES.   Four distinct actors per window, mPlayerNo 0..3, on the
                        arena's four OWN entrance records, with no
                        port-supplied stand-in start.
  rung 4  THE COLOURS.  Four distinct palette rows per window. The ROM computes
                        `Player+0x61C = base + (mPlayerNo << 1)`, one 16-colour
                        row per slot; this asserts four different Yoshis rather
                        than four Yoshis.
  rung 5  THE PADS.     Every window reads every OTHER window's pad. Each
                        instance holds one distinct D-pad direction, so a
                        window that reads its own key on somebody else's slot
                        fails here.
  rung 6  AGREEMENT.    The four windows agree on where all four players are.
                        Positions are compared at a common frame with a
                        tolerance, because the windows are not screenshot-
                        synchronous; the DIGEST is what proves exactness, and
                        that is rung 8.
  rung 7  THE MATCH.    A match runs to MATCH OVER and the marker carries FOUR
                        scores.
  rung 8  THE DIGEST.   port/tools/dhdiff.py over ALL SIX PAIRINGS with the
                        alignment gate live. Six, not three: a divergence
                        between two children is exactly as fatal as one against
                        the parent, and only the full pairing set catches a
                        window that agrees with its neighbour and with nobody
                        else.

QUIET AND MUTED, through mp2_proof.env_base, which is also what scrubs an
inherited SM64DS_* out of the environment so a lane's own knobs cannot leak in.

    python port/tools/vs4_proof.py [--frames N] [--map 0..3] [--keep]
"""
import argparse
import os
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vs4_proof")

# One held D-pad direction per window. Distinct on purpose: a window that reads
# its OWN key on another slot is the shape of a fan-out that never crossed the
# wire, and identical keys would hide it.
KEYS = (0x0040, 0x0080, 0x0020, 0x0010)

# THE PORT BASE IS DERIVED FROM THIS PROCESS'S PID, never a constant. The
# owner's live desk pair is 51765 and the 58434..58733 band is spoken for, so
# the window below avoids both and steps by 8 to leave slot k = base + k clear.
def port_base():
    return 42000 + (os.getpid() % 900) * 8


PROBE = re.compile(
    r"^\[vs\] f(\d+) slot(\d+) actor=([0-9A-Fa-f]+|NULL)"
    r"(?: no=(\d+) char=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)"
    r".*?pad=([0-9a-f]+).*?pal=(\d+))?", re.M)
COUNT = re.compile(r"^\[vs\] f(\d+) count=(\d+) me=(\d+) live=([\d,]+)", re.M)
SEAT = re.compile(r"^\s*\[a2\] VS: (\d+) players, I am slot (\d+)", re.M)
ROUND0 = re.compile(r"(?:accepted as slot \d+|slot \d+ joined) at round (\d+)")
MATCHOVER = re.compile(
    r"^\[vs\] MATCH OVER f(\d+) win=(\S+) scores=(-?\d+),(-?\d+),(-?\d+),(-?\d+)",
    re.M)


def rows(t):
    """-> {frame: {slot: dict|None}}"""
    out = {}
    for m in PROBE.finditer(t):
        f, s = int(m.group(1)), int(m.group(2))
        d = out.setdefault(f, {})
        if m.group(3) == "NULL" or m.group(4) is None:
            d[s] = None
            continue
        d[s] = dict(actor=m.group(3), no=int(m.group(4)), char=int(m.group(5)),
                    pos=(int(m.group(6)), int(m.group(7)), int(m.group(8))),
                    pad=int(m.group(9), 16), pal=int(m.group(10)))
    return out


# THE MATCH FIXTURE, and every window gets IT IDENTICALLY or the proof breaks
# the thing it is proving. SM64DS_VS_BREAKALL wakes the arena's caged stars and
# SM64DS_STAR_TRIGGER arms a collect on the named frames; both are LOCAL
# mutations that never cross the wire, so they are only lockstep-safe because
# each picks its actor by the same deterministic walk of the live list at the
# same frame on every console. Set them on one window and not another and the
# four worlds part company immediately -- which rung 8 would then report, quite
# correctly, as a desync.
BREAK_AT = "120"
COLLECT_AT = "200,280"

def launch(out, frames, vsmap, base, star_target, stagger):
    """Four instances, parent first. Returns [(rc, text, logpath), ...]."""
    procs, dirs, logs = [], [], []
    for k in range(4):
        d = os.path.join(out, "p%d" % k)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        dirs.append(d)
        logs.append(os.path.join(d, "run.log"))

    for k in range(4):
        e = M.env_base(ROOT, dirs[k], "p%d" % k)
        # A VS boot names its own destination and must not carry a second --
        # env_base pins SM64DS_LEVEL and the two would fight.
        e.pop("SM64DS_LEVEL", None)
        e["SM64DS_VS_MAP"] = str(vsmap)
        e["SM64DS_WINDOW_SELFTEST"] = str(frames)
        e["SM64DS_VS_PROBE"] = "1"
        # THE COUNT IS THE EXPECTATION, and it is what hal/comms_conductor.cpp
        # now waits on. This is the same variable the lobby's arming freeze
        # forces, so the harness and the product set it for the same reason.
        e["SM64DS_VS_PLAYERS"] = "4"
        e["SM64DS_COMMS_ROLE"] = "parent" if k == 0 else "child"
        e["SM64DS_COMMS_PORT"] = str(base)
        e["SM64DS_COMMS_REPORT"] = "1"
        # The desync tracker, at the level whose digest names the actor.
        e["SM64DS_VS_STATE_HASH"] = "2"
        e["SM64DS_COMMS_INJECT"] = "key=0x%04x" % KEYS[k]
        if k:
            # PIN THE SLOT. Over loopback the port would decide it anyway, but
            # pinning is what a four-player session needs over the RELAY, where
            # the ACCEPT is a broadcast with no recipient field and the parent
            # cannot resolve two children that both claim slot 1. Proving the
            # pinned shape here is what makes the lobby's seat->slot duty
            # testable.
            e["SM64DS_COMMS_SLOT"] = str(k)
        if star_target:
            e["SM64DS_VS_STAR_TARGET"] = str(star_target)
            e["SM64DS_VS_BREAKALL"] = BREAK_AT
            e["SM64DS_STAR_TRIGGER"] = COLLECT_AT
            e["SM64DS_VS_END_GRACE"] = "120"
            # The end flow's close is the DEFAULT in the product; naming it
            # here is what makes rung 7 a test of the close and not only of
            # the marker.
            e["SM64DS_VS_EXIT_ON_END"] = "1"
        procs.append(M.spawn(EXE, dirs[k], e, logs[k]))
        # STAGGERED, and the stagger is part of the test. Four processes that
        # started together would hide the very defect this lane closed: the
        # window a late console joins in is exactly when an early one used to
        # seat a short world.
        if k < 3:
            time.sleep(stagger)

    res = []
    for k in range(4):
        rc = M.finish(procs[k], 900)
        res.append((rc, M.text(logs[k]), logs[k]))
    return res


def common_frame(all_rows, want_slots=4):
    """The latest frame every window probed with all `want_slots` seated."""
    shared = None
    for r in all_rows:
        have = set(f for f, d in r.items()
                   if len(d) >= want_slots
                   and all(d.get(s) for s in range(want_slots)))
        shared = have if shared is None else (shared & have)
    return max(shared) if shared else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", default="1200")
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--star-target", type=int, default=2,
                    help="first-to-N ends the match; the fixture collects two, "
                         "so 2 lands it. 0 leaves the ROM's own clock as the "
                         "only trigger, which outlasts a practical selftest "
                         "(time-up is around f2791)")
    ap.add_argument("--stagger", type=float, default=1.0)
    ap.add_argument("--port", type=int, default=0)
    args = ap.parse_args()

    base = args.port or port_base()
    print("vs4_proof: four windows, map %d, %s frames, port base %d"
          % (args.map, args.frames, base), flush=True)
    res = launch(OUT, args.frames, args.map, base, args.star_target,
                 args.stagger)
    texts = [t for _, t, _ in res]
    logs = [p for _, _, p in res]
    all_rows = [rows(t) for t in texts]
    ok = True

    for k, (rc, _, lg) in enumerate(res):
        ok &= M.verdict(rc == 0, "window %d exited clean | rc=%d %s"
                        % (k, rc, lg))

    # ---- rung 1: the roster -------------------------------------------------
    seats = [SEAT.search(t) for t in texts]
    counts = [int(m.group(1)) if m else -1 for m in seats]
    slots = [int(m.group(2)) if m else -1 for m in seats]
    ok &= M.verdict(counts == [4, 4, 4, 4],
                    "rung1 ALL FOUR consoles seated a four-player world | "
                    "counts=%s (before this lane: 2,2,3,4 -- the parent seated "
                    "on the first peer and slots 2 and 3 never got an actor)"
                    % (counts,))
    ok &= M.verdict(slots == [0, 1, 2, 3],
                    "rung1 and each knows which one it is | slots=%s" % (slots,))

    # ---- rung 2: the round --------------------------------------------------
    starts = []
    for t in texts:
        m = ROUND0.search(t)
        starts.append(int(m.group(1)) if m else -1)
    ok &= M.verdict(len(set(starts)) == 1 and starts[0] >= 0,
                    "rung2 all four consoles START ON THE SAME ROUND | "
                    "rounds=%s. Different rounds means the same inputs are fed "
                    "to different frames, which is a different match even when "
                    "the count agrees." % (starts,))

    # ---- rungs 3/4/5: bodies, colours, pads --------------------------------
    cf = common_frame(all_rows)
    if cf is None:
        M.verdict(False, "no frame has all four slots seated in all four "
                         "windows; rungs 3-6 cannot run")
        print("\n".join(M.VERDICTS))
        return 1
    print("  (comparing at frame %d, the latest all four windows share)" % cf)

    for k, r in enumerate(all_rows):
        d = r[cf]
        nos = [d[s]["no"] for s in range(4)]
        actors = set(d[s]["actor"] for s in range(4))
        chars = set(d[s]["char"] for s in range(4))
        pals = [d[s]["pal"] for s in range(4)]
        pads = [d[s]["pad"] for s in range(4)]
        ok &= M.verdict(nos == [0, 1, 2, 3] and len(actors) == 4,
                        "rung3 w%d four distinct actors, mPlayerNo 0..3 | "
                        "no=%s actors=%d" % (k, nos, len(actors)))
        ok &= M.verdict(chars == {3},
                        "rung3 w%d every slot is Yoshi, the ROM's VS arm | "
                        "char=%s" % (k, sorted(chars)))
        ok &= M.verdict(len(set(pals)) == 4,
                        "rung4 w%d FOUR COLOURS, not four Yoshis | pal=%s "
                        "(base + (mPlayerNo << 1), one 16-colour row a slot)"
                        % (k, pals))
        ok &= M.verdict(pads == [KEYS[0], KEYS[1], KEYS[2], KEYS[3]],
                        "rung5 w%d reads every OTHER window's pad | pads=%s "
                        "expected=%s" % (k, ["%04x" % p for p in pads],
                                         ["%04x" % p for p in KEYS]))
        ok &= M.verdict("port-supplied start" not in texts[k],
                        "rung3 w%d every slot came from the ARENA's own "
                        "entrance table, not the stand-in" % k)

    # ---- rung 6: the four windows agree on where everybody is --------------
    # TOLERANCE, and it is not a fudge: the windows are not screenshot-
    # synchronous, so a probe line labelled frame N is taken at the same
    # SIMULATION frame but at a different wall time, and one console may have
    # consumed a round the others have not. 4096 Fix12i is ONE world unit
    # against players who move 9 to 40 units a frame, so a real divergence
    # cannot hide under it. Exactness is rung 8's job.
    TOL = 4096
    for s in range(4):
        ref = all_rows[0][cf][s]["pos"]
        worst, who = 0, 0
        for k in range(1, 4):
            p = all_rows[k][cf][s]["pos"]
            dmax = max(abs(a - b) for a, b in zip(ref, p))
            if dmax > worst:
                worst, who = dmax, k
        ok &= M.verdict(worst <= TOL,
                        "rung6 slot %d is in the same place in all four "
                        "windows | worst delta %d Fix12i (window %d) against a "
                        "%d tolerance" % (s, worst, who, TOL))

    # ---- rung 7: the match ends, with four scores --------------------------
    ends = [MATCHOVER.search(t) for t in texts]
    got = sum(1 for m in ends if m)
    ok &= M.verdict(got == 4,
                    "rung7 the match ended in ALL FOUR windows | %d of 4 "
                    "carry a MATCH OVER marker" % got)
    if got == 4:
        boards = [tuple(int(m.group(i)) for i in range(3, 7)) for m in ends]
        ok &= M.verdict(len(set(boards)) == 1,
                        "rung7 and the FOUR scores in the marker agree across "
                        "the windows | %s" % (boards,))

    # ---- rung 8: the digest, all six pairings ------------------------------
    dh = os.path.join(os.path.dirname(os.path.abspath(__file__)), "dhdiff.py")
    for a in range(4):
        for b in range(a + 1, 4):
            r = subprocess.run([sys.executable, dh, logs[a], logs[b]],
                               capture_output=True, text=True)
            tail = (r.stdout or r.stderr).strip().splitlines()
            ok &= M.verdict(r.returncode == 0,
                            "rung8 p%d vs p%d agree on every common frame | "
                            "dhdiff rc=%d %s" % (a, b, r.returncode,
                                                 tail[-1] if tail else ""))
            if r.returncode != 0:
                print("\n".join("      " + x for x in tail[-12:]))

    print("\nVERDICT:", "ALL GREEN" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
