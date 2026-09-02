"""KING OF THE STAR functional + determinism proof.

Four windows, one loopback VS session, win_mode = king (SM64DS_VS_KING_TARGET).
Quiet and muted through mp2_proof.env_base. The fixture is the same
deterministic one vs4_proof uses -- SM64DS_VS_BREAKALL wakes the arena stars and
SM64DS_STAR_TRIGGER arms a collect on named frames, both lockstep-safe local
mutations -- so a holder emerges without a human at the pad.

It proves, from the flight recorder every window writes:

  ONE STAR      the [king] probe's `stars=` never exceeds 1 in any window, ever.
  ACCRUAL       the holder's points climb over time (points sum rises).
  TRANSFER      the star is dropped and re-held (a holder change with stars back
                on the field), the SAME one star, not a sixth.
  WIN           MATCH OVER fires with win=king-target and a winner.
  DETERMINISM   dhdiff over all six window pairings agrees frame for frame, and
                every window's [king] line agrees on holder+points at each frame.

    python port/tools/king_proof.py [--frames N] [--map 0..3] [--target K]
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
OUT = os.path.join(ROOT, "build", "king_proof")

KEYS = (0x0040, 0x0080, 0x0020, 0x0010)
BREAK_AT = "120"                    # king-aware: wakes ONLY the active marker
COLLECT_AT = "200,400"             # collect, then re-collect after the drop
DROP_AT = "320"                    # ROM hurt->drop on the holder, mid-match


def port_base():
    return 43000 + (os.getpid() % 900) * 8


KING = re.compile(r"^\[king\] f(\d+) stars=(\d+) holder=(-?\d+) "
                  r"points=(-?\d+),(-?\d+),(-?\d+),(-?\d+)", re.M)
MATCHOVER = re.compile(
    r"^\[vs\] MATCH OVER f(\d+) win=(\S+) scores=(-?\d+),(-?\d+),(-?\d+),(-?\d+)"
    r".*?winner=(-?\d+)", re.M)
ARM = re.compile(r"^\[vs\] KING OF THE STAR: .*first to (\d+) wins", re.M)


def king_rows(t):
    """-> {frame: (stars, holder, (p0,p1,p2,p3))}"""
    out = {}
    for m in KING.finditer(t):
        f = int(m.group(1))
        out[f] = (int(m.group(2)), int(m.group(3)),
                  tuple(int(m.group(i)) for i in range(4, 8)))
    return out


def launch(out, frames, vsmap, base, target, stagger):
    procs, dirs, logs = [], [], []
    for k in range(4):
        d = os.path.join(out, "p%d" % k)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        dirs.append(d)
        logs.append(os.path.join(d, "run.log"))
    for k in range(4):
        e = M.env_base(ROOT, dirs[k], "p%d" % k)
        e.pop("SM64DS_LEVEL", None)
        e["SM64DS_VS_MAP"] = str(vsmap)
        e["SM64DS_WINDOW_SELFTEST"] = str(frames)
        e["SM64DS_VS_PROBE"] = "1"
        e["SM64DS_VS_PLAYERS"] = "4"
        e["SM64DS_COMMS_ROLE"] = "parent" if k == 0 else "child"
        e["SM64DS_COMMS_PORT"] = str(base)
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_VS_STATE_HASH"] = "2"
        e["SM64DS_COMMS_INJECT"] = "key=0x%04x" % KEYS[k]
        if k:
            e["SM64DS_COMMS_SLOT"] = str(k)
        # KING MODE, and the deterministic collect fixture that drives it.
        e["SM64DS_VS_KING_TARGET"] = str(target)
        e["SM64DS_VS_KING_PROBE"] = "1"
        e["SM64DS_VS_BREAKALL"] = BREAK_AT
        e["SM64DS_STAR_TRIGGER"] = COLLECT_AT
        e["SM64DS_VS_KING_DROP"] = DROP_AT
        e["SM64DS_VS_END_GRACE"] = "120"
        e["SM64DS_VS_EXIT_ON_END"] = "1"
        procs.append(M.spawn(EXE, dirs[k], e, logs[k]))
        if k < 3:
            time.sleep(stagger)
    res = []
    for k in range(4):
        rc = M.finish(procs[k], 900)
        res.append((rc, M.text(logs[k]), logs[k]))
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", default="1600")
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--target", type=int, default=3,
                    help="first-to-N points; small so a selftest reaches it")
    ap.add_argument("--stagger", type=float, default=1.0)
    ap.add_argument("--port", type=int, default=0)
    args = ap.parse_args()

    base = args.port or port_base()
    print("king_proof: four windows, map %d, %s frames, target %d, port base %d"
          % (args.map, args.frames, args.target, base), flush=True)
    res = launch(OUT, args.frames, args.map, base, args.target, args.stagger)
    texts = [t for _, t, _ in res]
    logs = [p for _, _, p in res]
    krows = [king_rows(t) for t in texts]
    ok = True

    for k, (rc, _, lg) in enumerate(res):
        ok &= M.verdict(rc == 0, "window %d exited clean | rc=%d %s"
                        % (k, rc, lg))

    # ---- the mode actually armed, with the target the lobby would export ----
    arms = [ARM.search(t) for t in texts]
    ok &= M.verdict(all(arms) and all(int(m.group(1)) == args.target
                                      for m in arms if m),
                    "king mode armed in all four windows at target %d | %s"
                    % (args.target,
                       [int(m.group(1)) if m else -1 for m in arms]))

    # ---- ONE STAR: stars never exceeds 1, in any window, on any frame -------
    max_stars = []
    for k, r in enumerate(krows):
        ms = max((v[0] for v in r.values()), default=0)
        max_stars.append(ms)
    ok &= M.verdict(all(ms <= 1 for ms in max_stars),
                    "ONE STAR: no window ever saw a second star | "
                    "max stars per window = %s (a 2 here is the invariant broken)"
                    % (max_stars,))
    saw_a_star = [any(v[0] == 1 for v in r.values()) for r in krows]
    ok &= M.verdict(all(saw_a_star),
                    "the one star was actually held at some point in every "
                    "window | %s" % (saw_a_star,))

    # ---- ACCRUAL: the holder's points climb over the match ------------------
    finals = []
    for r in krows:
        if r:
            finals.append(r[max(r)][2])
        else:
            finals.append((0, 0, 0, 0))
    peak = [max(f) for f in finals]
    ok &= M.verdict(all(p >= 1 for p in peak),
                    "ACCRUAL: a holder banked points in every window | "
                    "peak points per window = %s" % (peak,))

    # ---- TRANSFER: a holder change while the star is on the field/re-held ---
    transferred = []
    for r in krows:
        holders = [v[1] for f, v in sorted(r.items())]
        distinct = sorted(set(h for h in holders if h >= 0))
        # either two different slots held it, or it left a hand (holder -> -1)
        # and came back -- both mean the SAME one star moved.
        changes = sum(1 for a, b in zip(holders, holders[1:]) if a != b)
        transferred.append(len(distinct) >= 2 or changes >= 2)
    ok &= M.verdict(all(transferred),
                    "TRANSFER: the one star changed hands (dropped/re-held) in "
                    "every window | %s" % (transferred,))

    # ---- WIN: MATCH OVER, win=king-target, a winner slot -------------------
    ends = [MATCHOVER.search(t) for t in texts]
    got = sum(1 for m in ends if m)
    ok &= M.verdict(got == 4,
                    "WIN: MATCH OVER in all four windows | %d of 4" % got)
    if got == 4:
        wins = [m.group(2) for m in ends]
        winners = [int(m.group(7)) for m in ends]
        boards = [tuple(int(m.group(i)) for i in range(3, 7)) for m in ends]
        ok &= M.verdict(all(w == "king-target" for w in wins),
                        "WIN: the match ended on the KING target | win=%s" % wins)
        ok &= M.verdict(len(set(winners)) == 1 and winners[0] >= 0,
                        "WIN: all four windows name the SAME winner | %s"
                        % (winners,))
        ok &= M.verdict(len(set(boards)) == 1,
                        "WIN: the four points boards agree across windows | %s"
                        % (boards,))
        if len(set(winners)) == 1:
            w = winners[0]
            ok &= M.verdict(boards[0][w] >= args.target,
                            "WIN: the winner reached the target | P%d has %d, "
                            "target %d" % (w, boards[0][w], args.target))

    # ---- DETERMINISM: every window agrees on holder+points each frame -------
    common = None
    for r in krows:
        common = set(r) if common is None else (common & set(r))
    common = sorted(common or [])
    disagree = 0
    for f in common:
        vals = set(krows[k][f] for k in range(4))
        if len(vals) != 1:
            disagree += 1
    ok &= M.verdict(disagree == 0,
                    "DETERMINISM: all four windows agree on (stars,holder,points) "
                    "on every shared frame | %d shared frames, %d disagreements"
                    % (len(common), disagree))

    # ---- DETERMINISM: the actor digest, all six pairings (rung 8) ----------
    dh = os.path.join(os.path.dirname(os.path.abspath(__file__)), "dhdiff.py")
    for a in range(4):
        for b in range(a + 1, 4):
            r = subprocess.run([sys.executable, dh, logs[a], logs[b]],
                               capture_output=True, text=True)
            tail = (r.stdout or r.stderr).strip().splitlines()
            ok &= M.verdict(r.returncode == 0,
                            "DETERMINISM digest p%d vs p%d | rc=%d %s"
                            % (a, b, r.returncode, tail[-1] if tail else ""))
            if r.returncode != 0:
                print("\n".join("      " + x for x in tail[-8:]))

    print("\nVERDICT:", "ALL GREEN" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
