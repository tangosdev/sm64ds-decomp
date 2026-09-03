#!/usr/bin/env python3
"""Luigi Infection game-side proof (single-process VS arena).

Launches one walk_window in a 4-player VS arena with the quiet-spawn discipline
(minimized, unfocused, muted) and asserts the mode's game side from the flight
recorder:

  SEED   after frame 90 exactly one slot (the seed) is character 1 (Luigi) and
         every other slot keeps its pre-seed character.
  HIDE   every Luigi-team slot's minimap mapID is -1 (the Render == test can
         never match it) while every survivor keeps a live mapID.
  LIVE   the seeded slot keeps producing probe rows after the swap (it ticks).
  INERT  with no SM64DS_VS_LUIGI_INFECTION there is no [luigi] output at all and
         every slot stays its default character.

Run args pick which case. See --help.
"""
import argparse, os, re, sys

ROOT = r"C:\tmp\sm64ds-luigiinf"
sys.path.insert(0, os.path.join(ROOT, "port", "tools"))
import mp2_proof as M

EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")

VS = re.compile(r"^\[vs\] f(\d+) slot(\d+) actor=([0-9A-Fa-fx]+) no=(\d+) "
                r"char=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)", re.M)
MM = re.compile(r"^\[luigi\] MINIMAP curmap=(-?\d+)(.*)$", re.M)
MMSLOT = re.compile(r"s(\d+)\[(LUIGI|surv) mapID=(-?\d+)\]")
SEED = re.compile(r"\[luigi\] SEED f(\d+): slot (\d+) starts as Luigi")


def run(outdir, frames, extra_env):
    os.makedirs(os.path.join(outdir, "tmp"), exist_ok=True)
    e = M.env_base(ROOT, outdir, os.path.basename(outdir))
    e.pop("SM64DS_LEVEL", None)
    e["SM64DS_VS_MAP"] = "0"
    e["SM64DS_VS_PLAYERS"] = "4"
    e["SM64DS_VS_PROBE"] = "1"
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_FAULTS_FATAL"] = "1"
    e.update(extra_env)
    log = os.path.join(outdir, "run.log")
    p = M.spawn(EXE, outdir, e, log)
    rc = M.finish(p, 600)
    return rc, M.text(log)


def rows(t):
    out = {}
    for m in VS.finditer(t):
        f = int(m.group(1)); s = int(m.group(2))
        out.setdefault(s, []).append(
            dict(f=f, char=int(m.group(5)),
                 pos=(int(m.group(6)), int(m.group(7)), int(m.group(8)))))
    return out


def char_at(rws, slot, after):
    """the character reported for a slot on the last frame >= after."""
    seq = [r for r in rws.get(slot, []) if r["f"] >= after]
    return seq[-1]["char"] if seq else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--case", required=True,
                    choices=["seed", "inert", "timer"])
    ap.add_argument("--seed-slot", type=int, default=2)
    ap.add_argument("--frames", type=int, default=200)
    ap.add_argument("--out", default=None)
    args = ap.parse_args()

    outdir = args.out or os.path.join(r"C:\tmp\luigiinf-out", "case_" + args.case)
    fails = []

    if args.case == "seed":
        rc, t = run(outdir, args.frames,
                    {"SM64DS_VS_LUIGI_INFECTION": "1",
                     "SM64DS_VS_LUIGI_SEED": str(args.seed_slot),
                     "SM64DS_VS_LUIGI_PROBE": "1"})
        print("rc=%d" % rc)
        ms = SEED.search(t)
        if not ms:
            fails.append("no SEED line")
            seedslot = args.seed_slot
        else:
            seedslot = int(ms.group(2))
            print("SEED line: slot %d at f%s" % (seedslot, ms.group(1)))
        rws = rows(t)
        # after the seed, the tagger is char 1, others are not
        for s in sorted(rws):
            c = char_at(rws, s, 100)
            tag = "LUIGI" if s == seedslot else "surv"
            print("  slot %d char@post=%s (%s)" % (s, c, tag))
            if s == seedslot and c != 1:
                fails.append("seed slot %d char=%s, expected 1" % (s, c))
            if s != seedslot and c == 1:
                fails.append("survivor slot %d became Luigi (char 1)" % s)
        # liveness: seed slot still ticks past the swap
        late = [r for r in rws.get(seedslot, []) if r["f"] >= 150]
        if not late:
            fails.append("seed slot %d produced no rows after f150 (dead)" % seedslot)
        else:
            print("  seed slot live: %d rows after f150, last pos=%s"
                  % (len(late), late[-1]["pos"]))
        # minimap hide: last snapshot, luigi -1, survivors live
        snaps = list(MM.finditer(t))
        if not snaps:
            fails.append("no [luigi] MINIMAP snapshot")
        else:
            curmap, tail = int(snaps[-1].group(1)), snaps[-1].group(2)
            print("  last MINIMAP curmap=%d%s" % (curmap, tail))
            for sm in MMSLOT.finditer(tail):
                s, kind, mid = int(sm.group(1)), sm.group(2), int(sm.group(3))
                if kind == "LUIGI" and mid != -1:
                    fails.append("LUIGI slot %d mapID=%d, expected -1" % (s, mid))
                if kind == "surv" and mid < 0:
                    fails.append("survivor slot %d mapID=%d (hidden!)" % (s, mid))

    elif args.case == "inert":
        rc, t = run(outdir, args.frames, {})
        print("rc=%d" % rc)
        if "[luigi]" in t:
            fails.append("[luigi] output present with mode OFF")
        rws = rows(t)
        for s in sorted(rws):
            c = char_at(rws, s, 100)
            print("  slot %d char@post=%s" % (s, c))
            if c == 1:
                fails.append("slot %d is Luigi (char 1) with mode OFF" % s)

    elif args.case == "timer":
        # survivors win when the clock runs out with a survivor alive
        rc, t = run(outdir, max(args.frames, 260),
                    {"SM64DS_VS_LUIGI_INFECTION": "1",
                     "SM64DS_VS_LUIGI_SEED": str(args.seed_slot),
                     "SM64DS_VS_LUIGI_TIME": "1",
                     "SM64DS_VS_END_SCENE": "0", "SM64DS_VS_EXIT_ON_END": "0",
                     "SM64DS_VS_END_GRACE": "0"})
        print("rc=%d" % rc)
        mk = re.search(r"\[vs\] MATCH OVER f\d+ win=(\S+)", t)
        li = re.search(r"\[vs\] f\d+ LUIGI INFECTION OVER: (.+)", t)
        if li:
            print("  end: %s" % li.group(1))
        if mk:
            print("  marker win=%s" % mk.group(1))
        if not mk or mk.group(1) != "survivor-timeout":
            fails.append("expected win=survivor-timeout, got %s"
                         % (mk.group(1) if mk else None))

    print("\nRESULT: %s" % ("PASS" if not fails else "FAIL"))
    for f in fails:
        print("  - " + f)
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
