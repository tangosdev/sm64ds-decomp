#!/usr/bin/env python3
"""Measure the OPENING CUTSCENE's note loss, reproducibly.

    python port/tools/opening_audio_measure.py --tag after
    python port/tools/opening_audio_measure.py --tag after --frames 4000

Prints, for the whole run and per (player, track): note-ons that got a mixer
channel, note-ons that got nothing, and the longest run of consecutive drops
on a track (the thing that decides whether a musical part goes SILENT for a
stretch or merely THINS OUT).

WHY THIS SCRIPT EXISTS INSTEAD OF A COMMAND LINE IN A COMMIT MESSAGE. An
earlier version of this measurement was recorded as a bare list of environment
variables and could not be reproduced by anyone else: it silently depended on
the worktree's leftover save file. Two things have to be true or the opening
never plays and the run measures the title screen instead:

  1. THE SAVE MUST BE ABSENT. The opening cutscene only plays when the
     file-select menu is driven into a NEW FILE. cons carries file-backed save
     persistence, and the battery writes a save, so a worktree that has been
     tested in has one. This script points SM64DS_SAVE_PATH at a per-run file
     and deletes it first, so the new-file path is taken every time.
  2. THE TOUCH PROBE MUST HIT THE NEW-FILE SLOT, at (44,68). The coordinates
     come from port/tools/opening_cast_capture.ps1, which is the merged
     opening-cast lane's proven path into this same cutscene.

Determinism: SM64DS_NO_AUDIO=1 means no output device, so the mixer is clocked
at exactly one 60th of a second per video frame instead of by a sound card's
real-time drain. The note counts then repeat exactly run to run.

Verify the opening really ran: the log must contain "a cutscene script is
running". This script checks that and refuses to report numbers if it is
missing, which is exactly the failure an earlier measurement did not catch.
"""
import argparse, collections, os, re, shutil, subprocess, sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(os.path.dirname(__file__) + "/../")))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tag", required=True)
    ap.add_argument("--frames", type=int, default=4000)
    ap.add_argument("--repo", default=None)
    ap.add_argument("--outroot", default=os.environ.get("TEMP", "/tmp") + "/opening_audio")
    a = ap.parse_args()

    repo = a.repo or os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    exe  = os.path.join(repo, "build", "port", "walk_window.exe")
    if not os.path.exists(exe):
        sys.exit("walk_window.exe not found: %s" % exe)

    out = os.path.join(a.outroot, a.tag)
    shutil.rmtree(out, ignore_errors=True)
    os.makedirs(out)

    e = dict(os.environ)
    e.pop("SM64DS_NO_FOCUS", None)          # the proven opening path does not set it
    e.update({
        "SM64DS_TEST_LOCK":         "1",
        "SM64DS_TEST_LOCK_PATH":    r"C:\tmp\sm64ds-test-slot\windowed_test.lock",
        "SM64DS_TEST_LOCK_TIMEOUT": "5400",
        "SM64DS_ASSET_ROOT":        repo,
        "SM64DS_SAVE_PATH":         os.path.join(out, "sm64ds.sav"),   # deleted with out/
        "SM64DS_NO_AUDIO":          "1",     # deterministic 1/60 s mixer clock
        "SM64DS_SKIP_MENU":         "1",
        "SM64DS_SCENE_WINDOW":      "1",
        "SM64DS_SCENE_FRAMES":      "100000",
        "SM64DS_TOUCH_PROBE":       "560-563:44:68,620-623:44:68,680-683:44:68,"
                                    "760-763:44:68,860-863:44:68",
        "SM64DS_NO_PLAYLOG":        "1",
        "SM64DS_PITCH_DUMP":        os.path.join(out, "pitch.log"),
        "SM64DS_WINDOW_SELFTEST":   str(a.frames),
    })
    log = os.path.join(out, "run.log")
    with open(log, "wb") as f:
        rc = subprocess.call([exe], stdout=f, stderr=subprocess.STDOUT, env=e, cwd=repo)
    text = open(log, encoding="utf-8", errors="replace").read()
    if "a cutscene script is running" not in text:
        sys.exit("THE OPENING DID NOT PLAY -- this run measured something else.\n"
                 "Check the touch probe reached the new-file slot and that\n"
                 "%s was absent. Numbers withheld." % e["SM64DS_SAVE_PATH"])
    report(os.path.join(out, "pitch.log"), a.tag, rc)

def report(path, tag, rc):
    rx = re.compile(r"^\[pd\] (on|off) f=(\d+) p=(\d+) t=(\d+) ch=(-?\d+).*?key=(\d+)")
    on, off = collections.defaultdict(list), collections.defaultdict(list)
    for line in open(path, encoding="utf-8", errors="replace"):
        m = rx.match(line)
        if not m:
            continue
        (on if m.group(1) == "on" else off)[(int(m.group(3)), int(m.group(4)))].append(
            (int(m.group(2)), int(m.group(6))))
    k_on  = sum(len(v) for v in on.values())
    k_off = sum(len(v) for v in off.values())
    tot = k_on + k_off
    print("%s (walk_window rc=%d): %d note-ons requested, %d sounded, %d dropped (%.1f%%)"
          % (tag, rc, tot, k_on, k_off, 100.0 * k_off / tot if tot else 0.0))
    print("  player/track   sounded  dropped   drop%   longest consecutive drop run")
    for k in sorted(set(on) | set(off), key=lambda k: -len(off.get(k, []))):
        d = off.get(k, [])
        if not d:
            continue
        o = on.get(k, [])
        ev = sorted([(f, 1) for f, _ in o] + [(f, 0) for f, _ in d])
        run = best = 0
        for _, is_on in ev:
            run = 0 if is_on else run + 1
            best = max(best, run)
        print("  %6s        %7d  %7d  %5.1f%%   %d"
              % ("%d/%d" % k, len(o), len(d), 100.0 * len(d) / (len(o) + len(d)), best))
    dead = [k for k in off if off[k] and not on.get(k)]
    print("  tracks that lost EVERY note (went fully silent): %s"
          % (", ".join("%d/%d" % k for k in dead) if dead else "NONE"))

if __name__ == "__main__":
    main()
