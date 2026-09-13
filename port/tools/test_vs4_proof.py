#!/usr/bin/env python3
"""Unit tests for the vs4 proof's comparison boundary (port/tools/vs4_proof.py).

These need no game, no ROM and no build: they hand settled_frame() and
proven_bounds() synthetic [dh] logs and check which frame the proof would
compare at. Run with:

    python port/tools/test_vs4_proof.py        (or pytest, where it is installed)

WHAT THEY ARE PINNING. Two exclusions decide the boundary, and the bug they
are here to stop is either one silently going missing:

  * THE RAW LAST FRAME IS NEVER COMPARED, whether or not it repeats. Run
    link100's VS4TAIL added the re-simulation rule below and, in doing so,
    dropped this one, so four logs ending at f400 with the final frame
    repeated compared AT f400 -- the frame each window's process died on.
    A repeat of the last frame records that the window re-simulated it; it
    does not record that the window survived long enough to correct it again.
    Andrew's third review of PR #2474 is the source of the case.
  * THE LAST PROVEN RE-SIMULATED FRAME still bounds the candidate on top of
    that (VS4TAIL, from RBFIX's run 5): a frame logged exactly once by some
    window has no proof from that window that it was ever corrected, even if
    the window went on to log later frames.

The two are independent, so the ceiling is the tighter of them, and the
regression control is that BOTH cases below land on 399: the repeated-tail
log because the raw tail is dropped, and the no-repeat log because it is
dropped there too.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vs4_proof as V


# ---- fixtures ------------------------------------------------------------

def dh(frame, rounds, world="deadbeef"):
    return "[dh] f%d w=%s rounds=%d\n" % (frame, world, rounds)


def log_plain(last, rounds_at=lambda f: f + 1):
    """One [dh] row per frame, 0..last inclusive. No re-simulation anywhere."""
    return "".join(dh(f, rounds_at(f)) for f in range(last + 1))


def log_repeat_tail(last, repeats=2):
    """Same, but the FINAL frame is logged `repeats` times.

    This is Andrew's case: the window rewound into its own last frame and
    re-simulated it, then the process ended. The repeat is real; it is not
    proof the frame was finished with.
    """
    t = log_plain(last)
    return t + "".join(dh(last, last + 1) for _ in range(repeats - 1))


def four(texts):
    return ([V.dh_rounds(t) for t in texts], [V.dh_counts(t) for t in texts])


# ---- tests ---------------------------------------------------------------

def test_repeated_final_frame_is_still_excluded():
    """Four logs ending at 400, final frame repeated -> 399, not 400."""
    rounds, counts = four([log_repeat_tail(400) for _ in range(4)])
    tails, raw_excluded, proven, bound = V.proven_bounds(rounds, counts)
    assert tails == [400, 400, 400, 400], tails
    assert raw_excluded == [399, 399, 399, 399], raw_excluded
    # the re-simulation rule on its own would have said 400 here
    assert proven == [399, 399, 399, 399], proven
    assert bound == 399, bound
    assert V.settled_frame(rounds, counts) == 399


def test_no_repeat_control_is_also_399():
    """The control: no window repeats anything -> 399 as well."""
    rounds, counts = four([log_plain(400) for _ in range(4)])
    tails, raw_excluded, proven, bound = V.proven_bounds(rounds, counts)
    assert tails == [400, 400, 400, 400], tails
    assert proven == [399, 399, 399, 399], proven
    assert bound == 399, bound
    assert V.settled_frame(rounds, counts) == 399


def test_the_two_cases_agree():
    """Stated as its own assertion because it is the review's own sentence."""
    a = V.settled_frame(*four([log_repeat_tail(400) for _ in range(4)]))
    b = V.settled_frame(*four([log_plain(400) for _ in range(4)]))
    assert a == b == 399, (a, b)


def test_resimulation_bound_still_bites():
    """VS4TAIL's rule is not lost: a window whose f399 is a one-shot drops the
    ceiling below 399 even though every window reached f400.

    p0..p2 re-simulate f398 and f399; p3 logs both exactly once. p3 has proof
    of a re-simulation only at f397, so the ceiling is 397.
    """
    good = log_plain(400) + dh(398, 399) + dh(399, 400) + dh(397, 398)
    thin = log_plain(400) + dh(397, 398)
    rounds, counts = four([good, good, good, thin])
    tails, raw_excluded, proven, bound = V.proven_bounds(rounds, counts)
    assert raw_excluded == [399, 399, 399, 399], raw_excluded
    assert proven == [399, 399, 399, 397], proven
    assert bound == 397, bound
    assert V.settled_frame(rounds, counts) == 397


def test_no_resimulation_anywhere_falls_back_to_raw_tail():
    """Windows of different lengths, none of them rewinding: the shortest
    window's raw tail minus one is the ceiling."""
    rounds, counts = four([log_plain(400), log_plain(390),
                           log_plain(400), log_plain(400)])
    tails, raw_excluded, proven, bound = V.proven_bounds(rounds, counts)
    assert tails == [400, 390, 400, 400], tails
    assert proven == [399, 389, 399, 399], proven
    assert bound == 389, bound
    assert V.settled_frame(rounds, counts) == 389


def test_rounds_disagreement_still_screens_a_frame_out():
    """The agreement check stays on top: if one window reports a different
    rounds= at 399, the settled frame backs off to the latest frame all four
    do agree on."""
    same = log_plain(400)
    odd = "".join(dh(f, (f + 1) if f != 399 else 12345) for f in range(401))
    rounds, counts = four([same, same, same, odd])
    assert V.proven_bounds(rounds, counts)[3] == 399
    assert V.settled_frame(rounds, counts) == 398


def test_excluded_tail_is_written_beside_the_trimmed_copy(tmpdir):
    """trim_log keeps what it drops, and says so."""
    src = os.path.join(tmpdir, "run.log")
    dst = os.path.join(tmpdir, "run.settled.log")
    with open(src, "w", encoding="utf-8") as f:
        f.write(log_repeat_tail(400))
        f.write("[vs] f400 count=4 me=0 live=0,1,2,3\n")
    kept, excluded, tail_path = V.trim_log(src, dst, 399)
    assert kept == 400, kept                 # f0..f399, one row each
    assert len(excluded) == 2, excluded      # both f400 rows
    assert all(f == 400 for f, _ in excluded), excluded
    assert os.path.exists(tail_path), tail_path
    body = open(tail_path, encoding="utf-8").read()
    assert body.count("[dh] f400") == 2, body
    # non-[dh] lines are copied through to the trimmed file, not excluded
    assert "[vs] f400 count=4" in open(dst, encoding="utf-8").read()


# ---- runner --------------------------------------------------------------

def _standalone():
    import tempfile
    import traceback
    passed = failed = 0
    for name, fn in sorted(globals().items()):
        if not name.startswith("test_") or not callable(fn):
            continue
        try:
            if fn.__code__.co_argcount:
                with tempfile.TemporaryDirectory() as d:
                    fn(d)
            else:
                fn()
            print("  PASS %s" % name)
            passed += 1
        except Exception:
            print("  FAIL %s" % name)
            traceback.print_exc()
            failed += 1
    print("\n%d passed, %d failed" % (passed, failed))
    return 1 if failed else 0


if __name__ == "__main__":
    try:
        import pytest
    except ImportError:
        sys.exit(_standalone())
    sys.exit(pytest.main([__file__, "-q"]))
