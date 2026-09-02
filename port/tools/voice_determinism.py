#!/usr/bin/env python3
"""Voice chat changes the sound and NOTHING ELSE. Lane VOICE.

TWO CLAIMS, AND THE SECOND ONE IS THE STRONG ONE.

  1. A wide session with voice on all windows still reports NO DIVERGENCE
     between every pair of windows. That is the standing bar for any change
     that touches the comms path.

  2. The world hashes a window produces with voice ON are FRAME-FOR-FRAME
     IDENTICAL to the ones the same window produces with voice OFF. Claim 1
     alone can be satisfied by a change that moved every window the same wrong
     way; this one cannot. It is the same cross-run hash comparison the VS16
     host-loop lane used, run here between two configurations rather than two
     builds.

Both passes hold one direction per window (SM64DS_COMMS_INJECT) so the bodies
actually move and the hashes have something to be about, and both are driven
from the same fixed per-slot key table, which is what makes the two runs
comparable at all.

Quiet and muted through mp2_proof.spawn, plus SM64DS_NO_AUDIO=1 and the tone
hook on every window, so no recording device and no output device is opened by
any of them.

    python port/tools/voice_determinism.py [--windows N] [--frames N]

Exit 0 all green, 1 on the first red.
"""
import argparse
import json
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
OUT = os.path.join(ROOT, "runs", "voice", "out", "det")
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
DHDIFF = os.path.join(HERE, "dhdiff.py")

PORT = M.PORT_BASE + 64
VS_MAP = "0"

# One held direction per window, so every body walks a different way. The same
# table in both passes: two runs that pressed different buttons could not be
# compared frame for frame and the second claim would be empty.
KEYS = ["0x0040", "0x0080", "0x0020", "0x0010"]


def settings(d, on):
    cfg = {
        "VoiceEnabled": bool(on),
        "VoiceMicDevice": "",
        "VoiceVolume": 100,
        # Radii wide enough that every window is inside the near radius, so
        # the voice path is running at FULL tilt for the whole pass. Proving
        # determinism with the falloff holding everything at zero would prove
        # nothing about the loud case.
        "VoiceNearRadius": 100000,
        "VoiceFarRadius": 200000,
    }
    with open(os.path.join(d, "settings.json"), "w", encoding="utf-8") as f:
        json.dump(cfg, f, indent=2)


def run_pass(tag, n, frames, port, voice_on):
    """N windows, one session, one arena. Returns the per-window log paths."""
    base = os.path.join(OUT, tag)
    dirs, procs, logs = [], [], []
    # The parent outlives everyone: at five windows and up, a parent given the
    # children's budget spends it WAITING for the last joiner and sends BYE
    # while the last window is still booting, which reads exactly like a
    # desync and is not one. Same allowance vs16_ladder.sh makes.
    pbudget = str(int(frames) + 60 * (n + 8))
    for k in range(n):
        d = os.path.join(base, "p%d" % k)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        settings(d, voice_on)
        dirs.append(d)
    for k in range(n):
        e = M.env_base(ROOT, dirs[k], "%s_p%d" % (tag, k))
        e.pop("SM64DS_LEVEL", None)
        e.pop("SM64DS_SCENE", None)
        e["SM64DS_VS_MAP"] = VS_MAP
        e["SM64DS_VS_PLAYERS"] = str(n)
        e["SM64DS_VS_STATE_HASH"] = "2"
        e["SM64DS_COMMS_PORT"] = str(port)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_COMMS_INJECT"] = "key=%s" % KEYS[k % len(KEYS)]
        e["SM64DS_NO_AUDIO"] = "1"
        e["SM64DS_VOICE_TEST_TONE"] = "1"
        e["SM64DS_VOICE_REPORT"] = "1"
        if k == 0:
            e["SM64DS_COMMS_ROLE"] = "parent"
            e["SM64DS_WINDOW_SELFTEST"] = pbudget
        else:
            e["SM64DS_COMMS_ROLE"] = "child"
            e["SM64DS_COMMS_SLOT"] = str(k)
            e["SM64DS_WINDOW_SELFTEST"] = frames
        log = os.path.join(dirs[k], "run.log")
        logs.append(log)
        procs.append(M.spawn(EXE, dirs[k], e, log))
        if k == 0:
            time.sleep(1.0)
    rcs = [M.finish(p, 1200) for p in procs]
    return logs, rcs


def dhdiff(a, b):
    """-> (rc, first line of output). rc 0 agree, 1 diverge, 2 nothing to
    compare."""
    p = subprocess.run([sys.executable, DHDIFF, a, b],
                       capture_output=True, text=True)
    lines = [x for x in (p.stdout or p.stderr or "").strip().splitlines()
             if x.strip()]
    # THE VERDICT IS THE LAST LINE, not the first. dhdiff opens with a file
    # census and closes with "NO DIVERGENCE ... for all N common frames" or the
    # first differing frame, and quoting the census made every verdict here read
    # as a header rather than an answer.
    return p.returncode, (lines[-1] if lines else "(no output)")


def voice_counts(text):
    import re
    m = None
    for m in re.finditer(r"\[voice\] \S+: on=(\d+) tone=(\d+) dev=(\d+) "
                         r"cap=(\d+) tx=(\d+) rx=(\d+) bad=(\d+) dup=(\d+)",
                         text):
        pass
    if not m:
        return None
    return dict(on=int(m.group(1)), tone=int(m.group(2)), dev=int(m.group(3)),
                cap=int(m.group(4)), tx=int(m.group(5)), rx=int(m.group(6)),
                bad=int(m.group(7)), dup=int(m.group(8)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--windows", type=int, default=4)
    ap.add_argument("--frames", default="900")
    a = ap.parse_args()

    if not os.path.exists(EXE):
        print("no walk_window.exe at %s -- build first" % EXE)
        return 1
    for shadow in (os.path.join(os.path.dirname(EXE), "settings.json"),
                   os.path.join(ROOT, "settings.json")):
        if os.path.exists(shadow):
            print("refusing: %s would shadow the per-window settings.json"
                  % shadow)
            return 1
    os.makedirs(OUT, exist_ok=True)
    n = a.windows
    ok = True

    on_logs, on_rcs = run_pass("on", n, a.frames, PORT, True)
    ok &= M.verdict(all(r == 0 for r in on_rcs),
                    "voice-det every window with voice ON exited clean | rc %s"
                    % (on_rcs,))
    # THE ON PASS HAS TO ACTUALLY BE TALKING, or the cross-run comparison at
    # the bottom is trivially true and proves nothing. Every window sends, every
    # window receives from its peers, and nothing is malformed.
    for k in range(n):
        c = voice_counts(M.text(on_logs[k]))
        ok &= M.verdict(c is not None and c["on"] == 1 and c["tx"] > 0 and
                        c["rx"] > 0 and c["bad"] == 0 and c["dev"] == 0,
                        "voice-det ON p%d was really talking and listening | %s"
                        % (k, c))
    for i in range(n):
        for j in range(i + 1, n):
            rc, line = dhdiff(on_logs[i], on_logs[j])
            ok &= M.verdict(rc == 0,
                            "voice-det ON p%d vs p%d | %s" % (i, j, line))

    off_logs, off_rcs = run_pass("off", n, a.frames, PORT + 32, False)
    ok &= M.verdict(all(r == 0 for r in off_rcs),
                    "voice-det every window with voice OFF exited clean | rc %s"
                    % (off_rcs,))
    for k in range(n):
        c = voice_counts(M.text(off_logs[k]))
        ok &= M.verdict(c is None,
                        "voice-det OFF p%d sent and received NOTHING (the "
                        "report line is only emitted with voice on) | %s"
                        % (k, c))
    for i in range(n):
        for j in range(i + 1, n):
            rc, line = dhdiff(off_logs[i], off_logs[j])
            ok &= M.verdict(rc == 0,
                            "voice-det OFF p%d vs p%d | %s" % (i, j, line))

    # THE STRONG CLAIM. Same window, same slot, same held key, voice on against
    # voice off. Any world state voice touched would show here and nowhere
    # else.
    for k in range(n):
        rc, line = dhdiff(on_logs[k], off_logs[k])
        ok &= M.verdict(rc == 0,
                        "voice-det CROSS-RUN p%d: voice ON hashes are "
                        "identical to voice OFF | %s" % (k, line))

    print("")
    for v in M.VERDICTS:
        print(v)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
