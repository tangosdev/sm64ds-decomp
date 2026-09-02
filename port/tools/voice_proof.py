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


def read_wav_mono(path, take_s=0.0, floor=200):
    """The left channel of the dump, from where the voice actually starts.

    NOT A FIXED OFFSET, and the first version of this file used one and paid
    for it. A window that begins at a fixed second catches a different amount
    of pre-voice silence in every arm -- the session takes a variable time to
    form and the jitter buffer takes a variable time to prime -- and silence in
    the window drags the RMS down by an amount that has nothing to do with the
    falloff. So the window starts a quarter of a second after the FIRST sample
    that is above the noise floor, and every arm is then measuring the same
    thing.

    Returns (samples, rate, start_index). An all-silent file returns an empty
    list, which is the correct answer for the FAR arm and is read as one.
    """
    with wave.open(path, "rb") as w:
        rate = w.getframerate()
        ch = w.getnchannels()
        n = w.getnframes()
        raw = w.readframes(n)
    all_s = struct.unpack("<%dh" % (len(raw) // 2), raw)
    left = all_s[0::ch]
    first = -1
    for i, x in enumerate(left):
        if x > floor or x < -floor:
            first = i
            break
    if first < 0:
        return [], rate, -1
    a = first + int(0.25 * rate)
    b = len(left) if take_s <= 0.0 else min(len(left), a + int(take_s * rate))
    if a >= b:
        a, b = first, len(left)
    return list(left[a:b]), rate, first


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
        # A VS MATCH, NOT A LEVEL. env_base sets SM64DS_LEVEL=1 (castle
        # grounds) because every rung that inherits it reads level-path
        # output; this run needs the arena, because the arena is where more
        # than one Player body exists and a distance between two bodies is the
        # whole measurement. The boot chain reads LEVEL, so LEVEL has to go
        # rather than be overridden.
        e.pop("SM64DS_LEVEL", None)
        e.pop("SM64DS_SCENE", None)
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
    # THE TONE GOES ON BOTH WINDOWS, and the second one is not decoration.
    # Window B has VoiceEnabled on because it has to be listening, and a
    # listening window with no tone hook would open this machine's real
    # microphone -- which breaks the standing quiet rule (a test launch may not
    # touch the owner's hardware) and would put whatever is in the room into
    # window A's mix. With the tone on both, NO recording device is opened by
    # either window, and the assertions below check exactly that.
    #
    # B's own tone never reaches B's own measurement: a console does not
    # receive its own datagrams, and update_gains zeroes the local slot.
    ea["SM64DS_VOICE_TEST_TONE"] = "1"
    eb["SM64DS_VOICE_TEST_TONE"] = "1"
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


def modal(vals):
    """The value that held for most of the run, not the last one seen.

    THE LAST READING IS THE WRONG ONE AND USING IT COST A FALSE FAILURE. The
    report line is emitted once a second, the bodies drift while a match runs,
    and the measurement window below is four seconds near the START of the
    audio. Reading the final separation described a moment the window does not
    contain: one arm settled at 190 units after spending the whole run at 229,
    and the level in the dump -- correctly -- followed the 229. So every arm
    reads the value that held for the most seconds of its own run, which is the
    one the window is actually measuring.
    """
    if not vals:
        return None
    best, n = None, -1
    for v in set(vals):
        c = vals.count(v)
        if c > n:
            best, n = v, c
    return best


def last_distance(text):
    """The separation the game itself reported, out of the [voice] line."""
    got = re.findall(r"\| s\d+ d=(-?\d+) g=", text)
    return modal([int(v) for v in got if int(v) >= 0])


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


def gain_series(text):
    """Every gain the game reported over the run, in order. One a second."""
    return [float(v)
            for v in re.findall(r"\| s\d+ d=-?\d+ g=([0-9.]+)", text)]


def last_gain(text):
    """The gain that held for most of the run."""
    g = gain_series(text)
    return modal(g) if g else None


def expected_rms(text, full):
    """The level a whole run's gain series predicts, as an RMS.

    NOT `full * one_gain`, and getting that wrong cost two false failures. The
    measured RMS is a root-mean-square over the WHOLE audible run, and the gain
    is not constant across it -- the two bodies drift, so an arm can sit at one
    separation for the first half and another for the second. The honest
    prediction is therefore the root mean square of the reported gain series,
    which is what a time-varying gain does to a constant-amplitude source, and
    it collapses to `full * g` exactly when the gain never moved.
    """
    g = gain_series(text)
    if not g:
        return None
    return full * math.sqrt(sum(x * x for x in g) / len(g))


def measure(res):
    """RMS is the headline and the tone bin is the corroboration.

    RMS BECAUSE THE GAME'S OWN AUDIO IS ZERO HERE. Every window runs with
    SM64DS_VOLUME=0, which zeroes the DS mix at the host output stage, and the
    voice mix runs AFTER that stage - so every non-zero sample in this dump is
    voice and nothing else. The FAR arm is the assertion of exactly that: it
    receives datagrams the whole run and its dump is all zeros.

    The 440 Hz bin is reported beside it but is NOT the pass condition, and the
    reason is a harness artefact worth writing down. With SM64DS_NO_AUDIO the
    mixer is clocked off the VIDEO FRAME at a nominal 60 Hz (one 546-sample
    block a frame, out_win.cpp's no-device arm), while the tone generator is
    clocked off the wall. A headless selftest does not run at 60 fps, so the
    dump's own clock runs slower than real time, voice arrives faster than it
    is consumed, and the jitter ring drops its oldest frame to keep the delay
    bounded - exactly as designed. The result is a tone chopped into segments
    with phase steps between them, whose ENERGY is intact (RMS) but whose
    coherence over a four-second window is not (the bin). On a real device the
    mixer is clocked by the hardware at 32768 Hz and the drift does not exist.
    """
    if not os.path.exists(res["wav"]):
        return None
    s, rate, first = read_wav_mono(res["wav"])
    return dict(tone=goertzel(s, TONE_HZ, rate), rms=rms(s), n=len(s),
                rate=rate, first=first)


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

    # THE SETTINGS FILE THIS HARNESS WRITES HAS TO BE THE ONE THE GAME READS,
    # and host_settings.cpp's find_settings tries beside-the-exe FIRST, then
    # SM64DS_ASSET_ROOT, then the working directory. This harness writes into
    # the working directory, which is the third candidate. A settings.json
    # sitting in either of the first two would win silently and every arm below
    # would measure the wrong radii while reporting the ones it wrote. So the
    # run refuses rather than measures.
    for shadow in (os.path.join(os.path.dirname(EXE), "settings.json"),
                   os.path.join(ROOT, "settings.json")):
        if os.path.exists(shadow):
            print("refusing to run: %s would shadow the per-window "
                  "settings.json this harness writes" % shadow)
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

    # ---- arm 0a: THE CODEC, MEASURED ON THE SHIPPED FUNCTIONS.
    #
    # A sine round trip through the real encode_frame/decode_frame, reported by
    # the game itself so that what is measured is the code that ships rather
    # than a model of it. The threshold is 30 dB and the history is why it is
    # there: the first version of the encoder reset its step index to zero at
    # every 20 ms frame boundary, which made the quantiser re-climb from a step
    # size of 7 fifty times a second and read 18.7 dB. Carrying the index (and
    # writing it into the block, so a block still describes its own starting
    # state and a lost datagram still costs only its own 40 ms) reads 34.
    d0 = os.path.join(OUT, "codec")
    os.makedirs(os.path.join(d0, "tmp"), exist_ok=True)
    write_settings(d0, False, 512, 3072)
    e0 = M.env_base(ROOT, d0, "codec")
    e0["SM64DS_WINDOW_SELFTEST"] = "60"
    e0["SM64DS_VOICE_CODEC_SELFTEST"] = "1"
    e0["SM64DS_NO_AUDIO"] = "1"
    l0 = os.path.join(d0, "run.log")
    M.run_one(EXE, d0, e0, l0)
    m0 = re.search(r"\[voice\] codec selftest: (\d+) frames.*SNR "
                   r"(-?[0-9.]+) dB", M.text(l0))
    snr = float(m0.group(2)) if m0 else None
    ok &= M.verdict(snr is not None and snr > 30.0,
                    "voice CODEC a sine round trip through the shipped IMA "
                    "ADPCM reads above 30 dB | SNR %s dB over %s frames"
                    % (snr, m0.group(1) if m0 else "?"))

    # ---- arm 0b: A MISSING MICROPHONE SAYS SO ONCE AND THEN SHUTS UP.
    #
    # VoiceEnabled on, no tone hook, a device name nothing matches, and
    # SM64DS_VOICE_NO_DEVICE so the open refuses as if the machine had no
    # recording hardware -- which is what lets this run on a box that HAS a
    # microphone without the proof taking it. Before the failure latch this
    # printed its failure line on every frame; the assertion is that it now
    # prints once over a whole run and does no work after that.
    d1 = os.path.join(OUT, "nodev")
    os.makedirs(os.path.join(d1, "tmp"), exist_ok=True)
    write_settings(d1, True, 512, 3072, mic="NoSuchMicrophoneAnywhere")
    e1 = M.env_base(ROOT, d1, "nodev")
    e1["SM64DS_WINDOW_SELFTEST"] = "600"
    e1["SM64DS_NO_AUDIO"] = "1"
    e1["SM64DS_VOICE_NO_DEVICE"] = "1"
    l1 = os.path.join(d1, "run.log")
    rc1 = M.run_one(EXE, d1, e1, l1)
    t1 = M.text(l1)
    n_open_fail = len(re.findall(r"waveInOpen failed", t1))
    n_nomatch = len(re.findall(r"no recording device matches", t1))
    ok &= M.verdict(rc1 == 0 and n_open_fail == 1 and n_nomatch == 1,
                    "voice NO DEVICE says so ONCE over 600 frames and then "
                    "goes quiet | rc %s, %d 'waveInOpen failed' lines, %d 'no "
                    "recording device matches' lines"
                    % (rc1, n_open_fail, n_nomatch))

    # ---- arm 1: NEAR. Both radii well past any separation the arena has, so
    # the falloff is 1.0 and what the dump shows is the channel working at all.
    near = run_pair("near", 100000, 200000, a.frames, PORT)
    d = last_distance(near["tb"])
    g = last_gain(near["tb"])
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
    ok &= M.verdict(cb is not None and cb["dev"] == 0,
                    "voice NEITHER window opened a recording device | "
                    "listener dev=%s" % (cb["dev"] if cb else None,))
    ok &= M.verdict(d is not None,
                    "voice the listener measured a real separation | d=%s" % d)
    ok &= M.verdict(g is not None and abs(g - 1.0) < 1e-6,
                    "voice NEAR the game applies gain 1.000 | g=%s" % g)

    # THE LEVEL A GAIN OF 1.0 IS SUPPOSED TO PRODUCE, computed rather than
    # eyeballed. The generator's amplitude is 16000 of full scale, so a sine at
    # unity gain has an RMS of 16000 / sqrt(2) / 32768 = 0.3453. Anything else
    # would mean the resampler, the codec or the mix stage changed the level,
    # which is the failure this arm is looking for.
    want_full = 16000.0 / math.sqrt(2.0) / 32768.0
    want_near = expected_rms(near["tb"], want_full) or want_full
    ok &= M.verdict(mn is not None and
                    abs(mn["rms"] - want_near) / want_near < 0.10,
                    "voice NEAR the tone arrives at FULL LEVEL through capture, "
                    "IMA ADPCM, the wire, the jitter buffer and the resampler "
                    "| rms %.5f against %.5f predicted (16000/sqrt2 of full "
                    "scale), 440 Hz bin %.5f"
                    % (mn["rms"] if mn else -1, want_full,
                       mn["tone"] if mn else -1))
    ok &= M.verdict(d is not None and d >= 8,
                    "voice the two bodies are actually apart in the arena, so "
                    "the bracketing arms below mean something | d=%s" % d)
    if d is None or mn is None or d < 8:
        print("\n".join(M.VERDICTS))
        return 1

    # ---- arm 2: FAR. Both radii BELOW the separation the game just reported,
    # so the falloff is exactly 0. Not "quiet": the dump has to be all zeros,
    # which is also the assertion that every non-zero sample in the other two
    # arms is voice and not the game (SM64DS_VOLUME=0 zeroes the DS mix at the
    # output stage, and the voice mix runs after it).
    far_near = max(1, d // 4)
    far_far = max(2, d // 2)
    far = run_pair("far", far_near, far_far, a.frames, PORT + 2)
    mf = measure(far)
    cbf = voice_counts(far["tb"])
    gf = last_gain(far["tb"])
    ok &= M.verdict(cbf is not None and cbf["rx"] > 0,
                    "voice FAR the datagrams still ARRIVE (it is the mix that "
                    "is silent, not the channel) | %s" % (cbf,))
    ok &= M.verdict(gf is not None and gf == 0.0,
                    "voice FAR the game applies gain 0.000 at d=%d with far=%d "
                    "| g=%s" % (d, far_far, gf))
    ok &= M.verdict(mf is not None and mf["first"] == -1 and mf["rms"] == 0.0,
                    "voice FAR the listener's whole dump is EXACTLY ZERO - so "
                    "the game contributes nothing to these numbers and every "
                    "non-zero sample in the other arms is voice | rms=%.5f, "
                    "first non-silent sample %s"
                    % (mf["rms"] if mf else -1, mf["first"] if mf else None))

    # ---- arm 3: MID. The separation placed between the two radii, so the log
    # falloff lands strictly inside (0, 1). The prediction is taken from the
    # distance THAT RUN reported, not from the first arm's, because the bodies
    # are free to have settled somewhere else.
    mid_near = max(1, d // 2)
    mid_far = max(mid_near + 1, d * 2)
    mid = run_pair("mid", mid_near, mid_far, a.frames, PORT + 4)
    mm = measure(mid)
    gm = last_gain(mid["tb"])
    dm = last_distance(mid["tb"])
    want_math = None
    if dm:
        want_math = math.log(float(mid_far) / float(dm)) / \
            math.log(float(mid_far) / float(mid_near))
    ok &= M.verdict(gm is not None and want_math is not None and
                    abs(gm - want_math) < 0.01,
                    "voice MID the gain the game applied IS the log falloff | "
                    "g=%s, log(far/d)/log(far/near) = %.3f at d=%s near=%d "
                    "far=%d" % (gm, want_math if want_math else -1, dm,
                                mid_near, mid_far))
    want_mid = expected_rms(mid["tb"], want_full)
    ok &= M.verdict(mm is not None and want_mid is not None and want_mid > 0 and
                    abs(mm["rms"] - want_mid) / want_mid < 0.10,
                    "voice MID the LEVEL follows the gain | rms %.5f against "
                    "%.5f predicted (full level %.5f through the run's own "
                    "reported gain series, modal gain %.3f)"
                    % (mm["rms"] if mm else -1, want_mid if want_mid else -1,
                       want_full, gm if gm else 0))

    print("")
    print("measured separation d=%d world units, arena VS map %s" % (d, VS_MAP))
    print("  full-level prediction  %.5f rms (a 16000 amplitude sine)"
          % want_full)
    print("  NEAR near=%-6d far=%-6d  gain %.3f  rms %.5f  440Hz %.5f"
          % (100000, 200000, g if g else 0, mn["rms"], mn["tone"]))
    print("  MID  near=%-6d far=%-6d  gain %.3f  rms %.5f  440Hz %.5f"
          % (mid_near, mid_far, gm if gm else 0, mm["rms"] if mm else -1,
             mm["tone"] if mm else -1))
    print("  FAR  near=%-6d far=%-6d  gain %.3f  rms %.5f  440Hz %.5f"
          % (far_near, far_far, gf if gf else 0, mf["rms"] if mf else -1,
             mf["tone"] if mf else -1))
    print("")
    for v in M.VERDICTS:
        print(v)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
