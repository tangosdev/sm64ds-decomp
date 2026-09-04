#!/usr/bin/env python3
"""Auto-pick and manual override, proved on this machine. Lane VOICE.

WHAT IT PROVES, and why it takes a whole game run to prove it.

The bug this exists for is not a crash and not a log line: voice was silent
while every layer reported success. A per-device probe (hal/mic_probe_win.cpp,
commit a9f5dcc4) measured why, on the owner's machine:

    device 0  'Microphone (5- Razer BlackShark)'  opens OK, PEAK    1/32767
    device 1  'Microphone (Razer Seiren Mini)'    opens OK, PEAK  470/32767

Device 0 was the Windows DEFAULT recording device, so it is the one the game
got. Both devices open. Only one of them is a microphone in any useful sense.

So the claim under test is a CHOICE, not a return code, and the only honest way
to test a choice is to let the real game make it with the real settings file and
read which device it landed on. Each rung below runs walk_window.exe headless
with a settings.json the launcher could have written, and asserts on the
[voice] lines the capture backend prints:

  RUNG 1  AUTO      VoiceMicDevice "" (the default). The scan must run, must
                    report every device it measured, and must CHOOSE a device
                    whose peak cleared the silence floor. On a machine with a
                    live non-default mic that means it must NOT land on the
                    Windows default.
  RUNG 2  OVERRIDE  VoiceMicIndex 0. The scan must NOT run at all and the open
                    must be on device 0, whatever the scan would have said.
  RUNG 3  NAMED     VoiceMicDevice set to a name. The scan must not run and the
                    named device must be the one opened.

RUNG 1 IS MACHINE-DEPENDENT ON PURPOSE. It asserts "a live device was chosen"
rather than "device 1 was chosen": a machine whose only microphone is device 0
is not a failure of this feature. What it will not tolerate is the scan not
running, or running and choosing a device it just called silent.

Quiet and muted throughout, through mp2_proof.spawn: no window, no focus, no
audio out. A recording device IS opened -- that is the entire point.

    python port/tools/voice_micpick_proof.py [--frames N]

Exit 0 all green, 1 on the first red.
"""
import argparse
import json
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
OUT = os.path.join(ROOT, "runs", "voice", "micpick")
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")

# The scan costs a couple hundred milliseconds once, on the first tick that
# finds voice on. 240 frames is four seconds of selftest, which is far more
# than the open needs and still short enough to run three of them.
DEFAULT_FRAMES = 240


def write_settings(d, mic="", index=-1):
    """settings.json exactly as the launcher writes it, plus the two keys
    under test. The radii are written because the launcher writes them and the
    file should be the launcher's real shape; they do not matter here."""
    cfg = {
        "VoiceEnabled": True,
        "VoiceMicDevice": mic,
        "VoiceMicIndex": int(index),
        "VoiceVolume": 100,
        "VoiceNearRadius": 512,
        "VoiceFarRadius": 3072,
    }
    with open(os.path.join(d, "settings.json"), "w", encoding="utf-8") as f:
        json.dump(cfg, f, indent=2)
    return cfg


def run_rung(tag, frames, mic="", index=-1):
    d = os.path.join(OUT, tag)
    if os.path.isdir(d):
        shutil.rmtree(d, ignore_errors=True)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    write_settings(d, mic, index)
    e = M.env_base(ROOT, d, tag)
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    # The standing collision rule for a windowed test on this machine.
    e["SM64DS_TEST_LOCK"] = "1"
    e["SM64DS_TEST_LOCK_PATH"] = r"C:\tmp\sm64ds-test-slot\windowed_test.lock"
    # No tone hook: a generated tone REPLACES the device, and this rung is
    # about which device gets opened.
    e.pop("SM64DS_VOICE_TEST_TONE", None)
    log = os.path.join(d, "run.log")
    rc = M.run_one(EXE, d, e, log, timeout=300)
    with open(log, "r", encoding="utf-8", errors="replace") as f:
        text = f.read()
    lines = [ln.rstrip() for ln in text.splitlines()
             if "[voice]" in ln or "VoiceEnabled" in ln]
    return rc, lines, text


SCAN_RE = re.compile(r"auto-pick: measuring (\d+) recording device")
MEASURED_RE = re.compile(
    r"auto-pick:\s+device (\d+) '([^']*)': peak (-?\d+)/32767 -- (LIVE|silent)")
CHOSE_RE = re.compile(r"auto-pick: CHOSE device (\d+) '([^']*)'")
ALLSIL_RE = re.compile(r"auto-pick: ALL (\d+) measured recording device")
OPEN_RE = re.compile(r"capture open: .*device (-?\d+) '([^']*)'")
INDEX_RE = re.compile(r"VoiceMicIndex (-?\d+) selects device (-?\d+) '([^']*)'")
NAMED_RE = re.compile(r"mic device (\d+) '([^']*)' matches VoiceMicDevice")


def show(tag, lines):
    print("---- %s ----" % tag)
    for ln in lines:
        print("   " + ln)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", type=int, default=DEFAULT_FRAMES)
    a = ap.parse_args()

    if not os.path.isfile(EXE):
        print("RED  no walk_window.exe at %s" % EXE)
        return 1
    os.makedirs(OUT, exist_ok=True)
    reds = []

    # ---- RUNG 1: AUTO ----------------------------------------------------
    rc, lines, _ = run_rung("auto", a.frames, mic="", index=-1)
    show("RUNG 1  AUTO (VoiceMicDevice \"\", VoiceMicIndex -1)", lines)
    joined = "\n".join(lines)
    scan = SCAN_RE.search(joined)
    chose = CHOSE_RE.search(joined)
    allsil = ALLSIL_RE.search(joined)
    opened = OPEN_RE.search(joined)
    measured = MEASURED_RE.findall(joined)
    if rc != 0:
        reds.append("AUTO: walk_window exited %d" % rc)
    if not scan:
        reds.append("AUTO: the auto-pick scan did not run")
    if not measured:
        reds.append("AUTO: no device was measured")
    if not opened:
        reds.append("AUTO: no capture device was opened")
    if chose:
        idx = int(chose.group(1))
        # The chosen device must be one the scan itself called LIVE.
        live = [int(m[0]) for m in measured if m[3] == "LIVE"]
        if idx not in live:
            reds.append("AUTO: chose device %d, which the scan did not call "
                        "LIVE" % idx)
        if opened and int(opened.group(1)) != idx:
            reds.append("AUTO: chose device %d but opened device %s"
                        % (idx, opened.group(1)))
        silent = [int(m[0]) for m in measured if m[3] == "silent"]
        print("   VERDICT: auto-pick chose device %d '%s'; it skipped %d "
              "silent device(s) %s"
              % (idx, chose.group(2), len(silent), silent))
    elif allsil:
        # Not a red on a machine with no working microphone -- that is the
        # documented fallback -- but it means rung 1 proved the fallback and
        # not the pick, and the run has to say so rather than reading green.
        reds.append("AUTO: every device measured silent, so the LIVE-device "
                    "pick could not be proved on this machine (the fallback "
                    "path did run and did not hard-fail)")
    else:
        reds.append("AUTO: the scan neither chose a device nor reported "
                    "all-silent")

    # ---- RUNG 2: INDEX OVERRIDE -----------------------------------------
    rc2, lines2, _ = run_rung("index0", a.frames, mic="", index=0)
    show("RUNG 2  OVERRIDE (VoiceMicIndex 0)", lines2)
    j2 = "\n".join(lines2)
    if rc2 != 0:
        reds.append("OVERRIDE: walk_window exited %d" % rc2)
    if SCAN_RE.search(j2):
        reds.append("OVERRIDE: the auto-pick scan ran anyway")
    m = INDEX_RE.search(j2)
    if not m or int(m.group(1)) != 0:
        reds.append("OVERRIDE: VoiceMicIndex 0 was not honoured")
    o2 = OPEN_RE.search(j2)
    if not o2 or int(o2.group(1)) != 0:
        reds.append("OVERRIDE: the open was not on device 0 (%s)"
                    % (o2.group(1) if o2 else "no open line"))
    else:
        print("   VERDICT: the override opened device 0 '%s' with no scan"
              % o2.group(2))

    # ---- RUNG 3: NAMED OVERRIDE -----------------------------------------
    # The name comes from rung 1's own measurement, so this rung needs no
    # hard-coded device name and works on any machine.
    named_ok = False
    if measured:
        want = measured[-1][1]
        rc3, lines3, _ = run_rung("named", a.frames, mic=want, index=-1)
        show("RUNG 3  NAMED (VoiceMicDevice '%s')" % want, lines3)
        j3 = "\n".join(lines3)
        if rc3 != 0:
            reds.append("NAMED: walk_window exited %d" % rc3)
        if SCAN_RE.search(j3):
            reds.append("NAMED: the auto-pick scan ran anyway")
        n3 = NAMED_RE.search(j3)
        o3 = OPEN_RE.search(j3)
        if not n3:
            reds.append("NAMED: '%s' matched no device" % want)
        elif not o3 or o3.group(1) != n3.group(1):
            reds.append("NAMED: matched device %s but opened %s"
                        % (n3.group(1), o3.group(1) if o3 else "nothing"))
        else:
            named_ok = True
            print("   VERDICT: the name opened device %s '%s' with no scan"
                  % (n3.group(1), n3.group(2)))
    else:
        reds.append("NAMED: skipped, rung 1 measured no device to name")

    print()
    if reds:
        for r in reds:
            print("RED  " + r)
        return 1
    print("GREEN  auto-pick chose a live device, the index override and the "
          "name override both skipped the scan and opened what they named "
          "(named rung ran: %s)" % named_ok)
    return 0


if __name__ == "__main__":
    sys.exit(main())
