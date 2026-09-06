#!/usr/bin/env python3
"""Collide N walk_window selftests on purpose, and say which one broke.

WHY THIS EXISTS. port/tools/slot_lock.py serialises every windowed test on the
box through one machine-wide lock because two walk_window runs at the same time
"collide over that one slot and throw a random rc=1 on a DIFFERENT level or
scene each run". Nobody root-caused the collision; the lock is a workaround, and
it is the reason eight lanes queue behind one another for a fifteen-minute
battery. This script is the instrument that turns that anecdote into a
measurement: it launches N selftests CONCURRENTLY under battery.py's exact quiet
contract, records every child's rc and the tail of its own log, and repeats the
trial K times so a one-in-ten failure shows up as a rate rather than a story.

IT REUSES battery.py's LAUNCH RATHER THAN RE-SPELLING IT. selftest_env(),
scene_env(), NO_CONSOLE and SI_MIN are imported from battery.py, so a child here
is launched with the same environment, the same CREATE_NO_WINDOW and the same
SW_SHOWMINNOACTIVE the battery uses. A harness that spelled its own flags would
be measuring a different program: the whole question is whether the BATTERY's
launches collide, and the only way to keep that honest is to make the launch one
piece of code with one definition.

IT COLLIDES ITS OWN CHILDREN, NEVER SOMEBODY ELSE'S. The instances this script
starts are meant to overlap -- that is the measurement. What must not overlap is
this script and another lane's gate, and the two asks are not in tension:
--hold-slot (the default) takes the ONE machine-wide windowed slot for the whole
trial block and collides inside it. Holding the lock is what guarantees the only
concurrency in the sample is the concurrency this script created; without it a
trial can start in the gap between two of a battery's per-launch acquisitions and
red somebody's merge. --no-hold-slot falls back to waiting for the lockfile to
clear, which is weaker and is there for a box with no other lane on it.

ISOLATION MODES, because "two runs collide" is three different claims and they
are separable:

  --mode copies    (default) every instance gets its OWN COPY of the build
                   directory and runs with that copy as its working directory.
                   This is what two LANES look like: separate exe, separate
                   cwd, separate exe-adjacent files. A failure here is a
                   MACHINE-WIDE resource -- a named object, a fixed port, the
                   desktop, a timing assumption -- and nothing else.
  --mode samecwd   every instance runs the ONE build directory as its cwd. Adds
                   back the working-directory-relative names (the selftest BMP,
                   playlog/) and the exe-adjacent ones (crash.txt, exit.txt,
                   settings.json). A failure here but not in `copies` is a FILE
                   NAME, and the run tail says which.
  --mode sametemp  `copies`, plus one shared %TEMP% for every instance, which
                   is what the lanes on this box actually have. Isolates the
                   %TEMP%-relative group (the rich crash dumps, pruned to four
                   at every boot).

USAGE

    python port/tools/slot_collide.py <repo-root> --n 2 --trials 10
    python port/tools/slot_collide.py <repo-root> --n 4 --trials 10 --mode copies
    python port/tools/slot_collide.py <repo-root> --n 1 --trials 10   # baseline

    --levels 1,7,13     the level ids to spread across the instances
                        (default: the first --n mounted levels, rotated per
                        trial so a failure cannot be blamed on one level)
    --scenes 44,68      run scene selftests instead of level ones
    --frames 300        SM64DS_WINDOW_SELFTEST / SM64DS_SCENE_FRAMES
    --out DIR           write every child's stdout+stderr here as
                        trial<K>-inst<I>-<label>.log
    --instance-tag      set SM64DS_INSTANCE per child (the port's own
                        per-instance file suffix, hal/instance_tag.h)
    --serial            run the same set ONE AT A TIME. The control arm: if the
                        serial arm reds too, the failure is not the collision.

EXIT 0 when every child of every trial returned 0, 1 otherwise. The summary
table is the deliverable; the per-child logs are the evidence behind it.
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
import threading
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import battery
import slot_lock


def wait_for_slot(quiet=False):
    """Block while another lane holds the REAL windowed slot lock.

    This is not an acquire: nothing here holds the lock, and colliding is the
    whole point of the script. It only refuses to add a collision to somebody
    else's gate run, which is a different thing from serialising with them.
    """
    path = slot_lock.lock_path()
    waited = 0.0
    while os.path.exists(path) and not slot_lock._is_stale(path):
        if not quiet and waited == 0.0:
            pid, _, _ = slot_lock._read_holder(path)
            print(f"[collide] another lane holds the windowed slot (pid {pid});"
                  f" waiting rather than colliding with a real gate",
                  file=sys.stderr)
        time.sleep(2.0)
        waited += 2.0
    if waited and not quiet:
        print(f"[collide] slot free after {waited:.0f}s", file=sys.stderr)


def walk_window_running():
    """How many walk_window.exe processes exist right now (any owner)."""
    try:
        out = subprocess.run(["tasklist", "/FI", "IMAGENAME eq walk_window.exe",
                              "/NH"], capture_output=True, text=True,
                             timeout=30).stdout
    except (OSError, subprocess.SubprocessError):
        return -1
    return sum(1 for line in out.splitlines()
               if "walk_window" in line.lower())


def make_copies(build, n, workroot):
    """One private copy of the build directory per instance.

    Copied rather than junctioned: a junction shares the exe-adjacent files,
    which is exactly the variable this mode is trying to hold still.
    """
    dirs = []
    for i in range(n):
        d = os.path.join(workroot, "inst%d" % i)
        if os.path.isdir(d):
            shutil.rmtree(d, ignore_errors=True)
        os.makedirs(d, exist_ok=True)
        for name in os.listdir(build):
            src = os.path.join(build, name)
            if os.path.isfile(src) and (name.endswith(".exe") or
                                        name.endswith(".dll") or
                                        name.endswith(".json")):
                shutil.copy2(src, os.path.join(d, name))
        dirs.append(d)
    return dirs


def child_env(args, which, inst, tempdir=None):
    """battery.py's own environment for this row, plus the isolation knobs."""
    if args.scenes:
        env = battery.scene_env(which)
        env["SM64DS_SCENE_FRAMES"] = str(args.frames)
    else:
        env = battery.selftest_env(which)
        env["SM64DS_WINDOW_SELFTEST"] = str(args.frames)
    if args.instance_tag:
        env["SM64DS_INSTANCE"] = "c%d" % inst
    if tempdir:
        env["TEMP"] = tempdir
        env["TMP"] = tempdir
    return env


# The exe-adjacent post-mortem files, and what their PRESENCE means. This is
# the cheapest way to tell an external kill from the program's own decision:
# fault_probe.h's exit hook is a detour on RtlExitUserProcess, so a nonzero exit
# through the ORDERLY door (exit/ExitProcess) always leaves exit.txt, and
# TerminateProcess -- which is what taskkill /F does, and what hands back exit
# code 1 -- bypasses the detour and leaves NOTHING. So:
#   rc!=0 with exit.txt      the program chose to stop; the file says where
#   rc!=0 with crash.txt     it faulted; the file has the address
#   rc!=0 with NEITHER       something outside the process ended it
POSTMORTEM = ("crash.txt", "exit.txt", "startup_error.txt")


def postmortem(cwd, exe, tag=""):
    """Which post-mortem files exist beside the exe, with their first line."""
    out = {}
    d = os.path.dirname(os.path.abspath(exe)) or cwd
    for name in POSTMORTEM:
        stem, ext = os.path.splitext(name)
        for cand in ((stem + tag + ext) if tag else name, name):
            p = os.path.join(d, cand)
            if os.path.exists(p):
                try:
                    with open(p, "r", encoding="utf-8", errors="replace") as f:
                        out[cand] = f.read(400).replace("\n", " | ")[:400]
                except OSError:
                    out[cand] = "(unreadable)"
                break
    return out


def clear_postmortem(cwd, exe):
    d = os.path.dirname(os.path.abspath(exe)) or cwd
    for name in os.listdir(d):
        for stem in ("crash", "exit", "startup_error"):
            if name.startswith(stem) and name.endswith(".txt"):
                try:
                    os.remove(os.path.join(d, name))
                except OSError:
                    pass


def run_one(exe, cwd, env, timeout, result, idx):
    t0 = time.time()
    try:
        r = subprocess.run([exe], cwd=cwd, env=env, timeout=timeout,
                           capture_output=True, text=True,
                           creationflags=battery.NO_CONSOLE,
                           startupinfo=battery.SI_MIN)
        result[idx] = (r.returncode, r.stdout or "", r.stderr or "",
                       time.time() - t0)
    except subprocess.TimeoutExpired as e:
        result[idx] = ("TIMEOUT", (e.stdout or b"").decode("utf-8", "replace")
                       if isinstance(e.stdout, bytes) else (e.stdout or ""),
                       (e.stderr or b"").decode("utf-8", "replace")
                       if isinstance(e.stderr, bytes) else (e.stderr or ""),
                       time.time() - t0)
    except OSError as e:
        result[idx] = ("OSERROR %s" % e, "", "", time.time() - t0)


def main(argv=None):
    ap = argparse.ArgumentParser(add_help=True)
    ap.add_argument("root", nargs="?", default=".")
    ap.add_argument("--n", type=int, default=2)
    ap.add_argument("--trials", type=int, default=10)
    ap.add_argument("--mode", choices=("copies", "samecwd", "sametemp"),
                    default="copies")
    ap.add_argument("--levels", default="")
    ap.add_argument("--scenes", default="")
    ap.add_argument("--frames", type=int, default=300)
    ap.add_argument("--timeout", type=int, default=battery.STEP_TIMEOUT)
    ap.add_argument("--out", default="")
    ap.add_argument("--work", default="")
    ap.add_argument("--instance-tag", action="store_true")
    ap.add_argument("--serial", action="store_true")
    ap.add_argument("--no-wait-for-slot", action="store_true")
    ap.add_argument("--no-hold-slot", action="store_true")
    args = ap.parse_args(argv)

    root = os.path.abspath(args.root)
    build = os.path.join(root, "build", "port")
    exe_name = "walk_window.exe"
    if not os.path.exists(os.path.join(build, exe_name)):
        print(f"collide: no {exe_name} in {build}", file=sys.stderr)
        return 2

    if args.scenes:
        rows = [int(x) for x in args.scenes.split(",") if x.strip()]
        kind = "scene"
    elif args.levels:
        rows = [int(x) for x in args.levels.split(",") if x.strip()]
        kind = "level"
    else:
        rows = list(battery.mounted_levels(root))[:max(args.n, 2)]
        kind = "level"
    if not rows:
        print("collide: no rows to run", file=sys.stderr)
        return 2

    out = os.path.abspath(args.out) if args.out else ""
    if out:
        os.makedirs(out, exist_ok=True)

    workroot = os.path.abspath(args.work) if args.work else \
        os.path.join(root, "build", "collide")
    os.makedirs(workroot, exist_ok=True)

    if args.mode in ("copies", "sametemp"):
        cwds = make_copies(build, args.n, workroot)
    else:
        cwds = [build] * args.n

    temps = [None] * args.n
    if args.mode == "sametemp":
        shared = os.path.join(workroot, "sharedtemp")
        os.makedirs(shared, exist_ok=True)
        temps = [shared] * args.n

    if not args.no_hold_slot:
        # HOLD THE REAL SLOT FOR THE WHOLE BLOCK. See the banner: the only
        # overlap in the sample must be the overlap this script created.
        with slot_lock.slot(label="slot_collide n=%d %s" % (args.n, args.mode)):
            return trials(args, build, cwds, temps, rows, kind, out, exe_name)
    if not args.no_wait_for_slot:
        wait_for_slot()
    return trials(args, build, cwds, temps, rows, kind, out, exe_name)


def trials(args, build, cwds, temps, rows, kind, out, exe_name):
    print(f"collide: {args.trials} trial(s) of {args.n} instance(s), "
          f"mode={args.mode}, {kind}s {rows}, {args.frames} frames, "
          f"{'SERIAL' if args.serial else 'CONCURRENT'}")

    table = []
    reds = 0
    for t in range(args.trials):
        n_live = walk_window_running()
        if n_live > 0:
            print(f"  trial {t}: {n_live} walk_window.exe already running; "
                  f"waiting", file=sys.stderr)
            while walk_window_running() > 0:
                time.sleep(2.0)
        # Only when this process is NOT the holder: holding our own lock and
        # then waiting for it to clear would wait forever.
        if args.no_hold_slot and not args.no_wait_for_slot:
            wait_for_slot(quiet=True)

        picks =[rows[(t + i) % len(rows)] for i in range(args.n)]
        result = [None] * args.n
        threads = []
        exes = []
        for i in range(args.n):
            e = os.path.join(cwds[i], exe_name)
            exes.append(e if os.path.exists(e)
                        else os.path.join(build, exe_name))
        for e, c in zip(exes, cwds):
            clear_postmortem(c, e)
        t0 = time.time()
        for i in range(args.n):
            exe = exes[i]
            env = child_env(args, picks[i], i, temps[i])
            th = threading.Thread(target=run_one,
                                  args=(exe, cwds[i], env, args.timeout,
                                        result, i))
            threads.append(th)
        if args.serial:
            for th in threads:
                th.start()
                th.join()
        else:
            for th in threads:
                th.start()
            for th in threads:
                th.join()
        wall = time.time() - t0

        row = {"trial": t, "wall": round(wall, 1), "picks": picks,
               "rcs": [], "why": [], "post": []}
        for i in range(args.n):
            rc, so, se, secs = result[i]
            row["rcs"].append(rc)
            pm = postmortem(cwds[i], exes[i],
                            "." + ("c%d" % i) if args.instance_tag else "")
            row["post"].append(pm)
            log = f"=== trial {t} inst {i} {kind} {picks[i]} rc={rc} " \
                  f"{secs:.1f}s\ncwd={cwds[i]}\npost-mortem={pm}\n" \
                  f"--- stdout ---\n{so}\n--- stderr ---\n{se}\n"
            if out:
                with open(os.path.join(
                        out, f"trial{t:02d}-inst{i}-{kind}{picks[i]}.log"),
                        "w", encoding="utf-8", errors="replace") as f:
                    f.write(log)
            if rc != 0:
                reds += 1
                why = first_complaint(so, se)
                row["why"].append(
                    f"inst{i} {kind} {picks[i]}: {why} "
                    f"[post-mortem: {sorted(pm) or 'NOTHING -- killed from '
                                              'outside the process'}]")
        table.append(row)
        flag = "RED" if any(r != 0 for r in row["rcs"]) else "ok "
        print(f"  trial {t:2d}  {flag}  {wall:5.1f}s  "
              + "  ".join(f"{kind[0]}{p}=rc{r}"
                          for p, r in zip(picks, row["rcs"])))
        for w in row["why"]:
            print(f"          {w}")

    print(f"collide: {args.trials} trial(s) x {args.n} instance(s) = "
          f"{args.trials * args.n} runs, {reds} red")
    if out:
        with open(os.path.join(out, "summary.json"), "w",
                  encoding="utf-8") as f:
            json.dump({"mode": args.mode, "n": args.n,
                       "trials": args.trials, "serial": args.serial,
                       "kind": kind, "rows": rows, "frames": args.frames,
                       "reds": reds, "table": table}, f, indent=1)
    return 1 if reds else 0


# What did the failing child actually say? The first line of a failure is the
# lead, and a summary that only prints rc hands the reader nothing.
COMPLAINT_MARKS = ("UNHOSTED", "[io]", "did not open", "faulted", "refus",
                   "could not", "FATAL", "abort", "no window", "quarantine",
                   "error", "Error", "ERROR", "fail", "FAIL")


def first_complaint(stdout, stderr):
    for text in (stderr, stdout):
        for line in (text or "").splitlines():
            if any(m in line for m in COMPLAINT_MARKS):
                return line.strip()[:220]
    tail = [l for l in ((stderr or "") + (stdout or "")).splitlines() if l.strip()]
    return tail[-1].strip()[:220] if tail else "(no output)"


if __name__ == "__main__":
    sys.exit(main())
