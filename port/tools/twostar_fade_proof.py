"""TWO STARS IN ONE SESSION, AND THE PICTURE AFTER EACH OF THEM.

THE ROW THIS EXISTS FOR. A player reported that the screen went black after the
second star he collected in a session, stayed black while he could still hear
himself moving, and came back only when a text box appeared. The cause was the
seven-wipe pool: Stage::InitResources builds it fresh at every level boot and
the port's stand-ins are static, so the second star's arrival found
hal_wipes[5] already at its target, nothing wrote 0x4000050 / 0x4000054 again,
and engine A's brightness-decrease blend stayed at EVY 16 over the whole
picture. Every gate the port had was blind to it: no sweep, no proof and no
capture collects two stars in one process, and the arrival fade is not a byte
the selftest BMP carries.

So this is the regression row. It collects two stars in ONE process, answers
both level-clear menus, and asserts that the picture comes back after BOTH of
them: the arrival is allowed to be covered while the fade runs, and it is not
allowed to stay covered.

HOW IT DRIVES THE GAME, and every piece of it is an existing knob:

  SM64DS_LEVEL=6              boot into Bob-omb Battlefield
  SM64DS_STAR_TRIGGER=a,b     the ROM's own PowerStar touch gate, at two frames
  SM64DS_WARP_SEQ=6@f         LoadLevelNoReturn back into the course between them
  SM64DS_HOST_PAD             an A press every 50 host frames, which is what ends
                              the star talk and the course intro message
  SM64DS_TOUCH_PROBE          a stylus poke on the level-clear menu's first row
                              (DS 128,56 -- save and continue), which answers it
  SM64DS_FADE_WATCH           the per-frame luminance and register line this
                              reads its verdict out of

THE VERDICT IS THE PICTURE, not a register. For each star: find the level change
back to the castle, then count how many frames in a row the picture is fully
dark (every pixel below luminance 8) from there. The cartridge's own arrival
fade is thirty frames, so the floor is generous and the failure is not close to
it: the bug held the picture black for 121 frames and only a menu answer ended
it. --max-dark sets the ceiling, default 60.

Quiet by construction: minimized, never activated, muted, no console, and every
inherited SM64DS_* dropped so a lane's own environment cannot reach the run.

  python port/tools/twostar_fade_proof.py <root> [--max-dark 60] [--keep]
"""

import argparse
import os
import re
import subprocess
import sys
import tempfile

# the frame plan, in the counters each knob actually uses: STAR_TRIGGER and
# WARP_SEQ take ROM frames, TOUCH_PROBE takes the presentation counter, and the
# two agree until a mid-run level load makes the presentation counter run ahead
# -- which is why the second menu's poke frame is larger than its ROM frame.
STAR1_FRAME = 200
WARP_FRAME = 900
STAR2_FRAME = 1100
MENU1_POKE = "412-418:128:56"
MENU2_POKE = "1473-1479:128:56"
FRAMES = 2000

LINE = re.compile(
    r"\[fadew\] f(\d+)\s+luma\s+([0-9.]+)\s+dark\s+([0-9.]+)")
CHANGE = re.compile(r"\[lvl\] change: level (\d+) -> (\d+),")


def spawn(root, workdir, env_extra, timeout):
    exe = os.path.join(root, "build", "port", "walk_window.exe")
    if not os.path.exists(exe):
        sys.exit("twostar_fade_proof: no build at %s" %
                 os.path.relpath(exe, root))
    env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    env["TEMP"] = env["TMP"] = os.path.join(root, "tmp")
    env["SM64DS_NO_FOCUS"] = "1"
    env["SM64DS_MINIMIZED"] = "1"
    env["SM64DS_VOLUME"] = "0"
    env["SM64DS_FAULTS_FATAL"] = "1"
    env.update(env_extra)
    si = subprocess.STARTUPINFO()
    si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    si.wShowWindow = 7                      # SW_SHOWMINNOACTIVE
    nocon = getattr(subprocess, "CREATE_NO_WINDOW", 0)
    p = subprocess.Popen([exe], cwd=workdir, env=env,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         text=True, errors="replace",
                         startupinfo=si, creationflags=nocon)
    try:
        out, err = p.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        p.kill()
        out, err = p.communicate()
        return None, out, err
    return p.returncode, out, err


def dark_run_from(frames, start):
    """How many frames in a row, from the first dark frame at or after `start`,
    the picture is fully dark. 0 when it never goes dark at all."""
    i = 0
    keys = sorted(frames)
    while i < len(keys) and keys[i] < start:
        i += 1
    while i < len(keys) and frames[keys[i]] < 1.0:
        i += 1
    if i >= len(keys):
        return 0, None
    first = keys[i]
    n = 0
    while i < len(keys) and frames[keys[i]] >= 1.0:
        n += 1
        i += 1
    return n, first


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--max-dark", type=int, default=60,
                    help="frames the arrival may stay fully dark (default 60)")
    ap.add_argument("--keep", action="store_true",
                    help="keep the working directory and print its path")
    args = ap.parse_args()
    root = os.path.abspath(args.root)

    workdir = tempfile.mkdtemp(prefix="twostar_", dir=os.path.join(root, "tmp"))
    pad = ",".join("1000@%d-%d" % (f, f + 6) for f in range(300, 8000, 50))
    env = {
        "SM64DS_WINDOW_SELFTEST": str(FRAMES),
        "SM64DS_SELFTEST_IDLE": "1",
        "SM64DS_LEVEL": "6",
        "SM64DS_STAR_TRIGGER": "%d,%d" % (STAR1_FRAME, STAR2_FRAME),
        "SM64DS_WARP_SEQ": "6@%d" % WARP_FRAME,
        "SM64DS_HOST_PAD": pad,
        "SM64DS_TOUCH_PROBE": "%s,%s" % (MENU1_POKE, MENU2_POKE),
        "SM64DS_FADE_WATCH": "%d-%d" % (STAR1_FRAME - 20, FRAMES),
    }
    rc, out, err = spawn(root, workdir, env, 900)
    if args.keep:
        open(os.path.join(workdir, "stderr.txt"), "w",
             errors="replace").write(err or "")
        print("twostar_fade_proof: working directory kept at %s" %
              os.path.relpath(workdir, root))

    if rc is None:
        print("TWO-STAR FADE PROOF: FAIL -- the run did not finish inside its "
              "budget")
        return 1
    if rc != 0:
        print("TWO-STAR FADE PROOF: FAIL -- the run exited %s" % rc)
        return 1

    dark = {}
    for m in LINE.finditer(err or ""):
        dark[int(m.group(1))] = float(m.group(3))
    # the two returns to the castle, in order: level N -> 1
    returns = []
    rom = 0
    for line in (err or "").splitlines():
        fm = re.match(r"\[f(\d+)\]", line)
        if fm:
            rom = int(fm.group(1))
            continue
        cm = CHANGE.search(line)
        if cm and cm.group(2) == "1":
            returns.append(rom)
    if len(returns) < 2:
        print("TWO-STAR FADE PROOF: FAIL -- only %d star return(s) happened; "
              "the run collected fewer than two stars" % len(returns))
        return 1
    if not dark:
        print("TWO-STAR FADE PROOF: FAIL -- no [fadew] lines; the build has no "
              "SM64DS_FADE_WATCH")
        return 1

    bad = 0
    rows = []
    for i, at in enumerate(returns[:2]):
        n, first = dark_run_from(dark, at)
        rows.append("star %d: level change at rom f%d, picture fully dark for "
                    "%d frame(s) from f%s" %
                    (i + 1, at, n, first if first is not None else "-"))
        if n > args.max_dark:
            bad += 1
    verdict = "PASS" if not bad else "FAIL"
    print("TWO-STAR FADE PROOF: %s -- %s (ceiling %d)" %
          (verdict, "; ".join(rows), args.max_dark))
    return 0 if not bad else 1


if __name__ == "__main__":
    sys.exit(main())
