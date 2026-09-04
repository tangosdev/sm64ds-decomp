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

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "port", "tools"))
import mp2_proof as M

EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")

VS = re.compile(r"^\[vs\] f(\d+) slot(\d+) actor=([0-9A-Fa-fx]+) no=(\d+) "
                r"char=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)", re.M)
MM = re.compile(r"^\[luigi\] MINIMAP curmap=(-?\d+)(.*)$", re.M)
MMSLOT = re.compile(r"s(\d+)\[(LUIGI|surv) mapID=(-?\d+)\]")
SEED = re.compile(r"\[luigi\] SEED f(\d+): slot (\d+) starts as Luigi")
CD = re.compile(r"\[luigi\] COUNTDOWN f(\d+) shows (.+)")

# the pre-round START COUNTDOWN drives the ROM's VS 3-2-1 off the host frame
# counter (hal/star_flow.cpp): READY?+3 at f90, +2 at f150, +1 at f195, START
# and the tagger pick at f240 (a full 5s at ~30Hz). Deterministic on every peer.
CD_EXPECT = [(90, "READY? + 3"), (150, "READY? + 2"),
             (195, "READY? + 1"), (240, "START (0)")]
CD_PICK = 240


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
                    choices=["seed", "inert", "timer", "hit", "win", "det"])
    ap.add_argument("--seed-slot", type=int, default=2)
    ap.add_argument("--frames", type=int, default=320)
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
        # THE COUNTDOWN: READY?+3 -> +2 -> +1 -> START on the exact frames, and
        # only THEN the tagger pick -- proves the count renders before the seed.
        cd = {int(m.group(1)): m.group(2).strip() for m in CD.finditer(t)}
        for f, want in CD_EXPECT:
            got = cd.get(f)
            print("  countdown f%d: %s" % (f, got))
            if got != want:
                fails.append("countdown f%d = %r, expected %r" % (f, got, want))
        ms = SEED.search(t)
        if not ms:
            fails.append("no SEED line")
            seedslot, seedframe = args.seed_slot, -1
        else:
            seedslot, seedframe = int(ms.group(2)), int(ms.group(1))
            print("  SEED: slot %d at f%d" % (seedslot, seedframe))
            if seedframe != CD_PICK:
                fails.append("tagger picked at f%d, expected countdown end f%d"
                             % (seedframe, CD_PICK))
        # the pick must not precede the last count: START(0) is drawn no earlier
        # than the pick frame, and every count above 0 renders before it.
        for f in cd:
            if cd[f] == "START (0)" and f < CD_PICK:
                fails.append("START shown at f%d, before the pick f%d" % (f, CD_PICK))
        rws = rows(t)
        # after the seed, the tagger is char 1, others are not
        for s in sorted(rws):
            c = char_at(rws, s, CD_PICK + 10)
            tag = "LUIGI" if s == seedslot else "surv"
            print("  slot %d char@post=%s (%s)" % (s, c, tag))
            if s == seedslot and c != 1:
                fails.append("seed slot %d char=%s, expected 1" % (s, c))
            if s != seedslot and c == 1:
                fails.append("survivor slot %d became Luigi (char 1)" % s)
        # LIVENESS BY MOVEMENT + STATE, not census.
        #
        # (a) THE FREEZE, and that it LIFTS. The native 3-2-1 holds the players
        #     still (func_ov002_020c71e0 freezes while data_0209f2bc != 0). In a
        #     headless arena only the local player is driven, so it is the mover
        #     that shows it: pick the slot with the most distinct positions and
        #     assert it is FROZEN through the countdown window and MOVES once the
        #     count reaches 0 -- the freeze is real and it releases at the pick.
        def distinct(slot, lo, hi):
            return len({r["pos"] for r in rws.get(slot, [])
                        if lo <= r["f"] <= hi})
        mover = max(rws, key=lambda s: distinct(s, 0, 10 ** 9)) if rws else None
        during = distinct(mover, CD_EXPECT[0][0], CD_PICK - 5) if mover is not None else 0
        after = distinct(mover, CD_PICK + 5, 10 ** 9) if mover is not None else 0
        print("  local mover slot %s: %d distinct pos DURING countdown, %d AFTER"
              % (mover, during, after))
        if during > 1:
            fails.append("mover slot %s moved DURING the countdown (freeze failed)"
                         % mover)
        if after <= 1:
            fails.append("mover slot %s never moved after the countdown (freeze "
                         "did not lift)" % mover)
        # (b) THE TAGGER stays LIVE and keeps its Luigi state after the pick: it
        #     keeps producing rows and every one reads char 1 (an idle non-local
        #     slot does not move, so its liveness is the ticking + the state).
        tpost = [r for r in rws.get(seedslot, []) if r["f"] > CD_PICK]
        if not tpost:
            fails.append("tagger slot %d produced no rows after the pick" % seedslot)
        elif any(r["char"] != 1 for r in tpost):
            fails.append("tagger slot %d lost its Luigi character after the pick"
                         % seedslot)
        else:
            print("  tagger slot %d live post-pick: %d rows, all char=1"
                  % (seedslot, len(tpost)))
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

    elif args.case == "det":
        # DETERMINISM: the countdown is a pure function of the host frame and the
        # env SEED, so two instances run byte-identical countdowns and pick the
        # tagger on the identical frame -- the lockstep property a real VS session
        # needs (no local clock, no rand). Run twice with identical env, compare.
        seq = []
        for i in (1, 2):
            _, t = run(outdir + "_%d" % i, args.frames,
                       {"SM64DS_VS_LUIGI_INFECTION": "1",
                        "SM64DS_VS_LUIGI_SEED": str(args.seed_slot)})
            cd = ["f%s:%s" % (m.group(1), m.group(2).strip())
                  for m in CD.finditer(t)]
            sm = SEED.search(t)
            pick = "seed@f%s->slot%s" % (sm.group(1), sm.group(2)) if sm else "NONE"
            seq.append((cd, pick))
            print("  run %d: %s | %s" % (i, " ".join(cd), pick))
        if seq[0] != seq[1]:
            fails.append("two instances diverged (countdown or pick frame differ)")

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
        # survivors win when the clock runs out with a survivor alive. The match
        # clock (SM64DS_VS_LUIGI_TIME=1 -> 60 frames) is measured from the pick
        # at f240, so the timeout fires ~f300 -- AFTER the pre-round countdown.
        rc, t = run(outdir, max(args.frames, 380),
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

    elif args.case == "hit":
        # tag / immunity / survivor-vs-survivor through the real host resolver,
        # mode ON; plus a mode-OFF control that must match the survivor case.
        # the hittest fires at f260, after the countdown seeds the tagger at f240
        rc_on, t_on = run(outdir + "_on", 340,
                          {"SM64DS_VS_LUIGI_INFECTION": "1",
                           "SM64DS_VS_LUIGI_SEED": str(args.seed_slot),
                           "SM64DS_VS_LUIGI_HITTEST": "1"})
        rc_off, t_off = run(outdir + "_off", 340,
                            {"SM64DS_VS_LUIGI_HITTEST": "1"})
        print("rc on=%d off=%d" % (rc_on, rc_off))
        got = {}
        for tag, t in (("on", t_on), ("off", t_off)):
            for m in re.finditer(r"\[luigi\] HITTEST (\w) (\w[\w-]*).*\| (PASS|FAIL)", t):
                key = tag + ":" + m.group(1)
                got[key] = m.group(3)
                print("  %s %s -> %s" % (tag, m.group(1), m.group(3)))
        for k in ("on:C", "on:A", "on:B", "off:C"):
            if got.get(k) != "PASS":
                fails.append("scenario %s = %s (want PASS)" % (k, got.get(k)))
        # the regression equivalence: survivor-vs-survivor line identical on/off
        cs = {tag: re.search(r"HITTEST C SURV-SURV \(mode \w+\): (.*)$", t, re.M)
              for tag, t in (("on", t_on), ("off", t_off))}
        if cs["on"] and cs["off"]:
            norm = lambda s: re.sub(r"mode \w+", "mode", s.group(1))
            a, b = norm(cs["on"]), norm(cs["off"])
            print("  surv-surv on : %s" % a)
            print("  surv-surv off: %s" % b)
            if a != b:
                fails.append("survivor-vs-survivor differs mode on vs off")

    elif args.case == "win":
        # Luigi tags all survivors -> the all-infected LUIGIS-win path fires.
        # TAG-ALL runs at f260, after the countdown seeds the tagger at f240.
        rc, t = run(outdir, 380,
                    {"SM64DS_VS_LUIGI_INFECTION": "1",
                     "SM64DS_VS_LUIGI_SEED": str(args.seed_slot),
                     "SM64DS_VS_LUIGI_TAGALL": "1",
                     "SM64DS_VS_END_SCENE": "0", "SM64DS_VS_EXIT_ON_END": "0",
                     "SM64DS_VS_END_GRACE": "0"})
        print("rc=%d" % rc)
        ta = re.search(r"TAGALL f\d+: survivors_alive now (\d+)", t)
        mk = re.search(r"\[vs\] MATCH OVER f\d+ win=(\S+)", t)
        if ta:
            print("  tagall survivors_alive=%s" % ta.group(1))
        if mk:
            print("  marker win=%s" % mk.group(1))
        if not ta or ta.group(1) != "0":
            fails.append("tagall did not drive survivors to 0")
        if not mk or mk.group(1) != "luigi-all-infected":
            fails.append("expected win=luigi-all-infected, got %s"
                         % (mk.group(1) if mk else None))

    print("\nRESULT: %s" % ("PASS" if not fails else "FAIL"))
    for f in fails:
        print("  - " + f)
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
