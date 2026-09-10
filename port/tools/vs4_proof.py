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

AND IT TAKES THE MACHINE-WIDE WINDOWED TEST SLOT (port/tools/slot_lock.py) the
way battery.py and thread_create_proof.py take it, which it did not until run
link100. Four game windows walking an arena for twenty seconds of wall time are
exactly what that lock exists to serialise, and without it this proof ran
alongside whatever battery another lane had in flight. That is not a
theoretical hazard: lane FANTAIL's first vs4 run FAILED while lane R3CFIX had
battery windows up and went ALL GREEN 40/0 when re-run isolated, so an
unlocked red here cannot be told apart from a real desync -- which is the one
thing this proof is for. The hold is a PHASE hold across all four windows
(slot_lock.slot_reentrant, the same call battery.py uses for the same reason):
the four instances ARE one session and must be in flight together, so locking
per launch would deadlock the proof against itself. It is re-entrant so that a
caller that already holds the slot -- a battery row, or a lane holding it
across a whole gate -- pays nothing and keeps its own hold. Nothing changes
when SM64DS_TEST_LOCK is unset: slot_lock is opt-in and a bare run launches
exactly as it always did.

AND IT READS A ROLLBACK LOG, WHICH IS WHAT IT IS. Run link100, lane VS4BISECT.
The port runs rollback netcode in a VS session (hal/rollback.cpp; the windows
log "rollback moved the host counter N -> M; re-anchoring the ROM frame number
with it"), so ONE frame number is simulated more than once: once on predicted
remote input, then again after each rewind on the real input. Measured here,
about 150 of 401 frames per window are re-simulated, up to four times, and the
count differs per window every run because it follows packet timing.

Every rung below that indexes by frame therefore has to say WHICH simulation of
that frame it means. Two of them did not:

  * rungs 3-6 read the [vs] probe rows into a frame-keyed dict, so they hold the
    last emission -- the settled one -- but rung 6 then compared at
    max(shared frame), the very LAST frame of the run. That frame is where each
    window's selftest budget expired mid-frame, so it is the one frame whose
    settled value is not settled. It produced "slot 0 worst delta 4098529
    Fix12i" on windows whose worlds were byte-identical everywhere else.
  * rung 8 handed dhdiff.py the raw logs. dhdiff reads last-wins too, but its
    alignment gate compares the rounds= column on EVERY shared frame including
    that same ragged last one, and refuses the whole comparison (rc=2) over a
    one-frame disagreement. A refusal is not a divergence and must not be
    reported as one.

So this file now computes a SETTLED FRAME -- the latest frame that every window
has finished and that all four agree the round number of -- compares rungs 3-6
there, and hands rung 8 copies of the logs with the unsettled tail trimmed. It
also separates dhdiff rc=2 (REFUSED, nothing was compared) from rc=1 (DIVERGED).
Nothing is relaxed: a divergence in settled state is still red on every pairing,
which is the only thing a lockstep session promises.

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
import slot_lock  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vs4_proof")

# One held D-pad direction per window. Distinct on purpose: a window that reads
# its OWN key on another slot is the shape of a fan-out that never crossed the
# wire, and identical keys would hide it.
KEYS = (0x0040, 0x0080, 0x0020, 0x0010)

# What each window is given before it is killed. Named rather than typed into
# M.finish() twice, because the slot hold below is DERIVED from it: the lock's
# declared bound and the run's own budget have to move together or the stale
# backstop breaks a hold that is still legitimately running.
RUN_TIMEOUT = 900

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
# The divergence detector's own lines (hal/comms_sync.cpp). rounds= is the
# EXCHANGED counter both consoles agree on, which is why dhdiff.py aligns on it
# and why the settled frame below is defined by it.
DH = re.compile(r"^\[dh\] f(\d+) .*?rounds=(\d+)", re.M)
DHLINE = re.compile(r"^\[dh[=+]?\] f(\d+)\b")


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
    """Four instances, parent first, HOLDING THE WINDOWED TEST SLOT.

    The hold spans the whole phase -- all four spawns and all four waits --
    because the four instances are one session and have to be in flight at the
    same time; a per-launch lock would make this proof wait on itself forever.
    slot_reentrant rather than slot so that a caller already holding the slot
    (a battery row, or a lane holding it across a gate) nests for free instead
    of deadlocking, exactly as battery.py's phase hold does.

    The bound is DECLARED. slot_lock breaks an undeclared hold older than
    MAX_HOLD_SECONDS as stale, and the honest worst case here is the stagger
    plus one full window budget, so it is said out loud rather than left to be
    broken out from under a run that is still going.
    """
    worst = int(3 * max(stagger, 0) + RUN_TIMEOUT + 120)
    print("[slot_lock] vs4_proof is taking the machine-wide windowed test slot "
          "for four windows (declared hold %ds). Another lane's battery makes "
          "this wait; waiting is not being stuck." % worst, flush=True)
    with slot_lock.slot_reentrant(
            label="vs4_proof four windows map=%s frames=%s" % (vsmap, frames),
            max_hold=worst) as held:
        print("[slot_lock] held (%s); launching four windows" % held,
              flush=True)
        try:
            return _launch(out, frames, vsmap, base, star_target, stagger)
        finally:
            print("[slot_lock] releasing the windowed test slot", flush=True)


def _launch(out, frames, vsmap, base, star_target, stagger):
    """The four instances themselves. Only ever called with the slot held."""
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
        rc = M.finish(procs[k], RUN_TIMEOUT)
        res.append((rc, M.text(logs[k]), logs[k]))
    return res


def dh_rounds(t):
    """-> {frame: rounds}, LAST emission wins -- exactly how dhdiff.py loads it.

    Under rollback a frame appears more than once; the last appearance is the
    one simulated on the real remote input, so it is the one that means "the
    state of frame N".
    """
    out = {}
    for m in DH.finditer(t):
        out[int(m.group(1))] = int(m.group(2))
    return out


def dh_counts(t):
    """-> {frame: how many [dh] rows this window logged for it}.

    A frame logged more than once was re-simulated after a rewind -- a
    rewind only ever revisits a frame to correct it, never for any other
    reason -- so a count over 1 is this window's own proof that it is done
    changing its mind about that frame. A count of exactly 1 proves nothing
    either way from inside one window's log alone: most such frames are
    genuinely right the first time, but run 5 of RBFIX's five-run vs4 batch
    (out/RBFIX/vs4_run5_f399_dh_rows.txt) is the counterexample -- window p3
    logged f399 exactly once (w=67b52037, the predicted value) and then
    moved straight on to f400, also logged once, before its own process
    exited on the match's close. p0/p1/p2 each re-simulated f399 (two or
    three rows apiece) and settled on w=af782037; p3's one-shot row never
    got the correcting rewind, because that correction needed a network
    round p3's own session ended before receiving. See settled_frame().
    """
    out = {}
    for m in DH.finditer(t):
        f = int(m.group(1))
        out[f] = out.get(f, 0) + 1
    return out


def resim_report(t):
    """-> (distinct frames, frames simulated more than once, worst count)."""
    seen = {}
    for m in DH.finditer(t):
        f = int(m.group(1))
        seen[f] = seen.get(f, 0) + 1
    if not seen:
        return 0, 0, 0
    return (len(seen), sum(1 for v in seen.values() if v > 1), max(seen.values()))


def proven_bounds(all_rounds, all_counts):
    """-> (tails, raw_excluded, proven, bound): the ceiling on a settled frame.

    TWO INDEPENDENT EXCLUSIONS, applied per window, and the ceiling is the
    TIGHTER of the two. They were both here once; run link100's VS4TAIL added
    the second and, in doing so, dropped the first. Andrew's third review of PR
    #2474 caught that: four logs that all end at f400 with that final frame
    repeated made the function return 400, because a repeat of the raw last
    frame satisfied the re-simulation rule on its own. Both rules are needed,
    and this is the reconciliation.

      * THE RAW LAST FRAME IS NEVER COMPARED -- `tails[k] - 1`, unconditional.
        A run ends when the selftest budget expires, or when the match ends and
        SM64DS_VS_EXIT_ON_END closes the window, and a window can stop
        part-way through a frame it has already logged. Its last frame's state
        is whatever that window happened to reach when it was closed. A REPEAT
        of that frame does not rescue it: a repeat records that the window
        re-simulated the frame, which is a statement about the inputs it had
        when it re-ran, not proof that no further correction was owed. The
        frame is still the one the process died on. So the raw tail is dropped
        whether or not it repeats.
      * THE LAST PROVEN RE-SIMULATED FRAME (VS4TAIL, RBFIX's run 5 finding) --
        the last frame number appearing more than once in that window's own
        [dh] rows (see dh_counts()). A rewind only ever revisits a frame to
        correct it, so a count over 1 is that window's own proof it is done
        changing its mind. Run 5 is why this is needed on top of the raw-tail
        rule: all four windows shared the identical raw last frame f400, so
        "not the raw last frame" let f399 through, but p3's f399 was logged
        exactly once (out/RBFIX/vs4_run5_f399_dh_rows.txt) -- a predicted value
        p3 never got the chance to correct before its own process exited, even
        though it went on to log f400 straight afterward. Reaching a LATER
        frame is not proof an EARLIER one was corrected; only a repeat of that
        SAME frame is. A window with no re-simulated frame anywhere in its log
        (a clean lockstep run, or a very short one) has no such proof to offer
        and is bounded by the raw-tail rule alone.

    So: `min(last re-simulated, raw last - 1)`, and with no re-simulation at
    all, `raw last - 1`. On Andrew's case -- four logs ending at 400 with the
    final frame repeated -- that is 399, and on the no-repeat control it is
    399 as well, because the raw last frame is excluded either way.
    """
    tails = [max(r) for r in all_rounds]
    raw_excluded = [t - 1 for t in tails]
    proven = []
    for k, counts in enumerate(all_counts):
        resimmed = [f for f, c in counts.items() if c > 1]
        proven.append(min(max(resimmed), raw_excluded[k]) if resimmed
                      else raw_excluded[k])
    return tails, raw_excluded, proven, min(proven)


def settled_frame(all_rounds, all_counts):
    """The latest frame every window has PROVEN settled and agrees the round of.

    Three conditions, and all three are needed. The first two are per window
    and are proven_bounds()' business -- the raw last frame is excluded
    unconditionally, and the candidate is at or before the last frame the
    window has PROOF of having re-simulated. The third is across windows:

      * all four report the same rounds= at that frame. This is dhdiff.py's
        own alignment criterion, applied here so rungs 3-6 compare the same
        moment rather than trusting a position tolerance to absorb a whole
        consumed round. It is NOT sufficient by itself -- RBFIX's run 5 shows
        why: rounds= is the exchanged-round counter, which plateaus once no
        more packets are moving, so p0's corrected f399 (three rows, last one
        rounds=400) and p3's uncorrected f399 (one row, rounds=400) report
        the identical rounds= number despite different world hashes. The
        per-window bounds above are what actually screen that pair out; this
        agreement check stays on top of them as belt and braces.

    Returns None when no such frame exists, which is itself a finding: the four
    windows never agreed on a round at or before every window's proven-settled
    point, and the caller says so rather than comparing anyway.
    """
    if any(not r for r in all_rounds):
        return None
    _, _, _, bound = proven_bounds(all_rounds, all_counts)
    cand = set(all_rounds[0])
    for r in all_rounds[1:]:
        cand &= set(r)
    cand = [f for f in cand if f <= bound]
    agree = [f for f in cand
             if len(set(r[f] for r in all_rounds)) == 1]
    return max(agree) if agree else None


def trim_log(src, dst, last_frame):
    """Copy `src` to `dst`, dropping detector lines after `last_frame`.

    Only the [dh]/[dh=]/[dh+] family is trimmed, and only past the settled
    frame: the unsettled tail is removed, nothing inside the compared span is
    touched, and every other line is copied through so the trimmed file is
    still a readable log. dhdiff then compares a span both windows finished.

    THE EXCLUDED TAIL IS KEPT, not just dropped. Every [dh] row past
    `last_frame` is written to `<dst>.tail` beside the trimmed copy and
    returned, so the rows this proof declined to compare can be read next to
    the ones it did. The raw log is untouched either way; this file is the
    short answer to "what exactly was left out".

    -> (kept rows, list of (frame, line) excluded, path of the tail file)
    """
    kept = 0
    excluded = []
    tail_path = dst + ".tail"
    with open(src, "r", encoding="utf-8", errors="replace") as fi, \
            open(dst, "w", encoding="utf-8", errors="replace") as fo:
        for line in fi:
            m = DHLINE.match(line)
            if m and int(m.group(1)) > last_frame:
                excluded.append((int(m.group(1)), line.rstrip("\n")))
                continue
            if m:
                kept += 1
            fo.write(line)
    with open(tail_path, "w", encoding="utf-8", errors="replace") as ft:
        ft.write("# rows EXCLUDED from the comparison of %s\n" % src)
        ft.write("# every [dh] row after frame %d, which is the settled "
                 "frame every window proved\n" % last_frame)
        for _, line in excluded:
            ft.write(line + "\n")
    return kept, excluded, tail_path


def common_frame(all_rows, want_slots=4, at_most=None):
    """The latest frame every window probed with all `want_slots` seated."""
    shared = None
    for r in all_rows:
        have = set(f for f, d in r.items()
                   if len(d) >= want_slots
                   and all(d.get(s) for s in range(want_slots)))
        shared = have if shared is None else (shared & have)
    if not shared:
        return None
    if at_most is not None:
        shared = set(f for f in shared if f <= at_most)
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

    # THE ROLLBACK READING, stated before any rung uses it. These counts are
    # not a warning -- they are how a VS session runs here -- but they are what
    # makes "frame N" ambiguous, so the numbers go in the log.
    all_rounds = [dh_rounds(t) for t in texts]
    all_counts = [dh_counts(t) for t in texts]
    for k, t in enumerate(texts):
        n, again, worst = resim_report(t)
        print("  [rollback] window %d: %d frames, %d re-simulated after a "
              "rewind (worst %d simulations of one frame)"
              % (k, n, again, worst))
    settled = settled_frame(all_rounds, all_counts)
    if settled is None:
        print("  [rollback] NO SETTLED FRAME: the four windows never agreed on "
              "a round number for a frame they had all finished. Rungs 3-8 run "
              "on the raw tail, and a red below may be that.")
    else:
        tails, raw_excluded, proven, bound = proven_bounds(all_rounds,
                                                           all_counts)
        resim_last = []
        for counts in all_counts:
            r = [f for f, c in counts.items() if c > 1]
            resim_last.append(max(r) if r else None)
        print("  [rollback] settled frame %d. THE COMPARED PREFIX IS FRAMES "
              "0..%d INCLUSIVE (%d frames); every [dh] row after f%d is "
              "excluded from every rung."
              % (settled, settled, settled + 1, settled))
        print("  [rollback] how that ceiling was reached, per window: raw "
              "last frames %s, so the raw-tail exclusion alone allows %s "
              "(each window's own last frame is dropped whether or not it "
              "repeats); last PROVEN re-simulated frames %s; per-window "
              "ceiling min(the two) %s -> %d. The settled frame is the "
              "latest frame at or below that ceiling whose rounds= all four "
              "windows agree on. Rungs 3-6 compare at it, rung 8 compares up "
              "to it."
              % (tails, raw_excluded, resim_last, proven, bound))

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

    # ---- rung 2b: the wait ACTUALLY waited ---------------------------------
    # AND THIS RUNG EXISTS BECAUSE RUNG 1 CANNOT SEE THE FAILURE IT IS NAMED
    # AFTER. SM64DS_VS_PLAYERS forces the count, so a console whose wait EXPIRED
    # still seats four actors and still reports count=4 -- rung 1 passes while
    # the very mechanism under test did nothing. Found by running this proof at
    # a three-second stagger and reading the parent's log after it had already
    # said ALL GREEN: "the session reached 3 of the 4 players it was told to
    # expect", one line above slot 3 joining.
    #
    # The shortfall line is the conductor's own, so this reads the mechanism
    # rather than its side effect.
    short = [k for k, t in enumerate(texts) if "of the 4 players it was told "
             "to expect" in t or "players it was told to expect" in t]
    ok &= M.verdict(not short,
                    "rung2b no console's seat TIMED OUT waiting for the roster "
                    "| %s. A timeout here still passes rung 1, because the "
                    "forced count seats four actors either way -- which is "
                    "exactly why this rung is separate."
                    % ("none did" if not short
                       else "windows %s gave up short" % short))

    # ---- rungs 3/4/5: bodies, colours, pads --------------------------------
    cf = common_frame(all_rows, at_most=settled)
    if cf is None:
        M.verdict(False, "no frame has all four slots seated in all four "
                         "windows at or before the settled frame; rungs 3-6 "
                         "cannot run")
        print("\n".join(M.VERDICTS))
        return 1
    print("  (comparing at frame %d, the latest SETTLED frame all four "
          "windows share)" % cf)

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
    # THE TRIMMED COPIES, and the raw logs are untouched beside them. Trimming
    # the unsettled tail is what stops dhdiff's alignment gate refusing the
    # whole run over the one frame each window stopped part-way through; it
    # removes nothing inside the span being compared. `settled` is the SAME
    # proven-resimulated bound rungs 3-6 just compared at (settled_frame()'s
    # per-window re-simulation-count bound, not just "not any window's raw
    # last frame") -- rung 8 must not get a longer, less-trusted tail than
    # rungs 3-6 did, or it would refuse (or worse, silently pass) on exactly
    # the unsettled frame those rungs were careful to avoid.
    use = list(logs)
    if settled is not None:
        use = []
        for k, lg in enumerate(logs):
            dst = os.path.join(os.path.dirname(lg), "run.settled.log")
            kept, excluded, tail_path = trim_log(lg, dst, settled)
            use.append(dst)
            if excluded:
                fr = sorted(set(f for f, _ in excluded))
                print("  [rollback] window %d: %d row(s) compared up to f%d, "
                      "%d row(s) EXCLUDED for frames f%d..f%d, kept in %s"
                      % (k, kept, settled, len(excluded), fr[0], fr[-1],
                         tail_path))
                for _, line in excluded[-4:]:
                    print("      excluded: %s" % line.strip()[:150])
            else:
                print("  [rollback] window %d: %d row(s) compared up to f%d, "
                      "nothing excluded (this window logged no row past the "
                      "settled frame)" % (k, kept, settled))
    for a in range(4):
        for b in range(a + 1, 4):
            r = subprocess.run([sys.executable, dh, use[a], use[b]],
                               capture_output=True, text=True)
            tail = (r.stdout or r.stderr).strip().splitlines()
            # rc=2 IS NOT rc=1. Two windows that could not be compared have not
            # been shown to disagree, and reporting a refusal in the same words
            # as a divergence is how an infra condition gets read as a desync.
            what = {0: "AGREE", 1: "DIVERGED", 2: "REFUSED (nothing compared)"}
            ok &= M.verdict(r.returncode == 0,
                            "rung8 p%d vs p%d agree on every settled frame | "
                            "%s, dhdiff rc=%d, raw logs %s %s | %s"
                            % (a, b, what.get(r.returncode, "rc"),
                               r.returncode, logs[a], logs[b],
                               tail[-1] if tail else ""))
            if r.returncode != 0:
                print("\n".join("      " + x for x in tail[-12:]))

    print("\nVERDICT:", "ALL GREEN" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
