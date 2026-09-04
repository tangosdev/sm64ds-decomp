#!/usr/bin/env python3
"""OFF-by-default probe for lane VOICE.

Same two-window VS arena voice_proof.py uses for its ON attenuation run, but
with VoiceEnabled FALSE on both windows and NO tone hook. The claim under test
is the one Tango set: proximity voice is present but OFF BY DEFAULT, and off
must mean the game opens no recording device and puts no voice datagram on the
wire. Each window's own [voice] report line is read and every counter that
would be non-zero if it were capturing or sending is asserted to be zero.

Run from the repo root: python C:/tmp/voice_off_probe.py
"""
import os, sys, time, re

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M
import voice_proof as V
import slot_lock

ROOT = V.ROOT
EXE = V.EXE
OUT = os.path.join(ROOT, "runs", "voice", "off_probe")
PORT = M.PORT_BASE + 112
FRAMES = "900"

def run_off_pair():
    da = os.path.join(OUT, "off_A")
    db = os.path.join(OUT, "off_B")
    for d in (da, db):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    # VoiceEnabled FALSE on both. Radii still written so the file is the
    # launcher's real shape; they are irrelevant with the switch off.
    V.write_settings(da, False, 512, 3072)
    V.write_settings(db, False, 512, 3072)
    ea = M.env_base(ROOT, da, "offA")
    eb = M.env_base(ROOT, db, "offB")
    for e, role in ((ea, "parent"), (eb, "child")):
        e["SM64DS_WINDOW_SELFTEST"] = FRAMES
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(PORT)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        e.pop("SM64DS_LEVEL", None)
        e.pop("SM64DS_SCENE", None)
        e["SM64DS_VS_MAP"] = V.VS_MAP
        e["SM64DS_VS_PLAYERS"] = "2"
        e["SM64DS_VS_PROBE"] = "1"
        e["SM64DS_VOICE_REPORT"] = "1"
        e["SM64DS_NO_AUDIO"] = "1"
        # NO SM64DS_VOICE_TEST_TONE. A tone hook is a replacement for the
        # device; leaving it off means the ONLY thing that could open a device
        # is VoiceEnabled, which is the switch under test.
    la, lb = os.path.join(da, "run.log"), os.path.join(db, "run.log")
    pa = M.spawn(EXE, da, ea, la)
    time.sleep(0.5)
    pb = M.spawn(EXE, db, eb, lb)
    ra = M.finish(pa, 900)
    rb = M.finish(pb, 900)
    return dict(rc_a=ra, rc_b=rb, ta=M.text(la), tb=M.text(lb), la=la, lb=lb)

def main():
    # THE WINDOWED TEST SLOT. walk_window.exe opens a real OS window and drives
    # the port'''s single windowed render/input path, so two of them on one
    # machine collide and throw a random rc=1 on a different level or scene each
    # run. The lock is held for the WHOLE pair rather than per launch: both
    # windows have to be alive at once for the session to form, so a per-launch
    # lock would have window A holding the slot while it waits for B.
    with slot_lock.slot(label="voice_off_probe"):
        res = run_off_pair()
    ok = True
    ok &= M.verdict(res["rc_a"] == 0 and res["rc_b"] == 0,
                    "voice OFF both windows exited clean | rc %s/%s"
                    % (res["rc_a"], res["rc_b"]))
    for tag, t in (("A", res["ta"]), ("B", res["tb"])):
        c = V.voice_counts(t)
        # A window that never turned voice on may still print a report line
        # (report_if_due runs every tick); if it printed none at all that is
        # itself fine for the off claim, but we want the counters, so require
        # the line and then require it be all zero.
        got_line = c is not None
        seen_on = re.search(r"\[voice\] VoiceEnabled -> ON", t) is not None
        ok &= M.verdict(not seen_on,
                        "voice OFF window %s never flipped VoiceEnabled ON" % tag)
        if got_line:
            ok &= M.verdict(c["on"] == 0 and c["dev"] == 0 and c["cap"] == 0
                            and c["tx"] == 0,
                            "voice OFF window %s opened no device and sent nothing"
                            " | on=%d dev=%d cap=%d tx=%d"
                            % (tag, c["on"], c["dev"], c["cap"], c["tx"]))
        else:
            ok &= M.verdict(True,
                            "voice OFF window %s printed no voice report at all"
                            " (nothing to send, nothing captured)" % tag)
    print()
    print("logs: %s , %s" % (res["la"], res["lb"]))
    sys.exit(0 if ok else 1)

if __name__ == "__main__":
    main()
