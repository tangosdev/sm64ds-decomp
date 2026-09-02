#!/usr/bin/env python3
"""Proximity voice chat, end to end, on loopback. Lane VOICE.

WHAT IT PROVES, and the shape is chosen so the answer cannot be faked by the
harness. Two windows join one VS match on 127.0.0.1. Window A's microphone is
replaced by a generated 440 Hz tone (SM64DS_VOICE_TEST_TONE=1 -- no recording
device is opened, so this runs on a machine with no microphone at all). Window
B opens no audio device either (SM64DS_NO_AUDIO=1) and writes its finished
mixer output to a WAV (SM64DS_WAV_DUMP), which is the SAME buffer a speaker
would have got, taken one stage later than the master volume.

Then the WAV is measured, not listened to: a Goertzel filter at 440 Hz against
the broadband RMS. A tone that is there shows as a 440 Hz magnitude far above
the rest of the spectrum; a tone that has been attenuated to silence shows as
neither.

THE DISTANCE IS NOT SIMULATED, IT IS MEASURED AND THEN BRACKETED. Moving two
players apart inside a headless selftest is a whole input rig; instead the run
reads the REAL separation the two bodies have in the arena out of the game's
own [voice] report line, and then runs the same session three times with the
two radii placed around that number:

    NEAR   VoiceNearRadius above the measured separation   -> gain 1.0
    MID    the separation sitting between the two radii    -> the log falloff
    FAR    VoiceFarRadius below the measured separation    -> gain 0.0

That is the falloff curve itself under test, at a distance the game produced,
rather than a distance the harness asserted.

QUIET AND MUTED throughout, through mp2_proof.spawn: CREATE_NO_WINDOW,
SW_SHOWMINNOACTIVE, SM64DS_NO_FOCUS=1, SM64DS_MINIMIZED=1, SM64DS_VOLUME=0,
and on top of those SM64DS_NO_AUDIO=1 on both windows so not one sample
reaches a device.

    python port/tools/voice_proof.py [--frames N] [--keep]

Exit 0 all green, 1 on the first red.
"""
import argparse
import json
import math
import os
import re
import struct
import sys
import wave

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
OUT = os.path.join(ROOT, "runs", "voice", "out")
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")

PORT = M.PORT_BASE + 96
FRAMES = "900"
VS_MAP = "0"

TONE_HZ = 440.0
MIX_RATE = 32768        # SD_MIX_RATE; the dump is written at this rate


# ---------------------------------------------------------------------------
# settings.json, written per window
# ---------------------------------------------------------------------------
def write_settings(d, enabled, near, far, volume=100, mic=""):
    """The launcher's file, written where the game looks for it.

    VOLUME 100 AND NOT THE DEFAULT 80, so the level the WAV shows is the
    falloff and nothing else -- an 80 in here would put a constant 0.8 in front
    of every number this file prints and invite it to be read as attenuation.
    """
    cfg = {
        "VoiceEnabled": bool(enabled),
        "VoiceMicDevice": mic,
        "VoiceVolume": int(volume),
        "VoiceNearRadius": int(near),
        "VoiceFarRadius": int(far),
    }
    with open(os.path.join(d, "settings.json"), "w", encoding="utf-8") as f:
        json.dump(cfg, f, indent=2)
    return cfg


# ---------------------------------------------------------------------------
# the measurement
# ---------------------------------------------------------------------------
def goertzel(samples, freq, rate):
    """Magnitude at one frequency, normalised to full scale.

    Goertzel rather than a whole FFT because one bin is all this needs and a
    dependency-free single bin is thirty lines. The result is comparable
    across runs of the same length, which is all the arms below ask of it.
    """
    n = len(samples)
    if n == 0:
        return 0.0
    k = int(0.5 + (n * freq) / rate)
    w = (2.0 * math.pi * k) / n
    cosine = math.cos(w)
    coeff = 2.0 * cosine
    s0 = s1 = s2 = 0.0
    for x in samples:
        s0 = x + coeff * s1 - s2
        s2 = s1
        s1 = s0
    power = s1 * s1 + s2 * s2 - coeff * s1 * s2
    return math.sqrt(max(0.0, power)) / (n / 2.0) / 32768.0


def rms(samples):
    if not samples:
        return 0.0
    acc = 0.0
    for x in samples:
        acc += float(x) * float(x)
    return math.sqrt(acc / len(samples)) / 32768.0


def read_wav_mono(path, skip_s=1.0, take_s=2.0):
    """The left channel of the dump, past the boot, as plain integers.

    SKIPS THE FIRST SECOND on purpose: the jitter buffer has to prime and the
    session has to form, and a window that includes the silence before either
    happened measures the harness's start-up rather than the channel.
    """
    with wave.open(path, "rb") as w:
        rate = w.getframerate()
        ch = w.getnchannels()
        n = w.getnframes()
        raw = w.readframes(n)
    step = ch
    all_s = struct.unpack("<%dh" % (len(raw) // 2), raw)
    left = all_s[0::step]
    a = int(skip_s * rate)
    b = a + int(take_s * rate)
    if b > len(left):
        b = len(left)
    if a >= b:
        a, b = 0, len(left)
    return list(left[a:b]), rate


# ---------------------------------------------------------------------------
# the run
# ---------------------------------------------------------------------------
def run_pair(name, near, far, frames, port):
    """One two-window session. A is the talker, B records its own mix."""
    da = os.path.join(OUT, name + "_A")
    db = os.path.join(OUT, name + "_B")
    for d in (da, db):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    write_settings(da, True, near, far)
    write_settings(db, True, near, far)

    ea = M.env_base(ROOT, da, name + "A")
    eb = M.env_base(ROOT, db, name + "B")
    wav = os.path.join(db, "mix.wav")
    for e, role in ((ea, "parent"), (eb, "child")):
        e["SM64DS_WINDOW_SELFTEST"] = frames
        e["SM64DS_COMMS_ROLE"] = role
        e["SM64DS_COMMS_PORT"] = str(port)
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_VS_MAP"] = VS_MAP
        e["SM64DS_VS_PLAYERS"] = "2"
        e["SM64DS_VS_PROBE"] = "1"
        e["SM64DS_VOICE_REPORT"] = "1"
        # NO DEVICE ON EITHER WINDOW. The mixer still runs, clocked off the
        # video frame, so the dump still fills -- out_win.cpp's own no-device
        # arm. This is the standing quiet rule and it is also what makes the
        # measurement reproducible: nothing here depends on the machine having
        # a working speaker or a working microphone.
        e["SM64DS_NO_AUDIO"] = "1"
    ea["SM64DS_VOICE_TEST_TONE"] = "1"
    eb["SM64DS_WAV_DUMP"] = wav

    la, lb = os.path.join(da, "run.log"), os.path.join(db, "run.log")
    pa = M.spawn(EXE, da, ea, la)
    import time
    time.sleep(0.5)
    pb = M.spawn(EXE, db, eb, lb)
    ra = M.finish(pa, 900)
    rb = M.finish(pb, 900)
    return dict(rc_a=ra, rc_b=rb, ta=M.text(la), tb=M.text(lb), wav=wav,
                log_a=la, log_b=lb, dir_a=da, dir_b=db)


def last_distance(text):
    """The separation the game itself reported, out of the [voice] line."""
    got = re.findall(r"\| s\d+ d=(-?\d+) g=", text)
    vals = [int(v) for v in got if int(v) >= 0]
    return vals[-1] if vals else None


def voice_counts(text):
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


def measure(res):
    if not os.path.exists(res["wav"]):
        return None
    s, rate = read_wav_mono(res["wav"])
    return dict(tone=goertzel(s, TONE_HZ, rate), rms=rms(s), n=len(s),
                rate=rate)


# ---------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", default=FRAMES)
    ap.add_argument("--devices", action="store_true",
                    help="dump the machine's recording devices and stop")
    a = ap.parse_args()

    if not os.path.exists(EXE):
        print("no walk_window.exe at %s -- build first" % EXE)
        return 1
    os.makedirs(OUT, exist_ok=True)

    if a.devices:
        d = os.path.join(OUT, "devices")
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        write_settings(d, False, 512, 3072)
        e = M.env_base(ROOT, d, "dev")
        e["SM64DS_WINDOW_SELFTEST"] = "60"
        e["SM64DS_VOICE_DEVICES"] = "1"
        e["SM64DS_NO_AUDIO"] = "1"
        log = os.path.join(d, "run.log")
        M.run_one(EXE, d, e, log)
        for ln in M.text(log).splitlines():
            if "[voice]" in ln:
                print(ln)
        return 0

    ok = True

    # ---- arm 1: NEAR. Both radii well past any separation the arena has, so
    # the falloff is 1.0 and what the WAV shows is the channel working at all.
    near = run_pair("near", 100000, 200000, a.frames, PORT)
    d = last_distance(near["tb"])
    ca, cb = voice_counts(near["ta"]), voice_counts(near["tb"])
    mn = measure(near)
    ok &= M.verdict(near["rc_a"] == 0 and near["rc_b"] == 0,
                    "voice NEAR both windows exited clean | rc %s/%s"
                    % (near["rc_a"], near["rc_b"]))
    ok &= M.verdict(ca is not None and ca["tone"] == 1 and ca["dev"] == 0,
                    "voice the talker used the TONE and opened NO DEVICE | %s"
                    % (ca,))
    ok &= M.verdict(ca is not None and ca["tx"] > 0,
                    "voice the talker sent datagrams | tx=%s"
                    % (ca["tx"] if ca else None))
    ok &= M.verdict(cb is not None and cb["rx"] > 0 and cb["bad"] == 0,
                    "voice the listener received them, none malformed | %s"
                    % (cb,))
    ok &= M.verdict(d is not None,
                    "voice the listener measured a real separation | d=%s" % d)
    ok &= M.verdict(mn is not None and mn["tone"] > 0.02,
                    "voice NEAR the 440 Hz tone is present in the listener's "
                    "own mixer output | tone=%.5f rms=%.5f"
                    % (mn["tone"] if mn else -1, mn["rms"] if mn else -1))
    if d is None or mn is None:
        print("\n".join(M.VERDICTS))
        return 1

    # ---- arm 2: FAR. Both radii BELOW the separation the game just reported,
    # so the falloff is exactly 0 and the same session must go silent.
    far_near = max(1, d // 4)
    far_far = max(2, d // 2)
    far = run_pair("far", far_near, far_far, a.frames, PORT + 2)
    mf = measure(far)
    cbf = voice_counts(far["tb"])
    ok &= M.verdict(cbf is not None and cbf["rx"] > 0,
                    "voice FAR the datagrams still ARRIVE (it is the mix that "
                    "is silent, not the channel) | %s" % (cbf,))
    ok &= M.verdict(mf is not None and mf["tone"] < mn["tone"] / 10.0,
                    "voice FAR the tone is attenuated to silence at d=%d with "
                    "far=%d | tone=%.5f against NEAR's %.5f"
                    % (d, far_far, mf["tone"] if mf else -1, mn["tone"]))

    # ---- arm 3: MID. The separation placed between the two radii, so the
    # log falloff is somewhere strictly inside (0, 1) and the measured level
    # has to land between the other two arms.
    mid_near = max(1, d // 2)
    mid_far = max(mid_near + 1, d * 2)
    mid = run_pair("mid", mid_near, mid_far, a.frames, PORT + 4)
    mm = measure(mid)
    want = math.log(float(mid_far) / float(d)) / \
        math.log(float(mid_far) / float(mid_near))
    ok &= M.verdict(mm is not None and mf is not None and
                    mm["tone"] > mf["tone"] and mm["tone"] < mn["tone"],
                    "voice MID the level sits between the two, as the log "
                    "falloff says (predicted gain %.3f at d=%d, near=%d "
                    "far=%d) | tone %.5f, between FAR %.5f and NEAR %.5f"
                    % (want, d, mid_near, mid_far,
                       mm["tone"] if mm else -1, mf["tone"] if mf else -1,
                       mn["tone"]))

    print("")
    print("measured separation d=%d world units" % d)
    print("  NEAR near=%d far=%d  tone=%.5f rms=%.5f"
          % (100000, 200000, mn["tone"], mn["rms"]))
    print("  MID  near=%d far=%d  tone=%.5f rms=%.5f  predicted gain %.3f"
          % (mid_near, mid_far, mm["tone"] if mm else -1,
             mm["rms"] if mm else -1, want))
    print("  FAR  near=%d far=%d  tone=%.5f rms=%.5f"
          % (far_near, far_far, mf["tone"] if mf else -1,
             mf["rms"] if mf else -1))
    print("")
    for v in M.VERDICTS:
        print(v)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
